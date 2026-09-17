# 3897. 连接二进制片段得到的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-value-of-concatenated-binary-segments/
- 题目 slug：`maximum-value-of-concatenated-binary-segments`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.7 交换论证法
- 难度分：1998
- 外部题解来源：https://leetcode.cn/problems/maximum-value-of-concatenated-binary-segments/solutions/3949827/zuo-fa-tong-179-ti-pythonjavacgo-by-endl-rd0f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[做法同 179 题（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-value-of-concatenated-binary-segments/solutions/3949827/zuo-fa-tong-179-ti-pythonjavacgo-by-endl-rd0f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zuo-fa-tong-179-ti-pythonjavacgo-by-endl-rd0f`
- topic id：`3949827`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

本题是 [179. 最大数](https://leetcode.cn/problems/largest-number/) 的二进制版本，做法是一样的。

暴力做法是，直接生成 $n$ 个长为 $\textit{nums}_1[i] + \textit{nums}_0[i]$ 的字符串，然后排序。

但由于这个字符串的特殊性，无需先生成字符串再比较。对于两个片段 $S$ 和 $T$，如果 $S$ 中的 $\texttt{1}$ 更多，那么 $S$ 在 $T$ 左边。如果 $S$ 和 $T$ 中的 $\texttt{1}$ 一样多，那么 $\texttt{0}$ 更少的在左边。

**特殊情况**：没有 $\texttt{0}$ 的片段（全为 $\texttt{1}$）排在最前面。

设当前答案为 $\textit{ans}$，拼接方法如下：

- 在 $\textit{ans}$ 右边拼接 $k$ 个 $0$，等价于把 $\textit{ans}$ 左移 $k$ 位，等价于 $\textit{ans}\cdot 2^k$。
- 在 $\textit{ans}$ 右边拼接 $k$ 个 $1$，先左移 $k$ 位（乘以 $2^k$），再加上 $2^k - 1$，即 $\textit{ans}\cdot 2^k + 2^k - 1 = (\textit{ans} + 1) \cdot 2^k - 1$。

代码实现时，注意取模。为什么可以在**中途取模**？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

本题由于 $k$ 比较小，可以预处理所有 $2^k$。如果本题值域更大，可以用**快速幂**计算 $2^k$，原理见[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

[本题视频讲解](https://www.bilibili.com/video/BV14hDQBDEUu/?t=8m18s)，欢迎点赞关注~

```py [sol-Python3]
MOD = 1_000_000_007
MX = 10001
# 预处理 2 的幂
pow2 = [1] * MX
for i in range(1, MX):
    pow2[i] = pow2[i - 1] * 2 % MOD

class Solution:
    def maxValue(self, nums1: List[int], nums0: List[int]) -> int:
        # 依次判断：没有 0 的排在最前面，1 多的排前面，0 少的排前面
        idx = sorted(range(len(nums1)), key=lambda i: (nums0[i] != 0, -nums1[i], nums0[i]))

        ans = 0
        for i in idx:
            ans = ((ans + 1) * pow2[nums1[i]] - 1) * pow2[nums0[i]] % MOD
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 10001;
    private static final int[] pow2 = new int[MX];
    private static boolean initialized = false;

    public Solution() {
        if (initialized) {
            return;
        }
        initialized = true;

        // 预处理 2 的幂
        pow2[0] = 1;
        for (int i = 1; i < MX; i++) {
            pow2[i] = pow2[i - 1] * 2 % MOD;
        }
    }

    public int maxValue(int[] nums1, int[] nums0) {
        Integer[] idx = new Integer[nums1.length];
        for (int i = 0; i < nums1.length; i++) {
            idx[i] = i;
        }

        Arrays.sort(idx, (i, j) -> {
            if (nums0[i] == 0 && nums0[j] != 0) {
                return -1;
            }
            if (nums0[i] != 0 && nums0[j] == 0) {
                return 1;
            }
            if (nums1[i] != nums1[j]) {
                return nums1[j] - nums1[i];
            }
            return nums0[i] - nums0[j];
        });

        long ans = 0;
        for (int i : idx) {
            ans = ((ans + 1) * pow2[nums1[i]] - 1) % MOD * pow2[nums0[i]] % MOD;
        }
        return (int) ans;
    }
}
```

```cpp [sol-C++]
constexpr int MOD = 1'000'000'007;
constexpr int MX = 10001;
int pow2[MX] = {1};

auto init = [] {
    // 预处理 2 的幂
    for (int i = 1; i < MX; i++) {
        pow2[i] = pow2[i - 1] * 2 % MOD;
    }
    return 0;
}();

class Solution {
public:
    int maxValue(vector<int>& nums1, vector<int>& nums0) {
        vector<int> idx(nums1.size());
        ranges::iota(idx, 0); // idx[i] = i

        // 依次判断：没有 0 的排在最前面，1 多的排前面，0 少的排前面
        ranges::sort(idx, {}, [&](int i) {
            return tuple(nums0[i] != 0, -nums1[i], nums0[i]);
        });

        long long ans = 0;
        for (int i : idx) {
            ans = ((ans + 1) * pow2[nums1[i]] - 1) % MOD * pow2[nums0[i]] % MOD;
        }
        return ans;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const mx = 10001
var pow2 = [mx]int{1}

func init() {
	// 预处理 2 的幂
	for i := 1; i < mx; i++ {
		pow2[i] = pow2[i-1] * 2 % mod
	}
}

func maxValue(nums1, nums0 []int) (ans int) {
	idx := make([]int, len(nums1))
	for i := range idx {
		idx[i] = i
	}
	slices.SortFunc(idx, func(i, j int) int {
		if nums0[i] == 0 {
			return -1
		}
		if nums0[j] == 0 {
			return 1
		}
		return cmp.Or(nums1[j]-nums1[i], nums0[i]-nums0[j])
	})

	for _, i := range idx {
		ans = ((ans+1)*pow2[nums1[i]] - 1) % mod * pow2[nums0[i]] % mod
	}
	return
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}_1$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.7 交换论证法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.7 交换论证法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
