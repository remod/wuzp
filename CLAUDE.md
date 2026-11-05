# Agent rules

Rules, commands and skills for AI agents live in `.cursor/` and are shared by Cursor and Claude Code.
Claude Code reads them through `.claude/rules/cursor/` (generated copies) and symlinks in `.claude/commands/` and `.claude/skills/`.

To add or change a rule, edit `.cursor/rules/*.mdc`, then run `python3 .claude/sync-from-cursor.py` and commit both.
Never edit the generated files under `.claude/rules/cursor/`.
