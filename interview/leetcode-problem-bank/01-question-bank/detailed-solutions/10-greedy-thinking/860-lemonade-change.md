# 860. 柠檬水找零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/lemonade-change/
- 题目 slug：`lemonade-change`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/lemonade-change/solutions/2353707/fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-37oe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论+简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/lemonade-change/solutions/2353707/fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-37oe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-37oe`
- topic id：`2353707`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

由于 $10$ 美元钞票只能用于 $20$ 美元的找零，而 $5$ 美元钞票既可以用于 $20$ 美元的找零，又可以用于 $10$ 美元的找零，更加通用（使用场景更多），所以如果可以用 $10$ 美元，应当优先用 $10$ 美元，其次用 $5$ 美元。如果优先用 $5$ 美元，可能会面临 $\textit{bills}[i]=10$ 无法找零的情况。

设当前有 $\textit{five}$ 张 $5$ 美元钞票，$\textit{ten}$ 张 $10$ 美元钞票。$20$ 美元的无需统计，因为它无法用来找零。

设 $b=\textit{bills}[i]$，分类讨论：

- 如果 $b=5$，无需找零，$\textit{five}$ 加一。
- 如果 $b=10$，返还 $5$ 美元，$\textit{five}$ 减一，$\textit{ten}$ 加一。
- 如果 $b=20$ 且 $\textit{ten}>0$，返还 $10+5$ 美元，$\textit{five}$ 和 $\textit{ten}$ 都减一。
- 如果 $b=20$ 且 $\textit{ten}=0$，返还 $5+5+5$ 美元，$\textit{five}$ 减三。
- 如果发现 $\textit{five}<0$，说明无法正确找零，返回 $\texttt{false}$。
- 如果中途没有返回 $\texttt{false}$，那么循环结束后返回 $\texttt{true}$。

```py [sol-Python3]
class Solution:
    def lemonadeChange(self, bills: List[int]) -> bool:
        five = ten = 0
        for b in bills:
            if b == 5:  # 无需找零
                five += 1
            elif b == 10:  # 返还 5
                five -= 1
                ten += 1
            elif ten > 0:  # 此时 b=20，优先用 10 美元，返还 10+5
                five -= 1
                ten -= 1
            else:  # 此时 b=20，返还 5+5+5
                five -= 3
            if five < 0:  # 无法正确找零
                return False
        return True
```

```java [sol-Java]
class Solution {
    public boolean lemonadeChange(int[] bills) {
        int five = 0;
        int ten = 0;
        for (int b : bills) {
            if (b == 5) { // 无需找零
                five++;
            } else if (b == 10) { // 返还 5
                five--;
                ten++;
            } else if (ten > 0) { // 此时 b=20，优先用 10 美元，返还 10+5
                five--;
                ten--;
            } else { // 此时 b=20，返还 5+5+5
                five -= 3;
            }
            if (five < 0) { // 无法正确找零
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool lemonadeChange(vector<int>& bills) {
        int five = 0, ten = 0;
        for (int b : bills) {
            if (b == 5) { // 无需找零
                five++;
            } else if (b == 10) { // 返还 5
                five--;
                ten++;
            } else if (ten > 0) { // 此时 b=20，优先用 10 美元，返还 10+5
                five--;
                ten--;
            } else { // 此时 b=20，返还 5+5+5
                five -= 3;
            }
            if (five < 0) { // 无法正确找零
                return false;
            }
        }
        return true;
    }
};
```

```c [sol-C]
bool lemonadeChange(int* bills, int billsSize) {
    int five = 0, ten = 0;
    for (int i = 0; i < billsSize; i++) {
        int b = bills[i];
        if (b == 5) { // 无需找零
            five++;
        } else if (b == 10) { // 返还 5
            five--;
            ten++;
        } else if (ten > 0) { // 此时 b=20，优先用 10 美元，返还 10+5
            five--;
            ten--;
        } else { // 此时 b=20，返还 5+5+5
            five -= 3;
        }
        if (five < 0) { // 无法正确找零
            return false;
        }
    }
    return true;
}
```

```go [sol-Go]
func lemonadeChange(bills []int) bool {
    five, ten := 0, 0
    for _, b := range bills {
        if b == 5 { // 无需找零
            five++
        } else if b == 10 { // 返还 5
            five--
            ten++
        } else if ten > 0 { // 此时 b=20，优先用 10 美元，返还 10+5
            five--
            ten--
        } else { // 此时 b=20，返还 5+5+5
            five -= 3
        }
        if five < 0 { // 无法正确找零
            return false
        }
    }
    return true
}
```

```js [sol-JavaScript]
var lemonadeChange = function(bills) {
    let five = 0, ten = 0;
    for (const b of bills) {
        if (b === 5) { // 无需找零
            five++;
        } else if (b === 10) { // 返还 5
            five--;
            ten++;
        } else if (ten > 0) { // 此时 b=20，优先用 10 美元，返还 10+5
            five--;
            ten--;
        } else { // 此时 b=20，返还 5+5+5
            five -= 3;
        }
        if (five < 0) { // 无法正确找零
            return false;
        }
    }
    return true;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn lemonade_change(bills: Vec<i32>) -> bool {
        let mut five = 0;
        let mut ten = 0;
        for b in bills {
            if b == 5 { // 无需找零
                five += 1;
            } else if b == 10 { // 返还 5
                five -= 1;
                ten += 1;
            } else if ten > 0 { // 此时 b=20，优先用 10 美元，返还 10+5
                five -= 1;
                ten -= 1;
            } else { // 此时 b=20，返还 5+5+5
                five -= 3;
            }
            if five < 0 { // 无法正确找零
                return false;
            }
        }
        true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{bills}$ 的长度。
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
