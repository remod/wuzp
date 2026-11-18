---
name: prepare-to-ship
description: Take a task's finished changes (e.g. one issue) to a commit on local master that is ready to push — parallel review (Claude + Cursor), Cursor verifies and fixes the findings, gate, polish, commit, and merge into local master. Everything ship does except pushing and watching CI. Skips steps that already ran on exactly these changes. Run it for every task on its own, when the user asks for it (e.g. "/prepare-to-ship"); typing it is the approval to commit. ship runs it as its first step.
---

# Prepare to ship

Takes the changes of one task, e.g. one issue, from "works" to "reviewed, polished, committed and on local master, ready to
push". Local master is the queue of finished commits; `ship` runs this skill and then pushes local master, so every commit
queued there goes out in one push (one CI run). It never pushes itself.
CI tests every backend combination on the oldest and the newest supported dependency versions, so every step below can stop
the run. Stopping is always fine; committing something half-checked is not.

> Adapted from the shared `prepare-to-ship` skill of other repositories. This copy is maintained here: wuzp is a single
> repository without deployment, and its default branch is `master`.

## Arguments

- A base ref (e.g. `origin/master`): review `git diff <base>` instead of the default scope.

## Don't redo finished work

Before each step, check whether it already happened **on exactly the current changes**:
earlier in this conversation (a review, `/code-review`, polish or a test run on these
files), or because the user says so ("already reviewed", "polish ran"), or from git
(already committed, already on local master).

- A step counts as done only if none of the task files changed after it ran. If files
  changed since, redo just what the change affects: after a small follow-up edit,
  polish's fix-issues pass is enough; after a large one, rerun the step.
- Reuse earlier output instead of repeating it: findings from an earlier review go into
  step 3 as an extra review file.
- When unsure whether a step ran on the current state, run it.
- List skipped steps and the reason in the report.

## 1. Scope

1. Note the branch (`git branch --show-current`) and whether it is a worktree. The **task
   files** are the uncommitted changes (`git status --short`, `git diff HEAD`) plus, on a
   branch, its commits not yet on local master (`git log master..HEAD`).
2. The main checkout often holds other agents' uncommitted work. Leave files out that
   clearly don't belong to the task; ask if you can't tell. Write down the final task
   files; later steps only touch and stage those.

## 2. Review: Claude and Cursor in parallel, read-only

Write one review brief to a temp file and start both reviewers at the same time, each
writing to its own output file:

```bash
claude -p --model opus --effort medium --permission-mode plan < "$BRIEF" > "$TMP/review-claude.md" 2>&1 &
cursor-agent -p --mode ask --trust --workspace "$REPO" "$(cat "$BRIEF")" > "$TMP/review-cursor.md" 2>&1 &
wait
```

Claude runs on Opus at medium effort (pinned so the reviewer never inherits a pricier default). Cursor reviews in `ask` mode
(read-only): in `plan` mode a long answer ends up in a plan instead of the output. End the brief with "give your findings as
your final text reply". Check that both output files are non-empty before going on.

Run them in the background and wait for both; reviews can outlast a tool timeout. If
one CLI is missing or not allowed here, run that review in a fresh read-only subagent of
your own tool. Reviewers never edit files and never run test suites.

The review brief contains:
- the repo path (the worktree, if the task is on one), the task files, and how to see the
  diff (step 1);
- the goal: find **correctness bugs and real risks** in these changes — wrong results,
  wrong units or coordinate systems, broken compile-time checks (invalid code that
  compiles, or valid code that does not), undefined behavior (e.g. dangling Eigen
  expressions), changes that behave differently per backend or compiler, breaking API
  changes, and math or physics that contradicts `DESIGN.md` or its sources. Code style
  and structure are out of scope (polish covers them);
- follow the repo's `CLAUDE.md` and rules;
- report every finding as: `severity (high/medium/low) | file:line | what is wrong
  | concrete failure scenario (inputs/state → wrong result) | suggested fix`. No finding
  without a concrete failure scenario. "No findings" is a valid answer.

## 3. Verify and fix: Cursor

Start one Cursor agent with write access and Cursor's default model (`auto`):

```bash
cursor-agent -p --force --trust --workspace "$REPO" "$(cat "$FIX_BRIEF")" > "$TMP/fix.md" 2>&1
```

Run it in the background and wait for it. If `cursor-agent` is missing or not allowed
here, run the step in a fresh subagent of your own tool with the same brief. The fix
brief contains:
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
- open `questions` from step 3;
- a confirmed high-severity finding is not fixed;
- the changes now touch files outside the task list (`git status`).

## 5. Polish

Run the `polish` skill on the task's changes (in the worktree, if the task is on one). It
must end clean — fix-issues green (`ci/local.sh` passes) and no open `questions` —
otherwise stop and report.

## 6. Commit

- Stage the task files by path. Never `git add -A` or `git add .` in a shared checkout.
- Commit in the repo's style (`git log --oneline -10`): a first line that is one sentence
  in the imperative and ends with a period, then a body that explains why. Reference the
  issue (`Fixes #N.`, one keyword per issue). Add the attribution trailer your tool is
  configured to add, if any.
- Don't push or change branches.

## 7. Merge into local master

If the task was committed on a branch or worktree (skip this if it was committed on master
itself):

1. In the worktree, with a clean working tree: `git rebase master` (local master, not
   `origin/master`). If master hasn't moved since the branch was made, this does nothing.
2. Rebase conflict: resolve it only if the resolution is obvious and confined to task
   files (e.g. two entries added at the same place in `CHANGELOG.md`). Otherwise
   `git rebase --abort` and stop.
3. If the rebase brought in new commits, with or without conflicts, run polish's
   fix-issues pass again in the worktree. It must end clean, otherwise stop. A clean
   rebase can still break the build, e.g. when a function the task calls was renamed on
   master.
4. In the main checkout: `git merge --ff-only <branch>`. Never stash, reset or switch
   branches there: it holds other agents' uncommitted work, and the stash list is shared.
   If git refuses because the main checkout has uncommitted changes to files the commit
   touches, stop and ask. If it refuses because master moved again meanwhile, go back to
   1 once; if it happens again, stop.
5. Leave the branch and worktree in place until ship has pushed; follow-up fixes go there.

## Report

Keep it short:
- skipped steps and why;
- review: findings per reviewer, confirmed vs rejected (one-line reason each), what was
  fixed;
- polish: one line per pass;
- the branch or worktree, the commit (sha + message), and how it reached local master
  (fast-forward, or rebased onto N new commits plus the fix-issues result);
- anything left open, each on its own line or "none": questions, confirmed high-severity
  findings not fixed, files changed outside the task files.
