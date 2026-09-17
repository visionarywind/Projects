# 2789. 合并后数组中的最大元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-element-in-an-array-after-merge-operations/
- 题目 slug：`largest-element-in-an-array-after-merge-operations`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1485
- 外部题解来源：https://leetcode.cn/problems/largest-element-in-an-array-after-merge-operations/solutions/2685894/cong-bao-li-mei-ju-dao-yi-ci-bian-li-pyt-jjta/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从暴力枚举到一次遍历（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/largest-element-in-an-array-after-merge-operations/solutions/2685894/cong-bao-li-mei-ju-dao-yi-ci-bian-li-pyt-jjta/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-bao-li-mei-ju-dao-yi-ci-bian-li-pyt-jjta`
- topic id：`2685894`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

## 暴力思路

从 $i$ 开始，只要 $\textit{nums}[i-1] \le \textit{nums}[i]$，就不断向左合并数字。

例如示例 1 从 $i=3$ 开始，可以一直向左合并到 $i=0$。

$$
[2,3,\underline{7},\underline{9},3]\rightarrow[2,\underline{3},\underline{16},3]\rightarrow[\underline{2},\underline{19},3]\rightarrow[21,3]
$$

枚举从 $i=0,1,2,\cdots,n-1$ 开始向左合并，取合并结果的最大值作为答案。

时间复杂度为 $\mathcal{O}(n^2)$，太慢了。

## 优化

如果我们从 $i=n-1$ 开始向左合并，合并到 $j\ (j > 0)$ 停止，这意味着 

$$
\textit{nums}[j-1] > \textit{nums}[j] + \textit{nums}[j+1] + \cdots + \textit{nums}[n-1]
$$

所以从 $i=n-2,n-3,\cdots,j$ 开始向左合并，是不会合并出更大的数的，因为 $\textit{nums}$ 中的元素都是正数，这样合并出来的数字只会更小，永远不会比 $\textit{nums}[j-1]$ 更大。

所以无需枚举 $i=n-2,n-3,\cdots,j$，而是直接从 $i=j-1$ 开始向左合并，并且合并出的数字一定比从 $i=n-1$ 开始更大。

## 算法

1. 初始化元素和 $\textit{sum} = \textit{nums}[n-1]$，表示从 $n-1$ 开始向左合并。
2. 从 $i=n-2$ 开始倒着遍历数组。
3. 如果 $\textit{nums}[i] \le \textit{sum}$，那么就合并，把 $\textit{sum}$ 增加 $\textit{nums}[i]$。
4. 如果 $\textit{nums}[i] > \textit{sum}$，无法合并，并且这意味着从 $i$ 开始向左合并可以得到比 $\textit{sum}$ 更大的元素和，所以直接更新 $\textit{sum} = \textit{nums}[i]$，继续向左合并。
5. 遍历结束，返回 $\textit{sum}$。

```py [sol-Python3]
class Solution:
    def maxArrayValue(self, nums: List[int]) -> int:
        s = nums[-1]
        for i in range(len(nums) - 2, -1, -1):
            s = s + nums[i] if nums[i] <= s else nums[i]
        return s
```

```java [sol-Java]
class Solution {
    public long maxArrayValue(int[] nums) {
        int n = nums.length;
        long sum = nums[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            sum = nums[i] <= sum ? sum + nums[i] : nums[i];
        }
        return sum;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxArrayValue(vector<int> &nums) {
        long long sum = nums.back();
        for (int i = nums.size() - 2; i >= 0; i--) {
            sum = nums[i] <= sum ? sum + nums[i] : nums[i];
        }
        return sum;
    }
};
```

```go [sol-Go]
func maxArrayValue(nums []int) int64 {
	n := len(nums)
	sum := nums[n-1]
	for i := n - 2; i >= 0; i-- {
		if nums[i] <= sum {
			sum += nums[i] // 继续向左合并
		} else {
			sum = nums[i]
		}
	}
	return int64(sum)
}
```

```js [sol-JavaScript]
var maxArrayValue = function(nums) {
    const n = nums.length;
    let sum = nums[n - 1];
    for (let i = n - 2; i >= 0; i--) {
        sum = nums[i] <= sum ? sum + nums[i] : nums[i];
    }
    return sum;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_array_value(nums: Vec<i32>) -> i64 {
        let n = nums.len();
        let mut sum = nums[n - 1] as i64;
        for i in (0..n - 1).rev() {
            let x = nums[i] as i64;
            sum = if x <= sum { sum + x } else { x };
        }
        sum
    }
}
```

（部分语言）也可以直接在原数组上合并。

```py [sol-Python3]
class Solution:
    def maxArrayValue(self, nums: List[int]) -> int:
        for i in range(len(nums) - 1, 0, -1):
            if nums[i - 1] <= nums[i]:
                nums[i - 1] += nums[i]  # 把合并值向左传
        return nums[0]
```

```go [sol-Go]
func maxArrayValue(nums []int) int64 {
	for i := len(nums) - 1; i > 0; i-- {
		if nums[i-1] <= nums[i] {
			nums[i-1] += nums[i] // 把合并值向左传
		}
	}
	return int64(nums[0])
}
```

```js [sol-JavaScript]
var maxArrayValue = function(nums) {
    for (let i = nums.length - 1; i > 0; i--) {
        if (nums[i - 1] <= nums[i]) {
            nums[i - 1] += nums[i]; // 把合并值向左传
        }
    }
    return nums[0];
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

- [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
- [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
- [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
- [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
- [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
- [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.4 从最左/最右开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.4 从最左/最右开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
