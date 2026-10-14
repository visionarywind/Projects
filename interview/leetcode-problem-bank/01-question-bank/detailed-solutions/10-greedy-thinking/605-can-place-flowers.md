# 605. 种花问题

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/can-place-flowers/
- 题目 slug：`can-place-flowers`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1400
- 外部题解来源：https://leetcode.cn/problems/can-place-flowers/solutions/2463018/ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-6a6k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[本题最简单写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/can-place-flowers/solutions/2463018/ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-6a6k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-6a6k`
- topic id：`2463018`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

从左到右遍历数组，**能种花就立刻种花**。

如何判断能否种花？由于「花不能种植在相邻的地块上」，如果要在下标 $i$ 处种花，需要满足 $\textit{flowerbed}[i-1],\textit{flowerbed}[i],\textit{flowerbed}[i+1]$ 均为 $0$。

每种一朵花，就把 $n$ 减少 $1$。如果最后 $n\le 0$，则返回 $\texttt{true}$，否则返回 $\texttt{false}$。

为了简化判断逻辑，可以在数组的开头和末尾各插入一个 $0$。

```py [sol-Python3]
class Solution:
    def canPlaceFlowers(self, flowerbed: List[int], n: int) -> bool:
        flowerbed = [0] + flowerbed + [0]
        for i in range(1, len(flowerbed) - 1):
            if flowerbed[i - 1] == 0 and flowerbed[i] == 0 and flowerbed[i + 1] == 0:
                flowerbed[i] = 1  # 种花！
                n -= 1
        return n <= 0
```

```java [sol-Java]
class Solution {
    public boolean canPlaceFlowers(int[] flowerbed, int n) {
        int[] a = new int[flowerbed.length + 2];
        System.arraycopy(flowerbed, 0, a, 1, flowerbed.length);
        for (int i = 1; i < a.length - 1; i++) {
            if (a[i - 1] == 0 && a[i] == 0 && a[i + 1] == 0) {
                a[i] = 1; // 种花！
                n--;
            }
        }
        return n <= 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canPlaceFlowers(vector<int> &flowerbed, int n) {
        flowerbed.insert(flowerbed.begin(), 0);
        flowerbed.push_back(0);
        for (int i = 1; i + 1 < flowerbed.size(); i++) {
            if (flowerbed[i - 1] == 0 && flowerbed[i] == 0 && flowerbed[i + 1] == 0) {
                flowerbed[i] = 1; // 种花！
                n--;
            }
        }
        return n <= 0;
    }
};
```

```go [sol-Go]
func canPlaceFlowers(flowerbed []int, n int) bool {
    flowerbed = append(append([]int{0}, flowerbed...), 0)
    for i := 1; i < len(flowerbed)-1; i++ {
        if flowerbed[i-1] == 0 && flowerbed[i] == 0 && flowerbed[i+1] == 0 {
            flowerbed[i] = 1 // 种花！
            n--
        }
    }
    return n <= 0
}
```

```js [sol-JavaScript]
var canPlaceFlowers = function (flowerbed, n) {
    flowerbed.unshift(0);
    flowerbed.push(0);
    for (let i = 1; i < flowerbed.length - 1; i++) {
        if (flowerbed[i - 1] === 0 && flowerbed[i] === 0 && flowerbed[i + 1] === 0) {
            flowerbed[i] = 1; // 种花！
            n--;
        }
    }
    return n <= 0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn can_place_flowers(flowerbed: Vec<i32>, n: i32) -> bool {
        let mut a = vec![0; flowerbed.len() + 2];
        a[1..(flowerbed.len() + 1)].clone_from_slice(&flowerbed);
        let mut n = n;
        for i in 1..(a.len() - 1) {
            if a[i - 1] == 0 && a[i] == 0 && a[i + 1] == 0 {
                a[i] = 1; // 种花！
                n -= 1;
            }
        }
        n <= 0
    }
}
```

如果不想创建新数组/修改原数组，也可以这样写：

```py [sol-Python3]
class Solution:
    def canPlaceFlowers(self, flowerbed: List[int], n: int) -> bool:
        i, m = 0, len(flowerbed)
        while i < m:
            if (i == 0 or flowerbed[i - 1] == 0) and flowerbed[i] == 0 and (i == m - 1 or flowerbed[i + 1] == 0):
                n -= 1
                i += 2  # 下一个位置肯定不能种花，直接跳过
            else:
                i += 1
        return n <= 0
```

```java [sol-Java]
class Solution {
    public boolean canPlaceFlowers(int[] flowerbed, int n) {
        int m = flowerbed.length;
        for (int i = 0; i < m; i++) {
            if ((i == 0 || flowerbed[i - 1] == 0) && flowerbed[i] == 0 && (i == m - 1 || flowerbed[i + 1] == 0)) {
                n--;
                i++; // 下一个位置肯定不能种花，直接跳过
            }
        }
        return n <= 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canPlaceFlowers(vector<int> &flowerbed, int n) {
        int m = flowerbed.size();
        for (int i = 0; i < m; i++) {
            if ((i == 0 || flowerbed[i - 1] == 0) && flowerbed[i] == 0 && (i == m - 1 || flowerbed[i + 1] == 0)) {
                n--;
                i++; // 下一个位置肯定不能种花，直接跳过
            }
        }
        return n <= 0;
    }
};
```

```go [sol-Go]
func canPlaceFlowers(flowerbed []int, n int) bool {
    m := len(flowerbed)
    for i := 0; i < m; i++ {
        if (i == 0 || flowerbed[i-1] == 0) && flowerbed[i] == 0 && (i == m-1 || flowerbed[i+1] == 0) {
            n--
            i++ // 下一个位置肯定不能种花，直接跳过
        }
    }
    return n <= 0
}
```

```js [sol-JavaScript]
var canPlaceFlowers = function (flowerbed, n) {
    const m = flowerbed.length;
    for (let i = 0; i < m; i++) {
        if ((i === 0 || flowerbed[i - 1] === 0) && flowerbed[i] === 0 && (i === m - 1 || flowerbed[i + 1] === 0)) {
            n--;
            i++; // 下一个位置肯定不能种花，直接跳过
        }
    }
    return n <= 0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn can_place_flowers(flowerbed: Vec<i32>, n: i32) -> bool {
        let m = flowerbed.len();
        let mut n = n;
        let mut i = 0;
        while i < m {
            if (i == 0 || flowerbed[i - 1] == 0) && flowerbed[i] == 0 && (i == m - 1 || flowerbed[i + 1] == 0) {
                n -= 1;
                i += 2; // 下一个位置肯定不能种花，直接跳过
            } else {
                i += 1;
            }
        }
        n <= 0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{flowerbed}$ 的长度。
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
