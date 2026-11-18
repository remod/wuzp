---
name: ship
description: Take a task's finished changes to master in one go — prepare-to-ship (parallel Claude + Cursor review, Cursor verifies and fixes the findings, gate, polish, commit, merge into local master), then push local master and watch CI. Skips steps that already ran on exactly these changes. Run it only when the user asks to ship (e.g. "/ship", "ship it"); typing it is the approval to push. To stop before pushing, use prepare-to-ship.
---

# Ship

Takes the changes of one task from "works" to "pushed to master and green in CI".
wuzp has no deployment, but master must stay green: CI tests every backend combination on the oldest and the newest supported
dependency versions, so every step below can stop the run.
Stopping is always fine; pushing something half-checked is not.

> Adapted from the shared `ship` skill of other repositories. This copy is maintained here: wuzp is a single
> repository without deployment, and its default branch is `master`.

## Arguments

- A base ref (e.g. `origin/master`): review `git diff <base>` instead of the default scope.

## Don't redo finished work

Before each step, check whether it already happened **on exactly the current changes**:
earlier in this conversation, or from git (already committed, already pushed, CI already
green for that sha). When unsure, run the step. List skipped steps and the reason in the
report.

## 1. Prepare

Run the `prepare-to-ship` skill with the same base ref. It reviews, fixes, polishes and
commits the task, and merges the commit into local master; it skips its own steps that
already ran. If it stops with anything left open, stop too and report it.

## 2. Push local master

Never force-push. Never switch branches, stash or rebase in the main checkout: it holds
other agents' uncommitted work, and the stash list is shared.

1. `git fetch origin`, then list what goes out: `git log --oneline origin/master..master`.
   Every commit there is pushed, including other tasks' commits queued on local master.
   If one looks unfinished (e.g. an agent is still working on that task), ask first.
2. If `origin/master` is an ancestor of `master` (`git merge-base --is-ancestor
   origin/master master`): `git push origin master`. This touches no working tree.
3. Otherwise someone pushed to `origin/master` meanwhile. Integrate in a worktree: the
   task's worktree (run `git merge --ff-only master` there first), or, if the task was
   committed on master, a temporary one (`git worktree add --detach
   .claude/worktrees/ship-rebase master`, removed afterwards). There:
   - `git rebase origin/master`. Conflict: resolve it only if the resolution is obvious
     and confined to task files; otherwise abort the rebase and stop.
   - Run polish's fix-issues pass. It must end clean, otherwise stop.
   - `git push origin HEAD:master`.
   - Bring local master along in the main checkout: `git reset --keep origin/master`. It
     keeps uncommitted work and refuses if it would overwrite any; if it refuses, report
     it (the push is done, local master just lags). If local master got new commits since
     the rebase started, don't reset: report it instead, those commits aren't pushed.
4. Push rejected because `origin/master` moved between fetch and push: go to 3 once more.
   If it is rejected again, stop.

## 3. Watch CI

1. Find the run for the pushed commit: `gh run list --commit "$(git rev-parse master)"`
   (`--commit` needs the full sha, a short one finds nothing).
2. Watch it in the background until it finishes (`gh run watch <id> --exit-status`).
3. On failure, read the failed job's log (`gh run view <id> --log-failed`) and report
   the cause. Don't push a fix without asking.

## Report

Keep it short:
- prepare-to-ship's report (skipped steps, review, polish, commits);
- the commits pushed (from step 2.1) and the CI result with a link;
- anything left open: questions, and the task's branches and worktrees, now pushed, that
  can be removed.
