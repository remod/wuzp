<!-- Generated from .cursor/rules/bash-usage.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

Don't explicitly call `bash` to run scripts, because it is not in the allowlist. The scripts in `ci/` are executable: run them directly, e.g. `ci/local.sh`.
