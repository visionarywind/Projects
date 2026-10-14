# 326. 3 的幂

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/power-of-three/
- 题目 slug：`power-of-three`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.12 其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/power-of-three/solutions/2974674/o1-shu-xue-zuo-fa-yi-xing-gao-ding-pytho-w0uh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(1) 数学做法，一行搞定（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/power-of-three/solutions/2974674/o1-shu-xue-zuo-fa-yi-xing-gao-ding-pytho-w0uh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-shu-xue-zuo-fa-yi-xing-gao-ding-pytho-w0uh`
- topic id：`2974674`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

$3$ 的幂一定大于 $0$。如果 $n\le 0$，那么 $n$ 不是 $3$ 的幂。下文假定 $n>0$。

在本题的数据范围下，最大的 $3$ 的幂是 $3^{19} = 1162261467$。

> 怎么算的？比如可以写个循环，找最大的满足 $3^i\le 2^{31}-1$ 的整数 $i$。

- 如果 $n$ 是 $3$ 的幂。由于任何 $3$ 的幂都是 $3^{19}$ 的因子，所以 $3^{19}\bmod n = 0$。或者说 $n$ 的某个倍数等于 $3^{19}$。
- 如果 $n$ 不是 $3$ 的幂。那么 $n$ 必然有不等于 $3$ 的质因子，所以 $n$ 的倍数不可能等于 $3^{19}$ 这个只含质因子 $3$ 的数，所以 $3^{19}\bmod n \ne 0$。

因此，在本题数据范围下，当且仅当 $n>0$ 且 $3^{19}\bmod n = 0$ 时，$n$ 是 $3$ 的幂。

```py [sol-Python3]
class Solution:
    def isPowerOfThree(self, n: int) -> bool:
        return n > 0 and 1162261467 % n == 0
```

```java [sol-Java]
class Solution {
    public boolean isPowerOfThree(int n) {
        return n > 0 && 1162261467 % n == 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPowerOfThree(int n) {
        return n > 0 && 1162261467 % n == 0;
    }
};
```

```c [sol-C]
bool isPowerOfThree(int n) {
    return n > 0 && 1162261467 % n == 0;
}
```

```go [sol-Go]
func isPowerOfThree(n int) bool {
    return n > 0 && 1162261467%n == 0
}
```

```js [sol-JavaScript]
var isPowerOfThree = function(n) {
    return n > 0 && 1162261467 % n === 0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_power_of_three(n: i32) -> bool {
        n > 0 && 1162261467 % n == 0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

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
