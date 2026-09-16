# 2426. 满足不等式的数对数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-pairs-satisfying-inequality/
- 题目 slug：`number-of-pairs-satisfying-inequality`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.2 逆序对
- 难度分：2030
- 外部题解来源：https://leetcode.cn/problems/number-of-pairs-satisfying-inequality/solutions/1863557/by-endlesscheng-9prc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转换成逆序对模型（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-pairs-satisfying-inequality/solutions/1863557/by-endlesscheng-9prc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-9prc`
- topic id：`1863557`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1tW4y1e7rb) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

包含树状数组的原理，以及另外一种归并排序的做法。

---

树状数组/线段树逐渐成为周赛必备技能了。

本题用到的技巧是，合并下标相同的元素。

式子变形得

$$
\textit{nums}_1[i]-\textit{nums}_2[i]\le\textit{nums}_1[j]-\textit{nums}_2[j]+\textit{diff}
$$

记 $a[i]=\textit{nums}_1[i]-\textit{nums}_2[i]$，上式为

$$
a[i]\le a[j]+\textit{diff}
$$

因此本题和 [剑指 Offer 51. 数组中的逆序对](https://leetcode.cn/problems/shu-zu-zhong-de-ni-xu-dui-lcof/)、[315. 计算右侧小于当前元素的个数](https://leetcode.cn/problems/count-of-smaller-numbers-after-self/) 等题目实质上是同一类题，用**归并排序**或者**树状数组**等均可以通过。

**下面代码用的离散化树状数组，即使元素范围达到 $10^9$ 也适用。**

```py [sol1-Python3]
class Solution:
    def numberOfPairs(self, a: List[int], nums2: List[int], diff: int) -> int:
        for i, x in enumerate(nums2):
            a[i] -= x
        b = sorted(set(a))  # 配合下面的二分，离散化

        ans = 0
        t = BIT(len(b) + 1)
        for x in a:
            ans += t.query(bisect_right(b, x + diff))
            t.add(bisect_left(b, x) + 1)
        return ans

class BIT:
    def __init__(self, n):
        self.tree = [0] * n

    def add(self, x):
        while x < len(self.tree):
            self.tree[x] += 1
            x += x & -x

    def query(self, x):
        res = 0
        while x > 0:
            res += self.tree[x]
            x &= x - 1
        return res
```

```java [sol1-Java]
class Solution {
    public long numberOfPairs(int[] a, int[] nums2, int diff) {
        var n = a.length;
        for (var i = 0; i < n; ++i)
            a[i] -= nums2[i];
        var b = Arrays.stream(a).distinct().sorted().toArray(); // 配合下面的二分，离散化

        var ans = 0L;
        var t = new BIT(b.length + 1);
        for (var x : a) {
            ans += t.query(lowerBound(b, x + diff + 1));
            t.add(lowerBound(b, x) + 1);
        }
        return ans;
    }

    private int lowerBound(int[] a, int x) {
        int left = 0, right = a.length;
        while (left < right) {
            var mid = left + (right - left) / 2;
            if (a[mid] < x) left = mid + 1;
            else right = mid;
        }
        return left;
    }
}

class BIT {
    private final int[] tree;

    public BIT(int n) {
        tree = new int[n];
    }

    public void add(int x) {
        while (x < tree.length) {
            ++tree[x];
            x += x & -x;
        }
    }

    public int query(int x) {
        var res = 0;
        while (x > 0) {
            res += tree[x];
            x &= x - 1;
        }
        return res;
    }
}
```

```cpp [sol1-C++]
class BIT {
private:
    vector<int> tree;

public:
    BIT(int n) : tree(n) {}

    void add(int x) {
        while (x < tree.size()) {
            ++tree[x];
            x += x & -x;
        }
    }

    int query(int x) {
        int res = 0;
        while (x > 0) {
            res += tree[x];
            x &= x - 1;
        }
        return res;
    }
};

class Solution {
public:
    long long numberOfPairs(vector<int> &a, vector<int> &nums2, int diff) {
        int n = a.size();
        for (int i = 0; i < n; ++i)
            a[i] -= nums2[i];
        auto b = a;
        sort(b.begin(), b.end());
        b.erase(unique(b.begin(), b.end()), b.end()); // 配合下面的二分，离散化

        long ans = 0L;
        auto t = new BIT(n + 1);
        for (int x : a) {
            ans += t->query(upper_bound(b.begin(), b.end(), x + diff) - b.begin());
            t->add(lower_bound(b.begin(), b.end(), x) - b.begin() + 1);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func numberOfPairs(a, nums2 []int, diff int) (ans int64) {
	for i, x := range nums2 {
		a[i] -= x
	}
	// 配合下面的二分，离散化
	set := map[int]struct{}{}
	for _, v := range a {
		set[v] = struct{}{}
	}
	b := make(sort.IntSlice, 0, len(set))
	for x := range set {
		b = append(b, x)
	}
	sort.Ints(b)

	t := make(BIT, len(a)+1)
	for _, x := range a {
		ans += int64(t.query(b.Search(x + diff + 1)))
		t.add(b.Search(x) + 1)
	}
	return
}

type BIT []int

func (t BIT) add(x int) {
	for x < len(t) {
		t[x]++
		x += x & -x
	}
}

func (t BIT) query(x int) (res int) {
	for x > 0 {
		res += t[x]
		x &= x - 1
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{nums}_1$ 的长度。
- 空间复杂度：$O(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.2 逆序对`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.2 逆序对`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
