# 611. 有效三角形的个数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/valid-triangle-number/
- 题目 slug：`valid-triangle-number`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/valid-triangle-number/solutions/2432875/zhuan-huan-cheng-abcyong-xiang-xiang-shu-1ex3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：枚举最长边/枚举最短边/值域卷积（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/valid-triangle-number/solutions/2432875/zhuan-huan-cheng-abcyong-xiang-xiang-shu-1ex3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-cheng-abcyong-xiang-xiang-shu-1ex3`
- topic id：`2432875`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 分析

首先明确计算规则：从示例 1 可以知道，对于三元组 $(2,3,4)$ 和 $(4,3,2)$，我们只统计了其中的 $(2,3,4)$，并没有把 $(4,3,2)$ 也统计到答案中，所以题目意思是把这两个三元组当成是同一个三元组，我们不能重复统计。

既然有这样的规则，那么不妨规定三角形的三条边 $a,b,c$ 满足：

$$
1\le a\le b\le c
$$

这可以保证我们在统计合法三元组 $(a,b,c)$ 的个数时，不会把 $(c,b,a)$ 这样的三元组也统计进去。

由于三角形两边之和大于第三边，我们有

$$
\begin{aligned}
a+b &> c           \\
a+c &> b           \\
b+c &> a           \\
\end{aligned}
$$

上式中的 $a+c>b$ 是必然成立的，因为 $a+c\ge a+b>b$（注意 $a$ 至少是 $1$）。

同样的，$b+c>a$ 也必然成立，因为 $b+c\ge a+a>a$（注意 $a$ 至少是 $1$）。

所以只需考虑第一个式子。现在问题变成：

- 从 $\textit{nums}$ 中选三个数 $a,b,c$，计算满足 $1\le a\le b\le c$ 且 $a+b>c$ 的方案数。

## 方法一：枚举最长边 + 相向双指针

关于**相向双指针**的原理，请看视频 [两数之和 三数之和【基础算法精讲 01】](https://www.bilibili.com/video/BV1bP411c7oJ/)。

为了能够使用相向双指针，先对数组从小到大排序。

外层循环**枚举最长边** $c=\textit{nums}[k]$，内层循环用相向双指针枚举 $a=\textit{nums}[i]$ 和 $b=\textit{nums}[j]$，具体如下：

1. 初始化左右指针 $i=0,\ j=k-1$。
2. 如果 $\textit{nums}[i]+\textit{nums}[j] > c$，由于数组是有序的，$\textit{nums}[j]$ 与下标 $i'$ 在 $[i,j-1]$ 中的任何 $\textit{nums}[i']$ 相加，都是 $>c$ 的，因此直接找到了 $j-i$ 个合法方案，加到答案中，然后将 $j$ 减一。
3. 如果 $\textit{nums}[i]+\textit{nums}[j] \le c$，由于数组是有序的，$\textit{nums}[i]$ 与下标 $j'$ 在 $[i+1,j]$ 中的任何 $\textit{nums}[j']$ 相加，都是 $\le c$ 的，因此后面无需考虑 $\textit{nums}[i]$，将 $i$ 加一。
4. 重复上述过程直到 $i\ge j$ 为止。

### 优化前

```py [sol-Python3]
class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        nums.sort()
        ans = 0
        for k in range(2, len(nums)):
            c = nums[k]
            i = 0  # a=nums[i]
            j = k - 1  # b=nums[j]
            while i < j:
                if nums[i] + nums[j] > c:
                    # 由于 nums 已经从小到大排序
                    # nums[i]+nums[j] > c 同时意味着：
                    # nums[i+1]+nums[j] > c
                    # nums[i+2]+nums[j] > c
                    # ...
                    # nums[j-1]+nums[j] > c
                    # 从 i 到 j-1 一共 j-i 个
                    ans += j - i
                    j -= 1
                else:
                    # 由于 nums 已经从小到大排序
                    # nums[i]+nums[j] <= c 同时意味着
                    # nums[i]+nums[j-1] <= c
                    # ...
                    # nums[i]+nums[i+1] <= c
                    # 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int triangleNumber(int[] nums) {
        Arrays.sort(nums);
        int ans = 0;
        for (int k = 2; k < nums.length; k++) {
            int c = nums[k];
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] > c 同时意味着：
                    // nums[i+1]+nums[j] > c
                    // nums[i+2]+nums[j] > c
                    // ...
                    // nums[j-1]+nums[j] > c
                    // 从 i 到 j-1 一共 j-i 个
                    ans += j - i;
                    j--;
                } else {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] <= c 同时意味着
                    // nums[i]+nums[j-1] <= c
                    // ...
                    // nums[i]+nums[i+1] <= c
                    // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i++;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int ans = 0;
        for (int k = 2; k < nums.size(); k++) {
            int c = nums[k];
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] > c 同时意味着：
                    // nums[i+1]+nums[j] > c
                    // nums[i+2]+nums[j] > c
                    // ...
                    // nums[j-1]+nums[j] > c
                    // 从 i 到 j-1 一共 j-i 个
                    ans += j - i;
                    j--;
                } else {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] <= c 同时意味着
                    // nums[i]+nums[j-1] <= c
                    // ...
                    // nums[i]+nums[i+1] <= c
                    // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i++;
                }
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

