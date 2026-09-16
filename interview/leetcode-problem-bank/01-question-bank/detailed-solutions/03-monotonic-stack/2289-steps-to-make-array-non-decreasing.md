# 2289. 使数组按非递减顺序排列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/steps-to-make-array-non-decreasing/
- 题目 slug：`steps-to-make-array-non-decreasing`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.2 进阶
- 难度分：2482
- 外部题解来源：https://leetcode.cn/problems/steps-to-make-array-non-decreasing/solutions/1524614/by-endlesscheng-s2yc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[等价转换 + 利用单调性（Python/Java/C++/Go）](https://leetcode.cn/problems/steps-to-make-array-non-decreasing/solutions/1524614/by-endlesscheng-s2yc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-s2yc`
- topic id：`1524614`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1iF41157dG/) 已出炉，欢迎点赞三连~

---

#### 提示 1

元素 $x$ 会被左边某个比他大的元素 $y$ 给删除（如果存在的话）。

我们需要计算在删除 $x$ 之前，删除了多少个比 $y$ 小的元素，从而算出删除 $x$ 的时刻（第几步操作）。

答案可以转换成所有（能被删除的）元素被删除的时刻的最大值。

#### 提示 2

以 $[20,1,9,1,2,3]$ 为例。

- 时刻一 $20$ 删掉 $1$，$9$ 删掉 $1$；
- 时刻二 $20$ 删掉 $9$，$9$ 删掉 $2$;
- 时刻三 $20$ 接替了 $9$ 的任务，来删除数字 $3$。

虽然说数字 $3$ 是被 $20$ 删除的，但是由于 $20$ 立马接替了 $9$，我们可以**等价转换**看作 $3$ 是被 $9$ 删除的，也就是它左边离它最近且比它大的那个数。

**该等价转换不会影响数字被删除的时刻。**

#### 提示 3

再考虑这个例子 $[9,1,2,3,4,1,5]$。

$5$ 应该被 $9$ 删除。根据题目要求，在删除 $5$ 之前，需要把 $5$ 前面不超过 $5$ 的元素都删除，然后才能删除 $5$。所以在删除 $5$ 之前，我们需要知道 $9$ 到 $5$ 之间的所有元素被删除的时刻的最大值，这个时刻加一就是删除 $5$ 的时刻。

这可以用单调栈 + 线段树来做，单调栈求左边最近更大元素位置，线段树维护区间最大值。（[评论区](https://leetcode.cn/problems/steps-to-make-array-non-decreasing/comments/1587279)有人实现了这一思路）

但还有更巧妙的做法。

#### 提示 4

对于一串非降的序列，该序列的每个元素被删除的时刻是单调递增的。（假设序列左侧有个更大的元素去删除序列中的元素）

利用这一单调性，我们只需要存储这串非降序列的**最后一个元素**被删除的时刻。

某一段区间会包含若干个非降序列，也就包含了若干个最后一个元素被删除的时刻，提示 3 中所需要计算的最大值必然在这些时刻中。

#### 提示 5

我们可以用一个**单调递减栈**存储元素及其被删除的时刻，当遇到一个不小于栈顶的元素 $x$ 时，就不断弹出栈顶元素，并取弹出元素被删除时刻的最大值，这样就得到了提示 3 中所需要计算的时刻的最大值 $\textit{maxT}$。

然后将 $x$ 及 $\textit{maxT}+1$ 入栈。注意如果此时栈为空，说明前面没有比 $x$ 大的元素，$x$ 无法被删除，即 $\textit{maxT}=0$，这种情况需要将 $x$ 及 $0$ 入栈。

#### 复杂度分析

- 时间复杂度：$O(n)$。每个元素至多入栈出栈各一次。
- 空间复杂度：$O(n)$。最坏情况下栈中有 $n$ 个元素。

```Python [sol1-Python3]
class Solution:
    def totalSteps(self, nums: List[int]) -> int:
        ans, st = 0, []
        for num in nums:
            max_t = 0
            while st and st[-1][0] <= num:
                max_t = max(max_t, st.pop()[1])
            max_t = max_t + 1 if st else 0
            ans = max(ans, max_t)
            st.append((num, max_t))
        return ans
```

```java [sol1-Java]
class Solution {
    public int totalSteps(int[] nums) {
        var ans = 0;
        var st = new ArrayDeque<int[]>();
        for (var num : nums) {
            var maxT = 0;
            while (!st.isEmpty() && st.peek()[0] <= num)
                maxT = Math.max(maxT, st.pop()[1]);
            maxT = st.isEmpty() ? 0 : maxT + 1;
            ans = Math.max(ans, maxT);
            st.push(new int[]{num, maxT});
        }
        return ans;
    }
}
```

```C++ [sol1-C++]
class Solution {
public:
    int totalSteps(vector<int> &nums) {
        int ans = 0;
        stack<pair<int, int>> st;
        for (int num : nums) {
            int maxT = 0;
            while (!st.empty() && st.top().first <= num) {
                maxT = max(maxT, st.top().second);
                st.pop();
            }
            maxT = st.empty() ? 0 : maxT + 1;
            ans = max(ans, maxT);
            st.emplace(num, maxT);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func totalSteps(nums []int) (ans int) {
	type pair struct{ v, t int }
	st := []pair{}
	for _, num := range nums {
		maxT := 0
		for len(st) > 0 && st[len(st)-1].v <= num {
			maxT = max(maxT, st[len(st)-1].t)
			st = st[:len(st)-1]
		}
		if len(st) > 0 {
			maxT++
			ans = max(ans, maxT)
		} else {
			maxT = 0
		}
		st = append(st, pair{num, maxT})
	}
	return
}

func max(a, b int) int { if b > a { return b }; return a }
```

## 本地原创解析

### 1. 题意重述

本题来自 `一、单调栈 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、单调栈 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
