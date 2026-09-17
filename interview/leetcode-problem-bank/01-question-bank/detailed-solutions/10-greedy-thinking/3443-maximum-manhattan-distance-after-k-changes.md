# 3443. K 次修改后的最大曼哈顿距离

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-manhattan-distance-after-k-changes/
- 题目 slug：`maximum-manhattan-distance-after-k-changes`
- 来源专题：贪心与思维
- 来源分类路径：八、其他
- 难度分：1856
- 外部题解来源：https://leetcode.cn/problems/maximum-manhattan-distance-after-k-changes/solutions/3061765/heng-zong-zuo-biao-fen-bie-ji-suan-tan-x-lhhi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-manhattan-distance-after-k-changes/solutions/3061765/heng-zong-zuo-biao-fen-bie-ji-suan-tan-x-lhhi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`heng-zong-zuo-biao-fen-bie-ji-suan-tan-x-lhhi`
- topic id：`3061765`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

## 方法一

对于曼哈顿距离，由于水平方向的移动和垂直方向的移动互不影响，我们可以**分别计算横纵坐标**。

设当前向西走了 $a$ 步，向东走了 $b$ 步。比如 $a=2,\ b=5$。

把其中更小的 $a$ 改成向东走，可以让我们离原点更远。（改 $b$ 反而让我们离原点更近）

如果把 $a$ 减少 $d$，那么 $b$ 就能增大 $d$，所以修改后的当前位置的横坐标为

$$
(b+d) - (a-d) = b-a+2d
$$

如果 $b$ 更小，那么横坐标的绝对值为

$$
a-b+2d
$$

综合两种情况，修改后的当前位置横坐标的绝对值为

$$
|a-b|+2d
$$

其中 $d$ 为操作次数 $k$，但至多为 $a$（把 $a$ 减小为 $0$）且至多为 $b$（把 $b$ 减小为 $0$），即

$$
d = \min(a,b,k)
$$

然后把 $k$ 减少 $d$，按照同样的方法继续计算纵坐标。

用修改后的横纵坐标绝对值之和，更新答案的最大值。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1D5F6eRECp/?t=1m43s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxDistance(self, s: str, k: int) -> int:
        ans = 0
        cnt = defaultdict(int)
        for ch in s:
            cnt[ch] += 1
            left = k
            def f(a: int, b: int) -> int:
                nonlocal left
                d = min(a, b, left)
                left -= d
                return abs(a - b) + d * 2
            ans = max(ans, f(cnt['N'], cnt['S']) + f(cnt['E'], cnt['W']))
        return ans
```

```java [sol-Java]
class Solution {
    private int left;

    public int maxDistance(String s, int k) {
        int ans = 0;
        int[] cnt = new int['X']; // 'W' + 1 = 'X'
        for (char ch : s.toCharArray()) {
            cnt[ch]++;
            left = k;
            ans = Math.max(ans, f(cnt['N'], cnt['S']) + f(cnt['E'], cnt['W']));
        }
        return ans;
    }

    private int f(int a, int b) {
        int d = Math.min(Math.min(a, b), left);
        left -= d;
        return Math.abs(a - b) + d * 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxDistance(string s, int k) {
        int ans = 0;
        int cnt['X']{}; // 'W' + 1 = 'X'
        for (char ch : s) {
            cnt[ch]++;
            int left = k;
            auto f = [&](int a, int b) -> int {
                int d = min({a, b, left});
                left -= d;
                return abs(a - b) + d * 2;
            };
            ans = max(ans, f(cnt['N'], cnt['S']) + f(cnt['E'], cnt['W']));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxDistance(s string, k int) (ans int) {
	cnt := ['X']int{} // 'W' + 1 = 'X'
	for _, ch := range s {
		cnt[ch]++
		left := k
		f := func(a, b int) int {
			d := min(a, b, left)
			left -= d
			return abs(a-b) + d*2
		}
		ans = max(ans, f(cnt['N'], cnt['S'])+f(cnt['E'], cnt['W']))
	}
	return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(n+|\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

## 方法二

设当前位置为 $(x,y)$，当前位置到原点的曼哈顿距离为 $|x|+|y|$。

再看看方法一的这个式子

$$
|a-b|+2d
$$

其中 $|a-b|$ 就是 $|x|$，$d$ 可以理解为操作次数。所以**每操作一次，曼哈顿距离都会增大** $2$。但这不会超过移动的次数，即 $i+1$。

所以执行完 $s[i]$ 后的答案为

$$
\min(|x|+|y|+2k,i+1)
$$

```py [sol-Python3]
class Solution:
    def maxDistance(self, s: str, k: int) -> int:
        ans = x = y = 0
        for i, c in enumerate(s):
            if c == 'N': y += 1
            elif c == 'S': y -= 1
            elif c == 'E': x += 1
            else: x -= 1
            ans = max(ans, min(abs(x) + abs(y) + k * 2, i + 1))
        return ans
```

```java [sol-Java]
class Solution {
    public int maxDistance(String s, int k) {
        int ans = 0, x = 0, y = 0;
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            if (c == 'N') y++;
            else if (c == 'S') y--;
            else if (c == 'E') x++;
            else x--;
            ans = Math.max(ans, Math.min(Math.abs(x) + Math.abs(y) + k * 2, i + 1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxDistance(string s, int k) {
        int ans = 0, x = 0, y = 0;
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == 'N') y++;
            else if (s[i] == 'S') y--;
            else if (s[i] == 'E') x++;
            else x--;
            ans = max(ans, min(abs(x) + abs(y) + k * 2, i + 1));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxDistance(s string, k int) int {
	ans, x, y := 0, 0, 0
	for i, c := range s {
		switch c {
		case 'N': y++
		case 'S': y--
		case 'E': x++
		default:  x--
		}
		ans = max(ans, min(abs(x)+abs(y)+k*2, i+1))
	}
	return ans
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(n+|\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `八、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
