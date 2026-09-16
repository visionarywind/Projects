# 2078. 两栋颜色不同且距离最远的房子

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/two-furthest-houses-with-different-colors/
- 题目 slug：`two-furthest-houses-with-different-colors`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/two-furthest-houses-with-different-colors/solutions/1113521/on-zuo-fa-by-endlesscheng-an8b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 做法，脑筋急转弯（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/two-furthest-houses-with-different-colors/solutions/1113521/on-zuo-fa-by-endlesscheng-an8b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-zuo-fa-by-endlesscheng-an8b`
- topic id：`1113521`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

如果 $\textit{colors}[0] \ne \textit{colors}[n-1]$，那么答案是 $n-1$。

否则设 $c = \textit{colors}[0] = \textit{colors}[n-1]$。

设最大距离来自房子 $i$ 和房子 $j$。由于题目要求 $\textit{colors}[i]\ne \textit{colors}[j]$，所以这两个颜色不可能都等于 $c$。如果 $\textit{colors}[i]\ne c$，那么 $j$ 必然是离 $i$ 最远的 $0$ 或者 $n-1$（这两栋房子的颜色与房子 $i$ 不同）。这意味着，$0$ 或者 $n-1$ **必然参与最大距离的计算**。

- 对于房子 $0$ 来说，另一栋房子越远（越靠右）越好。从右往左找到颜色不等于 $c$ 的房子 $\textit{colors}[r]$，距离为 $r-0 = r$。
- 对于房子 $n-1$ 来说，另一栋房子越远（越靠左）越好。从左往右找到颜色不等于 $c$ 的房子 $\textit{colors}[\ell]$，距离为 $n-1-\ell$。

答案为二者的最大值

$$
\max(r, n-1-\ell)
$$

> 注意题目保证至少有两栋颜色不同的房子。

```py [sol-Python3]
class Solution:
    def maxDistance(self, colors: List[int]) -> int:
        n = len(colors)
        c = colors[0]
        if c != colors[-1]:
            return n - 1

        # 找最右边的颜色不等于 c 的房子
        # 题目保证至少有两栋颜色不同的房子
        r = n - 2
        while colors[r] == c:
            r -= 1

        # 找最左边的颜色不等于 c 的房子
        l = 1
        while colors[l] == c:
            l += 1

        return max(r, n - 1 - l)
```

```java [sol-Java]
class Solution {
    public int maxDistance(int[] colors) {
        int n = colors.length;
        int c = colors[0];
        if (c != colors[n - 1]) {
            return n - 1;
        }

        // 找最右边的颜色不等于 c 的房子
        // 题目保证至少有两栋颜色不同的房子
        int r = n - 2;
        while (colors[r] == c) {
            r--;
        }

        // 找最左边的颜色不等于 c 的房子
        int l = 1;
        while (colors[l] == c) {
            l++;
        }

        return Math.max(r, n - 1 - l);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxDistance(vector<int>& colors) {
        int n = colors.size();
        int c = colors[0];
        if (c != colors[n - 1]) {
            return n - 1;
        }

        // 找最右边的颜色不等于 c 的房子
        // 题目保证至少有两栋颜色不同的房子
        int r = n - 2;
        while (colors[r] == c) {
            r--;
        }

        // 找最左边的颜色不等于 c 的房子
        int l = 1;
        while (colors[l] == c) {
            l++;
        }

        return max(r, n - 1 - l);
    }
};
```

```c [sol-C]
int maxDistance(int* colors, int colorsSize) {
    int n = colorsSize;
    int c = colors[0];
    if (c != colors[n - 1]) {
        return n - 1;
    }

    // 找最右边的颜色不等于 c 的房子
    // 题目保证至少有两栋颜色不同的房子
    int r = n - 2;
    while (colors[r] == c) {
        r--;
    }

    // 找最左边的颜色不等于 c 的房子
    int l = 1;
    while (colors[l] == c) {
        l++;
    }

    return MAX(r, n - 1 - l);
}
```

```go [sol-Go]
func maxDistance(colors []int) int {
	n := len(colors)
	c := colors[0]
	if c != colors[n-1] {
		return n - 1
	}

	// 找最右边的颜色不等于 c 的房子
	// 题目保证至少有两栋颜色不同的房子
	r := n - 2
	for colors[r] == c {
		r--
	}

	// 找最左边的颜色不等于 c 的房子
	l := 1
	for colors[l] == c {
		l++
	}

	return max(r, n-1-l)
}
```

```js [sol-JavaScript]
var maxDistance = function(colors) {
    const n = colors.length;
    const c = colors[0];
    if (c !== colors[n - 1]) {
        return n - 1;
    }

    // 找最右边的颜色不等于 c 的房子
    // 题目保证至少有两栋颜色不同的房子
    let r = n - 2;
    while (colors[r] === c) {
        r--;
    }

    // 找最左边的颜色不等于 c 的房子
    let l = 1;
    while (colors[l] === c) {
        l++;
    }

    return Math.max(r, n - 1 - l);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_distance(colors: Vec<i32>) -> i32 {
        let n = colors.len();
        let c = colors[0];
        if c != colors[n - 1] {
            return (n - 1) as _;
        }

        // 找最右边的颜色不等于 c 的房子
        // 题目保证至少有两栋颜色不同的房子
        let mut r = n - 2;
        while colors[r] == c {
            r -= 1;
        }

        // 找最左边的颜色不等于 c 的房子
        let mut l = 1;
        while colors[l] == c {
            l += 1;
        }

        r.max(n - 1 - l) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{colors}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心与思维题单的「**§5.2 脑筋急转弯**」。

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

本题来自 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
