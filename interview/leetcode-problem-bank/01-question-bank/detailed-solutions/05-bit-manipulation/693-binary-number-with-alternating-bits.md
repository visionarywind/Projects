# 693. 交替位二进制数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-number-with-alternating-bits/
- 题目 slug：`binary-number-with-alternating-bits`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-number-with-alternating-bits/solutions/3904261/o1-zuo-fa-yuan-li-jiang-jie-pythonjavacc-wyki/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(1) 做法原理讲解（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/binary-number-with-alternating-bits/solutions/3904261/o1-zuo-fa-yuan-li-jiang-jie-pythonjavacc-wyki/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-zuo-fa-yuan-li-jiang-jie-pythonjavacc-wyki`
- topic id：`3904261`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

为了做到 $\mathcal{O}(1)$ 时间，我们需要快速判断**所有**相邻比特位是否都**不同**。

如何判断不同？用哪个位运算最合适？

用**异或运算**最合适。对于单个比特的异或，如果两个数不同，那么结果是 $1$；如果两个数相同，那么结果是 $0$。

如何对所有相邻比特位做异或运算？

例如 $n = 10101$，可以把 $n$ 右移一位，得到 $01010$，再与 $10101$ 做异或运算，计算的就是所有相邻比特位的异或值了。

如果异或结果全为 $1$，就说明所有相邻比特位都不同。

如何判断一个二进制数全为 $1$？

这相当于判断二进制数加一后，是否为 [231. 2 的幂](https://leetcode.cn/problems/power-of-two/)。

设 $x$ 为 `(n >> 1) ^ n`，如果 `(x + 1) & x` 等于 $0$，那么说明 $x$ 全为 $1$。

```py [sol-Python3]
class Solution:
    def hasAlternatingBits(self, n: int) -> bool:
        x = (n >> 1) ^ n
        return (x + 1) & x == 0
```

```java [sol-Java]
class Solution {
    public boolean hasAlternatingBits(int n) {
        int x = (n >> 1) ^ n;
        return ((x + 1) & x) == 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool hasAlternatingBits(int n) {
        uint32_t x = (n >> 1) ^ n;
        return ((x + 1) & x) == 0;
    }
};
```

```c [sol-C]
bool hasAlternatingBits(int n) {
    uint32_t x = (n >> 1) ^ n;
    return ((x + 1) & x) == 0;
}
```

```go [sol-Go]
func hasAlternatingBits(n int) bool {
	x := n>>1 ^ n
	return (x+1)&x == 0
}
```

```js [sol-JavaScript]
var hasAlternatingBits = function(n) {
    const x = (n >> 1) ^ n;
    return ((x + 1) & x) === 0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn has_alternating_bits(n: i32) -> bool {
        let x = (n >> 1) ^ n;
        (x + 1) & x == 0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面位运算题单的「**一、基础题**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
