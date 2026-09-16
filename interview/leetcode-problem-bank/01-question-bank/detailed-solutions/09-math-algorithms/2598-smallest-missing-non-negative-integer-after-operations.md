# 2598. 执行操作后的最大 MEX

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-missing-non-negative-integer-after-operations/
- 题目 slug：`smallest-missing-non-negative-integer-after-operations`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.9 同余
- 难度分：1846
- 外部题解来源：https://leetcode.cn/problems/smallest-missing-non-negative-integer-after-operations/solutions/2177789/tong-yu-pythonjavacgo-by-endlesscheng-qoan/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[同余分组（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/smallest-missing-non-negative-integer-after-operations/solutions/2177789/tong-yu-pythonjavacgo-by-endlesscheng-qoan/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-yu-pythonjavacgo-by-endlesscheng-qoan`
- topic id：`2177789`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

下文记 $m=\textit{value}$。

由于同一个数可以加减任意倍的 $m$，我们可以先把每个 $\textit{nums}[i]$ 变成与 $\textit{nums}[i]$ 关于模 $m$ **同余**的最小非负整数，以备后用。关于同余的介绍，请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

本题有负数，根据这篇文章中的公式，我们可以把每个 $\textit{nums}[i]$ 变成

$$
(\textit{nums}[i]\bmod m + m)\bmod m
$$

从而保证取模结果在 $[0,m)$ 中。

例如 $\textit{nums}=[1,-6,-4,3,5]$，$m=3$，取模后变成 $[1,0,2,0,2]$。

然后枚举答案：

- 有没有与 $0$ 关于模 $m$ 同余的数？有，我们消耗掉一个 $0$。
- 有没有与 $1$ 关于模 $m$ 同余的数？有，我们消耗掉一个 $1$。
- 有没有与 $2$ 关于模 $m$ 同余的数？有，我们消耗掉一个 $2$。
- 有没有与 $3$ 关于模 $m$ 同余的数？有，我们消耗掉一个 $0$。这个取模后等于 $0$ 的数，可以继续操作，变成 $3$。
- 有没有与 $4$ 关于模 $m$ 同余的数？也就是看是否还有 $1$，没有，那么答案等于 $4$。

怎么知道还有没有剩余元素？用一个哈希表 $\textit{cnt}$ 统计 $(\textit{nums}[i]\bmod m + m) \bmod m$ 的个数。

