# 2333. 最小差值平方和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-sum-of-squared-difference/
- 题目 slug：`minimum-sum-of-squared-difference`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：2011
- 外部题解来源：https://leetcode.cn/problems/minimum-sum-of-squared-difference/solutions/1658341/by-endlesscheng-5qmk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[等价转换 + 贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-sum-of-squared-difference/solutions/1658341/by-endlesscheng-5qmk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-5qmk`
- topic id：`1658341`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

根据题意，在 $\textit{nums}_1[i]$ 上 $+1$，等价于在 $\textit{nums}_2[i]$ 上 $-1$，反之亦然。

定义 $a[i]=|\textit{nums}_1[i]-\textit{nums}_2[i]|$，$k=k_1+k_2$，则原问题可以转换成：

> 对数组 $a$ 执行至多 $k$ 次 $-1$ 操作，能得到的 $\sum a[i]^2$ 的最小值。

对于两个数，先把大的 $-1$ 会更优。我们可以将 $a$ 从大往小排序，然后从左到右遍历 $a$，同时更新剩余操作次数 $k$。

当遍历至 $a[i]$ 时，$a[0]$ 到 $a[i-1]$ 均已减小至 $a[i]$，我们需要判断 $k$ 次操作能否让 $a[0]$ 到 $a[i]$ 全部减小至 $a[i+1]$，即比较 $k$ 与所需次数 $c = (i + 1)  (a[i] - a[i+1])$ 的大小：

- 如果 $c<k$，那么从 $a[0]$ 到 $a[i]$ 均可以减小至 $a[i+1]$，更新 $k=k-c$。
- 如果 $c\ge k$，那么从 $a[0]$ 到 $a[i]$ 中：
    - 有 $k \bmod (i+1)$ 个元素可以额外减小 $\left\lfloor\dfrac{k}{i+1}\right\rfloor+1$；
    - 有 $i+1-k \bmod (i+1)$ 个元素可以额外减小 $\left\lfloor\dfrac{k}{i+1}\right\rfloor$。
    - 后续无法继续减小，应退出循环。

代码实现时，可以在 $a$ 末尾加一个哨兵 $0$，减少边界判断。

请看 [视频讲解](https://www.bilibili.com/video/BV1Le4y1R7xu) 第三题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minSumSquareDiff(self, a: List[int], nums2: List[int], k1: int, k2: int) -> int:
        ans, k = 0, k1 + k2
        for i in range(len(a)):
            a[i] = abs(a[i] - nums2[i])
            ans += a[i] * a[i]
        if sum(a) <= k:
            return 0  # 所有 a[i] 均可为 0

        a.sort(reverse=True)
        a.append(0)  # 哨兵
        for i, v in enumerate(a):
            ans -= v * v  # 撤销上面的 ans += a[i] * a[i]
            j = i + 1
            c = j * (v - a[j])
            if c < k:
                k -= c
                continue
            v -= k // j
            return ans + k % j * (v - 1) * (v - 1) + (j - k % j) * v * v
```

```java [sol-Java]
class Solution {
    public long minSumSquareDiff(int[] a, int[] nums2, int k1, int k2) {
        int n = a.length;
        int k = k1 + k2;
        long ans = 0;
        long sum = 0;
        for (int i = 0; i < n; i++) {
            a[i] = Math.abs(a[i] - nums2[i]);
            sum += a[i];
            ans += (long) a[i] * a[i];
        }
        if (sum <= k) {
            return 0; // 所有 a[i] 均可为 0
        }

        Arrays.sort(a);
        for (int i = n - 1; ; i--) {
            int m = n - i;
            long v = a[i];
            long c = m * (v - (i > 0 ? a[i - 1] : 0));
            ans -= v * v; // 撤销上面的 ans += a[i] * a[i]
            if (c < k) {
                k -= c;
                continue;
            }
            v -= k / m;
            return ans + k % m * (v - 1) * (v - 1) + (m - k % m) * v * v;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minSumSquareDiff(vector<int>& a, vector<int>& nums2, int k1, int k2) {
        int n = a.size(), k = k1 + k2;
        long long ans = 0, sum = 0;
        for (int i = 0; i < n; i++) {
            a[i] = abs(a[i] - nums2[i]);
            sum += a[i];
            ans += (long) a[i] * a[i];
        }
        if (sum <= k) { // 所有 a[i] 均可为 0
            return 0;
        }

        ranges::sort(a, greater());
        a.push_back(0); // 哨兵
        for (int i = 0; ; i++) {
            long long j = i + 1, v = a[i], c = j * (v - a[j]);
            ans -= v * v; // 撤销上面的 ans += a[i] * a[i]
            if (c < k) {
                k -= c;
                continue;
            }
            v -= k / j;
            return ans + k % j * (v - 1) * (v - 1) + (j - k % j) * v * v;
        }
    }
};
```

```go [sol-Go]
func minSumSquareDiff(a, nums2 []int, k1, k2 int) int64 {
	ans, sum := 0, 0
	for i, v := range a {
		a[i] = abs(v - nums2[i])
		sum += a[i]
		ans += a[i] * a[i]
	}
	k := k1 + k2
	if sum <= k {
		return 0 // 所有 a[i] 均可为 0
	}

	slices.SortFunc(a, func(a, b int) int { return b - a })
	a = append(a, 0) // 哨兵
	for i, v := range a {
		i++
		ans -= v * v // 撤销上面的 ans += a[i] * a[i]
		if c := i * (v - a[i]); c < k {
			k -= c
			continue
		}
		v -= k / i
		ans += k%i*(v-1)*(v-1) + (i-k%i)*v*v
		break
	}
	return int64(ans)
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销和哨兵的开销。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.1 从最小/最大开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.1 从最小/最大开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
