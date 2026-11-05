---
name: polish
description: Pre-commit hardening of this repo's uncommitted changes — repeated code-structure passes, test coverage, then fix-issues (format, build, all tests). Run it before committing code changes, unless it already ran on exactly these changes, and whenever the user asks for it. Don't run it after each feature on your own. Never commits.
---

# Polish

Hardens the uncommitted changes in this repo. Each pass runs in a **fresh Cursor agent**
(see [Pass runner](#pass-runner)), whichever tool started polish. A fresh context reviews
the code with no memory of the earlier pass, which is what makes repeated passes
worthwhile.

> Adapted from the shared `polish` skill of other repositories. This copy is maintained here: wuzp is a
> header-only library without runtime monitoring, so it has no monitoring pass.

## When

Before a commit that includes code changes. Skip it when:

- the changes are docs only;
- it already ran on these changes and nothing changed since. After a small follow-up
  tweak to polished work, run only the fix-issues pass;
- the user says to commit without it.

## Scope

The uncommitted changes (`git diff HEAD` plus untracked files). If the user gave a base
ref, use `git diff <base>` instead.

## Pass instructions come from the repo

For each pass, read the matching command from `.cursor/commands/<name>.md` (or skill from
`.cursor/skills/<name>/SKILL.md`). It is the source of truth and is tailored to this
stack:

| Pass | Command or skill | Fallback if neither exists |
|---|---|---|
| structure | `ensure-good-code-structure` | "Ensure your recent changes have good code structure. Address all code structure optimization opportunities." |
| coverage | `ensure-high-code-coverage` or `ensure-high-test-coverage` | "Ensure your recent changes have high code coverage. Address all code coverage gaps." |
| fix-issues | `fix-issues` | Run `ci/local.sh` and fix what fails. Re-run until clean. |

## Sequence

Run these steps strictly one after another, never in parallel:

1. **structure**, repeated: at least 2 passes and at most 6. From the 2nd pass on, stop
   as soon as a pass reports `changes: none` or `minor`.
2. **coverage**: once.
3. **fix-issues**: once. It loops internally until clean.

## Pass runner

Run each pass as its own headless Cursor agent, from the repo root, with Cursor's default
model (`auto`, so no `--model`):

```bash
cursor-agent -p --force --trust --workspace "$(git rev-parse --show-toplevel)" \
  "$(cat "$BRIEF")" > "$OUT" 2>&1
```

- Write the brief to a temp file (`$BRIEF`) and capture the reply in `$OUT`. Passes can
  run longer than a tool-call timeout (fix-issues runs the full test suite): start the
  command in the background and wait for it to exit.
- One pass at a time. Never start the next pass, or any other test suite, while one runs.
  `ci/local.sh` uses half of the CPU cores; never use all of them.
- If `cursor-agent` is not installed, run the pass in a fresh subagent of your own tool
  instead, with the same brief.

## Pass brief

Give each pass:

- the repo path, and the instruction to work there and follow its `CLAUDE.md`/`AGENTS.md`
  and rules;
- the scope: run `git diff HEAD` and `git status --short`. It should only touch code
  related to these changes;
- the pass instruction, verbatim from the command or skill file;
- the rules:
  - Edit files directly and don't ask for confirmation.
  - Never commit, push, or change git state.
  - Where the command says "ask if you're not sure", don't guess. Leave that item
    alone and list it under `questions`.
- the reply format: `changes: none | minor | significant`, a summary of at most
  3 lines, `questions:` (or "none"), and for fix-issues the final status of each
  check.

## Finish

1. Report briefly: one table row per pass (pass, change level, summary), the final
   fix-issues status, then any collected `questions` for the user.
2. Polish itself never commits. If a commit was requested, make it after polish,
   unless there are open `questions`.
