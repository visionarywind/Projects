# LeetCode 算法刷题题库：灵神题单版

本题库基于灵神在 LeetCode 中文站公开分享的[《如何科学刷题？》](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd)及其链接的专题题单生成。索引沿用来源页面的专题与原始分类层级。

## 当前状态

- 来源专题：12 个。
- 单题详解：2317 个文件，全部为灵茶山艾府授权导入正文（`authorized-import`）。
- 清理说明（2026-10-14）：删除 1142 个仅有通用模板 + `// TODO` C++ 空壳、且未取得授权正文的文件；并从保留文件中移除本地通用模板段（题意重述/暴力思路/算法设计/扩展解析等空壳），只保留元信息与授权导入正文。未匹配授权正文的风险不再以空壳形式保留。
- 内容边界：本库正文为灵茶山艾府在 LeetCode 中文站公开分享的专题解析与题解导入（经用户确认授权），来源 URL、作者、导入时间记录在每个文件头部。本地原创实现层已移除，不再声称存在本地 C++ 实现。
- 详细的逐文件状态见[详解覆盖审计](05-solution-audit/solution-coverage.md)。

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

1. 初学阶段按[方法 A](03-practice/method-a-topic-training.md)和来源题单原始分类路径推进。
2. 查题使用[全量题目索引](01-question-bank/all-problems.md)，同一题多处出现时以第一次出现为主记录。
3. 复盘查看[覆盖矩阵](00-overview/coverage-matrix.md)和[质量审计](04-review/quality-audit.md)；随机训练和突击训练见 `03-practice/`。

## 维护方式

```bash
python3 interview/leetcode-problem-bank/scripts/import_authorized_solutions.py --import-problem-bodies --status missing-endlesscheng-solution --request-delay 2
python3 interview/leetcode-problem-bank/scripts/generate_solution_audit.py
python3 interview/leetcode-problem-bank/scripts/validate_leetcode_bank.py
python3 interview/leetcode-problem-bank/scripts/validate_cpp_solutions.py --batch full
git diff --check
```

如 LeetCode 接口结构变化，应先修正导入脚本；不得手工补造未取得的正文。
