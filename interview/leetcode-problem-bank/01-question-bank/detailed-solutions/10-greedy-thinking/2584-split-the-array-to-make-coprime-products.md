# 2584. 分割数组使乘积互质

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/split-the-array-to-make-coprime-products/
- 题目 slug：`split-the-array-to-make-coprime-products`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.5 合并区间
- 难度分：2159
- 外部题解来源：https://leetcode.cn/problems/split-the-array-to-make-coprime-products/solutions/2148324/ben-zhi-shi-tiao-yue-you-xi-by-endlessch-8chd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[本质是跳跃游戏（Python/Java/C++/Go）](https://leetcode.cn/problems/split-the-array-to-make-coprime-products/solutions/2148324/ben-zhi-shi-tiao-yue-you-xi-by-endlessch-8chd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ben-zhi-shi-tiao-yue-you-xi-by-endlessch-8chd`
- topic id：`2148324`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

对于一个质因子 $p$，设它在数组中的最左和最右的位置为 $\textit{left}$ 和 $\textit{right}$。

那么答案是不能在区间 $[\textit{left},\textit{right})$ 中的。注意区间右端点可能为答案。

因此这题本质上和 [55. 跳跃游戏](https://leetcode.cn/problems/jump-game/) 是类似的，找从 $0$ 出发，最远遇到的区间右端点，即为答案。

附：[视频讲解](https://www.bilibili.com/video/BV1SN411c7eD/)，包含**质因子分解**的讲解。

```py [sol-Python3]
class Solution:
    def findValidSplit(self, nums: List[int]) -> int:
        left = {}  # left[p] 表示质数 p 首次出现的下标
        right = [0] * len(nums)  # right[i] 表示左端点为 i 的区间的右端点的最大值

        def f(p: int, i: int) -> None:
            if p in left:
                right[left[p]] = i  # 记录左端点 l 对应的右端点的最大值
            else:
                left[p] = i  # 第一次遇到质数 p

        for i, x in enumerate(nums):
            d = 2
            while d * d <= x:  # 分解质因数
                if x % d == 0:
                    f(d, i)
                    x //= d
                    while x % d == 0:
                        x //= d
                d += 1
            if x > 1: f(x, i)

        max_r = 0
        for l, r in enumerate(right):
            if l > max_r:  # 最远可以遇到 max_r
                return max_r  # 也可以写 l-1
            max_r = max(max_r, r)
        return -1
```

```java [sol-Java]
class Solution {
    public int findValidSplit(int[] nums) {
        int n = nums.length;
        var left = new HashMap<Integer, Integer>(); // left[p] 表示质数 p 首次出现的下标
        var right = new int[n]; // right[i] 表示左端点为 i 的区间的右端点的最大值

        for (int i = 0; i < n; i++) {
            int x = nums[i];
            for (int d = 2; d * d <= x; ++d)  // 分解质因数
                if (x % d == 0) {
                    if (left.containsKey(d))
                        right[left.get(d)] = i; // 记录左端点对应的右端点的最大值
                    else
                        left.put(d, i); // 第一次遇到质数 d
                    for (x /= d; x % d == 0; x /= d) ;
                }
            if (x > 1)
                if (left.containsKey(x))
                    right[left.get(x)] = i;
                else
                    left.put(x, i);
        }

        for (int l = 0, maxR = 0; l < n; l++) {
            if (l > maxR) // 最远可以遇到 maxR
                return maxR; // 也可以写 l-1
            maxR = Math.max(maxR, right[l]);
        }
        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findValidSplit(vector<int> &nums) {
        unordered_map<int, int> left; // left[p] 表示质数 p 首次出现的下标
        int n = nums.size(), right[n]; // right[i] 表示左端点为 i 的区间的右端点的最大值
        memset(right, 0, sizeof(right));
        auto f = [&](int p, int i) {
            auto it = left.find(p);
            if (it == left.end())
                left[p] = i; // 第一次遇到质数 p
            else
                right[it->second] = i; // 记录左端点 l 对应的右端点的最大值
        };

        for (int i = 0; i < n; ++i) {
            int x = nums[i];
            for (int d = 2; d * d <= x; ++d) { // 分解质因数
                if (x % d == 0) {
                    f(d, i);
                    for (x /= d; x % d == 0; x /= d);
                }
            }
            if (x > 1) f(x, i);
        }

        for (int l = 0, max_r = 0; l < n; ++l) {
            if (l > max_r) // 最远可以遇到 max_r
                return max_r; // 也可以写 l-1
            max_r = max(max_r, right[l]);
        }
        return -1;
    }
};
```

```cpp [sol-C++ 更快写法]
class Solution {
public:
    int findValidSplit(vector<int> &nums) {
        unordered_map<int, int> left; // left[p] 表示质数 p 首次出现的下标
        int n = nums.size(), right[n]; // right[i] 表示左端点为 i 的区间的右端点的最大值
        memset(right, 0, sizeof(right));
        auto f = [&](int p, int i) {
            auto it = left.find(p);
            if (it == left.end())
                left[p] = i; // 第一次遇到质数 p
            else
                right[it->second] = i; // 记录左端点 l 对应的右端点的最大值
        };

        for (int i = 0; i < n; ++i) {
            int x = nums[i];
            if (x % 2 == 0) { // 单独处理 2，这样下面只需要枚举奇数
                f(2, i);
                x >>= __builtin_ctz(x);
            }
            for (int d = 3; d * d <= x; d += 2) { // 分解质因数
                if (x % d == 0) {
                    f(d, i);
                    for (x /= d; x % d == 0; x /= d);
                }
            }
            if (x > 1) f(x, i);
        }

        for (int l = 0, max_r = 0; l < n; ++l) {
            if (l > max_r) // 最远可以遇到 max_r
                return max_r; // 也可以写 l-1
            max_r = max(max_r, right[l]);
        }
        return -1;
    }
};
```

```go [sol-Go]
func findValidSplit(nums []int) int {
	left := map[int]int{} // left[p] 表示质数 p 首次出现的下标
	right := make([]int, len(nums)) // right[i] 表示左端点为 i 的区间的右端点的最大值
	f := func(p, i int) {
		if l, ok := left[p]; ok {
			right[l] = i // 记录左端点 l 对应的右端点的最大值
		} else {
			left[p] = i // 第一次遇到质数 p
		}
	}

	for i, x := range nums {
		for d := 2; d*d <= x; d++ { // 分解质因数
			if x%d == 0 {
				f(d, i)
				for x /= d; x%d == 0; x /= d {
				}
			}
		}
		if x > 1 {
			f(x, i)
		}
	}

	maxR := 0
	for l, r := range right {
		if l > maxR { // 最远可以遇到 maxR
			return maxR // 也可以写 l-1
		}
		maxR = max(maxR, r)
	}
	return -1
}

func max(a, b int) int { if a < b { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(n\sqrt U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$O\left(n + \dfrac{U}{\log U}\right)$。$U$ 范围内的质数个数有 $O\left(\dfrac{U}{\log U}\right)$ 个。

## 相似题目

- [55. 跳跃游戏](https://leetcode.cn/problems/jump-game/)
- [45. 跳跃游戏 II](https://leetcode.cn/problems/jump-game-ii/)
- [56. 合并区间](https://leetcode.cn/problems/merge-intervals/)
- [1024. 视频拼接](https://leetcode.cn/problems/video-stitching/)
- [1326. 灌溉花园的最少水龙头数目](https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/)

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、区间贪心 / §2.5 合并区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.5 合并区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
