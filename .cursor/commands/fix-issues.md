1. Run `ci/local.sh` (format, clang-tidy, all presets on `min` and `latest`, coverage, sanitizers, install). It runs in the dev containers and uses half of the CPU cores.
2. Fix format issues with the pinned clang-format, e.g. in the dev container: `cmake --build --preset gcc-wuzp-eigen --target wuzp_format`.
3. Fix compile errors, warnings and failing tests. Ask if you're not sure about the fix.
4. Re-run the failing jobs (e.g. `WUZP_PRESETS="clang-boost-blitz" ci/local.sh test-latest`) until they pass, then run `ci/local.sh` once more.
