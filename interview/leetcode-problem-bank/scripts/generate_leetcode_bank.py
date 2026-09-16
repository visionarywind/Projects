#!/usr/bin/env python3
"""Generate a local LeetCode problem bank from EndlessCheng's public lists.

The generator intentionally stores only problem metadata: title, URL, source
category path, order and optional rating. It does not copy problem statements,
solutions or long prose from the source pages.
"""

from __future__ import annotations

import html
import json
import re
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from urllib.error import URLError
from urllib.parse import parse_qs, unquote, urljoin, urlparse, urlunparse
from urllib.request import Request, urlopen

SOURCE_URL = "https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd"
BASE_DIR = Path(__file__).resolve().parents[1]
USER_AGENT = "Mozilla/5.0 (compatible; local-leetcode-bank-generator/1.0)"

METHOD_LABELS = {
    "0viNMK": "滑动窗口与双指针",
    "SqopEo": "二分算法",
    "9oZFK9": "单调栈",
    "YiXPXW": "网格图",
    "dHn9Vk": "位运算",
    "01LUak": "图论算法",
    "tXLS3i": "动态规划",
    "mOr1u6": "常用数据结构",
    "IYT3ss": "数学算法",
    "g6KTKL": "贪心与思维",
    "K0n2gO": "链表、树与回溯",
    "SJFwQI": "字符串",
}


@dataclass
class Problem:
    slug: str
    title: str
    url: str
    leetcode_id: str = ""
    rating: str = ""
    source_category: str = ""
    source_url: str = ""
    source_title: str = ""
    category_path: str = ""
    order: int = 0
    appearances: list[dict[str, Any]] = field(default_factory=list)


def fetch(url: str) -> str:
    request = Request(url, headers={"User-Agent": USER_AGENT})
    with urlopen(request, timeout=45) as response:
        return response.read().decode("utf-8", "ignore")


def extract_next_data(page: str) -> dict[str, Any]:
    match = re.search(r'<script id="__NEXT_DATA__" type="application/json">(.*?)</script>', page, re.S)
    if not match:
        raise ValueError("missing __NEXT_DATA__ script")
    return json.loads(html.unescape(match.group(1)))


def extract_question(data: dict[str, Any]) -> dict[str, Any]:
    queries = data.get("props", {}).get("pageProps", {}).get("dehydratedState", {}).get("queries", [])
    for query in queries:
        question = query.get("state", {}).get("data", {}).get("qaQuestion")
        if isinstance(question, dict) and question.get("content"):
            return question
    raise ValueError("missing qaQuestion content")


def strip_markdown(text: str) -> str:
    text = re.sub(r"!\[[^\]]*\]\([^)]*\)(?:\{[^}]*\})?", "", text)
    text = re.sub(r"\[([^\]]+)\]\([^)]*\)", r"\1", text)
    text = re.sub(r"[*_`$]", "", text)
    text = re.sub(r"<[^>]+>", "", text)
    return html.unescape(text).strip()


def normalize_link(url: str, base_url: str) -> str:
    url = html.unescape(url.strip())
    joined = urljoin(base_url, url)
    parsed = urlparse(joined)
    if parsed.path == "/link/":
        target = parse_qs(parsed.query).get("target", [""])[0]
        if target:
            joined = unquote(target)
            parsed = urlparse(joined)
    path = parsed.path
    if path.startswith("/problems/"):
        parts = [part for part in path.split("/") if part]
        if len(parts) >= 2:
            path = f"/problems/{parts[1]}/"
    query = "" if path.startswith("/problems/") else parsed.query
    return urlunparse((parsed.scheme or "https", parsed.netloc or "leetcode.cn", path, "", query, ""))


def slug_from_url(url: str) -> str:
    parsed = urlparse(url)
    parts = [part for part in parsed.path.split("/") if part]
    if len(parts) >= 2 and parts[0] == "problems":
        return parts[1]
    return ""


