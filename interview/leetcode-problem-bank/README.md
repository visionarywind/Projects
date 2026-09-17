# LeetCode 算法刷题题库：灵神题单版

本题库基于灵神在 LeetCode 中文站公开分享的《如何科学刷题？》及其链接到的专题题单生成，目标是把外部题单整理为本仓库可检索、可审计、可复用的本地刷题索引。

## 当前状态

- 来源入口：[https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd)
- 生成时间：2026-09-16 10:17:56 +0800
- 来源专题：12 个
- 去重题目：2817 题
- 内容边界：当前采用“双层内容模型”：授权导入层保存用户确认可导入的灵茶山艾府分类解析/题解内容，本地原创层保存本仓库生成的题意重述、推导、C++17、复杂度、易错点和扩展解析。
# LeetCode 算法刷题题库：灵神题单版

本题库基于灵神在 LeetCode 中文站公开分享的《如何科学刷题？》及其链接到的专题题单生成，目标是把外部题单整理为本仓库可检索、可审计、可复用的本地刷题索引。

## 当前状态

- 来源入口：[https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd)
- 生成时间：2026-09-16 10:17:56 +0800
- 来源专题：12 个
- 去重题目：2817 题
- 内容边界：当前采用“双层内容模型”：授权导入层保存用户确认可导入的灵茶山艾府分类解析/题解内容，本地原创层保存本仓库生成的题意重述、推导、C++17、复杂度、易错点和扩展解析。
- 详解状态：已按 12 个来源专题生成 3459 个按分类出现记录展开的单题详解文件；截至 2026-09-17，2110 个已导入灵茶山艾府正文，剩余 1349 个经完整检索暂未找到对应文章。正文导入使用低速请求、持久缓存和断点重试，临时失败不会标记为无题解。
- 证据状态：题目链接、标题、分类和顺序来自外部公开题单，记为 `[external-index]`；灵茶山艾府正文仅在成功取得 `solutionArticle.content` 且记录来源 URL、作者和导入时间时记为 `[authorized-import]`；本地解析和代码仍需逐题验证。

## 导航

- [全量题目索引](01-question-bank/all-problems.md)
- [按来源分类组织的题库](01-question-bank/by-source-category/)
- [单题详解](01-question-bank/detailed-solutions/)
- [授权分类解析](02-source-explanations/)
- [详解覆盖审计](05-solution-audit/solution-coverage.md)
- [授权与版权边界](05-solution-audit/copyright-and-authorization.md)
- [C++ 验证状态](05-solution-audit/cpp-validation-state.md)
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
python interview/leetcode-problem-bank/scripts/import_authorized_solutions.py --batch preview
python interview/leetcode-problem-bank/scripts/generate_local_solutions.py --batch preview --per-topic 3
python interview/leetcode-problem-bank/scripts/generate_solution_audit.py
python interview/leetcode-problem-bank/scripts/validate_leetcode_bank.py
python interview/leetcode-problem-bank/scripts/validate_cpp_solutions.py --batch preview
git diff --check
```

如果 LeetCode 页面结构变化导致生成结果异常，应先查看 `00-overview/generation-state.md` 和 `04-review/quality-audit.md`，不要手工补造未验证题目。

- 证据状态：题目链接、标题、分类和顺序来自外部公开题单，记为 `[external-index]`；灵茶山艾府正文仅在成功取得 `solutionArticle.content` 且记录来源 URL、作者和导入时间时记为 `[authorized-import]`；本地解析和代码仍需逐题验证。

## 导航

- [全量题目索引](01-question-bank/all-problems.md)
- [按来源分类组织的题库](01-question-bank/by-source-category/)
- [单题详解](01-question-bank/detailed-solutions/)
- [授权分类解析](02-source-explanations/)
- [详解覆盖审计](05-solution-audit/solution-coverage.md)
- [授权与版权边界](05-solution-audit/copyright-and-authorization.md)
- [C++ 验证状态](05-solution-audit/cpp-validation-state.md)
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
python interview/leetcode-problem-bank/scripts/import_authorized_solutions.py --batch preview
python interview/leetcode-problem-bank/scripts/generate_local_solutions.py --batch preview --per-topic 3
python interview/leetcode-problem-bank/scripts/generate_solution_audit.py
python interview/leetcode-problem-bank/scripts/validate_leetcode_bank.py
python interview/leetcode-problem-bank/scripts/validate_cpp_solutions.py --batch preview
git diff --check
```

如果 LeetCode 页面结构变化导致生成结果异常，应先查看 `00-overview/generation-state.md` 和 `04-review/quality-audit.md`，不要手工补造未验证题目。
