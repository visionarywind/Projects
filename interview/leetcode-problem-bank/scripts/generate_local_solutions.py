#!/usr/bin/env python3
"""Generate local detailed solution files with C++17 skeletons.

The generator separates authorized external content from local-original analysis.
Use ``--batch preview`` for the first N problems from each source topic, or
``--batch full`` to create a detailed solution file for every source-category
problem occurrence.
"""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from urllib.parse import urlparse

BASE_DIR = Path(__file__).resolve().parents[1]
SOURCE_CATEGORY_DIR = BASE_DIR / "01-question-bank" / "by-source-category"
DETAIL_DIR = BASE_DIR / "01-question-bank" / "detailed-solutions"


@dataclass
class Problem:
    order: int
    problem_id: str
    title: str
    url: str
    rating: str
    category_path: str
    source_file: Path
    source_title: str


def slug_from_url(url: str) -> str:
    parts = [part for part in urlparse(url).path.split("/") if part]
    if len(parts) >= 2 and parts[0] == "problems":
        return parts[1]
    return re.sub(r"[^a-zA-Z0-9]+", "-", url).strip("-")


def parse_source_file(path: Path) -> list[Problem]:
    text = path.read_text(encoding="utf-8")
    source_title = text.splitlines()[0].lstrip("# ").strip()
    category = ""
    problems: list[Problem] = []
    row_re = re.compile(
        r"^\|\s*(\d+)\s*\|\s*([^|]+?)\s*\|\s*\[([^\]]+)\]\((https://leetcode\.cn/problems/[^)]+)\)\s*\|\s*([^|]+?)\s*\|"
    )
    for line in text.splitlines():
        if line.startswith("## "):
            category = line[3:].strip()
            continue
        match = row_re.match(line)
        if not match:
            continue
        order, problem_id, linked_title, url, rating = [item.strip() for item in match.groups()]
        title = re.sub(r"^(.+?)\.\s*", "", linked_title)
        problems.append(
            Problem(
                order=int(order),
                problem_id=problem_id,
                title=title,
                url=url,
                rating=rating,
                category_path=category,
                source_file=path,
                source_title=source_title,
            )
        )
    return problems


def cpp_method_hint(title: str, category_path: str) -> str:
    text = f"{title} {category_path}"
    if "元音" in text or "子串" in text:
        return """class Solution {
public:
    int maxVowels(string s, int k) {
        auto isVowel = [](char c) {
            return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
        };
        int cur = 0, ans = 0;
        for (int i = 0; i < (int)s.size(); ++i) {
            cur += isVowel(s[i]);
            if (i >= k) cur -= isVowel(s[i - k]);
            if (i + 1 >= k) ans = max(ans, cur);
        }
        return ans;
    }
};"""
    if "平均" in text:
        return """class Solution {
public:
    double findMaxAverage(vector<int>& nums, int k) {
        long long cur = 0;
        for (int i = 0; i < k; ++i) cur += nums[i];
        long long best = cur;
        for (int i = k; i < (int)nums.size(); ++i) {
            cur += nums[i] - nums[i - k];
            best = max(best, cur);
        }
        return (double)best / k;
    }
};"""
    return """class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};"""


def render_solution(problem: Problem, fetched_at: str) -> str:
    slug = slug_from_url(problem.url)
    cpp = cpp_method_hint(problem.title, problem.category_path)
    return f"""# {problem.problem_id}. {problem.title}

## 元信息

- LeetCode 链接：{problem.url}
- 题目 slug：`{slug}`
- 来源专题：{problem.source_title}
- 来源分类路径：{problem.category_path}
- 难度分：{problem.rating}
- 外部题解来源：待从题目页题解列表解析灵茶山艾府题解；若找到则由 `import_authorized_solutions.py --import-problem-bodies` 回填。
- 外部题解授权状态：pending-fetch
- 本地解析状态：draft-generated
- C++ 验证状态：not-run
- 生成时间：{fetched_at}

## 授权导入：灵茶山艾府题解过程

> 本节用于保存用户确认授权导入的灵茶山艾府题解原文。当前状态为 `pending-fetch`；执行正文导入脚本后，本节会替换为题解标题、来源 URL、作者、导入时间和完整题解正文。

## 本地原创解析

### 1. 题意重述

本题来自 `{problem.category_path}`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

{cpp}
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `{problem.category_path}`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
"""


def write(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content.rstrip() + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--batch", default="preview", choices=["preview", "full"])
    parser.add_argument("--per-topic", type=int, default=3, help="preview mode: number of problems per topic")
    args = parser.parse_args()
    fetched_at = datetime.now(timezone.utc).astimezone().strftime("%Y-%m-%d %H:%M:%S %z")
    total = 0
    seen_paths: set[Path] = set()
    for path in sorted(SOURCE_CATEGORY_DIR.glob("*.md")):
        problems = parse_source_file(path)
        if args.batch == "preview":
            problems = problems[: args.per_topic]
        topic_dir = path.stem
        for problem in problems:
            slug = slug_from_url(problem.url)
            filename = f"{problem.problem_id}-{slug}.md".replace("/", "-").replace(" ", "-")
            out_path = DETAIL_DIR / topic_dir / filename
            if out_path in seen_paths:
                continue
            write(out_path, render_solution(problem, fetched_at))
            seen_paths.add(out_path)
            total += 1
    print(f"generated {total} {args.batch} detailed solution files")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
