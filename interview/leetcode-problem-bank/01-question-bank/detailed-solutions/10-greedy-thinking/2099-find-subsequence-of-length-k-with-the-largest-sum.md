# 2099. 找到和最大的长度为 K 的子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-subsequence-of-length-k-with-the-largest-sum/
- 题目 slug：`find-subsequence-of-length-k-with-the-largest-sum`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1447
- 外部题解来源：https://leetcode.cn/problems/find-subsequence-of-length-k-with-the-largest-sum/solutions/1152460/liang-ci-pai-xu-by-endlesscheng-994b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-subsequence-of-length-k-with-the-largest-sum/solutions/1152460/liang-ci-pai-xu-by-endlesscheng-994b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-pai-xu-by-endlesscheng-994b`
- topic id：`1152460`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

问题相当于选 $\textit{nums}$ 中最大的 $k$ 个数，这样总和才能最大。

但子序列的顺序是不能变的，怎么办？

1. 创建一个下标数组 $\textit{idx} = [0,1,2,\ldots,n-1]$，根据 $\textit{nums}[\textit{idx}[i]]$ 对 $\textit{idx}$ 排序，这样就知道最大的 $k$ 个数在哪里了。
2. 排序前 $k$ 大元素的下标。
3. 最后，获取这些下标对应的元素值，即为子序列。

```py [sol-Python3]
class Solution:
    def maxSubsequence(self, nums: List[int], k: int) -> List[int]:
        idx = sorted(range(len(nums)), key=lambda i: nums[i])  # 创建下标数组，对下标数组排序
        idx = sorted(idx[-k:])  # 取前 k 大元素的下标，排序
        return [nums[i] for i in idx]  # 取出 nums 的子序列
```

```java [sol-Java]
class Solution {
    public int[] maxSubsequence(int[] nums, int k) {
        // 创建下标数组，对下标数组排序
        Integer[] idx = new Integer[nums.length];
        Arrays.setAll(idx, i -> i);
        Arrays.sort(idx, (i, j) -> nums[j] - nums[i]);

        // 对前 k 个下标排序
        // 注：排序 int[] 比排序 Integer[] 快 2ms
        int[] ans = new int[k];
        for (int i = 0; i < k; i++) {
            ans[i] = idx[i];
        }
        Arrays.sort(ans);

        // 取出 nums 的子序列
        for (int i = 0; i < k; i++) {
            ans[i] = nums[ans[i]];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> maxSubsequence(vector<int>& nums, int k) {
        // 创建下标数组，对下标数组排序
        vector<int> idx(nums.size());
        ranges::iota(idx, 0);
        ranges::sort(idx, {}, [&](int i) { return -nums[i]; });

        // 对前 k 个下标排序
        idx.resize(k);
        ranges::sort(idx);

        // 取出 nums 的子序列
        for (int& i : idx) {
            i = nums[i];
        }
        return idx;
    }
};
```

```cpp [sol-C++ 快速选择]
class Solution {
public:
    vector<int> maxSubsequence(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> idx(n);
        ranges::iota(idx, 0);
        ranges::nth_element(idx, idx.begin() + k, {}, [&](int i) { return -nums[i]; });

        vector<int8_t> in_seq(n);
        for (int i = 0; i < k; i++) {
            in_seq[idx[i]] = true; // 标记前 k 大元素的下标
        }

        idx.resize(k);
        int j = 0;
        for (int i = 0; i < n; i++) {
            if (in_seq[i]) {
                idx[j++] = nums[i];
            }
        }
        return idx;
    }
};
```

```c [sol-C]
int* _nums;

int cmp_by_num(const void* a, const void* b) {
    return _nums[*(int*)b] - _nums[*(int*)a];
}

int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int* maxSubsequence(int* nums, int numsSize, int k, int* returnSize) {
    _nums = nums;

    // 创建下标数组，对下标数组排序
    int* idx = malloc(sizeof(int) * numsSize);
    for (int i = 0; i < numsSize; i++) {
        idx[i] = i;
    }
    qsort(idx, numsSize, sizeof(int), cmp_by_num);

    // 对前 k 个下标排序
    qsort(idx, k, sizeof(int), cmp);

    // 取出 nums 的子序列
    for (int i = 0; i < k; i++) {
        idx[i] = nums[idx[i]];
    }

    *returnSize = k;
    return idx;
}
```

```go [sol-Go]
func maxSubsequence(nums []int, k int) []int {
	// 创建下标数组，对下标数组排序
	idx := make([]int, len(nums))
	for i := range idx {
		idx[i] = i
	}
	slices.SortFunc(idx, func(i, j int) int { return nums[j] - nums[i] })

	// 对前 k 个下标排序
	idx = idx[:k]
	slices.Sort(idx)

	// 取出 nums 的子序列
	for i, j := range idx {
		idx[i] = nums[j]
	}
	return idx
}
```

```js [sol-JavaScript]
var maxSubsequence = function(nums, k) {
    // 创建下标数组，对下标数组排序
    let idx = [...nums.keys()]
    idx.sort((i, j) => nums[j] - nums[i]);

    // 对前 k 个下标排序
    idx.length = k;
    idx.sort((a, b) => a - b);

    // 取出 nums 的子序列
    return idx.map(i => nums[i]);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_subsequence(nums: Vec<i32>, k: i32) -> Vec<i32> {
        // 创建下标数组，对下标数组排序
        let mut idx = (0..nums.len()).collect::<Vec<_>>();
        idx.sort_unstable_by_key(|&i| -nums[i]);

        // 对前 k 个下标排序
        idx.truncate(k as usize);
        idx.sort_unstable();

        // 取出 nums 的子序列
        idx.into_iter().map(|i| nums[i]).collect()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$ 或 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。使用快速选择可以做到 $\mathcal{O}(n)$，见 C++ 代码。
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
