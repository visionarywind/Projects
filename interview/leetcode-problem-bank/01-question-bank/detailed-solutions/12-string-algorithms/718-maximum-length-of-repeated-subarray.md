# 718. 最长重复子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-length-of-repeated-subarray/
- 题目 slug：`maximum-length-of-repeated-subarray`
- 来源专题：字符串
- 来源分类路径：四、字符串哈希
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-length-of-repeated-subarray/solutions/866328/on-hou-zhui-shu-zu-by-endlesscheng-jwr2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划，附后缀数组 O(n+m) 做法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-length-of-repeated-subarray/solutions/866328/on-hou-zhui-shu-zu-by-endlesscheng-jwr2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-hou-zhui-shu-zu-by-endlesscheng-jwr2`
- topic id：`866328`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 13:14:52 +0800

本题类似 [1143. 最长公共子序列](https://leetcode.cn/problems/longest-common-subsequence/)，那题是求子序列，这题是求连续子数组。

**状态定义**。仿照 1143，定义 $f[i+1][j+1]$ 表示以 $\textit{nums}_1[i]$ 结尾的子数组和以 $\textit{nums}_2[j]$ 结尾的子数组的最长公共子数组的长度。这里 $+1$ 是为了兼容空子数组的情况，也就是把 $f[0][j]$ 和 $f[i][0]$ 定义为空。如果不写 $+1$，就需要特判 $i=0$ 或者 $j=0$ 的情况了。

分类讨论：

- 如果 $\textit{nums}_1[i]\ne \textit{nums}_2[j]$，那么 $f[i+1][j+1] = 0$。
- 如果 $\textit{nums}_1[i]= \textit{nums}_2[j]$，那么问题变成以 $\textit{nums}_1[i-1]$ 结尾的子数组和以 $\textit{nums}_2[j-1]$ 结尾的子数组的最长公共子数组的长度，即 $f[i+1][j+1] = f[i][j] + 1$。相当于在以 $\textit{nums}_1[i-1]$ 结尾的子数组后面添加 $\textit{nums}_1[i]$，在以 $\textit{nums}_2[j-1]$ 结尾的子数组后面添加 $\textit{nums}_2[j]$。

**初始值**：$f[0][j] = f[i][0] = 0$，空子数组没有公共部分。

**答案**：所有 $f[i][j]$ 的最大值。

```py [sol-Python3]
class Solution:
    def findLength(self, nums1: List[int], nums2: List[int]) -> int:
        n, m = len(nums1), len(nums2)
        f = [[0] * (m + 1) for _ in range(n + 1)]
        for i, x in enumerate(nums1):
            for j, y in enumerate(nums2):
                if x == y:
                    f[i + 1][j + 1] = f[i][j] + 1
        return max(map(max, f))  # 所有 f[i][j] 的最大值
```

```java [sol-Java]
class Solution {
    public int findLength(int[] nums1, int[] nums2) {
        int n = nums1.length;
        int m = nums2.length;
        int ans = 0;
        int[][] f = new int[n + 1][m + 1];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (nums1[i] == nums2[j]) {
                    f[i + 1][j + 1] = f[i][j] + 1;
                    ans = Math.max(ans, f[i + 1][j + 1]);
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findLength(vector<int>& nums1, vector<int>& nums2) {
        int n = nums1.size(), m = nums2.size(), ans = 0;
        vector f(n + 1, vector<int>(m + 1));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (nums1[i] == nums2[j]) {
                    f[i + 1][j + 1] = f[i][j] + 1; // 递推关系
                    ans = max(ans, f[i + 1][j + 1]);
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func findLength(nums1, nums2 []int) (ans int) {
    f := make([][]int, len(nums1)+1)
    for i := range f {
        f[i] = make([]int, len(nums2)+1)
    }
    for i, x := range nums1 {
        for j, y := range nums2 {
            if x == y {
                f[i+1][j+1] = f[i][j] + 1
                ans = max(ans, f[i+1][j+1])
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var findLength = function(nums1, nums2) {
    const n = nums1.length, m = nums2.length;
    const f = Array.from({ length: n + 1 }, () => Array(m + 1).fill(0));
    let ans = 0;
    for (let i = 0; i < n; i++) {
        for (let j = 0; j < m; j++) {
            if (nums1[i] === nums2[j]) {
                f[i + 1][j + 1] = f[i][j] + 1;
                ans = Math.max(ans, f[i + 1][j + 1]);
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_length(nums1: Vec<i32>, nums2: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut f = vec![vec![0; nums2.len() + 1]; nums1.len() + 1];
        for (i, x) in nums1.into_iter().enumerate() {
            for (j, &y) in nums2.iter().enumerate() {
                if x == y {
                    f[i + 1][j + 1] = f[i][j] + 1;
                    ans = ans.max(f[i + 1][j + 1]);
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $\textit{nums}_1$ 的长度，$m$ 是 $\textit{nums}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(nm)$。可以优化成 $\mathcal{O}(m)$，见 [1143 题解](https://leetcode.cn/problems/longest-common-subsequence/solutions/2133188/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-lbz5/)。

## 附：后缀数组

[后缀数组介绍](https://oi-wiki.org/string/sa/)

```go [sol-Go]
func findLength(nums1, nums2 []int) (ans int) {
    n1, n2 := int32(len(nums1)), int32(len(nums2))
    s := make([]byte, 0, n1+n2+1)
    for _, x := range nums1 {
        s = append(s, byte(x))
    }
    s = append(s, 101) // 用一个不在数组中的数拼接两个数组
    for _, x := range nums2 {
        s = append(s, byte(x))
    }
    n := len(s)

    // s 的后缀数组
    sa := (*struct {
        _  []byte
        sa []int32
    })(unsafe.Pointer(suffixarray.New(s))).sa

    // 后缀名次数组 rank（相当于 sa 的反函数）
    // 后缀 s[i:] 位于后缀字典序中的第 rank[i] 个
    // 特别地，rank[0] 即 s 在后缀字典序中的排名，rank[n-1] 即 s[n-1:] 在字典序中的排名
    rank := make([]int, n)
    for i, p := range sa {
        rank[p] = i
    }

    // 高度数组 height
    // height[0] = 0
    // height[i] = LCP(s[sa[i]:], s[sa[i-1]:])
    height := make([]int, n)
    h := 0
    for i, rk := range rank {
        if h > 0 {
            h--
        }
        if rk > 0 {
            for j := int(sa[rk-1]); i+h < len(s) && j+h < len(s) && s[i+h] == s[j+h]; h++ {
            }
        }
        height[rk] = h
    }

    // 若高度对应的 sa[i-1] 和 sa[i] 属于不同数组，则更新答案的最大值
    for i := 1; i < n; i++ {
        if sa[i] < n1 != (sa[i-1] < n1) {
            ans = max(ans, height[i])
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{nums}_1$ 的长度，$m$ 是 $\textit{nums}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

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

本题来自 `四、字符串哈希`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、字符串哈希`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
