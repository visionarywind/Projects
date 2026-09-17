# 3659. 数组元素分组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/partition-array-into-k-distinct-groups/
- 题目 slug：`partition-array-into-k-distinct-groups`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.8 相邻不同
- 难度分：1440
- 外部题解来源：https://leetcode.cn/problems/partition-array-into-k-distinct-groups/solutions/3762099/jie-lun-ti-pythonjavacgo-by-endlesscheng-dfzw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[结论题（Python/Java/C++/Go）](https://leetcode.cn/problems/partition-array-into-k-distinct-groups/solutions/3762099/jie-lun-ti-pythonjavacgo-by-endlesscheng-dfzw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jie-lun-ti-pythonjavacgo-by-endlesscheng-dfzw`
- topic id：`3762099`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

问题相当于每次从 $\textit{nums}$ 中删除恰好 $k$ 个不同元素，判断能否恰好把 $\textit{nums}$ 清空。

既然是 $k$ 个 $k$ 个地删，那么首先要满足 $\textit{nums}$ 的长度 $n$ 是 $k$ 的倍数。

设 $m$ 是出现次数最多的元素的出现次数。

对于 $k=2$ 的情况，我们有如下定理。

**定理**：如果 $m > n-m$，即出现次数最多的元素，比其余元素的个数还要多，那么无法满足题目要求，否则可以满足。

[证明+具体操作方案](https://zhuanlan.zhihu.com/p/1945782212176909162)

推广到一般情况，每次把 $m$ 减少 $1$，其余元素的个数就要减少 $k-1$，所以其余元素的个数必须至少为 $m\cdot(k-1)$，即

$$
m\cdot(k-1) \le n-m
$$

即

$$
m\cdot k \le n
$$

对于 C++ 和 Java，为防止乘法溢出，可以改为等价的

$$
m\le \left\lfloor\dfrac{n}{k}\right\rfloor
$$

具体请看 [视频讲解](https://www.bilibili.com/video/BV1X9eJz2EWE/?t=4m32s)，欢迎点赞关注~

## 哈希表写法

```py [sol-Python3]
class Solution:
    def partitionArray(self, nums: List[int], k: int) -> bool:
        n = len(nums)
        if n % k:
            return False
        mx = max(Counter(nums).values())
        return mx * k <= n
```

```java [sol-Java]
class Solution {
    public boolean partitionArray(int[] nums, int k) {
        int n = nums.length;
        if (n % k > 0) {
            return false;
        }
        Map<Integer, Integer> cnt = new HashMap<>();
        int mx = 0;
        for (int x : nums) {
            int c = cnt.merge(x, 1, Integer::sum); // c = ++cnt[x]
            mx = Math.max(mx, c);
        }
        return mx <= n / k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool partitionArray(vector<int>& nums, int k) {
        int n = nums.size();
        if (n % k) {
            return false;
        }
        unordered_map<int,int> cnt;
        int mx = 0;
        for (int x : nums) {
            mx = max(mx, ++cnt[x]);
        }
        return mx <= n / k;
    }
};
```

```go [sol-Go]
func partitionArray(nums []int, k int) bool {
	n := len(nums)
	if n%k > 0 {
		return false
	}
	cnt := map[int]int{}
	mx := 0
	for _, x := range nums {
		cnt[x]++
		mx = max(mx, cnt[x])
	}
	return mx <= n/k
}
```

## 数组写法

```py [sol-Python3]
class Solution:
    def partitionArray(self, nums: List[int], k: int) -> bool:
        n = len(nums)
        if n % k:
            return False
        cnt = [0] * (max(nums) + 1)
        for x in nums:
            cnt[x] += 1
        return max(cnt) * k <= n
```

```java [sol-Java]
class Solution {
    public boolean partitionArray(int[] nums, int k) {
        int n = nums.length;
        if (n % k > 0) {
            return false;
        }

        int u = 0;
        for (int x : nums) {
            u = Math.max(u, x);
        }

        int[] cnt = new int[u + 1];
        int mx = 0;
        for (int x : nums) {
            cnt[x]++;
            mx = Math.max(mx, cnt[x]);
        }
        return mx <= n / k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool partitionArray(vector<int>& nums, int k) {
        int n = nums.size();
        if (n % k) {
            return false;
        }
        vector<int> cnt(ranges::max(nums) + 1);
        int mx = 0;
        for (int x : nums) {
            mx = max(mx, ++cnt[x]);
        }
        return mx <= n / k;
    }
};
```

```go [sol-Go]
func partitionArray(nums []int, k int) bool {
	n := len(nums)
	if n%k > 0 {
		return false
	}
	cnt := make([]int, slices.Max(nums)+1)
	mx := 0
	for _, x := range nums {
		cnt[x]++
		mx = max(mx, cnt[x])
	}
	return mx <= n/k
}
```

## 优化

如果在遍历中发现 $\textit{cnt}[x] > \left\lfloor\dfrac{n}{k}\right\rfloor$，就立刻返回 $\texttt{false}$。

```py [sol-Python3]
class Solution:
    def partitionArray(self, nums: List[int], k: int) -> bool:
        n = len(nums)
        if n % k:
            return False
        cnt = [0] * (max(nums) + 1)
        n //= k  # 避免在循环中反复做除法
        for x in nums:
            cnt[x] += 1
            if cnt[x] > n:
                return False
        return True
```

```java [sol-Java]
class Solution {
    public boolean partitionArray(int[] nums, int k) {
        int n = nums.length;
        if (n % k > 0) {
            return false;
        }

        int u = 0;
        for (int x : nums) {
            u = Math.max(u, x);
        }

        int[] cnt = new int[u + 1];
        for (int x : nums) {
            cnt[x]++;
            if (cnt[x] > n / k) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool partitionArray(vector<int>& nums, int k) {
        int n = nums.size();
        if (n % k) {
            return false;
        }
        vector<int> cnt(ranges::max(nums) + 1);
        int mx = 0;
        for (int x : nums) {
            cnt[x]++;
            if (cnt[x] > n / k) {
                return false;
            }
        }
        return true;
    }
};
```

```go [sol-Go]
func partitionArray(nums []int, k int) bool {
	n := len(nums)
	if n%k > 0 {
		return false
	}
	cnt := make([]int, slices.Max(nums)+1)
	for _, x := range nums {
		cnt[x]++
		if cnt[x] > n/k {
			return false
		}
	}
	return true
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(n+U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(n+U)$。

## 专题训练

见下面贪心题单的「**§1.8 相邻不同**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.8 相邻不同`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.8 相邻不同`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
