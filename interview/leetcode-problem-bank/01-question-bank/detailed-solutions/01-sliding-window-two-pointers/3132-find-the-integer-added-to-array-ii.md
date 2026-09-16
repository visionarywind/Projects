# 3132. 找出与数组相加的整数 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-integer-added-to-array-ii/
- 题目 slug：`find-the-integer-added-to-array-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.2 判断子序列
- 难度分：1620
- 外部题解来源：https://leetcode.cn/problems/find-the-integer-added-to-array-ii/solutions/2759118/onlogn-pai-xu-shuang-zhi-zhen-pythonjava-rdj9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(nlogn) 排序+判断子序列（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-the-integer-added-to-array-ii/solutions/2759118/onlogn-pai-xu-shuang-zhi-zhen-pythonjava-rdj9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`onlogn-pai-xu-shuang-zhi-zhen-pythonjava-rdj9`
- topic id：`2759118`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

把两个数组都从小到大排序。

由于只能移除两个元素，所以 $\textit{nums}_1$ 的前三小元素必定有一个是保留下来的，我们可以枚举**保留下来的最小元素**是 $\textit{nums}_1[0]$ 还是 $\textit{nums}_1[1]$ 还是 $\textit{nums}_1[2]$。

⚠**注意**：保留下来的**最小**元素绝对不可能是 $\textit{nums}_1[3]$ 或者更大的数，因为这意味着我们把 $\textit{nums}_1[0],\textit{nums}_1[1],\textit{nums}_1[2]$ 都移除了，而题目要求只能移除两个元素。

例如排序后 $\textit{nums}_1=[2, 5, 6, 7, 8, 10],\ \textit{nums}_2 = [3, 4, 5, 8]$，如果 $\textit{nums}_1$ 中保留下来的最小元素是 $\textit{nums}_1[1]=5$，那么 $x = \textit{nums}_2[0] - \textit{nums}_1[1] = 3-5=-2$，这意味着如果我们把 $\textit{nums}_1$ 的每个数都加上 $x=-2$，得到 $\textit{nums}_1'=[0,3,4,5,6,8]$，问题就变成判断 $\textit{nums}_2$ 是否为 $\textit{nums}_1'$ 的子序列，如果是子序列，那么我们就可以放心地移除多余的两个数了。

做法见 [392. 判断子序列](https://leetcode.cn/problems/is-subsequence/)，可以用**同向双指针**解决，[我的题解](https://leetcode.cn/problems/is-subsequence/solution/jian-ji-xie-fa-pythonjavaccgojsrust-by-e-mz22/)。

代码实现时，可以先判断保留下来的最小元素是 $\textit{nums}_1[2]$，再判断是 $\textit{nums}_1[1]$，最后判断是 $\textit{nums}_1[0]$。这是因为 $\textit{nums}_1[i]$ 越大，答案 $x$ 越小，第一个满足的就是答案。此外，由于题目保证答案一定存在，所以当 $\textit{nums}_1[2]$ 和 $\textit{nums}_1[1]$ 都不满足时，直接返回 $\textit{nums}_2[0] - \textit{nums}_1[0]$，无需判断。

请看 [视频讲解](https://www.bilibili.com/video/BV1Pw4m1C79N/) 第二题，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def minimumAddedInteger(self, nums1: List[int], nums2: List[int]) -> int:
        nums1.sort()
        nums2.sort()
        # 枚举保留 nums1[2] 或者 nums1[1] 或者 nums1[0]
        # 倒着枚举是因为 nums1[i] 越大答案越小，第一个满足的就是答案
        for i in range(2, 0, -1):
            x = nums2[0] - nums1[i]
            # 在 {nums1[i] + x} 中找子序列 nums2
            j = 0
            for v in nums1[i:]:
                if nums2[j] == v + x:
                    j += 1
                    # nums2 是 {nums1[i] + x} 的子序列
                    if j == len(nums2):
                        return x
        # 题目保证答案一定存在
        return nums2[0] - nums1[0]
```

```py [sol-Python3 iter]
class Solution:
    def minimumAddedInteger(self, nums1: List[int], nums2: List[int]) -> int:
        nums1.sort()
        nums2.sort()
        for i in range(2, 0, -1):
            x = nums2[0] - nums1[i]
            it = iter(nums1[i:])
            # 判断 {nums2[j] - x} 是否为 nums1[i:] 的子序列
            # in 会消耗迭代器
            if all(v - x in it for v in nums2):
                return x
        return nums2[0] - nums1[0]
```

