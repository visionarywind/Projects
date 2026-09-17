# 批次日志

## preview：2026-09-16

- 范围：最初按 12 个来源专题每个前 3 题生成 preview；后续扩展为按来源分类出现记录生成 3459 个单题详解文件。
- 分类解析：已生成 12 个 `02-source-explanations/*.md` 文件。
- 授权导入实现：已确认 LeetCode GraphQL 的 `questionSolutionArticles` 可按题目检索文章，`solutionArticle` 可返回正文；成功记录才写为 `authorized-import`。
- 当前正文覆盖：1369 个 `authorized-import`、1126 个 `pending-fetch`、964 个 `missing-endlesscheng-solution`。
- 失败原因：批量请求过程中出现 LeetCode “请求过于频繁”以及网络读取超时；未成功取得正文的文件没有伪造正文内容。
- 本地原创：已生成详解结构、通用推导、C++17 代码区、复杂度、易错点和扩展解析；这些内容仍需逐题增强。
- C++ 状态：已对 3459 个文件完成代码块/`class Solution` 结构检查；多数题目仍含 TODO 骨架，尚未完成题面签名、样例和编译级验证。
- 后续：降低请求速率、增加持久缓存和断点续传后重试 `pending-fetch`；对 `missing-endlesscheng-solution` 单独核验题解列表、slug 映射或授权材料；再逐题补齐真实 C++17 和原创解析。

## retry：2026-09-17

- 范围：从未完成记录按偏移分批重试，低速请求间隔 2 秒，临时错误最多重试 1 次。
- 导入器修复：增加限流/临时网络错误分类、成功正文持久缓存、已有正文跳过、断点和指数退避参数。
- 本轮结果：审计正文总数由 1369 增至 2094，新增 725 个；执行了 421 次稳定无匹配检索（其中部分记录在本轮前已是缺失状态）；最终临时失败 0 个。
- 当前审计：2094 个 `authorized-import`、35 个 `pending-fetch`、1330 个 `missing-endlesscheng-solution`。
- 说明：`missing-endlesscheng-solution` 只代表完整检索未找到作者匹配；限流和超时不会写入该状态。
- 后续：对 1142 个未取得正文的记录核对题目别名、历史 slug、作者字段和来源文章索引；不可把当前未匹配视为作者没有发表题解。

## 第二轮重新检测：2026-09-17

- 范围：重新检索 `missing-endlesscheng-solution` 记录，低速请求间隔 2 秒，临时错误最多重试 2 次。
- 执行输出：回填 207 个题解文件；4 次超时重试后恢复，最终临时失败 0 个。
- 文件审计：3459 个详解文件中，2317 个 `authorized-import`，0 个 `pending-fetch`，1142 个 `missing-endlesscheng-solution`。同一题在多个来源分类出现，执行器的处理行数不等于文件数。
