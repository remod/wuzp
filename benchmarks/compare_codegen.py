#!/usr/bin/env python3
"""Compares the code that the compiler generated for the operations of benchmarks/codegen/wuzp.cpp, with wuzp and Eigen, with
that of the same operations on plain Eigen in benchmarks/codegen/eigen.cpp (see "Generated code" in benchmarks/README.md).

Usage: benchmarks/compare_codegen.py <build directory of a benchmarks preset>, e.g. build/gcc-benchmarks

For each operation, it prints the number of instructions on both sides, including those of the functions that it calls and that
the compiler did not inline, if they are defined in the same object file (e.g. not those of the standard library), and whether the
instructions are identical, or the same apart from their order and the numbering of the SIMD registers. The normalized listings are
written to <build directory>/codegen, to compare them with diff. Differences are reported, not errors.

It follows every call and jump out of a function: those with a relocation, and those to local functions in the same section,
which the assembler resolves without one, e.g. to helpers in anonymous namespaces, GCC's clones (.isra, .constprop, .part) and
the cold parts of functions (.cold) in other sections. It fails if it cannot find an operation, or cannot tell where a call or a
jump goes. It first checks that it follows them in benchmarks/codegen/self_check.cpp. It only supports x86-64: on other
architectures, it skips the comparison.
"""

import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

OPERATIONS = ["sum", "productWithScalar", "norm", "transformVector", "transformPoint", "compose"]
# The prefixes of x86-64 instructions that objdump prints before a call or a jump.
BRANCH_PREFIXES = re.compile(r"^(?:(?:bnd|notrack|rex\.W)\s+)+")
DIRECT_BRANCH = re.compile(r"^(call|jmp|j[a-z]+)\s+([0-9a-f]+) <[^>]*>$")
INDIRECT_BRANCH = re.compile(r"^(call|jmp)\s+\*")
RELOCATION_TARGET = re.compile(r"^(.*?)([+-]0x[0-9a-f]+)?$")


class CodegenError(Exception):
    pass


@dataclass
class Function:
    name: str
    section: str
    start: int
    size: int
    # The instructions as (address, text, relocation target or None).
    instructions: list = field(default_factory=list)

    def contains(self, address):
        return self.start <= address < self.start + self.size


@dataclass
class ObjectFile:
    path: Path
    # Every function symbol by its (mangled) name, and the functions of every section.
    functions: dict
    sections: dict

    def function_at(self, section, address):
        for function in self.sections.get(section, []):
            if function.contains(address):
                return function
        return None


def run(*command, stdin=None):
    return subprocess.run(command, input=stdin, check=True, capture_output=True, text=True).stdout


def demangle(names):
    names = list(names)
    if not names:
        return {}
    return dict(zip(names, run("c++filt", stdin="\n".join(names) + "\n").splitlines()))


def architecture(path):
    match = re.search(r"architecture: ([^,]+),", run("objdump", "-f", str(path)))
    return match.group(1) if match else "unknown"


def read_object(path):
    """Reads the function symbols of the object file and their instructions, without the padding after them."""
    obj = ObjectFile(path, {}, {})
    # E.g. "0000000000000020 g     F .text\t0000000000000017 _ZN7codegen3sumE...". The flags take 7 characters.
    for line in run("objdump", "-t", str(path)).splitlines():
        match = re.match(r"^([0-9a-f]+) (.{7}) (\S+)\t([0-9a-f]+) (?:\.hidden )?(\S+)$", line)
        if match and "F" in match.group(2):
            function = Function(match.group(5), match.group(3), int(match.group(1), 16), int(match.group(4), 16))
            obj.functions[function.name] = function
            obj.sections.setdefault(function.section, []).append(function)
    section = None
    last = None
    for line in run("objdump", "-d", "-r", "--no-show-raw-insn", str(path)).splitlines():
        if match := re.match(r"^Disassembly of section (\S+):$", line):
            section = match.group(1)
            last = None
        elif match := re.match(r"^\s*([0-9a-f]+): (R_\S+)\t(.*)$", line):
            # A relocation belongs to the instruction before it.
            if last is not None:
                function, index = last
                address, text, _ = function.instructions[index]
                function.instructions[index] = (address, text, match.group(3))
        elif match := re.match(r"^\s*([0-9a-f]+):\t(.*)$", line):
            address = int(match.group(1), 16)
            owner = obj.function_at(section, address)
            if owner is None:
                last = None  # Padding between functions.
                continue
            # objdump's comments, e.g. "# 15 <...+0x15>" after a RIP-relative operand, name addresses within the object.
            text = re.sub(r"\s+#.*$", "", match.group(2)).strip()
            owner.instructions.append((address, text, None))
            last = (owner, len(owner.instructions) - 1)
    return obj


def branch_target(obj, function, text, relocation):
    """Where a call or a jump goes: ("internal", offset), ("function", Function), ("external", name), or None if the instruction
    is no branch."""
    text = BRANCH_PREFIXES.sub("", text)
    if INDIRECT_BRANCH.match(text):
        raise CodegenError(f"Cannot follow the indirect branch '{text}' in {function.name} of {obj.path}.")
    match = DIRECT_BRANCH.match(text)
    if not match:
        return None
    if relocation is not None:
        symbol, addend = RELOCATION_TARGET.match(relocation).groups()
        if symbol in obj.functions:
            return ("function", obj.functions[symbol])
        if symbol in obj.sections:
            # Relative to a section: the 32-bit displacement ends the instruction, so the target is the addend plus 4.
            target = obj.function_at(symbol, int(addend or "0x0", 16) + 4)
            if target is None:
                raise CodegenError(f"Cannot find the function at {relocation} that {function.name} of {obj.path} calls.")
            return ("function", target)
        if symbol.startswith("."):
            raise CodegenError(f"Cannot follow the branch to {relocation} in {function.name} of {obj.path}.")
        return ("external", symbol)
    target = int(match.group(2), 16)
    if function.contains(target):
        return ("internal", target - function.start)
    callee = obj.function_at(function.section, target)
    if callee is None:
        raise CodegenError(f"Cannot find the function at {target:#x} that {function.name} of {obj.path} branches to.")
    return ("function", callee)


