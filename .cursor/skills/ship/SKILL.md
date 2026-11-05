---
name: ship
description: Take a task's finished changes to master in one go — parallel review (Claude + Cursor), Cursor verifies and fixes the findings, gate, polish, commit, rebase onto master, push to master, then watch CI. Skips steps that already ran on exactly these changes. Run it only when the user asks to ship (e.g. "/ship", "ship it"); typing it is the approval to push. `--no-push` stops after the commit.
---

# Ship

Takes the changes of one task from "works" to "pushed to master and green in CI".
wuzp has no deployment, but master must stay green: CI tests every backend combination on the oldest and the newest supported
dependency versions, so every step below can stop the run.
Stopping is always fine; pushing something half-checked is not.

> Adapted from the shared `ship` skill of other repositories. This copy is maintained here: wuzp is a single
> repository without deployment, and its default branch is `master`.

## Arguments

- `--no-push`: stop after the commit (step 6).
- A base ref (e.g. `origin/master`): review `git diff <base>` instead of the default scope.

## Don't redo finished work

Before each step, check whether it already happened **on exactly the current changes**:
earlier in this conversation (a review, `/code-review`, polish or a test run on these
files), or because the user says so ("already reviewed", "polish ran"), or from git
(already committed, already pushed, CI already green for that sha).

- A step counts as done only if none of the task files changed after it ran. If files
  changed since, redo just what the change affects: after a small follow-up edit,
  polish's fix-issues pass is enough; after a large one, rerun the step.
- Reuse earlier output instead of repeating it: findings from an earlier review go into
  step 2–3 as an extra review file.
- When unsure whether a step ran on the current state, run it.
- List skipped steps and the reason in the report.

## 0. Scope

1. Note the branch (`git branch --show-current`) and whether it is a worktree. The **task
   files** are the uncommitted changes (`git status --short`, `git diff HEAD`) plus, on a
   branch, its commits not yet on `origin/master` (`git log origin/master..HEAD`).
2. Leave files out that clearly don't belong to the task; ask if you can't tell. Write
   down the final task files; later steps only touch and stage those.

## 1. Review: Claude and Cursor in parallel, read-only

Write one review brief to a temp file and start both reviewers at the same time, each
writing to its own output file:

```bash
claude -p --permission-mode plan < "$BRIEF" > "$TMP/review-claude.md" 2>&1 &
cursor-agent -p --mode ask --trust --workspace "$REPO" "$(cat "$BRIEF")" > "$TMP/review-cursor.md" 2>&1 &
wait
```

Cursor reviews in `ask` mode (read-only): in `plan` mode a long answer ends up in a plan
instead of the output. End the brief with "give your findings as your final text reply".
Check that both output files are non-empty before going on.

Run them in the background and wait for both; reviews can outlast a tool timeout. If
one CLI is missing, run that review in a fresh read-only subagent of your own tool.
Reviewers never edit files and never run test suites.

The review brief contains:
- the repo path, the task files, and how to see the diff (step 0);
- the goal: find **correctness bugs and real risks** in these changes — wrong results,
  wrong units or coordinate systems, broken compile-time checks (invalid code that
  compiles, or valid code that does not), undefined behavior (e.g. dangling Eigen
  expressions), changes that behave differently per backend or compiler, breaking API
  changes, and math or physics that contradicts `DESIGN.md`. Code style and structure
  are out of scope (polish covers them);
- follow the repo's `CLAUDE.md` and rules;
- report every finding as: `severity (high/medium/low) | file:line | what is wrong
  | concrete failure scenario (inputs/state → wrong result) | suggested fix`. No finding
  without a concrete failure scenario. "No findings" is a valid answer.

## 2–3. Verify and fix: Cursor

Start one Cursor agent with write access and Cursor's default model (`auto`):

```bash
cursor-agent -p --force --trust --workspace "$REPO" "$(cat "$FIX_BRIEF")" > "$TMP/fix.md" 2>&1
```

Run it in the background and wait for it. The fix brief contains:
- the repo path, the task files and the diff scope;
- the review files (`$TMP/review-claude.md`, `$TMP/review-cursor.md`, plus any earlier
  review reused under "Don't redo finished work");
- the job:
  1. merge the lists; the same problem found twice is one finding;
  2. **verify each finding against the code**. Reviewers are often confidently wrong.
     Keep a finding only if the failure scenario really happens; otherwise reject it
     with a one-line reason;
  3. fix every confirmed finding, with a unit or compile test;
  4. run fast checks for the touched code, e.g.
     `WUZP_PRESETS="gcc-wuzp-eigen clang-boost-blitz" ci/local.sh format test-latest`.
     Leave the full run to polish;
- the rules: only touch task files and tests for them; never commit, push or change git
  state; never use all CPU cores; if a fix needs a design decision, don't guess — list it
  under `questions`;
- the reply format: a table `finding | reviewers (claude/cursor/both) | verdict
  (confirmed/rejected) | reason or fix`, then `questions:` (or "none").

## 4. Gate

Stop and report before polishing when any of these hold:
- open `questions` from step 2–3;
- a confirmed high-severity finding is not fixed;
- the changes now touch files outside the task list.

## 5. Polish

Run the `polish` skill. Its passes run in Cursor. It must end clean — fix-issues green
(`ci/local.sh` passes) and no open `questions` — otherwise stop and report.

## 6. Commit

- Stage the task files by path. Never `git add -A` or `git add .`.
- Commit in the repo's style (`git log --oneline -10`): a first line that is one sentence
  in the imperative and ends with a period, then a body that explains why. Add the
  attribution trailer your tool is configured to add, if any.
- With `--no-push`, stop here and report.

## 7. Integrate and push

Never force-push. Never switch the branch of a shared checkout.

- **On master:** `git pull --rebase --autostash origin master`, then `git push origin master`.
- **On a branch or worktree:** `git fetch origin && git rebase origin/master`, then
  `git push origin HEAD:master`. Leave the branch and worktree in place and mention them.
- Rebase conflict: resolve it only if the resolution is obvious and confined to task
  files, then rerun polish's fix-issues pass. Otherwise abort the rebase and stop.
- Push rejected because master moved: fetch, rebase once more, push again. If it is
  rejected a second time, stop.

## 8. Watch CI

1. Find the run for the pushed commit: `gh run list --commit "$(git rev-parse HEAD)"`
   (`--commit` needs the full sha, a short one finds nothing).
2. Watch it in the background until it finishes (`gh run watch <id> --exit-status`).
3. On failure, read the failed job's log (`gh run view <id> --log-failed`) and report
   the cause. Don't push a fix without asking.

## Report

Keep it short:
- skipped steps and why;
- review: findings per reviewer, confirmed vs rejected, what was fixed;
- polish: one line per pass;
- the commit (sha + message), where it was pushed, and the CI result with a link;
- anything left open: questions, branches or worktrees to clean up.
