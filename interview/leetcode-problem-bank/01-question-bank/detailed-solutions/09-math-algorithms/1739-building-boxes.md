# 1739. 放置盒子

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/building-boxes/
- 题目 slug：`building-boxes`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：2198
- 外部题解来源：https://leetcode.cn/problems/building-boxes/solutions/2031813/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-8vbe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没想明白？一个动画秒懂！附 O(1) 做法（Python/Java/C++/Go）](https://leetcode.cn/problems/building-boxes/solutions/2031813/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-8vbe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-8vbe`
- topic id：`2031813`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

<![1739-1.png](https://pic.leetcode.cn/1671765455-jhGgLp-1739-1.png),![1739-2.png](https://pic.leetcode.cn/1671765455-iRHnBC-1739-2.png),![1739-3.png](https://pic.leetcode.cn/1671765455-iGSebO-1739-3.png),![1739-4.png](https://pic.leetcode.cn/1671765455-vULAkC-1739-4.png),![1739-5.png](https://pic.leetcode.cn/1671765455-KtZtTE-1739-5.png),![1739-6.png](https://pic.leetcode.cn/1671765455-KjofVz-1739-6.png),![1739-7.png](https://pic.leetcode.cn/1671765455-kfHwdk-1739-7.png),![1739-8.png](https://pic.leetcode.cn/1671765455-hTrTGG-1739-8.png),![1739-9.png](https://pic.leetcode.cn/1671765455-Gjcdcx-1739-9.png),![1739-10.png](https://pic.leetcode.cn/1671765455-PPGHPj-1739-10.png)>

根据上图，当接触地面的盒子 $\textit{ans}$ 等于

$$
1+2+3+\cdots+i=\dfrac{i(i+1)}{2}
$$

时，对应的盒子上限 $\textit{maxN}$ 为 

$$
1+(1+2)+(1+2+3)+\cdots+\dfrac{i(i+1)}{2}=\dfrac{i(i+1)(i+2)}{6}
$$

在 $\dfrac{i(i+1)(i+2)}{6}$ 的基础上，接触地面的盒子再增加 $j$ 个 $(j\le i+1)$，盒子上限就会再增加 $1+2+\cdots+j=\dfrac{j(j+1)}{2}$ 个。

因此，设 $x$ 为最大的满足 $\dfrac{i(i+1)(i+2)}{6} \le n$ 的 $i$，$y$ 为最小的满足 $\dfrac{x(x+1)(x+2)}{6} + \dfrac{j(j+1)}{2} \ge n$ 的 $j$，则答案为 $\dfrac{x(x+1)}{2}+y$。

实现时可以把 $x$ 和 $y$ 直接加到 $\textit{ans}$ 中。

```py [sol-Python3]
class Solution:
    def minimumBoxes(self, n: int) -> int:
        ans = max_n = 0
        i = j = 1
        while max_n + ans + i <= n:
            ans += i
            max_n += ans
            i += 1
        while max_n < n:
            ans += 1
            max_n += j
            j += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumBoxes(int n) {
        int ans = 0;
        int maxN = 0;
        for (int i = 1; maxN + ans + i <= n; i++) {
            ans += i;
            maxN += ans;
        }
        for (int j = 1; maxN < n; j++) {
            ans++;
            maxN += j;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumBoxes(int n) {
        int ans = 0, max_n = 0;
        for (int i = 1; max_n + ans + i <= n; i++) {
            ans += i;
            max_n += ans;
        }
        for (int j = 1; max_n < n; j++) {
            ans++;
            max_n += j;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumBoxes(n int) (ans int) {
	maxN := 0
	for i := 1; maxN+ans+i <= n; i++ {
		ans += i
		maxN += ans
	}
	for j := 1; maxN < n; j++ {
		ans++
		maxN += j
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\sqrt[3]n)$。根据上面推导的公式，$i$ 和 $j$ 至多为 $\mathcal{O}(\sqrt[3]n)$。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干变量。

#### 优化

直接计算 $x$ 和 $y$。

对于 $x$，哪个整数是满足 $\dfrac{x(x+1)(x+2)}{6} \le n$ 的最大整数呢？

如果 $x=\lfloor\sqrt[3]{6n}\rfloor$：

- 由于 $(x+1)(x+2)(x+3)>(x+1)^3>6n$，所以 $x+1$ 必不满足要求。
- 由于 $(x-1)x(x+1)=x(x^2-1)=x^3-x<x^3\le 6n$，所以 $x-1$ 必满足要求。
- $x$ 是否满足要求？计算一下就知道了。

因此，取 $x=\lfloor\sqrt[3]{6n}\rfloor$，如果 $\dfrac{x(x+1)(x+2)}{6} > n$ 则将 $x$ 减一。

对于 $y$，设 $N=n-\dfrac{x(x+1)(x+2)}{6}$，则

$$
\dfrac{y(y+1)}{2}\ge N
$$

解得

$$
y\ge \dfrac{-1+\sqrt{1+8N}}{2}
$$

由于 $y$ 是整数，所以

$$
y=\left\lceil\dfrac{-1+\sqrt{1+8N}}{2}\right\rceil = \left\lceil\dfrac{-1+\lceil\sqrt{1+8N}\rceil}{2}\right\rceil = \left\lfloor\dfrac{\lceil\sqrt{1+8N}\rceil}{2}\right\rfloor
$$

```py [sol-Python3]
class Solution:
    def minimumBoxes(self, n: int) -> int:
        x = floor(cbrt(6 * n))
        ans = x * (x + 1) // 2
        max_n = x * (x + 1) * (x + 2) // 6
        if max_n > n:
            max_n -= ans
            ans -= x
        return ans + ceil(sqrt(1 + 8 * (n - max_n))) // 2
```

```java [sol-Java]
class Solution {
    public int minimumBoxes(int n) {
        int x = (int) Math.cbrt(6L * n);
        int ans = x * (x + 1) / 2;
        int maxN = (int) ((long) x * (x + 1) * (x + 2) / 6);
        if (maxN > n) {
            maxN -= ans;
            ans -= x;
        }
        return ans + (int) Math.ceil(Math.sqrt(1 + 8 * (n - maxN))) / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumBoxes(int n) {
        int x = cbrt(6L * n);
        int ans = x * (x + 1) / 2;
        int max_n = (long) x * (x + 1) * (x + 2) / 6;
        if (max_n > n) {
            max_n -= ans;
            ans -= x;
        }
        return ans + (int) ceil(sqrt(1 + 8 * (n - max_n))) / 2;
    }
};
```

```go [sol-Go]
func minimumBoxes(n int) int {
    x := int(math.Cbrt(float64(6 * n)))
    ans := x * (x + 1) / 2
    maxN := x * (x + 1) * (x + 2) / 6
    if maxN > n {
        maxN -= ans
        ans -= x
    }
    return ans + int(math.Ceil(math.Sqrt(float64(1+8*(n-maxN)))))/2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。计算开立方和开平方均视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干变量。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
