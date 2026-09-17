# 1187. 使数组严格递增

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/make-array-strictly-increasing/
- 题目 slug：`make-array-strictly-increasing`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：2316
- 外部题解来源：https://leetcode.cn/problems/make-array-strictly-increasing/solutions/2236095/zui-chang-di-zeng-zi-xu-lie-de-bian-xing-jhgg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最长递增子序列的变形：选或不选/枚举选哪个（Python/Java/C++/Go）](https://leetcode.cn/problems/make-array-strictly-increasing/solutions/2236095/zui-chang-di-zeng-zi-xu-lie-de-bian-xing-jhgg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-chang-di-zeng-zi-xu-lie-de-bian-xing-jhgg`
- topic id：`2236095`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 前置知识

1. [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。
2. 本题用到了 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/) 的思路，详见[【基础算法精讲 20】](https://www.bilibili.com/video/BV1Xj411K7oF/)。
3. 本题需要用二分查找优化算法，详见[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

## 初步分析

为方便描述，下文将 $\textit{arr}_1$ 简记为 $a$，$\textit{arr}_2$ 简记为 $b$。

问题等价于从 $a$ 中找到一个最长严格递增子序列 $\textit{lis}$，把不在 $\textit{lis}$ 中的元素替换成 $b$ 中的元素后，$a$ 是严格递增的，求不在 $\textit{lis}$ 中的元素个数的最小值。

对于最长递增子序列问题（或者一般的动态规划问题），通常都可以用「选或不选」和「枚举选哪个」来启发思考。

## 方法一：「选或不选」

例如 $a=[0,4,2,2],b=[1,2,3,4]$，假如 $a[3]=2$ 替换成了 $b[3]=4$，那么对于 $a[2]=2$ 来说:

- 选择不替换，问题变成「把 $[0,4]$ 替换成严格递增数组，且数组的最后一个数小于 $2$，所需要的最小操作数」。
- 选择替换，那么应该替换得越大越好，但必须小于 $4$（因为 $a[3]$ 替换成了 $4$），那么替换成 $3$ 最佳，问题变成「把 $[0,4]$ 替换成严格递增数组，且最后一个数小于 $3$，所需要的最小操作数」。

所以子问题为：把从 $a[0]$ 到 $a[i]$ 的这段前缀替换成严格递增数组，且数组的最后一个数小于 $\textit{pre}$，所需要的最小操作数。记为 $\textit{dfs}(i,\textit{pre})$。

分类讨论：

- 如果 $a[i]<\textit{pre}$，那么可以不替换 $a[i]$，此时 $\textit{dfs}(i,\textit{pre})=\textit{dfs}(i-1,a[i])$。
- 如果 $b$ 中有比 $\textit{pre}$ 小的数，那么选其中最大的 $b[k]$，去替换 $a[i]$，此时 $\textit{dfs}(i,\textit{pre})=\textit{dfs}(i-1,b[k])+1$。
- 其余情况，$\textit{dfs}(i,\textit{pre})=\infty$，表示无法做到。
- 所有情况取最小值，即为 $\textit{dfs}(i,\textit{pre})$ 的答案。

递归边界：$\textit{dfs}(-1,\textit{pre})=0$。数组为空，无需任何操作。

递归入口：$\textit{dfs}(n-1,\infty)$。为简化代码逻辑，假设 $a[n-1]$ 右侧还有一个无穷大的数。如果 $\textit{dfs}(n-1,\infty)=\infty$，则返回 $-1$，否则返回 $\textit{dfs}(n-1,\infty)$。

和最长递增子序列一样，存在重叠子问题，代码实现时可以用记忆化搜索优化。考虑到 $\textit{pre}$ 的范围比较大，可以用哈希表来记忆化。

> 注：如果把数组元素都映射到一个比较小的范围，就可以避免使用哈希表了。这一技巧叫「离散化」。

在 $b$ 中找小于 $\textit{pre}$ 的最大的数，可以对 $b$ 排序，然后二分查找。为了方便使用库函数二分，转换成二分查找 $\ge\textit{pre}$ 的最小的数的前一个数。

```py [sol-Python3]
class Solution:
    def makeArrayIncreasing(self, a: List[int], b: List[int]) -> int:
        b.sort()  # 为能二分查找，对 b 排序

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, pre: int) -> int:
            if i < 0:
                return 0
            res = dfs(i - 1, a[i]) if a[i] < pre else inf  # 不替换 a[i]
            k = bisect_left(b, pre) - 1  # 二分查找 b 中小于 pre 的最大数的下标
            if k >= 0:  # a[i] 替换成小于 pre 的最大数
                res = min(res, dfs(i - 1, b[k]) + 1)
            return res

        ans = dfs(len(a) - 1, inf)  # 假设 a[n-1] 右侧有个无穷大的数
        return ans if ans < inf else -1
```

```java [sol-Java]
class Solution {
    private int[] a, b;
    private Map<Integer, Integer> memo[];

    public int makeArrayIncreasing(int[] a, int[] b) {
        this.a = a;
        this.b = b;
        Arrays.sort(b); // 为能二分查找，对 b 排序
        int n = a.length;
        memo = new HashMap[n];
        Arrays.setAll(memo, e -> new HashMap<>());
        int ans = dfs(n - 1, Integer.MAX_VALUE); // 假设 a[n-1] 右侧有个无穷大的数
        return ans < Integer.MAX_VALUE / 2 ? ans : -1;
    }

    private int dfs(int i, int pre) {
        if (i < 0) return 0;
        if (memo[i].containsKey(pre))
            return memo[i].get(pre); // 之前计算过了
        // 不替换 a[i]
        int res = a[i] < pre ? dfs(i - 1, a[i]) : Integer.MAX_VALUE / 2;
        // 二分查找 b 中小于 pre 的最大数的下标
        int k = lowerBound(b, pre) - 1;
        if (k >= 0) // a[i] 替换成小于 pre 的最大数
            res = Math.min(res, dfs(i - 1, b[k]) + 1);
        memo[i].put(pre, res); // 记忆化
        return res;
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int target) {
        int left = -1, right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = (left + right) >>> 1;
            if (nums[mid] < target)
                left = mid; // 范围缩小到 (mid, right)
            else
                right = mid; // 范围缩小到 (left, mid)
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int makeArrayIncreasing(vector<int>& a, vector<int>& b) {
        ranges::sort(b); // 为能二分查找，对 b 排序
        int n = a.size();
        unordered_map<int, int> memo[n];

        auto dfs = [&](this auto&&dfs, int i, int pre) -> int {
            if (i < 0) {
                return 0;
            }
            if (auto it = memo[i].find(pre); it != memo[i].end()) {
                return it->second; // 之前计算过了
            }

            // 不替换 a[i]
            int res = a[i] < pre ? dfs(i - 1, a[i]) : INT_MAX / 2;
            // 二分查找 b 中小于 pre 的最大数的下标
            auto k = ranges::lower_bound(b, pre);
            if (k != b.begin()) { // a[i] 替换成小于 pre 的最大数
                res = min(res, dfs(i - 1, *--k) + 1);
            }
            return memo[i][pre] = res;
        };

        int ans = dfs(n - 1, INT_MAX); // 假设 a[n-1] 右侧有个无穷大的数
        return ans < INT_MAX / 2 ? ans : -1;
    }
};
```

```go [sol-Go]
func makeArrayIncreasing(a, b []int) int {
    slices.Sort(b) // 为能二分查找，对 b 排序
    n := len(a)
    memo := make([]map[int]int, n)
    for i := range memo {
        memo[i] = map[int]int{}
    }
    var dfs func(int, int) int
    dfs = func(i, pre int) int {
        if i < 0 {
            return 0
        }
        if v, ok := memo[i][pre]; ok { // 之前计算过了
            return v
        }
        res := math.MaxInt / 2
        if a[i] < pre { // 不替换 a[i]
            res = dfs(i-1, a[i])
        }
        // 二分查找 b 中小于 pre 的最大数的下标
        k := sort.SearchInts(b, pre) - 1
        if k >= 0 { // a[i] 替换成小于 pre 的最大数
            res = min(res, dfs(i-1, b[k])+1)
        }
        memo[i][pre] = res // 记忆化
        return res
    }
    ans := dfs(n-1, math.MaxInt) // 假设 a[n-1] 右侧有个无穷大的数
    if ans < math.MaxInt/2 {
        return ans
    }
    return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm\log m)$，其中 $n$ 为 $a$ 的长度，$m$ 为 $b$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(nm)$（注意 $\textit{pre}$ 如果来自 $a$ 的话只能是 $a[i+1]$），单个状态的计算时间为 $\mathcal{O}(\log m)$，因此时间复杂度为 $\mathcal{O}(nm\log m)$。
- 空间复杂度：$\mathcal{O}(nm)$。即状态个数。

## 方法二：「枚举选哪个」

为了避免使用哈希表（或者离散化），我们来尝试另外一种思路，它只需要 $\mathcal{O}(n)$ 个状态。

在方法二中，我们把重点放在 $\textit{lis}$ 上，关注哪些 $a[i]$ **没有被替换**，那么答案就是 $n-\text{length}(\textit{lis})$。

### 1) 记忆化搜索

仿照最长递增子序列的状态定义，用 $\textit{dfs}(i)$ 表示以 $a[i]$ 结尾的 $\textit{lis}$ 的长度，这里 $a[i]$ **没有被替换**。

枚举 $a[i]$ 左侧最近的没有被替换的元素 $a[j]$，那么必须满足从 $a[j+1]$ 到 $a[i-1]$ 的这段子数组，能被替换成 $b$ 中的元素，且替换后从 $a[j]$ 到 $a[i]$ 是严格递增的。为了保证替换的元素互不相同，需要对 $b$ 去重。

设 $b[k]$ 为 $\ge a[i]$ 的最小元素。

> 注：即使 $b[k]$ 不存在也没关系，下面不会用到这个数。

$a[i-1]$ 最大可以替换成 $b$ 中小于 $a[i]$ 的最大元素，即 $b[k-1]$，然后 $a[i-2]$ 最大可以替换成 $b[k-2]$，……，$a[j+1]$ 最大可以替换成 $b[k-(i-j-1)]$。

> 注：从 $a[j+1]$ 到 $a[i-1]$ 一共有 $i-j-1$ 个数。

所以，只有满足 $b[k-(i-j-1)]>a[j]$，才能完成替换操作。此时更新 $\textit{dfs}(i) = \max(\textit{dfs}(i), \textit{dfs}(j)+1)$。

> 注：要求 $k-(i-j-1) \ge 0$，也就是 $j\ge i-k-1$。

特别地（当 $j=i-1$ 时），只要满足 $a[i-1] < a[i]$，就可以更新 $\textit{dfs}(i) = \max(\textit{dfs}(i), \textit{dfs}(i-1)+1)$。

特别地（当 $j=-1$ 时），如果 $k\ge i$，那么可以把 $a[0]$ 到 $a[i-1]$ 全部替换掉。把这种情况作为 $\textit{dfs}(i)$ 的初始值，即 $1$。如果 $k<i$ 则 $\textit{dfs}(i)$ 初始化为 $-\infty$。

递归边界：$\textit{dfs}(0)=1$，表示 $a[0]$ 没有被替换。注意能递归到 $i=0$，说明替换是可行的。

递归入口：$\textit{dfs}(n)$。为简化代码逻辑，假设 $a[n]=\infty$，这样 $a[n]$ 无需被替换。如果 $\textit{dfs}(n)=-\infty$，则返回 $-1$，否则返回 $n+1-\textit{dfs}(n)$。

和最长递增子序列一样，存在重叠子问题，代码实现时可以用记忆化搜索优化。

```py [sol-Python3]
class Solution:
    def makeArrayIncreasing(self, arr1: List[int], b: List[int]) -> int:
        a = arr1 + [inf]  # 简化代码逻辑
        b = sorted(set(b))  # 去重+排序

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> int:
            # i=0 时不会继续递归，返回 1
            k = bisect_left(b, a[i])
            res = 0 if k >= i else -inf  # 替换 a[i] 左侧全部元素
            if i and a[i - 1] < a[i]:  # 无替换
                res = max(res, dfs(i - 1))
            for j in range(i - 2, max(i - k - 2, -1), -1):
                if b[k - (i - j - 1)] > a[j]:
                    # a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                    res = max(res, dfs(j))
            return res + 1  # 把 +1 移到这里，表示 a[i] 不替换

        ans = dfs(len(a) - 1)  # 注意 a 已经添加了一个元素，len(a)=n+1
        return -1 if ans < 0 else len(a) - ans
```

```java [sol-Java]
class Solution {
    private int[] a, b, memo;
    private int m;

    public int makeArrayIncreasing(int[] a, int[] b) {
        this.a = a;
        this.b = b;
        Arrays.sort(b);
        for (int i = 1; i < b.length; ++i)
            if (b[m] != b[i])
                b[++m] = b[i]; // 原地去重
        ++m;
        int n = a.length;
        memo = new int[n + 1]; // 0 表示还没有计算过
        int ans = dfs(n);
        return ans < 0 ? -1 : n + 1 - ans;
    }

    private int dfs(int i) {
        if (memo[i] != 0) return memo[i]; // 之前计算过了
        int x = i < a.length ? a[i] : Integer.MAX_VALUE;
        int k = lowerBound(b, m, x);
        int res = k < i ? Integer.MIN_VALUE : 0; // 小于 a[i] 的数全部替换
        if (i > 0 && a[i - 1] < x) // 无替换
            res = Math.max(res, dfs(i - 1));
        for (int j = i - 2; j >= i - k - 1 && j >= 0; --j)
            if (b[k - (i - j - 1)] > a[j])
                // a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                res = Math.max(res, dfs(j));
        return memo[i] = ++res; // 把 +1 移到这里，表示 a[i] 不替换
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int right, int target) {
        int left = -1; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = (left + right) >>> 1;
            if (nums[mid] < target)
                left = mid; // 范围缩小到 (mid, right)
            else
                right = mid; // 范围缩小到 (left, mid)
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int makeArrayIncreasing(vector<int>& a, vector<int>& b) {
        a.push_back(INT_MAX); // 简化代码逻辑
        ranges::sort(b);
        b.erase(ranges::unique(b).begin(), b.end()); // 原地去重
        int n = a.size(), memo[n];
        memset(memo, 0, sizeof(memo)); // 0 表示还没有计算过

        function<int(int)> dfs = [&](int i) -> int {
            int &res = memo[i]; // 注意这里是引用，下面会直接修改 memo[i]
            if (res) return res; // 之前计算过了
            int k = ranges::lower_bound(b, a[i]) - b.begin();
            res = k < i ? INT_MIN : 0; // 小于 a[i] 的数全部替换
            if (i && a[i - 1] < a[i]) // 无替换
                res = max(res, dfs(i - 1));
            for (int j = i - 2; j >= i - k - 1 && j >= 0; --j)
                if (b[k - (i - j - 1)] > a[j])
                    // a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                    res = max(res, dfs(j));
            return ++res; // 把 +1 移到这里，表示 a[i] 不替换
        };

        int ans = dfs(n - 1); // 注意 a 已经添加了一个元素
        return ans < 0 ? -1 : n - ans;
    }
};
```

```go [sol-Go]
func makeArrayIncreasing(a, b []int) int {
    a = append(a, math.MaxInt) // 简化代码逻辑
    slices.Sort(b)
    b = slices.Compact(b) // 去重

    n := len(a) // 注意 a 已经添加了一个元素
    memo := make([]int, n) // 0 表示还没有计算过
    var dfs func(int) int
    dfs = func(i int) int {
        p := &memo[i]
        if *p != 0 { // 之前计算过了
            return *p
        }
        k := sort.SearchInts(b, a[i])
        res := 0 // 小于 a[i] 的数全部替换
        if k < i {
            res = math.MinInt
        }
        if i > 0 && a[i-1] < a[i] { // 无替换
            res = max(res, dfs(i-1))
        }
        for j := i - 2; j >= i-k-1 && j >= 0; j-- {
            if b[k-(i-j-1)] > a[j] {
                // a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                res = max(res, dfs(j))
            }
        }
        *p = res + 1 // 把 +1 移到这里，表示 a[i] 不替换
        return *p
    }
    ans := dfs(n - 1) // 注意 a 已经添加了一个元素
    if ans < 0 {
        return -1
    }
    return n - ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot(\log m + \min(n,m)))$，其中 $n$ 为 $a$ 的长度，$m$ 为 $b$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(\log m +\min(n,m))$，因此时间复杂度为 $\mathcal{O}(n\cdot(\log m + \min(n,m)))$。
- 空间复杂度：$\mathcal{O}(n)$。即状态个数。

### 2) 1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

通用做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值（本题无需考虑）。

由于递归中 $i$ 不会为负，所以无需做任何下标的调整，照搬上面的代码就完事了。

> 注：考虑到记忆化搜索中有部分状态没有递归到，可能记忆化搜索实际会更快一些。

```py [sol-Python3]
class Solution:
    def makeArrayIncreasing(self, arr1: List[int], b: List[int]) -> int:
        a = arr1 + [inf]  # 简化代码逻辑
        b = sorted(set(b))  # 去重+排序
        n = len(a)  # 注意 a 已经添加了一个元素
        f = [0] * n  # 这里初始化成任何值都行
        for i, x in enumerate(a):
            k = bisect_left(b, x)
            res = 0 if k >= i else -inf  # 小于 a[i] 的数全部替换
            if i and a[i - 1] < x:  # 无替换
                res = max(res, f[i - 1])
            for j in range(i - 2, max(i - k - 2, -1), -1):
                if b[k - (i - j - 1)] > a[j]:
                    # a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                    res = max(res, f[j])
            f[i] = res + 1  # 把 +1 移到这里，表示 a[i] 不替换
        return -1 if f[-1] < 0 else n - f[-1]  # 注意 a 已经添加了一个元素
```

```java [sol-Java]
class Solution {
    public int makeArrayIncreasing(int[] a, int[] b) {
        Arrays.sort(b);
        int n = a.length, m = 0;
        for (int i = 1; i < b.length; ++i)
            if (b[m] != b[i])
                b[++m] = b[i]; // 原地去重
        ++m;
        var f = new int[n + 1];
        for (int i = 0; i <= n; i++) {
            int x = i < n ? a[i] : Integer.MAX_VALUE;
            int k = lowerBound(b, m, x);
            int res = k < i ? Integer.MIN_VALUE : 0; // 小于 a[i] 的数全部替换
            if (i > 0 && a[i - 1] < x) // 无替换
                res = Math.max(res, f[i - 1]);
            for (int j = i - 2; j >= i - k - 1 && j >= 0; --j)
                if (b[k - (i - j - 1)] > a[j])
                    // a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                    res = Math.max(res, f[j]);
            f[i] = res + 1; // 把 +1 移到这里，表示 a[i] 不替换
        }
        return f[n] < 0 ? -1 : n + 1 - f[n];
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int right, int target) {
        int left = -1; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = (left + right) >>> 1;
            if (nums[mid] < target)
                left = mid; // 范围缩小到 (mid, right)
            else
                right = mid; // 范围缩小到 (left, mid)
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int makeArrayIncreasing(vector<int>& a, vector<int>& b) {
        a.push_back(INT_MAX); // 简化代码逻辑
        ranges::sort(b);
        b.erase(ranges::unique(b).begin(), b.end()); // 原地去重
        int n = a.size(), f[n];
        for (int i = 0; i < n; ++i) {
            int k = ranges::lower_bound(b, a[i]) - b.begin();
            int res = k < i ? INT_MIN : 0; // 小于 a[i] 的数全部替换
            if (i && a[i - 1] < a[i]) // 无替换
                res = max(res, f[i - 1]);
            for (int j = i - 2; j >= i - k - 1 && j >= 0; --j)
                if (b[k - (i - j - 1)] > a[j])
                    // a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                    res = max(res, f[j]);
            f[i] = res + 1; // 把 +1 移到这里，表示 a[i] 不替换
        }
        return f[n - 1] < 0 ? -1 : n - f[n - 1]; // 注意 a 已经添加了一个元素
    }
};
```

```go [sol-Go]
func makeArrayIncreasing(a, b []int) int {
    a = append(a, math.MaxInt) // 简化代码逻辑
    slices.Sort(b)
    b = slices.Compact(b) // 去重

    n := len(a)
    f := make([]int, n)
    for i, x := range a {
        k := sort.SearchInts(b, x)
        res := 0 // 小于 a[i] 的数全部替换
        if k < i {
            res = math.MinInt
        }
        if i > 0 && a[i-1] < x { // 无替换
            res = max(res, f[i-1])
        }
        for j := i - 2; j >= i-k-1 && j >= 0; j-- {
            if b[k-(i-j-1)] > a[j] {
                // a[j+1] 到 a[i-1] 替换成 b[k-(i-j-1)] 到 b[k-1]
                res = max(res, f[j])
            }
        }
        f[i] = res + 1 // 把 +1 移到这里，表示 a[i] 不替换
    }
    if f[n-1] < 0 { // 注意 a 已经添加了一个元素
        return -1
    }
    return n - f[n-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot(\log m + \min(n,m)))$，其中 $n$ 为 $a$ 的长度，$m$ 为 $b$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(\log m +\min(n,m))$，因此时间复杂度为 $\mathcal{O}(n\cdot(\log m + \min(n,m)))$。
- 空间复杂度：$\mathcal{O}(n)$。即状态个数。

## 思考题（变形题）

**问**：假设 $a[i]$ 可以被替换为**任意整数**。那么至少替换多少个数，使得替换后的 $a$ 是严格递增的？

**答**：严格递增等价于对于任意 $i>j$，有

$$
a[i]-a[j]\ge i-j
$$

变形得

$$
a[i]-i\ge a[j]-j
$$

故构造 $b[i]=a[i]-i$，求 $b$ 的最长非递减子序列，再用 $n$ 减去这个子序列的长度，就得到了答案。

> 为什么不能直接求 $a$ 的最长严格递增子序列？试试 $a=[1,2,2,3]$，必须替换 $2$ 个数才能变成严格递增数组。

## 总结

「选或不选」和「枚举选哪个」是思考动态规划问题的基本策略，通常从这两个思路入手就可以找到子问题。

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

本题来自 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
