# 260. 只出现一次的数字 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/single-number-iii/
- 题目 slug：`single-number-iii`
- 来源专题：位运算
- 来源分类路径：九、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/single-number-iii/solutions/2484352/tu-jie-yi-zhang-tu-miao-dong-zhuan-huan-np9d2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】一张图秒懂！转换成 136！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/single-number-iii/solutions/2484352/tu-jie-yi-zhang-tu-miao-dong-zhuan-huan-np9d2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-yi-zhang-tu-miao-dong-zhuan-huan-np9d2`
- topic id：`2484352`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![](https://pic.leetcode.cn/1697089221-bDdgkc-lc260-c.png)

代码实现时，需要找到异或和中的某个值为 $1$ 的比特位，用于区分 $\textit{nums}[i]$ 是在第一组还是在第二组。

## 写法一

第一种方式是计算异或和的 **lowbit**，即二进制最低位的 $1$，举例如下：

```cpp
     s = 101100
    ~s = 010011
(~s)+1 = 010100 // 根据补码的定义，这就是 -s   效果：s 的最低 1 左侧取反，右侧不变
s & -s = 000100 // lowbit
```

更多位运算技巧，请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class Solution:
    def singleNumber(self, nums: List[int]) -> List[int]:
        xor_all = reduce(xor, nums)
        lowbit = xor_all & -xor_all  # 用来区分元素在哪一组

        ans = [0, 0]
        for x in nums:
            if (x & lowbit) == 0:  # x 在第一组
                ans[0] ^= x
            else:  # x 在第二组
                ans[1] ^= x
        return ans
```

```java [sol-Java]
class Solution {
    public int[] singleNumber(int[] nums) {
        int xorAll = 0;
        for (int x : nums) {
            xorAll ^= x;
        }
        int lowbit = xorAll & -xorAll; // 用来区分元素在哪一组

        int[] ans = new int[2];
        for (int x : nums) {
            if ((x & lowbit) == 0) { // x 在第一组
                ans[0] ^= x;
            } else { // x 在第二组
                ans[1] ^= x;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> singleNumber(vector<int>& nums) {
        unsigned int xor_all = 0;
        for (int x : nums) {
            xor_all ^= x;
        }
        int lowbit = xor_all & -xor_all; // 用来区分元素在哪一组

        vector<int> ans(2);
        for (int x : nums) {
            if ((x & lowbit) == 0) { // x 在第一组
                ans[0] ^= x;
            } else { // x 在第二组
                ans[1] ^= x;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func singleNumber(nums []int) []int {
    xorAll := 0
    for _, x := range nums {
        xorAll ^= x
    }
    lowbit := xorAll & -xorAll // 用来区分元素在哪一组

    ans := make([]int, 2)
    for _, x := range nums {
        if x&lowbit == 0 { // x 在第一组
            ans[0] ^= x
        } else { // x 在第二组
            ans[1] ^= x
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var singleNumber = function (nums) {
    let xorAll = 0;
    for (const x of nums) {
        xorAll ^= x;
    }
    const lowbit = xorAll & -xorAll; // 用来区分元素在哪一组

    const ans = [0, 0];
    for (const x of nums) {
        if ((x & lowbit) === 0) { // x 在第一组
            ans[0] ^= x;
        } else { // x 在第二组
            ans[1] ^= x;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn single_number(nums: Vec<i32>) -> Vec<i32> {
        let xor_all = nums.iter().fold(0, |xor, &x| xor ^ x);
        let lowbit = xor_all & -xor_all; // 用来区分元素在哪一组

        let mut ans = vec![0, 0];
        for x in nums {
            if (x & lowbit) == 0 { // x 在第一组
                ans[0] ^= x;
            } else { // x 在第二组
                ans[1] ^= x;
            }
        }
        ans
    }
}
```

## 写法二

第二种方式是计算异或和的**尾零的个数**，这样可以直接取到 $\textit{nums}[i]$ 在该比特位上的值，从而避免 $\texttt{if-else}$。

> 注：如果没有计算尾零个数的库函数，可改为计算二进制的长度减一。

```py [sol-Python3]
class Solution:
    def singleNumber(self, nums: List[int]) -> List[int]:
        xor_all = reduce(xor, nums)
        shift = xor_all.bit_length() - 1 if xor_all > 0 else 31

        ans = [0, 0]
        for x in nums:
            ans[x >> shift & 1] ^= x
        return ans
```

```java [sol-Java]
class Solution {
    public int[] singleNumber(int[] nums) {
        int xorAll = 0;
        for (int x : nums) {
            xorAll ^= x;
        }
        int trailingZeros = Integer.numberOfTrailingZeros(xorAll);

        int[] ans = new int[2];
        for (int x : nums) {
            ans[x >>> trailingZeros & 1] ^= x;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> singleNumber(vector<int>& nums) {
        int xor_all = 0;
        for (int x : nums) {
            xor_all ^= x;
        }
        int trailing_zeros = countr_zero((uint32_t) xor_all);

        vector<int> ans(2);
        for (int x : nums) {
            ans[x >> trailing_zeros & 1] ^= x;
        }
        return ans;
    }
};
```

```go [sol-Go]
func singleNumber(nums []int) []int {
    xorAll := 0
    for _, x := range nums {
        xorAll ^= x
    }
    tz := bits.TrailingZeros(uint(xorAll))

    ans := make([]int, 2)
    for _, x := range nums {
        ans[x>>tz&1] ^= x
    }
    return ans
}
```

```js [sol-JavaScript]
var singleNumber = function (nums) {
    let xorAll = 0;
    for (const x of nums) {
        xorAll ^= x;
    }
    const shift = 31 - Math.clz32(xorAll);

    const ans = [0, 0];
    for (const x of nums) {
        ans[x >> shift & 1] ^= x;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn single_number(nums: Vec<i32>) -> Vec<i32> {
        let xor_all = nums.iter().fold(0, |xor, &x| xor ^ x);
        let tz = xor_all.trailing_zeros();

        let mut ans = vec![0, 0];
        for x in nums {
            ans[x as usize >> tz & 1] ^= x;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 相似题目

- [136. 只出现一次的数字](https://leetcode.cn/problems/single-number/)
- [137. 只出现一次的数字 II](https://leetcode.cn/problems/single-number-ii/)

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