def slugify(text: str, fallback: str) -> str:
    text = re.sub(r"[（(].*?[）)]", "", text)
    mapping = {
        "滑动窗口与双指针": "sliding-window-two-pointers",
        "二分算法": "binary-search",
        "单调栈": "monotonic-stack",
        "网格图": "grid-graph",
        "位运算": "bit-manipulation",
        "图论算法": "graph-algorithms",
        "动态规划": "dynamic-programming",
        "常用数据结构": "data-structures",
        "数学算法": "math-algorithms",
        "贪心与思维": "greedy-thinking",
        "链表、树与回溯": "linked-list-tree-backtracking",
        "字符串": "string-algorithms",
    }
    if text in mapping:
        return mapping[text]
    ascii_text = re.sub(r"[^a-zA-Z0-9]+", "-", text).strip("-").lower()
    return ascii_text or fallback


def markdown_links(markdown: str) -> list[tuple[str, str]]:
    return re.findall(r"\[([^\]]+)\]\(([^)]+)\)", markdown)


def discover_category_pages(main_content: str) -> list[dict[str, str]]:
    pages: list[dict[str, str]] = []
    seen: set[str] = set()
    in_complete_list = False
    for raw_line in main_content.splitlines():
        line = raw_line.strip()
        if line.startswith("#### 完整题单"):
            in_complete_list = True
            continue
        if in_complete_list and line.startswith("## "):
            break
        if not in_complete_list:
            continue
        match = re.match(r"\d+\.\s+\[([^\]]+)\]\(([^)]+)\)", line)
        if not match:
            continue
        title = strip_markdown(match.group(1))
        url = normalize_link(match.group(2), SOURCE_URL)
        if url in seen:
            continue
        seen.add(url)
        short_id = [part for part in urlparse(url).path.split("/") if part][-1]
        pages.append(
            {
                "title": title,
                "short_title": METHOD_LABELS.get(short_id, re.sub(r"[（(].*", "", title).strip()),
                "url": url,
                "source_id": short_id,
            }
        )
    return pages


def parse_problem_link(text: str, url: str, tail: str) -> tuple[str, str, str]:
    clean_text = strip_markdown(text)
    patterns = [
        r"^(\d+)\.\s*(.+)$",
        r"^((?:LCP|LCR)\s*\d+)\.\s*(.+)$",
        r"^(剑指 Offer(?: II)?\s*[\w-]+)\.\s*(.+)$",
        r"^(面试题\s*[\d.]+)\.\s*(.+)$",
    ]
    leetcode_id = ""
    title = clean_text
    for pattern in patterns:
        match = re.match(pattern, clean_text)
        if match:
            leetcode_id = match.group(1).strip()
            title = match.group(2).strip()
            break
    rating_match = re.search(r"(?:^|\s)(\d{3,4})(?:\s|$)", tail)
    rating = rating_match.group(1) if rating_match else ""
    return leetcode_id, title, rating


def extract_tail_after_link(line: str, link_text: str, link_url: str) -> str:
    token = f"]({link_url})"
    idx = line.find(token)
    if idx == -1:
        return ""
    return strip_markdown(line[idx + len(token):])


def parse_category_page(page: dict[str, str], content: str, start_order: int) -> tuple[list[Problem], dict[str, Any]]:
    headings: dict[int, str] = {}
    problems: list[Problem] = []
    skipped_solution_links = 0
    source_order = start_order
    for raw_line in content.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        heading = re.match(r"^(#{2,4})\s+(.+)$", line)
        if heading:
            level = len(heading.group(1))
            headings[level] = strip_markdown(heading.group(2))
            for old_level in list(headings):
                if old_level > level:
                    del headings[old_level]
            continue
        for link_text, link_url in markdown_links(line):
            normalized = normalize_link(link_url, page["url"])
            slug = slug_from_url(normalized)
            if not slug:
                continue
            if "/solutions/" in normalized:
                skipped_solution_links += 1
                continue
            tail = extract_tail_after_link(line, link_text, link_url)
            leetcode_id, title, rating = parse_problem_link(link_text, normalized, tail)
            if not leetcode_id or not title:
                continue
            source_order += 1
            category_path = " / ".join(headings[level] for level in sorted(headings) if headings[level])
            if not category_path:
                category_path = page["short_title"]
            problems.append(
                Problem(
                    slug=slug,
                    title=title,
                    url=normalized,
                    leetcode_id=leetcode_id,
                    rating=rating,
                    source_category=page["short_title"],
                    source_url=page["url"],
                    source_title=page["title"],
                    category_path=category_path,
                    order=source_order,
                    appearances=[
                        {
                            "source_category": page["short_title"],
                            "category_path": category_path,
                            "order": source_order,
                            "rating": rating,
                        }
                    ],
                )
            )
    return problems, {"skipped_solution_links": skipped_solution_links, "last_order": source_order}


