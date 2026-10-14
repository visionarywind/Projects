# 26. 删除有序数组中的重复项

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-duplicates-from-sorted-array/
- 题目 slug：`remove-duplicates-from-sorted-array`
- 来源专题：位运算
- 来源分类路径：三、与或（AND/OR）的性质 / AND/OR LogTrick
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-duplicates-from-sorted-array/solutions/2807162/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-rvyk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[跟着我过一遍示例 2，你就明白怎么做了（附库函数写法）Python/Java/C++/C/Go/JS/Rust](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/solutions/2807162/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-rvyk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-rvyk`
- topic id：`2807162`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

为方便描述，把 $\textit{nums}$ 记作 $a$。

示例 2 的 $a=[0,0,1,1,1,2,2,3,3,4]$。

首先 $a_0=0$ 肯定要保留，我们从 $a_1$ 开始讨论：

- 如果 $a_1 = a_0$，那么 $a_1$ 是重复项，不保留。
- 如果 $a_1 \ne a_0$，那么 $a_1$ 不是重复项，保留。

其它元素的规则也类似，具体如下表：

| $i$ | $a_i$  | 是否等于 $a_{i-1}$  |  去重后  |
|---|---|---|---|
| $1$  |  $0$  | $=$  | $[0]$  |
| $2$  |  $1$  | $\ne$  |  $[0,1]$ |
| $3$  |  $1$  | $=$ |  $[0,1]$ |
| $4$  |  $1$  | $=$  | $[0,1]$  |
| $5$  |  $2$  | $\ne$  | $[0,1,2]$  |
| $6$  |  $2$  | $=$ |  $[0,1,2]$ |
| $7$  |  $3$  | $\ne$ |  $[0,1,2,3]$ |
| $8$  |  $3$  | $=$ | $[0,1,2,3]$  |
| $9$  |  $4$  | $\ne$ |  $[0,1,2,3,4]$ |

具体算法如下：

1. 初始化 $k=1$，表示保留的元素要填入的下标。
2. 从 $i=1$ 开始遍历 $\textit{nums}$。
3. 如果 $\textit{nums}[i]=\textit{nums}[i-1]$，那么 $\textit{nums}[i]$ 是重复项，不保留。
4. 如果 $\textit{nums}[i]\ne\textit{nums}[i-1]$，那么 $\textit{nums}[i]$ 不是重复项，保留，填入 $\textit{nums}[k]$ 中，然后把 $k$ 加一。
5. 遍历结束后，$k$ 就是 $\textit{nums}$ 中的唯一元素的数量，返回 $k$。

```py [sol-Python3]
class Solution:
    def removeDuplicates(self, nums: List[int]) -> int:
        k = 1
        for i in range(1, len(nums)):
            if nums[i] != nums[i - 1]:  # nums[i] 不是重复项
                nums[k] = nums[i]  # 保留 nums[i]
                k += 1
        return k
```

```py [sol-Python3 pairwise]
class Solution:
    def removeDuplicates(self, nums: List[int]) -> int:
        k = 1
        for x, y in pairwise(nums):
            if x != y:  # y 不是重复项
                nums[k] = y  # 保留 y
                k += 1
        return k
```

```java [sol-Java]
class Solution {
    public int removeDuplicates(int[] nums) {
        int k = 1;
        for (int i = 1; i < nums.length; i++) {
            if (nums[i] != nums[i - 1]) { // nums[i] 不是重复项
                nums[k++] = nums[i]; // 保留 nums[i]
            }
        }
        return k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int k = 1;
        for (int i = 1; i < nums.size(); i++) {
            if (nums[i] != nums[i - 1]) { // nums[i] 不是重复项
                nums[k++] = nums[i]; // 保留 nums[i]
            }
        }
        return k;
    }
};
```

```c [sol-C]
int removeDuplicates(int* nums, int numsSize) {
    int k = 1;
    for (int i = 1; i < numsSize; i++) {
        if (nums[i] != nums[i - 1]) { // nums[i] 不是重复项
            nums[k++] = nums[i]; // 保留 nums[i]
        }
    }
    return k;
}
```

```go [sol-Go]
func removeDuplicates(nums []int) int {
    k := 1
    for i := 1; i < len(nums); i++ {
        if nums[i] != nums[i-1] { // nums[i] 不是重复项
            nums[k] = nums[i] // 保留 nums[i]
            k++
        }
    }
    return k
}
```

```js [sol-JavaScript]
var removeDuplicates = function(nums) {
    let k = 1;
    for (let i = 1; i < nums.length; i++) {
        if (nums[i] != nums[i - 1]) { // nums[i] 不是重复项
            nums[k++] = nums[i]; // 保留 nums[i]
        }
    }
    return k;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_duplicates(nums: &mut Vec<i32>) -> i32 {
        let mut k = 1;
        for i in 1..nums.len() {
            if nums[i] != nums[i - 1] { // nums[i] 不是重复项
                nums[k] = nums[i]; // 保留 nums[i]
                k += 1;
            }
        }
        k as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

### 附：库函数写法

```py [sol-Python3]
class Solution:
    def removeDuplicates(self, nums: List[int]) -> int:
        nums[:] = sorted(set(nums))  # 注：需要额外空间
        return len(nums)
```

```cpp [sol-C++]
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        return ranges::unique(nums).begin() - nums.begin();
    }
};
```

```go [sol-Go]
func removeDuplicates(nums []int) int {
    return len(slices.Compact(nums))
}
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_duplicates(nums: &mut Vec<i32>) -> i32 {
        nums.dedup();
        nums.len() as _
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
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
