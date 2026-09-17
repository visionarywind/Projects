# 3017. 按距离统计房屋对数目 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-the-number-of-houses-at-a-certain-distance-ii/
- 题目 slug：`count-the-number-of-houses-at-a-certain-distance-ii`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：2709
- 外部题解来源：https://leetcode.cn/problems/count-the-number-of-houses-at-a-certain-distance-ii/solutions/2613373/yong-che-xiao-de-fang-shi-si-kao-pythonj-o253/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种分类讨论思路（Python/Java/C++/Go）](https://leetcode.cn/problems/count-the-number-of-houses-at-a-certain-distance-ii/solutions/2613373/yong-che-xiao-de-fang-shi-si-kao-pythonj-o253/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-che-xiao-de-fang-shi-si-kao-pythonj-o253`
- topic id：`2613373`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

[视频讲解](https://www.bilibili.com/video/BV1Q5411C7mN/) 第四题。

## 前置知识：差分数组

请看[【模板讲解】差分数组](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)

## 方法一：分类讨论+撤销操作

首先，如果没有额外加一条在 $x$ 和 $y$ 之间的边，那么对于房子 $i$：

- 它到它左侧房子的最短距离分别是 $1,2,3,\cdots, i-1$，用差分数组把 $[1,i-1]$ 都加一。
- 它到它右侧房子的最短距离分别是 $1,2,3,\cdots, n-i$，用差分数组把 $[1,n-i]$ 都加一。

然后考虑 $x$ 和 $y$ 加的这条边带来的影响。假设 $x\le y$，如果 $x+1 \ge y$，那么这条边不影响最短距离，否则分类讨论：

- 如果 $i\le x$，那么：
   - 对于编号为 $[y,n]$ 的房子，我们可以通过 $x$ 到 $y$ 更快地到达，原来 $i$ 到这些房子的距离是 $y-i,y-i+1,\cdots, n-i$，所以我们撤销原来对区间 $[y-i,n-i]$ 的加一，也就是用差分数组把 $[y-i,n-i]$ 都减一，然后把区间 $[y-i-\textit{dec}, n-i-\textit{dec}]$ 都加一，其中 $\textit{dec} = y-x-1$ 表示缩短的距离。
   - 对于在 $x$ 和 $y$ 之间的编号 $j$，如果走 $x$ 到 $y$ 的距离更短，即 $x-i + 1 + y-j < j-i$，解得 $j > (x+y+1)/2$，那么从 $j=(x+y+1)/2+1$ 到 $y-1$ 都可以变短，所以和上面一样，先撤销，用差分数组把 $[j-i, y-1-i]$ 都减一，然后把区间 $[x-i+2, x-i+y-j+1]$ 都加一。为什么是 $[x-i+2, x-i+y-j+1]$？因为 $i$ 到 $y-1$ 的距离是 $x-i+2$（从 $i$ 到 $x$ 到 $y$ 到 $y-1$），所以区间左端点为 $x-i+2$，由于区间长度是 $y-j$，所以区间右端点是 $x-i+y-j+1$。
- 如果 $x < i < \left\lfloor\dfrac{x+y}{2}\right\rfloor$，那么同上：
   - 用差分数组把 $[y-i,n-i]$ 都减一，然后把区间 $[y-i-\textit{dec}, n-i-\textit{dec}]$ 都加一，其中 $\textit{dec} = (y - i) - (i - x + 1)$ 表示缩短的距离。
   - 对于在 $x$ 和 $y$ 之间的编号 $j$，如果走 $x$ 到 $y$ 的距离更短，即 $i-x + 1 + y-j < j-i$，解得 $j >  i + (y-x+1)/2$，那么从 $j=i + (y-x+1)/2+1$ 到 $y-1$ 都可以变短，所以和上面一样，先撤销，用差分数组把 $[j-i, y-1-i]$ 都减一，然后把区间 $[i-x+2, i-x+y-j+1]$ 都加一。
- 对于更大的 $i$，我们可以利用对称性转换成上面两种情况。

```py [sol-Python3]
class Solution:
    def countOfPairs(self, n: int, x: int, y: int) -> List[int]:
        if x > y:
            x, y = y, x

        diff = [0] * (n + 1)

        def add(l: int, r: int, v: int) -> None:
            if l > r: return
            diff[l] += v
            diff[r + 1] -= v

        def update(i: int, x: int, y: int) -> None:
            add(y - i, n - i, -1)  # 撤销 [y,n]
            dec = y - x - 1  # 缩短的距离
            add(y - i - dec, n - i - dec, 1)

            j = (x + y + 1) // 2 + 1
            add(j - i, y - 1 - i, -1)  # 撤销 [j, y-1]
            add(x - i + 2, x - i + y - j + 1, 1)

        def update2(i: int, x: int, y: int) -> None:
            add(y - i, n - i, -1)  # 撤销 [y,n]
            dec = (y - i) - (i - x + 1)  # 缩短的距离
            add(y - i - dec, n - i - dec, 1)

            j = i + (y - x + 1) // 2 + 1
            add(j - i, y - 1 - i, -1)  # 撤销 [j, y-1]
            add(i - x + 2, i - x + y - j + 1, 1)

        for i in range(1, n + 1):
            add(1, i - 1, 1)
            add(1, n - i, 1)
            if x + 1 >= y:
                continue
            if i <= x:
                update(i, x, y)
            elif i >= y:
                update(n + 1 - i, n + 1 - y, n + 1 - x)
            elif i < (x + y) // 2:
                update2(i, x, y)
            elif i > (x + y + 1) // 2:
                update2(n + 1 - i, n + 1 - y, n + 1 - x)

        return list(accumulate(diff))[1:]
```

```java [sol-Java]
class Solution {
    public long[] countOfPairs(int n, int x, int y) {
        if (x > y) {
            int temp = x;
            x = y;
            y = temp;
        }

        diff = new int[n + 1];

        for (int i = 1; i <= n; i++) {
            add(1, i - 1, 1);
            add(1, n - i, 1);
            if (x + 1 >= y) {
                continue;
            }
            if (i <= x) {
                update(i, x, y, n);
            } else if (i >= y) {
                update(n + 1 - i, n + 1 - y, n + 1 - x, n);
            } else if (i < (x + y) / 2) {
                update2(i, x, y, n);
            } else if (i > (x + y + 1) / 2) {
                update2(n + 1 - i, n + 1 - y, n + 1 - x, n);
            }
        }

        long[] ans = new long[n];
        long sumD = 0;
        for (int i = 0; i < n; i++) {
            sumD += diff[i + 1];
            ans[i] = sumD;
        }
        return ans;
    }

    private int[] diff;

    private void add(int l, int r, int v) {
        if (l > r) return;
        diff[l] += v;
        diff[r + 1] -= v;
    }

    private void update(int i, int x, int y, int n) {
        add(y - i, n - i, -1); // 撤销 [y,n]
        int dec = y - x - 1; // 缩短的距离
        add(y - i - dec, n - i - dec, 1);

        int j = (x + y + 1) / 2 + 1;
        add(j - i, y - 1 - i, -1); // 撤销 [j, y-1]
        add(x - i + 2, x - i + y - j + 1, 1);
    }

    private void update2(int i, int x, int y, int n) {
        add(y - i, n - i, -1); // 撤销 [y,n]
        int dec = (y - i) - (i - x + 1); // 缩短的距离
        add(y - i - dec, n - i - dec, 1);

        int j = i + (y - x + 1) / 2 + 1;
        add(j - i, y - 1 - i, -1); // 撤销 [j, y-1]
        add(i - x + 2, i - x + y - j + 1, 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> countOfPairs(int n, int x, int y) {
        if (x > y) {
            swap(x, y);
        }

        vector<int> diff(n + 1);

        auto add = [&](int l, int r, int v) {
            if (l > r) return;
            diff[l] += v;
            diff[r + 1] -= v;
        };

        auto update = [&](int i, int x, int y) {
            add(y - i, n - i, -1); // 撤销 [y,n]
            int dec = y - x - 1; // 缩短的距离
            add(y - i - dec, n - i - dec, 1);

            int j = (x + y + 1) / 2 + 1;
            add(j - i, y - 1 - i, -1); // 撤销 [j, y-1]
            add(x - i + 2, x - i + y - j + 1, 1);
        };

        auto update2 = [&](int i, int x, int y) {
            add(y - i, n - i, -1); // 撤销 [y,n]
            int dec = (y - i) - (i - x + 1); // 缩短的距离
            add(y - i - dec, n - i - dec, 1);

            int j = i + (y - x + 1) / 2 + 1;
            add(j - i, y - 1 - i, -1); // 撤销 [j, y-1]
            add(i - x + 2, i - x + y - j + 1, 1);
        };

        for (int i = 1; i <= n; i++) {
            add(1, i - 1, 1);
            add(1, n - i, 1);
            if (x + 1 >= y) {
                continue;
            }
            if (i <= x) {
                update(i, x, y);
            } else if (i >= y) {
                update(n + 1 - i, n + 1 - y, n + 1 - x);
            } else if (i < (x + y) / 2) {
                update2(i, x, y);
            } else if (i > (x + y + 1) / 2) {
                update2(n + 1 - i, n + 1 - y, n + 1 - x);
            }
        }

        vector<long long> ans(n);
        long long sum_d = 0;
        for (int i = 0; i < n; i++) {
            sum_d += diff[i + 1];
            ans[i] = sum_d;
        }
        return ans;
    }
};
```

```go [sol-Go]
func countOfPairs(n, x, y int) []int64 {
	if x > y {
		x, y = y, x
	}

	diff := make([]int, n+1)
	add := func(l, r, v int) {
		if l > r {
			return
		}
		diff[l] += v
		diff[r+1] -= v
	}

	update := func(i, x, y int) {
		add(y-i, n-i, -1) // 撤销 [y,n]
		dec := y - x - 1  // 缩短的距离
		add(y-i-dec, n-i-dec, 1)

		j := (x+y+1)/2 + 1
		add(j-i, y-1-i, -1) // 撤销 [j, y-1]
		add(x-i+2, x-i+y-j+1, 1)
	}

	update2 := func(i, x, y int) {
		add(y-i, n-i, -1) // 撤销 [y,n]
		dec := (y - i) - (i - x + 1) // 缩短的距离
		add(y-i-dec, n-i-dec, 1)

		j := i + (y-x+1)/2 + 1
		add(j-i, y-1-i, -1) // 撤销 [j, y-1]
		add(i-x+2, i-x+y-j+1, 1)
	}

	for i := 1; i <= n; i++ {
		add(1, i-1, 1)
		add(1, n-i, 1)
		if x+1 >= y {
			continue
		}
		if i <= x {
			update(i, x, y)
		} else if i >= y {
			update(n+1-i, n+1-y, n+1-x)
		} else if i < (x+y)/2 {
			update2(i, x, y)
		} else if i > (x+y+1)/2 {
			update2(n+1-i, n+1-y, n+1-x)
		}
	}

	ans := make([]int64, n)
	sumD := int64(0)
	for i, d := range diff[1:] {
		sumD += int64(d)
		ans[i] = sumD
	}
	return ans
}
```

## 方法二：分类讨论+直接计算

题目说 $(i,j)$ 和 $(j,i)$ 这两个房屋对我们都要统计，我们也可以只统计 $i<j$ 的房屋对，最后把答案乘 $2$。

分类讨论：

- 如果 $i\le x$，那么：
  - 设 $k = (x+y+1)/2$（见方法一），对于编号在 $[i+1, k]$ 内的房子，我们可以直接到达，把区间 $[1,k-i]$ 都加一。
  - 对于编号在 $[k+1, y-1]$ 内的房子，我们可以用 $x$ 到 $y$ 的边到达，把区间 $[x-i+2, x-i+y-k]$ 都加一。注：从 $i$ 经过 $x$ 到 $y$ 的边，到达 $k+1$ 走过的路径长度为 $x-i+1+y-(k+1) = x-i+y-k$。
  - 对于编号在 $[y, n]$ 内的房子，我们也可以用 $x$ 到 $y$ 的边到达，把区间 $[x-i+1, x-i+1+n-y]$ 都加一。
- 如果 $x < i < \left\lfloor\dfrac{x+y}{2}\right\rfloor$，那么同上：
  - 设 $k = i + (y-x+1)/2$（见方法一），对于编号在 $[i+1, k]$ 内的房子，我们可以直接到达，把区间 $[1,k-i]$ 都加一。
  - 对于编号在 $[k+1, y-1]$ 内的房子，我们可以用 $x$ 到 $y$ 的边到达，把区间 $[i-x+2, i-x+y-k]$ 都加一。注：从 $i$ 经过 $x$ 到 $y$ 的边，到达 $k+1$ 走过的路径长度为 $i-x+1+y-(k+1) = i-x+y-k$。
  - 对于编号在 $[y, n]$ 内的房子，我们也可以用 $x$ 到 $y$ 的边到达，把区间 $[i-x+1, i-x+1+n-y]$ 都加一。
- 对于更大的 $i$，它到右侧的房屋无需通过 $x$ 到 $y$ 的边，我们只需把区间 $[1,n-i]$ 都加一。

此外，当 $x+1\ge y$ 时，我们可以直接算出 $\textit{ans}[i]=2\cdot (n-i)$，其中 $i$ 从 $1$ 开始。

```py [sol-Python3]
class Solution:
    def countOfPairs(self, n: int, x: int, y: int) -> List[int]:
        if x > y:
            x, y = y, x

        if x + 1 >= y:
            return list(range((n - 1) * 2, -1, -2))

        diff = [0] * (n + 1)

        def add(l: int, r: int) -> None:
            diff[l] += 2
            diff[r + 1] -= 2

        for i in range(1, n):
            if i <= x:
                k = (x + y + 1) // 2
                add(1, k - i)
                add(x - i + 2, x - i + y - k)
                add(x - i + 1, x - i + 1 + n - y)
            elif i < (x + y) // 2:
                k = i + (y - x + 1) // 2
                add(1, k - i)
                add(i - x + 2, i - x + y - k)
                add(i - x + 1, i - x + 1 + n - y)
            else:
                add(1, n - i)

        return list(accumulate(diff))[1:]
```

```java [sol-Java]
class Solution {
    public long[] countOfPairs(int n, int x, int y) {
        if (x > y) {
            int temp = x;
            x = y;
            y = temp;
        }

        long[] ans = new long[n];
        if (x + 1 >= y) {
            for (int i = 1; i < n; i++) {
                ans[i - 1] = (n - i) * 2;
            }
            return ans;
        }

        diff = new int[n + 1];
        for (int i = 1; i < n; i++) {
            if (i <= x) {
                int k = (x + y + 1) / 2;
                add(1, k - i);
                add(x - i + 2, x - i + y - k);
                add(x - i + 1, x - i + 1 + n - y);
            } else if (i < (x + y) / 2) {
                int k = i + (y - x + 1) / 2;
                add(1, k - i);
                add(i - x + 2, i - x + y - k);
                add(i - x + 1, i - x + 1 + n - y);
            } else {
                add(1, n - i);
            }
        }

        long sumD = 0;
        for (int i = 0; i < n; i++) {
            sumD += diff[i + 1];
            ans[i] = sumD * 2;
        }
        return ans;
    }

    private int[] diff;

    private void add(int l, int r) {
        diff[l]++;
        diff[r + 1]--;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> countOfPairs(int n, int x, int y) {
        if (x > y) {
            swap(x, y);
        }

        vector<long long> ans(n);
        if (x + 1 >= y) {
            for (int i = 1; i < n; i++) {
                ans[i - 1] = (n - i) * 2;
            }
            return ans;
        }

        vector<int> diff(n + 1, 0);

        auto add = [&](int l, int r) {
            diff[l]++;
            diff[r + 1]--;
        };

        for (int i = 1; i < n; i++) {
            if (i <= x) {
                int k = (x + y + 1) / 2;
                add(1, k - i);
                add(x - i + 2, x - i + y - k);
                add(x - i + 1, x - i + 1 + n - y);
            } else if (i < (x + y) / 2) {
                int k = i + (y - x + 1) / 2;
                add(1, k - i);
                add(i - x + 2, i - x + y - k);
                add(i - x + 1, i - x + 1 + n - y);
            } else {
                add(1, n - i);
            }
        }

        long long sum_d = 0;
        for (int i = 0; i < n; i++) {
            sum_d += diff[i + 1];
            ans[i] = sum_d * 2;
        }
        return ans;
    }
};
```

```go [sol-Go]
func countOfPairs(n, x, y int) []int64 {
	if x > y {
		x, y = y, x
	}

	ans := make([]int64, n)
	if x+1 >= y {
		for i := 1; i < n; i++ {
			ans[i-1] = int64(n-i) * 2
		}
		return ans
	}

	diff := make([]int, n+1)
	add := func(l, r int) {
		diff[l]++
		diff[r+1]--
	}

	for i := 1; i < n; i++ {
		if i <= x {
			k := (x + y + 1) / 2
			add(1, k-i)
			add(x-i+2, x-i+y-k)
			add(x-i+1, x-i+1+n-y)
		} else if i < (x+y)/2 {
			k := i + (y-x+1)/2
			add(1, k-i)
			add(i-x+2, i-x+y-k)
			add(i-x+1, i-x+1+n-y)
		} else {
			add(1, n-i)
		}
	}

	sumD := int64(0)
	for i, d := range diff[1:] {
		sumD += int64(d)
		ans[i] = sumD * 2
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