def merge_problems(items: list[Problem]) -> tuple[list[Problem], list[str]]:
    merged: dict[str, Problem] = {}
    conflicts: list[str] = []
    for item in items:
        key = item.leetcode_id or item.slug or item.url
        if key not in merged:
            merged[key] = item
            continue
        existing = merged[key]
        existing.appearances.extend(item.appearances)
        if item.source_category not in existing.source_category.split("；"):
            existing.source_category += f"；{item.source_category}"
        if item.title != existing.title:
            conflicts.append(f"{key}: `{existing.title}` vs `{item.title}`")
        if item.slug != existing.slug:
            conflicts.append(f"{key}: `{existing.slug}` vs `{item.slug}`")
    return sorted(merged.values(), key=lambda p: p.order), conflicts


def md_escape(text: str) -> str:
    return text.replace("|", "\\|").replace("\n", " ")


def problem_link(problem: Problem) -> str:
    title = md_escape(problem.title)
    prefix = f"{problem.leetcode_id}. " if problem.leetcode_id else ""
    return f"[{prefix}{title}]({problem.url})"


def write(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content.rstrip() + "\n", encoding="utf-8")


def render_readme(total: int, category_count: int, fetched_at: str) -> str:
    return f"""# LeetCode 算法刷题题库：灵神题单版

本题库基于灵神在 LeetCode 中文站公开分享的《如何科学刷题？》及其链接到的专题题单生成，目标是把外部题单整理为本仓库可检索、可审计、可复用的本地刷题索引。

## 当前状态

- 来源入口：[{SOURCE_URL}]({SOURCE_URL})
- 生成时间：{fetched_at}
- 来源专题：{category_count} 个
- 去重题目：{total} 题
- 内容边界：仅保存题号、标题、链接、来源分类、顺序、难度分和短状态；不保存 LeetCode 题面全文，不保存题解全文，不复制灵神文章大段原文。
- 证据状态：题目链接、标题、分类和顺序来自外部公开题单，记为 `[external-index]`；难度分来自题单右侧数字，未重新验证时仍视为外部索引字段。

## 导航

- [全量题目索引](01-question-bank/all-problems.md)
- [按来源分类组织的题库](01-question-bank/by-source-category/)
- [覆盖矩阵](00-overview/coverage-matrix.md)
- [生成状态](00-overview/generation-state.md)
- [资料与证据](00-overview/sources-and-evidence.md)
- [方法 A：专题训练](03-practice/method-a-topic-training.md)
- [方法 B：随机训练](03-practice/method-b-random-training.md)
- [方法 C：突击训练](03-practice/method-c-sprint-training.md)
- [质量审计](04-review/quality-audit.md)

## 使用方式

1. 初学阶段先看 [方法 A：专题训练](03-practice/method-a-topic-training.md)，按来源题单的原始分类路径推进。
2. 查题时使用 [全量题目索引](01-question-bank/all-problems.md)，同一题在多个专题出现时以第一次出现为主记录。
3. 复盘时查看 [覆盖矩阵](00-overview/coverage-matrix.md) 和 [质量审计](04-review/quality-audit.md)，关注重复题、无题号题、无难度分题和未在线验证字段。
4. 随机训练和面试前突击分别参考 `03-practice/` 下的方法 B/C 页面。

## 维护方式

```bash
python interview/leetcode-problem-bank/scripts/generate_leetcode_bank.py
python interview/leetcode-problem-bank/scripts/validate_leetcode_bank.py
git diff --check
```

如果 LeetCode 页面结构变化导致生成结果异常，应先查看 `00-overview/generation-state.md` 和 `04-review/quality-audit.md`，不要手工补造未验证题目。
"""


