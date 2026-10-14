# 81. 搜索旋转排序数组 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/search-in-rotated-sorted-array-ii/
- 题目 slug：`search-in-rotated-sorted-array-ii`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/search-in-rotated-sorted-array-ii/solutions/3058425/ji-yu-33-ti-de-jian-ji-xie-fa-zhi-xu-zen-uayi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[基于 33 题的简洁写法，只需增加一个 if（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/search-in-rotated-sorted-array-ii/solutions/3058425/ji-yu-33-ti-de-jian-ji-xie-fa-zhi-xu-zen-uayi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-yu-33-ti-de-jian-ji-xie-fa-zhi-xu-zen-uayi`
- topic id：`3058425`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：[33. 搜索旋转排序数组](https://leetcode.cn/problems/search-in-rotated-sorted-array/)，[我的题解](https://leetcode.cn/problems/search-in-rotated-sorted-array/solutions/1987503/by-endlesscheng-auuh/)。

> 本文的二分区间是开区间 $(\textit{left},\textit{right})$。

本题与 33 题的区别是有相同元素，这会导致在二分查找时，可能会遇到恰好二分元素 $\textit{nums}[\textit{mid}]$ 与数组末尾元素 $\textit{nums}[n-1]$ 相同的情况，此时无法确定答案在左半区间中还是右半区间中。

既然无法确定最小值所在区间，那么干脆去掉 $\textit{nums}$ 的最后一个数，继续二分。换句话说，此时问题变成了一个规模为 $n-1$ 的子问题。

你可能会有疑问：这会不会碰巧去掉了 $\textit{target}$？

这是不会的：

- 如果去掉的数是 $\textit{target}$，那么 $\textit{nums}[\textit{mid}]$ 也等于 $\textit{target}$，这说明 $\textit{target}$ 仍然在数组中。
- 如果去掉的数不是 $\textit{target}$，那么我们排除了一个不等于 $\textit{target}$ 的数。

为了方便写代码，我们可以把 $\textit{right}$ 当作「数组最后一个数的下标」：

- 如果 $\textit{nums}[\textit{mid}] = \textit{nums}[\textit{right}]$，那么和上面一样，去掉 $\textit{nums}[\textit{right}]$，也就是把 $\textit{right}$ 减一。
- 如果 $\text{check}(\textit{nums}[\textit{mid}]) = \texttt{true}$，那么下标大于 $\textit{mid}$ 的数都在 $\textit{target}$ 的右边，都可以去掉，也就是把 $\textit{right}$ 更新为 $\textit{mid}$。
- 如果 $\text{check}(\textit{nums}[\textit{mid}]) = \texttt{false}$，和 33 题一样，把 $\textit{left}$ 更新为 $\textit{mid}$。

下面的代码用的开区间写法，其他二分写法也是可以的，原理见[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

```py [sol-Python3]
class Solution:
    def search(self, nums: List[int], target: int) -> int:
        def check(i: int) -> bool:
            x = nums[i]
            if x > nums[right]:
                return target > nums[right] and x >= target
            return target > nums[right] or x >= target

        left, right = -1, len(nums) - 1  # 开区间 (-1, n-1)
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            if nums[mid] == nums[right]:
                right -= 1
            elif check(mid):
                right = mid
            else:
                left = mid
        return nums[right] == target
```

```java [sol-Java]
class Solution {
    public boolean search(int[] nums, int target) {
        int left = -1;
        int right = nums.length - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = (left + right) >>> 1;
            if (nums[mid] == nums[right]) {
                right--;
            } else if (check(nums, target, right, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return nums[right] == target;
    }

    private boolean check(int[] nums, int target, int right, int i) {
        int x = nums[i];
        if (x > nums[right]) {
            return target > nums[right] && x >= target;
        }
        return target > nums[right] || x >= target;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int left = -1, right = nums.size() - 1; // 开区间 (-1, n-1)

        auto check = [&](int i) -> bool {
            int x = nums[i];
            if (x > nums[right]) {
                return target > nums[right] && x >= target;
            }
            return target > nums[right] || x >= target;
        };

        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            if (nums[mid] == nums[right]) {
                right--;
            } else if (check(mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return nums[right] == target;
    }
};
```

```c [sol-C]
bool search(int* nums, int numsSize, int target) {
    int left = -1, right = numsSize - 1; // 开区间 (-1, n-1)

    bool check(int i) {
        int x = nums[i];
        if (x > nums[right]) {
            return target > nums[right] && x >= target;
        }
        return target > nums[right] || x >= target;
    }

    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2;
        if (nums[mid] == nums[right]) {
            right--;
        } else if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right] == target;
}
```

```go [sol-Go]
func search(nums []int, target int) bool {
    left, right := -1, len(nums)-1 // 开区间 (-1, n-1)

    check := func(i int) bool {
        x := nums[i]
        if x > nums[right] {
            return target > nums[right] && x >= target
        }
        return target > nums[right] || x >= target
    }

    for left+1 < right { // 开区间不为空
        mid := (left + right) / 2
        if nums[mid] == nums[right] {
            right--
        } else if check(mid) {
            right = mid
        } else {
            left = mid
        }
    }
    return nums[right] == target
}
```

```js [sol-JavaScript]
var search = function(nums, target) {
    function check(i) {
        const x = nums[i];
        if (x > nums[right]) {
            return target > nums[right] && x >= target;
        }
        return target > nums[right] || x >= target;
    }

    let left = -1, right = nums.length - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] === nums[right]) {
            right--;
        } else if (check(mid, right)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right] === target;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> bool {
        let check = |i: usize, right: usize| -> bool {
            let x = nums[i];
            if x > nums[right] {
                return target > nums[right] && x >= target;
            }
            target > nums[right] || x >= target
        };

        let mut left = 0;
        let mut right = nums.len() - 1; // 左闭右开区间 [0, n-1)
        while left < right { // 区间不为空
            let mid = left + (right - left) / 2;
            if nums[mid] == nums[right] {
                right -= 1;
            } else if check(mid, right) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        nums[right] == target
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。最坏情况下，数组元素均相同且都不等于 $\textit{target}$，循环内会一直执行 $\textit{right}$ 减一，此时循环会执行 $\mathcal{O}(n)$ 次。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. 【本题相关】[二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
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
