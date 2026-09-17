# 2407. 最长递增子序列 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-increasing-subsequence-ii/
- 题目 slug：`longest-increasing-subsequence-ii`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：2280
- 外部题解来源：https://leetcode.cn/problems/longest-increasing-subsequence-ii/solutions/1816257/zhi-yu-xian-duan-shu-pythonjavacgo-by-en-p1gz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[线段树优化 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-increasing-subsequence-ii/solutions/1816257/zhi-yu-xian-duan-shu-pythonjavacgo-by-en-p1gz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-yu-xian-duan-shu-pythonjavacgo-by-en-p1gz`
- topic id：`1816257`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1it4y1L7kL) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

有关线段树的入门讲解可以看我的 [这个视频](https://www.bilibili.com/video/BV18t4y1p736?t=8m7s)。
 
---

#### 前言

在求解「上升子序列（IS）」问题时，一般有两种优化方法：

1. 维护固定长度的 IS 的末尾元素的最小值 + 二分优化；
2. 基于**值域**的线段树、平衡树等数据结构优化。

这两种做法都可以用 $O(n\log n)$ 的时间解决 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)。

---

对于本题，由于有一个差值不超过 $k$ 的约束，用线段树更好处理。

具体来说，定义 $f[i][j]$ 表示 $\textit{nums}$ 的前 $i$ 个元素中，以元素 $j$（注意不是 $\textit{nums}[j]$）结尾的满足题目两个条件的子序列的最长长度。

当 $j\ne\textit{nums}[i]$ 时，$f[i][j] = f[i-1][j]$。