int triangleNumber(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    for (int k = 2; k < numsSize; k++) {
        int c = nums[k];
        int i = 0; // a=nums[i]
        int j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] > c 同时意味着：
                // nums[i+1]+nums[j] > c
                // nums[i+2]+nums[j] > c
                // ...
                // nums[j-1]+nums[j] > c
                // 从 i 到 j-1 一共 j-i 个
                ans += j - i;
                j--;
            } else {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] <= c 同时意味着
                // nums[i]+nums[j-1] <= c
                // ...
                // nums[i]+nums[i+1] <= c
                // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                i++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func triangleNumber(nums []int) (ans int) {
    slices.Sort(nums)
    for k := 2; k < len(nums); k++ {
        c := nums[k]
        i := 0 // a=nums[i]
        j := k - 1 // b=nums[j]
        for i < j {
            if nums[i]+nums[j] > c {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] > c 同时意味着：
                // nums[i+1]+nums[j] > c
                // nums[i+2]+nums[j] > c
                // ...
                // nums[j-1]+nums[j] > c
                // 从 i 到 j-1 一共 j-i 个
                ans += j - i
                j--
            } else {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] <= c 同时意味着
                // nums[i]+nums[j-1] <= c
                // ...
                // nums[i]+nums[i+1] <= c
                // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                i++
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var triangleNumber = function (nums) {
    nums.sort((a, b) => a - b);
    let ans = 0;
    for (let k = 2; k < nums.length; k++) {
        const c = nums[k];
        let i = 0; // a=nums[i]
        let j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] > c 同时意味着：
                // nums[i+1]+nums[j] > c
                // nums[i+2]+nums[j] > c
                // ...
                // nums[j-1]+nums[j] > c
                // 从 i 到 j-1 一共 j-i 个
                ans += j - i;
                j--;
            } else {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] <= c 同时意味着
                // nums[i]+nums[j-1] <= c
                // ...
                // nums[i]+nums[i+1] <= c
                // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                i++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn triangle_number(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let mut ans = 0;
        for k in 2..nums.len() {
            let c = nums[k];
            let mut i = 0; // a=nums[i]
            let mut j = k - 1; // b=nums[j]
            while i < j {
                if nums[i] + nums[j] > c {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] > c 同时意味着：
                    // nums[i+1]+nums[j] > c
                    // nums[i+2]+nums[j] > c
                    // ...
                    // nums[j-1]+nums[j] > c
                    // 从 i 到 j-1 一共 j-i 个
                    ans += j - i;
                    j -= 1;
                } else {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] <= c 同时意味着
                    // nums[i]+nums[j-1] <= c
                    // ...
                    // nums[i]+nums[i+1] <= c
                    // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i += 1;
                }
            }
        }
        ans as _
    }
}
```

### 优化

类似 [15. 三数之和的做法](https://leetcode.cn/problems/3sum/solution/shuang-zhi-zhen-xiang-bu-ming-bai-yi-ge-pno55/)，本题也有两个优化。

首先把循环改成倒序枚举 $k$。

**第一个优化**：在执行双指针之前，如果发现最小的 $a$ 和 $b$ 相加大于 $c$，也就是

$$
\textit{nums}[0] + \textit{nums}[1] > \textit{nums}[k]
$$

说明从 $\textit{nums}[0]$ 到 $\textit{nums}[k]$ 中任选三个数 $a,b,c$ 都满足 $a+b>c$，那么直接把 $C_{k+1}^3=\dfrac{(k+1)k(k-1)}{6}$ 加入答案，退出外层循环。这是为什么要倒序枚举 $k$ 的原因（正序枚举没法退出外层循环）。

**第二个优化**：在执行双指针之前，如果发现最大的 $a$ 和 $b$ 相加小于等于 $c$，也就是

$$
\textit{nums}[k-2] + \textit{nums}[k-1] \le \textit{nums}[k]
$$

说明不存在 $a+b>c$，不执行双指针，继续外层循环。

> 注：由于数据原因，上述优化在本题可能并不明显。

```py [sol-Python3]
class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        nums.sort()
        ans = 0
        for k in range(len(nums) - 1, 1, -1):
            c = nums[k]
            if nums[0] + nums[1] > c:  # 优化一
                ans += (k + 1) * k * (k - 1) // 6
                break
            if nums[k - 2] + nums[k - 1] <= c:  # 优化二
                continue
            i = 0  # a=nums[i]
            j = k - 1  # b=nums[j]
            while i < j:
                if nums[i] + nums[j] > c:
                    ans += j - i
                    j -= 1
                else:
                    i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int triangleNumber(int[] nums) {
        Arrays.sort(nums);
        int ans = 0;
        for (int k = nums.length - 1; k > 1; k--) {
            int c = nums[k];
            if (nums[0] + nums[1] > c) { // 优化一
                ans += (k + 1) * k * (k - 1) / 6;
                break;
            }
            if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
                continue;
            }
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    ans += j - i;
                    j--;
                } else {
                    i++;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int ans = 0;
        for (int k = nums.size() - 1; k > 1; k--) {
            int c = nums[k];
            if (nums[0] + nums[1] > c) { // 优化一
                ans += (k + 1) * k * (k - 1) / 6;
                break;
            }
            if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
                continue;
            }
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    ans += j - i;
                    j--;
                } else {
                    i++;
                }
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

