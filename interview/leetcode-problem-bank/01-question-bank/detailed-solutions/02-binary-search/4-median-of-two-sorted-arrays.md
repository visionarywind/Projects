# 4. 寻找两个正序数组的中位数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/median-of-two-sorted-arrays/
- 题目 slug：`median-of-two-sorted-arrays`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/median-of-two-sorted-arrays/solutions/2950686/tu-jie-xun-xu-jian-jin-cong-shuang-zhi-z-p2gd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】循序渐进：从排序到双指针到二分（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/median-of-two-sorted-arrays/solutions/2950686/tu-jie-xun-xu-jian-jin-cong-shuang-zhi-z-p2gd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-xun-xu-jian-jin-cong-shuang-zhi-z-p2gd`
- topic id：`2950686`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前言

本质上，我们要在两个有序数组中，找到第 $k$ 小的数，其中 $k=\left\lceil\dfrac{m+n}{2}\right\rceil$。

- 如果 $m+n$ 是奇数，返回第 $k$ 小的数。例如 $m+n=5$，返回第 $\left\lceil\dfrac{5}{2}\right\rceil = 3$ 小的数。
- 如果 $m+n$ 是偶数，返回第 $k$ 小的数和第 $k+1$ 小的数的平均值。例如 $m+n=6$，返回第 $3$ 小的数和第 $4$ 小的数的平均值。

本文先从最暴力的**排序**做法开始，然后讲解**双指针**做法，最后过渡到**二分**做法。

## 一、引入：均匀分组

![lc4-1-c.png](https://pic.leetcode.cn/1728911111-PpxpUR-lc4-1-c.png)

这里的关键是「均匀分组」，每组 $5$ 个数，只要第一组的最大值 $\le$ 第二组的最小值，我们就找到了答案。

怎么想到要均匀分组的？请看百科中关于中位数的介绍：

> 中位数……可将数值集合**划分**为**大小相等**的两部分。

设 $\textit{merged}$ 为 $a+b$ 排序后的数组。

第 $k$ 小的数在 $\textit{merged}$ 中的下标为 $k-1$，也就是 $\left\lceil\dfrac{m+n}{2}\right\rceil - 1 = \left\lfloor\dfrac{m+n+1}{2}\right\rfloor - 1 = \left\lfloor\dfrac{m+n-1}{2}\right\rfloor$。

```py [sol-Python3]
class Solution:
    def findMedianSortedArrays(self, a: List[int], b: List[int]) -> float:
        merged = a + b
        merged.sort()

        s = len(merged)
        k = (s - 1) // 2
        return merged[k] if s % 2 else (merged[k] + merged[k + 1]) / 2
