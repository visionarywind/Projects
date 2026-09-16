# 资料与证据

访问基线：2026-09-16 10:17:56 +0800

## 证据标签

- `primary-source`：来自 LeetCode 官方题目页的题号、难度、标签等；当前批次未批量抓取官方题目页。
- `external-index`：来自灵神公开题单页面的题目链接、标题、分类、顺序或难度分。
- `authorized-import`：按用户确认授权前提导入的灵茶山艾府分类解析或题解内容。
- `local-original`：本仓库生成的原创解析、C++17 代码、复杂度、易错点和扩展解析。
- `derived`：由 URL slug、标题层级或链接规范化推导出的字段。
- `manual-review`：后续人工校验后的字段。
- `unknown`：来源缺失、解析失败或尚未验证。

## 主要资料入口

| 类别 | 资料 | 用途 | 证据标签 |
|---|---|---|---|
| 总入口 | [如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd) | 发现专题题单、方法 A/B/C 的训练入口 | external-index |
| 灵神专题题单 | [滑动窗口与双指针](https://leetcode.cn/circle/discuss/0viNMK/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [二分算法](https://leetcode.cn/circle/discuss/SqopEo/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [单调栈](https://leetcode.cn/circle/discuss/9oZFK9/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [网格图](https://leetcode.cn/circle/discuss/YiXPXW/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [位运算](https://leetcode.cn/circle/discuss/dHn9Vk/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [图论算法](https://leetcode.cn/circle/discuss/01LUak/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [动态规划](https://leetcode.cn/circle/discuss/tXLS3i/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [常用数据结构](https://leetcode.cn/circle/discuss/mOr1u6/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [数学算法](https://leetcode.cn/circle/discuss/IYT3ss/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [贪心与思维](https://leetcode.cn/circle/discuss/g6KTKL/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [链表、树与回溯](https://leetcode.cn/circle/discuss/K0n2gO/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |
| 灵神专题题单 | [字符串](https://leetcode.cn/circle/discuss/SJFwQI/) | 题目链接、原始分类路径、出现顺序、难度分 | external-index |

## 内容边界

当前题库采用“双层内容模型”：题目索引字段来自公开题单，分类解析和题解正文按用户确认的授权导入前提保存为 `authorized-import`，本地补充的解题过程、C++17、复杂度、易错点和扩展解析标记为 `local-original`。当前按来源分类出现记录生成 3459 个单题详解文件，其中 1369 个已成功取得灵茶山艾府正文；1126 个仍为 `pending-fetch`，964 个标记为 `missing-endlesscheng-solution`。批量请求受站点频率限制和超时影响，未抓取内容不得写成已导入。
