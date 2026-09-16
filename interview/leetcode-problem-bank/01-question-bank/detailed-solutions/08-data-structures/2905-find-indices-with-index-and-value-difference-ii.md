# 2905. 找出满足差值条件的下标 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-indices-with-index-and-value-difference-ii/
- 题目 slug：`find-indices-with-index-and-value-difference-ii`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1764
- 外部题解来源：https://leetcode.cn/problems/find-indices-with-index-and-value-difference-ii/solutions/2483143/shuang-zhi-zhen-wei-hu-zui-da-zui-xiao-p-h4bx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 做法：双指针+维护最大最小（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-indices-with-index-and-value-difference-ii/solutions/2483143/shuang-zhi-zhen-wei-hu-zui-da-zui-xiao-p-h4bx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shuang-zhi-zhen-wei-hu-zui-da-zui-xiao-p-h4bx`
- topic id：`2483143`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

不妨假设 $i$ 在左，$j$ 在右，即 $i\le j - \textit{indexDifference}$。

枚举 $j$，寻找左边的 $i$。要想满足 $|\textit{nums}[i]-\textit{nums}[j]|\ge \textit{valueDifference}$，根据绝对值的几何意义，$\textit{nums}[i]$ 应当尽量远离 $\textit{nums}[j]$，才能让绝对值尽量大。也就是说，要找的 $\textit{nums}[i]$ 应当**尽量大**或者**尽量小**。

类似 [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/)，我们可以在枚举 $j$ 的同时，维护 $\textit{nums}[0]$ 到 $\textit{nums}[j - \textit{indexDifference}]$ 中的最大值 $\textit{mx}$ 和最小值 $\textit{mn}$。

例如 $\textit{indexDifference}=2$，从 $j=2$ 开始遍历数组：

- 遍历到 $\textit{nums}[2]$，用 $\textit{nums}[0]$ 更新 $\textit{mx}$ 和 $\textit{mn}$。
- 遍历到 $\textit{nums}[3]$，用 $\textit{nums}[1]$ 更新 $\textit{mx}$ 和 $\textit{mn}$。
- 遍历到 $\textit{nums}[4]$，用 $\textit{nums}[2]$ 更新 $\textit{mx}$ 和 $\textit{mn}$。
- 依此类推。

这个过程可以保证 $\textit{mx}$ 和 $\textit{mn}$ 在数组中的下标 $i$ 满足 $i\le j - \textit{indexDifference}$，即题目的第一个要求。

对于题目的第二个要求，可以转换成如下两个不等式：

- $\textit{mx} -\textit{nums}[j] \ge \textit{valueDifference}$
- $\textit{nums}[j] - mn \ge \textit{valueDifference}$

满足其一即可返回答案。不用算绝对值的原因见下面的答疑。

由于要输出 $\textit{mx}$ 或者 $\textit{mn}$ 在数组中的下标，我们可以记录 $\textit{mx}$ 在数组中的下标 $\textit{maxIdx}$，以及 $\textit{mn}$ 在数组中的下标 $\textit{minIdx}$。由于知道下标就能知道元素值，所以只需记录下标，无需记录 $\textit{mx}$ 和 $\textit{mn}$。

### 答疑

**问**：为什么不用算绝对值？万一 $\textit{mx} < \textit{nums}[j]$，并且 $|\textit{mx} - \textit{nums}[j]| = \textit{nums}[j] - \textit{mx} \ge \textit{valueDifference}$，不就错过答案了吗？

**答**：在上述条件成立的前提下，由于 $\textit{mn} \le \textit{mx}$，得

$$
\textit{nums}[j] - \textit{mn} \ge \textit{nums}[j] - \textit{mx} \ge \textit{valueDifference}
$$ 

所以此时 $\textit{mn}$ 是满足要求的，不会错过答案。

```py [sol-Python3]
class Solution:
    def findIndices(self, nums: List[int], indexDifference: int, valueDifference: int) -> List[int]:
        max_idx = min_idx = 0
        for j in range(indexDifference, len(nums)):
            i = j - indexDifference
            if nums[i] > nums[max_idx]:
                max_idx = i
            elif nums[i] < nums[min_idx]:
                min_idx = i
            if nums[max_idx] - nums[j] >= valueDifference:
                return [max_idx, j]
            if nums[j] - nums[min_idx] >= valueDifference:
                return [min_idx, j]
        return [-1, -1]
```

