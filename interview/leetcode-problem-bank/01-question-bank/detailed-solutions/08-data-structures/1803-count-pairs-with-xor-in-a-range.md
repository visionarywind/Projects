# 1803. 统计异或值在范围内的数对有多少

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/
- 题目 slug：`count-pairs-with-xor-in-a-range`
- 来源专题：常用数据结构
- 来源分类路径：六、字典树（trie） / §6.4 0-1 字典树（异或字典树）
- 难度分：2479
- 外部题解来源：https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/solutions/2045560/bu-hui-zi-dian-shu-zhi-yong-ha-xi-biao-y-p2pu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[不会字典树？只用哈希表也能做！（Python/Java/C++/Go）](https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/solutions/2045560/bu-hui-zi-dian-shu-zhi-yong-ha-xi-biao-y-p2pu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bu-hui-zi-dian-shu-zhi-yong-ha-xi-biao-y-p2pu`
- topic id：`2045560`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![1803-1.png](https://pic.leetcode.cn/1672839517-EkuLMm-1803-1.png)

代码实现时，可以按照「统计前 $k$ 个比特、统计前 $k-1$ 个比特、统计前 $k-2$ 个比特……统计前 $1$ 个比特」执行。在遍历 $\textit{cnt}$ 的同时，可以把每个键都右移 $1$ 位，从而得到下一次要统计的 $\textit{cnt}$，这样不用重新遍历 $\textit{nums}$，效率也更高。

## 答疑

**问**：$\textit{cnt}[x]\cdot \textit{cnt}[x\oplus t]$ 这里是怎么来的？

**答**：比如 $\textit{nums}$ 中有 $2$ 个 $1$，$3$ 个 $2$，那么有 $2\cdot 3=6$ 个异或值等于 $3$ 的数对。

**问**：你这个统计的逻辑有问题，如果 $t=0$ 就算错了！

**答**：题目数据范围保证 $\textit{low}\ge 1$，即使对于 $t=0$ 计算出了错误的结果，$[0,\textit{high}+1)$ 和 $[0,\textit{low})$ 这两个区间的计算结果相减，恰好可以把 $t=0$ 的结果抵消掉。

**问**：「数对异或值的后两个比特是什么都可以」是什么意思？

**答**：相当于把 $t=10000,10001,10010,10011$ 这 $4$ 个问题一起计算。由于 $\textit{nums}$ 的任意两个数的异或的后两个比特必然为 $00,01,10,11$ 中的一个，因此只需要看这个异或的前三个比特是否为 $100$ 就行了，因此「$t=10000,10001,10010,10011$ 这 $4$ 个问题的数对个数之和」等价于「在只统计 $\textit{nums}[i]$ 的前三个比特的情况下，计算 $t=100$ 的数对个数」。

**问**：感觉这个统计的思想和字典树有点像，可以展开讲讲吗？

**答**：其实这个方法就是字典树自底向上的过程，由于没有像字典树那样逐位统计 $\textit{nums}[i]$ 的每个比特位，所以时间复杂度和空间复杂度都比字典树的写法要优（见复杂度分析）。

```py [sol-Python3]
class Solution:
    def countPairs(self, nums: List[int], low: int, high: int) -> int:
        cnt = Counter(nums)
        ans = 0
        high += 1
        while high:
            nxt = Counter()
            for x, c in cnt.items():
                if high & 1: ans += c * cnt[x ^ (high - 1)]
                if low & 1:  ans -= c * cnt[x ^ (low - 1)]
                nxt[x >> 1] += c
            cnt = nxt
            low >>= 1
            high >>= 1
        return ans // 2
```

```java [sol-Java]
class Solution {
    public int countPairs(int[] nums, int low, int high) {
        int ans = 0;
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            cnt.merge(x, 1, Integer::sum); // cnt[x]++
        }
        for (high++; high > 0; high >>= 1, low >>= 1) {
            Map<Integer, Integer> nxt = new HashMap<>();
            for (var e : cnt.entrySet()) {
                int x = e.getKey(), c = e.getValue();
                if ((high & 1) == 1) ans += c * cnt.getOrDefault(x ^ (high - 1), 0);
                if ((low & 1) == 1)  ans -= c * cnt.getOrDefault(x ^ (low - 1), 0);
                nxt.put(x >> 1, nxt.getOrDefault(x >> 1, 0) + c);
            }
            cnt = nxt;
        }
        return ans / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countPairs(vector<int>& nums, int low, int high) {
        int ans = 0;
        unordered_map<int, int> cnt;
        for (int x : nums) {
            cnt[x]++;
        }
        for (high++; high; high >>= 1, low >>= 1) {
            unordered_map<int, int> nxt;
            for (auto& [x, c] : cnt) {
                if (high & 1 && cnt.count(x ^ (high - 1))) ans += c * cnt[x ^ (high - 1)];
                if (low & 1 && cnt.count(x ^ (low - 1)))   ans -= c * cnt[x ^ (low - 1)];
                nxt[x >> 1] += c;
            }
            cnt = move(nxt);
        }
        return ans / 2;
    }
};
```

```go [sol-Go]
func countPairs(nums []int, low, high int) (ans int) {
    cnt := map[int]int{}
    for _, x := range nums {
        cnt[x]++
    }
    for high++; high > 0; high >>= 1 {
        nxt := map[int]int{}
        for x, c := range cnt {
            if high&1 > 0 { ans += c * cnt[x^(high-1)] }
            if low&1 > 0  { ans -= c * cnt[x^(low-1)] }
            nxt[x>>1] += c
        }
        cnt = nxt
        low >>= 1
    }
    return ans / 2
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，严格来说是 $O\left(n+n\log\dfrac{U}{n}\right)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。如果 $U\le n$，第一次循环是 $O(U)$，第二次循环是 $O(U/2)$，第三次是 $O(U/4)$ …… 累加这个等比数列得到 $O(U)$，但是统计 $\textit{cnt}$ 要 $O(n)$，$U\le n$，所以时间复杂度为 $O(n)$；如果 $U>n$，那么前面 $O\left(\log\dfrac{U}{n}\right)$ 次循环的时间复杂度都是 $O(n)$，其余的加起来是 $O(n)$，所以时间复杂度为 $O\left(n\log\dfrac{U}{n}\right)$。两者取最大值，由于在大 $O$ 记号下最大值和相加是一样的，所以时间复杂度为 $O\left(n+n\log\dfrac{U}{n}\right)$。
- 空间复杂度：$O(n)$。

## 相似题目

- [Codeforces 665E. Beautiful Subarrays](https://codeforces.com/problemset/problem/665/E)
- [Codeforces 817E. Choosing The Commander](https://codeforces.com/problemset/problem/817/E)

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
