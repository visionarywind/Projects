#!/usr/bin/env python3
"""Validate C++ blocks in generated detailed solution files."""

from __future__ import annotations

import argparse
import re
import subprocess
import tempfile
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parents[1]
DETAIL_DIR = BASE_DIR / "01-question-bank" / "detailed-solutions"
AUDIT_DIR = BASE_DIR / "05-solution-audit"


def extract_cpp_blocks(text: str) -> list[str]:
    return re.findall(r"```cpp\n(.*?)\n```", text, re.S)


def validate_file(path: Path, compile_check: bool) -> tuple[str, str]:
    text = path.read_text(encoding="utf-8")
    blocks = extract_cpp_blocks(text)
    if not blocks:
        return "missing-cpp", "未发现 C++ 代码块"
    code = blocks[0]
    if "class Solution" not in code:
        return "missing-solution-class", "C++ 代码块缺少 class Solution"
    if "TODO" in code:
        return "not-run", "preview 骨架含 TODO，需要按题面签名补全"
    if not compile_check:
        return "not-run", "未请求编译，仅完成结构检查"
    with tempfile.TemporaryDirectory() as tmp:
        cpp = Path(tmp) / "main.cpp"
        cpp.write_text(code + "\nint main() { return 0; }\n", encoding="utf-8")
        result = subprocess.run(["g++", "-std=c++17", str(cpp), "-o", str(Path(tmp) / "a.out")], capture_output=True, text=True, timeout=20)
        if result.returncode != 0:
            return "compile-failed", result.stderr.strip().splitlines()[-1] if result.stderr.strip() else "编译失败"
    return "compiled", "C++17 编译通过基础 harness"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--batch", default="full", choices=["preview", "full"])
    parser.add_argument("--compile", action="store_true")
    args = parser.parse_args()
    rows = []
    counts: dict[str, int] = {}
    for path in sorted(DETAIL_DIR.rglob("*.md")):
        status, note = validate_file(path, args.compile)
        counts[status] = counts.get(status, 0) + 1
        rows.append((path.relative_to(BASE_DIR), status, note))
    AUDIT_DIR.mkdir(parents=True, exist_ok=True)
    lines = [
        "# C++ 验证状态",
        "",
        f"批次：{args.batch}",
        f"编译检查：{'启用' if args.compile else '未启用'}",
        "",
        "## 汇总",
        "",
        "| 状态 | 数量 |",
        "|---|---:|",
    ]
    for status, count in sorted(counts.items()):
        lines.append(f"| {status} | {count} |")
    lines.extend(["", "## 明细", "", "| 文件 | 状态 | 说明 |", "|---|---|---|"])
    for rel, status, note in rows:
        lines.append(f"| {rel} | {status} | {note.replace('|', '/')} |")
    (AUDIT_DIR / "cpp-validation-state.md").write_text("\n".join(lines).rstrip() + "\n", encoding="utf-8")
    print(f"validated {len(rows)} detailed solution files")
    return 0 if rows else 1


if __name__ == "__main__":
    raise SystemExit(main())
