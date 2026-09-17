# 授权与版权边界

## 当前授权前提

用户已在本会话中选择“授权内容导入”。本题库增强按用户确认拥有授权或可提供可复制内容的前提执行。

## 内容分层

| 层级 | 内容 | 状态标记 | 说明 |
|---|---|---|---|
| 授权导入层 | 灵茶山艾府专题解析、分类说明、题解过程 | `authorized-by-user-confirmation`、`authorized-import`、`pending-fetch`、`missing-endlesscheng-solution` | 成功正文必须记录来源 URL、作者、导入时间和授权状态；失败记录不得伪造正文 |
| 本地原创层 | 本仓库生成的题意重述、推导、C++17、复杂度、易错点、扩展解析 | `local-original`、`draft-generated` | 与授权导入内容分区保存；当前多数仍为通用模板 |
| 验证层 | C++ 编译、样例、链接可用性、人工复核 | `not-run`、`compiled`、`sample-tested` | 未执行时不得写成已验证 |

## 当前导入边界

- 已导入 12 个来源专题的分类解析文件。
- 已按来源分类出现记录生成 3459 个单题详解文件。
- 当前已成功导入 2094 个灵茶山艾府题解正文，状态为 `authorized-import`。
- 另有 35 个仍为 `pending-fetch`，1330 个经当前完整检索标记为 `missing-endlesscheng-solution`；重试批次使用低速请求和持久缓存，临时失败不会写入缺失状态。
- 正文接口使用 LeetCode GraphQL `questionSolutionArticles` 搜索文章，再用 `solutionArticle` 取得正文；未取得正文的记录保留可重试或缺失状态。
- 本地原创解析为生成草稿，后续需要按题面和授权题解逐题补强。

## 后续要求

1. 若补充完整题解原文，必须在单题文件中记录题解 URL、作者、导入时间和授权状态。
2. 授权导入内容不得与本地原创解析混写。
3. C++ 代码未运行时保持 `[待验证]` 或 `not-run`。
4. 若某题没有可授权题解正文，应保持 `pending-fetch` 或 `missing-endlesscheng-solution`，不能伪造导入内容。
5. 重新批量导入时应增加请求间隔、持久缓存和断点续传，避免覆盖已经成功导入的正文。
