# 2652. 倍数求和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-multiples/
- 题目 slug：`sum-multiples`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.3 容斥原理
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sum-multiples/solutions/2241283/o1-rong-chi-yuan-li-by-endlesscheng-yxc4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数学做法：O(1) 容斥原理（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/sum-multiples/solutions/2241283/o1-rong-chi-yuan-li-by-endlesscheng-yxc4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-rong-chi-yuan-li-by-endlesscheng-yxc4`
- topic id：`2241283`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

在 $[1,n]$ 中，$m$ 的倍数有 $k = \left\lfloor\dfrac{n}{m}\right\rfloor$ 个，即

$$
m,2m,\cdots,km
$$

结合等差数列求和公式，这些数的和为

$$
s(m) = \dfrac{k(k+1)}{2} \cdot m
$$

再结合**容斥原理**，可以算出 $3$ **或** $5$ **或** $7$ 的倍数之和，即

$$
s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105)
$$

具体请看 [视频讲解](https://www.bilibili.com/video/BV1Bs4y1A7Wa/) 第二题。

```py [sol-Python3]
class Solution:
    def sumOfMultiples(self, n: int) -> int:
        def s(m: int) -> int:
            return n // m * (n // m + 1) // 2 * m
        return s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105)
```

```java [sol-Java]
class Solution {
    public int sumOfMultiples(int n) {
        return s(n, 3) + s(n, 5) + s(n, 7) - s(n, 15) - s(n, 21) - s(n, 35) + s(n, 105);
    }

    private int s(int n, int m) {
        return n / m * (n / m + 1) / 2 * m;
    }
}
```

```cpp [sol-C++]
class Solution {
    int s(int n, int m) {
        return n / m * (n / m + 1) / 2 * m;
    }
public:
    int sumOfMultiples(int n) {
        return s(n, 3) + s(n, 5) + s(n, 7) - s(n, 15) - s(n, 21) - s(n, 35) + s(n, 105);
    }
};
```

```go [sol-Go]
func s(n, m int) int {
	return n / m * (n/m + 1) / 2 * m
}

func sumOfMultiples(n int) int {
	return s(n, 3) + s(n, 5) + s(n, 7) - s(n, 15) - s(n, 21) - s(n, 35) + s(n, 105)
}
```

```js [sol-JavaScript]
var sumOfMultiples = function(n) {
    function s(m) {
        return Math.floor(n / m) * (Math.floor(n / m) + 1) / 2 * m;
    }
    return s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn sum_of_multiples(n: i32) -> i32 {
        let s = |m| n / m * (n / m + 1) / 2 * m;
        s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [1201. 丑数 III](https://leetcode.cn/problems/ugly-number-iii/)