```java [sol-Java]
class Solution {
    public int[] findIndices(int[] nums, int indexDifference, int valueDifference) {
        int maxIdx = 0;
        int minIdx = 0;
        for (int j = indexDifference; j < nums.length; j++) {
            int i = j - indexDifference;
            if (nums[i] > nums[maxIdx]) {
                maxIdx = i;
            } else if (nums[i] < nums[minIdx]) {
                minIdx = i;
            }
            if (nums[maxIdx] - nums[j] >= valueDifference) {
                return new int[]{maxIdx, j};
            }
            if (nums[j] - nums[minIdx] >= valueDifference) {
                return new int[]{minIdx, j};
            }
        }
        return new int[]{-1, -1};
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findIndices(vector<int>& nums, int indexDifference, int valueDifference) {
        int max_idx = 0, min_idx = 0;
        for (int j = indexDifference; j < nums.size(); j++) {
            int i = j - indexDifference;
            if (nums[i] > nums[max_idx]) {
                max_idx = i;
            } else if (nums[i] < nums[min_idx]) {
                min_idx = i;
            }
            if (nums[max_idx] - nums[j] >= valueDifference) {
                return {max_idx, j};
            }
            if (nums[j] - nums[min_idx] >= valueDifference) {
                return {min_idx, j};
            }
        }
        return {-1, -1};
    }
};
```

```c [sol-C]
int* findIndices(int* nums, int numsSize, int indexDifference, int valueDifference, int* returnSize) {
    int* ans = malloc(2 * sizeof(int));
    *returnSize = 2;
    int max_idx = 0, min_idx = 0;
    for (int j = indexDifference; j < numsSize; j++) {
        int i = j - indexDifference;
        if (nums[i] > nums[max_idx]) {
            max_idx = i;
        } else if (nums[i] < nums[min_idx]) {
            min_idx = i;
        }
        if (nums[max_idx] - nums[j] >= valueDifference) {
            ans[0] = max_idx;
            ans[1] = j;
            return ans;
        }
        if (nums[j] - nums[min_idx] >= valueDifference) {
            ans[0] = min_idx;
            ans[1] = j;
            return ans;
        }
    }
    ans[0] = -1;
    ans[1] = -1;
    return ans;
}
```

```go [sol-Go]
func findIndices(nums []int, indexDifference, valueDifference int) []int {
	maxIdx, minIdx := 0, 0
	for j := indexDifference; j < len(nums); j++ {
		i := j - indexDifference
		if nums[i] > nums[maxIdx] {
			maxIdx = i
		} else if nums[i] < nums[minIdx] {
			minIdx = i
		}
		if nums[maxIdx]-nums[j] >= valueDifference {
			return []int{maxIdx, j}
		}
		if nums[j]-nums[minIdx] >= valueDifference {
			return []int{minIdx, j}
		}
	}
	return []int{-1, -1}
}
```

```js [sol-JavaScript]
var findIndices = function(nums, indexDifference, valueDifference) {
    let maxIdx = 0, minIdx = 0;
    for (let j = indexDifference; j < nums.length; j++) {
        const i = j - indexDifference;
        if (nums[i] > nums[maxIdx]) {
            maxIdx = i;
        } else if (nums[i] < nums[minIdx]) {
            minIdx = i;
        }
        if (nums[maxIdx] - nums[j] >= valueDifference) {
            return [maxIdx, j];
        }
        if (nums[j] - nums[minIdx] >= valueDifference) {
            return [minIdx, j];
        }
    }
    return [-1, -1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_indices(nums: Vec<i32>, index_difference: i32, value_difference: i32) -> Vec<i32> {
        let mut max_idx = 0;
        let mut min_idx = 0;
        for j in index_difference as usize..nums.len() {
            let i = j - index_difference as usize;
            if nums[i] > nums[max_idx] {
                max_idx = i;
            } else if nums[i] < nums[min_idx] {
                min_idx = i;
            }
            if nums[max_idx] - nums[j] >= value_difference {
                return vec![max_idx as i32, j as i32];
            }
            if nums[j] - nums[min_idx] >= value_difference {
                return vec![min_idx as i32, j as i32];
            }
        }
        vec![-1, -1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n-\textit{indexDifference})$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

改成求满足要求的下标对的个数，要怎么做？

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
