import re
import shutil
import subprocess
from pathlib import Path


def parse_tone_enum(header_path):
    """Parse the Tone enum from melody.h to build a name->frequency map."""
    content = header_path.read_text()
    tone_map = {}
    in_enum = False
    for line in content.splitlines():
        if "enum Tone" in line:
            in_enum = True
            continue
        if in_enum:
            if "};" in line:
                break
            m = re.match(r"\s*(\w+)\s*=\s*(\d+)", line)
            if m:
                tone_map[m.group(1)] = int(m.group(2))
    return tone_map


def eval_int_expr(expr):
    """Safely evaluate a simple integer arithmetic expression (e.g. '4/2')."""
    expr = expr.strip()
    if re.fullmatch(r"[\d+\-*/() ]+", expr):
        return int(eval(expr))  # noqa: S307 — only digits and arithmetic ops
    return int(expr)


def run_preprocessor(file_path, include_dirs):
    """Run the C preprocessor on a file and return the output."""
    cpp = shutil.which("cpp") or shutil.which("gcc")
    if cpp is None:
        return None

    cmd = [cpp]
    if "gcc" in Path(cpp).name:
        cmd.append("-E")
    for d in include_dirs:
        cmd.extend(["-I", str(d)])
    cmd.append(str(file_path))

    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        return None
    # Strip preprocessor line markers (# <line> "file" ...)
    lines = [l for l in result.stdout.splitlines() if not l.startswith("#")]
    return "\n".join(lines)


def find_melody_files(project_root):
    """Find all melody.cpp files in lib subdirectories"""
    lib_path = project_root / "lib"
    return list(lib_path.glob("*/melody.cpp"))


def parse_melodies(file_path, tone_map, include_dirs=None):
    """Extract all named melody arrays from a C++ file.

    Returns a dict of {melody_name: [(frequency, beats), ...]}.
    """
    content = None
    if include_dirs is not None:
        content = run_preprocessor(file_path, include_dirs)

    if content is None:
        content = file_path.read_text()

    # Remove block comments
    content = re.sub(r"/\*.*?\*/", "", content, flags=re.DOTALL)
    # Remove line comments
    content = re.sub(r"//[^\n]*", "", content)

    melodies = {}
    # Match: const Note melody_name[] = { ... };
    array_pattern = r"const\s+Note\s+(\w+)\s*\[\s*\]\s*=\s*\{(.*?)\};"
    for m in re.finditer(array_pattern, content, re.DOTALL):
        name = m.group(1)
        body = m.group(2)

        notes = []
        note_pattern = r"\{\s*(\w+)\s*,\s*([\d+\-*/() ]+)\s*\}"
        for nm in re.finditer(note_pattern, body):
            tone_name = nm.group(1)
            beats = eval_int_expr(nm.group(2))
            freq = tone_map.get(tone_name, 0)
            notes.append((freq, beats))

        # Remove trailing end_of_melody sentinel (freq=0, beats=0)
        if notes and notes[-1] == (0, 0):
            notes.pop()

        melodies[name] = notes

    return melodies
