# 1673. 找出最具竞争力的子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-most-competitive-subsequence/
- 题目 slug：`find-the-most-competitive-subsequence`
- 来源专题：单调栈
- 来源分类路径：四、最小字典序
- 难度分：1802
- 外部题解来源：https://leetcode.cn/problems/find-the-most-competitive-subsequence/solutions/2788312/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-36c4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[跟着我过一遍示例 2，你就明白怎么做了（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-the-most-competitive-subsequence/solutions/2788312/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-36c4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-36c4`
- topic id：`2788312`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**题意**：返回 $\textit{nums}$ 的长度恰好为 $k$ 的字典序最小子序列。

由于要返回的是 $\textit{nums}$ 的一个子序列，我们尝试在遍历 $\textit{nums}$ 的过程中，去生成字典序最小的子序列。

以示例 2 为例，$\textit{nums}=[2,4,3,3,5,4,9,6],\ k = 4$。

- $\textit{nums}[0]=2$，目前生成的子序列为 $[2]$。
- $\textit{nums}[1]=4$，由于 $4>2$，不能把子序列中的 $2$ 替换掉，因为这会让字典序变大。由于目标子序列长度为 $k=4$，所以直接把 $4$ 加到子序列的末尾，目前生成的子序列为 $[2,4]$。
- $\textit{nums}[2]=3$，由于 $3<4$，把子序列末尾的 $4$ 去掉，添加 $3$，会让字典序变小。注意，不能继续把 $2$ 也去掉，这会让字典序变大。目前生成的子序列为 $[2,3]$。
- $\textit{nums}[3]=3$，无法让字典序变小，且当前子序列长度不足 $k=4$，所以直接把 $3$ 加到子序列的末尾，目前生成的子序列为 $[2,3,3]$。
- $\textit{nums}[4]=5$，无法让字典序变小，且当前子序列长度不足 $k=4$，所以直接把 $5$ 加到子序列的末尾，目前生成的子序列为 $[2,3,3,5]$。
- $\textit{nums}[5]=4$，由于 $4<5$，把子序列末尾的 $5$ 去掉，添加 $4$，会让字典序变小。注意，不能继续把 $3$ 也去掉，这会让字典序变大。目前生成的子序列为 $[2,3,3,4]$。**注意**：假如 $k=7$，这里不能把子序列末尾的 $5$ 去掉，因为一旦去掉，即使把后面的所有元素都加到子序列中（得到 $[2,3,3,4,9,6]$），长度也无法达到 $7$，不符合题目要求。
- $\textit{nums}[6]=9$，无法让字典序变小，且当前子序列长度已经等于 $k=4$，所以什么也不做，目前生成的子序列为 $[2,3,3,4]$。
- $\textit{nums}[7]=6$，无法让字典序变小，且当前子序列长度已经等于 $k=4$，所以什么也不做，目前生成的子序列为 $[2,3,3,4]$。

按照上面的流程，为了维护生成的子序列，我们需要一个后进先出的数据结构：栈。当然，用列表/数组表示栈也可以。

由于题目要求生成长为 $k$ 的子序列，我们不能无限制地弹出栈顶元素。在弹出栈顶元素**之前**，必须保证栈中元素个数加上剩余元素个数是大于 $k$ 的。如果代码中不写这个判断，最后可能会得到一个长度小于 $k$ 的子序列。

算法如下：

1. 创建一个空栈。
2. 从左到右遍历 $\textit{nums}$。
3. 设 $x=\textit{nums}[i]$。如果栈不为空，且 $x$ 小于栈顶，且栈的大小加上剩余元素个数（$n-i$）大于 $k$，则可以弹出栈顶。不断循环直到不满足这三个条件之一。
4. 如果栈的大小小于 $k$，把 $x$ 入栈。
5. 遍历结束，栈（从底到顶的顺序）就是答案。

