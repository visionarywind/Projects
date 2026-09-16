# 2815. 数组中的最大数对和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/max-pair-sum-in-an-array/
- 题目 slug：`max-pair-sum-in-an-array`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/max-pair-sum-in-an-array/solutions/2385996/yi-ci-bian-li-by-endlesscheng-6zt9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/max-pair-sum-in-an-array/solutions/2385996/yi-ci-bian-li-by-endlesscheng-6zt9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-ci-bian-li-by-endlesscheng-6zt9`
- topic id：`2385996`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

用一个长为 $10$ 的数组 $\textit{maxVal}[i]$ 维护最大数位为 $i$ 的元素的最大值。

当我们遍历到 $\textit{nums}[i]$ 时，设其最大数位为 $\textit{maxD}$，那么另一个最大数位也是 $\textit{maxD}$ 的最大元素为 $\textit{maxVal}[\textit{maxD}]$，用

$$
\textit{nums}[i] + \textit{maxVal}[\textit{maxD}]
$$

更新答案的最大值。

[视频讲解](https://www.bilibili.com/video/BV1wh4y1Q7XW/)

```py [sol-Python3]
class Solution:
    def maxSum(self, nums: List[int]) -> int:
        ans = -1
        max_val = [-inf] * 10  # 表示不存在最大值
        for v in nums:
            max_d = max(map(int, str(v)))
            ans = max(ans, v + max_val[max_d])
            max_val[max_d] = max(max_val[max_d], v)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxSum(int[] nums) {
        int ans = -1;
        int[] maxVal = new int[10];
        Arrays.fill(maxVal, Integer.MIN_VALUE); // 表示不存在最大值
        for (int v : nums) {
            int maxD = 0;
            for (int x = v; x > 0; x /= 10) {
                maxD = Math.max(maxD, x % 10);
            }
            ans = Math.max(ans, v + maxVal[maxD]);
            maxVal[maxD] = Math.max(maxVal[maxD], v);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSum(vector<int>& nums) {
        int ans = -1;
        int max_val[10];
        ranges::fill(max_val, INT_MIN); // 表示不存在最大值
        for (int v : nums) {
            int max_d = 0;
            for (int x = v; x; x /= 10) {
                max_d = max(max_d, x % 10);
            }
            ans = max(ans, v + max_val[max_d]);
            max_val[max_d] = max(max_val[max_d], v);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxSum(int* nums, int numsSize) {
    int ans = -1;
    int max_val[10];
    memset(max_val, -0x3f, sizeof(max_val)); // 表示不存在最大值
    for (int i = 0; i < numsSize; i++) {
        int v = nums[i];
        int max_d = 0;
        for (int x = v; x; x /= 10) {
            max_d = MAX(max_d, x % 10);
        }
        ans = MAX(ans, v + max_val[max_d]);
        max_val[max_d] = MAX(max_val[max_d], v);
    }
    return ans;
}
```

```go [sol-Go]
func maxSum(nums []int) int {
	ans := -1
	maxVal := [10]int{}
	for i := range maxVal {
		maxVal[i] = math.MinInt // 表示不存在最大值
	}
	for _, v := range nums {
		maxD := 0
		for x := v; x > 0; x /= 10 {
			maxD = max(maxD, x%10)
		}
		ans = max(ans, v+maxVal[maxD])
		maxVal[maxD] = max(maxVal[maxD], v)
	}
	return ans
}
```

```js [sol-JavaScript]
var maxSum = function(nums) {
    const maxVal = Array(10).fill(-Infinity); // 表示不存在最大值
    let ans = -1;
    for (const v of nums) {
        let maxD = 0;
        for (let x = v; x; x = Math.floor(x / 10)) {
            maxD = Math.max(maxD, x % 10);
        }
        ans = Math.max(ans, v + maxVal[maxD]);
        maxVal[maxD] = Math.max(maxVal[maxD], v);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_sum(nums: Vec<i32>) -> i32 {
        let mut ans = -1;
        let mut max_val = [i32::MIN; 10]; // 表示不存在最大值
        for v in nums {
            let mut max_d = 0;
            let mut x = v;
            while x > 0 {
                max_d = max_d.max(x % 10);
                x /= 10;
            }
            let max_d = max_d as usize;
            ans = ans.max(v + max_val[max_d]);
            max_val[max_d] = max_val[max_d].max(v);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

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
