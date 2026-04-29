# CI Scripts

This folder contains scripts for maintaining code quality in the Robotto project.

## Files

- `.clang-format`: Clang-format style configuration (Allman braces, 4-space indent, etc.)
- `check_include_guards.py`: Verifies and fixes include guards and copyright headers
- `format_code.py`: Runs clang-format on source files
- `hooks/pre-commit`: Git pre-commit hook that runs both checks

## Commands

### Include Guards and Headers

Check for issues:
```bash
python3 ci/check_include_guards.py --root .
```

Fix issues automatically:
```bash
python3 ci/check_include_guards.py --root . --fix
```

### Code Formatting

Check for formatting issues:
```bash
python3 ci/format_code.py --root .
```

Apply formatting fixes:
```bash
python3 ci/format_code.py --root . --fix
```

### Pre-commit Hook

To enable automatic checks on commit:
```bash
ln -sf ../../ci/hooks/pre-commit .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
```

The hook runs both include guard and formatting checks.

## What They Do

- **Include Guards**: Ensures `.h` files have full relative-path guards and standard copyright headers in all `.c`/`.h` files
- **Formatting**: Applies consistent C/C++ formatting using clang-format with the provided style

Both scripts exclude third-party and generated code directories.