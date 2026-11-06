To initialize your context for this repo:

1. Read all rules in `.cursor/rules/`.
2. Read `README.md` (usage), `CONTRIBUTING.md` (dev containers, building, testing, CI) and `DESIGN.md` (conventions, review results, open design questions).
3. Stack: **C++20**, header-only, **CMake** (presets per compiler and backend combination), **GoogleTest**, backends **Eigen**, **Blitz++**, **Boost.Units** and wuzp's own units. Development and CI run in the dev containers in `.devcontainer/` (`min`: Ubuntu 22.04, `latest`: Ubuntu 26.04).
