# 633. 平方数之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-square-numbers/
- 题目 slug：`sum-of-square-numbers`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sum-of-square-numbers/solutions/2973811/liang-chong-fang-fa-mei-ju-shuang-zhi-zh-c26z/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举/双指针（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sum-of-square-numbers/solutions/2973811/liang-chong-fang-fa-mei-ju-shuang-zhi-zh-c26z/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-mei-ju-shuang-zhi-zh-c26z`
- topic id：`2973811`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：枚举

枚举 $a=0,1,2,\ldots$ 把等式变成

$$
b^2 = c - a^2
$$

即

$$
b = \sqrt{c - a^2}
$$

如果 $b$ 是整数，返回 $\texttt{true}$。

或者，设 $b' =  \lfloor\sqrt{c - a^2}\rfloor$，如果 $a^2+b'^2=c$ 成立，返回 $\texttt{true}$。

### 细节

$a$ 最大枚举到哪？

如果枚举到 $2a^2 > c$ 时，仍然没有找到符合等式的 $a$ 和 $b$，则停止枚举，返回 $\texttt{false}$。

为什么？此时 $a^2 > c-a^2=b^2$，假如继续枚举能找到符合等式的 $a$ 和 $b$，比如 $a=5,\ b=3$，那么之前在枚举到 $a=3$ 时，也能发现 $a=3,\ b=5$ 符合等式，矛盾。所以当枚举到 $2a^2 > c$ 时，后面不可能找到符合等式的 $a$ 和 $b$。

对于 C++ 等部分语言，如果直接计算 $2a^2$，可能会发生溢出。可以把循环条件 $2a^2 \le c$ 改成等价的 $a^2\le \left\lfloor\dfrac{c}{2}\right\rfloor$，从而避免溢出。

```py [sol-Python3]
class Solution:
    def judgeSquareSum(self, c: int) -> bool:
        a = 0
        while a * a * 2 <= c:
            b = isqrt(c - a * a)
            if a * a + b * b == c:
                return True
            a += 1
        return False
