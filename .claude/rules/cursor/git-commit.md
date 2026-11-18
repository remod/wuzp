<!-- Generated from .cursor/rules/git-commit.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

Never commit or push changes without the user explicitly asking for it.

Before committing the code changes of a task, e.g. an issue, run the `prepare-to-ship` skill: it reviews them, fixes the confirmed findings and runs the `polish` skill (whose "When" section says when to skip polish). Run it for every task on its own, so that each gets its own review and commit.

Commit messages follow the repository's style: a first line that is one sentence in the imperative and ends with a period, then a body that explains why.
