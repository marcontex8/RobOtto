#!/usr/bin/env python3
"""Verify include guards and standard copyright header on repository headers."""

import argparse
import pathlib
import re
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

COMMENT_HEADER_LINES = [
    '/*',
    ' * Copyright (c) 2026 Marco Contessotto',
    ' * All rights reserved.',
    ' */',
]

INCLUDE_GUARD_PATTERN = re.compile(r'^\s*#ifndef\s+([A-Z0-9_]+)')
DEFINE_PATTERN = re.compile(r'^\s*#define\s+([A-Z0-9_]+)')
ENDIF_PATTERN = re.compile(r'^\s*#endif\s*/\*\s*([A-Z0-9_]+)\s*\*/')


def repo_source_files(root: pathlib.Path):
    for path in sorted(root.rglob('*')):
        if path.suffix.lower() not in {'.h', '.c'}:
            continue
        if any(str(path).startswith(str(root / prefix)) for prefix in EXCLUDE_DIRS):
            continue
        yield path


def repo_header_files(root: pathlib.Path):
    return (path for path in repo_source_files(root) if path.suffix.lower() == '.h')


def expected_guard(root: pathlib.Path, path: pathlib.Path) -> str:
    return str(path.relative_to(root)).replace('/', '_').replace('.', '_').upper()


def read_guard_lines(lines):
    ifndef_line = None
    define_line = None
    endif_line = None

    for idx, line in enumerate(lines[:20]):
        m = INCLUDE_GUARD_PATTERN.match(line)
        if m:
            ifndef_line = (idx, m.group(1))
            continue
        if ifndef_line and define_line is None:
            m = DEFINE_PATTERN.match(line)
            if m:
                define_line = (idx, m.group(1))
                break

    for idx in range(len(lines) - 1, -1, -1):
        m = ENDIF_PATTERN.match(lines[idx])
        if m:
            endif_line = (idx, m.group(1))
            break

    return ifndef_line, define_line, endif_line


def find_top_comment_block(lines):
    start = None
    for idx, line in enumerate(lines):
        stripped = line.strip()
        if not stripped:
            continue
        if stripped.startswith('/*'):
            start = idx
        break

    if start is None or not lines[start].strip().startswith('/*'):
        return None

    end = None
    for idx in range(start, min(start + 40, len(lines))):
        if lines[idx].strip().endswith('*/'):
            end = idx
            break

    if end is None:
        return None

    return start, end


def read_comment_header(lines):
    header_block = find_top_comment_block(lines)
    if header_block is None:
        return None

    start, end = header_block
    block_lines = [lines[idx].rstrip() for idx in range(start, end + 1)]
    return (start, end, block_lines == COMMENT_HEADER_LINES)


def verify_header(path: pathlib.Path):
    lines = path.read_text(encoding='utf-8', errors='ignore').splitlines()
    header_info = read_comment_header(lines)
    if header_info is None:
        return False, ['missing standard copyright header']

    start, end, match = header_info
    if not match:
        return False, ['copyright header block does not match expected text']

    return True, []


def verify_guard(root: pathlib.Path, path: pathlib.Path):
    lines = path.read_text(encoding='utf-8', errors='ignore').splitlines()
    expected = expected_guard(root, path)
    ifndef_line, define_line, endif_line = read_guard_lines(lines)
    issues = []

    if ifndef_line is None or define_line is None or endif_line is None:
        issues.append('missing guard or guard comment')
        return False, expected, issues

    if ifndef_line[1] != expected:
        issues.append(f'ifndef is {ifndef_line[1]!r}, expected {expected!r}')
    if define_line[1] != expected:
        issues.append(f'define is {define_line[1]!r}, expected {expected!r}')
    if endif_line[1] != expected:
        issues.append(f'endif comment is {endif_line[1]!r}, expected {expected!r}')

    if define_line[0] > ifndef_line[0] + 1 and all(
        not lines[idx].strip() for idx in range(ifndef_line[0] + 1, define_line[0])
    ):
        issues.append('blank line between #ifndef and #define')

    return len(issues) == 0, expected, issues


