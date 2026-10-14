# 1561. 你可以获得的最大硬币数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-coins-you-can-get/
- 题目 slug：`maximum-number-of-coins-you-can-get`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1406
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-coins-you-can-get/solutions/3035907/tan-xin-jian-ji-xie-fa-pythonjavaccgojsr-2ptn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-number-of-coins-you-can-get/solutions/3035907/tan-xin-jian-ji-xie-fa-pythonjavaccgojsr-2ptn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-jian-ji-xie-fa-pythonjavaccgojsr-2ptn`
- topic id：`3035907`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

问题相当于每次选三堆硬币，Alice 先取一堆，我们再取一堆，Bob 取剩下的那一堆。

- 由于 Bob 总是最后取，每一轮，我们可以把（剩余硬币堆中的）最少的那一堆硬币给 Bob。
- 由于 Alice 总是先取，那么我们不可能取到 $\textit{piles}$ 的最大值，但可以取到次大值。同理，第三大的给 Alice，我们取第四大的。依此类推。

因此，把 $\textit{piles}$ 从大到小排序后，我们可以取走下标为

$$
1,3,5,\ldots,2m-3,2m-1
$$

的硬币堆，其中 $m=\dfrac{n}{3}$，$n$ 是 $\textit{piles}$ 的长度。（题目保证 $n$ 是 $3$ 的倍数）

也可以把 $\textit{piles}$ 从小到大排序，取走下标为

$$
m,m+2,m+4,\ldots,n-4,n-2
$$

的硬币堆。

```py [sol-Python3]
class Solution:
    def maxCoins(self, piles: List[int]) -> int:
        piles.sort()
        return sum(piles[len(piles)//3::2])
```

```java [sol-Java]
class Solution {
    public int maxCoins(int[] piles) {
        Arrays.sort(piles);
        int n = piles.length;
        int ans = 0;
        for (int i = n / 3; i < n; i += 2) {
            ans += piles[i];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxCoins(vector<int>& piles) {
        ranges::sort(piles);
        int n = piles.size();
        int ans = 0;
        for (int i = n / 3; i < n; i += 2) {
            ans += piles[i];
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int maxCoins(int* piles, int n) {
    qsort(piles, n, sizeof(int), cmp);
    int ans = 0;
    for (int i = n / 3; i < n; i += 2) {
        ans += piles[i];
    }
    return ans;
}
```

```go [sol-Go]
func maxCoins(piles []int) (ans int) {
    slices.Sort(piles)
    n := len(piles)
    for i := n / 3; i < n; i += 2 {
        ans += piles[i]
    }
    return
}
```

```js [sol-JavaScript]
var maxCoins = function(piles) {
    piles.sort((a, b) => a - b);
    const n = piles.length;
    let ans = 0;
    for (let i = n / 3; i < n; i += 2) {
        ans += piles[i];
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_coins(mut piles: Vec<i32>) -> i32 {
        piles.sort_unstable();
        piles[piles.len() / 3..].iter().step_by(2).sum()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{piles}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。Python 忽略切片开销。

## 相似题目

[3457. 吃披萨](https://leetcode.cn/problems/eat-pizzas/)

更多相似题目，见下面贪心题单中的「**§1.1 从最小/最大开始贪心**」。

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
