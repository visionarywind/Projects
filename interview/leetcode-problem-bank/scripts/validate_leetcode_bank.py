#!/usr/bin/env python3
"""Offline validation for the generated LeetCode problem bank."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parents[1]
ALL_PROBLEMS = BASE_DIR / "01-question-bank" / "all-problems.md"
SOURCE_DIR = BASE_DIR / "01-question-bank" / "by-source-category"
DETAIL_DIR = BASE_DIR / "01-question-bank" / "detailed-solutions"
SOURCE_EXPLANATION_DIR = BASE_DIR / "02-source-explanations"
AUDIT_DIR = BASE_DIR / "05-solution-audit"
REQUIRED_FILES = [
    BASE_DIR / "README.md",
    BASE_DIR / "00-overview" / "coverage-matrix.md",
    BASE_DIR / "00-overview" / "generation-state.md",
    BASE_DIR / "00-overview" / "sources-and-evidence.md",
    ALL_PROBLEMS,
    BASE_DIR / "03-practice" / "method-a-topic-training.md",
    BASE_DIR / "03-practice" / "method-b-random-training.md",
    BASE_DIR / "03-practice" / "method-c-sprint-training.md",
    BASE_DIR / "04-review" / "quality-audit.md",
]

PROBLEM_URL_RE = re.compile(r"https://leetcode\.cn/problems/([A-Za-z0-9-]+)/")
ROW_RE = re.compile(r"^\|\s*\d+\s*\|\s*([^|]+)\|\s*\[([^\]]+)\]\((https://leetcode\.cn/problems/[^)]+)\)")


def fail(message: str, errors: list[str]) -> None:
    errors.append(message)


def validate_required_files(errors: list[str]) -> None:
    for path in REQUIRED_FILES:
        if not path.exists():
            fail(f"missing required file: {path.relative_to(BASE_DIR)}", errors)
    if not SOURCE_DIR.exists():
        fail("missing source category directory: 01-question-bank/by-source-category", errors)
    if not DETAIL_DIR.exists():
        fail("missing detailed solutions directory: 01-question-bank/detailed-solutions", errors)
    if not SOURCE_EXPLANATION_DIR.exists():
        fail("missing source explanations directory: 02-source-explanations", errors)
    for audit_name in ["solution-coverage.md", "copyright-and-authorization.md", "cpp-validation-state.md", "batch-log.md"]:
        if not (AUDIT_DIR / audit_name).exists():
            fail(f"missing solution audit file: 05-solution-audit/{audit_name}", errors)


def validate_all_problems(errors: list[str]) -> None:
    if not ALL_PROBLEMS.exists():
        return
    text = ALL_PROBLEMS.read_text(encoding="utf-8")
    slugs: dict[str, str] = {}
    rows = 0
    for line in text.splitlines():
        if not line.startswith("|") or "leetcode.cn/problems" not in line:
            continue
        rows += 1
        match = ROW_RE.match(line)
        if not match:
            fail(f"malformed problem row: {line[:160]}", errors)
            continue
        problem_id, title, url = [item.strip() for item in match.groups()]
        slug_match = PROBLEM_URL_RE.search(url)
        if not slug_match:
            fail(f"invalid problem URL: {url}", errors)
            continue
        slug = slug_match.group(1)
        if slug in slugs:
            fail(f"duplicate slug in all-problems.md: {slug}", errors)
        slugs[slug] = title
        if not title:
            fail(f"empty title for slug: {slug}", errors)
        if problem_id == "-":
            fail(f"missing problem id for slug: {slug}", errors)
        elif not re.match(r"^(\d+|LCP\s*\d+|LCR\s*\d+|剑指 Offer(?: II)?\s*[\w-]+|面试题\s*[\d.]+)$", problem_id):
            fail(f"unrecognized problem id `{problem_id}` for slug: {slug}", errors)
    if rows == 0:
        fail("all-problems.md contains no problem rows", errors)


def validate_category_pages(errors: list[str]) -> None:
    if not SOURCE_DIR.exists():
        return
    pages = sorted(SOURCE_DIR.glob("*.md"))
    if not pages:
        fail("no source category markdown files generated", errors)
        return
    for path in pages:
        text = path.read_text(encoding="utf-8")
        if "https://leetcode.cn/circle/discuss/" not in text:
            fail(f"category page missing source discuss URL: {path.relative_to(BASE_DIR)}", errors)
        if "https://leetcode.cn/problems/" not in text:
            fail(f"category page contains no problem links: {path.relative_to(BASE_DIR)}", errors)
        for url in re.findall(r"https://leetcode\.cn/problems/[^)\s]+", text):
            if not PROBLEM_URL_RE.match(url):
                fail(f"invalid problem URL in {path.name}: {url}", errors)


def validate_solution_enhancements(errors: list[str], require_bodies: bool = False) -> None:
    if not SOURCE_EXPLANATION_DIR.exists() or not DETAIL_DIR.exists():
        return
    explanation_pages = sorted(SOURCE_EXPLANATION_DIR.glob("*.md"))
    if len(explanation_pages) < 12:
        fail(f"expected at least 12 source explanation files, found {len(explanation_pages)}", errors)
    for path in explanation_pages:
        text = path.read_text(encoding="utf-8")
        for heading in ["## 授权导入：专题/分类解析", "## 本地原创增强解析"]:
            if heading not in text:
                fail(f"source explanation missing `{heading}`: {path.relative_to(BASE_DIR)}", errors)
    detail_pages = sorted(DETAIL_DIR.rglob("*.md"))
    if len(detail_pages) < 36:
        fail(f"expected at least 36 preview detailed solution files, found {len(detail_pages)}", errors)
    required_sections = [
        "## 元信息",
        "## 授权导入：灵茶山艾府题解过程",
        "## 本地原创解析",
        "### 7. C++17 实现",
        "### 10. 扩展解析",
    ]
    for path in detail_pages:
        text = path.read_text(encoding="utf-8")
        for section in required_sections:
            if section not in text:
                fail(f"detailed solution missing `{section}`: {path.relative_to(BASE_DIR)}", errors)
        if "```cpp" not in text:
            fail(f"detailed solution missing cpp block: {path.relative_to(BASE_DIR)}", errors)
        if require_bodies:
            marker = "## 授权导入：灵茶山艾府题解过程"
            body = text.split(marker, 1)[1].split("\n## ", 1)[0].strip() if marker in text else ""
            if not body or body.startswith("> 本节用于保存"):
                fail(f"detailed solution missing imported solution body: {path.relative_to(BASE_DIR)}", errors)
            if "外部题解授权状态：authorized-import" not in text:
                fail(f"detailed solution body status is not authorized-import: {path.relative_to(BASE_DIR)}", errors)


def validate_content_boundaries(errors: list[str]) -> None:
    suspicious_terms = ["题目描述", "示例 1", "示例1", "提示：", "解题思路"]
    for path in BASE_DIR.rglob("*.md"):
        text = path.read_text(encoding="utf-8")
        # Authorized imports are intentionally allowed to retain the source
        # article wording. Apply the copied-content heuristic only to local
        # metadata and locally generated analysis sections.
        text_for_scan = re.sub(
            r"\n## 授权导入：.*?(?=\n## 本地原创解析|\Z)",
            "\n",
            text,
            flags=re.S,
        )
        for term in suspicious_terms:
            if term in text_for_scan:
                fail(f"possible copied problem/solution content `{term}` in {path.relative_to(BASE_DIR)}", errors)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--require-authorized-bodies",
        action="store_true",
        help="fail unless every detailed solution contains a non-empty authorized body",
    )
    args = parser.parse_args()
    errors: list[str] = []
    validate_required_files(errors)
    validate_all_problems(errors)
    validate_category_pages(errors)
    validate_solution_enhancements(errors, require_bodies=args.require_authorized_bodies)
    validate_content_boundaries(errors)
    if errors:
        print("validation failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1
    print("validation passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
