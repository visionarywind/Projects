#!/usr/bin/env python3
"""Generate solution coverage audit for detailed solution files."""

from __future__ import annotations

from collections import Counter
from datetime import datetime, timezone
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parents[1]
DETAIL_DIR = BASE_DIR / "01-question-bank" / "detailed-solutions"
SOURCE_EXPLANATION_DIR = BASE_DIR / "02-source-explanations"
AUDIT_DIR = BASE_DIR / "05-solution-audit"


def solution_status(text: str) -> str:
    prefix = "- 外部题解授权状态："
    for line in text.splitlines():
        if line.startswith(prefix):
            return line[len(prefix):].strip()
    return "unknown"


def has_imported_body(text: str) -> bool:
    marker = "## 授权导入：灵茶山艾府题解过程"
    if marker not in text:
        return False
    body = text.split(marker, 1)[1].split("\n## ", 1)[0].strip()
    return bool(body) and not body.startswith("> 本节用于保存")


def main() -> int:
    fetched_at = datetime.now(timezone.utc).astimezone().strftime("%Y-%m-%d %H:%M:%S %z")
    detail_files = sorted(DETAIL_DIR.rglob("*.md"))
    explanation_files = sorted(SOURCE_EXPLANATION_DIR.glob("*.md"))
    by_topic = Counter(path.parent.name for path in detail_files)
    statuses = Counter()
    imported_body_count = 0
    for path in detail_files:
        text = path.read_text(encoding="utf-8")
        statuses[solution_status(text)] += 1
        imported_body_count += has_imported_body(text)

    lines = [
        "# 详解覆盖审计",
        "",
        f"生成时间：{fetched_at}",
        "批次：full（按来源分类出现记录生成）",
        "",
        "## 汇总",
        "",
        f"- 来源分类解析文件：{len(explanation_files)}",
        f"- 单题详解文件：{len(detail_files)}",
        f"- 已导入灵茶山艾府正文：{imported_body_count}",
        f"- `authorized-import`：{statuses.get('authorized-import', 0)}",
        f"- `pending-fetch`：{statuses.get('pending-fetch', 0)}",
        f"- `missing-endlesscheng-solution`：{statuses.get('missing-endlesscheng-solution', 0)}",
        f"- 其他/缺少状态：{sum(count for status, count in statuses.items() if status not in {'authorized-import', 'pending-fetch', 'missing-endlesscheng-solution'})}",
        "- 导入接口：LeetCode GraphQL `questionSolutionArticles` + `solutionArticle`；批量运行可能受到站点频率限制或超时影响。",
        "- 本地解析状态：当前仍为生成模板，C++ 答案和逐题原创推导需独立复核。",
        "",
        "## 按专题统计",
        "",
        "| 专题目录 | 详解文件数 | 已导入正文 | pending/缺失 |",
        "|---|---:|---:|---:|",
    ]
    for topic, count in sorted(by_topic.items()):
        topic_files = [path for path in detail_files if path.parent.name == topic]
        topic_imported = sum(has_imported_body(path.read_text(encoding="utf-8")) for path in topic_files)
        lines.append(f"| {topic} | {count} | {topic_imported} | {count - topic_imported} |")

    lines.extend(["", "## 明细", "", "| 文件 | 授权正文状态 |", "|---|---|"])
    for path in detail_files:
        lines.append(f"| {path.relative_to(BASE_DIR)} | {solution_status(path.read_text(encoding='utf-8'))} |")
    AUDIT_DIR.mkdir(parents=True, exist_ok=True)
    (AUDIT_DIR / "solution-coverage.md").write_text("\n".join(lines).rstrip() + "\n", encoding="utf-8")
    print(f"wrote solution coverage for {len(detail_files)} files ({imported_body_count} authorized bodies)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
