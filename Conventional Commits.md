# Conventional Commits

> See: [Coventional Commits Official Website](https://www.conventionalcommits.org/en/v1.0.0/) for more info.

According to the Conventional Commits Guidelines, a commit message should be structured as follows:

```
[Scope] <type>: <description>

[Optional Body]
[Optional Footer]
```

1. **Scope**: The scope of the commit. Because Rikkyu is a multimodule repository, so the scope is very important when you commit a change. The module of the code (which should be the interpreter / compiler you're working for the Esolang) could help others to know which module you are working on.
2. **Type**: The type of the commit. The types should be one of the following:
    - `fix`: You fix a bug.
    - `feat`: You add a new feature.
    - `refactor`: You refactor the code.
    - `docs`: You update the documentation.
    - `ci`: You update the configuration for CI (Continuous Integration) system.
    - `chore`: You update the configuration for the project. (Such as Build System(Which we use is CMake)).
    - `test`: You add or delete a test case.
3. **Description**: The description of the commit. It should be a short description of the change.
4. **Optional Body**: If you want to append more information about the commit, you can append it here.
5. **Optional Footer**: Optional footer should be one of the following:
    - `BREAKING CHANGES`: You make a big change that breaks the compatibility. Which you should make an indicator in commit. Please use `!` as the indicator. For example: `refactor!: Refactor the Command Argument Parser`.

### SemVer referring

- `BREAKING CHANGES` should refers to a MAJOR change.
- `feat.` should refers to a MINOR change.
- `fix.` should refers to a PATCH change.

That's easy enough, huh?