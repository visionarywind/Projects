# 2916. 子数组不同元素数目的平方和 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/subarrays-distinct-element-sum-of-squares-ii/
- 题目 slug：`subarrays-distinct-element-sum-of-squares-ii`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）
- 难度分：2816
- 外部题解来源：https://leetcode.cn/problems/subarrays-distinct-element-sum-of-squares-ii/solutions/2502897/yi-bu-bu-ti-shi-ni-si-kao-ben-ti-pythonj-zhhs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一步步提示你思考本题！（Python/Java/C++/Go）](https://leetcode.cn/problems/subarrays-distinct-element-sum-of-squares-ii/solutions/2502897/yi-bu-bu-ti-shi-ni-si-kao-ben-ti-pythonj-zhhs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-bu-bu-ti-shi-ni-si-kao-ben-ti-pythonj-zhhs`
- topic id：`2502897`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

推荐先完成本题的简单版本：[2262. 字符串的总引力](https://leetcode.cn/problems/total-appeal-of-a-string/)，这题和本题都在 [视频讲解](https://www.bilibili.com/video/BV1Tz4y1N7Wx/) 第四题中讲了。

为方便描述，下文将 $\textit{nums}$ 简记为 $a$。

## 提示 1

把右端点相同的子数组，分为同一组。

右端点为 $i$ 的子数组，可以看成是右端点为 $i-1$ 的子数组，在末尾添加上 $a[i]$。

添加后，右端点为 $i-1$ 的这些子数组的「不同计数的平方」之和**增加**了多少？

## 提示 2

假设一个子数组的「不同计数」为 $x$，那么它的「不同计数的平方」为 $x^2$。

如果这个子数组的「不同计数」增加了 $1$，那么它的「不同计数的平方」的**增加量**为

$$
(x+1)^2 - x^2 = 2x+1
$$

## 提示 3

假设 $i=3$，那么右端点为 $i-1$ 的子数组有

- $a[0..2]$，设其「不同计数」为 $x_0$。
- $a[1..2]$，设其「不同计数」为 $x_1$。
- $a[2..2]$，设其「不同计数」为 $x_2$。

其中 $a[k..i]$ 表示从 $a[k]$ 到 $a[i]$ 的子数组。

考虑从子数组 $a[k..i-1]$ 到子数组 $a[k..i]$，分类讨论：

- 如果 $a[i]$ 之前没有遇到过（例如 $a=[1,2,3,4]$），那么这些子数组的「不同计数」都会增加 $1$。根据提示 2，「不同计数的平方」之和的**增加量**为 $(x_0+x_1+x_2)\cdot 2 + 3$。
- 如果 $a[i]$ 之前遇到过，设其上次出现的下标为 $j$，那么：
    - 对于子数组 $a[0..i-1],\ a[1..i-1],\ a[2..i-1],\cdots,a[j..i-1]$，在其末尾添加 $a[i]$ 后，这些子数组的「不同计数」是不会变化的，因为 $a[i]$ 已经在 $a[j]$ 处出现过了。 
    - 对于子数组 $a[j+1..i-1],\ a[j+2..i-1],\cdots,a[i-1..i-1]$，由于不包含 $a[i]$，这些子数组的「不同计数」都会增加 $1$，「不同计数的平方」之和的**增加量**计算方式同上。
- 别忘了 $a[i..i]$ 也是一个子数组，把它的「不同计数」加一。

所以，我们需要一个这样的数据结构，用来维护子数组的「不同计数」：

1. 定义 $f[k]$ 表示左端点为 $k$ 的子数组的「不同计数」，如果当前遍历到 $\textit{nums}[i]$，那么 $f[k]$ 就对应着子数组 $a[k..i]$。
2. 区间加一：例如 $a[1..3]$，$a[2..3]$ 和 $a[3..3]$ 的「不同计数」都增加了 $1$，那么就把区间 $[1,3]$ 的「不同计数」加一。
3. 询问区间元素和：为了计算出「不同计数的平方」之和的**增加量**，需要知道从 $f[j+1]$ 到 $f[i-1]$ 的「不同计数」，这里 $j$ 为 $a[i]$ 上次出现的下标。

这可以用 lazy 线段树实现，具体请看[【双周赛 98】](https://www.bilibili.com/video/BV15D4y1G7ms/)第四题的讲解（[2569. 更新数组后处理求和查询](https://leetcode.cn/problems/handling-sum-queries-after-update/)）

## 提示 4

用一个变量 $s$ 维护右端点为 $i$ 的子数组的「不同计数的平方」之和。

遍历 $\textit{nums}$，每次循环按照上述规则更新 $s$：

1. 为了方便调用线段树，假设下标从 $1$ 开始。
2. 设 $a[i]$ 上次出现的下标为 $j$（不存在则为 $0$）。询问 $[j+1,i]$ 的元素和，设为 $s_1$。把 $s_1\cdot 2 + i-j$ 加到 $s$ 中。
3. 把区间 $[j+1,i]$ 都加一。
4. 把 $s$ 加到答案中。
5. 更新 $a[i]$ 的上一次出现位置为 $i$。

代码实现时，由于查询的区间和更新的区间是同一个，可以同时完成。

```py [sol-Python3]
class Solution:
    def sumCounts(self, nums: List[int]) -> int:
        n = len(nums)
        sum = [0] * (n * 4)
        todo = [0] * (n * 4)

        def do(o: int, l: int, r: int, add: int) -> None:
            sum[o] += add * (r - l + 1)
            todo[o] += add

        # o=1  [l,r] 1<=l<=r<=n
        # 把 [L,R] 加一，同时返回加一之前的区间和
        def query_and_add1(o: int, l: int, r: int, L: int, R: int) -> int:
            if L <= l and r <= R:
                res = sum[o]
                do(o, l, r, 1)
                return res

            m = (l + r) // 2
            add = todo[o]
            if add:
                do(o * 2, l, m, add)
                do(o * 2 + 1, m + 1, r, add)
                todo[o] = 0

            res = 0
            if L <= m: res += query_and_add1(o * 2, l, m, L, R)
            if m < R:  res += query_and_add1(o * 2 + 1, m + 1, r, L, R)
            sum[o] = sum[o * 2] + sum[o * 2 + 1]
            return res

        ans = s = 0
        last = {}
        for i, x in enumerate(nums, 1):
            j = last.get(x, 0)
            s += query_and_add1(1, 1, n, j + 1, i) * 2 + i - j
            ans += s
            last[x] = i
        return ans % 1_000_000_007
```

```java [sol-Java]
class Solution {
    private long[] sum;
    private int[] todo;

    public int sumCounts(int[] nums) {
        int n = nums.length;
        sum = new long[n * 4];
        todo = new int[n * 4];

        long ans = 0, s = 0;
        var last = new HashMap<Integer, Integer>();
        for (int i = 1; i <= n; i++) {
            int x = nums[i - 1];
            int j = last.getOrDefault(x, 0);
            s += queryAndAdd1(1, 1, n, j + 1, i) * 2 + i - j;
            ans = (ans + s) % 1_000_000_007;
            last.put(x, i);
        }
        return (int) ans;
    }

    private void do_(int o, int l, int r, int add) {
        sum[o] += (long) add * (r - l + 1);
        todo[o] += add;
    }

    // o=1  [l,r] 1<=l<=r<=n
    // 把 [L,R] 加一，同时返回加一之前的区间和
    private long queryAndAdd1(int o, int l, int r, int L, int R) {
        if (L <= l && r <= R) {
            long res = sum[o];
            do_(o, l, r, 1);
            return res;
        }

        int m = (l + r) / 2;
        int add = todo[o];
        if (add != 0) {
            do_(o * 2, l, m, add);
            do_(o * 2 + 1, m + 1, r, add);
            todo[o] = 0;
        }

        long res = 0;
        if (L <= m) res += queryAndAdd1(o * 2, l, m, L, R);
        if (m < R)  res += queryAndAdd1(o * 2 + 1, m + 1, r, L, R);
        sum[o] = sum[o * 2] + sum[o * 2 + 1];
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<long long> sum;
    vector<int> todo;

    void do_(int o, int l, int r, int add) {
        sum[o] += (long long) add * (r - l + 1);
        todo[o] += add;
    }

    // o=1  [l,r] 1<=l<=r<=n
    // 把 [L,R] 加一，同时返回加一之前的区间和
    long long query_and_add1(int o, int l, int r, int L, int R) {
        if (L <= l && r <= R) {
            long long res = sum[o];
            do_(o, l, r, 1);
            return res;
        }

        int m = (l + r) / 2;
        int add = todo[o];
        if (add != 0) {
            do_(o * 2, l, m, add);
            do_(o * 2 + 1, m + 1, r, add);
            todo[o] = 0;
        }

        long long res = 0;
        if (L <= m) res += query_and_add1(o * 2, l, m, L, R);
        if (m < R)  res += query_and_add1(o * 2 + 1, m + 1, r, L, R);
        sum[o] = sum[o * 2] + sum[o * 2 + 1];
        return res;
    }

public:
    int sumCounts(vector<int> &nums) {
        int n = nums.size();
        sum.resize(n * 4);
        todo.resize(n * 4);

        long long ans = 0, s = 0;
        unordered_map<int, int> last;
        for (int i = 1; i <= n; i++) {
            int x = nums[i - 1];
            int j = last.count(x) ? last[x] : 0;
            s += query_and_add1(1, 1, n, j + 1, i) * 2 + i - j;
            ans = (ans + s) % 1'000'000'007;
            last[x] = i;
        }
        return ans;
    }
};
```

```go [sol-Go]
type lazySeg []struct{ sum, todo int }

func (t lazySeg) do(o, l, r, add int) {
	t[o].sum += add * (r - l + 1)
	t[o].todo += add
}

// o=1  [l,r] 1<=l<=r<=n
// 把 [L,R] 加一，同时返回加一之前的区间和
func (t lazySeg) queryAndAdd1(o, l, r, L, R int) (res int) {
	if L <= l && r <= R {
		res = t[o].sum
		t.do(o, l, r, 1)
		return
	}
	m := (l + r) >> 1
	if add := t[o].todo; add != 0 {
		t.do(o<<1, l, m, add)
		t.do(o<<1|1, m+1, r, add)
		t[o].todo = 0
	}
	if L <= m {
		res = t.queryAndAdd1(o<<1, l, m, L, R)
	}
	if m < R {
		res += t.queryAndAdd1(o<<1|1, m+1, r, L, R)
	}
	t[o].sum = t[o<<1].sum + t[o<<1|1].sum
	return
}

func sumCounts(nums []int) (ans int) {
	last := map[int]int{}
	n := len(nums)
	t := make(lazySeg, n*4)
	s := 0
	for i, x := range nums {
		i++
		j := last[x]
		s += t.queryAndAdd1(1, 1, n, j+1, i)*2 + i - j
		ans = (ans + s) % 1_000_000_007
		last[x] = i
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
