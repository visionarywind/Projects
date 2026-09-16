# 3542. 将所有元素变为 0 的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-convert-all-elements-to-zero/
- 题目 slug：`minimum-operations-to-convert-all-elements-to-zero`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.2 进阶
- 难度分：1890
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-convert-all-elements-to-zero/solutions/3673804/cong-fen-zhi-dao-dan-diao-zhan-jian-ji-x-mzbl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从分治到单调栈，简洁写法（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-convert-all-elements-to-zero/solutions/3673804/cong-fen-zhi-dao-dan-diao-zhan-jian-ji-x-mzbl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-fen-zhi-dao-dan-diao-zhan-jian-ji-x-mzbl`
- topic id：`3673804`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 分治

回顾示例 3 $\textit{nums}=[1,2,1,2,1,2]$ 的操作过程：

- 首先，只需要一次操作（选择整个数组），就可以把所有的最小值 $1$ 都变成 $0$。现在数组是 $[0,2,0,2,0,2]$。
- 这些被 $0$ 分割开的 $2$，无法合在一起操作（因为子数组会包含 $0$，导致 $2$ 无法变成 $0$），只能一个一个操作。

一般地：

1. 先通过一次操作，把 $\textit{nums}$ 的最小值都变成 $0$（如果最小值已经是 $0$ 则跳过这步）。
2. 此时 $\textit{nums}$ 被这些 $0$ 划分成了若干段，后续操作只能在每段内部，不能跨段操作（否则子数组会包含 $0$）。每一段是规模更小的子问题，可以用第一步的方法解决。这样我们可以写一个递归去处理。递归边界：如果操作后全为 $0$，直接返回。

找最小值可以用 ST 表或者线段树，但这种做法很麻烦。有没有简单的做法呢？

## 单调栈

从左往右遍历数组，只在「必须要操作」的时候，才把答案加一。

什么时候必须要操作？

示例 3 $\textit{nums}=[1,2,1,2,1,2]$，因为 $2$ 左右两侧都有小于 $2$ 的数，需要单独操作。

又例如 $\textit{nums}=[1,2,3,2,1]$：

- 遍历到**第二个** $2$ 时，可以知道 $3$ 左右两侧都有小于 $3$ 的数，所以 $3$ 必须要操作一次，答案加一。注意这不表示第一次操作的是 $3$，而是某次操作会把 $3$ 变成 $0$。
- 遍历到末尾 $1$ 时，可以知道中间的两个 $2$，左边有 $1$，右边也有 $1$，必须操作一次，答案加一。比如选择 $[2,3,2]$ 可以把这两个 $2$ 都变成 $0$。
- 最后，数组中的 $1$ 需要操作一次都变成 $0$。

我们怎么知道「$3$ 左右两侧都有小于 $3$ 的数」？

遍历数组的同时，把遍历过的元素用栈记录：

- 如果当前元素比栈顶大（或者栈为空），那么直接入栈。
- 如果当前元素比栈顶小，那么对于栈顶来说，左边（栈顶倒数第二个数）比栈顶小（原因后面解释），右边（当前元素）也比栈顶小，所以栈顶必须操作一次。然后弹出栈顶。
- 如果当前元素等于栈顶，可以在同一次操作中把当前元素与栈顶都变成 $0$，所以无需入栈。注意这保证了栈中没有重复元素。

如果当前元素比栈顶小，就弹出栈顶，我们会得到一个底小顶大的单调栈，这就保证了「对于栈顶来说，左边（栈顶倒数第二个数）比栈顶小」。

遍历结束后，因为栈是严格递增的，所以栈中每个非零数字都需要操作一次。

代码实现时，可以直接把 $\textit{nums}$ 当作栈。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1m7EuzqEqr/?t=2m47s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minOperations(self, nums: List[int]) -> int:
        ans = 0
        st = []
        for x in nums:
            while st and x < st[-1]:
                st.pop()
                ans += 1
            # 如果 x 与栈顶相同，那么 x 与栈顶可以在同一次操作中都变成 0，x 无需入栈
            if not st or x != st[-1]:
                st.append(x)
        return ans + len(st) - (st[0] == 0)  # 0 不需要操作
```

```py [sol-Python3 原地]
class Solution:
    def minOperations(self, nums: List[int]) -> int:
        ans = 0
        top = -1  # 栈顶下标（把 nums 当作栈）
        for x in nums:
            while top >= 0 and x < nums[top]:
                top -= 1  # 出栈
                ans += 1
            # 如果 x 与栈顶相同，那么 x 与栈顶可以在同一次操作中都变成 0，x 无需入栈
            if top < 0 or x != nums[top]:
                top += 1
                nums[top] = x  # 入栈
        return ans + top + (nums[0] > 0)
```

```java [sol-Java]
class Solution {
    public int minOperations(int[] nums) {
        int ans = 0;
        int top = -1; // 栈顶下标（把 nums 当作栈）
        for (int x : nums) {
            while (top >= 0 && x < nums[top]) {
                top--; // 出栈
                ans++;
            }
            // 如果 x 与栈顶相同，那么 x 与栈顶可以在同一次操作中都变成 0，x 无需入栈
            if (top < 0 || x != nums[top]) {
                nums[++top] = x; // 入栈
            }
        }
        return ans + top + (nums[0] > 0 ? 1 : 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(vector<int>& nums) {
        int ans = 0;
        int top = -1; // 栈顶下标（把 nums 当作栈）
        for (int x : nums) {
            while (top >= 0 && x < nums[top]) {
                top--; // 出栈
                ans++;
            }
            // 如果 x 与栈顶相同，那么 x 与栈顶可以在同一次操作中都变成 0，x 无需入栈
            if (top < 0 || x != nums[top]) {
                nums[++top] = x; // 入栈
            }
        }
        return ans + top + (nums[0] > 0);
    }
};
```

```go [sol-Go]
func minOperations(nums []int) (ans int) {
	st := nums[:0] // 原地
	for _, x := range nums {
		for len(st) > 0 && x < st[len(st)-1] {
			st = st[:len(st)-1]
			ans++
		}
		// 如果 x 与栈顶相同，那么 x 与栈顶可以在同一次操作中都变成 0，x 无需入栈
		if len(st) == 0 || x != st[len(st)-1] {
			st = append(st, x)
		}
	}
	if st[0] == 0 { // 0 不需要操作
		ans--
	}
	return ans + len(st)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。每个元素至多入栈出栈各一次，所以二重循环的循环次数是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。原地做法可以做到 $\mathcal{O}(1)$ 空间。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