```py [sol-Py3]
class Solution:
    def mostCompetitive(self, nums: List[int], k: int) -> List[int]:
        st = []
        for i, x in enumerate(nums):
            while st and x < st[-1] and len(st) + len(nums) - i > k:
                st.pop()
            if len(st) < k:
                st.append(x)
        return st
```

```py [sol-Py3]
class Solution:
    def mostCompetitive(self, nums: List[int], k: int) -> List[int]:
        m = 0  # 栈的大小
        for i, x in enumerate(nums):
            while m and x < nums[m - 1] and m + len(nums) - i > k:
                m -= 1  # 出栈
            if m < k:
                nums[m] = x  # 入栈（把 nums 当作栈）
                m += 1
        del nums[m:]
        return nums
```

```java [sol-Java]
class Solution {
    public int[] mostCompetitive(int[] nums, int k) {
        int n = nums.length;
        int[] st = new int[k]; // 用数组模拟栈（栈容量为 k）
        int m = 0; // 栈的大小
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            while (m > 0 && x < st[m - 1] && m + n - i > k) {
                m--; // 出栈
            }
            if (m < k) {
                st[m++] = x; // 入栈
            }
        }
        return st;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> mostCompetitive(vector<int>& nums, int k) {
        vector<int> st;
        for (int i = 0; i < nums.size(); i++) {
            int x = nums[i];
            while (!st.empty() && x < st.back() && st.size() + nums.size() - i > k) {
                st.pop_back();
            }
            if (st.size() < k) {
                st.push_back(x);
            }
        }
        return st;
    }
};
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> mostCompetitive(vector<int>& nums, int k) {
        int m = 0; // 栈的大小
        for (int i = 0; i < nums.size(); i++) {
            int x = nums[i];
            while (m && x < nums[m - 1] && m + nums.size() - i > k) {
                m--; // 出栈
            }
            if (m < k) {
                nums[m++] = x; // 入栈（把 nums 当作栈）
            }
        }
        nums.resize(m);
        return nums;
    }
};
```

```go [sol-Go]
func mostCompetitive(nums []int, k int) []int {
    st := []int{}
    for i, x := range nums {
        for len(st) > 0 && x < st[len(st)-1] && len(st)+len(nums)-i > k {
            st = st[:len(st)-1]
        }
        if len(st) < k {
            st = append(st, x)
        }
    }
    return st
}
```

```go [sol-Go]
func mostCompetitive(nums []int, k int) []int {
    st := nums[:0] // 把 nums 当作栈
    for i, x := range nums {
        for len(st) > 0 && x < st[len(st)-1] && len(st)+len(nums)-i > k {
            st = st[:len(st)-1]
        }
        if len(st) < k {
            st = append(st, x)
        }
    }
    return st
}
```

```js [sol-JS]
var mostCompetitive = function(nums, k) {
    const st = [];
    for (let i = 0; i < nums.length; i++) {
        const x = nums[i];
        while (st.length && x < st[st.length - 1] && st.length + nums.length - i > k) {
            st.pop();
        }
        if (st.length < k) {
            st.push(x);
        }
    }
    return st;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn most_competitive(nums: Vec<i32>, k: i32) -> Vec<i32> {
        let k = k as usize;
        let mut st = Vec::with_capacity(k); // 预分配空间
        for (i, &x) in nums.iter().enumerate() {
            while !st.is_empty() && x < *st.last().unwrap() && st.len() + nums.len() - i > k {
                st.pop();
            }
            if st.len() < k {
                st.push(x);
            }
        }
        st
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然我们写了一个二重循环，但注意到每个元素至多入栈出栈各一次，所以二重循环的时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(k)$ 或 $\mathcal{O}(1)$。如果直接把 $\textit{nums}$ 当作栈，或者不计入返回值的空间，则空间复杂度为 $\mathcal{O}(1)$。

## 相似题目

见 [单调栈题单](https://leetcode.cn/circle/discuss/9oZFK9/) 中的「最小字典序」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
