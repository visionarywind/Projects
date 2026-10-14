# 868. 二进制间距

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-gap/
- 题目 slug：`binary-gap`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：1307
- 外部题解来源：https://leetcode.cn/problems/binary-gap/solutions/3906088/ji-suan-wei-ling-ge-shu-pythonjavacgocjs-jwgs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[计算尾零个数（Python/Java/C++/Go/C/JS/Rust）](https://leetcode.cn/problems/binary-gap/solutions/3906088/ji-suan-wei-ling-ge-shu-pythonjavacgocjs-jwgs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-suan-wei-ling-ge-shu-pythonjavacgocjs-jwgs`
- topic id：`3906088`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

以 $n = 1010010$ 为例。从右往左，我们需要计算 $1001$ 的间距 $3$，以及 $101$ 的间距 $2$：

1. 去掉 $n$ 末尾的 $10$，得到 $10100$。这一步可以先计算出 $n$ 的 $\text{lowbit} = 10$，然后把 $n$ 更新成 $\dfrac{n}{2\cdot \text{lowbit}}$。$\text{lowbit}$ 的原理请看 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。
2. 计算 $10100$ 的尾零个数加一，得到 $3$，即 $1001$ 的间距。然后把 $10100$ 右移 $3$ 位，得到 $10$。
3. 计算 $10$ 的尾零个数加一，得到 $2$，即 $101$ 的间距。然后把 $101$ 右移 $2$ 位，得到 $0$。算法结束。

```py [sol-Python3]
class Solution:
    def binaryGap(self, n: int) -> int:
        ans = 0
        n //= (n & -n) * 2  # 去掉 n 末尾的 100..0
        while n > 0:
            gap = (n & -n).bit_length()  # n 的尾零个数加一
            ans = max(ans, gap)
            n >>= gap  # 去掉 n 末尾的 100..0
        return ans
```

```java [sol-Java]
class Solution {
    public int binaryGap(int n) {
        int ans = 0;
        n /= (n & -n) * 2; // 去掉 n 末尾的 100..0
        while (n > 0) {
            int gap = Integer.numberOfTrailingZeros(n) + 1;
            ans = Math.max(ans, gap);
            n >>= gap; // 去掉 n 末尾的 100..0
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int binaryGap(int n) {
        int ans = 0;
        n /= (n & -n) * 2; // 去掉 n 末尾的 100..0
        while (n > 0) {
            int gap = countr_zero((uint32_t) n) + 1;
            ans = max(ans, gap);
            n >>= gap; // 去掉 n 末尾的 100..0
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int binaryGap(int n) {
    int ans = 0;
    n /= (n & -n) * 2; // 去掉 n 末尾的 100..0
    while (n > 0) {
        int gap = __builtin_ctz(n) + 1;
        ans = MAX(ans, gap);
        n >>= gap; // 去掉 n 末尾的 100..0
    }
    return ans;
}
```

```go [sol-Go]
func binaryGap(n int) (ans int) {
	n /= n & -n * 2 // 去掉 n 末尾的 100..0
	for n > 0 {
		gap := bits.TrailingZeros(uint(n)) + 1
		ans = max(ans, gap)
		n >>= gap // 去掉 n 末尾的 100..0
	}
	return
}
```

```js [sol-JavaScript]
var binaryGap = function(n) {
    let ans = 0;
    n /= (n & -n) * 2; // 去掉 n 末尾的 100..0
    while (n > 0) {
        const gap = 32 - Math.clz32(n & -n); // n 的尾零个数加一
        ans = Math.max(ans, gap);
        n >>= gap; // 去掉 n 末尾的 100..0
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn binary_gap(mut n: i32) -> i32 {
        let mut ans = 0;
        n /= (n & -n) * 2; // 去掉 n 末尾的 100..0
        while n > 0 {
            let gap = n.trailing_zeros() + 1;
            ans = ans.max(gap);
            n >>= gap; // 去掉 n 末尾的 100..0
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k)$，其中 $k$ 是 $n$ 二进制中的 $1$ 的个数。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面位运算题单的「**一、基础题**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