def render_all_problems(problems: list[Problem], fetched_at: str) -> str:
    rows = []
    for idx, p in enumerate(problems, 1):
        appearances = len(p.appearances)
        rows.append(
            f"| {idx} | {p.leetcode_id or '-'} | {problem_link(p)} | {p.rating or 'Unknown'} | "
            f"{md_escape(p.source_category)} | {md_escape(p.category_path)} | {appearances} | external-index |"
        )
    return "\n".join(
        [
            "# 全量题目索引",
            "",
            "<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->",
            "",
            f"生成时间：{fetched_at}",
            "",
            "本页按题目去重；同一题在多个来源分类中出现时，`出现次数` 会大于 1。难度分来自来源题单右侧数字，未重新访问 LeetCode 官方题目页验证。",
            "",
            "| 序号 | 题号 | 题目 | 难度分 | 来源题单 | 来源分类路径 | 出现次数 | 证据状态 |",
            "|---:|---:|---|---:|---|---|---:|---|",
            *rows,
        ]
    )


def render_category_page(page: dict[str, str], page_problems: list[Problem], fetched_at: str) -> str:
    by_path: dict[str, list[Problem]] = defaultdict(list)
    for p in page_problems:
        by_path[p.category_path].append(p)
    lines = [
        f"# {page['short_title']}",
        "",
        "<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->",
        "",
        f"来源：[{md_escape(page['title'])}]({page['url']})",
        f"生成时间：{fetched_at}",
        "",
        "本页按来源题单中的标题层级组织，只保存题目元数据和链接，不复制题面或题解。",
    ]
    for path in sorted(by_path, key=lambda p: min(item.order for item in by_path[p])):
        lines.extend(["", f"## {path}", "", "| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |", "|---:|---:|---|---:|---|"])
        for p in sorted(by_path[path], key=lambda item: item.order):
            lines.append(f"| {p.order} | {p.leetcode_id or '-'} | {problem_link(p)} | {p.rating or 'Unknown'} | external-index |")
    return "\n".join(lines)


def render_coverage(problems: list[Problem], pages: list[dict[str, str]], fetched_at: str) -> str:
    by_source = Counter()
    rating_known = Counter()
    for p in problems:
        for appearance in p.appearances:
            by_source[appearance["source_category"]] += 1
        rating_known["有难度分" if p.rating else "无难度分"] += 1
    rows = []
    for page in pages:
        rows.append(f"| {page['short_title']} | {by_source[page['short_title']]} | {page['url']} | external-index |")
    return "\n".join(
        [
            "# 覆盖矩阵",
            "",
            "<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->",
            "",
            f"生成时间：{fetched_at}",
            "",
            "## 来源题单覆盖",
            "",
            "| 来源题单 | 题目出现次数 | 来源链接 | 证据状态 |",
            "|---|---:|---|---|",
            *rows,
            "",
            "## 字段覆盖",
            "",
            "| 字段状态 | 数量 | 说明 |",
            "|---|---:|---|",
            f"| 去重题目 | {len(problems)} | 以题号优先、slug 次之去重 |",
            f"| 有难度分 | {rating_known['有难度分']} | 难度分来自来源题单，未在线复核 |",
            f"| 无难度分 | {rating_known['无难度分']} | 来源题单未给出或解析未识别 |",
            f"| 多处出现 | {sum(1 for p in problems if len(p.appearances) > 1)} | 同题被多个分类引用 |",
        ]
    )


def render_generation_state(
    fetched_at: str,
    pages: list[dict[str, str]],
    raw_count: int,
    problems: list[Problem],
    conflicts: list[str],
    errors: list[str],
) -> str:
    page_rows = [f"| {idx} | {page['short_title']} | {page['url']} |" for idx, page in enumerate(pages, 1)]
    conflict_rows = [f"- {item}" for item in conflicts] or ["- 暂未发现题号/slug 冲突。"]
    error_rows = [f"- {item}" for item in errors] or ["- 暂未发现抓取或解析错误。"]
    return "\n".join(
        [
            "# 生成状态",
            "",
            "状态格式版本：1",
            "主题：LeetCode 算法刷题题库：灵神题单版",
            f"生成时间：{fetched_at}",
            f"来源入口：{SOURCE_URL}",
            "交付模式：从公开题单抽取题目元数据，生成本地 Markdown 索引；不保存题面和题解。",
            "",
            "## 来源题单",
            "",
            "| 顺序 | 来源题单 | URL |",
            "|---:|---|---|",
            *page_rows,
            "",
            "## 统计",
            "",
            f"- 来源专题数：{len(pages)}",
            f"- 原始题目出现次数：{raw_count}",
            f"- 去重题目数：{len(problems)}",
            f"- 多处出现题目数：{sum(1 for p in problems if len(p.appearances) > 1)}",
            f"- 无难度分题目数：{sum(1 for p in problems if not p.rating)}",
            "",
            "## 冲突记录",
            "",
            *conflict_rows,
            "",
            "## 异常记录",
            "",
            *error_rows,
            "",
            "## 后续工作",
            "",
            "- 如需官方难度、标签或题面状态，应单独访问 LeetCode 官方题目页并标注 `primary-source`，当前批次未执行该在线复核。",
            "- 如来源页面结构变化，应先修正解析脚本，再重新生成文档。",
        ]
    )


