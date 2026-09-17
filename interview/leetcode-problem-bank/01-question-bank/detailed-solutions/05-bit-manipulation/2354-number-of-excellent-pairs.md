# 2354. 优质数对的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-excellent-pairs/
- 题目 slug：`number-of-excellent-pairs`
- 来源专题：位运算
- 来源分类路径：六、恒等式
- 难度分：2076
- 外部题解来源：https://leetcode.cn/problems/number-of-excellent-pairs/solutions/1694032/deng-jie-zhuan-huan-pythonjavacgo-by-end-2qzs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[等价转换 + 后缀和优化（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-excellent-pairs/solutions/1694032/deng-jie-zhuan-huan-pythonjavacgo-by-end-2qzs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`deng-jie-zhuan-huan-pythonjavacgo-by-end-2qzs`
- topic id：`1694032`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV14a411U7QZ?t=11m) 已出炉，额外讲解了**如何用集合论来思考二进制**。欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

#### 提示 1

对于 $x|y$ 和 $x\&y$，在同一个比特位上，如果都有 $1$，那这个 $1$ 会被统计两次；如果一个为 $1$ 另一个为 $0$，那这个 $1$ 会被统计一次。

#### 提示 2

例如 $x=110$，$y=011$，只统计一次的部分为 $x'=100$，$y'=001$，统计了两次的部分为 $x\&y=010$。我们可以直接把 $010$ 重新分配到 $x'$ 和 $y'$ 上，这样又得到了 $x$ 和 $y$。

记 $c(x)$ 为 $x$ 的二进制表示中的 $1$ 的个数，则有如下等式：

$$
c(x|y)+c(x\&y)=c(x)+c(y)
$$

另外一种思路是把二进制数看成集合，根据容斥原理 $|A \cup B| = |A| + |B| - |A \cap B|$，得

$$
|A \cup B| + |A \cap B| = |A| + |B|
$$

再转换到二进制上，同样可以得到上面的等式。

#### 提示 3

遍历去重后的 $\textit{nums}$，统计 $c(\textit{nums}[i])$ 的个数，记录在 $\textit{cnt}$ 中，然后写一个二重循环遍历 $\textit{cnt}$，对于所有的 $c(x)+c(y)\ge k$，累加 $\textit{cnt}[c(x)]\cdot\textit{cnt}[c(y)]$，表示从这两组中各选一个 $x$ 和 $y$ 组成优质数对的个数（乘法原理）。

#### 复杂度分析

- 时间复杂度：$O(n+U^2)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U$ 为不同的 $c(\textit{nums}[i])$ 的个数，不超过 $30$。
- 空间复杂度：$O(n+U)$。

```py [sol1-Python3]
class Solution:
    def countExcellentPairs(self, nums: List[int], k: int) -> int:
        cnt = Counter(x.bit_count() for x in set(nums))
        ans = 0
        for cx, ccx in cnt.items():
            for cy, ccy in cnt.items():
                if cx + cy >= k:  # (x,y) 是优质数对
                    ans += ccx * ccy
        return ans
```

```java [sol1-Java]
class Solution {
    public long countExcellentPairs(int[] nums, int k) {
        var vis = new HashSet<Integer>();
        var cnt = new HashMap<Integer, Integer>();
        for (var x : nums) {
            if (vis.add(x)) {
                var c = Integer.bitCount(x);
                cnt.merge(c, 1, Integer::sum);
            }
        }
        var ans = 0L;
        for (var x : cnt.entrySet()) {
            for (var y : cnt.entrySet()) {
                if (x.getKey() + y.getKey() >= k) {
                    ans += (long) x.getValue() * y.getValue();
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long countExcellentPairs(vector<int>& nums, int k) {
        unordered_map<int, int> cnt;
        for (int x : unordered_set<int>(nums.begin(), nums.end())) { // 去重
            cnt[popcount((uint32_t) x)]++;
        }
        long long ans = 0;
        for (auto& [cx, ccx] : cnt) {
            for (auto& [cy, ccy] : cnt) {
                if (cx + cy >= k) { // (x,y) 是优质数对
                    ans += 1LL * ccx * ccy;
                }
            }
        }
        return ans;
    }
};
```

