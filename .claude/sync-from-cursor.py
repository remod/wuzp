#!/usr/bin/env python3
"""Mirror this repo's Cursor config into Claude Code's format.

`.cursor/` is the source of truth. Claude Code only reads `.md` rule files and
scopes them with `paths:` instead of `globs:`, so rules are generated copies;
commands and skills are symlinks.

    python3 .claude/sync-from-cursor.py          # regenerate
    python3 .claude/sync-from-cursor.py --check  # exit 1 if out of sync
"""

import os
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CURSOR = ROOT / ".cursor"
CLAUDE = ROOT / ".claude"
GENERATED_RULES = CLAUDE / "rules" / "cursor"
SCRIPT = ".claude/sync-from-cursor.py"

# Cursor-only commands that have no meaning in Claude Code (rules load automatically there).
SKIPPED_COMMANDS = {"initialize"}


def split_frontmatter(text):
    lines = text.splitlines()
    if not lines or lines[0].strip() != "---":
        return {}, text
    for end in range(1, len(lines)):
        if lines[end].strip() == "---":
            fields = {}
            for line in lines[1:end]:
                key, sep, value = line.partition(":")
                if sep:
                    fields[key.strip()] = value.strip()
            return fields, "\n".join(lines[end + 1 :]).lstrip("\n")
    return {}, text


def to_claude_rule(source, mdc_text):
    fields, body = split_frontmatter(mdc_text)
    globs = [g.strip().strip("\"'") for g in fields.get("globs", "").strip("[]").split(",")]
    globs = [g for g in globs if g]
    header = ""
    if globs:
        header = "---\npaths:\n" + "".join(f'  - "{g}"\n' for g in globs) + "---\n\n"
    note = f"<!-- Generated from {source} by {SCRIPT}. Edit the source, then rerun the script. -->\n\n"
    return header + note + body.rstrip() + "\n"


def expected_rules():
    rules = {}
    for mdc in sorted((CURSOR / "rules").rglob("*.mdc")):
        relative = mdc.relative_to(CURSOR / "rules").with_suffix(".md")
        rules[GENERATED_RULES / relative] = to_claude_rule(mdc.relative_to(ROOT), mdc.read_text())
    return rules


def expected_links():
    """Map link path -> relative target for commands, skills and the MCP config."""
    links = {}
    for command in sorted((CURSOR / "commands").glob("*.md")):
        if command.stem not in SKIPPED_COMMANDS:
            links[CLAUDE / "commands" / command.name] = Path("../../.cursor/commands") / command.name
    for skill in sorted((CURSOR / "skills").glob("*/SKILL.md")):
        name = skill.parent.name
        links[CLAUDE / "skills" / name] = Path("../../.cursor/skills") / name
    if (CURSOR / "mcp.json").is_file():
        links[ROOT / ".mcp.json"] = Path(".cursor/mcp.json")
    return links


def managed_links():
    """Existing symlinks that point into .cursor/, i.e. ones this script created."""
    found = set()
    for folder in (CLAUDE / "commands", CLAUDE / "skills"):
        if folder.is_dir():
            found |= {p for p in folder.iterdir() if p.is_symlink() and ".cursor/" in os.readlink(p)}
    mcp = ROOT / ".mcp.json"
    if mcp.is_symlink() and ".cursor/" in os.readlink(mcp):
        found.add(mcp)
    return found


def find_drift(rules, links):
    drift = []
    existing_rules = set(GENERATED_RULES.rglob("*.md")) if GENERATED_RULES.is_dir() else set()
    for path, content in rules.items():
        if not path.is_file() or path.read_text() != content:
            drift.append(path)
    drift += sorted(existing_rules - rules.keys())
    for link, target in links.items():
        if not link.is_symlink() or Path(os.readlink(link)) != target:
            drift.append(link)
    drift += sorted(managed_links() - links.keys())
    return [p.relative_to(ROOT) for p in drift]


def sync(rules, links):
    shutil.rmtree(GENERATED_RULES, ignore_errors=True)
    for path, content in rules.items():
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
    for stale in managed_links() - links.keys():
        stale.unlink()
    for link, target in links.items():
        if link.is_symlink():
            link.unlink()
        elif link.exists():
            print(f"skipped {link.relative_to(ROOT)}: a real file is in the way", file=sys.stderr)
            continue
        link.parent.mkdir(parents=True, exist_ok=True)
        link.symlink_to(target)


def main():
    rules, links = expected_rules(), expected_links()
    drift = find_drift(rules, links)
    if "--check" in sys.argv:
        if drift:
            print(
                "Claude config is out of sync with .cursor/ "
                f"(run `python3 {SCRIPT}` and commit): " + ", ".join(map(str, drift))
            )
            sys.exit(1)
        return
    sync(rules, links)
    print(f"Synced {len(rules)} rules, {len(links)} links ({len(drift)} changed).")


if __name__ == "__main__":
    main()
