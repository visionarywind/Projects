# 344. 反转字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-string/
- 题目 slug：`reverse-string`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.1 反转字符串
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/reverse-string/solutions/2376290/ji-chong-bu-tong-de-xie-fa-pythonjavacgo-9trb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种写法：双指针/单指针/库函数（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/reverse-string/solutions/2376290/ji-chong-bu-tong-de-xie-fa-pythonjavacgo-9trb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-chong-bu-tong-de-xie-fa-pythonjavacgo-9trb`
- topic id：`2376290`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

设 $s$ 长度为 $n$。反转可以看成是交换 $s[0]$ 和 $s[n-1]$，交换 $s[1]$ 和 $s[n-2]$，交换 $s[2]$ 和 $s[n-3]$，依此类推。

## 双指针写法

初始化两个指针 $\textit{left}=0$ 和 $\textit{right}=n-1$ 表示需要交换的位置。每次交换后，将指针向中间移动，$\textit{left}$ 加一，$\textit{right}$ 减一。

什么时候退出循环呢？

- 如果 $n$ 是奇数，最终 $\textit{left}=\textit{right}=\left\lfloor\dfrac{n}{2}\right\rfloor$，无需交换。
- 如果 $n$ 是偶数，最后交换的是 $\textit{left}=\dfrac{n}{2}-1$ 以及 $\textit{right}=\dfrac{n}{2}$，然后各移动一位变成 $\textit{left}=\dfrac{n}{2},\ \textit{right}=\dfrac{n}{2}-1$，退出循环。

综上所述，当 $\textit{left}\ge \textit{right}$ 时，所有字符交换完毕，退出循环。

```py [sol-Python3]
class Solution:
    def reverseString(self, s: List[str]) -> None:
        left = 0
        right = len(s) - 1
        while left < right:
            s[left], s[right] = s[right], s[left]
            left += 1
            right -= 1
```

```java [sol-Java]
class Solution {
    public void reverseString(char[] s) {
        int n = s.length;
        for (int left = 0, right = n - 1; left < right; left++, right--) {
            char tmp = s[left];
            s[left] = s[right];
            s[right] = tmp;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    void reverseString(vector<char>& s) {
        int n = s.size();
        for (int left = 0, right = n - 1; left < right; left++, right--) {
            swap(s[left], s[right]);
        }
    }
};
```

```c [sol-C]
void reverseString(char* s, int sSize) {
    for (int left = 0, right = sSize - 1; left < right; left++, right--) {
        char tmp = s[left];
        s[left] = s[right];
        s[right] = tmp;
    }
}
```

```go [sol-Go]
func reverseString(s []byte) {
    for left, right := 0, len(s)-1; left < right; left, right = left+1, right-1 {
        s[left], s[right] = s[right], s[left]
    }
}
```

```js [sol-JavaScript]
var reverseString = function(s) {
    const n = s.length;
    for (let left = 0, right = n - 1; left < right; left++, right--) {
        [s[left], s[right]] = [s[right], s[left]];
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn reverse_string(s: &mut Vec<char>) {
        let n = s.len();
        let mut left = 0;
        let mut right = n - 1;
        while left < right {
            s.swap(left, right);
            left += 1;
            right -= 1;
        }
    }
}
```

## 单指针写法

由于 $\textit{left}+\textit{right}=n-1$ 恒成立，所以只需要用一个变量 $i$ 表示 $\textit{left}$，$n-1-i$ 就是 $\textit{right}$。

根据上面的讨论，循环直到 $i=\left\lfloor\dfrac{n}{2}\right\rfloor$ 时停止。

```py [sol-Python3]
class Solution:
    def reverseString(self, s: List[str]) -> None:
        for i in range(len(s) // 2):
            s[i], s[-i - 1] = s[-i - 1], s[i]
```

```java [sol-Java]
class Solution {
    public void reverseString(char[] s) {
        int n = s.length;
        for (int i = 0; i < n / 2; i++) {
            char tmp = s[i];
            s[i] = s[n - 1 - i];
            s[n - 1 - i] = tmp;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    void reverseString(vector<char>& s) {
        int n = s.size();
        for (int i = 0; i < n / 2; i++) {
            swap(s[i], s[n - 1 - i]);
        }
    }
};
```

```c [sol-C]
void reverseString(char* s, int sSize) {
    for (int i = 0; i < sSize / 2; i++) {
        char tmp = s[i];
        s[i] = s[sSize - 1 - i];
        s[sSize - 1 - i] = tmp;
    }
}
```

```go [sol-Go]
func reverseString(s []byte) {
    for i, n := 0, len(s); i < n/2; i++ {
        s[i], s[n-1-i] = s[n-1-i], s[i]
    }
}
```

```js [sol-JavaScript]
var reverseString = function(s) {
    const n = s.length;
    for (let i = 0; i < n / 2; i++) {
        [s[i], s[n - 1 - i]] = [s[n - 1 - i], s[i]];
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn reverse_string(s: &mut Vec<char>) {
        let n = s.len();
        for i in 0..n / 2 {
            s.swap(i, n - 1 - i);
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 库函数写法

部分语言可以用一些语法糖或者标准库解决。

```py [sol-Python3]
class Solution:
    def reverseString(self, s: List[str]) -> None:
        s.reverse()
```

```py [sol-Python3 写法二]
class Solution:
    def reverseString(self, s: List[str]) -> None:
        # s[:] = s[::-1] 是切片赋值语法，表示用 s[::-1] 替换 s 中的元素。
        # 注意不能写成 s = s[::-1]，因为 s 只是形参，修改 s 不会影响函数外部传入的实参。
        # 注意这不是原地操作，需要 O(n) 额外空间。
        s[:] = s[::-1]
```

```cpp [sol-C++]
class Solution {
public:
    void reverseString(vector<char>& s) {
        ranges::reverse(s);
    }
};
```

```go [sol-Go]
func reverseString(s []byte) {
    slices.Reverse(s)
}
```

```js [sol-JavaScript]
var reverseString = function(s) {
    s.reverse();
};
```

```rust [sol-Rust]
impl Solution {
    pub fn reverse_string(s: &mut Vec<char>) {
        s.reverse()
    }
}
```

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