[本题视频讲解](https://www.bilibili.com/video/BV1EL411C7YU/?t=42m20s)，欢迎点赞关注~

## 写法一

```py [sol-Python3]
class Solution:
    def findSmallestInteger(self, nums: List[int], m: int) -> int:
        cnt = Counter(x % m for x in nums)
        mex = 0
        while cnt[mex % m]:
            cnt[mex % m] -= 1
            mex += 1
        return mex
```

```java [sol-Java]
class Solution {
    public int findSmallestInteger(int[] nums, int m) {
        int[] cnt = new int[m];
        for (int x : nums) {
            cnt[(x % m + m) % m]++; // 保证取模结果在 [0, m) 中
        }

        int mex = 0;
        while (cnt[mex % m]-- > 0) {
            mex++;
        }
        return mex;
    }
}
```

```java [sol-Java 哈希表]
class Solution {
    public int findSmallestInteger(int[] nums, int m) {
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            cnt.merge((x % m + m) % m, 1, Integer::sum);
        }

        int mex = 0;
        while (cnt.merge(mex % m, -1, Integer::sum) >= 0) {
            mex++;
        }
        return mex;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findSmallestInteger(vector<int>& nums, int m) {
        unordered_map<int, int> cnt;
        for (int x : nums) {
            cnt[(x % m + m) % m]++; // 保证取模结果在 [0, m) 中
        }

        int mex = 0;
        while (cnt[mex % m]-- > 0) {
            mex++;
        }
        return mex;
    }
};
```

```c [sol-C]
int findSmallestInteger(int* nums, int numsSize, int m) {
    int* cnt = calloc(m, sizeof(int));
    for (int i = 0; i < numsSize; i++) {
        cnt[(nums[i] % m + m) % m]++; // 保证取模结果在 [0, m) 中
    }

    int mex = 0;
    while (cnt[mex % m]-- > 0) {
        mex++;
    }

    free(cnt);
    return mex;
}
```

```go [sol-Go]
func findSmallestInteger(nums []int, m int) (mex int) {
	cnt := map[int]int{}
	for _, x := range nums {
		cnt[(x%m+m)%m]++ // 保证取模结果在 [0, m) 中
	}

	for cnt[mex%m] > 0 {
		cnt[mex%m]--
		mex++
	}
	return
}
```

```js [sol-JavaScript]
var findSmallestInteger = function(nums, m) {
    const cnt = new Map();
    for (const x of nums) {
        const v = (x % m + m) % m; // 保证取模结果在 [0, m) 中
        cnt.set(v, (cnt.get(v) ?? 0) + 1);
    }

    let mex = 0;
    while ((cnt.get(mex % m) ?? 0) > 0) {
        cnt.set(mex % m, cnt.get(mex % m) - 1);
        mex++;
    }
    return mex;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn find_smallest_integer(nums: Vec<i32>, m: i32) -> i32 {
        let mut cnt = HashMap::new();
        for x in nums {
            // 保证取模结果在 [0, m) 中
            *cnt.entry((x % m + m) % m).or_insert(0) += 1;
        }

        for mex in 0.. {
            if let Some(c) = cnt.get_mut(&(mex % m)) {
                if *c > 0 {
                    *c -= 1;
                    continue;
                }
            }
            return mex;
        }
        unreachable!()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。由于加多少个数，就只能减多少个数，所以第二个循环至多循环 $\mathcal{O}(n)$ 次。
- 空间复杂度：$\mathcal{O}(\min(n,m))$。哈希表中至多有 $\mathcal{O}(\min(n,m))$ 个元素。

## 写法二

![lc2598-c.png](https://pic.leetcode.cn/1759992283-huZjMi-lc2598-c.png){:width=500px}

此外，把哈希表换成数组更快。

```py [sol-Python3]
class Solution:
    def findSmallestInteger(self, nums: List[int], m: int) -> int:
        cnt = [0] * m
        for x in nums:
            cnt[x % m] += 1

        i = cnt.index(min(cnt))
        return m * cnt[i] + i
```

```java [sol-Java]
class Solution {
    public int findSmallestInteger(int[] nums, int m) {
        int[] cnt = new int[m];
        for (int x : nums) {
            cnt[(x % m + m) % m]++;
        }

        int i = 0;
        for (int j = 1; j < m; j++) {
            if (cnt[j] < cnt[i]) {
                i = j;
            }
        }

        return m * cnt[i] + i;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findSmallestInteger(vector<int>& nums, int m) {
        vector<int> cnt(m);
        for (int x : nums) {
            cnt[(x % m + m) % m]++;
        }

        int i = ranges::min_element(cnt) - cnt.begin();
        return m * cnt[i] + i;
    }
};
```

```c [sol-C]
int findSmallestInteger(int* nums, int numsSize, int m) {
    int* cnt = calloc(m, sizeof(int));
    for (int i = 0; i < numsSize; i++) {
        cnt[(nums[i] % m + m) % m]++;
    }

    int i = 0;
    for (int j = 1; j < m; j++) {
        if (cnt[j] < cnt[i]) {
            i = j;
        }
    }

    int ans = m * cnt[i] + i;
    free(cnt);
    return ans;
}
```

```go [sol-Go]
func findSmallestInteger(nums []int, m int) int {
	cnt := make([]int, m)
	for _, x := range nums {
		cnt[(x%m+m)%m]++
	}

	i := 0
	for j := 1; j < m; j++ {
		if cnt[j] < cnt[i] {
			i = j
		}
	}

	return m*cnt[i] + i
}
```

```js [sol-JavaScript]
var findSmallestInteger = function(nums, m) {
    const cnt = Array(m).fill(0);
    for (const x of nums) {
        cnt[(x % m + m) % m]++;
    }

    let i = 0;
    for (let j = 1; j < m; j++) {
        if (cnt[j] < cnt[i]) {
            i = j;
        }
    }

    return m * cnt[i] + i;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_smallest_integer(nums: Vec<i32>, m: i32) -> i32 {
        let mut cnt = vec![0; m as usize];
        for x in nums {
            cnt[((x % m + m) % m) as usize] += 1;
        }

        let mut i = 0;
        for j in 1..m as usize {
            if cnt[j] < cnt[i] {
                i = j;
            }
        }

        m * cnt[i] + i as i32
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 相似题目

见下面数学题单的「**§1.9 同余**」。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.9 同余`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.9 同余`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