```go [sol1-Go]
func countExcellentPairs(nums []int, k int) (ans int64) {
	vis := map[int]bool{}
	cnt := map[int]int{}
	for _, x := range nums {
		if !vis[x] {
			vis[x] = true
			cnt[bits.OnesCount(uint(x))]++
		}
	}
	for cx, ccx := range cnt {
		for cy, ccy := range cnt {
			if cx+cy >= k { // (x,y) 是优质数对
				ans += int64(ccx) * int64(ccy)
			}
		}
	}
	return
}
```

进一步地，二重循环可以用前缀和（或者后缀和）来优化。

我们可以从小到大遍历 $\textit{cnt}[c(x)]$，由于 $c(y)\ge k-c(x)$，$c(y)$ 也会从大到小减小，我们可以用后缀和维护这些 $\textit{cnt}[c(y)]$ 的和。

#### 复杂度分析

- 时间复杂度：$O(n+U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=30$。
- 空间复杂度：$O(n+U)$。

```py [sol2-Python3]
class Solution:
    def countExcellentPairs(self, nums: List[int], k: int) -> int:
        cnt = [0] * 30
        for x in set(nums):
            cnt[x.bit_count()] += 1
        ans = 0
        s = sum(cnt[k:])
        for cx, ccx in enumerate(cnt):
            ans += ccx * s
            if 0 <= k - 1 - cx < 30:  # 下一轮循环 cx 是 cx+1
                s += cnt[k - 1 - cx]
        return ans
```

```java [sol2-Java]
class Solution {
    static final int U = 30;

    public long countExcellentPairs(int[] nums, int k) {
        var vis = new HashSet<Integer>();
        var cnt = new int[U];
        for (var x : nums) {
            if (vis.add(x)) {
                cnt[Integer.bitCount(x)]++;
            }
        }
        var ans = 0L;
        var s = 0;
        for (var i = k; i < U; i++) {
            s += cnt[i];
        }
        for (var cx = 0; cx < U; cx++) {
            ans += (long) cnt[cx] * s;
            var cy = k - 1 - cx; // 下一轮循环 cx 是 cx+1
            if (0 <= cy && cy < U) {
                s += cnt[cy];
            }
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
    static constexpr int U = 30;
public:
    long long countExcellentPairs(vector<int>& nums, int k) {
        int cnt[U]{};
        for (int x : unordered_set<int>(nums.begin(), nums.end())) { // 去重
            cnt[popcount((uint32_t) x)]++;
        }
        long long ans = 0;
        int s = 0;
        for (int i = k; i < U; i++) {
            s += cnt[i];
        }
        for (int cx = 0; cx < U; cx++) {
            ans += 1LL * cnt[cx] * s;
            int cy = k - 1 - cx; // 下一轮循环 cx 是 cx+1
            if (0 <= cy && cy < U) {
                s += cnt[cy];
            }
        }
        return ans;
    }
};
```

```go [sol2-Go]
func countExcellentPairs(nums []int, k int) (ans int64) {
	const U = 30
	vis := map[int]bool{}
	cnt := [U]int{}
	for _, x := range nums {
		if !vis[x] {
			vis[x] = true
			cnt[bits.OnesCount(uint(x))]++
		}
	}
	s := 0
	for i := k; i < U; i++ {
		s += cnt[i]
	}
	for cx, ccx := range cnt {
		ans += int64(ccx) * int64(s)
		cy := k - 1 - cx // 下一轮循环 cx 是 cx+1
		if 0 <= cy && cy < U {
			s += cnt[cy]
		}
	}
	return
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `六、恒等式`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、恒等式`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
