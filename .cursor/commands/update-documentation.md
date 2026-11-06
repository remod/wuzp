Review the documentation and ensure it matches the current code and workflows:

- `README.md`: features, installation (CMake components and supported dependency versions), examples, performance.
- `benchmarks/README.md`: how to run the benchmarks, and the measured results. Re-measure them when the backends or the measured operations change.
- `CONTRIBUTING.md`: dev containers, presets, compile tests, CI, versioning and releases.
- `CHANGELOG.md`: an entry in `Unreleased` for each change users notice.
- `DESIGN.md`: conventions (transformation rules, absolute/relative rules), known defects and open design questions. Update the TODOs when a question is decided or a defect is fixed.
- Comments in the public headers for changed APIs.
