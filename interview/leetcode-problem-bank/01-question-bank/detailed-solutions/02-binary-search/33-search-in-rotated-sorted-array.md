# 33. 搜索旋转排序数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/search-in-rotated-sorted-array/
- 题目 slug：`search-in-rotated-sorted-array`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/search-in-rotated-sorted-array/solutions/1987503/by-endlesscheng-auuh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：两次二分 / 一次二分（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/search-in-rotated-sorted-array/solutions/1987503/by-endlesscheng-auuh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-auuh`
- topic id：`1987503`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：两次二分

首先 [153. 寻找旋转排序数组中的最小值](https://leetcode.cn/problems/find-minimum-in-rotated-sorted-array/solution/by-endlesscheng-owgd/)，找到 $\textit{nums}$ 的最小值的下标 $i$。

根据旋转排序数组的定义，下标在 $[0,i-1]$ 中的元素都比下标在 $[i,n-1]$ 中的元素大（注意题目保证 $\textit{nums}$ 没有重复元素）。特别地，如果 $i=0$，那么 $\textit{nums}$ 是严格递增数组，没有发生旋转。

根据这一性质，分类讨论：

- 如果 $\textit{target} > \textit{nums}[n-1]$，那么 $\textit{target}$ 只可能在子数组 $[0,i-1]$ 中。由于子数组 $[0,i-1]$ 是递增的，我们可以在 $[0,i-1]$ 中二分查找 $\textit{target}$。
- 如果 $\textit{target} \le \textit{nums}[n-1]$，那么 $\textit{target}$ 只可能在子数组 $[i,n-1]$ 中。由于子数组 $[i,n-1]$ 是递增的，我们可以在 $[i,n-1]$ 中二分查找 $\textit{target}$。

注意上述讨论**兼容** $i=0$ 的情况：

- 如果 $\textit{target} > \textit{nums}[n-1]$，由于 $\textit{nums}$ 是递增的，所以 $\textit{target}$ 比 $\textit{nums}$ 中的每个数都要大，所以 $\textit{nums}$ 不存在 $\textit{target}$。代码调用 $\texttt{lowerBound}$ 传入的 $\textit{right}=0$，`nums[0] == target` 是 $\texttt{false}$，最后会返回 $-1$。
- 如果 $\textit{target} \le \textit{nums}[n-1]$，那么在 $[i,n-1]$ 中二分也就是在 $[0,n-1]$ 中二分。

**二分基础知识**：[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

**本题视频讲解**：[【基础算法精讲 05】](https://www.bilibili.com/video/BV1QK411d76w/)。

下面代码用的开区间二分，用其他二分写法也是可以的。不同二分写法的区别见 [我的题解](https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array/solutions/1980196/er-fen-cha-zhao-zong-shi-xie-bu-dui-yi-g-t9l9/)。

```py [sol-Python3]
class Solution:
    # 153. 寻找旋转排序数组中的最小值（返回的是下标）
    def findMin(self, nums: List[int]) -> int:
        left, right = -1, len(nums) - 1  # 开区间 (-1, n-1)
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            if nums[mid] < nums[-1]:
                right = mid
            else:
                left = mid
        return right

    # 有序数组中找 target 的下标
    def lower_bound(self, nums: List[int], left: int, right: int, target: int) -> int:
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            # 循环不变量：
            # nums[right] >= target
            # nums[left] < target
            if nums[mid] >= target:
                right = mid  # 范围缩小到 (left, mid)
            else:
                left = mid  # 范围缩小到 (mid, right)
        return right if nums[right] == target else -1

    def search(self, nums: List[int], target: int) -> int:
        i = self.findMin(nums)
        if target > nums[-1]:  # target 只可能在第一段
            return self.lower_bound(nums, -1, i, target)  # 开区间 (-1, i)
        # target 只可能在第二段
        # 由于此时 target <= nums[-1]，所以 lower_bound 中的循环结束后，right < n 一定成立，无需判断 right == n
        return self.lower_bound(nums, i - 1, len(nums), target)  # 开区间 (i-1, n)
```

```java [sol-Java]
class Solution {
    public int search(int[] nums, int target) {
        int n = nums.length;
        int i = findMin(nums);
        if (target > nums[n - 1]) { // target 只可能在第一段
            return lowerBound(nums, -1, i, target); // 开区间 (-1, i)
        }
        // target 只可能在第二段
        // 由于此时 target <= nums[n-1]，所以 lowerBound 中的循环结束后，right < n 一定成立，无需判断 right == n
        return lowerBound(nums, i - 1, n, target); // 开区间 (i-1, n)
    }

