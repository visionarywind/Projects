# 2597. 美丽子集的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/the-number-of-beautiful-subsets/
- 题目 slug：`the-number-of-beautiful-subsets`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.1 一维 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/the-number-of-beautiful-subsets/solutions/2177818/tao-lu-zi-ji-xing-hui-su-pythonjavacgo-b-fcgs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：回溯 / 动态规划 / O(n) 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/the-number-of-beautiful-subsets/solutions/2177818/tao-lu-zi-ji-xing-hui-su-pythonjavacgo-b-fcgs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tao-lu-zi-ji-xing-hui-su-pythonjavacgo-b-fcgs`
- topic id：`2177818`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：回溯

**前置知识**：[回溯算法套路①子集型回溯【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)，包含两种写法。

在枚举 [78. 子集](https://leetcode.cn/problems/subsets/) 的基础上，额外加个判断。

在选择 $x=\textit{nums}[i]$ 的时候，如果之前选过 $x-k$ 或 $x+k$，则不能选，否则可以选。

代码实现时，可以用哈希表（或者数组）记录选过的数及其出现次数，从而 $\mathcal{O}(1)$ 判断 $x-k$ 和 $x+k$ 是否选过。

### 写法一：输入视角，选或不选

```py [sol-Python3]
class Solution:
    def beautifulSubsets(self, nums: List[int], k: int) -> int:
        ans = -1  # 去掉空集
        cnt = defaultdict(int)

        # nums[i] 选或不选
        def dfs(i: int) -> None:
            if i == len(nums):
                nonlocal ans
                ans += 1
                return
            dfs(i + 1)  # 不选
            x = nums[i]
            if cnt[x - k] == 0 and cnt[x + k] == 0:  # 可以选
                cnt[x] += 1  # 选
                dfs(i + 1)  # 讨论 nums[i+1] 选或不选
                cnt[x] -= 1  # 撤销，恢复现场

        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    public int beautifulSubsets(int[] nums, int k) {
        Map<Integer, Integer> cnt = new HashMap<>();
        dfs(0, nums, k, cnt);
        return ans;
    }

    private int ans = -1; // 去掉空集

    // nums[i] 选或不选
    private void dfs(int i, int[] nums, int k, Map<Integer, Integer> cnt) {
        if (i == nums.length) {
            ans++;
            return;
        }
        dfs(i + 1, nums, k, cnt); // 不选
        int x = nums[i];
        if (cnt.getOrDefault(x - k, 0) == 0 && cnt.getOrDefault(x + k, 0) == 0) { // 可以选
            cnt.merge(x, 1, Integer::sum); // 选
            dfs(i + 1, nums, k, cnt); // 讨论 nums[i+1] 选或不选
            cnt.merge(x, -1, Integer::sum); // 撤销，恢复现场
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int beautifulSubsets(vector<int>& nums, int k) {
        int ans = -1; // 去掉空集
        unordered_map<int, int> cnt;

        // nums[i] 选或不选
        auto dfs = [&](this auto&& dfs, int i) -> void {
            if (i == nums.size()) {
                ans++;
                return;
            }
            dfs(i + 1); // 不选
            int x = nums[i];
            if (cnt[x - k] == 0 && cnt[x + k] == 0) { // 可以选
                cnt[x]++; // 选
                dfs(i + 1); // 讨论 nums[i+1] 选或不选
                cnt[x]--; // 撤销，恢复现场
            }
        };

        dfs(0);
        return ans;
    }
};
```

```go [sol-Go]
func beautifulSubsets(nums []int, k int) int {
    ans := -1 // 去掉空集
    cnt := map[int]int{}

    // nums[i] 选或不选
    var dfs func(int)
    dfs = func(i int) {
        if i == len(nums) {
            ans++
            return
        }
        dfs(i + 1) // 不选
        x := nums[i]
        if cnt[x-k] == 0 && cnt[x+k] == 0 { // 可以选
            cnt[x]++ // 选
            dfs(i + 1) // 讨论 nums[i+1] 选或不选
            cnt[x]-- // 撤销，恢复现场
        }
    }

    dfs(0)
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^n)$，其中 $n$ 为 $\textit{nums}$ 的长度。搜索树是一棵高为 $\mathcal{O}(n)$ 的二叉树，有 $\mathcal{O}(2^n)$ 个节点。
- 空间复杂度：$\mathcal{O}(n)$。

### 写法二：答案视角，枚举选哪个

```py [sol-Python3]
class Solution:
    def beautifulSubsets(self, nums: List[int], k: int) -> int:
        ans = -1  # 去掉空集
        cnt = defaultdict(int)

        # 在 [i, n-1] 中选一个数
        def dfs(i: int) -> None:
            nonlocal ans
            ans += 1
            if i == len(nums):
                return
            for j in range(i, len(nums)):  # 枚举选哪个
                x = nums[j]
                if cnt[x - k] == 0 and cnt[x + k] == 0:  # 可以选
                    cnt[x] += 1  # 选
                    dfs(j + 1)  # 下一个数在 [j+1, n-1] 中选
                    cnt[x] -= 1  # 撤销，恢复现场

        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    public int beautifulSubsets(int[] nums, int k) {
        Map<Integer, Integer> cnt = new HashMap<>();
        dfs(0, nums, k, cnt);
        return ans;
    }

    private int ans = -1; // 去掉空集

    private int dfs(int i, int[] nums, int k, Map<Integer, Integer> cnt) {
        ans++;
        if (i == nums.length) {
            return ans;
        }
        for (int j = i; j < nums.length; j++) { // 枚举选哪个
            int x = nums[j];
            if (cnt.getOrDefault(x - k, 0) == 0 && cnt.getOrDefault(x + k, 0) == 0) { // 可以选
                cnt.merge(x, 1, Integer::sum); // 选
                ans = dfs(j + 1, nums, k, cnt); // 下一个数在 [j+1, n-1] 中选
                cnt.merge(x, -1, Integer::sum); // 撤销，恢复现场
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int beautifulSubsets(vector<int>& nums, int k) {
        int ans = -1; // 去掉空集
        unordered_map<int, int> cnt;

        // 在 [i, n-1] 中选一个数
        auto dfs = [&](this auto&& dfs, int i) -> void {
            ans++;
            if (i == nums.size()) {
                return;
            }
            for (int j = i; j < nums.size(); j++) { // 枚举选哪个
                int x = nums[j];
                if (cnt[x - k] == 0 && cnt[x + k] == 0) { // 可以选
                    cnt[x]++; // 选
                    dfs(j + 1); // 下一个数在 [j+1, n-1] 中选
                    cnt[x]--; // 撤销，恢复现场
                }
            }
        };

        dfs(0);
        return ans;
    }
};
```

```go [sol-Go]
func beautifulSubsets(nums []int, k int) int {
    ans := -1 // 去掉空集
    cnt := map[int]int{}

    var dfs func(int)
    dfs = func(i int) {
        ans++
        if i == len(nums) {
            return
        }
        for j := i; j < len(nums); j++ { // 枚举选哪个
            x := nums[j]
            if cnt[x-k] == 0 && cnt[x+k] == 0 { // 可以选
                cnt[x]++ // 选
                dfs(j + 1) // 下一个数在 [j+1, n-1] 中选
                cnt[x]-- // 撤销，恢复现场
            }
        }
    }

    dfs(0)
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：动态规划

例如 $\textit{nums}=[1,2,3,4,7,8],\ k=2$，我们可以把数组按照模 $k$ 的结果，分成两组：

- $[2,4,8]$，这些数模 $k$ 等于 $0$。
- $[1,3,7]$，这些数模 $k$ 等于 $1$。

从第一组选一些数，从第二组选一些数。第一组中的数字 $x$ 和第二组中的数字 $y$，二者相差一定不等于 $k$（不同余）。

这意味着我们**只需考虑每组内选数字的方案数**，然后根据乘法原理，把各个组的方案数相乘，即为答案。

所以按照模 $k$ 的结果分组，每一组用有序集合（或者哈希表）统计元素及其出现次数。

每一组怎么思考呢？

把有序集合的 key 转成列表 $a$（或者把哈希表的 key 排序）。例如 $a=[1,3,7]$，相邻的数字如果相差恰好为 $k$，那么不能同时选。

这类似 [198. 打家劫舍](https://leetcode.cn/problems/house-robber/)，[视频讲解](https://www.bilibili.com/video/BV1Xj411K7oF/)。

设 $a$ 的长度为 $m$。考虑最大的数 $a[m-1]$ 选或不选：

- 不选 $a[m-1]$，那么问题变成 $m-1$ 个数的子问题。
- 选 $a[m-1]$：
  - 设 $c$ 为 $a[m-1]$ 的出现次数，由于大小为 $c$ 的集合有 $2^c-1$ 个非空子集，所以选至少一个 $a[m-1]$ 的方案数为 $2^c-1$。
  - 如果 $a[m-1]-a[m-2] = k$，那么 $a[m-2]$ 不能选，问题变成 $m-2$ 个数的子问题。
  - 如果 $a[m-1]-a[m-2] \ne k$，那么 $a[m-2]$ 可选可不选，问题变成 $m-1$ 个数的子问题。

类似打家劫舍，定义 $f[i+1]$ 表示在 $a[0]$ 到 $a[i]$ 中选数的方案数：

- 不选 $a[i]$，那么问题变成在 $a[0]$ 到 $a[i-1]$ 中选数的方案数，即 $f[i+1] = f[i]$。
- 选 $a[i]$ 且 $a[i]-a[i-1]=k$，那么问题变成在 $a[0]$ 到 $a[i-2]$ 中选数的方案数，即 $f[i+1] = f[i-1]\cdot (2^{c_i}-1)$。
- 选 $a[i]$ 且 $a[i]-a[i-1]\ne k$，那么问题变成在 $a[0]$ 到 $a[i-1]$ 中选数的方案数，即 $f[i+1] = f[i]\cdot (2^{c_i}-1)$。

其中不选和选互斥，方案数根据加法原理相加。

整理得

$$
f[i+1] =
\begin{cases}
f[i] + f[i-1]\cdot (2^{c_i}-1), & a[i]-a[i-1]=k     \\
f[i]\cdot 2^{c_i}, & a[i]-a[i-1]\ne k    \\
\end{cases}
$$

其中 $c_i$ 为 $a[i]$ 的出现次数。

初始值：

- $f[0]=1$。空集算一个方案。
- $f[1] = 2^{c_0}$。因为 $a[0]$ 左边没有数字，需要单独计算选 $a[0]$ 的方案数，即 $2^{c_0}-1$，加上不选 $a[0]$ 的方案数 $1$，所以 $f[1] = 2^{c_0}$。

这一组的答案：$f[m]$，即在 $a[0]$ 到 $a[m-1]$ 中选数的方案数。

最后，根据乘法原理，把每组的答案相乘，即为答案。但是，虽然每一组都可以不选数，但不能总共一个数都不选，所以要把空集去掉，也就是最终答案要减一。

[本题视频讲解](https://www.bilibili.com/video/BV1EL411C7YU/)。

### 写法一

```py [sol-Python3]
class Solution:
    def beautifulSubsets(self, nums: List[int], k: int) -> int:
        groups = defaultdict(Counter)
        for x in nums:
            # 模 k 同余的数分到同一组，记录元素 x 及其出现次数
            groups[x % k][x] += 1

        ans = 1
        for cnt in groups.values():
            # 计算这一组的方案数
            a = sorted(cnt.items())
            m = len(a)
            f = [0] * (m + 1)
            f[0] = 1
            f[1] = 1 << a[0][1]
            for i in range(1, m):
                if a[i][0] - a[i - 1][0] == k:
                    f[i + 1] = f[i] + f[i - 1] * ((1 << a[i][1]) - 1)
                else:
                    f[i + 1] = f[i] << a[i][1]
            ans *= f[m]  # 每组方案数相乘
        return ans - 1  # 去掉空集
```

```java [sol-Java]
class Solution {
    public int beautifulSubsets(int[] nums, int k) {
        Map<Integer, TreeMap<Integer, Integer>> groups = new HashMap<>();
        for (int x : nums) {
            // 模 k 同余的数分到同一组，记录元素 x 及其出现次数
            groups.computeIfAbsent(x % k, i -> new TreeMap<>()).merge(x, 1, Integer::sum);
        }

        int ans = 1;
        for (TreeMap<Integer, Integer> cnt : groups.values()) {
            // 计算这一组的方案数
            int m = cnt.size();
            int[] f = new int[m + 1];
            f[0] = 1;
            int i = 1;
            int pre = 0;
            for (Map.Entry<Integer, Integer> e : cnt.entrySet()) {
                int x = e.getKey();
                int c = e.getValue();
                if (i > 1 && x - pre == k) {
                    f[i] = f[i - 1] + f[i - 2] * ((1 << c) - 1);
                } else {
                    f[i] = f[i - 1] << c;
                }
                pre = x;
                i++;
            }
            ans *= f[m]; // 每组方案数相乘
        }
        return ans - 1; // 去掉空集
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int beautifulSubsets(vector<int>& nums, int k) {
        unordered_map<int, map<int, int>> groups;
        for (int x : nums) {
            // 模 k 同余的数分到同一组，记录元素 x 及其出现次数
            groups[x % k][x]++;
        }

        int ans = 1;
        for (auto& [_, cnt] : groups) {
            // 计算这一组的方案数
            int m = cnt.size();
            vector<int> f(m + 1);
            auto it = cnt.begin();
            f[0] = 1;
            f[1] = 1 << it++->second;
            for (int i = 1; i < m; i++, it++) {
                auto [x, c] = *it;
                if (x - prev(it)->first == k) {
                    f[i + 1] = f[i] + f[i - 1] * ((1 << c) - 1);
                } else {
                    f[i + 1] = f[i] << c;
                }
            }
            ans *= f[m]; // 每组方案数相乘
        }
        return ans - 1; // 去掉空集
    }
};
```

```go [sol-Go]
func beautifulSubsets(nums []int, k int) int {
    groups := map[int]map[int]int{}
    for _, x := range nums {
        // 模 k 同余的数分到同一组，记录元素 x 及其出现次数
        if groups[x%k] == nil {
            groups[x%k] = map[int]int{}
        }
        groups[x%k][x]++
    }

    ans := 1
    for _, cnt := range groups {
        // 计算这一组的方案数
        a := slices.Sorted(maps.Keys(cnt))
        m := len(a)
        f := make([]int, m+1)
        f[0] = 1
        f[1] = 1 << cnt[a[0]]
        for i := 1; i < m; i++ {
            c := cnt[a[i]]
            if a[i]-a[i-1] == k {
                f[i+1] = f[i] + f[i-1]*(1<<c-1)
            } else {
                f[i+1] = f[i] << c
            }
        }
        ans *= f[m] // 每组方案数相乘
    }
    return ans - 1 // 去掉空集
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序/维护有序集合上。
- 空间复杂度：$\mathcal{O}(n)$。

### 写法二：空间优化

同打家劫舍，用两个变量 $f_0$ 和 $f_1$ 滚动计算。

```py [sol-Python3]
class Solution:
    def beautifulSubsets(self, nums: List[int], k: int) -> int:
        groups = defaultdict(Counter)
        for x in nums:
            # 模 k 同余的数分到同一组，记录元素 x 及其出现次数
            groups[x % k][x] += 1

        ans = 1
        for cnt in groups.values():
            # 计算这一组的方案数
            a = sorted(cnt.items())
            f0, f1 = 1, 1 << a[0][1]
            for (pre, _), (x, c) in pairwise(a):
                if x - pre == k:
                    f0, f1 = f1, f1 + f0 * ((1 << c) - 1)
                else:
                    f0, f1 = f1, f1 << c
            ans *= f1  # 每组方案数相乘
        return ans - 1  # 去掉空集
```

```java [sol-Java]
class Solution {
    public int beautifulSubsets(int[] nums, int k) {
        Map<Integer, TreeMap<Integer, Integer>> groups = new HashMap<>();
        for (int x : nums) {
            // 模 k 同余的数分到同一组，记录元素 x 及其出现次数
            groups.computeIfAbsent(x % k, i -> new TreeMap<>()).merge(x, 1, Integer::sum);
        }

        int ans = 1;
        for (TreeMap<Integer, Integer> cnt : groups.values()) {
            // 计算这一组的方案数
            int f0 = 1;
            int f1 = 1; // 下面第一轮循环无论进入哪个分支，都会算出 f1 = 1 << c0
            int pre = 0; // 可以初始化成任意值
            for (Map.Entry<Integer, Integer> e : cnt.entrySet()) {
                int x = e.getKey();
                int c = e.getValue();
                int newF = x - pre == k ? f1 + f0 * ((1 << c) - 1) : f1 << c;
                f0 = f1;
                f1 = newF;
                pre = x;
            }
            ans *= f1; // 每组方案数相乘
        }
        return ans - 1; // 去掉空集
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int beautifulSubsets(vector<int>& nums, int k) {
        unordered_map<int, map<int, int>> groups;
        for (int x : nums) {
            // 模 k 同余的数分到同一组，记录元素 x 及其出现次数
            groups[x % k][x]++;
        }

        int ans = 1;
        for (auto& [_, cnt] : groups) {
            // 计算这一组的方案数
            auto it = cnt.begin();
            int f0 = 1, f1 = 1 << it->second;
            for (it++; it != cnt.end(); it++) {
                auto [x, c] = *it;
                int new_f = x - prev(it)->first == k ? f1 + f0 * ((1 << c) - 1) : f1 << c;
                f0 = f1;
                f1 = new_f;
            }
            ans *= f1; // 每组方案数相乘
        }
        return ans - 1; // 去掉空集
    }
};
```

```go [sol-Go]
func beautifulSubsets(nums []int, k int) int {
	groups := map[int]map[int]int{}
	for _, x := range nums {
		// 模 k 同余的数分到同一组，记录元素 x 及其出现次数
		if groups[x%k] == nil {
			groups[x%k] = map[int]int{}
		}
		groups[x%k][x]++
	}

	ans := 1
	for _, cnt := range groups {
		// 计算这一组的方案数
		a := slices.Sorted(maps.Keys(cnt))
		f0, f1, newF := 1, 1<<cnt[a[0]], 0
		for i := 1; i < len(a); i++ {
			c := cnt[a[i]]
			if a[i]-a[i-1] == k {
				newF = f1 + f0*(1<<c-1)
			} else {
				newF = f1 << c
			}
			f0 = f1
			f1 = newF
		}
		ans *= f1 // 每组方案数相乘
	}
	return ans - 1 // 去掉空集
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序/维护有序集合上。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法三：动态规划 + 最长连续序列 trick

比如 $k=2$，某一组为 $[1,3,5,9,11,13]$，由于 $5$ 和 $9$ 相差不为 $k$，所以可以进一步拆成两个序列 $[1,3,5]$ 和 $[9,11,13]$。这两个序列互相独立，可以分别计算方案数再相乘。

这两个序列都是公差为 $k$ 的等差数列。利用 [128. 最长连续序列](https://leetcode.cn/problems/longest-consecutive-sequence/)（这题相当于 $k=1$）的技巧，找到等差数列的首项，就能通过不断 $+k$ 找到等差数列中的其他元素。这种做法无需排序，也无需有序集合，从而把方法二的时间复杂度优化到 $\mathcal{O}(n)$，原理见 [我的题解](https://leetcode.cn/problems/longest-consecutive-sequence/solutions/3005726/ha-xi-biao-on-zuo-fa-pythonjavacgojsrust-whop/)。

```py [sol-Python3]
class Solution:
    def beautifulSubsets(self, nums: List[int], k: int) -> int:
        cnt = Counter(nums)
        ans = 1
        for x, c in cnt.items():
            if x - k in cnt:  # x 不是等差数列的首项
                continue
            # 计算这一组的方案数
            f0, f1 = 1, 1 << c
            x += k
            while x in cnt:
                f0, f1 = f1, f1 + f0 * ((1 << cnt[x]) - 1)
                x += k
            ans *= f1  # 每组方案数相乘
        return ans - 1  # 去掉空集
```

```java [sol-Java]
class Solution {
    public int beautifulSubsets(int[] nums, int k) {
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            cnt.merge(x, 1, Integer::sum);
        }

        int ans = 1;
        for (Map.Entry<Integer, Integer> e : cnt.entrySet()) {
            int x = e.getKey();
            if (cnt.containsKey(x - k)) { // x 不是等差数列的首项
                continue;
            }
            // 计算这一组的方案数
            int f0 = 1;
            int f1 = 1 << e.getValue();
            for (x += k; cnt.containsKey(x); x += k) {
                int newF = f1 + f0 * ((1 << cnt.get(x)) - 1);
                f0 = f1;
                f1 = newF;
            }
            ans *= f1; // 每组方案数相乘
        }
        return ans - 1; // 去掉空集
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int beautifulSubsets(vector<int>& nums, int k) {
        unordered_map<int, int> cnt;
        for (int x : nums) {
            cnt[x]++;
        }

        int ans = 1;
        for (auto& [x, c] : cnt) {
            if (cnt.contains(x - k)) { // x 不是等差数列的首项
                continue;
            }
            // 计算这一组的方案数
            int f0 = 1, f1 = 1 << c;
            for (int y = x + k; cnt.contains(y); y += k) {
                int new_f = f1 + f0 * ((1 << cnt[y]) - 1);
                f0 = f1;
                f1 = new_f;
            }
            ans *= f1; // 每组方案数相乘
        }
        return ans - 1; // 去掉空集
    }
};
```

```go [sol-Go]
func beautifulSubsets(nums []int, k int) int {
	cnt := map[int]int{}
	for _, x := range nums {
		cnt[x]++
	}

	ans := 1
	for x, c := range cnt {
		if cnt[x-k] > 0 { // x 不是等差数列的首项
			continue
		}
		// 计算这一组的方案数
		f0, f1 := 1, 1<<c
		for x += k; cnt[x] > 0; x += k {
			f0, f1 = f1, f1+f0*(1<<cnt[x]-1)
		}
		ans *= f1 // 每组方案数相乘
	}
	return ans - 1 // 去掉空集
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。理由见 128 题的复杂度分析。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
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

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.1 一维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.1 一维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
