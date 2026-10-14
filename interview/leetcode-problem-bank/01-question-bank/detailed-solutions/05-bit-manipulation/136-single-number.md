# 136. 只出现一次的数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/single-number/
- 题目 slug：`single-number`
- 来源专题：位运算
- 来源分类路径：九、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/single-number/solutions/2481594/li-yong-yi-huo-de-xing-zhi-fu-ti-dan-pyt-oizc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 一次遍历，附相似题目（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/single-number/solutions/2481594/li-yong-yi-huo-de-xing-zhi-fu-ti-dan-pyt-oizc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`li-yong-yi-huo-de-xing-zhi-fu-ti-dan-pyt-oizc`
- topic id：`2481594`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

利用异或运算 $a\oplus a = 0$ 的性质，我们可以用异或来「消除」所有出现了两次的元素，最后剩下的一定是只出现一次的元素。

例如 $\textit{nums}=[4,1,2,1,2]$，把所有元素异或：

$$
\begin{aligned}
&4\oplus 1\oplus 2\oplus 1\oplus 2\\
=\ &4\oplus (1\oplus 1)\oplus(2\oplus 2)\\
=\ &4\oplus 0 \oplus 0\\
=\ &4
\end{aligned}
$$

其中用到了异或运算的交换律 $a\oplus b = b\oplus a$，以及结合律 $(a\oplus b) \oplus c  = a\oplus (b\oplus c)$（类比加法）。

代码中，初始化 $\textit{ans}=0$ 是因为 $0\oplus a = a$，相当于我们从第一个数开始，和其他数异或。

```py [sol-Python3]
class Solution:
    def singleNumber(self, nums: List[int]) -> int:
        return reduce(xor, nums)
```

```java [sol-Java]
class Solution {
    public int singleNumber(int[] nums) {
        int ans = 0;
        for (int x : nums) {
            ans ^= x;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int ans = 0;
        for (int x : nums) {
            ans ^= x;
        }
        return ans;
    }
};
```

```c [sol-C]
int singleNumber(int* nums, int numsSize) {
    int ans = 0;
    for (int i = 0; i < numsSize; i++) {
        ans ^= nums[i];
    }
    return ans;
}
```

```go [sol-Go]
func singleNumber(nums []int) (ans int) {
    for _, x := range nums {
        ans ^= x
    }
    return
}
```

```js [sol-JavaScript]
var singleNumber = function(nums) {
    let ans = 0;
    for (const x of nums) {
        ans ^= x;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn single_number(nums: Vec<i32>) -> i32 {
        nums.into_iter().fold(0, |xor, x| xor ^ x)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [260. 只出现一次的数字 III](https://leetcode.cn/problems/single-number-iii/)
- [137. 只出现一次的数字 II](https://leetcode.cn/problems/single-number-ii/)
- [2965. 找出缺失和重复的数字](https://leetcode.cn/problems/find-missing-and-repeated-values/)
- [645. 错误的集合](https://leetcode.cn/problems/set-mismatch/)
- [3595. 一次或两次](https://leetcode.cn/problems/once-twice/)（会员题）

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
