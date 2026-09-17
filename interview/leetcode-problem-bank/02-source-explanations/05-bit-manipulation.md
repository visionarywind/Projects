# 位运算

- 来源 URL：https://leetcode.cn/circle/discuss/dHn9Vk/
- 来源标题：分享丨【算法题单】位运算（基础/性质/拆位/试填/恒等式/思维）
- 抓取时间：2026-09-17 16:46:02 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 位运算

推荐先阅读：[从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

### 一、基础题

**用位运算代替数组操作**：

### 二、异或（XOR）的性质

本质是模 $2$ 剩余系的加法。
另见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 中的「**§6.4 0-1 字典树（异或字典树）**」。

### 三、与或（AND/OR）的性质

$\text{AND}$ 的数越多，结果越小。
$\text{OR}$ 的数越多，结果越大。

### 三、与或（AND/OR）的性质 / AND/OR LogTrick

[LogTrick 入门教程](https://zhuanlan.zhihu.com/p/1933215367158830792)，包含原地写法，以及额外维护一个列表的写法。
如果你不熟悉原地去重算法，可以看 [26. 删除有序数组中的重复项](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/)，[我的题解](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/solutions/2807162/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-rvyk/)。
```py [sol-Python3]
# 对于每个右端点 i，计算所有子数组的或值，打印这些或值的分布范围（子数组左端点范围）
# 时间复杂度 O(nlogU)，其中 U = max(nums)
def logTrick(nums: List[int]) -> None:
    or_left = []  # (子数组或值，最小左端点)
    for i, x in enumerate(nums):
        # 计算以 i 为右端点的子数组或值
        for p in or_left:
            p[0] |= x  # **根据题目修改**
        # x 单独一个数作为子数组

### 七、线性基

模板（最大异或和）：
```py [sol-Python3]
class XorBasis:
    # n 为值域最大值 U 的二进制长度，例如 U=1e9 时 n=30
    def __init__(self, n: int):
        self.b = [0] * n
    def insert(self, x: int) -> None:
        b = self.b
        # 从高到低遍历，保证计算 max_xor 的时候，参与 XOR 的基的最高位（或者说二进制长度）是互不相同的
        for i in range(len(b) - 1, -1, -1):
            if x >> i:  # 由于大于 i 的位都被我们异或成了 0，所以 x >> i 的结果只能是 0 或 1
                if b[i] == 0:  # x 和之前的基是线性无关的

### 八、思维题

贪心、脑筋急转弯等。

### 算法题单

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 三、与或（AND/OR）的性质 / AND/OR LogTrick | 我的题解 | https://leetcode.cn/problems/remove-duplicates-from-sorted-array/solutions/2807162/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-rvyk/ | pending-fetch |
| 三、与或（AND/OR）的性质 / AND/OR LogTrick | 原理讲解（方法二） | https://leetcode.cn/problems/find-subarray-with-bitwise-or-closest-to-k/solutions/2798206/li-yong-and-de-xing-zhi-pythonjavacgo-by-gg4d/ | pending-fetch |

## 本地原创增强解析

位运算 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