```

```java [sol-Java]
class Solution {
    public boolean judgeSquareSum(int c) {
        for (int a = 0; a * a <= c / 2; a++) {
            int b = (int) Math.sqrt(c - a * a);
            if (a * a + b * b == c) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool judgeSquareSum(int c) {
        for (int a = 0; a * a <= c / 2; a++) {
            int b = sqrt(c - a * a);
            if (a * a + b * b == c) {
                return true;
            }
        }
        return false;
    }
};
```

```c [sol-C]
bool judgeSquareSum(int c) {
    for (int a = 0; a * a <= c / 2; a++) {
        int b = sqrt(c - a * a);
        if (a * a + b * b == c) {
            return true;
        }
    }
    return false;
}
```

```go [sol-Go]
func judgeSquareSum(c int) bool {
    for a := 0; a*a <= c/2; a++ {
        b := int(math.Sqrt(float64(c - a*a)))
        if a*a+b*b == c {
            return true
        }
    }
    return false
}
```

```js [sol-JavaScript]
var judgeSquareSum = function(c) {
    for (let a = 0; a * a * 2 <= c; a++) {
        const b = Math.floor(Math.sqrt(c - a * a));
        if (a * a + b * b === c) {
            return true;
        }
    }
    return false;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn judge_square_sum(c: i32) -> bool {
        for a in 0.. {
            if a * a > c / 2 {
                break;
            }
            let b = ((c - a * a) as f64).sqrt() as i32;
            if a * a + b * b == c {
                return true;
            }
        }
        false
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\sqrt c)$。开平方有专门的 CPU 指令，可以视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：相向双指针

设 $k = \lfloor\sqrt c\rfloor$。本题相当于给你一个有序数组 $[0,1,4,9,16,\ldots,k^2]$，判断是否存在**两数之和**等于 $c$。

和 [167. 两数之和 II - 输入有序数组](https://leetcode.cn/problems/two-sum-ii-input-array-is-sorted/) 一样，使用**相向双指针**解决，原理请看视频[【基础算法精讲 01】](https://www.bilibili.com/video/BV1bP411c7oJ/)。

### 细节

注意 $a$ 和 $b$ 可以相等。比如 $c=2$，是两个 $1$ 的平方和，此时 $a=b=1$。

所以下面代码的循环条件是 $a\le b$，等号是必要的。

对于 C++ 等部分语言，如果直接计算 $a^2+b^2$，可能会发生溢出。本来 $a^2+b^2 > c$，溢出后 $a^2+b^2$ 变成负数，$a^2+b^2 < c$ 反而成立了，导致我们移动了错误的指针。

可以把判断条件改为 `a * a == c - b * b` 以及 `a * a < c - b * b`，从而避免溢出。

### 答疑

**问**：为什么 $a^2+b^2 < c$ 时，可以把 $a$ 加一？不会错过答案吗？

**答**：原理在视频中讲了。对于本题来说，$a^2+b^2 < c$ 成立，同时还意味着 $a^2+(b-1)^2 < c, a^2+(b-2)^2 < c,\ldots$ 都成立，这意味着 $a$ 和 $[a,b]$ 中的任意整数 $b'$ 都满足 $a^2+b'^2 < c$，所以 $a^2$ 不可能在两数之和的答案中，移动左指针 $a$。同理，$a^2+b^2 > c$ 成立，同时还意味着 $(a+1)^2+b^2 > c, (a+2)^2+b^2 > c,\ldots$ 都成立，这意味着 $b$ 和 $[a,b]$ 中的任意整数 $a'$ 都满足 $a'^2+b^2 > c$，所以 $b^2$ 不可能在两数之和的答案中，移动右指针 $b$。

```py [sol-Python3]
class Solution:
    def judgeSquareSum(self, c: int) -> bool:
        a, b = 0, isqrt(c)
        while a <= b:
            s = a * a + b * b
            if s == c:
                return True
            if s < c:
                a += 1
            else:
                b -= 1
        return False
```

```java [sol-Java]
class Solution {
    public boolean judgeSquareSum(int c) {
        int a = 0;
        int b = (int) Math.sqrt(c);
        while (a <= b) {
            if (a * a == c - b * b) { // 避免溢出
                return true;
            }
            if (a * a < c - b * b) {
                a++;
            } else {
                b--;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool judgeSquareSum(int c) {
        int a = 0, b = sqrt(c);
        while (a <= b) {
            if (a * a == c - b * b) { // 避免溢出
                return true;
            }
            if (a * a < c - b * b) {
                a++;
            } else {
                b--;
            }
        }
        return false;
    }
};
```

```c [sol-C]
bool judgeSquareSum(int c) {
    int a = 0, b = sqrt(c);
    while (a <= b) {
        if (a * a == c - b * b) { // 避免溢出
            return true;
        }
        if (a * a < c - b * b) {
            a++;
        } else {
            b--;
        }
    }
    return false;
}
```

```go [sol-Go]
func judgeSquareSum(c int) bool {
    a, b := 0, int(math.Sqrt(float64(c)))
    for a <= b {
        s := a*a + b*b
        if s == c {
            return true
        }
        if s < c {
            a++
        } else {
            b--
        }
    }
    return false
}
```

```js [sol-JavaScript]
var judgeSquareSum = function(c) {
    let a = 0, b = Math.floor(Math.sqrt(c));
    while (a <= b) {
        const s = a * a + b * b;
        if (s === c) {
            return true;
        }
        if (s < c) {
            a++;
        } else {
            b--;
        }
    }
    return false;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn judge_square_sum(c: i32) -> bool {
        let mut a = 0;
        let mut b = (c as f64).sqrt() as i32;
        while a <= b {
            if a * a == c - b * b {
                return true;
            }
            if a * a < c - b * b {
                a += 1;
            } else {
                b -= 1;
            }
        }
        false
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\sqrt c)$。
- 空间复杂度：$\mathcal{O}(1)$。

更多相似题目，见下面双指针题单中的「**§3.1 相向双指针**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
