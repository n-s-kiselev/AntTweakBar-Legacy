# AI Development Standard

This package provides a reusable project instruction infrastructure for working with both OpenAI Codex and Claude Code.

## Included files

```text
AGENTS.md
CLAUDE.md
PLANS.md
.agents/skills/
.claude/skills/
```

`AGENTS.md` is the shared source of truth.

`CLAUDE.md` imports `AGENTS.md` with:

```markdown
@AGENTS.md
```

The skills are duplicated intentionally under both `.agents/skills/` and `.claude/skills/`. No symbolic links are used.

## Installation

Copy all files and directories from this package into the root of a project repository.

Do not overwrite an existing `AGENTS.md`, `CLAUDE.md`, or `PLANS.md` without reviewing and merging project-specific instructions.

After copying:

1. Complete the **Project-Specific Information** section in `AGENTS.md`.
2. Verify the bootstrap and build commands.
3. Add project-specific architecture and test documentation.
4. Commit the standard together with the initial project configuration.
5. Keep duplicate skills synchronized when editing the standard.

## Updating a skill

Because symbolic links are intentionally not used, every shared skill exists twice:

```text
.agents/skills/<name>/SKILL.md
.claude/skills/<name>/SKILL.md
```

When changing one copy, apply the identical change to the other copy and verify they match.

A simple verification command is:

```sh
diff -ru .agents/skills .claude/skills
```

No output means the two skill trees are identical.

## Included skills

- `git-workflow`
- `minimal-patch`
- `bug-investigation`
- `existing-pattern`
- `code-review`
- `build-verification`
- `large-feature-development`
- `documentation-update`
- `release-checklist`

## Project-specific rules

Do not add project-specific architecture details to reusable skills.

Place them in:

- the **Project-Specific Information** section of `AGENTS.md`;
- normal project documentation;
- a task-specific plan under `docs/plans/`.

The root standard can later be extended with rules for individual subdirectories, but this package intentionally does not include them.
