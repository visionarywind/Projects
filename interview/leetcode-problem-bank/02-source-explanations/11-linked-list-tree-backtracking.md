# 链表、树与回溯

- 来源 URL：https://leetcode.cn/circle/discuss/K0n2gO/
- 来源标题：分享丨【算法题单】链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）
- 抓取时间：2026-09-17 16:46:02 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 一、链表

> 注：由于周赛中的链表题可以转成数组处理，难度比直接处理链表低，故不标明难度分。
带着问题去做下面的题目：

### 一、链表 / §1.2 删除节点

[视频讲解【基础算法精讲 08】](https://www.bilibili.com/video/BV1VP4y1Q71e/)

### 一、链表 / §1.4 反转链表

[视频讲解【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)

### 一、链表 / §1.5 前后指针

[视频讲解【基础算法精讲 08】](https://www.bilibili.com/video/BV1VP4y1Q71e/)

### 一、链表 / §1.6 快慢指针

[视频讲解【基础算法精讲 07】](https://www.bilibili.com/video/BV1KG4y1G7cu/)

### 二、二叉树

**学习递归，从二叉树开始。**
晕递归的同学，请先看视频讲解[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)，欢迎点赞~
一般有三种遍历方式：
- **先序遍历**：见 §2.2 自顶向下 DFS。
- **中序遍历**：见 §2.9 二叉搜索树。
- **后序遍历**：见 §2.3 自底向上 DFS。
带着问题去做下面的题目：

### 二、二叉树 / §2.2 自顶向下 DFS（先序遍历）

在「递」的过程中维护值。
> 有些题目自顶向下和自底向上都可以做。有些题目也可以用 BFS 做。

### 二、二叉树 / §2.3 自底向上 DFS（后序遍历）

在「归」的过程中计算。
[如何灵活运用递归？【基础算法精讲 10】](https://www.bilibili.com/video/BV18M411z7bb/)

### 二、二叉树 / §2.6 二叉树的直径

[视频讲解【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)
另见本题单的「§3.5 树的直径」。

### 二、二叉树 / §2.8 最近公共祖先

[视频讲解【基础算法精讲 12】](https://www.bilibili.com/video/BV1W44y1Z7AR/)

### 二、二叉树 / §2.9 二叉搜索树

一般是中序遍历。
[视频讲解【基础算法精讲 11】](https://www.bilibili.com/video/BV14G411P7C1/)

### 二、二叉树 / §2.12 树形 DP

更多树形 DP，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 的「**十二、树形 DP**」。

### 二、二叉树 / §2.13 二叉树 BFS

[视频讲解【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)

### 三、一般树 / §3.5 树的直径

[视频讲解【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)

### 三、一般树 / §3.7 DFS 时间戳

可以把树上问题转化成数组问题，比如子树的操作转化成子数组的操作。通常要结合其他数据结构。

### 三、一般树 / §3.8 最近公共祖先（LCA）、倍增算法

带权树 LCA 模板（节点编号从 $0$ 开始）：
```py [sol-Python3]
class LcaBinaryLifting:
    def __init__(self, edges: List[List[int]]):
        n = len(edges) + 1
        m = n.bit_length()
        g = [[] for _ in range(n)]
        for x, y, w in edges:
            # 如果题目的节点编号从 1 开始，改成 x-1 和 y-1
            g[x].append((y, w))
            g[y].append((x, w))
        depth = [0] * n

### 三、一般树 / §3.13 其他

另见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 的「**十二、树形 DP**」，包含换根 DP。

### 四、回溯

本质是搜索树上的 DFS。
推荐先完成 **§2.7** 节。先理解二叉树上的回溯，再来学习一般情况下的回溯。

### 四、回溯 / §4.1 入门回溯

[视频讲解【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)

### 四、回溯 / §4.2 子集型回溯

[视频讲解【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)
有「**选或不选**」和「**枚举选哪个**」两种写法。
也可以用**二进制枚举**做。

### 四、回溯 / §4.3 划分型回溯

[视频讲解【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)
把分割线（逗号）看成是可以「选或不选」的东西，本质是子集型回溯。

### 四、回溯 / §4.4 组合型回溯

[视频讲解【基础算法精讲 15】](https://www.bilibili.com/video/BV1xG4y1F7nC/)
有个数上的约束。也算作子集型回溯。

### 四、回溯 / §4.5 排列型回溯

[视频讲解【基础算法精讲 16】](https://www.bilibili.com/video/BV1mY411D7f6/)
部分题目也可以用状压 DP 做。

### 四、回溯 / §4.8 折半搜索

英文名 meet in the middle。

### 算法题单

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 四、回溯 / §4.8 折半搜索 | 讲解 | https://leetcode.cn/problems/target-sum/solutions/2119041/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-s1cx/ | pending-fetch |

## 本地原创增强解析

链表、树与回溯 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
