# 898. 子数组按位或操作

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/bitwise-ors-of-subarrays/
- 题目 slug：`bitwise-ors-of-subarrays`
- 来源专题：位运算
- 来源分类路径：三、与或（AND/OR）的性质 / AND/OR LogTrick
- 难度分：2133
- 外部题解来源：https://leetcode.cn/problems/bitwise-ors-of-subarrays/solutions/2839640/logtrick-ji-qi-jin-jie-tong-ji-mei-ge-ji-rleb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[LogTrick 及其进阶：统计每个结果的出现次数（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/bitwise-ors-of-subarrays/solutions/2839640/logtrick-ji-qi-jin-jie-tong-ji-mei-ge-ji-rleb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`logtrick-ji-qi-jin-jie-tong-ji-mei-ge-ji-rleb`
- topic id：`2839640`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

详细讲解请看 [LogTrick 入门教程](https://zhuanlan.zhihu.com/p/1933215367158830792)。

怎么计算子数组的 OR？

首先，我们有如下 $\mathcal{O}(n^2)$ 的暴力算法：

从左到右正向遍历 $\textit{arr}$，对于 $x=\textit{arr}[i]$，从 $i-1$ 开始倒着遍历 $\textit{arr}[j]$，更新 $\textit{arr}[j]=\textit{arr}[j]\ \vert\ x$。

- $i=1$ 时，我们会把 $\textit{arr}[0]$ 到 $\textit{arr}[1]$ 的 OR 记录在 $\textit{arr}[0]$ 中。
- $i=2$ 时，我们会把 $\textit{arr}[1]$ 到 $\textit{arr}[2]$ 的 OR 记录在 $\textit{arr}[1]$ 中，$\textit{arr}[0]$ 到 $\textit{arr}[2]$ 的 OR 记录在 $\textit{arr}[0]$ 中。
- $i=3$ 时，我们会把 $\textit{arr}[2]$ 到 $\textit{arr}[3]$ 的 OR 记录在 $\textit{arr}[2]$ 中；$\textit{arr}[1]$ 到 $\textit{arr}[3]$ 的 OR 记录在 $\textit{arr}[1]$ 中；$\textit{arr}[0]$ 到 $\textit{arr}[3]$ 的 OR 记录在 $\textit{arr}[0]$ 中。
- 按照该算法，可以计算出所有子数组的 OR。注意单个元素也算子数组。

下面来优化该算法。

前置知识：[从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

把二进制数看成集合，两个数的 OR 就是两个集合的**并集**。

对于两个二进制数 $a$ 和 $b$，如果 $a\ \vert\ b = a$，从集合的角度上看，$b$ 对应的集合是 $a$ 对应的集合的子集。

据此我们可以提出如下优化：

仍然是从左到右正向遍历 $\textit{arr}$，对于 $x=\textit{arr}[i]$，从 $i-1$ 开始倒着遍历 $\textit{arr}[j]$：
- 如果 $\textit{arr}[j]\ \vert\ x\ne\textit{arr}[j]$，说明 $\textit{arr}[j]$ 可以变大（求并集后，集合元素只会增多不会减少），更新 $\textit{arr}[j]=\textit{arr}[j]\ \vert\ x$。
- 否则 $\textit{arr}[j]\ \vert\ x=\textit{arr}[j]$，从集合的角度看，此时 $x$ 不仅是 $\textit{arr}[j]$ 的子集，同时也是 $\textit{arr}[k]\ (k<j)$ 的子集（因为前面的循环保证了每个集合都是其左侧相邻集合的子集），在 $B\subseteq A$ 的前提下，$A\cup B=A$，所以后续的循环都不会改变元素值，退出内层循环。
- 在循环中，把 $\textit{arr}[j]$ 加到一个集合中（用来去重）。
- 注意单个元素也可以组成子数组，要把 $\textit{arr}[i]$ 也加到集合中。

具体例子可以看 [视频讲解](https://www.bilibili.com/video/BV1Qx4y1E7zj/) 第四题（计算的是子数组 AND）。

```py [sol-Python3]
class Solution:
    def subarrayBitwiseORs(self, arr: List[int]) -> int:
        st = set()
        for i, x in enumerate(arr):
            st.add(x)
            for j in range(i - 1, -1, -1):
                if arr[j] | x == arr[j]:
                    break
                arr[j] |= x
                st.add(arr[j])
        return len(st)
```

```java [sol-Java]
class Solution {
    public int subarrayBitwiseORs(int[] arr) {
        Set<Integer> st = new HashSet<>();
        for (int i = 0; i < arr.length; i++) {
            st.add(arr[i]);
            for (int j = i - 1; j >= 0 && (arr[j] | arr[i]) != arr[j]; j--) {
                arr[j] |= arr[i];
                st.add(arr[j]);
            }
        }
        return st.size();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int subarrayBitwiseORs(vector<int>& arr) {
        unordered_set<int> st;
        for (int i = 0; i < arr.size(); i++) {
            st.insert(arr[i]);
            for (int j = i - 1; j >= 0 && (arr[j] | arr[i]) != arr[j]; j--) {
                arr[j] |= arr[i];
                st.insert(arr[j]);
            }
        }
        return st.size();
    }
};
```

```go [sol-Go]
func subarrayBitwiseORs(arr []int) int {
    set := map[int]bool{}
    for i, x := range arr {
        set[x] = true
        for j := i - 1; j >= 0 && arr[j]|x != arr[j]; j-- {
            arr[j] |= x
            set[arr[j]] = true
        }
    }
    return len(set)
}
```

```js [sol-JavaScript]
var subarrayBitwiseORs = function(arr) {
    const st = new Set();
    for (let i = 0; i < arr.length; i++) {
        st.add(arr[i]);
        for (let j = i - 1; j >= 0 && (arr[j] | arr[i]) != arr[j]; j--) {
            arr[j] |= arr[i];
            st.add(arr[j]);
        }
    }
    return st.size;
};
```

```rust [sol-Rust]
use std::collections::HashSet;

impl Solution {
    pub fn subarray_bitwise_o_rs(mut arr: Vec<i32>) -> i32 {
        let mut st = HashSet::new();
        for i in 0..arr.len() {
            st.insert(arr[i]);
            for j in (0..i).rev() {
                if (arr[j] | arr[i]) == arr[j] {
                    break;
                }
                arr[j] |= arr[i];
                st.insert(arr[j]);
            }
        }
        st.len() as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{arr}$ 的长度，$U=\max(\textit{arr})$。由于 $10^9<2^{30}$，二进制数对应集合的大小不会超过 $30$，因此在或运算下，每个数字至多可以增大 $30$ 次（从空集增大到有 $30$ 个元素）。**总体上看**，二重循环的总循环次数等于每个数字可以增大的次数之和，即 $O(n\log U)$。
- 空间复杂度：$\mathcal{O}(n\log U)$。最坏情况下有 $\mathcal{O}(n\log U)$ 种不同的结果。

## 思考题

如果要计算每个结果的出现次数，怎么做？

#### 解答

在遍历的同时，额外用一个 map $\textit{cntEnd}$ 维护以 $i$ 为右端点的子数组 OR 的个数。内层循环结束后，把 $\textit{cntEnd}$ 中的数据添加到答案中。

```cpp
map<int, int> logTrick(vector<int>& a) {
    map<int, int> cnt, cnt_end;
    for (int i = 0; i < a.size(); i++) {
        cnt_end[a[i]]++;
        for (int j = i - 1; j >= 0 && (a[j] | a[i]) != a[j]; j--) {
            if (--cnt_end[a[j]] == 0) {
                cnt_end.erase(a[j]); // 保证 cnt_end.size() = O(log U)
            }
            a[j] |= a[i];
            cnt_end[a[j]]++;
        }
        for (auto& [v, c] : cnt_end) {
            cnt[v] += c;
        }
    }
    return cnt;
}
```

可以做做 [CF475D. CGCDSSQ](https://codeforces.com/contest/475/problem/D)。

## 相似题目

见位运算题单中的「**LogTrick**」。

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
