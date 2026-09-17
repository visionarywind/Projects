# 1695. 删除子数组的最大得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-erasure-value/
- 题目 slug：`maximum-erasure-value`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础
- 难度分：1529
- 外部题解来源：https://leetcode.cn/problems/maximum-erasure-value/solutions/3720052/hua-dong-chuang-kou-bu-er-shu-zu-ha-xi-j-rsvy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[滑动窗口 + 数组 / 哈希集合（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-erasure-value/solutions/3720052/hua-dong-chuang-kou-bu-er-shu-zu-ha-xi-j-rsvy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-dong-chuang-kou-bu-er-shu-zu-ha-xi-j-rsvy`
- topic id：`3720052`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

本题和 [3. 无重复字符的最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/) 是一样的，额外维护窗口中的元素和即可。见 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

本题可以用布尔数组实现（更快），也可以用哈希集合实现（更通用，适用于 $\textit{nums}[i]\le 10^9$ 这种值域更大的场景）。

**数组写法**

```py [sol-Python3]
class Solution:
    def maximumUniqueSubarray(self, nums: List[int]) -> int:
        has = [False] * (max(nums) + 1)
        ans = s = left = 0
        for x in nums:
            while has[x]:
                has[nums[left]] = False
                s -= nums[left]
                left += 1
            has[x] = True
            s += x
            ans = max(ans, s)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumUniqueSubarray(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        boolean[] has = new boolean[mx + 1];
        int ans = 0, s = 0, left = 0;
        for (int x : nums) {
            while (has[x]) {
                has[nums[left]] = false;
                s -= nums[left];
                left++;
            }
            has[x] = true;
            s += x;
            ans = Math.max(ans, s);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumUniqueSubarray(vector<int>& nums) {
        int mx = ranges::max(nums);
        vector<int8_t> has(mx + 1);
        int ans = 0, s = 0, left = 0;
        for (int x : nums) {
            while (has[x]) {
                has[nums[left]] = false;
                s -= nums[left];
                left++;
            }
            has[x] = true;
            s += x;
            ans = max(ans, s);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumUniqueSubarray(nums []int) (ans int) {
	mx := slices.Max(nums)
	has := make([]bool, mx+1)
	s, left := 0, 0
	for _, x := range nums {
		for has[x] {
			has[nums[left]] = false
			s -= nums[left]
			left++
		}
		has[x] = true
		s += x
		ans = max(ans, s)
	}
	return
}
```

```js [sol-JavaScript]
var maximumUniqueSubarray = function(nums) {
    const mx = Math.max(...nums);
    const has = Array(mx + 1).fill(false);
    let ans = 0, s = 0, left = 0;
    for (const x of nums) {
        while (has[x]) {
            has[nums[left]] = false;
            s -= nums[left];
            left++;
        }
        has[x] = true;
        s += x;
        ans = Math.max(ans, s);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_unique_subarray(nums: Vec<i32>) -> i32 {
        let mx = *nums.iter().max().unwrap();
        let mut has = vec![false; (mx + 1) as usize];
        let mut ans = 0;
        let mut s = 0;
        let mut left = 0;
        for &x in &nums {
            while has[x as usize] {
                has[nums[left] as usize] = false;
                s -= nums[left];
                left += 1;
            }
            has[x as usize] = true;
            s += x;
            ans = ans.max(s);
        }
        ans
    }
}
```

**哈希集合写法**

```py [sol-Python3]
class Solution:
    def maximumUniqueSubarray(self, nums: List[int]) -> int:
        st = set()
        ans = s = left = 0
        for x in nums:
            while x in st:
                st.remove(nums[left])
                s -= nums[left]
                left += 1
            st.add(x)
            s += x
            ans = max(ans, s)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumUniqueSubarray(int[] nums) {
        Set<Integer> set = new HashSet<>();
        int ans = 0, s = 0, left = 0;
        for (int x : nums) {
            while (set.contains(x)) {
                set.remove(nums[left]);
                s -= nums[left];
                left++;
            }
            set.add(x);
            s += x;
            ans = Math.max(ans, s);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumUniqueSubarray(vector<int>& nums) {
        unordered_set<int> st;
        int ans = 0, s = 0, left = 0;
        for (int x : nums) {
            while (st.contains(x)) {
                st.erase(nums[left]);
                s -= nums[left];
                left++;
            }
            st.insert(x);
            s += x;
            ans = max(ans, s);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumUniqueSubarray(nums []int) (ans int) {
	has := map[int]bool{}
	s, left := 0, 0
	for _, x := range nums {
		for has[x] {
			delete(has, nums[left])
			s -= nums[left]
			left++
		}
		has[x] = true
		s += x
		ans = max(ans, s)
	}
	return
}
```

```js [sol-JavaScript]
var maximumUniqueSubarray = function(nums) {
    const set = new Set();
    let ans = 0, s = 0, left = 0;
    for (const x of nums) {
        while (set.has(x)) {
            set.delete(nums[left]);
            s -= nums[left];
            left++;
        }
        set.add(x);
        s += x;
        ans = Math.max(ans, s);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashSet;

impl Solution {
    pub fn maximum_unique_subarray(nums: Vec<i32>) -> i32 {
        let mut set = HashSet::new();
        let mut ans = 0;
        let mut s = 0;
        let mut left = 0;
        for &x in &nums {
            while set.contains(&x) {
                set.remove(&nums[left]);
                s -= nums[left];
                left += 1;
            }
            set.insert(x);
            s += x;
            ans = ans.max(s);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $n$ 次，所以总循环次数为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

如果 $\textit{nums}$ 中有负数，要怎么做？

欢迎在评论区分享你的思路/代码。

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

本题来自 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
