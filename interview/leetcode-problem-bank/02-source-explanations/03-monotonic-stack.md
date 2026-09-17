# 单调栈

- 来源 URL：https://leetcode.cn/circle/discuss/9oZFK9/
- 来源标题：分享｜【算法题单】单调栈（矩形面积/贡献法/最小字典序）
- 抓取时间：2026-09-17 13:14:52 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 单调栈

> 他向远方望去，无法看到高山背后的矮山，只看到一座座更高的山峰。
推荐先做做 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 中的「枚举右，维护左」以及第三章「栈」的基础题目后，再来刷本题单。

### 一、单调栈 / §1.1 基础

原理讲解：[单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)
```py [sol-Python3]
# 返回 left，其中 left[i] 是 nums[i] 左侧最近的严格大于 nums[i] 的数的下标，若不存在则为 -1
# 时间复杂度 O(len(nums))
def left_greater(nums: list[int]) -> list[int]:
    n = len(nums)
    left = [-1] * n
    for i, x in enumerate(nums):
        while st and nums[st[-1]] <= x:  # 如果求严格小于，改成 >=
            st.pop()
            left[i] = st[-1]
        st.append(i)

### 关联题单

- **单调栈优化 DP**：见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 的「§11.2 单调栈优化 DP」。
- **单调队列**：见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 的「§4.3 单调队列」。

### 算法题单

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 三、贡献法 | 2962. 统计最大元素出现至少 K 次的子数组·我的题解 | https://leetcode.cn/problems/count-subarrays-where-max-element-appears-at-least-k-times/solutions/2560940/hua-dong-chuang-kou-fu-ti-dan-pythonjava-xvwg/ | pending-fetch |

## 本地原创增强解析

单调栈 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
