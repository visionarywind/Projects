# 2549. 统计桌面上的不同数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-distinct-numbers-on-board/
- 题目 slug：`count-distinct-numbers-on-board`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1266
- 外部题解来源：https://leetcode.cn/problems/count-distinct-numbers-on-board/solutions/2080589/shu-xue-o1-jie-by-endlesscheng-hucn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数学 O(1) 解（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/count-distinct-numbers-on-board/solutions/2080589/shu-xue-o1-jie-by-endlesscheng-hucn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-xue-o1-jie-by-endlesscheng-hucn`
- topic id：`2080589`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

由于 $n \bmod (n-1) = 1$ 一定满足要求，我们可以从 $n$ 开始，不断生成 $n-1,n-2,\cdots$，最后 $[2,n]$ 中的数字都会在桌面上，这有 $n-1$ 个。

注意特判 $n=1$ 的情况，此时答案为 $1$。

```py [sol-Python3]
class Solution:
    def distinctIntegers(self, n: int) -> int:
        return n - 1 if n > 1 else 1  # max(n - 1, 1)
```

```java [sol-Java]
class Solution {
    public int distinctIntegers(int n) {
        return Math.max(n - 1, 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int distinctIntegers(int n) {
        return max(n - 1, 1);
    }
};
```

```go [sol-Go]
func distinctIntegers(n int) int {
	return max(n-1, 1)
}
```

```js [sol-JavaScript]
var distinctIntegers = function(n) {
    return Math.max(n - 1, 1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn distinct_integers(n: i32) -> i32 {
        1.max(n - 1)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

- [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
- [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
- [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
- [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
- [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
- [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
