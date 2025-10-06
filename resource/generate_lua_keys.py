#!/usr/bin/env python3
"""
generate_lua_glue.py

Usage:
    python generate_lua_glue.py path/to/header.h > header_lua.c

The script reads a C header, extracts all simple #define constants
and writes a chunk of C code that pushes those constants into Lua
as globals.

Author: <your name>
"""

import re
import sys
from pathlib import Path

# ------------------------------------------------------------------
# 1)  Read the header
# ------------------------------------------------------------------
def read_header(path: Path) -> str:
    """Return the raw text of the header file."""
    try:
        return path.read_text(encoding="utf-8")
    except Exception as exc:
        sys.exit(f"Error reading {path}: {exc}")

# ------------------------------------------------------------------
# 2)  Strip comments
# ------------------------------------------------------------------
def strip_comments(text: str) -> str:
    """
    Remove block comments /* … */ and line comments // … from C code.
    The regexes are intentionally permissive; they work well enough for
    most headers and avoid the complexity of a full C lexer.
    """
    # 1. Block comments (non‑greedy)
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)

    # 2. Line comments
    text = re.sub(r"//.*", "", text)

    return text

# ------------------------------------------------------------------
# 3)  Extract simple #define constants
# ------------------------------------------------------------------
def extract_defines(text: str) -> list[str]:
    """
    Return a list of identifiers that are defined by a plain
    #define (i.e. no macro arguments).

    Example:
        #define EV_ABS 0          →   "EV_ABS"
        #define SET_VALUE(x) 1    →   (ignored)
    """
    defines = []
    # A single line that starts with #define and contains the name.
    # The `(?!\s*\()` part ensures that a macro with arguments is skipped.
    pattern = re.compile(r"^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\b(?!\s*\()", re.M)

    for match in pattern.finditer(text):
        defines.append(match.group(1))

    return defines

# ------------------------------------------------------------------
# 4)  Generate Lua glue code
# ------------------------------------------------------------------
def generate_lua_glue(defines: list[str]) -> str:
    """
    Produce C code that pushes each constant as a Lua global.
    """
    lines = []
    for name in defines:
        lines.append(f"lua_pushnumber(L, {name});")
        lines.append(f'lua_setglobal(L, "{name}");')
        lines.append("")          # Blank line for readability
    return "\n".join(lines)

# ------------------------------------------------------------------
# 5)  Main routine
# ------------------------------------------------------------------
def main(argv: list[str]) -> None:
    if len(argv) != 2:
        sys.exit("Usage: python generate_lua_glue.py <header.h> > <generated_file.c>")

    header_path = Path(argv[1])
    if not header_path.is_file():
        sys.exit(f"File not found: {header_path}")

    raw_text = read_header(header_path)
    text_no_comments = strip_comments(raw_text)
    defines = extract_defines(text_no_comments)

    if not defines:
        sys.exit("No simple #define constants found in the header.")

    glue_code = generate_lua_glue(defines)
    print(glue_code)


if __name__ == "__main__":
    main(sys.argv)
