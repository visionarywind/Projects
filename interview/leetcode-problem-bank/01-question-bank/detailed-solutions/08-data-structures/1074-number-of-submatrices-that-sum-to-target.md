# 1074. 元素和为目标值的子矩阵数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-submatrices-that-sum-to-target/
- 题目 slug：`number-of-submatrices-that-sum-to-target`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：2189
- 外部题解来源：https://leetcode.cn/problems/number-of-submatrices-that-sum-to-target/solutions/3704958/mei-ju-shang-xia-bian-jie-zhuan-hua-chen-h6i4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举上下边界，转化成 560 题（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/number-of-submatrices-that-sum-to-target/solutions/3704958/mei-ju-shang-xia-bian-jie-zhuan-hua-chen-h6i4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-shang-xia-bian-jie-zhuan-hua-chen-h6i4`
- topic id：`3704958`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

枚举子矩阵的上下边界，计算子矩阵每一列的元素和，我们可以得到一个一维数组，每一项表示子矩阵一列的元素和。

现在问题变成：

- 在这个一维数组中，有多少个非空连续子数组，元素和等于 $\textit{target}$？

这题就是 [560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/)，做法见 [我的题解](https://leetcode.cn/problems/subarray-sum-equals-k/solutions/2781031/qian-zhui-he-ha-xi-biao-cong-liang-ci-bi-4mwr/)。

```py [sol-Python3]
class Solution:
    # 560. 和为 K 的子数组
    def subarraySum(self, nums: List[int], k: int) -> int:
        ans = s = 0
        cnt = defaultdict(int)
        for x in nums:
            cnt[s] += 1
            s += x
            ans += cnt[s - k]
        return ans

    def numSubmatrixSumTarget(self, matrix: List[List[int]], target: int) -> int:
        n = len(matrix[0])
        ans = 0
        for i in range(len(matrix)):  # 上边界
            col_sum = [0] * n
            for row in matrix[i:]:  # 下边界
                for j, x in enumerate(row):
                    col_sum[j] += x
                ans += self.subarraySum(col_sum, target)
        return ans
```

```java [sol-Java]
class Solution {
    public int numSubmatrixSumTarget(int[][] matrix, int target) {
        int m = matrix.length;
        int n = matrix[0].length;
        int ans = 0;
        for (int i = 0; i < m; i++) { // 上边界
            int[] colSum = new int[n];
            for (int i2 = i; i2 < m; i2++) { // 下边界
                for (int j = 0; j < n; j++) {
                    colSum[j] += matrix[i2][j];
                }
                ans += subarraySum(colSum, target);
            }
        }
        return ans;
    }

    // 560. 和为 K 的子数组
    private int subarraySum(int[] nums, int k) {
        int ans = 0;
        int s = 0;
        Map<Integer, Integer> cnt = new HashMap<>(nums.length, 1); // 预分配空间
        for (int x : nums) {
            cnt.merge(s, 1, Integer::sum); // cnt[s]++
            s += x;
            ans += cnt.getOrDefault(s - k, 0);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 560. 和为 K 的子数组
    int subarraySum(vector<int>& nums, int k) {
        int ans = 0, s = 0;
        unordered_map<int, int> cnt;
        for (int x : nums) {
            cnt[s]++;
            s += x;
            // 注意不要直接 += cnt[s-k]，如果 s-k 不存在，这会插入 s-k，消耗更多空间
            ans += cnt.contains(s - k) ? cnt[s - k] : 0;
        }
        return ans;
    }

public:
    int numSubmatrixSumTarget(vector<vector<int>>& matrix, int target) {
        int m = matrix.size(), n = matrix[0].size();
        int ans = 0;
        for (int i = 0; i < m; i++) { // 上边界
            vector<int> col_sum(n);
            for (int i2 = i; i2 < m; i2++) { // 下边界
                for (int j = 0; j < n; j++) {
                    col_sum[j] += matrix[i2][j];
                }
                ans += subarraySum(col_sum, target);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
// 560. 和为 K 的子数组
func subarraySum(nums []int, k int) (ans int) {
    cnt := make(map[int]int, len(nums)) // 预分配空间
    s := 0
    for _, x := range nums {
        cnt[s]++
        s += x
        ans += cnt[s-k]
    }
    return
}

func numSubmatrixSumTarget(matrix [][]int, target int) (ans int) {
    n := len(matrix[0])
    for i := range matrix { // 上边界
        colSum := make([]int, n)
        for _, row := range matrix[i:] { // 下边界
            for j, x := range row {
                colSum[j] += x
            }
            ans += subarraySum(colSum, target)
        }
    }
    return
}
```

```js [sol-JavaScript]
// 560. 和为 K 的子数组
var subarraySum = function(nums, k) {
    let ans = 0, s = 0;
    const cnt = new Map();
    for (const x of nums) {
        cnt.set(s, (cnt.get(s) ?? 0) + 1);
        s += x;
        ans += cnt.get(s - k) ?? 0;
    }
    return ans;
};

var numSubmatrixSumTarget = function(matrix, target) {
    const m = matrix.length, n = matrix[0].length;
    let ans = 0;
    for (let i = 0; i < m; i++) { // 上边界
        const colSum = Array(n).fill(0);
        for (let i2 = i; i2 < m; i2++) { // 下边界
            for (let j = 0; j < n; j++) {
                colSum[j] += matrix[i2][j];
            }
            ans += subarraySum(colSum, target);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    // 560. 和为 K 的子数组
    fn subarray_sum(nums: &[i32], k: i32) -> i32 {
        let mut ans = 0;
        let mut s = 0;
        let mut cnt = HashMap::with_capacity(nums.len()); // 预分配空间
        for x in nums {
            *cnt.entry(s).or_insert(0) += 1;
            s += x;
            if let Some(&c) = cnt.get(&(s - k)) {
                ans += c;
            }
        }
        return ans;
    }

    pub fn num_submatrix_sum_target(matrix: Vec<Vec<i32>>, target: i32) -> i32 {
        let n = matrix[0].len();
        let mut ans = 0;
        for i in 0..matrix.len() { // 上边界
            let mut col_sum = vec![0; n];
            for row in &matrix[i..] { // 下边界
                for (j, &x) in row.iter().enumerate() {
                    col_sum[j] += x;
                }
                ans += Self::subarray_sum(&col_sum, target);
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m^2n)$，其中 $m$ 和 $n$ 分别为 $\textit{matrix}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

见下面数据结构题单的「**§1.2 前缀和与哈希表**」。

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

本题来自 `一、前缀和 / §1.2 前缀和与哈希表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.2 前缀和与哈希表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