    // 153. 寻找旋转排序数组中的最小值（返回的是下标）
    private int findMin(int[] nums) {
        int n = nums.length;
        int left = -1;
        int right = n - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = (left + right) >>> 1;
            if (nums[mid] < nums[n - 1]) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    // 有序数组中找 target 的下标
    private int lowerBound(int[] nums, int left, int right, int target) {
        while (left + 1 < right) { // 开区间不为空
            // 循环不变量：
            // nums[right] >= target
            // nums[left] < target
            int mid = (left + right) >>> 1;
            if (nums[mid] >= target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        return nums[right] == target ? right : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 153. 寻找旋转排序数组中的最小值（返回的是下标）
    int findMin(vector<int>& nums) {
        int left = -1, right = nums.size() - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            if (nums[mid] < nums.back()) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    // 有序数组中找 target 的下标
    int lower_bound(vector<int>& nums, int left, int right, int target) {
        while (left + 1 < right) { // 开区间不为空
            // 循环不变量：
            // nums[right] >= target
            // nums[left] < target
            int mid = left + (right - left) / 2;
            if (nums[mid] >= target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        return nums[right] == target ? right : -1;
    }

public:
    int search(vector<int>& nums, int target) {
        int i = findMin(nums);
        if (target > nums.back()) { // target 只可能在第一段
            return lower_bound(nums, -1, i, target); // 开区间 (-1, i)
        }
        // target 只可能在第二段
        // 由于此时 target <= nums[n-1]，所以 lower_bound 中的循环结束后，right < n 一定成立，无需判断 right == n
        return lower_bound(nums, i - 1, nums.size(), target); // 开区间 (i-1, n)
    }
};
```

```c [sol-C]
// 153. 寻找旋转排序数组中的最小值（返回的是下标）
int findMin(int* nums, int numsSize) {
    int left = -1, right = numsSize - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2;
        if (nums[mid] < nums[numsSize - 1]) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
}

// 有序数组中找 target 的下标
int lowerBound(int* nums, int left, int right, int target) {
    while (left + 1 < right) { // 开区间不为空
        // 循环不变量：
        // nums[right] >= target
        // nums[left] < target
        int mid = left + (right - left) / 2;
        if (nums[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    return nums[right] == target ? right : -1;
}

int search(int* nums, int numsSize, int target) {
    int i = findMin(nums, numsSize);
    if (target > nums[numsSize - 1]) { // target 只可能在第一段
        return lowerBound(nums, -1, i, target); // 开区间 (-1, i)
    }
    // target 只可能在第二段
    // 由于此时 target <= nums[n-1]，所以 lowerBound 中的循环结束后，right < n 一定成立，无需判断 right == n
    return lowerBound(nums, i - 1, numsSize, target); // 开区间 (i-1, n)
}
```

```go [sol-Go]
// 153. 寻找旋转排序数组中的最小值（返回的是下标）
func findMin(nums []int) int {
    left, right := -1, len(nums)-1 // 开区间 (-1, n-1)
    for left+1 < right { // 开区间不为空
        mid := left + (right-left)/2
        if nums[mid] < nums[len(nums)-1] {
            right = mid
        } else {
            left = mid
        }
    }
    return right
}

// 有序数组中找 target 的下标
func lowerBound(nums []int, left, right, target int) int {
    for left+1 < right { // 开区间不为空
        // 循环不变量：
        // nums[right] >= target
        // nums[left] < target
        mid := left + (right-left)/2
        if nums[mid] >= target {
            right = mid // 范围缩小到 (left, mid)
        } else {
            left = mid // 范围缩小到 (mid, right)
        }
    }
    if nums[right] != target {
        return -1
    }
    return right
}

func search(nums []int, target int) int {
    i := findMin(nums)
    if target > nums[len(nums)-1] { // target 只可能在第一段
        return lowerBound(nums, -1, i, target) // 开区间 (-1, i)
    }
    // target 只可能在第二段
    // 由于此时 target <= nums[n-1]，所以 lowerBound 中的循环结束后，right < n 一定成立，无需判断 right == n
    return lowerBound(nums, i-1, len(nums), target) // 开区间 (i-1, n)
}
```

```js [sol-JavaScript]
// 153. 寻找旋转排序数组中的最小值（返回的是下标）
var findMin = function(nums) {
    let left = -1, right = nums.length - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] < nums[nums.length - 1]) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
};

// 有序数组中找 target 的下标
var lowerBound = function(nums, left, right, target) {
    while (left + 1 < right) { // 开区间不为空
        // 循环不变量：
        // nums[right] >= target
        // nums[left] < target
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    return nums[right] === target ? right : -1;
};

var search = function(nums, target) {
    const i = findMin(nums);
    if (target > nums[nums.length - 1]) { // target 只可能在第一段
        return lowerBound(nums, -1, i, target); // 开区间 (-1, i)
    }
    // target 只可能在第二段
    // 由于此时 target <= nums[n-1]，所以 lowerBound 中的循环结束后，right < n 一定成立，无需判断 right === n
    return lowerBound(nums, i - 1, nums.length, target); // 开区间 (i-1, n)
};
```

```rust [sol-Rust]
impl Solution {
    // 153. 寻找旋转排序数组中的最小值（返回的是下标）
    fn find_min(nums: &[i32]) -> usize {
        let mut left = 0;
        let mut right = nums.len() - 1; // 左闭右开区间 [0, n-1)
        while left < right { // 区间不为空
            let mid = left + (right - left) / 2;
            if nums[mid] < nums[nums.len() - 1] {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        left
    }

    // 有序数组中找 target 的下标
    fn lower_bound(nums: &[i32], mut left: usize, mut right: usize, target: i32) -> i32 {
        while left < right { // 区间不为空
            let mid = left + (right - left) / 2;
            if nums[mid] >= target {
                right = mid; // 范围缩小到 [left, mid)
            } else {
                left = mid + 1; // 范围缩小到 [mid+1, right)
            }
        }
        if nums[left] == target { left as _ } else { -1 }
    }

    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        let i = Self::find_min(&nums);
        if target > nums[nums.len() - 1] { // target 只可能在第一段
            Self::lower_bound(&nums, 0, i, target) // 左闭右开区间 [0, i)
        } else { // target 只可能在第二段
            // 由于此时 target <= nums[n-1]，所以 lower_bound 中的循环结束后，right < n 一定成立，无需判断 right == n
            Self::lower_bound(&nums, i, nums.len(), target) // 左闭右开区间 [i, n)
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：一次二分

旋转排序数组有什么性质？

把 $\textit{nums}$ 中的某个数 $x$ 与 $\textit{nums}[n-1]$ 比大小：

- 如果 $x > \textit{nums}[n-1]$，那么可以推出以下结论：
  - $\textit{nums}$ 由两个递增段组成。
  - 第一段的所有元素均大于第二段的所有元素。
  - $x$ 在第一段。
- 如果 $x \le \textit{nums}[n-1]$，那么 $x$ 在第二段。或者 $\textit{nums}$ 就是递增数组，此时只有一段。

### 写法一

设 $x=\textit{nums}[\textit{mid}]$ 是我们二分的数。

我们需要判断 $x$ 和 $\textit{target}$ 的位置关系，谁在左边，谁在右边？

把 $x$ 和 $\textit{target}$ 这两个数都与 $\textit{nums}[n-1]$ 比大小，可以知道这两个数分别在哪一段。

分类讨论：

- 如果 $\textit{target} > \textit{nums}[n-1] \ge x$，那么 $\textit{target}$ 在第一段，$x$ 在第二段，说明 $\textit{target}$ 在 $x$ 的左边。
- 如果 $x > \textit{nums}[n-1] \ge \textit{target}$，那么 $x$ 在第一段，$\textit{target}$ 在第二段，说明 $\textit{target}$ 在 $x$ 的右边。
- 否则 $x$ 和 $\textit{target}$ 在同一段。和 $\texttt{lowerBound}$ 函数一样，比较 $x$ 和 $\textit{target}$ 的大小，即可区分谁在左谁在右。

下面代码用的开区间二分，用其他二分写法也是可以的。

二分的范围可以是 $(-1,n-1)$，也就是闭区间 $[0,n-2]$。

这是因为，如果 $\textit{target} = \textit{nums}[n-1]$，那么下面代码每次循环更新的都是 $\textit{left}$，而 $\textit{right}$ 始终不变。循环结束后，答案自然就是 $n-1$ 了。

#### 答疑

**问**：在 $[0,n-2]$ 中二分，是否需要考虑所有数都比 $\textit{target}$ 小的情况？或者更一般地，是否需要考虑 $\textit{target}$ 不在 $\textit{nums}$ 中的情况？

**答**：不需要。如果 $\textit{target}$ 不在 $\textit{nums}$ 中，那么无论最终的 $\textit{right}$ 是多少，`nums[right] == target` 一定不成立，算法一定会返回 $-1$。

```py [sol-Python3]
class Solution:
    def search(self, nums: List[int], target: int) -> int:
        left, right = -1, len(nums) - 1  # 开区间 (-1, n-1)
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            x = nums[mid]
            if target > nums[-1] >= x:  # target 在第一段，x 在第二段
                right = mid  # 下轮循环去左边找
            elif x > nums[-1] >= target:  # x 在第一段，target 在第二段
                left = mid  # 下轮循环去右边找
            elif x >= target:  # 否则，x 和 target 在同一段，这就和方法一的 lower_bound 一样了
                right = mid
            else:
                left = mid
        return right if nums[right] == target else -1
```

```java [sol-Java]
class Solution {
    public int search(int[] nums, int target) {
        int last = nums[nums.length - 1];
        int left = -1;
        int right = nums.length - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = (left + right) >>> 1;
            int x = nums[mid];
            if (target > last && x <= last) { // target 在第一段，x 在第二段
                right = mid; // 下轮循环去左边找
            } else if (x > last && target <= last) { // x 在第一段，target 在第二段
                left = mid; // 下轮循环去右边找
            } else if (x >= target) { // 否则，x 和 target 在同一段，这就和方法一的 lowerBound 一样了
                right = mid;
            } else {
                left = mid;
            }
        }
        return nums[right] == target ? right : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int last = nums.back();
        int left = -1, right = nums.size() - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            int x = nums[mid];
            if (target > last && x <= last) { // target 在第一段，x 在第二段
                right = mid; // 下轮循环去左边找
            } else if (x > last && target <= last) { // x 在第一段，target 在第二段
                left = mid; // 下轮循环去右边找
            } else if (x >= target) { // 否则，x 和 target 在同一段，这就和方法一的 lower_bound 一样了
                right = mid;
            } else {
                left = mid;
            }
        }
        return nums[right] == target ? right : -1;
    }
};
```

```c [sol-C]
int search(int* nums, int numsSize, int target) {
    int last = nums[numsSize - 1];
    int left = -1, right = numsSize - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2;
        int x = nums[mid];
        if (target > last && x <= last) { // target 在第一段，x 在第二段
            right = mid; // 下轮循环去左边找
        } else if (x > last && target <= last) { // x 在第一段，target 在第二段
            left = mid; // 下轮循环去右边找
        } else if (x >= target) { // 否则，x 和 target 在同一段，这就和方法一的 lowerBound 一样了
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right] == target ? right : -1;
}
```

```go [sol-Go]
func search(nums []int, target int) int {
    last := nums[len(nums)-1]
    left, right := -1, len(nums)-1 // 开区间 (-1, n-1)
    for left+1 < right { // 开区间不为空
        mid := left + (right-left)/2
        x := nums[mid]
        if target > last && x <= last { // target 在第一段，x 在第二段
            right = mid // 下轮循环去左边找
        } else if x > last && target <= last { // x 在第一段，target 在第二段
            left = mid // 下轮循环去右边找
        } else if x >= target { // 否则，x 和 target 在同一段，这就和方法一的 lowerBound 一样了
            right = mid
        } else {
            left = mid
        }
    }
    if nums[right] != target {
        return -1
    }
    return right
}
```

```js [sol-JavaScript]
var search = function(nums, target) {
    const last = nums[nums.length - 1];
    let left = -1, right = nums.length - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        const mid = Math.floor((left + right) / 2);
        const x = nums[mid];
        if (target > last && x <= last) { // target 在第一段，x 在第二段
            right = mid; // 下轮循环去左边找
        } else if (x > last && target <= last) { // x 在第一段，target 在第二段
            left = mid; // 下轮循环去右边找
        } else if (x >= target) { // 否则，x 和 target 在同一段，这就和方法一的 lowerBound 一样了
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right] === target ? right : -1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        let n = nums.len();
        let last = nums[n - 1];
        let mut left = 0;
        let mut right = n - 1; // 左闭右开区间 [0, n-1)
        while left < right { // 区间不为空
            let mid = left + (right - left) / 2;
            let x = nums[mid];
            if target > last && x <= last { // target 在第一段，x 在第二段
                right = mid; // 下轮循环去左边找
            } else if x > last && target <= last { // x 在第一段，target 在第二段
                left = mid + 1; // 下轮循环去右边找
            } else if x >= target { // 否则，x 和 target 在同一段，这就和方法一的 lower_bound 一样了
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        if nums[left] == target { left as _ } else { -1 }
    }
}
```

### 写法二

下面只讨论 $\textit{target}$ 在 $x$ 左边，或者 $x=\textit{target}$ 的情况。其余情况 $\textit{target}$ 一定在 $x$ 的右边。

- 如果 $x > \textit{nums}[n-1]$，说明 $x$ 在第一段中，那么 $\textit{target}$ 也必须在第一段中（否则 $\textit{target}$ 一定在 $x$ 的右边）且 $x$ 必须大于等于 $\textit{target}$。
   - 写成代码就是 `target > nums[n - 1] && x >= target`。
- 如果 $x \le \textit{nums}[n-1]$，说明 $x$ 在第二段中（或者 $\textit{nums}$ 只有一段），那么 $\textit{target}$ 可以在第一段，也可以在第二段。
   - 如果 $\textit{target}$ 在第一段，那么 $\textit{target}$ 一定在 $x$ 左边。
   - 如果 $\textit{target}$ 在第二段，那么 $x$ 必须大于等于 $\textit{target}$。
   - 写成代码就是 `target > nums[n - 1] || x >= target`。

根据这两种情况，去判断 $x$ 和 $\textit{target}$ 的位置关系，从而不断地缩小 $\textit{target}$ 所在位置的范围，二分找到 $\textit{target}$。

```py [sol-Python3]
class Solution:
    def search(self, nums: List[int], target: int) -> int:
        def check(i: int) -> bool:
            x = nums[i]
            if x > nums[-1]:
                return target > nums[-1] and x >= target
            return target > nums[-1] or x >= target

        left, right = -1, len(nums) - 1  # 开区间 (-1, n-1)
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right if nums[right] == target else -1
```

```py [sol-Python3 库函数]
class Solution:
    def search(self, nums: List[int], target: int) -> int:
        def check(i: int) -> bool:
            x = nums[i]
            if x > nums[-1]:
                return target > nums[-1] and x >= target
            return target > nums[-1] or x >= target

        i = bisect_left(range(len(nums) - 1), True, key=check)
        return i if nums[i] == target else -1
```

```java [sol-Java]
class Solution {
    public int search(int[] nums, int target) {
        int left = -1;
        int right = nums.length - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = (left + right) >>> 1;
            if (check(nums, target, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return nums[right] == target ? right : -1;
    }

    private boolean check(int[] nums, int target, int i) {
        int last = nums[nums.length - 1];
        int x = nums[i];
        if (x > last) {
            return target > last && x >= target;
        }
        return target > last || x >= target;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int last = nums.back();
        auto check = [&](int i) -> bool {
            int x = nums[i];
            if (x > last) {
                return target > last && x >= target;
            }
            return target > last || x >= target;
        };

        int left = -1, right = nums.size() - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid; // 更简洁的写法
        }
        return nums[right] == target ? right : -1;
    }
};
```

```c [sol-C]
int search(int* nums, int numsSize, int target) {
    int last = nums[numsSize - 1];

    bool check(int i) {
        int x = nums[i];
        if (x > last) {
            return target > last && x >= target;
        }
        return target > last || x >= target;
    }

    int left = -1, right = numsSize - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2;
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right] == target ? right : -1;
}
```

```go [sol-Go]
func search(nums []int, target int) int {
    last := nums[len(nums)-1]
    check := func(i int) bool {
        x := nums[i]
        if x > last {
            return target > last && x >= target
        }
        return target > last || x >= target
    }

    left, right := -1, len(nums)-1 // 开区间 (-1, n-1)
    for left+1 < right { // 开区间不为空
        mid := left + (right-left)/2
        if check(mid) {
            right = mid
        } else {
            left = mid
        }
    }
    if nums[right] != target {
        return -1
    }
    return right
}
```

```go [sol-Go 库函数]
func search(nums []int, target int) int {
    last := nums[len(nums)-1]
    i := sort.Search(len(nums)-1, func(i int) bool {
        x := nums[i]
        if x > last {
            return target > last && x >= target
        }
        return target > last || x >= target
    })
    if nums[i] != target {
        return -1
    }
    return i
}
```

```js [sol-JavaScript]
var search = function(nums, target) {
    const last = nums[nums.length - 1];
    function check(i) {
        const x = nums[i];
        if (x > last) {
            return target > last && x >= target;
        }
        return target > last || x >= target;
    }

    let left = -1, right = nums.length - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        const mid = Math.floor((left + right) / 2);
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right] === target ? right : -1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn search(nums: Vec<i32>, target: i32) -> i32 {
        let n = nums.len();
        let last = nums[n - 1];
        let check = |i: usize| -> bool {
            let x = nums[i];
            if x > last {
                target > last && x >= target
            } else {
                target > last || x >= target
            }
        };

        let mut left = 0;
        let mut right = n - 1; // 左闭右开区间 [0, n-1)
        while left < right { // 区间不为空
            let mid = left + (right - left) / 2;
            if check(mid) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        if nums[left] == target { left as _ } else { -1 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
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

## 本地原创解析

### 1. 题意重述

本题来自 `四、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `四、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
