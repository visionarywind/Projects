# 1521. 找到最接近目标值的函数值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-a-value-of-a-mysterious-function-closest-to-target/
- 题目 slug：`find-a-value-of-a-mysterious-function-closest-to-target`
- 来源专题：位运算
- 来源分类路径：三、与或（AND/OR）的性质 / AND/OR LogTrick
- 难度分：3171
- 外部题解来源：https://leetcode.cn/problems/find-a-value-of-a-mysterious-function-closest-to-target/solutions/2798247/li-yong-and-de-xing-zhi-pythonjavacgo-by-mjx7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：LogTrick/滑动窗口+栈（Python/Java/C++/Go）](https://leetcode.cn/problems/find-a-value-of-a-mysterious-function-closest-to-target/solutions/2798247/li-yong-and-de-xing-zhi-pythonjavacgo-by-mjx7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`li-yong-and-de-xing-zhi-pythonjavacgo-by-mjx7`
- topic id：`2798247`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题和 [3171. 找到按位或最接近 K 的子数组](https://leetcode.cn/problems/find-subarray-with-bitwise-or-closest-to-k/) 几乎一样，仅仅是把 OR 改成了 AND。本文只留代码，具体原理请看 [我的题解](https://leetcode.cn/problems/find-subarray-with-bitwise-or-closest-to-k/solutions/2798206/li-yong-and-de-xing-zhi-pythonjavacgo-by-gg4d/)，两种方法都有讲解。

## 方法一：LogTrick

```py [sol-Python3]
class Solution:
    def closestToTarget(self, arr: List[int], k: int) -> int:
        ans = inf
        for i, x in enumerate(arr):
            ans = min(ans, abs(x - k))
            for j in range(i - 1, -1, -1):
                if arr[j] & x == arr[j]:
                    break
                arr[j] &= x
                ans = min(ans, abs(arr[j] - k))
        return ans
```

```java [sol-Java]
class Solution {
    public int closestToTarget(int[] arr, int k) {
        int ans = Integer.MAX_VALUE;
        for (int i = 0; i < arr.length; i++) {
            int x = arr[i];
            ans = Math.min(ans, Math.abs(x - k));
            for (int j = i - 1; j >= 0 && (arr[j] & x) != arr[j]; j--) {
                arr[j] &= x;
                ans = Math.min(ans, Math.abs(arr[j] - k));
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int closestToTarget(vector<int>& arr, int k) {
        int ans = INT_MAX;
        for (int i = 0; i < arr.size(); i++) {
            int x = arr[i];
            ans = min(ans, abs(x - k));
            for (int j = i - 1; j >= 0 && (arr[j] & x) != arr[j]; j--) {
                arr[j] &= x;
                ans = min(ans, abs(arr[j] - k));
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func closestToTarget(arr []int, k int) int {
	ans := math.MaxInt
	for i, x := range arr {
		ans = min(ans, abs(x-k))
		for j := i - 1; j >= 0 && arr[j]&x != arr[j]; j-- {
			arr[j] &= x
			ans = min(ans, abs(arr[j]-k))
		}
	}
	return ans
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{arr}$ 的长度，$U=\max(\textit{arr})$。由于 $10^6<2^{20}-1$，二进制数对应集合的大小不会超过 $19$，因此在与运算下，每个数字至多可以减少 $19$ 次。总体上看，二重循环的总循环次数等于每个数字可以减少的次数之和，即 $O(n\log U)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：滑动窗口+栈

**注**：$-1$ 的二进制全为 $1$，与任何数 $x$ 的 AND 结果都等于 $x$，适合作为初始值。

```py [sol-Python3]
class Solution:
    def closestToTarget(self, arr: List[int], k: int) -> int:
        ans = inf
        left = bottom = 0
        right_and = -1
        for right, x in enumerate(arr):
            right_and &= x
            while left <= right and arr[left] & right_and < k:
                ans = min(ans, k - (arr[left] & right_and))
                left += 1
                if bottom < left:
                    # 重新构建一个栈
                    for i in range(right - 1, left - 1, -1):
                        arr[i] &= arr[i + 1]
                    bottom = right
                    right_and = -1
            if left <= right:
                ans = min(ans, (arr[left] & right_and) - k)
        return ans
```

```java [sol-Java]
class Solution {
    public int closestToTarget(int[] arr, int k) {
        int ans = Integer.MAX_VALUE;
        int left = 0;
        int bottom = 0;
        int rightAnd = -1;
        for (int right = 0; right < arr.length; right++) {
            rightAnd &= arr[right];
            while (left <= right && (arr[left] & rightAnd) < k) {
                ans = Math.min(ans, k - (arr[left] & rightAnd));
                left++;
                if (bottom < left) {
                    // 重新构建一个栈
                    for (int i = right - 1; i >= left; i--) {
                        arr[i] &= arr[i + 1];
                    }
                    bottom = right;
                    rightAnd = -1;
                }
            }
            if (left <= right) {
                ans = Math.min(ans, (arr[left] & rightAnd) - k);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int closestToTarget(vector<int>& arr, int k) {
        int ans = INT_MAX, left = 0, bottom = 0, right_and = -1;
        for (int right = 0; right < arr.size(); right++) {
            right_and &= arr[right];
            while (left <= right && (arr[left] & right_and) < k) {
                ans = min(ans, k - (arr[left] & right_and));
                left++;
                if (bottom < left) {
                    // 重新构建一个栈
                    for (int i = right - 1; i >= left; i--) {
                        arr[i] &= arr[i + 1];
                    }
                    bottom = right;
                    right_and = -1;
                }
            }
            if (left <= right) {
                ans = min(ans, (arr[left] & right_and) - k);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func closestToTarget(arr []int, k int) int {
    ans := math.MaxInt
    left, bottom, rightAnd := 0, 0, -1
    for right, x := range arr {
        rightAnd &= x
        for left <= right && arr[left]&rightAnd < k {
            ans = min(ans, k-(arr[left]&rightAnd))
            left++
            if bottom < left {
                // 重新构建一个栈
                for i := right - 1; i >= left; i-- {
                    arr[i] &= arr[i+1]
                }
                bottom = right
                rightAnd = -1
            }
        }
        if left <= right {
            ans = min(ans, (arr[left]&rightAnd)-k)
        }
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{arr}$ 的长度。虽然我们写了个三重循环，但每个元素至多入栈出栈各一次，所以三重循环的**总**循环次数是 $\mathcal{O}(n)$ 的，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

更多相似题目，见位运算题单中的「**LogTrick**」。

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

本题来自 `三、与或（AND/OR）的性质 / AND/OR LogTrick`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、与或（AND/OR）的性质 / AND/OR LogTrick`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
