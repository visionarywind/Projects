# 3834. 合并相邻且相等的元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-adjacent-equal-elements/
- 题目 slug：`merge-adjacent-equal-elements`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.3 邻项消除
- 难度分：1429
- 外部题解来源：https://leetcode.cn/problems/merge-adjacent-equal-elements/solutions/3898725/yong-zhan-mo-ni-pythonjavacgo-by-endless-vi8x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用栈模拟，两种写法（Python/Java/C++/Go）](https://leetcode.cn/problems/merge-adjacent-equal-elements/solutions/3898725/yong-zhan-mo-ni-pythonjavacgo-by-endless-vi8x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-zhan-mo-ni-pythonjavacgo-by-endless-vi8x`
- topic id：`3898725`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于每次操作的都是最左边的相邻相等元素，我们可以从左到右遍历 $\textit{nums}$，一旦发现 $\textit{nums}[i]$ 和左侧剩余最近元素相等，就执行合并操作。如果合并后，新元素与左侧剩余最近元素相等，就继续执行合并操作。

如何快速找到左侧剩余最近元素、删除（合并）左侧剩余最近元素？

我们需要一个后进先出的数据结构——栈，模拟上述过程。

[本题视频讲解](https://www.bilibili.com/video/BV1idFoz3Efi/)，欢迎点赞关注~

## 写法一：先入栈，再出栈

```py [sol-Python3]
class Solution:
    def mergeAdjacent(self, nums: List[int]) -> List[int]:
        st = []
        for x in nums:
            st.append(x)
            while len(st) > 1 and st[-1] == st[-2]:
                st.pop()
                st[-1] *= 2
        return st
```

```java [sol-Java]
class Solution {
    public List<Long> mergeAdjacent(int[] nums) {
        List<Long> st = new ArrayList<>();
        for (int x : nums) {
            st.add((long) x);
            while (st.size() > 1 && st.getLast().equals(st.get(st.size() - 2))) {
                st.removeLast();
                int i = st.size() - 1;
                st.set(i, st.get(i) * 2);
            }
        }
        return st;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> mergeAdjacent(vector<int>& nums) {
        vector<long long> st;
        for (int x : nums) {
            st.push_back(x);
            while (st.size() > 1 && st.back() == st[st.size() - 2]) {
                st.pop_back();
                st.back() *= 2;
            }
        }
        return st;
    }
};
```

```go [sol-Go]
func mergeAdjacent(nums []int) []int64 {
	st := nums[:0] // 原地
	for _, x := range nums {
		st = append(st, x)
		for len(st) > 1 && st[len(st)-1] == st[len(st)-2] {
			st = st[:len(st)-1]
			st[len(st)-1] *= 2
		}
	}
	// 力扣的 int 就是 int64，直接 O(1) 转成 []int64
	return *(*[]int64)(unsafe.Pointer(&st))
}
```

## 写法二：先出栈，再入栈

```py [sol-Python3]
class Solution:
    def mergeAdjacent(self, nums: List[int]) -> List[int]:
        st = []
        for x in nums:
            while st and st[-1] == x:
                st.pop()
                x *= 2
            st.append(x)
        return st
```

```java [sol-Java]
class Solution {
    public List<Long> mergeAdjacent(int[] nums) {
        List<Long> st = new ArrayList<>();
        for (long x : nums) {
            while (!st.isEmpty() && st.getLast() == x) {
                st.removeLast();
                x *= 2;
            }
            st.add(x);
        }
        return st;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> mergeAdjacent(vector<int>& nums) {
        vector<long long> st;
        for (long long x : nums) {
            while (!st.empty() && st.back() == x) {
                st.pop_back();
                x *= 2;
            }
            st.push_back(x);
        }
        return st;
    }
};
```

```go [sol-Go]
func mergeAdjacent(nums []int) []int64 {
	st := nums[:0] // 原地
	for _, x := range nums {
		for len(st) > 0 && st[len(st)-1] == x {
			st = st[:len(st)-1]
			x *= 2
		}
		st = append(st, x)
	}
	// 力扣的 int 就是 int64，直接 O(1) 转成 []int64
	return *(*[]int64)(unsafe.Pointer(&st))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。部分语言可以直接把 $\textit{nums}$ 当作栈，从而做到 $\mathcal{O}(1)$ 空间。

## 专题训练

见下面数据结构题单的「**§3.3 邻项消除**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.3 邻项消除`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.3 邻项消除`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
