# 2833. 距离原点最远的点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/furthest-point-from-origin/
- 题目 slug：`furthest-point-from-origin`
- 来源专题：贪心与思维
- 来源分类路径：八、其他
- 难度分：1294
- 外部题解来源：https://leetcode.cn/problems/furthest-point-from-origin/solutions/2413317/nao-jin-ji-zhuan-wan-yi-xing-dai-ma-by-e-yfn0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/furthest-point-from-origin/solutions/2413317/nao-jin-ji-zhuan-wan-yi-xing-dai-ma-by-e-yfn0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-yi-xing-dai-ma-by-e-yfn0`
- topic id：`2413317`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

示例 2 的 $\textit{moves} = \texttt{\_R\_\_LL\_}$：

- 先只看 $\texttt{R}$ 和 $\texttt{L}$，往右走了 $1$ 步，再往左走 $2$ 步，相当于往左走了 $1$ 步，位于数轴的 $-1$。
- 还剩下 $4$ 个 $\texttt{\_}$，怎么走最优？当然是继续往左走啦，继续往左走 $4$ 步，最终位于 $-5$。

设 $\textit{cntR}$ 为 $\texttt{R}$ 的个数，$\textit{cntL}$ 为 $\texttt{L}$ 的个数，那么 $\texttt{\_}$ 的个数为 $n - \textit{cntR} - \textit{cntL}$。先只看 $\texttt{R}$ 和 $\texttt{L}$，我们到原点的距离为 $|\textit{cntR} - \textit{cntL}|$。然后继续走 $n - \textit{cntR} - \textit{cntL}$ 步，最终答案为

$$
|\textit{cntR} - \textit{cntL}| + n - \textit{cntR} - \textit{cntL}
$$

```py [sol-Python3]
class Solution:
    def furthestDistanceFromOrigin(self, moves: str) -> int:
        cnt_r = moves.count('R')
        cnt_l = moves.count('L')
        return abs(cnt_r - cnt_l) + len(moves) - cnt_r - cnt_l
```

```java [sol-Java]
class Solution {
    public int furthestDistanceFromOrigin(String moves) {
        int cntR = 0;
        int cntL = 0;
        for (char c : moves.toCharArray()) {
            if (c == 'R') {
                cntR++;
            } else if (c == 'L') {
                cntL++;
            }
        }
        return Math.abs(cntR - cntL) + moves.length() - cntR - cntL;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int furthestDistanceFromOrigin(string moves) {
        int cnt_r = ranges::count(moves, 'R');
        int cnt_l = ranges::count(moves, 'L');
        return abs(cnt_r - cnt_l) + moves.size() - cnt_r - cnt_l;
    }
};
```

```c [sol-C]
int furthestDistanceFromOrigin(char* moves) {
    int cnt_r = 0;
    int cnt_l = 0;
    int i = 0;
    for (; moves[i]; i++) {
        if (moves[i] == 'R') {
            cnt_r++;
        } else if (moves[i] == 'L') {
            cnt_l++;
        }
    }
    return abs(cnt_r - cnt_l) + i - cnt_r - cnt_l;
}
```

```go [sol-Go]
func furthestDistanceFromOrigin(moves string) int {
	cntR := strings.Count(moves, "R")
	cntL := strings.Count(moves, "L")
	return abs(cntR-cntL) + len(moves) - cntR - cntL
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

```js [sol-JavaScript]
var furthestDistanceFromOrigin = function(moves) {
    const cntR = [...moves].filter(c => c === 'R').length;
    const cntL = [...moves].filter(c => c === 'L').length;
    return Math.abs(cntR - cntL) + moves.length - cntR - cntL;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn furthest_distance_from_origin(moves: String) -> i32 {
        let cnt_r = moves.bytes().filter(|&c| c == b'R').count() as i32;
        let cnt_l = moves.bytes().filter(|&c| c == b'L').count() as i32;
        (cnt_r - cnt_l).abs() + moves.len() as i32 - cnt_r - cnt_l
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{moves}$ 的长度。
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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
