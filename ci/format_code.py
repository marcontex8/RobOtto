#!/usr/bin/env python3
"""Run clang-format on repository source files using the style from ci/.clang-format."""

import argparse
import pathlib
import subprocess
import sys

EXCLUDE_DIRS = {
    '.settings',
    '.venv',
    'Core',
    'Debug',
    'Documentation',
    'Drivers',
    'Esp32',
    'ThirdParty',
    'UnitTests/build/',
}


def repo_source_files(root: pathlib.Path):
    for path in sorted(root.rglob('*')):
        if path.suffix.lower() not in {'.h', '.c'}:
            continue
        if any(str(path).startswith(str(root / prefix)) for prefix in EXCLUDE_DIRS):
            continue
        yield path


def run_clang_format(root: pathlib.Path, path: pathlib.Path, fix: bool) -> bool:
    """Run clang-format on a file. Return True if file was changed or needs change."""
    style_file = root / 'ci' / '.clang-format'
    cmd = ['clang-format', f'--style=file:{style_file}']
    if fix:
        cmd.extend(['-i', str(path)])
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, cwd=root)
            return result.returncode == 0
        except FileNotFoundError:
            print("ERROR: clang-format not found. Please install clang-format.")
            sys.exit(1)
    else:
        # Check mode: run clang-format and compare output to original
        cmd.append(str(path))
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, cwd=root)
            if result.returncode != 0:
                return False  # Error in formatting
            original = path.read_text(encoding='utf-8', errors='ignore')
            return result.stdout == original
        except FileNotFoundError:
            print("ERROR: clang-format not found. Please install clang-format.")
            sys.exit(1)


def main() -> int:
    parser = argparse.ArgumentParser(
        description='Run clang-format on repository source files.'
    )
    parser.add_argument('--fix', action='store_true', help='Apply formatting fixes in-place')
    parser.add_argument('--root', type=pathlib.Path, default=pathlib.Path.cwd(), help='Repository root path')
    args = parser.parse_args()

    root = args.root.resolve()
    style_file = root / 'ci' / '.clang-format'
    if not style_file.exists():
        print(f"ERROR: Style file not found: {style_file}")
        return 1

    failed = False
    changed = False

    for path in repo_source_files(root):
        ok = run_clang_format(root, path, args.fix)
        if ok:
            continue

        print(f'[{path}] formatting issues')
        if args.fix:
            # If fix mode and it returned False, something went wrong
            failed = True
        else:
            failed = True

    if failed and not args.fix:
        print('\nERROR: Code formatting issues found.')
        return 1
    if changed and args.fix:
        print('\nApplied formatting fixes.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
