# 1007. 行相等的最少多米诺旋转

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-domino-rotations-for-equal-row/
- 题目 slug：`minimum-domino-rotations-for-equal-row`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1541
- 外部题解来源：https://leetcode.cn/problems/minimum-domino-rotations-for-equal-row/solutions/3042326/du-bian-cheng-tops0-huo-zhe-bottoms0pyth-zvnj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[都变成 tops[0] 或者 bottoms[0]（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-domino-rotations-for-equal-row/solutions/3042326/du-bian-cheng-tops0-huo-zhe-bottoms0pyth-zvnj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`du-bian-cheng-tops0-huo-zhe-bottoms0pyth-zvnj`
- topic id：`3042326`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

目标是让第一排或者第二排的所有数都相同，这个「所有」必然包含**第一个骨牌中的数**。所以要么都变成 $\textit{tops}[0]$，要么都变成 $\textit{bottoms}[0]$。计算这两种情况，取最小值。

写一个函数 $\text{minRot}(\textit{target})$，表示都变成 $\textit{target}$ 的最小旋转次数。我们要计算的是 $\min(\text{minRot}(\textit{tops}[0]),\text{minRot}(\textit{bottoms}[0]))$。

定义变量 $\textit{toTop}$，表示把第一排（上半）都变成 $\textit{target}$ 的最小旋转次数，初始值为 $0$。

定义变量 $\textit{toBottom}$，表示把第二排（下半）都变成 $\textit{target}$ 的最小旋转次数，初始值为 $0$。

遍历骨牌，设 $x=\textit{tops}[i]$ 和 $y=\textit{bottoms}[i]$，分类讨论：

- 如果 $x\ne \textit{target}$ 且 $y\ne \textit{target}$，无法满足要求，返回 $\infty$。
- 如果 $x\ne \textit{target}$ 且 $y= \textit{target}$，旋转到上半的次数 $\textit{toTop}$ 加一。
- 如果 $x= \textit{target}$ 且 $y\ne \textit{target}$，旋转到下半的次数 $\textit{toBottom}$ 加一。
- 如果 $x= \textit{target}$ 且 $y= \textit{target}$，无需旋转。

遍历结束，都变成 $\textit{target}$ 的最小旋转次数为 $\min(\textit{toTop},\textit{toBottom})$，即 $\text{minRot}(\textit{target})$ 的返回值。

如果 $\min(\text{minRot}(\textit{tops}[0]),\text{minRot}(\textit{bottoms}[0]))=\infty$，说明两种情况都无法满足要求，返回 $-1$。

```py [sol-Python3]
class Solution:
    def minDominoRotations(self, tops: List[int], bottoms: List[int]) -> int:
        def min_rot(target: int) -> int:
            to_top = to_bottom = 0
            for x, y in zip(tops, bottoms):
                if x != target and y != target:
                    return inf
                if x != target:
                    to_top += 1  # 把 y 旋转到上半
                elif y != target:
                    to_bottom += 1  # 把 x 旋转到下半
            return min(to_top, to_bottom)

        ans = min(min_rot(tops[0]), min_rot(bottoms[0]))
        return -1 if ans == inf else ans
```

```java [sol-Java]
class Solution {
    public int minDominoRotations(int[] tops, int[] bottoms) {
        int ans = Math.min(minRot(tops, bottoms, tops[0]), minRot(tops, bottoms, bottoms[0]));
        return ans == Integer.MAX_VALUE ? -1 : ans;
    }

    private int minRot(int[] tops, int[] bottoms, int target) {
        int toTop = 0;
        int toBottom = 0;
        for (int i = 0; i < tops.length; i++) {
            int x = tops[i];
            int y = bottoms[i];
            if (x != target && y != target) {
                return Integer.MAX_VALUE;
            }
            if (x != target) {
                toTop++; // 把 y 旋转到上半
            } else if (y != target) {
                toBottom++; // 把 x 旋转到下半
            }
        }
        return Math.min(toTop, toBottom);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDominoRotations(vector<int>& tops, vector<int>& bottoms) {
        auto min_rot = [&](int target) -> int {
            int to_top = 0, to_bottom = 0;
            for (int i = 0; i < tops.size(); i++) {
                int x = tops[i], y = bottoms[i];
                if (x != target && y != target) {
                    return INT_MAX;
                }
                if (x != target) {
                    to_top++; // 把 y 旋转到上半
                } else if (y != target) {
                    to_bottom++; // 把 x 旋转到下半
                }
            }
            return min(to_top, to_bottom);
        };

        int ans = min(min_rot(tops[0]), min_rot(bottoms[0]));
        return ans == INT_MAX ? -1 : ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minDominoRotations(int* tops, int topsSize, int* bottoms, int bottomsSize) {
    int min_rot(int target) {
        int to_top = 0, to_bottom = 0;
        for (int i = 0; i < topsSize; i++) {
            int x = tops[i], y = bottoms[i];
            if (x != target && y != target) {
                return INT_MAX;
            }
            if (x != target) {
                to_top++; // 把 y 旋转到上半
            } else if (y != target) {
                to_bottom++; // 把 x 旋转到下半
            }
        }
        return MIN(to_top, to_bottom);
    }

    int res1 = min_rot(tops[0]);
    int res2 = min_rot(bottoms[0]);
    int ans = MIN(res1, res2);
    return ans == INT_MAX ? -1 : ans;
}
```

```go [sol-Go]
func minDominoRotations(tops, bottoms []int) int {
    minRot := func(target int) int {
        toTop, toBottom := 0, 0
        for i, x := range tops {
            y := bottoms[i]
            if x != target && y != target {
                return math.MaxInt
            }
            if x != target {
                toTop++ // 把 y 旋转到上半
            } else if y != target {
                toBottom++ // 把 x 旋转到下半
            }
        }
        return min(toTop, toBottom)
    }

    ans := min(minRot(tops[0]), minRot(bottoms[0]))
    if ans == math.MaxInt {
        return -1
    }
    return ans
}
```

```js [sol-JavaScript]
var minDominoRotations = function(tops, bottoms) {
    function minRot(target) {
        let toTop = 0, toBottom = 0;
        for (let i = 0; i < tops.length; i++) {
            const x = tops[i], y = bottoms[i];
            if (x !== target && y !== target) {
                return Infinity;
            }
            if (x !== target) {
                toTop++; // 把 y 旋转到上半
            } else if (y !== target) {
                toBottom++; // 把 x 旋转到下半
            }
        }
        return Math.min(toTop, toBottom);
    }

    const ans = Math.min(minRot(tops[0]), minRot(bottoms[0]));
    return ans === Infinity ? -1 : ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_domino_rotations(tops: Vec<i32>, bottoms: Vec<i32>) -> i32 {
        let min_rot = |target: i32| -> i32 {
            let mut to_top = 0;
            let mut to_bottom = 0;
            for (&x, &y) in tops.iter().zip(bottoms.iter()) {
                if x != target && y != target {
                    return i32::MAX;
                }
                if x != target {
                    to_top += 1; // 把 y 旋转到上半
                } else if y != target {
                    to_bottom += 1; // 把 x 旋转到下半
                }
            }
            to_top.min(to_bottom)
        };

        let ans = min_rot(tops[0]).min(min_rot(bottoms[0]));
        if ans == i32::MAX { -1 } else { ans }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{tops}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果可以去掉一块多米诺骨牌呢？

欢迎在评论区发表你的思路/代码。

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
