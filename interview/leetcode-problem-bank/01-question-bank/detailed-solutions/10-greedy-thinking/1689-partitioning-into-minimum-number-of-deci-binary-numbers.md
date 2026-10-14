# 1689. 十-二进制数的最少数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/partitioning-into-minimum-number-of-deci-binary-numbers/
- 题目 slug：`partitioning-into-minimum-number-of-deci-binary-numbers`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1355
- 外部题解来源：https://leetcode.cn/problems/partitioning-into-minimum-number-of-deci-binary-numbers/solutions/3907414/nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-mvsu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/partitioning-into-minimum-number-of-deci-binary-numbers/solutions/3907414/nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-mvsu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-mvsu`
- topic id：`3907414`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

例如 $n=321$，其中最大的数字是 $3$。这个 $3$ 至少要拆分成 $3$ 个 $1$，即 $321=1\_\_ + 1\_\_ + 1\_\_$。对于 $n$ 中的其余数字 $d$，可以拆分成 $d$ 个 $1$ 和 $3-d$ 个 $0$，即 $2=1+1+0$ 和 $1=1+0+0$，填到对应的位置上，得到 $321 = 111 + 110 + 100$。

一般地，设 $m$ 为 $n$ 中的最大数字，那么答案为 $m$。构造方案为：设 $n$ 的第 $i$ 个数字为 $n_i$，那么拆分出的这 $m$ 个数的第 $i$ 位上，有 $n_i$ 个 $1$ 和 $m-n_i$ 个 $0$（填入顺序随意）。

```py [sol-Python3]
class Solution:
    def minPartitions(self, n: str) -> int:
        return int(max(n))
```

```java [sol-Java]
class Solution {
    public int minPartitions(String n) {
        int mx = 0;
        for (char ch : n.toCharArray()) {
            mx = Math.max(mx, ch);
        }
        return mx - '0';
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPartitions(string n) {
        return ranges::max(n) - '0';
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int minPartitions(char* n) {
    char mx = 0;
    for (int i = 0; n[i]; i++) {
        mx = MAX(mx, n[i]);
    }
    return mx - '0';
}
```

```go [sol-Go]
func minPartitions(n string) int {
	ans := rune(0)
	for _, ch := range n {
		ans = max(ans, ch)
	}
	return int(ans - '0')
}
```

```js [sol-JavaScript]
var minPartitions = function(n) {
    return Number(_.max(n));
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_partitions(n: String) -> i32 {
        (n.as_bytes().iter().max().unwrap() - b'0') as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(|n|)$，其中 $|n|$ 表示 $n$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心与思维题单的「**§5.2 脑筋急转弯**」。

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
