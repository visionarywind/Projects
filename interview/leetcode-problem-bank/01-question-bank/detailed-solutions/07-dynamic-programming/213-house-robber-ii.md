# 213. 打家劫舍 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/house-robber-ii/
- 题目 slug：`house-robber-ii`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.2 打家劫舍 / 答疑
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/house-robber-ii/solutions/2445622/jian-ji-xie-fa-zhi-jie-diao-yong-198-ti-qhvri/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法，直接调用 198 题代码（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/house-robber-ii/solutions/2445622/jian-ji-xie-fa-zhi-jie-diao-yong-198-ti-qhvri/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-zhi-jie-diao-yong-198-ti-qhvri`
- topic id：`2445622`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：[198. 打家劫舍](https://leetcode.cn/problems/house-robber/)，[视频讲解](https://www.bilibili.com/video/BV1Xj411K7oF/)。

讨论是否偷 $\textit{nums}[0]$：

- 如果偷 $\textit{nums}[0]$，那么 $\textit{nums}[1]$ 和 $\textit{nums}[n-1]$ 不能偷，问题变成从 $\textit{nums}[2]$ 到 $\textit{nums}[n-2]$ 的非环形版本，调用 198 题的代码解决。
- 如果不偷 $\textit{nums}[0]$，那么问题变成从 $\textit{nums}[1]$ 到 $\textit{nums}[n-1]$ 的非环形版本，同样调用 198 题的代码解决。

这两种方案覆盖了所有情况（毕竟 $\textit{nums}[0]$ 只有偷与不偷，没有第三种选择），所以取两种方案的最大值，即为答案。

把我在 198 题写的 [代码](https://leetcode.cn/problems/house-robber/solution/ru-he-xiang-chu-zhuang-tai-ding-yi-he-zh-1wt1/) 复制过来即可。

```py [sol-Python3]
class Solution:
    # 198. 打家劫舍
    def rob1(self, nums: List[int]) -> int:
        f0 = f1 = 0
        for x in nums:
            f0, f1 = f1, max(f1, f0 + x)
        return f1

    def rob(self, nums: List[int]) -> int:
        return max(nums[0] + self.rob1(nums[2:-1]), self.rob1(nums[1:]))
```

```java [sol-Java]
class Solution {
    public int rob(int[] nums) {
        int n = nums.length;
        return Math.max(nums[0] + rob1(nums, 2, n - 1), rob1(nums, 1, n));
    }

    // 198. 打家劫舍
    // 偷下标在 [start, end) 中的房子
    private int rob1(int[] nums, int start, int end) {
        int f0 = 0;
        int f1 = 0;
        for (int i = start; i < end; i++) {
            int newF = Math.max(f1, f0 + nums[i]);
            f0 = f1;
            f1 = newF;
        }
        return f1;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 198. 打家劫舍
    // 偷下标在 [start, end) 中的房子
    int rob1(vector<int>& nums, int start, int end) {
        int f0 = 0, f1 = 0;
        for (int i = start; i < end; i++) {
            int new_f = max(f1, f0 + nums[i]);
            f0 = f1;
            f1 = new_f;
        }
        return f1;
    }

public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        return max(nums[0] + rob1(nums, 2, n - 1), rob1(nums, 1, n));
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

// 198. 打家劫舍
// 偷下标在 [start, end) 中的房子
int rob1(int* nums, int start, int end) {
    int f0 = 0, f1 = 0;
    for (int i = start; i < end; i++) {
        int new_f = MAX(f1, f0 + nums[i]);
        f0 = f1;
        f1 = new_f;
    }
    return f1;
}

int rob(int* nums, int numsSize) {
    int choose0 = nums[0] + rob1(nums, 2, numsSize - 1);
    int not_choose0 = rob1(nums, 1, numsSize);
    return MAX(choose0, not_choose0);
}
```

```go [sol-Go]
// 198. 打家劫舍
// 偷下标在 [start, end) 中的房子
func rob1(nums []int, start, end int) int {
    f0, f1 := 0, 0
    for i := start; i < end; i++ {
        f0, f1 = f1, max(f1, f0+nums[i])
    }
    return f1
}

func rob(nums []int) int {
    n := len(nums)
    return max(nums[0]+rob1(nums, 2, n-1), rob1(nums, 1, n))
}
```

```js [sol-JavaScript]
// 198. 打家劫舍
var rob1 = function(nums) {
    let f0 = 0, f1 = 0;
    for (const x of nums) {
        [f0, f1] = [f1, Math.max(f1, f0 + x)]
    }
    return f1;
};

var rob = function(nums) {
    const n = nums.length;
    return Math.max(nums[0] + rob1(nums.slice(2, n - 1)), rob1(nums.slice(1)))
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        // 198. 打家劫舍
        // 偷下标在 [start, end) 中的房子
        fn rob1(nums: &[i32], start: usize, end: usize) -> i32 {
            let mut f0 = 0;
            let mut f1 = 0;
            for i in start..end {
                let new_f = f1.max(f0 + nums[i]);
                f0 = f1;
                f1 = new_f;
            }
            f1
        }

        let n = nums.len();
        rob1(&nums, 1, n).max(nums[0] + rob1(&nums, 2, n - 1))
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。为了写起来方便，Python 和 JS 使用了切片，把切片改成普通的循环可以做到 $\mathcal{O}(1)$ 空间。

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
