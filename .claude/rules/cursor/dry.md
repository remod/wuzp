<!-- Generated from .cursor/rules/dry.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

Follow the DRY principle. Don't repeat yourself.

- Reuse the helpers and traits in `wuzp/include/wuzp/core/helpers.hpp` and `traits.hpp` instead of duplicating type logic.
- Put operations that every backend needs into the core, and only the backend-specific parts into `wuzp_eigen/`, `wuzp_blitz/`, `wuzp_boost/` or `wuzp_mp_units/`.
- For tests, reuse the helpers in `tests/src/` (e.g. `coordinate_systems.hpp`, `compile_test_types.hpp`, `Unit.hpp`, `ValueMatrix.hpp`) when the same setup appears in several tests.
