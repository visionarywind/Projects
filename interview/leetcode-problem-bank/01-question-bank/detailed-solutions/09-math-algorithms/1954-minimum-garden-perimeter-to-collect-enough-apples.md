# 1954. 收集足够苹果的最小花园周长

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-garden-perimeter-to-collect-enough-apples/
- 题目 slug：`minimum-garden-perimeter-to-collect-enough-apples`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1759
- 外部题解来源：https://leetcode.cn/problems/minimum-garden-perimeter-to-collect-enough-apples/solutions/2577652/tu-jie-o1-zuo-fa-pythonjavacgojsrust-by-oms4k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】O(1) 做法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-garden-perimeter-to-collect-enough-apples/solutions/2577652/tu-jie-o1-zuo-fa-pythonjavacgojsrust-by-oms4k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-o1-zuo-fa-pythonjavacgojsrust-by-oms4k`
- topic id：`2577652`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

![LC1954-c.png](https://pic.leetcode.cn/1703216655-CnqSqn-LC1954-c.png)

设正方形的**周长**为 $8n$，则其**边长**为 $2n$。

问题相当于求最小的 $n$，满足

$$
2n(n+1)(2n+1)\ge \textit{neededApples}
$$

上式变形为

$$
n(n+1)(n+\dfrac{1}{2})\ge \dfrac{1}{4}\textit{neededApples}
$$

设 $m = \left\lfloor \sqrt[3]{\dfrac{1}{4}\textit{neededApples}} \right\rfloor$。

- 由于 $(m-1)m(m-\dfrac{1}{2}) < m^3 \le \dfrac{1}{4}\textit{neededApples}$，所以 $m-1$ 必不满足要求。
- 由于 $(m+1)(m+2)(m+\dfrac{3}{2}) > (m+1)^3 > \dfrac{1}{4}\textit{neededApples}$，所以 $m+1$ 必满足要求。注意 $m+1>\left\lceil\sqrt[3]{\dfrac{1}{4}\textit{neededApples}}\right\rceil$。
- $m$ 是否满足要求？计算一下就知道了。

因此，直接计算出 $n = \left\lfloor \sqrt[3]{\dfrac{1}{4}\textit{neededApples}} \right\rfloor$，如果 $2n(n+1)(2n+1)< \textit{neededApples}$ 则将 $n$ 加一。

> 注：在本题的数据范围下，`cbrt` 算出的整数部分是正确的，不会因为浮点误差导致对 `xxx.999999` 下取整的错误。

```py [sol-Python3]
class Solution:
    def minimumPerimeter(self, neededApples: int) -> int:
        n = int(cbrt(neededApples / 4))
        if 2 * n * (n + 1) * (2 * n + 1) < neededApples:
            n += 1
        return 8 * n
```

```java [sol-Java]
class Solution {
    public long minimumPerimeter(long neededApples) {
        long n = (long) Math.cbrt(neededApples / 4.0);
        if (2 * n * (n + 1) * (2 * n + 1) < neededApples) {
            n++;
        }
        return 8 * n;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumPerimeter(long long neededApples) {
        long long n = cbrt(neededApples / 4.0);
        if (2 * n * (n + 1) * (2 * n + 1) < neededApples) {
            n++;
        }
        return 8 * n;
    }
};
```

```go [sol-Go]
func minimumPerimeter(neededApples int64) int64 {
    n := int64(math.Cbrt(float64(neededApples) / 4))
    if 2*n*(n+1)*(2*n+1) < neededApples {
        n++
    }
    return 8 * n
}
```

```js [sol-JavaScript]
var minimumPerimeter = function(neededApples) {
    let n = Math.floor(Math.cbrt(neededApples / 4));
    if (2 * n * (n + 1) * (2 * n + 1) < neededApples) {
        n++;
    }
    return 8 * n;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_perimeter(needed_apples: i64) -> i64 {
        let mut n = (needed_apples as f64 / 4.0).cbrt() as i64;
        if 2 * n * (n + 1) * (2 * n + 1) < needed_apples {
            n += 1;
        }
        8 * n
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。开立方有专用的计算函数 `cbrt`，时间可以视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

#### 相似题目

- [1739. 放置盒子](https://leetcode.cn/problems/building-boxes/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