def render_sources(fetched_at: str, pages: list[dict[str, str]]) -> str:
    rows = [f"| 灵神专题题单 | [{md_escape(page['short_title'])}]({page['url']}) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |" for page in pages]
    return "\n".join(
        [
            "# 资料与证据",
            "",
            f"访问基线：{fetched_at}",
            "",
            "## 证据标签",
            "",
            "- `primary-source`：来自 LeetCode 官方题目页的题号、难度、标签等；当前批次未批量抓取官方题目页。",
            "- `external-index`：来自灵神公开题单页面的题目链接、标题、分类、顺序或难度分。",
            "- `derived`：由 URL slug、标题层级或链接规范化推导出的字段。",
            "- `manual-review`：后续人工校验后的字段。",
            "- `unknown`：来源缺失、解析失败或尚未验证。",
            "",
            "## 主要资料入口",
            "",
            "| 类别 | 资料 | 用途 | 证据标签 |",
            "|---|---|---|---|",
            f"| 总入口 | [如何科学刷题？]({SOURCE_URL}) | 发现专题题单、方法 A/B/C 的训练入口 | external-index |",
            *rows,
            "",
            "## 内容边界",
            "",
            "本题库只保存题目元数据：题号、标题、链接、来源题单、分类路径、顺序、难度分和短状态。文档不保存 LeetCode 题面全文、不保存题解全文，也不复制灵神文章大段原文。方法论页面只保留转写后的短摘要和来源链接。",
        ]
    )


def render_practice(method: str, fetched_at: str) -> str:
    if method == "a":
        title = "方法 A：专题训练"
        body = """专题训练是第一版题库的主路径。建议按来源题单的原始分类推进：先完成基础分类和较低难度分题目，再回到同一分类补更高难度题。题目入口见 [按来源分类组织的题库](../01-question-bank/by-source-category/)。

执行建议：

1. 先从滑动窗口、二分、常用数据结构、链表/树/回溯、网格图和动态规划入门分类开始。
2. 每个分类先完成难度分较低的题；遇到题解仍难以理解的题，记录后跳过，不阻塞整体进度。
3. 同一题若出现在多个分类，优先按第一次出现的分类完成，复盘时再回到其他分类理解它为什么被重复引用。
4. 每轮结束后查看覆盖矩阵，确认哪些来源分类仍为空白或缺少复盘。"""
    elif method == "b":
        title = "方法 B：随机训练"
        body = """随机训练用于检验自己在未知题型下识别算法的能力。它不替代专题训练，更适合作为阶段性复盘。

执行建议：

1. 完成一轮专题训练后，再开始随机训练。
2. 随机做题时先隐藏或忽略标签，独立判断题型。
3. 做完后回到本题库的来源分类，标记自己误判的知识点。
4. 对误判频繁的分类，回到对应来源题单补基础题。"""
    else:
        title = "方法 C：突击训练"
        body = """突击训练面向时间有限的面试前准备。当前题库仍以灵神专题题单为主，HOT 100 和面试 150 仅作为外部补充入口，不在本批次展开复制。

执行建议：

1. 时间紧迫时优先刷经典高频题单，并把不会的题映射回本题库中的专题分类。
2. 若 HOT 100/面试 150 与本题库重复，以 LeetCode 题号为准合并复盘记录。
3. 不建议在突击阶段展开大量新专题；优先补滑动窗口、二分、树、图、动态规划和常用数据结构中的薄弱点。"""
    return "\n".join(
        [
            f"# {title}",
            "",
            f"生成时间：{fetched_at}",
            "",
            "本页是对来源文章训练方法的本地化摘要，不复制原文。来源见 [资料与证据](../00-overview/sources-and-evidence.md)。",
            "",
            body,
        ]
    )


