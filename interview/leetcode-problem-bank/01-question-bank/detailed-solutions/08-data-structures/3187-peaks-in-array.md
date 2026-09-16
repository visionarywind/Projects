# 3187. 数组中的峰值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/peaks-in-array/
- 题目 slug：`peaks-in-array`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.1 树状数组
- 难度分：2154
- 外部题解来源：https://leetcode.cn/problems/peaks-in-array/solutions/2812394/shu-zhuang-shu-zu-pythonjavacgo-by-endle-tj0w/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树状数组（Python/Java/C++/Go）](https://leetcode.cn/problems/peaks-in-array/solutions/2812394/shu-zhuang-shu-zu-pythonjavacgo-by-endle-tj0w/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-zhuang-shu-zu-pythonjavacgo-by-endle-tj0w`
- topic id：`2812394`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

为方便描述，把 $\textit{nums}$ 记作 $a$。

如果 $1\le i \le n-2$ 且 $a[i-1] < a[i] > a[i+1]$，那么把 $a[i]$ 视作 $1$，否则视作 $0$。

如此转换后，操作 1 相当于计算从 $l+1$ 到 $r-1$ 的子数组的元素和。请注意，题目说的是「子数组」的第一个和最后一个不是峰值元素，注意是子数组，不是整个数组。