```java [sol-Java]
class Solution {
    public int minimumAddedInteger(int[] nums1, int[] nums2) {
        Arrays.sort(nums1);
        Arrays.sort(nums2);
        // 枚举保留 nums1[2] 或者 nums1[1] 或者 nums1[0]
        // 倒着枚举是因为 nums1[i] 越大答案越小，第一个满足的就是答案
        for (int i = 2; i > 0; i--) {
            int x = nums2[0] - nums1[i];
            // 在 {nums1[i] + x} 中找子序列 nums2
            int j = 0;
            for (int k = i; k < nums1.length; k++) {
                if (nums2[j] == nums1[k] + x && ++j == nums2.length) {
                    // nums2 是 {nums1[i] + x} 的子序列
                    return x;
                }
            }
        }
        // 题目保证答案一定存在
        return nums2[0] - nums1[0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumAddedInteger(vector<int>& nums1, vector<int>& nums2) {
        ranges::sort(nums1);
        ranges::sort(nums2);
        // 枚举保留 nums1[2] 或者 nums1[1] 或者 nums1[0]
        // 倒着枚举是因为 nums1[i] 越大答案越小，第一个满足的就是答案
        for (int i = 2; i > 0; i--) {
            int x = nums2[0] - nums1[i];
            // 在 {nums1[i] + x} 中找子序列 nums2
            int j = 0;
            for (int k = i; k < nums1.size(); k++) {
                if (nums2[j] == nums1[k] + x && ++j == nums2.size()) {
                    // nums2 是 {nums1[i] + x} 的子序列
                    return x;
                }
            }
        }
        // 题目保证答案一定存在
        return nums2[0] - nums1[0];
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int minimumAddedInteger(int* nums1, int nums1Size, int* nums2, int nums2Size) {
    qsort(nums1, nums1Size, sizeof(int), cmp);
    qsort(nums2, nums2Size, sizeof(int), cmp);
    // 枚举保留 nums1[2] 或者 nums1[1] 或者 nums1[0]
    // 倒着枚举是因为 nums1[i] 越大答案越小，第一个满足的就是答案
    for (int i = 2; i > 0; i--) {
        int x = nums2[0] - nums1[i];
        // 在 {nums1[i] + x} 中找子序列 nums2
        int j = 0;
        for (int k = i; k < nums1Size; k++) {
            if (nums2[j] == nums1[k] + x && ++j == nums2Size) {
                // nums2 是 {nums1[i] + x} 的子序列
                return x;
            }
        }
    }
    // 题目保证答案一定存在
    return nums2[0] - nums1[0];
}
```

```go [sol-Go]
func minimumAddedInteger(nums1, nums2 []int) int {
	slices.Sort(nums1)
	slices.Sort(nums2)
	// 枚举保留 nums1[2] 或者 nums1[1] 或者 nums1[0]
	// 倒着枚举是因为 nums1[i] 越大答案越小，第一个满足的就是答案
	for i := 2; i > 0; i-- {
		x := nums2[0] - nums1[i]
		// 在 {nums1[i] + x} 中找子序列 nums2
		j := 0
		for _, v := range nums1[i:] {
			if nums2[j] == v+x {
				j++
				// nums2 是 {nums1[i] + x} 的子序列
				if j == len(nums2) {
					return x
				}
			}
		}
	}
	// 题目保证答案一定存在
	return nums2[0] - nums1[0]
}
```

```js [sol-JS]
var minimumAddedInteger = function(nums1, nums2) {
    nums1.sort((a, b) => a - b);
    nums2.sort((a, b) => a - b);
    // 枚举保留 nums1[2] 或者 nums1[1] 或者 nums1[0]
    // 倒着枚举是因为 nums1[i] 越大答案越小，第一个满足的就是答案
    for (let i = 2; i > 0; i--) {
        const x = nums2[0] - nums1[i];
        // 在 {nums1[i] + x} 中找子序列 nums2
        let j = 0;
        for (let k = i; k < nums1.length; k++) {
            if (nums2[j] === nums1[k] + x && ++j === nums2.length) {
                // nums2 是 {nums1[i] + x} 的子序列
                return x;
            }
        }
    }
    // 题目保证答案一定存在
    return nums2[0] - nums1[0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_added_integer(mut nums1: Vec<i32>, mut nums2: Vec<i32>) -> i32 {
        nums1.sort_unstable();
        nums2.sort_unstable();
        // 枚举保留 nums1[2] 或者 nums1[1] 或者 nums1[0]
        // 倒着枚举是因为 nums1[i] 越大答案越小，第一个满足的就是答案
        for i in (1..3).rev() {
            let x = nums2[0] - nums1[i];
            // 在 {nums1[i] + x} 中找子序列 nums2
            let mut j = 0;
            for &v in &nums1[i..] {
                if nums2[j] == v + x && { j += 1; j == nums2.len() } {
                    // nums2 是 {nums1[i] + x} 的子序列
                    return x;
                }
            }
        }
        // 题目保证答案一定存在
        nums2[0] - nums1[0]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}_1$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销，忽略切片的空间。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、双序列双指针 / §4.2 判断子序列`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、双序列双指针 / §4.2 判断子序列`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
