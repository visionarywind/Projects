# 1385. 两个数组间的距离值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-distance-value-between-two-arrays/
- 题目 slug：`find-the-distance-value-between-two-arrays`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.2 进阶
- 难度分：1235
- 外部题解来源：https://leetcode.cn/problems/find-the-distance-value-between-two-arrays/solutions/3010185/liang-chong-fang-fa-er-fen-cha-zhao-san-15u9b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分查找/双指针（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-the-distance-value-between-two-arrays/solutions/3010185/liang-chong-fang-fa-er-fen-cha-zhao-san-15u9b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-fen-cha-zhao-san-15u9b`
- topic id：`3010185`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

对于 $\textit{arr}_1$ 中的元素 $x$，如果 $\textit{arr}_2$ 没有值在闭区间 $[x-d,x+d]$ 中的元素，把答案（计数器）加一。

## 方法一：排序 + 二分查找

把 $\textit{arr}_2$ 从小到大排序，这样我们可以二分查找。

遍历 $\textit{arr}_1$，设 $x=\textit{arr}_1[i]$。我们要判断在 $\textit{arr}_2$ 中是否存在元素，在闭区间 $[x-d,x+d]$ 中。

考虑 $[x-d,x+d]$ 中的第一个数（最左边的数），也就是在 $\textit{arr}_2$ 中二分查找 $\ge x-d$ 的最小的数 $y$。如果 $y$ 不存在，或者 $y>x+d$，则说明没有在 $[x-d,x+d]$ 中的元素，符合题目要求，把答案加一。

```py [sol-Python3]
class Solution:
    def findTheDistanceValue(self, arr1: List[int], arr2: List[int], d: int) -> int:
        arr2.sort()
        ans = 0
        for x in arr1:
            i = bisect_left(arr2, x - d)
            if i == len(arr2) or arr2[i] > x + d:
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int findTheDistanceValue(int[] arr1, int[] arr2, int d) {
        Arrays.sort(arr2);
        int ans = 0;
        for (int x : arr1) {
            int i = Arrays.binarySearch(arr2, x - d);
            if (i < 0) {
                i = ~i; // -i - 1
            }
            if (i == arr2.length || arr2[i] > x + d) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findTheDistanceValue(vector<int>& arr1, vector<int>& arr2, int d) {
        ranges::sort(arr2);
        int ans = 0;
        for (int x : arr1) {
            auto it = ranges::lower_bound(arr2, x - d);
            if (it == arr2.end() || *it > x + d) {
                ans++;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

// lowerBound 返回最小的满足 nums[i] >= target 的下标 i
// 如果数组为空，或者所有数都 < target，则返回 numsSize
// 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]
int lowerBound(int* nums, int numsSize, int target) {
    int left = -1, right = numsSize; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        int mid = left + (right - left) / 2;
        if (nums[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    // 循环结束后 left+1 = right
    // 此时 nums[left] < target 而 nums[right] >= target
    // 所以 right 就是第一个 >= target 的元素下标
    return right;
}

int findTheDistanceValue(int* arr1, int arr1Size, int* arr2, int arr2Size, int d) {
    qsort(arr2, arr2Size, sizeof(int), cmp);
    int ans = 0;
    for (int i = 0; i < arr1Size; i++) {
        int x = arr1[i];
        int j = lowerBound(arr2, arr2Size, x - d);
        if (j == arr2Size || arr2[j] > x + d) {
            ans++;
        }
    }
    return ans;
}
```

```go [sol-Go]
func findTheDistanceValue(arr1, arr2 []int, d int) (ans int) {
    slices.Sort(arr2)
    for _, x := range arr1 {
        i := sort.SearchInts(arr2, x-d)
        if i == len(arr2) || arr2[i] > x+d {
            ans++
        }
    }
    return
}
```

