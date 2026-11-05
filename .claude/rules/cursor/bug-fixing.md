<!-- Generated from .cursor/rules/bug-fixing.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

1. **Understand the bug in detail.** Reproduce it and find what *causes* it; do not jump to a speculative fix. For compiler-specific behavior, check the compilers' documentation and issue trackers.
2. **Reproduce with a test first.** Write a unit or compile test that fails for the right reason, then fix.
3. **Fix the root cause**, in the right layer: a wrong trait in the core is fixed in the core, not worked around in a backend or a test.
4. **Fan out.** Search the codebase for the same mistake (same pattern, sibling specializations, the other backends) and fix those the same way.
5. If the fix touches the math or physics, check it against `DESIGN.md` and update it.
