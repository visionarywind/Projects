# 1144. 递减元素使数组呈锯齿状

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/decrease-elements-to-make-array-zigzag/
- 题目 slug：`decrease-elements-to-make-array-zigzag`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1559
- 外部题解来源：https://leetcode.cn/problems/decrease-elements-to-make-array-zigzag/solutions/2135375/mei-you-si-lu-yi-bu-bu-ti-shi-ni-si-kao-cm0h2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没有思路？一步步提示你思考！（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/decrease-elements-to-make-array-zigzag/solutions/2135375/mei-you-si-lu-yi-bu-bu-ti-shi-ni-si-kao-cm0h2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-you-si-lu-yi-bu-bu-ti-shi-ni-si-kao-cm0h2`
- topic id：`2135375`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

### 提示 1

只有减少操作意味着什么？

如果你想把 $x\le y$ 改成 $x>y$ 的话，减少 $x$ 是毫无意义的，只能减少 $y$。

### 提示 2

题目给了两种锯齿数组，分别考虑。

### 提示 3-1

以第一种为例，要想转换成 $A[0] > A[1] < A[2] > A[3] < A[4] > \cdots$，根据提示 1，哪些数**一定不需要**减少？

### 提示 3-2

$\textit{nums}[0],\textit{nums}[2],\cdots$ 是一定不需要的。

你也可以这样思考：

假设把 $\textit{nums}$ 转换成第一种锯齿形，且 $\textit{nums}[2]$ 减少了，那么把 $\textit{nums}[2]$ 恢复成它的原始值，数组仍然符合第一种锯齿形。所以 $\textit{nums}[2]$ 是不需要减少的。其它数同理。

### 提示 4

为了使操作次数尽量少，$\textit{nums}[i]$ 不断减小到要比左右相邻数字都小，就立刻停止。所以 $\textit{nums}[i]$ 要修改成 $m=\min(\textit{nums}[i-1],\textit{nums}[i+1])-1$，修改次数为 $\textit{nums}[i]-m$，如果 $\textit{nums}[i]$ 本来就不超过 $m$，就无需修改。

因此，$\textit{nums}[i]$ 的修改次数为

$$
\max(\textit{nums}[i]-\min(\textit{nums}[i-1],\textit{nums}[i+1])+1, 0)
$$

如果 $i-1$ 或者 $i+1$ 下标越界，则对应的数字视作无穷大。

最后，把偶数和奇数下标对应的修改次数分别累加，结果分别设为 $s_0$ 和 $s_1$，那么答案就是 $\min(s_0,s_1)$。

```py [sol1-Python3]
class Solution:
    def movesToMakeZigzag(self, nums):
        s = [0] * 2
        for i, x in enumerate(nums):
            left = nums[i - 1] if i else inf
            right = nums[i + 1] if i < len(nums) - 1 else inf
            s[i % 2] += max(x - min(left, right) + 1, 0)
        return min(s)
```

```java [sol1-Java]
class Solution {
    public int movesToMakeZigzag(int[] nums) {
        int[] s = new int[2];
        for (int i = 0, n = nums.length; i < n; ++i) {
            int left = i > 0 ? nums[i - 1] : Integer.MAX_VALUE;
            int right = i < n - 1 ? nums[i + 1] : Integer.MAX_VALUE;
            s[i % 2] += Math.max(nums[i] - Math.min(left, right) + 1, 0);
        }
        return Math.min(s[0], s[1]);
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int movesToMakeZigzag(vector<int> &nums) {
        int s[2]{}, n = nums.size();
        for (int i = 0; i < n; ++i) {
            int left = i ? nums[i - 1] : INT_MAX;
            int right = i < n - 1 ? nums[i + 1] : INT_MAX;
            s[i % 2] += max(nums[i] - min(left, right) + 1, 0);
        }
        return min(s[0], s[1]);
    }
};
```

```go [sol1-Go]
func movesToMakeZigzag(nums []int) int {
    s := [2]int{}
    for i, x := range nums {
        left, right := math.MaxInt, math.MaxInt
        if i > 0 {
            left = nums[i-1]
        }
        if i < len(nums)-1 {
            right = nums[i+1]
        }
        s[i%2] += max(x-min(left, right)+1, 0)
    }
    return min(s[0], s[1])
}

func min(a, b int) int { if a > b { return b }; return a }
func max(a, b int) int { if a < b { return b }; return a }
```

```js [sol1-JavaScript]
var movesToMakeZigzag = function (nums) {
    const n = nums.length
    let s = [0, 0];
    for (let i = 0; i < n; i++) {
        const left = i ? nums[i - 1] : Number.MAX_SAFE_INTEGER;
        const right = i < n - 1 ? nums[i + 1] : Number.MAX_SAFE_INTEGER;
        s[i % 2] += Math.max(nums[i] - Math.min(left, right) + 1, 0);
    }
    return Math.min(s[0], s[1]);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

[往期每日一题题解（按 tag 分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
