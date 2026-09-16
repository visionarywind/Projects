# 1539. 第 k 个缺失的正整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kth-missing-positive-number/
- 题目 slug：`kth-missing-positive-number`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/kth-missing-positive-number/solutions/3959163/olog-n-xiang-xi-tui-dao-jian-ji-xie-fa-p-67ag/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(log n) 详细推导，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/kth-missing-positive-number/solutions/3959163/olog-n-xiang-xi-tui-dao-jian-ji-xie-fa-p-67ag/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`olog-n-xiang-xi-tui-dao-jian-ji-xie-fa-p-67ag`
- topic id：`3959163`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先确定答案落在哪两个数之间。这取决于在 $\textit{arr}[i]$ 之前缺失了多少个正整数。

在 $\textit{arr}[i]$ 之前的正整数为 $1,2,\ldots,\textit{arr}[i] - 1$，这有 $\textit{arr}[i] - 1$ 个。其中在 $\textit{arr}$ 中的正整数为 $\textit{arr}[0],\textit{arr}[1],\ldots, \textit{arr}[i-1]$，这有 $i$ 个（注意 $\textit{arr}$ 是严格递增的，没有重复元素）。

二者相减，得到在 $\textit{arr}[i]$ 之前缺失的正整数个数：

$$
\textit{arr}[i] - 1 - i
$$

设 $b[i] = \textit{arr}[i] - 1 - i$，由于 $b[i+1] - b[i] = (\textit{arr}[i+1] - 1 - (i+1)) - (\textit{arr}[i] - 1 - i) = \textit{arr}[i+1] - \textit{arr}[i] - 1 \ge 0$，所以 $b[i]$ 是（非严格）递增数组。

示例 1 的 $b=[1,1,1,3,6]$。既然 $b$ 是递增的，我们可以在 $b$ 中**二分查找**第一个满足 $b[i]\ge k$ 的下标 $i$。在示例 1 中，我们找到的是 $b[4] = 6$，这意味着 $\textit{arr}[4]-1=10$ 是缺失的第 $6$ 个正整数。我们找的是缺失的第 $k=5$ 个正整数，那么从 $10$ 再往前数 $6-k=1$ 个数，即 $10-1=9$，就是缺失的第 $5$ 个正整数。

一般地，设 $i$ 是第一个满足 $\textit{arr}[i] - 1 - i\ge k$ 的下标，那么 $\textit{arr}[i]-1$ 是缺失的第 $\textit{arr}[i] - 1 - i$ 个正整数。再往前数 $\textit{arr}[i] - 1 - i - k$ 个数，就是缺失的第 $k$ 个正整数，即

$$
\textit{arr}[i]-1 - (\textit{arr}[i] - 1 - i - k) = i + k
$$

关于二分查找的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

[本题视频讲解](https://www.bilibili.com/video/BV15pZcBzEmR/?t=11m2s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def findKthPositive(self, arr: list[int], k: int) -> int:
        left = -1
        right = len(arr)
        while left + 1 < right:
            mid = (left + right) // 2
            if arr[mid] - 1 - mid >= k:
                right = mid
            else:
                left = mid
        return right + k  # 推导过程见题解
```

```py [sol-Python3 库函数二分]
class Solution:
    def findKthPositive(self, arr: list[int], k: int) -> int:
        i = bisect_left(range(len(arr)), True, key=lambda i: arr[i] - 1 - i >= k)
        return i + k  # 推导过程见题解
```

```java [sol-Java]
class Solution {
    public int findKthPositive(int[] arr, int k) {
        int left = -1;
        int right = arr.length;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (arr[mid] - 1 - mid >= k) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right + k; // 推导过程见题解
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findKthPositive(vector<int>& arr, int k) {
        int left = -1, right = arr.size();
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (arr[mid] - 1 - mid >= k ? right : left) = mid;
        }
        return right + k; // 推导过程见题解
    }
};
```

```c [sol-C]
int findKthPositive(int* arr, int arrSize, int k) {
    int left = -1, right = arrSize;
    while (left + 1 < right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] - 1 - mid >= k) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right + k; // 推导过程见题解
}
```

```go [sol-Go]
func findKthPositive(arr []int, k int) int {
	i := sort.Search(len(arr), func(i int) bool {
		return arr[i]-1-i >= k
	})
	return i + k // 推导过程见题解
}
```

```js [sol-JavaScript]
var findKthPositive = function(arr, k) {
    let left = -1, right = arr.length;
    while (left + 1 < right) {
        const mid = Math.floor((left + right) / 2);
        if (arr[mid] - 1 - mid >= k) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right + k; // 推导过程见题解
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_kth_positive(arr: Vec<i32>, k: i32) -> i32 {
        let mut left = 0;
        let mut right = arr.len();
        while left < right {
            let mid = left + (right - left) / 2;
            if arr[mid] - 1 - mid as i32 >= k {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        left as i32 + k // 推导过程见题解
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 是 $\textit{arr}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

[3911. 移除子数组元素后第 K 小偶数](https://leetcode.cn/problems/k-th-smallest-remaining-even-integer-in-subarray-queries/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
