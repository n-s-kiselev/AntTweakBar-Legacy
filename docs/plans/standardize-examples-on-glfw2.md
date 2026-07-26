# Standardize examples on vendored GLFW2

Status: Completed
Owner: Codex
Last updated: 2026-07-26

## 1. Goal

Move GLFW3 and its cursor-integration investigation to the neighboring
`AntTweakBarGLFW3` repository, and make every GLFW example in this legacy
repository build exclusively against the vendored GLFW 2.7.9 copy.

## 2. Motivation

AntTweakBar's native cursor handling is compatible with GLFW2 but conflicts
with GLFW3 on macOS and Linux. GLFW3 integration will be developed separately;
this repository should favor examples that work completely today.

## 3. Current State

Eight examples use GLFW3, one successful reference example uses GLFW2, and
`nob.c` builds both vendored versions. The neighboring repository already has
an older `external/glfw` tree and uncommitted user changes outside it.

## 4. Requirements

- Preserve the current GLFW3 source and design conclusions in
  `AntTweakBarGLFW3`.
- Remove GLFW3 dependency/build paths from this repository.
- Build every GLFW example with local GLFW2.
- Preserve GLUT behavior, including the Linux-only exclusion of TwDualGLUT.
- Validate, commit, and push both repositories.

## 5. Non-Goals

- Implement the proposed AntTweakBar cursor callback API.
- Repair GLFW3 cursor behavior in this repository.
- Change the rendered content or controls of examples.

## 6. Constraints

- Keep `nob.c` as the only build system.
- Preserve unrelated uncommitted changes in the neighboring repository.
- Keep GLFW2's existing no-GLU and Retina fixes.

## 7. Relevant Files and Components

- `nob.c`, `vendor/glfw*`, `examples/*GLFW*`, and project documentation.
- `../AntTweakBarGLFW3/external/glfw` and a new design note there.

## 8. Existing Patterns to Reuse

`TwSimpleGLFW2.c` supplies the working GLFW2 initialization, callback,
framebuffer scaling, mouse scaling, and widget scaling behavior.

## 9. Proposed Design

Use a small example-local GLFW2 adapter to centralize the mechanical
single-window API differences while all actual windowing calls resolve to
GLFW2. Keep the example rendering and AntTweakBar logic unchanged. Collapse
the build to one GLFW example kind and one vendored GLFW object.

## 10. Implementation Steps

### Step 1: Preserve GLFW3 work in the neighboring repository

Status: Completed

### Step 2: Port every GLFW example to local GLFW2

Status: Completed

### Step 3: Remove GLFW3 build and vendor fingerprints

Status: Completed

### Step 4: Update documentation

Status: Completed

### Step 5: Validate, review, commit, and push both repositories

Status: Completed

## 11. Validation Strategy

Run `./nob -clean && ./nob && ./nob -examples` on macOS, smoke-launch the
resulting examples, preprocess the Linux example list, inspect both diffs,
and verify clean status after pushes. Linux and MinGW compilation must be
reported as untested locally.

## 12. Risks and Mitigations

The GLFW2 single-window API differs substantially from GLFW3. Centralizing
callback signature and Retina-coordinate translation avoids eight divergent
ports and follows the proven reference behavior.

## 13. Open Questions

None.

## 14. Progress Log

### 2026-07-26

- Inspected both repositories and recorded pre-existing neighboring changes.
- Confirmed the GLFW3 source trees differ and that all eight examples use the
  same small GLFW3 API surface.
- Copied the exact legacy vendored GLFW3 tree to `external/glfw` in the
  neighboring repository and added the cursor-integration design note.
- Added the single-window GLFW2 adapter and moved all GLFW examples/build
  entries to the vendored GLFW2 object.
- Removed the local GLFW3 vendor/build path and updated current documentation.
- A clean macOS library/examples build produced all 12 examples; all nine GLFW
  executables stayed running during smoke launches.

## 15. Final Result

The legacy repository now has a single vendored GLFW implementation (2.7.9),
and every GLFW example uses it. GLFW3 source and future cursor-binding design
are preserved in the dedicated neighboring repository.

## 16. Remaining Limitations

Linux and Windows/MinGW builds were not available locally and require
confirmation on those platforms.
