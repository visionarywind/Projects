# 2091. 从数组中移除最大值和最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/removing-minimum-and-maximum-from-array/
- 题目 slug：`removing-minimum-and-maximum-from-array`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1384
- 外部题解来源：https://leetcode.cn/problems/removing-minimum-and-maximum-from-array/solutions/1126273/san-chong-tan-xin-ce-lue-qu-zui-xiao-zhi-fhnn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种情况（Python/Java/C++/Go）](https://leetcode.cn/problems/removing-minimum-and-maximum-from-array/solutions/1126273/san-chong-tan-xin-ce-lue-qu-zui-xiao-zhi-fhnn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-tan-xin-ce-lue-qu-zui-xiao-zhi-fhnn`
- topic id：`1126273`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

设 $\textit{nums}$ 的长度为 $n$，其最小值的下标为 $p$，最大值的下标为 $q$。假设 $p\le q$（若不满足则交换）。

根据题意，有三种删除策略：

- 删除包含 $p$ 和 $q$ 的前缀，长为 $q+1$。
- 删除包含 $p$ 和 $q$ 的后缀，长为 $n-p$。
- 删除包含 $p$ 的前缀，以及包含 $q$ 的后缀，长为 $(p+1)+(n-q)$。

取三者最小值，即为答案。

```py [sol-Python3]
class Solution:
    def minimumDeletions(self, nums: List[int]) -> int:
        n = len(nums)
        p = q = 0
        for i, x in enumerate(nums):
            if x < nums[p]:
                p = i
            elif x > nums[q]:
                q = i

        if p > q:
            p, q = q, p  # 保证 p <= q，方便下面计算
        return min(q + 1, n - p, p + 1 + n - q)
```

```py [sol-Python3 写法二]
class Solution:
    def minimumDeletions(self, nums: List[int]) -> int:
        n = len(nums)
        p = nums.index(min(nums))
        q = nums.index(max(nums))

        if p > q:
            p, q = q, p  # 保证 p <= q，方便下面计算
        return min(q + 1, n - p, p + 1 + n - q)
```

```java [sol-Java]
class Solution {
    public int minimumDeletions(int[] nums) {
        int n = nums.length;
        int p = 0;
        int q = 0;
        for (int i = 0; i < n; i++) {
            if (nums[i] < nums[p]) {
                p = i;
            } else if (nums[i] > nums[q]) {
                q = i;
            }
        }

        if (p > q) {
            // 保证 p <= q，方便下面计算
            int tmp = p;
            p = q;
            q = tmp;
        }
        return Math.min(Math.min(q + 1, n - p), p + 1 + n - q);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumDeletions(vector<int>& nums) {
        int n = nums.size();
        int p = 0, q = 0;
        for (int i = 0; i < n; i++) {
            if (nums[i] < nums[p]) {
                p = i;
            } else if (nums[i] > nums[q]) {
                q = i;
            }
        }

        if (p > q) {
            swap(p, q); // 保证 p <= q，方便下面计算
        }
        return min({q + 1, n - p, p + 1 + n - q});
    }
};
```

```go [sol-Go]
func minimumDeletions(nums []int) int {
	n := len(nums)
	p, q := 0, 0
	for i, x := range nums {
		if x < nums[p] {
			p = i
		} else if x > nums[q] {
			q = i
		}
	}

	if p > q {
		p, q = q, p // 保证 p <= q，方便下面计算
	}
	return min(q+1, n-p, p+1+n-q)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心与思维题单的「**§5.8 分类讨论**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