def render_quality(problems: list[Problem], conflicts: list[str], errors: list[str], fetched_at: str) -> str:
    missing_id = [p for p in problems if not p.leetcode_id]
    missing_rating = [p for p in problems if not p.rating]
    multi = [p for p in problems if len(p.appearances) > 1]
    return "\n".join(
        [
            "# 质量审计",
            "",
            f"生成时间：{fetched_at}",
            "",
            "## 静态检查摘要",
            "",
            f"- 去重题目数：{len(problems)}",
            f"- 无题号题目数：{len(missing_id)}",
            f"- 无难度分题目数：{len(missing_rating)}",
            f"- 多处出现题目数：{len(multi)}",
            f"- 题号/slug 冲突数：{len(conflicts)}",
            f"- 抓取或解析异常数：{len(errors)}",
            "",
            "## 版权与引用边界",
            "",
            "- [x] 未保存 LeetCode 题面全文。",
            "- [x] 未保存题解全文。",
            "- [x] 未复制灵神文章大段原文。",
            "- [x] 每个来源题单保留 URL。",
            "- [x] 题目表仅包含元数据和链接。",
            "",
            "## 待验证项",
            "",
            "- [ ] 未批量访问 LeetCode 官方题目页核对官方难度和标签。",
            "- [ ] 未进行在线链接可用性校验；当前只做 URL 形式和来源解析校验。",
            "- [ ] 未人工确认每个题目的分类是否仍与来源页面最新版本一致。",
        ]
    )


def generate() -> int:
    fetched_at = datetime.now(timezone.utc).astimezone().strftime("%Y-%m-%d %H:%M:%S %z")
    errors: list[str] = []
    main_html = fetch(SOURCE_URL)
    main_question = extract_question(extract_next_data(main_html))
    main_content = main_question["content"]
    pages = discover_category_pages(main_content)
    if not pages:
        raise RuntimeError("no category pages discovered from source post")

    all_items: list[Problem] = []
    page_items: dict[str, list[Problem]] = {}
    order = 0
    for page in pages:
        try:
            page_html = fetch(page["url"])
            question = extract_question(extract_next_data(page_html))
            content = question["content"]
            if question.get("title"):
                page["title"] = question["title"]
            items, meta = parse_category_page(page, content, order)
            order = meta["last_order"]
            page_items[page["source_id"]] = items
            all_items.extend(items)
            if not items:
                errors.append(f"{page['short_title']}: 未解析到题目链接")
        except (URLError, TimeoutError, ValueError, RuntimeError) as exc:
            errors.append(f"{page['short_title']}: {exc}")
            page_items[page["source_id"]] = []

    problems, conflicts = merge_problems(all_items)

    write(BASE_DIR / "README.md", render_readme(len(problems), len(pages), fetched_at))
    write(BASE_DIR / "01-question-bank" / "all-problems.md", render_all_problems(problems, fetched_at))
    for page in pages:
        filename = f"{pages.index(page)+1:02d}-{slugify(page['short_title'], page['source_id'])}.md"
        write(
            BASE_DIR / "01-question-bank" / "by-source-category" / filename,
            render_category_page(page, page_items.get(page["source_id"], []), fetched_at),
        )
    write(BASE_DIR / "00-overview" / "coverage-matrix.md", render_coverage(problems, pages, fetched_at))
    write(BASE_DIR / "00-overview" / "generation-state.md", render_generation_state(fetched_at, pages, len(all_items), problems, conflicts, errors))
    write(BASE_DIR / "00-overview" / "sources-and-evidence.md", render_sources(fetched_at, pages))
    write(BASE_DIR / "03-practice" / "method-a-topic-training.md", render_practice("a", fetched_at))
    write(BASE_DIR / "03-practice" / "method-b-random-training.md", render_practice("b", fetched_at))
    write(BASE_DIR / "03-practice" / "method-c-sprint-training.md", render_practice("c", fetched_at))
    write(BASE_DIR / "04-review" / "quality-audit.md", render_quality(problems, conflicts, errors, fetched_at))
    print(f"generated {len(problems)} unique problems from {len(pages)} source categories")
    if errors:
        print(f"warnings: {len(errors)}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(generate())