当 $j=\textit{nums}[i]$ 时，我们可以从 $f[i-1][j']$ 转移过来，这里 $j-k\le j'<j$，取最大值，得

$$
f[i][j] = 1 + \max_{j'=j-k}^{j-1} f[i-1][j']
$$

上式有一个「区间求最大值」的过程，这非常适合用**线段树**计算，且由于 $f[i]$ 只会从 $f[i-1]$ 转移过来，我们可以把 $f$ 的第一个维度优化掉。这样我们可以**用线段树表示整个 $f$ 数组**，在上面查询和更新。

最后答案为 $\max(f[n-1])$，对应到线段树上就是根节点的值。

```py [sol1-Python3]
class Solution:
    def lengthOfLIS(self, nums: List[int], k: int) -> int:
        u = max(nums)
        mx = [0] * (4 * u)

        def modify(o: int, l: int, r: int, i: int, val: int) -> None:
            if l == r:
                mx[o] = val
                return
            m = (l + r) // 2
            if i <= m: modify(o * 2, l, m, i, val)
            else: modify(o * 2 + 1, m + 1, r, i, val)
            mx[o] = max(mx[o * 2], mx[o * 2 + 1])

        # 返回区间 [L,R] 内的最大值
        def query(o: int, l: int, r: int, L: int, R: int) -> int:  # L 和 R 在整个递归过程中均不变，将其大写，视作常量
            if L <= l and r <= R: return mx[o]
            res = 0
            m = (l + r) // 2
            if L <= m: res = query(o * 2, l, m, L, R)
            if R > m: res = max(res, query(o * 2 + 1, m + 1, r, L, R))
            return res

        for x in nums:
            if x == 1:
                modify(1, 1, u, 1, 1)
            else:
                res = 1 + query(1, 1, u, max(x - k, 1), x - 1)
                modify(1, 1, u, x, res)
        return mx[1]
```

```java [sol1-Java]
class Solution {
    int[] max;

    public int lengthOfLIS(int[] nums, int k) {
        var u = 0;
        for (var x : nums) u = Math.max(u, x);
        max = new int[u * 4];
        for (var x : nums) {
            if (x == 1) modify(1, 1, u, 1, 1);
            else {
                var res = 1 + query(1, 1, u, Math.max(x - k, 1), x - 1);
                modify(1, 1, u, x, res);
            }
        }
        return max[1];
    }

    private void modify(int o, int l, int r, int idx, int val) {
        if (l == r) {
            max[o] = val;
            return;
        }
        var m = (l + r) / 2;
        if (idx <= m) modify(o * 2, l, m, idx, val);
        else modify(o * 2 + 1, m + 1, r, idx, val);
        max[o] = Math.max(max[o * 2], max[o * 2 + 1]);
    }

    // 返回区间 [L,R] 内的最大值
    private int query(int o, int l, int r, int L, int R) { // L 和 R 在整个递归过程中均不变，将其大写，视作常量
        if (L <= l && r <= R) return max[o];
        var res = 0;
        var m = (l + r) / 2;
        if (L <= m) res = query(o * 2, l, m, L, R);
        if (R > m) res = Math.max(res, query(o * 2 + 1, m + 1, r, L, R));
        return res;
    }
}
```

```cpp [sol1-C++]
class Solution {
    vector<int> max;

    void modify(int o, int l, int r, int i, int val) {
        if (l == r) {
            max[o] = val;
            return;
        }
        int m = (l + r) / 2;
        if (i <= m) modify(o * 2, l, m, i, val);
        else modify(o * 2 + 1, m + 1, r, i, val);
        max[o] = std::max(max[o * 2], max[o * 2 + 1]);
    }

    // 返回区间 [L,R] 内的最大值
    int query(int o, int l, int r, int L, int R) { // L 和 R 在整个递归过程中均不变，将其大写，视作常量
        if (L <= l && r <= R) return max[o];
        int res = 0;
        int m = (l + r) / 2;
        if (L <= m) res = query(o * 2, l, m, L, R);
        if (R > m) res = std::max(res, query(o * 2 + 1, m + 1, r, L, R));
        return res;
    }

public:
    int lengthOfLIS(vector<int> &nums, int k) {
        int u = *max_element(nums.begin(), nums.end());
        max.resize(u * 4);
        for (int x: nums) {
            if (x == 1) modify(1, 1, u, 1, 1);
            else {
                int res = 1 + query(1, 1, u, std::max(x - k, 1), x - 1);
                modify(1, 1, u, x, res);
            }
        }
        return max[1];
    }
};
```

```go [sol1-Go]
type seg []struct{ l, r, max int }

func (t seg) build(o, l, r int) {
	t[o].l, t[o].r = l, r
	if l == r {
		return
	}
	m := (l + r) >> 1
	t.build(o<<1, l, m)
	t.build(o<<1|1, m+1, r)
}

func (t seg) modify(o, i, val int) {
	if t[o].l == t[o].r {
		t[o].max = val
		return
	}
	m := (t[o].l + t[o].r) >> 1
	if i <= m {
		t.modify(o<<1, i, val)
	} else {
		t.modify(o<<1|1, i, val)
	}
	t[o].max = max(t[o<<1].max, t[o<<1|1].max)
}

// 返回区间 [l,r] 内的最大值
func (t seg) query(o, l, r int) int {
	if l <= t[o].l && t[o].r <= r {
		return t[o].max
	}
	m := (t[o].l + t[o].r) >> 1
	if r <= m {
		return t.query(o<<1, l, r)
	}
	if m < l {
		return t.query(o<<1|1, l, r)
	}
	return max(t.query(o<<1, l, r), t.query(o<<1|1, l, r))
}

func lengthOfLIS(nums []int, k int) int {
	mx := 0
	for _, x := range nums {
		mx = max(mx, x)
	}
	t := make(seg, mx*4)
	t.build(1, 1, mx)
	for _, x := range nums {
		if x == 1 {
			t.modify(1, 1, 1)
		} else {
			t.modify(1, x, 1+t.query(1, max(x-k, 1), x-1))
		}
	}
	return t[1].max
}

func max(a, b int) int {
	if b > a {
		return b
	}
	return a
}
```

#### 复杂度分析

- 时间复杂度：$O(n\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$O(U)$。

## 本地原创解析

### 1. 题意重述

本题来自 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