int triangleNumber(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    for (int k = numsSize - 1; k > 1; k--) {
        int c = nums[k];
        if (nums[0] + nums[1] > c) { // 优化一
            ans += (k + 1) * k * (k - 1) / 6;
            break;
        }
        if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
            continue;
        }
        int i = 0; // a=nums[i]
        int j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                ans += j - i;
                j--;
            } else {
                i++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func triangleNumber(nums []int) (ans int) {
    slices.Sort(nums)
    for k := len(nums) - 1; k > 1; k-- {
        c := nums[k]
        if nums[0]+nums[1] > c { // 优化一
            ans += (k + 1) * k * (k - 1) / 6
            break
        }
        if nums[k-2]+nums[k-1] <= c { // 优化二
            continue
        }
        i := 0 // a=nums[i]
        j := k - 1 // b=nums[j]
        for i < j {
            if nums[i]+nums[j] > c {
                ans += j - i
                j--
            } else {
                i++
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var triangleNumber = function(nums) {
    nums.sort((a, b) => a - b);
    let ans = 0;
    for (let k = nums.length - 1; k > 1; k--) {
        const c = nums[k];
        if (nums[0] + nums[1] > c) { // 优化一
            ans += (k + 1) * k * (k - 1) / 6;
            break;
        }
        if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
            continue;
        }
        let i = 0; // a=nums[i]
        let j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                ans += j - i;
                j--;
            } else {
                i++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn triangle_number(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let mut ans = 0;
        for k in (2..nums.len()).rev() {
            let c = nums[k];
            if nums[0] + nums[1] > c { // 优化一
                ans += (k + 1) * k * (k - 1) / 6;
                break;
            }
            if nums[k - 2] + nums[k - 1] <= c { // 优化二
                continue;
            }
            let mut i = 0; // a=nums[i]
            let mut j = k - 1; // b=nums[j]
            while i < j {
                if nums[i] + nums[j] > c {
                    ans += j - i;
                    j -= 1;
                } else {
                    i += 1;
                }
            }
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 方法二：枚举最短边 + 同向双指针

枚举最短边 $a$，问题变成计算满足 $c-b<a$ 的 $(b,c)$ 个数。

这个条件意味着，当 $a$ 固定不变时，$b$ 和 $c$ 不能隔太远。

这可以用**同向双指针**解决，原理见 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

1. 枚举 $a=\textit{nums}[i]$，其中 $i=0,1,2,\ldots,n-3$。
2. 如果 $a=0$，则跳过。
3. 现在计算，对于 $k=i+2,i+3,\ldots,n-1$，有多少个符合要求的 $j$？。
4. 枚举 $k$ 的同时，维护指针 $j$，初始值为 $i+1$。
5. 如果发现 $\textit{nums}[k]-\textit{nums}[j]\ge a$，说明 $b$ 和 $c$ 隔太远了，那么把 $j$ 不断加一，直到 $c-b<a$，也就是 $\textit{nums}[k]-\textit{nums}[j]< a$ 为止。
6. 此时，对于固定的 $a=\textit{nums}[i]$ 和固定的 $c=\textit{nums}[k]$，$\textit{nums}[j],\textit{nums}[j+1],\ldots,\textit{nums}[k-1]$ 都可以作为 $b$，这一共有 $k-j$ 个，加入答案。

```py [sol-Python3]
class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        nums.sort()
        n = len(nums)
        ans = 0
        for i in range(n - 2):
            a = nums[i]
            if a == 0:  # 三角形的边不能是 0
                continue
            j = i + 1
            for k in range(i + 2, n):
                while nums[k] - nums[j] >= a:
                    j += 1
                # 如果 a=nums[i] 和 c=nums[k] 固定不变
                # 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j
        return ans
```

```java [sol-Java]
class Solution {
    public int triangleNumber(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        int ans = 0;
        for (int i = 0; i < n - 2; i++) {
            int a = nums[i];
            if (a == 0) { // 三角形的边不能是 0
                continue;
            }
            int j = i + 1;
            for (int k = i + 2; k < n; k++) {
                while (nums[k] - nums[j] >= a) {
                    j++;
                }
                // 如果 a=nums[i] 和 c=nums[k] 固定不变
                // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size(), ans = 0;
        for (int i = 0; i < n - 2; i++) {
            int a = nums[i];
            if (a == 0) { // 三角形的边不能是 0
                continue;
            }
            int j = i + 1;
            for (int k = i + 2; k < n; k++) {
                while (nums[k] - nums[j] >= a) {
                    j++;
                }
                // 如果 a=nums[i] 和 c=nums[k] 固定不变
                // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j;
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

int triangleNumber(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    for (int i = 0; i < numsSize - 2; i++) {
        int a = nums[i];
        if (a == 0) { // 三角形的边不能是 0
            continue;
        }
        int j = i + 1;
        for (int k = i + 2; k < numsSize; k++) {
            while (nums[k] - nums[j] >= a) {
                j++;
            }
            // 如果 a=nums[i] 和 c=nums[k] 固定不变
            // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
            ans += k - j;
        }
    }
    return ans;
}
```

```go [sol-Go]
func triangleNumber(nums []int) (ans int) {
    slices.Sort(nums)
    n := len(nums)
    for i, a := range nums[:max(n-2, 0)] {
        if a == 0 { // 三角形的边不能是 0
            continue
        }
        j := i + 1
        for k := i + 2; k < n; k++ {
            for nums[k]-nums[j] >= a {
                j++
            }
            // 如果 a=nums[i] 和 c=nums[k] 固定不变
            // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
            ans += k - j
        }
    }
    return
}
```

```js [sol-JavaScript]
var triangleNumber = function(nums) {
    nums.sort((a, b) => a - b);
    const n = nums.length;
    let ans = 0;
    for (let i = 0; i < n - 2; i++) {
        let a = nums[i];
        if (a === 0) { // 三角形的边不能是 0
            continue;
        }
        let j = i + 1;
        for (let k = i + 2; k < n; k++) {
            while (nums[k] - nums[j] >= a) {
                j++;
            }
            // 如果 a=nums[i] 和 c=nums[k] 固定不变
            // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
            ans += k - j;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn triangle_number(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let n = nums.len();
        let mut ans = 0;
        for i in 0..n.saturating_sub(2) {
            let a = nums[i];
            if a == 0 { // 三角形的边不能是 0
                continue;
            }
            let mut j = i + 1;
            for k in i + 2..n {
                while nums[k] - nums[j] >= a {
                    j += 1;
                }
                // 如果 a=nums[i] 和 c=nums[k] 固定不变
                // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j;
            }
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{nums}$ 的长度。里面的二重循环，由于 $j$ 一直在增大，所以里面的二重循环的时间复杂度是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 附：基于值域的卷积做法

本题 $U=\max(\textit{nums})\le 1000$ 比较小。可以先用 FFT（或者 NTT）计算 $\textit{nums}$（不包括 $0$）的频率数组 $\textit{cnt}$ 的自卷积，得到 $\textit{cnt}_2$，其中 $\textit{cnt}_2[i]$ 表示两数之和 $a+b = i$ 的方案数。

注意 $\textit{cnt}_2$ 包含了一些重复项：

- 如果 $i$ 是偶数，那么 $\textit{cnt}_2[i]$ 重复计算了 $(i/2,i/2)$ 这个数对（对于 $\textit{nums}$ 中的每个 $i/2$ 都重复算了一次），减去 $\textit{cnt}[i/2]$。
- 然后，$\textit{cnt}_2[i]$ 重复计算了 $a+b$ 和 $b+a$（$a\ne b$），要除以 $2$。

下文中的 $\textit{cnt}_2$ 已去掉重复项。

正难则反，用任选三个正整数的方案数，减去不合法的方案数，即为答案。

不合法方案即 $a+b\le c$ 的方案数。我们可以枚举 $c = 1,2,\dots,U$，同时计算 $\textit{cnt}_2$ 的前缀和 $\textit{sum}$，那么 $\textit{sum}\cdot \textit{cnt}[c]$ 就是 $a+b\le c$ 的三元组 $(a,b,c)$ 的个数。

```py [sol-Python3]
from scipy.signal import convolve

class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        mx = max(nums)
        cnt = [0] * (mx + 1)
        for x in nums:
            cnt[x] += 1
        c0 = cnt[0]
        cnt[0] = 0

        # 计算 cnt 的自卷积
        cnt2 = convolve(cnt, cnt).round().tolist()

        m = len(nums) - c0  # nums 中的正整数个数
        ans = comb(m, 3)
        s = 0  # a+b <= c 的 (a,b) 的方案数
        for c in range(1, mx + 1):
            c2 = cnt2[c]
            if c % 2 == 0:
                c2 -= cnt[c // 2]  # (c/2,c/2) 算了两次
            s += c2 // 2  # (a,b) 和 (b,a) 各算了一次
            ans -= s * cnt[c]  # 减去 a+b <= c 的 (a,b,c) 的方案数
        return ans
```

```go [sol-Go]
type fft struct {
	n        int
	omega    []complex128
	omegaInv []complex128
}

func newFFT(n int) *fft {
	omega := make([]complex128, n)
	omegaInv := make([]complex128, n)
	for i := range omega {
		sin, cos := math.Sincos(2 * math.Pi * float64(i) / float64(n))
		omega[i] = complex(cos, sin)
		omegaInv[i] = complex(cos, -sin)
	}
	return &fft{n, omega, omegaInv}
}

func (t *fft) transform(a, omega []complex128) {
	n := t.n
	for i, j := 0, 0; i < n; i++ {
		if i > j { // 保证同一对元素只交换一次
			a[i], a[j] = a[j], a[i]
		}
		for l := n / 2; ; l /= 2 {
			j ^= l
			if j >= l {
				break
			}
		}
	}
	for l := 2; l <= n; l *= 2 {
		m := l / 2
		for st := 0; st < n; st += l {
			b := a[st:]
			for i := range m {
				v := omega[n/l*i] * b[m+i]
				b[m+i] = b[i] - v
				b[i] += v
			}
		}
	}
}

func (t *fft) dft(a []complex128) {
	t.transform(a, t.omega)
}

func (t *fft) idft(a []complex128) {
	t.transform(a, t.omegaInv)
	cn := complex(float64(t.n), 0)
	for i := range a {
		a[i] /= cn
	}
}

// 计算 a 的自卷积
func selfPolyConvFFT(a []int) []int {
	n := len(a)
	limit := 1 << bits.Len(uint(n*2-1))
	A := make([]complex128, limit)
	for i, v := range a {
		A[i] = complex(float64(v), 0)
	}

	t := newFFT(limit)
	t.dft(A)
	for i, x := range A {
		A[i] *= x
	}
	t.idft(A)

	conv := make([]int, n*2-1)
	for i := range conv {
		conv[i] = int(math.Round(real(A[i])))
	}
	return conv
}

func triangleNumber(nums []int) int {
	mx := slices.Max(nums)
	cnt := make([]int, mx+1)
	for _, x := range nums {
		cnt[x]++
	}
	c0 := cnt[0]
	cnt[0] = 0

	cnt2 := selfPolyConvFFT(cnt)

	m := len(nums) - c0 // nums 中的正整数个数
	ans := m * (m - 1) * (m - 2) / 6 // C(m,3)
	sum := 0 // a+b <= c 的 (a,b) 的方案数
	for c := 1; c <= mx; c++ {
		c2 := cnt2[c]
		if c%2 == 0 {
			c2 -= cnt[c/2] // (c/2,c/2) 算了两次
		}
		sum += c2 / 2 // (a,b) 和 (b,a) 各算了一次
		ans -= sum * cnt[c] // 减去 a+b <= c 的 (a,b,c) 的方案数
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。本题 $n$ 和 $U$ 的数量级相同，所以瓶颈在计算卷积上。
- 空间复杂度：$\mathcal{O}(U)$。

## 相似题目

- [923. 三数之和的多种可能](https://leetcode.cn/problems/3sum-with-multiplicity/)
- [259. 较小的三数之和](https://leetcode.cn/problems/3sum-smaller/)（会员题）

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

## 分析

首先明确计算规则：从示例 1 可以知道，对于三元组 $(2,3,4)$ 和 $(4,3,2)$，我们只统计了其中的 $(2,3,4)$，并没有把 $(4,3,2)$ 也统计到答案中，所以题目意思是把这两个三元组当成是同一个三元组，我们不能重复统计。

既然有这样的规则，那么不妨规定三角形的三条边 $a,b,c$ 满足：

$$
1\le a\le b\le c
$$

这可以保证我们在统计合法三元组 $(a,b,c)$ 的个数时，不会把 $(c,b,a)$ 这样的三元组也统计进去。

由于三角形两边之和大于第三边，我们有

$$
\begin{aligned}
a+b &> c           \\
a+c &> b           \\
b+c &> a           \\
\end{aligned}
$$

上式中的 $a+c>b$ 是必然成立的，因为 $a+c\ge a+b>b$（注意 $a$ 至少是 $1$）。

同样的，$b+c>a$ 也必然成立，因为 $b+c\ge a+a>a$（注意 $a$ 至少是 $1$）。

所以只需考虑第一个式子。现在问题变成：

- 从 $\textit{nums}$ 中选三个数 $a,b,c$，计算满足 $1\le a\le b\le c$ 且 $a+b>c$ 的方案数。

## 方法一：枚举最长边 + 相向双指针

关于**相向双指针**的原理，请看视频 [两数之和 三数之和【基础算法精讲 01】](https://www.bilibili.com/video/BV1bP411c7oJ/)。

为了能够使用相向双指针，先对数组从小到大排序。

外层循环**枚举最长边** $c=\textit{nums}[k]$，内层循环用相向双指针枚举 $a=\textit{nums}[i]$ 和 $b=\textit{nums}[j]$，具体如下：

1. 初始化左右指针 $i=0,\ j=k-1$。
2. 如果 $\textit{nums}[i]+\textit{nums}[j] > c$，由于数组是有序的，$\textit{nums}[j]$ 与下标 $i'$ 在 $[i,j-1]$ 中的任何 $\textit{nums}[i']$ 相加，都是 $>c$ 的，因此直接找到了 $j-i$ 个合法方案，加到答案中，然后将 $j$ 减一。
3. 如果 $\textit{nums}[i]+\textit{nums}[j] \le c$，由于数组是有序的，$\textit{nums}[i]$ 与下标 $j'$ 在 $[i+1,j]$ 中的任何 $\textit{nums}[j']$ 相加，都是 $\le c$ 的，因此后面无需考虑 $\textit{nums}[i]$，将 $i$ 加一。
4. 重复上述过程直到 $i\ge j$ 为止。

### 优化前

```py [sol-Python3]
class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        nums.sort()
        ans = 0
        for k in range(2, len(nums)):
            c = nums[k]
            i = 0  # a=nums[i]
            j = k - 1  # b=nums[j]
            while i < j:
                if nums[i] + nums[j] > c:
                    # 由于 nums 已经从小到大排序
                    # nums[i]+nums[j] > c 同时意味着：
                    # nums[i+1]+nums[j] > c
                    # nums[i+2]+nums[j] > c
                    # ...
                    # nums[j-1]+nums[j] > c
                    # 从 i 到 j-1 一共 j-i 个
                    ans += j - i
                    j -= 1
                else:
                    # 由于 nums 已经从小到大排序
                    # nums[i]+nums[j] <= c 同时意味着
                    # nums[i]+nums[j-1] <= c
                    # ...
                    # nums[i]+nums[i+1] <= c
                    # 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int triangleNumber(int[] nums) {
        Arrays.sort(nums);
        int ans = 0;
        for (int k = 2; k < nums.length; k++) {
            int c = nums[k];
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] > c 同时意味着：
                    // nums[i+1]+nums[j] > c
                    // nums[i+2]+nums[j] > c
                    // ...
                    // nums[j-1]+nums[j] > c
                    // 从 i 到 j-1 一共 j-i 个
                    ans += j - i;
                    j--;
                } else {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] <= c 同时意味着
                    // nums[i]+nums[j-1] <= c
                    // ...
                    // nums[i]+nums[i+1] <= c
                    // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i++;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int ans = 0;
        for (int k = 2; k < nums.size(); k++) {
            int c = nums[k];
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] > c 同时意味着：
                    // nums[i+1]+nums[j] > c
                    // nums[i+2]+nums[j] > c
                    // ...
                    // nums[j-1]+nums[j] > c
                    // 从 i 到 j-1 一共 j-i 个
                    ans += j - i;
                    j--;
                } else {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] <= c 同时意味着
                    // nums[i]+nums[j-1] <= c
                    // ...
                    // nums[i]+nums[i+1] <= c
                    // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i++;
                }
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

int triangleNumber(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    for (int k = 2; k < numsSize; k++) {
        int c = nums[k];
        int i = 0; // a=nums[i]
        int j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] > c 同时意味着：
                // nums[i+1]+nums[j] > c
                // nums[i+2]+nums[j] > c
                // ...
                // nums[j-1]+nums[j] > c
                // 从 i 到 j-1 一共 j-i 个
                ans += j - i;
                j--;
            } else {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] <= c 同时意味着
                // nums[i]+nums[j-1] <= c
                // ...
                // nums[i]+nums[i+1] <= c
                // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                i++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func triangleNumber(nums []int) (ans int) {
    slices.Sort(nums)
    for k := 2; k < len(nums); k++ {
        c := nums[k]
        i := 0 // a=nums[i]
        j := k - 1 // b=nums[j]
        for i < j {
            if nums[i]+nums[j] > c {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] > c 同时意味着：
                // nums[i+1]+nums[j] > c
                // nums[i+2]+nums[j] > c
                // ...
                // nums[j-1]+nums[j] > c
                // 从 i 到 j-1 一共 j-i 个
                ans += j - i
                j--
            } else {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] <= c 同时意味着
                // nums[i]+nums[j-1] <= c
                // ...
                // nums[i]+nums[i+1] <= c
                // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                i++
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var triangleNumber = function (nums) {
    nums.sort((a, b) => a - b);
    let ans = 0;
    for (let k = 2; k < nums.length; k++) {
        const c = nums[k];
        let i = 0; // a=nums[i]
        let j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] > c 同时意味着：
                // nums[i+1]+nums[j] > c
                // nums[i+2]+nums[j] > c
                // ...
                // nums[j-1]+nums[j] > c
                // 从 i 到 j-1 一共 j-i 个
                ans += j - i;
                j--;
            } else {
                // 由于 nums 已经从小到大排序
                // nums[i]+nums[j] <= c 同时意味着
                // nums[i]+nums[j-1] <= c
                // ...
                // nums[i]+nums[i+1] <= c
                // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                i++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn triangle_number(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let mut ans = 0;
        for k in 2..nums.len() {
            let c = nums[k];
            let mut i = 0; // a=nums[i]
            let mut j = k - 1; // b=nums[j]
            while i < j {
                if nums[i] + nums[j] > c {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] > c 同时意味着：
                    // nums[i+1]+nums[j] > c
                    // nums[i+2]+nums[j] > c
                    // ...
                    // nums[j-1]+nums[j] > c
                    // 从 i 到 j-1 一共 j-i 个
                    ans += j - i;
                    j -= 1;
                } else {
                    // 由于 nums 已经从小到大排序
                    // nums[i]+nums[j] <= c 同时意味着
                    // nums[i]+nums[j-1] <= c
                    // ...
                    // nums[i]+nums[i+1] <= c
                    // 所以在后续的内层循环中，nums[i] 不可能作为三角形的边长，没有用了
                    i += 1;
                }
            }
        }
        ans as _
    }
}
```

### 优化

类似 [15. 三数之和的做法](https://leetcode.cn/problems/3sum/solution/shuang-zhi-zhen-xiang-bu-ming-bai-yi-ge-pno55/)，本题也有两个优化。

首先把循环改成倒序枚举 $k$。

**第一个优化**：在执行双指针之前，如果发现最小的 $a$ 和 $b$ 相加大于 $c$，也就是

$$
\textit{nums}[0] + \textit{nums}[1] > \textit{nums}[k]
$$

说明从 $\textit{nums}[0]$ 到 $\textit{nums}[k]$ 中任选三个数 $a,b,c$ 都满足 $a+b>c$，那么直接把 $C_{k+1}^3=\dfrac{(k+1)k(k-1)}{6}$ 加入答案，退出外层循环。这是为什么要倒序枚举 $k$ 的原因（正序枚举没法退出外层循环）。

**第二个优化**：在执行双指针之前，如果发现最大的 $a$ 和 $b$ 相加小于等于 $c$，也就是

$$
\textit{nums}[k-2] + \textit{nums}[k-1] \le \textit{nums}[k]
$$

说明不存在 $a+b>c$，不执行双指针，继续外层循环。

> 注：由于数据原因，上述优化在本题可能并不明显。

```py [sol-Python3]
class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        nums.sort()
        ans = 0
        for k in range(len(nums) - 1, 1, -1):
            c = nums[k]
            if nums[0] + nums[1] > c:  # 优化一
                ans += (k + 1) * k * (k - 1) // 6
                break
            if nums[k - 2] + nums[k - 1] <= c:  # 优化二
                continue
            i = 0  # a=nums[i]
            j = k - 1  # b=nums[j]
            while i < j:
                if nums[i] + nums[j] > c:
                    ans += j - i
                    j -= 1
                else:
                    i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int triangleNumber(int[] nums) {
        Arrays.sort(nums);
        int ans = 0;
        for (int k = nums.length - 1; k > 1; k--) {
            int c = nums[k];
            if (nums[0] + nums[1] > c) { // 优化一
                ans += (k + 1) * k * (k - 1) / 6;
                break;
            }
            if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
                continue;
            }
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    ans += j - i;
                    j--;
                } else {
                    i++;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int ans = 0;
        for (int k = nums.size() - 1; k > 1; k--) {
            int c = nums[k];
            if (nums[0] + nums[1] > c) { // 优化一
                ans += (k + 1) * k * (k - 1) / 6;
                break;
            }
            if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
                continue;
            }
            int i = 0; // a=nums[i]
            int j = k - 1; // b=nums[j]
            while (i < j) {
                if (nums[i] + nums[j] > c) {
                    ans += j - i;
                    j--;
                } else {
                    i++;
                }
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

int triangleNumber(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    for (int k = numsSize - 1; k > 1; k--) {
        int c = nums[k];
        if (nums[0] + nums[1] > c) { // 优化一
            ans += (k + 1) * k * (k - 1) / 6;
            break;
        }
        if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
            continue;
        }
        int i = 0; // a=nums[i]
        int j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                ans += j - i;
                j--;
            } else {
                i++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func triangleNumber(nums []int) (ans int) {
    slices.Sort(nums)
    for k := len(nums) - 1; k > 1; k-- {
        c := nums[k]
        if nums[0]+nums[1] > c { // 优化一
            ans += (k + 1) * k * (k - 1) / 6
            break
        }
        if nums[k-2]+nums[k-1] <= c { // 优化二
            continue
        }
        i := 0 // a=nums[i]
        j := k - 1 // b=nums[j]
        for i < j {
            if nums[i]+nums[j] > c {
                ans += j - i
                j--
            } else {
                i++
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var triangleNumber = function(nums) {
    nums.sort((a, b) => a - b);
    let ans = 0;
    for (let k = nums.length - 1; k > 1; k--) {
        const c = nums[k];
        if (nums[0] + nums[1] > c) { // 优化一
            ans += (k + 1) * k * (k - 1) / 6;
            break;
        }
        if (nums[k - 2] + nums[k - 1] <= c) { // 优化二
            continue;
        }
        let i = 0; // a=nums[i]
        let j = k - 1; // b=nums[j]
        while (i < j) {
            if (nums[i] + nums[j] > c) {
                ans += j - i;
                j--;
            } else {
                i++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn triangle_number(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let mut ans = 0;
        for k in (2..nums.len()).rev() {
            let c = nums[k];
            if nums[0] + nums[1] > c { // 优化一
                ans += (k + 1) * k * (k - 1) / 6;
                break;
            }
            if nums[k - 2] + nums[k - 1] <= c { // 优化二
                continue;
            }
            let mut i = 0; // a=nums[i]
            let mut j = k - 1; // b=nums[j]
            while i < j {
                if nums[i] + nums[j] > c {
                    ans += j - i;
                    j -= 1;
                } else {
                    i += 1;
                }
            }
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 方法二：枚举最短边 + 同向双指针

枚举最短边 $a$，问题变成计算满足 $c-b<a$ 的 $(b,c)$ 个数。

这个条件意味着，当 $a$ 固定不变时，$b$ 和 $c$ 不能隔太远。

这可以用**同向双指针**解决，原理见 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

1. 枚举 $a=\textit{nums}[i]$，其中 $i=0,1,2,\ldots,n-3$。
2. 如果 $a=0$，则跳过。
3. 现在计算，对于 $k=i+2,i+3,\ldots,n-1$，有多少个符合要求的 $j$？。
4. 枚举 $k$ 的同时，维护指针 $j$，初始值为 $i+1$。
5. 如果发现 $\textit{nums}[k]-\textit{nums}[j]\ge a$，说明 $b$ 和 $c$ 隔太远了，那么把 $j$ 不断加一，直到 $c-b<a$，也就是 $\textit{nums}[k]-\textit{nums}[j]< a$ 为止。
6. 此时，对于固定的 $a=\textit{nums}[i]$ 和固定的 $c=\textit{nums}[k]$，$\textit{nums}[j],\textit{nums}[j+1],\ldots,\textit{nums}[k-1]$ 都可以作为 $b$，这一共有 $k-j$ 个，加入答案。

```py [sol-Python3]
class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        nums.sort()
        n = len(nums)
        ans = 0
        for i in range(n - 2):
            a = nums[i]
            if a == 0:  # 三角形的边不能是 0
                continue
            j = i + 1
            for k in range(i + 2, n):
                while nums[k] - nums[j] >= a:
                    j += 1
                # 如果 a=nums[i] 和 c=nums[k] 固定不变
                # 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j
        return ans
```

```java [sol-Java]
class Solution {
    public int triangleNumber(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        int ans = 0;
        for (int i = 0; i < n - 2; i++) {
            int a = nums[i];
            if (a == 0) { // 三角形的边不能是 0
                continue;
            }
            int j = i + 1;
            for (int k = i + 2; k < n; k++) {
                while (nums[k] - nums[j] >= a) {
                    j++;
                }
                // 如果 a=nums[i] 和 c=nums[k] 固定不变
                // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size(), ans = 0;
        for (int i = 0; i < n - 2; i++) {
            int a = nums[i];
            if (a == 0) { // 三角形的边不能是 0
                continue;
            }
            int j = i + 1;
            for (int k = i + 2; k < n; k++) {
                while (nums[k] - nums[j] >= a) {
                    j++;
                }
                // 如果 a=nums[i] 和 c=nums[k] 固定不变
                // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j;
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

int triangleNumber(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    for (int i = 0; i < numsSize - 2; i++) {
        int a = nums[i];
        if (a == 0) { // 三角形的边不能是 0
            continue;
        }
        int j = i + 1;
        for (int k = i + 2; k < numsSize; k++) {
            while (nums[k] - nums[j] >= a) {
                j++;
            }
            // 如果 a=nums[i] 和 c=nums[k] 固定不变
            // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
            ans += k - j;
        }
    }
    return ans;
}
```

```go [sol-Go]
func triangleNumber(nums []int) (ans int) {
    slices.Sort(nums)
    n := len(nums)
    for i, a := range nums[:max(n-2, 0)] {
        if a == 0 { // 三角形的边不能是 0
            continue
        }
        j := i + 1
        for k := i + 2; k < n; k++ {
            for nums[k]-nums[j] >= a {
                j++
            }
            // 如果 a=nums[i] 和 c=nums[k] 固定不变
            // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
            ans += k - j
        }
    }
    return
}
```

```js [sol-JavaScript]
var triangleNumber = function(nums) {
    nums.sort((a, b) => a - b);
    const n = nums.length;
    let ans = 0;
    for (let i = 0; i < n - 2; i++) {
        let a = nums[i];
        if (a === 0) { // 三角形的边不能是 0
            continue;
        }
        let j = i + 1;
        for (let k = i + 2; k < n; k++) {
            while (nums[k] - nums[j] >= a) {
                j++;
            }
            // 如果 a=nums[i] 和 c=nums[k] 固定不变
            // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
            ans += k - j;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn triangle_number(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let n = nums.len();
        let mut ans = 0;
        for i in 0..n.saturating_sub(2) {
            let a = nums[i];
            if a == 0 { // 三角形的边不能是 0
                continue;
            }
            let mut j = i + 1;
            for k in i + 2..n {
                while nums[k] - nums[j] >= a {
                    j += 1;
                }
                // 如果 a=nums[i] 和 c=nums[k] 固定不变
                // 那么 b 可以是 nums[j],nums[j+1],...,nums[k-1]，一共有 k-j 个
                ans += k - j;
            }
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{nums}$ 的长度。里面的二重循环，由于 $j$ 一直在增大，所以里面的二重循环的时间复杂度是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 附：基于值域的卷积做法

本题 $U=\max(\textit{nums})\le 1000$ 比较小。可以先用 FFT（或者 NTT）计算 $\textit{nums}$（不包括 $0$）的频率数组 $\textit{cnt}$ 的自卷积，得到 $\textit{cnt}_2$，其中 $\textit{cnt}_2[i]$ 表示两数之和 $a+b = i$ 的方案数。

注意 $\textit{cnt}_2$ 包含了一些重复项：

- 如果 $i$ 是偶数，那么 $\textit{cnt}_2[i]$ 重复计算了 $(i/2,i/2)$ 这个数对（对于 $\textit{nums}$ 中的每个 $i/2$ 都重复算了一次），减去 $\textit{cnt}[i/2]$。
- 然后，$\textit{cnt}_2[i]$ 重复计算了 $a+b$ 和 $b+a$（$a\ne b$），要除以 $2$。

下文中的 $\textit{cnt}_2$ 已去掉重复项。

正难则反，用任选三个正整数的方案数，减去不合法的方案数，即为答案。

不合法方案即 $a+b\le c$ 的方案数。我们可以枚举 $c = 1,2,\dots,U$，同时计算 $\textit{cnt}_2$ 的前缀和 $\textit{sum}$，那么 $\textit{sum}\cdot \textit{cnt}[c]$ 就是 $a+b\le c$ 的三元组 $(a,b,c)$ 的个数。

```py [sol-Python3]
from scipy.signal import convolve

class Solution:
    def triangleNumber(self, nums: List[int]) -> int:
        mx = max(nums)
        cnt = [0] * (mx + 1)
        for x in nums:
            cnt[x] += 1
        c0 = cnt[0]
        cnt[0] = 0

        # 计算 cnt 的自卷积
        cnt2 = convolve(cnt, cnt).round().tolist()

        m = len(nums) - c0  # nums 中的正整数个数
        ans = comb(m, 3)
        s = 0  # a+b <= c 的 (a,b) 的方案数
        for c in range(1, mx + 1):
            c2 = cnt2[c]
            if c % 2 == 0:
                c2 -= cnt[c // 2]  # (c/2,c/2) 算了两次
            s += c2 // 2  # (a,b) 和 (b,a) 各算了一次
            ans -= s * cnt[c]  # 减去 a+b <= c 的 (a,b,c) 的方案数
        return ans
```

```go [sol-Go]
type fft struct {
	n        int
	omega    []complex128
	omegaInv []complex128
}

func newFFT(n int) *fft {
	omega := make([]complex128, n)
	omegaInv := make([]complex128, n)
	for i := range omega {
		sin, cos := math.Sincos(2 * math.Pi * float64(i) / float64(n))
		omega[i] = complex(cos, sin)
		omegaInv[i] = complex(cos, -sin)
	}
	return &fft{n, omega, omegaInv}
}

func (t *fft) transform(a, omega []complex128) {
	n := t.n
	for i, j := 0, 0; i < n; i++ {
		if i > j { // 保证同一对元素只交换一次
			a[i], a[j] = a[j], a[i]
		}
		for l := n / 2; ; l /= 2 {
			j ^= l
			if j >= l {
				break
			}
		}
	}
	for l := 2; l <= n; l *= 2 {
		m := l / 2
		for st := 0; st < n; st += l {
			b := a[st:]
			for i := range m {
				v := omega[n/l*i] * b[m+i]
				b[m+i] = b[i] - v
				b[i] += v
			}
		}
	}
}

func (t *fft) dft(a []complex128) {
	t.transform(a, t.omega)
}

func (t *fft) idft(a []complex128) {
	t.transform(a, t.omegaInv)
	cn := complex(float64(t.n), 0)
	for i := range a {
		a[i] /= cn
	}
}

// 计算 a 的自卷积
func selfPolyConvFFT(a []int) []int {
	n := len(a)
	limit := 1 << bits.Len(uint(n*2-1))
	A := make([]complex128, limit)
	for i, v := range a {
		A[i] = complex(float64(v), 0)
	}

	t := newFFT(limit)
	t.dft(A)
	for i, x := range A {
		A[i] *= x
	}
	t.idft(A)

	conv := make([]int, n*2-1)
	for i := range conv {
		conv[i] = int(math.Round(real(A[i])))
	}
	return conv
}

func triangleNumber(nums []int) int {
	mx := slices.Max(nums)
	cnt := make([]int, mx+1)
	for _, x := range nums {
		cnt[x]++
	}
	c0 := cnt[0]
	cnt[0] = 0

	cnt2 := selfPolyConvFFT(cnt)

	m := len(nums) - c0 // nums 中的正整数个数
	ans := m * (m - 1) * (m - 2) / 6 // C(m,3)
	sum := 0 // a+b <= c 的 (a,b) 的方案数
	for c := 1; c <= mx; c++ {
		c2 := cnt2[c]
		if c%2 == 0 {
			c2 -= cnt[c/2] // (c/2,c/2) 算了两次
		}
		sum += c2 / 2 // (a,b) 和 (b,a) 各算了一次
		ans -= sum * cnt[c] // 减去 a+b <= c 的 (a,b,c) 的方案数
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。本题 $n$ 和 $U$ 的数量级相同，所以瓶颈在计算卷积上。
- 空间复杂度：$\mathcal{O}(U)$。

## 相似题目

- [923. 三数之和的多种可能](https://leetcode.cn/problems/3sum-with-multiplicity/)
- [259. 较小的三数之和](https://leetcode.cn/problems/3sum-smaller/)（会员题）

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

## 本地原创解析

### 1. 题意重述

本题来自 `三、单序列双指针 / §3.2 相向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.2 相向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