def normalized_relocation(obj, relocation):
    """A relocation of a non-branch instruction, without the offsets of constants: a symbol keeps its name, a local label or a
    section, e.g. of a constant, becomes <constant>."""
    symbol, _ = RELOCATION_TARGET.match(relocation).groups()
    if symbol.startswith(".") or symbol in obj.sections:
        return "<constant>"
    return symbol


def listing(obj, operation):
    """The listing of the operation and of the functions it calls that are defined in the object file, recursively, as lines: a
    header with the name of each function, then its instructions, indented, normalized so that identical code compares equal
    across object files: without addresses, with jump targets relative to the function, calls to followed functions by their
    number in the listing, and without the offsets of constants."""
    # Not its clones and cold parts, e.g. "codegen::norm(...) [clone .cold]".
    roots = [
        name
        for name, demangled in demangle(obj.functions).items()
        if demangled.startswith(f"codegen::{operation}(") and " [clone " not in demangled
    ]
    if len(roots) != 1:
        raise CodegenError(f"Found {len(roots)} functions codegen::{operation} in {obj.path}, expected 1.")
    followed = [obj.functions[roots[0]]]
    lines = []
    index = 0
    while index < len(followed):
        function = followed[index]
        lines.append(f"function {index}: {function.name}")
        for _, text, relocation in function.instructions:
            target = branch_target(obj, function, text, relocation)
            if target is None:
                if relocation is not None:
                    text = f"{text}  {normalized_relocation(obj, relocation)}"
            else:
                mnemonic = BRANCH_PREFIXES.sub("", text).split()[0]
                if target[0] == "internal":
                    text = f"{mnemonic} <+{target[1]:#x}>"
                elif target[0] == "function":
                    if target[1] not in followed:
                        followed.append(target[1])
                    text = f"{mnemonic} <function {followed.index(target[1])}>"
                else:
                    text = f"{mnemonic} <{target[1]}>"
            lines.append(f"\t{text}")
        index += 1
    # The names in the headers and of the external functions, demangled.
    return run("c++filt", stdin="\n".join(lines) + "\n").splitlines(), followed


def instructions(lines):
    return [line for line in lines if line.startswith("\t")]


def same_instructions(lhs, rhs):
    """Whether the instructions are the same, apart from their order and the numbering of the SIMD registers."""

    def normalized(lines):
        return sorted(re.sub(r"%([xyz]mm)[0-9]+", r"%\1", line) for line in instructions(lines))

    return normalized(lhs) == normalized(rhs)


def self_check(obj):
    """Checks that the listings follow a call to a local helper and to an inline function that the compiler did not inline, and
    the cold part of a function if the compiler split it off."""
    for operation, callee in [("selfCheckLocalHelper", "its local helper"), ("selfCheckInlineHelper", "its inline helper")]:
        _, followed = listing(obj, operation)
        if len(followed) < 2:
            raise CodegenError(f"The listing of codegen::{operation} in {obj.path} does not follow {callee}.")
    _, followed = listing(obj, "selfCheckColdPart")
    root = followed[0].name
    split = {function.name for function in obj.functions.values() if function.name.startswith(f"{root}.")}
    missing = split - {function.name for function in followed}
    if missing:
        raise CodegenError(f"The listing of codegen::selfCheckColdPart in {obj.path} does not follow {', '.join(sorted(missing))}.")


def main():
    if len(sys.argv) != 2:
        print(__doc__, file=sys.stderr)
        return 2
    build_dir = Path(sys.argv[1])
    object_dir = build_dir / "benchmarks" / "CMakeFiles" / "wuzp_codegen.dir" / "codegen"
    listing_dir = build_dir / "codegen"
    objects = {side: object_dir / f"{side}.cpp.o" for side in ["eigen", "wuzp", "self_check"]}
    for path in objects.values():
        if not path.is_file():
            raise CodegenError(f"{path} does not exist: build the target wuzp_codegen first, with GCC or Clang.")
    if (arch := architecture(objects["wuzp"])) != "i386:x86-64":
        print(f"Skipping the comparison of the generated code: it supports x86-64 only, not {arch}.")
        return 0
    eigen, wuzp, check = (read_object(objects[side]) for side in ["eigen", "wuzp", "self_check"])
    self_check(check)
    listing_dir.mkdir(parents=True, exist_ok=True)
    print(f"{'Operation':<18} {'Eigen':>6} {'wuzp':>6}  {'Identical':<9}  Same instructions")
    for operation in OPERATIONS:
        lines = {}
        for side, obj in [("eigen", eigen), ("wuzp", wuzp)]:
            lines[side], _ = listing(obj, operation)
            (listing_dir / f"{operation}.{side}.txt").write_text("\n".join(lines[side]) + "\n")
        identical = "yes" if instructions(lines["eigen"]) == instructions(lines["wuzp"]) else "no"
        same = "yes" if same_instructions(lines["eigen"], lines["wuzp"]) else "no"
        print(f"{operation:<18} {len(instructions(lines['eigen'])):>6} {len(instructions(lines['wuzp'])):>6}  {identical:<9}  {same}")
    print(f"The listings are in {listing_dir}, e.g. diff {listing_dir}/compose.{{eigen,wuzp}}.txt")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except CodegenError as error:
        print(f"compare_codegen.py: {error}", file=sys.stderr)
        sys.exit(1)