由于操作 2 要动态修改数组，我们可以用**树状数组**维护，具体请看 [带你发明树状数组](https://leetcode.cn/problems/range-sum-query-mutable/solution/dai-ni-fa-ming-shu-zhuang-shu-zu-fu-shu-lyfll/)。

具体来说：

1. 先把区间 $[\max(i-1,1),\min(i+1,n-2)]$ 中的峰值元素从树状数组中去掉。
2. 然后修改 $a[i]=\textit{val}$。
3. 最后再把区间 $[\max(i-1,1),\min(i+1,n-2)]$ 中的峰值元素加入树状数组。

[本题视频讲解](https://www.bilibili.com/video/BV1T1421k7Hi/) 第四题，欢迎点赞关注！

```py [sol-Python3]
class Fenwick:
    __slots__ = 'f'

    def __init__(self, n: int):
        self.f = [0] * n

    def update(self, i: int, val: int) -> None:
        while i < len(self.f):
            self.f[i] += val
            i += i & -i

    def pre(self, i: int) -> int:
        res = 0
        while i > 0:
            res += self.f[i]
            i &= i - 1
        return res

    def query(self, l: int, r: int) -> int:
        if r < l:
            return 0
        return self.pre(r) - self.pre(l - 1)

class Solution:
    def countOfPeaks(self, nums, queries):
        n = len(nums)
        f = Fenwick(n - 1)
        def update(i: int, val: int) -> None:
            if nums[i - 1] < nums[i] and nums[i] > nums[i + 1]:
                f.update(i, val)
        for i in range(1, n - 1):
            update(i, 1)

        ans = []
        for op, i, val in queries:
            if op == 1:
                ans.append(f.query(i + 1, val - 1))
                continue
            for j in range(max(i - 1, 1), min(i + 2, n - 1)):
                update(j, -1)
            nums[i] = val
            for j in range(max(i - 1, 1), min(i + 2, n - 1)):
                update(j, 1)
        return ans
```

```java [sol-Java]
class Fenwick {
    private final int[] f;

    Fenwick(int n) {
        f = new int[n];
    }

    void update(int i, int val) {
        for (; i < f.length; i += i & -i) {
            f[i] += val;
        }
    }

    private int pre(int i) {
        int res = 0;
        for (; i > 0; i &= i - 1) {
            res += f[i];
        }
        return res;
    }

    int query(int l, int r) {
        if (r < l) {
            return 0;
        }
        return pre(r) - pre(l - 1);
    }
}

class Solution {
    public List<Integer> countOfPeaks(int[] nums, int[][] queries) {
        int n = nums.length;
        Fenwick f = new Fenwick(n - 1);
        for (int i = 1; i < n - 1; i++) {
            update(f, nums, i, 1);
        }

        List<Integer> ans = new ArrayList<>();
        for (int[] q : queries) {
            if (q[0] == 1) {
                ans.add(f.query(q[1] + 1, q[2] - 1));
                continue;
            }
            int i = q[1];
            for (int j = Math.max(i - 1, 1); j <= Math.min(i + 1, n - 2); j++) {
                update(f, nums, j, -1);
            }
            nums[i] = q[2];
            for (int j = Math.max(i - 1, 1); j <= Math.min(i + 1, n - 2); j++) {
                update(f, nums, j, 1);
            }
        }
        return ans;
    }

    private void update(Fenwick f, int[] nums, int i, int val) {
        if (nums[i - 1] < nums[i] && nums[i] > nums[i + 1]) {
            f.update(i, val);
        }
    }
}
```

```cpp [sol-C++]
class Fenwick {
    vector<int> f;

public:
    Fenwick(int n) : f(n) {}

    void update(int i, int val) {
        for (; i < f.size(); i += i & -i) {
            f[i] += val;
        }
    }

    int pre(int i) {
        int res = 0;
        for (; i > 0; i &= i - 1) {
            res += f[i];
        }
        return res;
    }

    int query(int l, int r) {
        if (r < l) {
            return 0;
        }
        return pre(r) - pre(l - 1);
    }
};

class Solution {
public:
    vector<int> countOfPeaks(vector<int>& nums, vector<vector<int>>& queries) {
        int n = nums.size();
        Fenwick f(n - 1);
        auto update = [&](int i, int val) {
            if (nums[i - 1] < nums[i] && nums[i] > nums[i + 1]) {
                f.update(i, val);
            }
        };
        for (int i = 1; i < n - 1; i++) {
            update(i, 1);
        }

        vector<int> ans;
        for (auto& q : queries) {
            if (q[0] == 1) {
                ans.push_back(f.query(q[1] + 1, q[2] - 1));
                continue;
            }
            int i = q[1];
            for (int j = max(i - 1, 1); j <= min(i + 1, n - 2); j++) {
                update(j, -1);
            }
            nums[i] = q[2];
            for (int j = max(i - 1, 1); j <= min(i + 1, n - 2); j++) {
                update(j, 1);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type fenwick []int

func (f fenwick) update(i, val int) {
	for ; i < len(f); i += i & -i {
		f[i] += val
	}
}

func (f fenwick) pre(i int) (res int) {
	for ; i > 0; i &= i - 1 {
		res += f[i]
	}
	return res
}

func (f fenwick) query(l, r int) int {
	if r < l {
		return 0
	}
	return f.pre(r) - f.pre(l-1)
}

func countOfPeaks(nums []int, queries [][]int) (ans []int) {
	n := len(nums)
	f := make(fenwick, n-1)
	update := func(i, val int) {
		if nums[i-1] < nums[i] && nums[i] > nums[i+1] {
			f.update(i, val)
		}
	}
	for i := 1; i < n-1; i++ {
		update(i, 1)
	}

	for _, q := range queries {
		if q[0] == 1 {
			ans = append(ans, f.query(q[1]+1, q[2]-1))
			continue
		}
		i := q[1]
		for j := max(i-1, 1); j <= min(i+1, n-2); j++ {
			update(j, -1)
		}
		nums[i] = q[2]
		for j := max(i-1, 1); j <= min(i+1, n-2); j++ {
			update(j, 1)
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+q)\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

**注**：可以用 [带你发明树状数组](https://leetcode.cn/problems/range-sum-query-mutable/solution/dai-ni-fa-ming-shu-zhuang-shu-zu-fu-shu-lyfll/) 中的技巧，$\mathcal{O}(n)$ 初始化树状数组，从而做到 $\mathcal{O}(n + q\log n)$ 的时间复杂度。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.1 树状数组`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.1 树状数组`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