```js [sol-JavaScript]
var findTheDistanceValue = function(arr1, arr2, d) {
    arr2.sort((a, b) => a - b);
    let ans = 0;
    for (const x of arr1) {
        const i = _.sortedIndex(arr2, x - d);
        if (i === arr2.length || arr2[i] > x + d) {
            ans++;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_the_distance_value(arr1: Vec<i32>, mut arr2: Vec<i32>, d: i32) -> i32 {
        arr2.sort_unstable();
        arr1.into_iter().filter(|&x| {
            let i = arr2.partition_point(|&y| y < x - d);
            i == arr2.len() || arr2[i] > x + d
        }).count() as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((m+n)\log m)$，其中 $n$ 是 $\textit{arr}_1$ 的长度，$m$ 是 $\textit{arr}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 方法二：排序 + 双指针

把 $\textit{arr}_1$ 和 $\textit{arr}_2$ 从小到大排序。

遍历 $\textit{arr}_1$，设 $x=\textit{arr}_1[i]$，同时用另一个指针 $j$ 维护最小的满足 $\textit{arr}_2[j] \ge x-d$ 的数的下标。

如果发现 $\textit{arr}_2[j] > x+d$，那么 $\textit{arr}_2$ 没有在 $[x-d,x+d]$ 中的数，答案加一。

```py [sol-Python3]
class Solution:
    def findTheDistanceValue(self, arr1: List[int], arr2: List[int], d: int) -> int:
        arr1.sort()
        arr2.sort()
        ans = j = 0
        for x in arr1:
            while j < len(arr2) and arr2[j] < x - d:
                j += 1
            if j == len(arr2) or arr2[j] > x + d:
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int findTheDistanceValue(int[] arr1, int[] arr2, int d) {
        Arrays.sort(arr1);
        Arrays.sort(arr2);
        int ans = 0;
        int j = 0;
        for (int x : arr1) {
            while (j < arr2.length && arr2[j] < x - d) {
                j++;
            }
            if (j == arr2.length || arr2[j] > x + d) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findTheDistanceValue(vector<int>& arr1, vector<int>& arr2, int d) {
        ranges::sort(arr1);
        ranges::sort(arr2);
        int ans = 0, j = 0;
        for (int x : arr1) {
            while (j < arr2.size() && arr2[j] < x - d) {
                j++;
            }
            if (j == arr2.size() || arr2[j] > x + d) {
                ans++;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int findTheDistanceValue(int* arr1, int arr1Size, int* arr2, int arr2Size, int d) {
    qsort(arr1, arr1Size, sizeof(int), cmp);
    qsort(arr2, arr2Size, sizeof(int), cmp);
    int ans = 0, j = 0;
    for (int i = 0; i < arr1Size; i++) {
        int x = arr1[i];
        while (j < arr2Size && arr2[j] < x - d) {
            j++;
        }
        if (j == arr2Size || arr2[j] > x + d) {
            ans++;
        }
    }
    return ans;
}
```

```go [sol-Go]
func findTheDistanceValue(arr1, arr2 []int, d int) (ans int) {
    slices.Sort(arr1)
    slices.Sort(arr2)
    j := 0
    for _, x := range arr1 {
        for j < len(arr2) && arr2[j] < x-d {
            j++
        }
        if j == len(arr2) || arr2[j] > x+d {
            ans++
		}
    }
    return
}
```

```js [sol-JavaScript]
var findTheDistanceValue = function(arr1, arr2, d) {
    arr1.sort((a, b) => a - b);
    arr2.sort((a, b) => a - b);
    let ans = 0, j = 0;
    for (const x of arr1) {
        while (j < arr2.length && arr2[j] < x - d) {
            j++;
        }
        if (j === arr2.length || arr2[j] > x + d) {
            ans++;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_the_distance_value(mut arr1: Vec<i32>, mut arr2: Vec<i32>, d: i32) -> i32 {
        arr1.sort_unstable();
        arr2.sort_unstable();
        let mut ans = 0;
        let mut j = 0;
        for x in arr1 {
            while j < arr2.len() && arr2[j] < x - d {
                j += 1;
            }
            if (j == arr2.len() || arr2[j] > x + d) {
                ans += 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n+m\log m)$，其中 $n$ 是 $\textit{arr}_1$ 的长度，$m$ 是 $\textit{arr}_2$ 的长度。瓶颈在排序上，双指针是 $\mathcal{O}(n+m)$ 的。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. 【本题相关】[滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
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