```

```java [sol-Java]
class Solution {
    public double findMedianSortedArrays(int[] a, int[] b) {
        int m = a.length;
        int n = b.length;
        int[] merged = new int[m + n];
        System.arraycopy(a, 0, merged, 0, m);
        System.arraycopy(b, 0, merged, m, n);
        Arrays.sort(merged);

        int s = m + n;
        int k = (s - 1) / 2;
        return s % 2 > 0 ? merged[k] : (merged[k] + merged[k + 1]) / 2.0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double findMedianSortedArrays(vector<int>& a, vector<int>& b) {
        auto merged = a;
        merged.insert(merged.end(), b.begin(), b.end());
        ranges::sort(merged);

        int s = merged.size();
        int k = (s - 1) / 2;
        return s % 2 ? merged[k] : (merged[k] + merged[k + 1]) / 2.0;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

double findMedianSortedArrays(int* a, int m, int* b, int n) {
    int s = m + n;
    int* merged = malloc(s * sizeof(int));
    memcpy(merged, a, m * sizeof(int));
    memcpy(merged + m, b, n * sizeof(int));
    qsort(merged, s, sizeof(int), cmp);

    int k = (s - 1) / 2;
    double ans = s % 2 ? merged[k] : (merged[k] + merged[k + 1]) / 2.0;

    free(merged);
    return ans;
}
```

```go [sol-Go]
func findMedianSortedArrays(a, b []int) float64 {
	merged := append(slices.Clone(a), b...)
	slices.Sort(merged)

	s := len(merged)
	k := (s - 1) / 2
	if s%2 > 0 {
		return float64(merged[k])
	}
	return float64(merged[k]+merged[k+1]) / 2
}
```

```js [sol-JavaScript]
var findMedianSortedArrays = function(a, b) {
    const merged = a.concat(b);
    merged.sort((x, y) => x - y);

    const s = merged.length;
    const k = Math.floor((s - 1) / 2);
    return s % 2 ? merged[k] : (merged[k] + merged[k + 1]) / 2;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_median_sorted_arrays(a: Vec<i32>, b: Vec<i32>) -> f64 {
        let mut merged = a;
        merged.extend(b);
        merged.sort_unstable();

        let s = merged.len();
        let k = (s - 1) / 2;
        if s % 2 > 0 {
            merged[k] as f64
        } else {
            (merged[k] + merged[k + 1]) as f64 / 2.0
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((m+n)\log (m+n))$，其中 $m$ 是 $a$ 的长度，$n$ 是 $b$ 的长度。
- 空间复杂度：$\mathcal{O}(m+n)$。

## 二、枚举：双指针做法

> 如果您在阅读的过程中产生了一些疑问，请看后文的「答疑」。

![lc4-4-c2.png](https://pic.leetcode.cn/1738370778-kTmyuh-lc4-4-c2.png)

下面来说具体做法。

设 $a$ 的 $b$ 的长度分别为 $m$ 和 $n$，且 $m\le n$（如果 $m > n$ 则交换 $a$ 和 $b$）。

- 为方便处理 $i=-1$，即 $a$ 有 $0$ 个数在第一组的情况，我们可以往 $a$ 的最左边插入一个哨兵 $-\infty$，这可以保证数组仍然是有序的。对于 $j=-1$ 的情况也同理，往 $b$ 的最左边插入一个 $-\infty$。
- 为方便处理 $i+1=m$，即 $a$ 有 $m$ 个数在第一组的情况，我们可以往 $a$ 的最右边插入一个哨兵 $\infty$，这可以保证数组仍然是有序的。对于 $j+1=n$ 的情况也同理，往 $b$ 的最右边插入一个 $\infty$。这可以避免 $a_{i+1}$ 和 $b_{j+1}$ 下标越界。
- 插入 $-\infty$ 和 $\infty$ 后，便可保证无论 $a$ 和 $b$ 是什么样的，一定存在一个 $i$，满足 $a_i\le b_{j+1}$ 且 $a_{i+1} > b_j$。
- $m$ 和 $n$ 的值不变。

如此修改后，$i$ 的含义变成了 $a$ 有 $i$ 个数在第一组，$j$ 的含义变成了 $b$ 有 $j$ 个数在第一组。

初始化 $i=0$，那么 $j$ 应该初始化成多少？

- 如果 $m+n$ 是偶数，那么每组的大小为 $\dfrac{m+n}{2}$，$j$ 应当初始化成 $\dfrac{m+n}{2}$。
- 如果 $m+n$ 是奇数，我们规定第一组比第二组多一个数，第一组的大小为 $\dfrac{m+n+1}{2}$，$j$ 应当初始化成 $\dfrac{m+n+1}{2}$。

两种情况可以合并为：$j$ 初始化成 $\left\lfloor\dfrac{m+n+1}{2}\right\rfloor$。

为了保证组的大小不变，$i$ 每增加 $1$，$j$ 就要减少 $1$。所以有

$$
j = \left\lfloor\dfrac{m+n+1}{2}\right\rfloor - i
$$

根据图片中的结论，只要发现 $a_i\le b_{j+1}$ 且 $a_{i+1} > b_j$，那么：

- 如果 $m+n$ 是偶数，中位数为 $\max(a_i,b_j)$ 和 $\min(a_{i+1},b_{j+1})$ 的平均值。
- 如果 $m+n$ 是奇数，中位数为 $\max(a_i,b_j)$。

### 答疑

**问**：为什么图中说存在一个位置，满足 $a_i\le b_{j+1}$ 且 $a_{i+1} > b_j$？

**答**：根据 $i$ 和 $j$ 的关系，$i$ 变大，$j$ 会随着变小。把 $b$ 反转，变成一个递减数组，这样 $j$ 会随着 $i$ 的变大而变大，我们可以更容易地观察出性质。把这两个数组画成折线图，一个递增另一个递减，并且由于我们插入了 $-\infty$ 和 $\infty$，所以二者必然相交。这说明存在一个位置，满足 $a_i\le b_{j+1}$ 且 $a_{i+1} > b_j$。

**问**：保证 $m\le n$ 有什么好处？

**答**：如果 $m>n$，我们没法从 $i=0$ 开始枚举。以 $m=5,n=3$ 为例，$i=0$ 时，$b$ 数组需要有 $4$ 个数在第一组，但 $n=3<4$，无法做到。保证 $m\le n$ 可以让我们从 $i=0$ 开始枚举，写起来更方便。

**问**：如果数组中存在重复元素，上述做法是否正确？

**答**：仍然是正确的，因为只用到了「$a$ 和 $b$ 是有序数组」的条件。

### 写法一

```py [sol-Python3]
class Solution:
    def findMedianSortedArrays(self, a: List[int], b: List[int]) -> float:
        if len(a) > len(b):
            a, b = b, a  # 保证下面的 i 可以从 0 开始枚举

        m, n = len(a), len(b)
        a = [-inf] + a + [inf]
        b = [-inf] + b + [inf]

        # 枚举 nums1 有 i 个数在第一组
        # 那么 nums2 有 j = (m + n + 1) // 2 - i 个数在第一组
        i, j = 0, (m + n + 1) // 2
        while True:
            if a[i] <= b[j + 1] and a[i + 1] > b[j]:  # 写 >= 也可以
                max1 = max(a[i], b[j])  # 第一组的最大值
                min2 = min(a[i + 1], b[j + 1])  # 第二组的最小值
                return max1 if (m + n) % 2 else (max1 + min2) / 2
            i += 1  # 继续枚举
            j -= 1
```

```java [sol-Java]
class Solution {
    public double findMedianSortedArrays(int[] nums1, int[] nums2) {
        if (nums1.length > nums2.length) {
            // 交换 nums1 和 nums2，保证下面的 i 可以从 0 开始枚举
            int[] tmp = nums1;
            nums1 = nums2;
            nums2 = tmp;
        }

        int m = nums1.length;
        int n = nums2.length;
        int[] a = new int[m + 2];
        int[] b = new int[n + 2];
        a[0] = b[0] = Integer.MIN_VALUE; // 最左边插入 -∞
        a[m + 1] = b[n + 1] = Integer.MAX_VALUE; // 最右边插入 ∞
        System.arraycopy(nums1, 0, a, 1, m); // 数组没法直接插入，只能 copy
        System.arraycopy(nums2, 0, b, 1, n);

        // 枚举 nums1 有 i 个数在第一组
        // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
        int i = 0;
        int j = (m + n + 1) / 2;
        while (true) {
            if (a[i] <= b[j + 1] && a[i + 1] > b[j]) { // 写 >= 也可以
                int max1 = Math.max(a[i], b[j]); // 第一组的最大值
                int min2 = Math.min(a[i + 1], b[j + 1]); // 第二组的最小值
                return (m + n) % 2 > 0 ? max1 : (max1 + min2) / 2.0;
            }
            i++; // 继续枚举
            j--;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double findMedianSortedArrays(vector<int>& a, vector<int>& b) {
        if (a.size() > b.size()) {
            swap(a, b); // 保证下面的 i 可以从 0 开始枚举
        }

        int m = a.size(), n = b.size();
        a.insert(a.begin(), INT_MIN); // 最左边插入 -∞
        b.insert(b.begin(), INT_MIN);
        a.push_back(INT_MAX); // 最右边插入 ∞
        b.push_back(INT_MAX);

        // 枚举 nums1 有 i 个数在第一组
        // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
        int i = 0, j = (m + n + 1) / 2;
        while (true) {
            if (a[i] <= b[j + 1] && a[i + 1] > b[j]) { // 写 >= 也可以
                int max1 = max(a[i], b[j]); // 第一组的最大值
                int min2 = min(a[i + 1], b[j + 1]); // 第二组的最小值
                return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
            }
            i++; // 继续枚举
            j--;
        }
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

double findMedianSortedArrays(int* nums1, int m, int* nums2, int n) {
    if (m > n) {
        // 利用递归交换 nums1 和 nums2
        return findMedianSortedArrays(nums2, n, nums1, m);
    }

    int* a = malloc((m + 2) * sizeof(int));
    int* b = malloc((n + 2) * sizeof(int));
    a[0] = b[0] = INT_MIN; // 最左边插入 -∞
    a[m + 1] = b[n + 1] = INT_MAX; // 最右边插入 ∞
    memcpy(a + 1, nums1, m * sizeof(int)); // 数组没法直接插入，只能 copy
    memcpy(b + 1, nums2, n * sizeof(int));

    // 枚举 nums1 有 i 个数在第一组
    // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
    int i = 0, j = (m + n + 1) / 2;
    while (true) {
        if (a[i] <= b[j + 1] && a[i + 1] > b[j]) { // 写 >= 也可以
            int max1 = MAX(a[i], b[j]); // 第一组的最大值
            int min2 = MIN(a[i + 1], b[j + 1]); // 第二组的最小值
            free(a);
            free(b);
            return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
        }
        i++; // 继续枚举
        j--;
    }
}
```

```go [sol-Go]
func findMedianSortedArrays(a, b []int) float64 {
    if len(a) > len(b) {
        a, b = b, a // 保证下面的 i 可以从 0 开始枚举
    }

    m, n := len(a), len(b)
    a = append([]int{math.MinInt}, append(a, math.MaxInt)...)
    b = append([]int{math.MinInt}, append(b, math.MaxInt)...)

    // 枚举 nums1 有 i 个数在第一组
    // 那么 nums2 有 j = (m+n+1)/2 - i 个数在第一组
    i, j := 0, (m+n+1)/2
    for {
        if a[i] <= b[j+1] && a[i+1] > b[j] { // 写 >= 也可以
            max1 := max(a[i], b[j])     // 第一组的最大值
            min2 := min(a[i+1], b[j+1]) // 第二组的最小值
            if (m+n)%2 > 0 {
                return float64(max1)
            }
            return float64(max1+min2) / 2
        }
        i++ // 继续枚举
        j--
    }
}
```

```js [sol-JavaScript]
var findMedianSortedArrays = function(a, b) {
    if (a.length > b.length) {
        [a, b] = [b, a]; // 保证下面的 i 可以从 0 开始枚举
    }

    const m = a.length, n = b.length;
    a = [-Infinity, ...a, Infinity];
    b = [-Infinity, ...b, Infinity];

    // 枚举 nums1 有 i 个数在第一组
    // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
    let i = 0, j = Math.floor((m + n + 1) / 2);
    while (true) {
        if (a[i] <= b[j + 1] && a[i + 1] > b[j]) { // 写 >= 也可以
            const max1 = Math.max(a[i], b[j]); // 第一组的最大值
            const min2 = Math.min(a[i + 1], b[j + 1]); // 第二组的最小值
            return (m + n) % 2 ? max1 : (max1 + min2) / 2;
        }
        i++; // 继续枚举
        j--;
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_median_sorted_arrays(mut a: Vec<i32>, mut b: Vec<i32>) -> f64 {
        if a.len() > b.len() {
            std::mem::swap(&mut a, &mut b); // 保证下面的 i 可以从 0 开始枚举
        }

        let m = a.len();
        let n = b.len();
        a.insert(0, i32::MIN); // 最左边插入 -∞
        b.insert(0, i32::MIN);
        a.push(i32::MAX); // 最右边插入 ∞
        b.push(i32::MAX);

        // 枚举 nums1 有 i 个数在第一组
        // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
        let mut i = 0;
        let mut j = (m + n + 1) / 2;
        loop {
            if a[i] <= b[j + 1] && a[i + 1] > b[j] { // 写 >= 也可以
                let max1 = a[i].max(b[j]); // 第一组的最大值
                let min2 = a[i + 1].min(b[j + 1]); // 第二组的最小值
                return if (m + n) % 2 > 0 { max1 as f64 } else { (max1 + min2) as f64 / 2.0 };
            }
            i += 1; // 继续枚举
            j -= 1;
        }
    }
}
```

### 写法二（小优化）

实际上，$a_i\le b_{j+1}$ 和 $a_{i+1} > b_j$ 这两个条件不需要都判断，我们只需要判断其中一个即可。为什么？且听我说。

由于 $a$ 和 $b$ 是有序的，随着 $i$ 的不断变大，$j$ 的不断变小，$a_{i+1} > b_j$ 会从「不成立」变成「成立」。

把循环条件改成：如果 $a_{i+1} \le b_j$，继续循环；如果 $a_{i+1} > b_j$，退出循环。

退出循环之后，除了可以说明 $a_{i+1} > b_j$ 成立外，还有一个隐含的性质：在退出循环之前的最后一轮循环，我们在比较哪两个数？正好就是 $a_i$ 和 $b_{j+1}$！并且这两个数的大小关系是 $a_i\le b_{j+1}$。

```py [sol-Python3]
class Solution:
    def findMedianSortedArrays(self, a: List[int], b: List[int]) -> float:
        if len(a) > len(b):
            a, b = b, a  # 保证下面的 i 可以从 0 开始枚举

        m, n = len(a), len(b)
        a = [-inf] + a + [inf]
        b = [-inf] + b + [inf]

        # 枚举 nums1 有 i 个数在第一组
        # 那么 nums2 有 j = (m + n + 1) // 2 - i 个数在第一组
        i, j = 0, (m + n + 1) // 2
        while a[i + 1] <= b[j]:
            i += 1  # 继续枚举
            j -= 1

        max1 = max(a[i], b[j])  # 第一组的最大值
        min2 = min(a[i + 1], b[j + 1])  # 第二组的最小值
        return max1 if (m + n) % 2 else (max1 + min2) / 2
```

```java [sol-Java]
class Solution {
    public double findMedianSortedArrays(int[] nums1, int[] nums2) {
        if (nums1.length > nums2.length) {
            // 交换 nums1 和 nums2，保证下面的 i 可以从 0 开始枚举
            int[] tmp = nums1;
            nums1 = nums2;
            nums2 = tmp;
        }

        int m = nums1.length;
        int n = nums2.length;
        int[] a = new int[m + 2];
        int[] b = new int[n + 2];
        a[0] = b[0] = Integer.MIN_VALUE; // 最左边插入 -∞
        a[m + 1] = b[n + 1] = Integer.MAX_VALUE; // 最右边插入 ∞
        System.arraycopy(nums1, 0, a, 1, m); // 数组没法直接插入，只能 copy
        System.arraycopy(nums2, 0, b, 1, n);

        // 枚举 nums1 有 i 个数在第一组
        // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
        int i = 0;
        int j = (m + n + 1) / 2;
        while (a[i + 1] <= b[j]) {
            i++; // 继续枚举
            j--;
        }

        int max1 = Math.max(a[i], b[j]); // 第一组的最大值
        int min2 = Math.min(a[i + 1], b[j + 1]); // 第二组的最小值
        return (m + n) % 2 > 0 ? max1 : (max1 + min2) / 2.0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double findMedianSortedArrays(vector<int>& a, vector<int>& b) {
        if (a.size() > b.size()) {
            swap(a, b); // 保证下面的 i 可以从 0 开始枚举
        }

        int m = a.size(), n = b.size();
        a.insert(a.begin(), INT_MIN); // 最左边插入 -∞
        b.insert(b.begin(), INT_MIN);
        a.push_back(INT_MAX); // 最右边插入 ∞
        b.push_back(INT_MAX);

        // 枚举 nums1 有 i 个数在第一组
        // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
        int i = 0, j = (m + n + 1) / 2;
        while (a[i + 1] <= b[j]) {
            i++; // 继续枚举
            j--;
        }

        int max1 = max(a[i], b[j]); // 第一组的最大值
        int min2 = min(a[i + 1], b[j + 1]); // 第二组的最小值
        return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

double findMedianSortedArrays(int* nums1, int m, int* nums2, int n) {
    if (m > n) {
        // 利用递归交换 nums1 和 nums2
        return findMedianSortedArrays(nums2, n, nums1, m);
    }

    int* a = malloc((m + 2) * sizeof(int));
    int* b = malloc((n + 2) * sizeof(int));
    a[0] = b[0] = INT_MIN; // 最左边插入 -∞
    a[m + 1] = b[n + 1] = INT_MAX; // 最右边插入 ∞
    memcpy(a + 1, nums1, m * sizeof(int)); // 数组没法直接插入，只能 copy
    memcpy(b + 1, nums2, n * sizeof(int));

    // 枚举 nums1 有 i 个数在第一组
    // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
    int i = 0, j = (m + n + 1) / 2;
    while (a[i + 1] <= b[j]) {
        i++; // 继续枚举
        j--;
    }

    int max1 = MAX(a[i], b[j]); // 第一组的最大值
    int min2 = MIN(a[i + 1], b[j + 1]); // 第二组的最小值
    free(a);
    free(b);
    return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
}
```

```go [sol-Go]
func findMedianSortedArrays(a, b []int) float64 {
    if len(a) > len(b) {
        a, b = b, a // 保证下面的 i 可以从 0 开始枚举
    }

    m, n := len(a), len(b)
    a = append([]int{math.MinInt}, append(a, math.MaxInt)...)
    b = append([]int{math.MinInt}, append(b, math.MaxInt)...)

    // 枚举 nums1 有 i 个数在第一组
    // 那么 nums2 有 j = (m+n+1)/2 - i 个数在第一组
    i, j := 0, (m+n+1)/2
    for a[i+1] <= b[j] {
        i++ // 继续枚举
        j--
    }

    max1 := max(a[i], b[j])     // 第一组的最大值
    min2 := min(a[i+1], b[j+1]) // 第二组的最小值
    if (m+n)%2 > 0 {
        return float64(max1)
    }
    return float64(max1+min2) / 2
}
```

```js [sol-JavaScript]
var findMedianSortedArrays = function(a, b) {
    if (a.length > b.length) {
        [a, b] = [b, a]; // 保证下面的 i 可以从 0 开始枚举
    }

    const m = a.length, n = b.length;
    a = [-Infinity, ...a, Infinity];
    b = [-Infinity, ...b, Infinity];

    // 枚举 nums1 有 i 个数在第一组
    // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
    let i = 0, j = Math.floor((m + n + 1) / 2);
    while (a[i + 1] <= b[j]) {
        i++; // 继续枚举
        j--;
    }

    const max1 = Math.max(a[i], b[j]); // 第一组的最大值
    const min2 = Math.min(a[i + 1], b[j + 1]); // 第二组的最小值
    return (m + n) % 2 ? max1 : (max1 + min2) / 2;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_median_sorted_arrays(mut a: Vec<i32>, mut b: Vec<i32>) -> f64 {
        if a.len() > b.len() {
            std::mem::swap(&mut a, &mut b); // 保证下面的 i 可以从 0 开始枚举
        }

        let m = a.len();
        let n = b.len();
        a.insert(0, i32::MIN); // 最左边插入 -∞
        b.insert(0, i32::MIN);
        a.push(i32::MAX); // 最右边插入 ∞
        b.push(i32::MAX);

        // 枚举 nums1 有 i 个数在第一组
        // 那么 nums2 有 j = (m + n + 1) / 2 - i 个数在第一组
        let mut i = 0;
        let mut j = (m + n + 1) / 2;
        while a[i + 1] <= b[j] {
            i += 1; // 继续枚举
            j -= 1;
        }

        let max1 = a[i].max(b[j]); // 第一组的最大值
        let min2 = a[i + 1].min(b[j + 1]); // 第二组的最小值
        if (m + n) % 2 > 0 { max1 as f64 } else { (max1 + min2) as f64 / 2.0 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n)$，其中 $m$ 是 $a$ 的长度，$n$ 是 $b$ 的长度。往 $a$ 前面插入一个元素的时间复杂度是 $\mathcal{O}(m)$，往 $b$ 前面插入一个元素的时间复杂度是 $\mathcal{O}(n)$，加起来是 $\mathcal{O}(m+n)$。
- 空间复杂度：$\mathcal{O}(m+n)$。

## 三、优化：二分做法

由于 $a$ 和 $b$ 是有序数组，$i$ 越小，$a_i\le b_{j+1}$ 越能成立；$i$ 越大，$a_i\le b_{j+1}$ 越不能成立。

所以可以二分**最大的**满足 $a_i\le b_{j+1}$ 的 $i$。二分结束后，我们有 $a_i\le b_{j+1}$ 且 $a_{i+1} > b_j$。

关于二分的原理，请看视频[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

最后，讨论二分的上下界。**本文用开区间二分，其他二分写法也是可以的。**

- 开区间二分左边界：$0$。在插入 $-\infty$ 后，$a_i\le b_{j+1}$ 在 $i=0$ 时一定成立。
- 开区间二分右边界：$m+1$。在插入 $\infty$ 后，$a_i\le b_{j+1}$ 在 $i=m+1$ 时一定不成立。

### 答疑

**问**：能否二分红色折线图的最小值？

**答**：这种做法会在有重复元素时失效。试想一下，如果我们在折线图上二分，碰巧遇到了相邻且相同的元素，你要更新 $\textit{left}$ 还是更新 $\textit{right}$ 呢？

**问**：为什么上面的双指针写法二，循环判断的是 $a_{i+1} \le b_j$，这里却变成了 $a_{i} \le b_{j+1}$？

**答**：本质是一样的。上面的双指针写法，也可以写成 $a_{i} \le b_{j+1}$，但需要在循环结束后把 $i$ 减一，把 $j$ 加一。对比来看，上面的双指针写法，循环结束后立刻得到了我们想要的 $i$，而写成 $a_{i} \le b_{j+1}$ 需要额外做个加减一的微调。下面的二分写法，根据循环不变量的定义，写成 $a_{i} \le b_{j+1}$，循环结束后的 $\textit{left}$ 就是我们想要的 $i$。

### 写法一

注意在数组前面插入元素的时间复杂度是线性的，所以和上面的复杂度分析一样，都是 $\mathcal{O}(n+m)$。

真正满足题目时间复杂度要求的是后面的写法二。

```py [sol-Python3]
class Solution:
    def findMedianSortedArrays(self, a: List[int], b: List[int]) -> float:
        if len(a) > len(b):
            a, b = b, a

        m, n = len(a), len(b)
        a = [-inf] + a + [inf]
        b = [-inf] + b + [inf]

        # 循环不变量：a[left] <= b[j+1]
        # 循环不变量：a[right] > b[j+1]
        left, right = 0, m + 1
        while left + 1 < right:  # 开区间 (left, right) 不为空
            i = (left + right) // 2
            j = (m + n + 1) // 2 - i
            if a[i] <= b[j + 1]:
                left = i  # 缩小二分区间为 (i, right)
            else:
                right = i  # 缩小二分区间为 (left, i)

        # 此时 left 等于 right-1
        # a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        i = left
        j = (m + n + 1) // 2 - i
        max1 = max(a[i], b[j])
        min2 = min(a[i + 1], b[j + 1])
        return max1 if (m + n) % 2 else (max1 + min2) / 2
```

```java [sol-Java]
class Solution {
    public double findMedianSortedArrays(int[] nums1, int[] nums2) {
        if (nums1.length > nums2.length) {
            // 交换 nums1 和 nums2，保证下面的 i 可以从 0 开始枚举
            int[] tmp = nums1;
            nums1 = nums2;
            nums2 = tmp;
        }

        int m = nums1.length;
        int n = nums2.length;
        int[] a = new int[m + 2];
        int[] b = new int[n + 2];
        a[0] = b[0] = Integer.MIN_VALUE;
        a[m + 1] = b[n + 1] = Integer.MAX_VALUE;
        System.arraycopy(nums1, 0, a, 1, m);
        System.arraycopy(nums2, 0, b, 1, n);

        // 循环不变量：a[left] <= b[j+1]
        // 循环不变量：a[right] > b[j+1]
        int left = 0;
        int right = m + 1;
        while (left + 1 < right) { // 开区间 (left, right) 不为空
            int i = left + (right - left) / 2;
            int j = (m + n + 1) / 2 - i;
            if (a[i] <= b[j + 1]) {
                left = i; // 缩小二分区间为 (i, right)
            } else {
                right = i; // 缩小二分区间为 (left, i)
            }
        }

        // 此时 left 等于 right-1
        // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        int i = left;
        int j = (m + n + 1) / 2 - i;
        int max1 = Math.max(a[i], b[j]);
        int min2 = Math.min(a[i + 1], b[j + 1]);
        return (m + n) % 2 > 0 ? max1 : (max1 + min2) / 2.0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double findMedianSortedArrays(vector<int>& a, vector<int>& b) {
        if (a.size() > b.size()) {
            swap(a, b); // 保证下面的 i 可以从 0 开始枚举
        }

        int m = a.size(), n = b.size();
        a.insert(a.begin(), INT_MIN);
        b.insert(b.begin(), INT_MIN);
        a.push_back(INT_MAX);
        b.push_back(INT_MAX);

        // 循环不变量：a[left] <= b[j+1]
        // 循环不变量：a[right] > b[j+1]
        int left = 0, right = m + 1;
        while (left + 1 < right) { // 开区间 (left, right) 不为空
            int i = left + (right - left) / 2;
            int j = (m + n + 1) / 2 - i;
            if (a[i] <= b[j + 1]) {
                left = i; // 缩小二分区间为 (i, right)
            } else {
                right = i; // 缩小二分区间为 (left, i)
            }
        }

        // 此时 left 等于 right-1
        // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        int i = left;
        int j = (m + n + 1) / 2 - i;
        int max1 = max(a[i], b[j]);
        int min2 = min(a[i + 1], b[j + 1]);
        return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

double findMedianSortedArrays(int* nums1, int m, int* nums2, int n) {
    if (m > n) {
        // 利用递归交换 nums1 和 nums2
        return findMedianSortedArrays(nums2, n, nums1, m);
    }

    int* a = malloc((m + 2) * sizeof(int));
    int* b = malloc((n + 2) * sizeof(int));
    a[0] = b[0] = INT_MIN;
    a[m + 1] = b[n + 1] = INT_MAX;
    memcpy(a + 1, nums1, m * sizeof(int));
    memcpy(b + 1, nums2, n * sizeof(int));

    // 循环不变量：a[left] <= b[j+1]
    // 循环不变量：a[right] > b[j+1]
    int left = 0, right = m + 1;
    while (left + 1 < right) { // 开区间 (left, right) 不为空
        int i = left + (right - left) / 2;
        int j = (m + n + 1) / 2 - i;
        if (a[i] <= b[j + 1]) {
            left = i; // 缩小二分区间为 (i, right)
        } else {
            right = i; // 缩小二分区间为 (left, i)
        }
    }

    // 此时 left 等于 right-1
    // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
    int i = left;
    int j = (m + n + 1) / 2 - i;
    int max1 = MAX(a[i], b[j]);
    int min2 = MIN(a[i + 1], b[j + 1]);
    free(a);
    free(b);
    return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
}
```

```go [sol-Go]
func findMedianSortedArrays(a, b []int) float64 {
    if len(a) > len(b) {
        a, b = b, a // 保证下面的 i 可以从 0 开始枚举
    }

    m, n := len(a), len(b)
    a = append([]int{math.MinInt}, append(a, math.MaxInt)...)
    b = append([]int{math.MinInt}, append(b, math.MaxInt)...)

    // 循环不变量：a[left] <= b[j+1]
    // 循环不变量：a[right] > b[j+1]
    left, right := 0, m+1
    for left+1 < right { // 开区间 (left, right) 不为空
        i := left + (right-left)/2
        j := (m+n+1)/2 - i
        if a[i] <= b[j+1] {
            left = i // 缩小二分区间为 (i, right)
        } else {
            right = i // 缩小二分区间为 (left, i)
        }
    }

    // 此时 left 等于 right-1
    // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
    i := left
    j := (m+n+1)/2 - i
    max1 := max(a[i], b[j])
    min2 := min(a[i+1], b[j+1])
    if (m+n)%2 > 0 {
        return float64(max1)
    }
    return float64(max1+min2) / 2
}
```

```js [sol-JavaScript]
var findMedianSortedArrays = function(a, b) {
    if (a.length > b.length) {
        [a, b] = [b, a];
    }

    const m = a.length, n = b.length;
    a = [-Infinity, ...a, Infinity];
    b = [-Infinity, ...b, Infinity];

    // 循环不变量：a[left] <= b[j+1]
    // 循环不变量：a[right] > b[j+1]
    let left = 0, right = m + 1;
    while (left + 1 < right) { // 开区间 (left, right) 不为空
        const i = Math.floor((left + right) / 2);
        const j = Math.floor((m + n + 1) / 2) - i;
        if (a[i] <= b[j + 1]) {
            left = i; // 缩小二分区间为 (i, right)
        } else {
            right = i; // 缩小二分区间为 (left, i)
        }
    }

    // 此时 left 等于 right-1
    // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
    const i = left;
    const j = Math.floor((m + n + 1) / 2) - i;
    const max1 = Math.max(a[i], b[j]);
    const min2 = Math.min(a[i + 1], b[j + 1]);
    return (m + n) % 2 ? max1 : (max1 + min2) / 2;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_median_sorted_arrays(mut a: Vec<i32>, mut b: Vec<i32>) -> f64 {
        if a.len() > b.len() {
            std::mem::swap(&mut a, &mut b); // 保证下面的 i 可以从 0 开始枚举
        }

        let m = a.len();
        let n = b.len();
        a.insert(0, i32::MIN);
        b.insert(0, i32::MIN);
        a.push(i32::MAX);
        b.push(i32::MAX);

        // 循环不变量：a[left] <= b[j+1]
        // 循环不变量：a[right] > b[j+1]
        let mut left = 0;
        let mut right = m + 1;
        while left + 1 < right { // 开区间 (left, right) 不为空
            let i = left + (right - left) / 2;
            let j = (m + n + 1) / 2 - i;
            if a[i] <= b[j + 1] {
                left = i; // 缩小二分区间为 (i, right)
            } else {
                right = i; // 缩小二分区间为 (left, i)
            }
        }

        // 此时 left 等于 right-1
        // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        let i = left;
        let j = (m + n + 1) / 2 - i;
        let max1 = a[i].max(b[j]);
        let min2 = a[i + 1].min(b[j + 1]);
        if (m + n) % 2 > 0 {
            max1 as f64
        } else {
            (max1 + min2) as f64 / 2.0
        }
    }
}
```

### 写法二（最终版本）

去掉插入的 $-\infty$ 和 $\infty$，$a$ 和 $b$ 的下标都减一。

如此修改后，$i$ 的含义变成了 $a$ 有 $i+1$ 个数在第一组，$j$ 的含义变成了 $b$ 有 $j+1$ 个数在第一组。

前文的关系式

$$
j = \left\lfloor\dfrac{m+n+1}{2}\right\rfloor - i
$$

修改成

$$
j + 1 = \left\lfloor\dfrac{m+n+1}{2}\right\rfloor - (i+1)
$$

即

$$
j = \left\lfloor\dfrac{m+n+1}{2}\right\rfloor - i - 2 = \left\lfloor\dfrac{m+n-3}{2}\right\rfloor-i
$$

开区间二分的左右边界改成 $-1$ 和 $m$。

### 答疑

**问**：当 $m=0$ 时，是否会算出 $i=0$？

**答**：不会，$m=0$ 不会进入二分循环，$i=\textit{left}=-1$。

```py [sol-Python3]
class Solution:
    def findMedianSortedArrays(self, a: List[int], b: List[int]) -> float:
        if len(a) > len(b):
            a, b = b, a

        m, n = len(a), len(b)
        # 循环不变量：a[left] <= b[j+1]
        # 循环不变量：a[right] > b[j+1]
        left, right = -1, m
        while left + 1 < right:  # 开区间 (left, right) 不为空
            i = (left + right) // 2
            j = (m + n - 3) // 2 - i
            if a[i] <= b[j + 1]:
                left = i  # 缩小二分区间为 (i, right)
            else:
                right = i  # 缩小二分区间为 (left, i)

        # 此时 left 等于 right-1
        # a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        i = left
        j = (m + n - 3) // 2 - i
        ai = a[i] if i >= 0 else -inf
        bj = b[j] if j >= 0 else -inf
        ai1 = a[i + 1] if i + 1 < m else inf
        bj1 = b[j + 1] if j + 1 < n else inf
        max1 = max(ai, bj)
        min2 = min(ai1, bj1)
        return max1 if (m + n) % 2 else (max1 + min2) / 2
```

```py [sol-Python3 库函数]
class Solution:
    def findMedianSortedArrays(self, a: List[int], b: List[int]) -> float:
        if len(a) > len(b):
            a, b = b, a

        m, n = len(a), len(b)
        # 注意 range(m) 是 O(1) 的，不是 O(m)
        i = bisect_left(range(m), True, key=lambda i: a[i] > b[(m + n - 1) // 2 - i]) - 1

        j = (m + n - 3) // 2 - i
        ai = a[i] if i >= 0 else -inf
        bj = b[j] if j >= 0 else -inf
        ai1 = a[i + 1] if i + 1 < m else inf
        bj1 = b[j + 1] if j + 1 < n else inf
        max1 = max(ai, bj)
        min2 = min(ai1, bj1)
        return max1 if (m + n) % 2 else (max1 + min2) / 2
```

```java [sol-Java]
class Solution {
    public double findMedianSortedArrays(int[] a, int[] b) {
        if (a.length > b.length) {
            // 交换 a 和 b
            int[] tmp = a;
            a = b;
            b = tmp;
        }

        int m = a.length;
        int n = b.length;
        // 循环不变量：a[left] <= b[j+1]
        // 循环不变量：a[right] > b[j+1]
        int left = -1;
        int right = m;
        while (left + 1 < right) { // 开区间 (left, right) 不为空
            int i = left + (right - left) / 2;
            int j = (m + n + 1) / 2 - i - 2;
            if (a[i] <= b[j + 1]) {
                left = i; // 缩小二分区间为 (i, right)
            } else {
                right = i; // 缩小二分区间为 (left, i)
            }
        }

        // 此时 left 等于 right-1
        // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        int i = left;
        int j = (m + n + 1) / 2 - i - 2;
        int ai = i >= 0 ? a[i] : Integer.MIN_VALUE;
        int bj = j >= 0 ? b[j] : Integer.MIN_VALUE;
        int ai1 = i + 1 < m ? a[i + 1] : Integer.MAX_VALUE;
        int bj1 = j + 1 < n ? b[j + 1] : Integer.MAX_VALUE;
        int max1 = Math.max(ai, bj);
        int min2 = Math.min(ai1, bj1);
        return (m + n) % 2 > 0 ? max1 : (max1 + min2) / 2.0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double findMedianSortedArrays(vector<int>& a, vector<int>& b) {
        if (a.size() > b.size()) {
            swap(a, b);
        }

        int m = a.size(), n = b.size();
        // 循环不变量：a[left] <= b[j+1]
        // 循环不变量：a[right] > b[j+1]
        int left = -1, right = m;
        while (left + 1 < right) { // 开区间 (left, right) 不为空
            int i = left + (right - left) / 2;
            int j = (m + n + 1) / 2 - i - 2;
            if (a[i] <= b[j + 1]) {
                left = i; // 缩小二分区间为 (i, right)
            } else {
                right = i; // 缩小二分区间为 (left, i)
            }
        }

        // 此时 left 等于 right-1
        // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        int i = left;
        int j = (m + n + 1) / 2 - i - 2;
        int ai = i >= 0 ? a[i] : INT_MIN;
        int bj = j >= 0 ? b[j] : INT_MIN;
        int ai1 = i + 1 < m ? a[i + 1] : INT_MAX;
        int bj1 = j + 1 < n ? b[j + 1] : INT_MAX;
        int max1 = max(ai, bj);
        int min2 = min(ai1, bj1);
        return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

double findMedianSortedArrays(int* a, int m, int* b, int n) {
    if (m > n) {
        // 利用递归交换 a 和 b
        return findMedianSortedArrays(b, n, a, m);
    }

    // 循环不变量：a[left] <= b[j+1]
    // 循环不变量：a[right] > b[j+1]
    int left = -1, right = m;
    while (left + 1 < right) { // 开区间 (left, right) 不为空
        int i = left + (right - left) / 2;
        int j = (m + n + 1) / 2 - i - 2;
        if (a[i] <= b[j + 1]) {
            left = i; // 缩小二分区间为 (i, right)
        } else {
            right = i; // 缩小二分区间为 (left, i)
        }
    }

    // 此时 left 等于 right-1
    // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
    int i = left;
    int j = (m + n + 1) / 2 - i - 2;
    int ai = i >= 0 ? a[i] : INT_MIN;
    int bj = j >= 0 ? b[j] : INT_MIN;
    int ai1 = i + 1 < m ? a[i + 1] : INT_MAX;
    int bj1 = j + 1 < n ? b[j + 1] : INT_MAX;
    int max1 = MAX(ai, bj);
    int min2 = MIN(ai1, bj1);
    return (m + n) % 2 ? max1 : (max1 + min2) / 2.0;
}
```

```go [sol-Go]
func findMedianSortedArrays(a, b []int) float64 {
    if len(a) > len(b) {
        a, b = b, a
    }

    m, n := len(a), len(b)
    // 循环不变量：a[left] <= b[j+1]
    // 循环不变量：a[right] > b[j+1]
    left, right := -1, m
    for left+1 < right { // 开区间 (left, right) 不为空
        i := left + (right-left)/2
        j := (m+n+1)/2 - i - 2
        if a[i] <= b[j+1] {
            left = i // 缩小二分区间为 (i, right)
        } else {
            right = i // 缩小二分区间为 (left, i)
        }
    }

    // 此时 left 等于 right-1
    // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
    i := left
    j := (m+n+1)/2 - i - 2
    ai := math.MinInt; if i >= 0 { ai = a[i] }
    bj := math.MinInt; if j >= 0 { bj = b[j] }
    ai1 := math.MaxInt; if i+1 < m { ai1 = a[i+1] }
    bj1 := math.MaxInt; if j+1 < n { bj1 = b[j+1] }
    max1 := max(ai, bj)
    min2 := min(ai1, bj1)
    if (m+n)%2 > 0 {
        return float64(max1)
    }
    return float64(max1+min2) / 2
}
```

```go [sol-Go 库函数]
func findMedianSortedArrays(a, b []int) float64 {
    if len(a) > len(b) {
        a, b = b, a
    }

    m, n := len(a), len(b)
    i := sort.Search(m, func(i int) bool {
        j := (m+n+1)/2 - i - 2
        return a[i] > b[j+1]
    }) - 1

    j := (m+n+1)/2 - i - 2
    ai := math.MinInt; if i >= 0 { ai = a[i] }
    bj := math.MinInt; if j >= 0 { bj = b[j] }
    ai1 := math.MaxInt; if i+1 < m { ai1 = a[i+1] }
    bj1 := math.MaxInt; if j+1 < n { bj1 = b[j+1] }
    max1 := max(ai, bj)
    min2 := min(ai1, bj1)
    if (m+n)%2 > 0 {
        return float64(max1)
    }
    return float64(max1+min2) / 2
}
```

```js [sol-JS]
var findMedianSortedArrays = function(a, b) {
    if (a.length > b.length) {
        [a, b] = [b, a];
    }

    const m = a.length, n = b.length;
    // 循环不变量：a[left] <= b[j+1]
    // 循环不变量：a[right] > b[j+1]
    let left = -1, right = m;
    while (left + 1 < right) { // 开区间 (left, right) 不为空
        const i = Math.floor((left + right) / 2);
        const j = Math.floor((m + n - 3) / 2) - i;
        if (a[i] <= b[j + 1]) {
            left = i; // 缩小二分区间为 (i, right)
        } else {
            right = i; // 缩小二分区间为 (left, i)
        }
    }

    // 此时 left 等于 right-1
    // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
    const i = left;
    const j = Math.floor((m + n - 3) / 2) - i;
    const ai = i >= 0 ? a[i] : -Infinity;
    const bj = j >= 0 ? b[j] : -Infinity;
    const ai1 = i + 1 < m ? a[i + 1] : Infinity;
    const bj1 = j + 1 < n ? b[j + 1] : Infinity;
    const max1 = Math.max(ai, bj);
    const min2 = Math.min(ai1, bj1);
    return (m + n) % 2 ? max1 : (max1 + min2) / 2;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_median_sorted_arrays(a: Vec<i32>, b: Vec<i32>) -> f64 {
        if a.len() > b.len() {
            return Self::find_median_sorted_arrays(b, a);
        }

        let m = a.len();
        let n = b.len();
        // 循环不变量：a[left] <= b[j+1]
        // 循环不变量：a[right] > b[j+1]
        let mut left = -1;
        let mut right = m as i32;
        while left + 1 < right { // 开区间 (left, right) 不为空
            let i = (left + right) as usize / 2;
            let j = (m + n - 1) / 2 - i; // 注意 j 已经加过 1 了
            if a[i] <= b[j] {
                left = i as i32; // 缩小二分区间为 (i, right)
            } else {
                right = i as i32; // 缩小二分区间为 (left, i)
            }
        }

        // 此时 left 等于 right-1
        // a[left] <= b[j+1] 且 a[right] > b[(j-1)+1] = b[j]，所以答案是 i=left
        let i = left;
        let j = (m + n + 1) as i32 / 2 - i - 2;
        let ai = if i >= 0 { a[i as usize] } else { i32::MIN };
        let bj = if j >= 0 { b[j as usize] } else { i32::MIN };
        let ai1 = if i + 1 < m as i32 { a[(i + 1) as usize] } else { i32::MAX };
        let bj1 = if j + 1 < n as i32 { b[(j + 1) as usize] } else { i32::MAX };
        let max1 = ai.max(bj);
        let min2 = ai1.min(bj1);
        if (m + n) % 2 > 0 {
            max1 as f64
        } else {
            (max1 + min2) as f64 / 2.0
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log\min(m,n))$，其中 $m$ 是 $a$ 的长度，$n$ 是 $b$ 的长度。**注**：这个复杂度比题目所要求的 $\mathcal{O}(\log (m+n))$ 更优！
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

改成求两个有序数组的第 $k$ 小，代码要怎么改？

欢迎在评论区分享你的思路/代码。

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
