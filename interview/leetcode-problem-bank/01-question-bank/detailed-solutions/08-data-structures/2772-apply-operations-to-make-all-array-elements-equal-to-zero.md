# 2772. 使数组中的所有元素都等于零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/apply-operations-to-make-all-array-elements-equal-to-zero/
- 题目 slug：`apply-operations-to-make-all-array-elements-equal-to-zero`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.2 进阶
- 难度分：2029
- 外部题解来源：https://leetcode.cn/problems/apply-operations-to-make-all-array-elements-equal-to-zero/solutions/2336744/chai-fen-shu-zu-pythonjavacgojs-by-endle-8qrt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[差分数组（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/apply-operations-to-make-all-array-elements-equal-to-zero/solutions/2336744/chai-fen-shu-zu-pythonjavacgojs-by-endle-8qrt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chai-fen-shu-zu-pythonjavacgojs-by-endle-8qrt`
- topic id：`2336744`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1XW4y1f7Wv/) 第四题。

## 提示 1

想一想，如果 $\textit{nums}[0]>0$，我们必须要执行什么样的操作，才能让 $\textit{nums}[0]=0$？

## 提示 2

对于 $\textit{nums}[0]>0$ 的情况，必须把 $\textit{nums}[0]$ 到 $\textit{nums}[k-1]$ 都减去 $\textit{nums}[0]$。

然后思考 $\textit{nums}[1]$ 要怎么处理，依此类推。

## 提示 3

子数组同时加上/减去一个数，非常适合用 [差分数组](https://leetcode.cn/circle/discuss/FfMCgb/) 来维护，请至少做一道差分数组题目再往下阅读。

设差分数组为 $d$。那么把 $\textit{nums}[i]$ 到 $\textit{nums}[i+k-1]$ 同时减去 $1$，等价于把 $d[i]$ 减 $1$，$d[i+k]$ 加 $1$。

注意子数组长度必须恰好等于 $k$，所以当 $i+k\le n$ 时，才能执行上述操作。

遍历数组的同时，用变量 $\textit{sumD}$ 累加差分值。遍历到 $\textit{nums}[i]$ 时，$\textit{nums}[i]+\textit{sumD}$ 就是 $\textit{nums}[i]$ 的实际值了。

分类讨论：

- 如果 $\textit{nums}[i]<0$，由于无法让元素值增大，返回 `false`。
- 如果 $\textit{nums}[i]=0$，无需操作，遍历下一个数。
- 如果 $\textit{nums}[i]>0$：
  - 如果 $i+k> n$，无法执行操作，所以 $\textit{nums}[i]$ 无法变成 $0$，返回 `false`。
  - 如果 $i+k\le n$，按照上面说的执行操作，修改差分数组，遍历下一个数。

如果遍历中途没有返回 `false`，那么最后返回 `true`。

```py [sol-Python3]
class Solution:
    def checkArray(self, nums: List[int], k: int) -> bool:
        n = len(nums)
        d = [0] * (n + 1)
        sum_d = 0
        for i, x in enumerate(nums):
            sum_d += d[i]
            x += sum_d
            if x == 0: continue  # 无需操作
            if x < 0 or i + k > n: return False  # 无法操作
            sum_d -= x  # 直接加到 sum_d 中
            d[i + k] += x
        return True
```

```java [sol-Java]
class Solution {
    public boolean checkArray(int[] nums, int k) {
        int n = nums.length, sumD = 0;
        var d = new int[n + 1];
        for (int i = 0; i < n; i++) {
            sumD += d[i];
            int x = nums[i];
            x += sumD;
            if (x == 0) continue; // 无需操作
            if (x < 0 || i + k > n) return false; // 无法操作
            sumD -= x; // 直接加到 sumD 中
            d[i + k] += x;
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool checkArray(vector<int> &nums, int k) {
        int n = nums.size(), sum_d = 0;
        vector<int> d(n + 1);
        for (int i = 0; i < n; i++) {
            sum_d += d[i];
            int x = nums[i];
            x += sum_d;
            if (x == 0) continue; // 无需操作
            if (x < 0 || i + k > n) return false; // 无法操作
            sum_d -= x; // 直接加到 sum_d 中
            d[i + k] += x;
        }
        return true;
    }
};
```

```go [sol-Go]
func checkArray(nums []int, k int) bool {
	n := len(nums)
	d := make([]int, n+1)
	sumD := 0
	for i, x := range nums {
		sumD += d[i]
		x += sumD
		if x == 0 { // 无需操作
			continue
		}
		if x < 0 || i+k > n { // 无法操作
			return false
		}
		sumD -= x // 直接加到 sumD 中
		d[i+k] += x
	}
	return true
}
```

```js [sol-JavaScript]
var checkArray = function (nums, k) {
    const n = nums.length;
    let d = new Array(n + 1).fill(0);
    let sumD = 0;
    for (let i = 0; i < n; i++) {
        sumD += d[i];
        let x = nums[i];
        x += sumD;
        if (x == 0) continue; // 无需操作
        if (x < 0 || i + k > n) return false; // 无法操作
        sumD -= x; // 直接加到 sumD 中
        d[i + k] += x;
    }
    return true;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

#### 相似题目

- [2528. 最大化城市的最小供电站数目](https://leetcode.cn/problems/maximize-the-minimum-powered-city/)

## 本地原创解析

### 1. 题意重述

本题来自 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