def fix_header(path: pathlib.Path):
    lines = path.read_text(encoding='utf-8', errors='ignore').splitlines()
    header_info = read_comment_header(lines)
    modified = False

    if header_info is None:
        insert_at = 0
        for idx, line in enumerate(lines):
            if line.strip():
                insert_at = idx
                break
        lines[insert_at:insert_at] = COMMENT_HEADER_LINES + ['']
        modified = True
    else:
        start, end, match = header_info
        if not match:
            lines[start:end + 1] = COMMENT_HEADER_LINES
            if end + 1 < len(lines) and lines[end + 1].strip():
                lines.insert(end + 1, '')
            modified = True

    if modified:
        path.write_text('\n'.join(lines) + '\n', encoding='utf-8')

    return modified


def fix_guard(root: pathlib.Path, path: pathlib.Path, expected: str):
    lines = path.read_text(encoding='utf-8', errors='ignore').splitlines()
    ifndef_line, define_line, endif_line = read_guard_lines(lines)
    header_info = read_comment_header(lines)
    modified = False

    if ifndef_line is not None and define_line is not None:
        blank_start = ifndef_line[0] + 1
        blank_end = define_line[0]
        if blank_end > blank_start and all(not lines[idx].strip() for idx in range(blank_start, blank_end)):
            del lines[blank_start:blank_end]
            modified = True
            ifndef_line, define_line, endif_line = read_guard_lines(lines)

    if ifndef_line is None:
        insert_at = 0
        if header_info is not None:
            insert_at = header_info[1] + 1
        else:
            for idx, line in enumerate(lines):
                stripped = line.strip()
                if stripped and not stripped.startswith('/*') and not stripped.startswith('//'):
                    insert_at = idx
                    break

        lines[insert_at:insert_at] = [f'#ifndef {expected}', f'#define {expected}', '']
        modified = True
    elif ifndef_line[1] != expected:
        lines[ifndef_line[0]] = f'#ifndef {expected}'
        modified = True

    if define_line is None:
        insert_at = ifndef_line[0] + 1 if ifndef_line else 2
        lines.insert(insert_at, f'#define {expected}')
        modified = True
    elif define_line[1] != expected:
        lines[define_line[0]] = f'#define {expected}'
        modified = True

    if endif_line is None:
        if lines and lines[-1].strip():
            lines.append('')
        lines.append(f'#endif /* {expected} */')
        modified = True
    elif endif_line[1] != expected:
        lines[endif_line[0]] = f'#endif /* {expected} */'
        modified = True

    if modified:
        path.write_text('\n'.join(lines) + '\n', encoding='utf-8')

    return modified


def main() -> int:
    parser = argparse.ArgumentParser(
        description='Verify include guards and standard copyright header in repository headers.'
    )
    parser.add_argument('--fix', action='store_true', help='Fix mismatches in-place')
    parser.add_argument('--root', type=pathlib.Path, default=pathlib.Path.cwd(), help='Repository root path')
    args = parser.parse_args()

    root = args.root.resolve()
    failed = False
    changed = False

    for path in repo_source_files(root):
        header_ok, header_issues = verify_header(path)
        guard_ok = True
        guard_issues = []
        expected = None

        if path.suffix.lower() == '.h':
            guard_ok, expected, guard_issues = verify_guard(root, path)

        if guard_ok and header_ok:
            continue

        print(f'[{path}]')
        for issue in header_issues:
            print('  -', issue)
        for issue in guard_issues:
            print('  -', issue)

        if args.fix:
            if not header_ok and fix_header(path):
                print('  header fixed')
                changed = True
            if path.suffix.lower() == '.h' and not guard_ok and fix_guard(root, path, expected):
                print('  guard fixed')
                changed = True
        else:
            failed = True

    if failed:
        print('\nERROR: include guard or header verification failed.')
        return 1
    if changed:
        print('\nFixed header and/or guard issues.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
