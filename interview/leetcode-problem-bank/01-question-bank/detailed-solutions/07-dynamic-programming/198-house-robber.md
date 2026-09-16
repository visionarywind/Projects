# 198. 打家劫舍

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/house-robber/
- 题目 slug：`house-robber`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.2 打家劫舍 / 答疑
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/house-robber/solutions/2102725/ru-he-xiang-chu-zhuang-tai-ding-yi-he-zh-1wt1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频讲解】动态规划入门：从记忆化搜索到递推（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/house-robber/solutions/2102725/ru-he-xiang-chu-zhuang-tai-ding-yi-he-zh-1wt1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-xiang-chu-zhuang-tai-ding-yi-he-zh-1wt1`
- topic id：`2102725`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 本题视频讲解

请看 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

#### 答疑

**问**：为什么只需要考虑从左往右（从右往左）偷？我就不能从中间开始偷吗？

**答**：先偷 A 再偷 B，先偷 B 再偷 A，都是一样的，因为我们只关心最终能偷多少钱。推广，任意一种偷房子的顺序，都可以**重新排列**成从左到右（从右往左）偷。

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

```py [sol-Python3]
class Solution:
    def rob(self, nums: List[int]) -> int:
        # dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> int:
            if i < 0:  # 递归边界（没有房子）
                return 0
            return max(dfs(i - 1), dfs(i - 2) + nums[i])

        return dfs(len(nums) - 1)  # 从最后一个房子开始思考
```

```java [sol-Java]
class Solution {
    public int rob(int[] nums) {
        int n = nums.length;
        int[] memo = new int[n];
        Arrays.fill(memo, -1); // -1 表示没有计算过
        return dfs(n - 1, nums, memo); // 从最后一个房子开始思考
    }

    // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
    private int dfs(int i, int[] nums, int[] memo) {
        if (i < 0) { // 递归边界（没有房子）
            return 0;
        }
        if (memo[i] != -1) { // 之前计算过
            return memo[i];
        }
        int notChoose = dfs(i - 1, nums, memo);
        int choose = dfs(i - 2, nums, memo) + nums[i];
        return memo[i] = Math.max(notChoose, choose); // 返回答案并记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        vector<int> memo(n, -1); // -1 表示没有计算过

        // lambda 递归函数
        // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i < 0) { // 递归边界（没有房子）
                return 0;
            }
            if (memo[i] != -1) { // 之前计算过
                return memo[i];
            }
            return memo[i] = max(dfs(i - 1), dfs(i - 2) + nums[i]);
        };

        return dfs(n - 1); // 从最后一个房子开始思考
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int dfs(int i, int* nums, int* memo) {
    if (i < 0) {
        return 0; // 递归边界（没有房子）
    }
    if (memo[i] != -1) {
        return memo[i]; // 之前计算过
    }
    int not_choose = dfs(i - 1, nums, memo);
    int choose = dfs(i - 2, nums, memo) + nums[i];
    return memo[i] = MAX(not_choose, choose);
}

int rob(int* nums, int numsSize) {
    int* memo = malloc(numsSize * sizeof(int));
    memset(memo, -1, numsSize * sizeof(int));
    int ans = dfs(numsSize - 1, nums, memo); // 从最后一个房子开始思考
    free(memo);
    return ans;
}
```

```go [sol-Go]
func rob(nums []int) int {
    n := len(nums)
    memo := make([]int, n)
    for i := range memo {
        memo[i] = -1 // -1 表示没有计算过
    }

    // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
    var dfs func(int) int
    dfs = func(i int) int {
        if i < 0 { // 递归边界（没有房子）
            return 0
        }
        if memo[i] != -1 { // 之前计算过
            return memo[i]
        }
        res := max(dfs(i-1), dfs(i-2)+nums[i])
        memo[i] = res // 记忆化：保存计算结果
        return res
    }

    return dfs(n - 1) // 从最后一个房子开始思考
}
```

```js [sol-JavaScript]
var rob = function(nums) {
    const n = nums.length;
    const memo = Array(n).fill(-1); // -1 表示没有计算过

    // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
    function dfs(i) {
        if (i < 0) { // 递归边界（没有房子）
            return 0;
        }
        if (memo[i] !== -1) { // 之前计算过
            return memo[i];
        }
        const res = Math.max(dfs(i - 1), dfs(i - 2) + nums[i]);
        memo[i] = res; // 记忆化：保存计算结果
        return res;
    }

    return dfs(n - 1); // 从最后一个房子开始思考
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
        fn dfs(i: usize, nums: &[i32], memo: &mut [i32]) -> i32 {
            if i >= nums.len() { // 注意这相当于 i < 0
                return 0; // 递归边界（没有房子）
            }
            if memo[i] != -1 {
                return memo[i]; // 之前计算过
            }
            memo[i] = dfs(i - 1, nums, memo).max(dfs(i - 2, nums, memo) + nums[i]);
            memo[i]
        }

        let n = nums.len();
        let mut memo = vec![-1; n]; // -1 表示没有计算过
        dfs(n - 1, &nums, &mut memo) // 从最后一个房子开始思考
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 二、1:1 翻译成递推

直接翻译的话，$\textit{dfs}(i)$ 翻译成 $f[i]$。

但记忆化搜索会访问 $\textit{dfs}(-2)$ 和 $\textit{dfs}(-1)$，$f[-2]$ 和 $f[-1]$ 下标越界了。

**解决办法**：在 $f$ 数组的前面插入两个 $0$，把 $f$ 数组整体往右偏移 $2$ 位。偏移后，$\textit{dfs}(i)$ 翻译成 $f[i+2]$。

注意只有 $f$ 发生了偏移，$\textit{nums}$ 并没有偏移。

```py [sol-Python3]
class Solution:
    def rob(self, nums: List[int]) -> int:
        f = [0] * (len(nums) + 2)
        for i, x in enumerate(nums):
            f[i + 2] = max(f[i + 1], f[i] + x)
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int rob(int[] nums) {
        int n = nums.length;
        int[] f = new int[n + 2];
        for (int i = 0; i < n; i++) {
            f[i + 2] = Math.max(f[i + 1], f[i] + nums[i]);
        }
        return f[n + 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        vector<int> f(n + 2);
        for (int i = 0; i < n; i++) {
            f[i + 2] = max(f[i + 1], f[i] + nums[i]);
        }
        return f[n + 1];
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int rob(int* nums, int numsSize) {
    int* f = malloc((numsSize + 2) * sizeof(int));
    f[0] = f[1] = 0;
    for (int i = 0; i < numsSize; i++) {
        f[i + 2] = MAX(f[i + 1], f[i] + nums[i]);
    }
    int ans = f[numsSize + 1];
    free(f);
    return ans;
}
```

```go [sol-Go]
func rob(nums []int) int {
    n := len(nums)
    f := make([]int, n+2)
    for i, x := range nums {
        f[i+2] = max(f[i+1], f[i]+x)
    }
    return f[n+1]
}
```

```js [sol-JavaScript]
var rob = function(nums) {
    const n = nums.length;
    const f = Array(n + 2).fill(0);
    for (let i = 0; i < n; i++) {
        f[i + 2] = Math.max(f[i + 1], f[i] + nums[i]);
    }
    return f[n + 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut f = vec![0; n + 2];
        for (i, x) in nums.iter().enumerate() {
            f[i + 2] = f[i + 1].max(f[i] + x);
        }
        f[n + 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 三、空间优化

```py [sol-Python3]
class Solution:
    def rob(self, nums: List[int]) -> int:
        f0 = f1 = 0
        for x in nums:
            f0, f1 = f1, max(f1, f0 + x)
        return f1
```

```java [sol-Java]
class Solution {
    public int rob(int[] nums) {
        int f0 = 0;
        int f1 = 0;
        for (int x : nums) {
            int newF = Math.max(f1, f0 + x);
            f0 = f1;
            f1 = newF;
        }
        return f1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rob(vector<int>& nums) {
        int f0 = 0, f1 = 0;
        for (int x : nums) {
            int new_f = max(f1, f0 + x);
            f0 = f1;
            f1 = new_f;
        }
        return f1;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int rob(int* nums, int numsSize) {
    int f0 = 0, f1 = 0;
    for (int i = 0; i < numsSize; i++) {
        int new_f = MAX(f1, f0 + nums[i]);
        f0 = f1;
        f1 = new_f;
    }
    return f1;
}
```

```go [sol-Go]
func rob(nums []int) int {
    f0, f1 := 0, 0
    for _, x := range nums {
        f0, f1 = f1, max(f1, f0+x)
    }
    return f1
}
```

```js [sol-JavaScript]
var rob = function(nums) {
    let f0 = 0, f1 = 0;
    for (const x of nums) {
        [f0, f1] = [f1, Math.max(f1, f0 + x)]
    }
    return f1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        let mut f0 = 0;
        let mut f1 = 0;
        for x in nums {
            let new_f = f1.max(f0 + x);
            f0 = f1;
            f1 = new_f;
        }
        f1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。其中 $n$ 是 $\textit{nums}$ 的长度。
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

## 本题视频讲解

请看 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

#### 答疑

**问**：为什么只需要考虑从左往右（从右往左）偷？我就不能从中间开始偷吗？

**答**：先偷 A 再偷 B，先偷 B 再偷 A，都是一样的，因为我们只关心最终能偷多少钱。推广，任意一种偷房子的顺序，都可以**重新排列**成从左到右（从右往左）偷。

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

```py [sol-Python3]
class Solution:
    def rob(self, nums: List[int]) -> int:
        # dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> int:
            if i < 0:  # 递归边界（没有房子）
                return 0
            return max(dfs(i - 1), dfs(i - 2) + nums[i])

        return dfs(len(nums) - 1)  # 从最后一个房子开始思考
```

```java [sol-Java]
class Solution {
    public int rob(int[] nums) {
        int n = nums.length;
        int[] memo = new int[n];
        Arrays.fill(memo, -1); // -1 表示没有计算过
        return dfs(n - 1, nums, memo); // 从最后一个房子开始思考
    }

    // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
    private int dfs(int i, int[] nums, int[] memo) {
        if (i < 0) { // 递归边界（没有房子）
            return 0;
        }
        if (memo[i] != -1) { // 之前计算过
            return memo[i];
        }
        int notChoose = dfs(i - 1, nums, memo);
        int choose = dfs(i - 2, nums, memo) + nums[i];
        return memo[i] = Math.max(notChoose, choose); // 返回答案并记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        vector<int> memo(n, -1); // -1 表示没有计算过

        // lambda 递归函数
        // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i < 0) { // 递归边界（没有房子）
                return 0;
            }
            if (memo[i] != -1) { // 之前计算过
                return memo[i];
            }
            return memo[i] = max(dfs(i - 1), dfs(i - 2) + nums[i]);
        };

        return dfs(n - 1); // 从最后一个房子开始思考
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int dfs(int i, int* nums, int* memo) {
    if (i < 0) {
        return 0; // 递归边界（没有房子）
    }
    if (memo[i] != -1) {
        return memo[i]; // 之前计算过
    }
    int not_choose = dfs(i - 1, nums, memo);
    int choose = dfs(i - 2, nums, memo) + nums[i];
    return memo[i] = MAX(not_choose, choose);
}

int rob(int* nums, int numsSize) {
    int* memo = malloc(numsSize * sizeof(int));
    memset(memo, -1, numsSize * sizeof(int));
    int ans = dfs(numsSize - 1, nums, memo); // 从最后一个房子开始思考
    free(memo);
    return ans;
}
```

```go [sol-Go]
func rob(nums []int) int {
    n := len(nums)
    memo := make([]int, n)
    for i := range memo {
        memo[i] = -1 // -1 表示没有计算过
    }

    // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
    var dfs func(int) int
    dfs = func(i int) int {
        if i < 0 { // 递归边界（没有房子）
            return 0
        }
        if memo[i] != -1 { // 之前计算过
            return memo[i]
        }
        res := max(dfs(i-1), dfs(i-2)+nums[i])
        memo[i] = res // 记忆化：保存计算结果
        return res
    }

    return dfs(n - 1) // 从最后一个房子开始思考
}
```

```js [sol-JavaScript]
var rob = function(nums) {
    const n = nums.length;
    const memo = Array(n).fill(-1); // -1 表示没有计算过

    // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
    function dfs(i) {
        if (i < 0) { // 递归边界（没有房子）
            return 0;
        }
        if (memo[i] !== -1) { // 之前计算过
            return memo[i];
        }
        const res = Math.max(dfs(i - 1), dfs(i - 2) + nums[i]);
        memo[i] = res; // 记忆化：保存计算结果
        return res;
    }

    return dfs(n - 1); // 从最后一个房子开始思考
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        // dfs(i) 表示从 nums[0] 到 nums[i] 最多能偷多少
        fn dfs(i: usize, nums: &[i32], memo: &mut [i32]) -> i32 {
            if i >= nums.len() { // 注意这相当于 i < 0
                return 0; // 递归边界（没有房子）
            }
            if memo[i] != -1 {
                return memo[i]; // 之前计算过
            }
            memo[i] = dfs(i - 1, nums, memo).max(dfs(i - 2, nums, memo) + nums[i]);
            memo[i]
        }

        let n = nums.len();
        let mut memo = vec![-1; n]; // -1 表示没有计算过
        dfs(n - 1, &nums, &mut memo) // 从最后一个房子开始思考
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 二、1:1 翻译成递推

直接翻译的话，$\textit{dfs}(i)$ 翻译成 $f[i]$。

但记忆化搜索会访问 $\textit{dfs}(-2)$ 和 $\textit{dfs}(-1)$，$f[-2]$ 和 $f[-1]$ 下标越界了。

**解决办法**：在 $f$ 数组的前面插入两个 $0$，把 $f$ 数组整体往右偏移 $2$ 位。偏移后，$\textit{dfs}(i)$ 翻译成 $f[i+2]$。

注意只有 $f$ 发生了偏移，$\textit{nums}$ 并没有偏移。

```py [sol-Python3]
class Solution:
    def rob(self, nums: List[int]) -> int:
        f = [0] * (len(nums) + 2)
        for i, x in enumerate(nums):
            f[i + 2] = max(f[i + 1], f[i] + x)
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int rob(int[] nums) {
        int n = nums.length;
        int[] f = new int[n + 2];
        for (int i = 0; i < n; i++) {
            f[i + 2] = Math.max(f[i + 1], f[i] + nums[i]);
        }
        return f[n + 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        vector<int> f(n + 2);
        for (int i = 0; i < n; i++) {
            f[i + 2] = max(f[i + 1], f[i] + nums[i]);
        }
        return f[n + 1];
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int rob(int* nums, int numsSize) {
    int* f = malloc((numsSize + 2) * sizeof(int));
    f[0] = f[1] = 0;
    for (int i = 0; i < numsSize; i++) {
        f[i + 2] = MAX(f[i + 1], f[i] + nums[i]);
    }
    int ans = f[numsSize + 1];
    free(f);
    return ans;
}
```

```go [sol-Go]
func rob(nums []int) int {
    n := len(nums)
    f := make([]int, n+2)
    for i, x := range nums {
        f[i+2] = max(f[i+1], f[i]+x)
    }
    return f[n+1]
}
```

```js [sol-JavaScript]
var rob = function(nums) {
    const n = nums.length;
    const f = Array(n + 2).fill(0);
    for (let i = 0; i < n; i++) {
        f[i + 2] = Math.max(f[i + 1], f[i] + nums[i]);
    }
    return f[n + 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut f = vec![0; n + 2];
        for (i, x) in nums.iter().enumerate() {
            f[i + 2] = f[i + 1].max(f[i] + x);
        }
        f[n + 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 三、空间优化

```py [sol-Python3]
class Solution:
    def rob(self, nums: List[int]) -> int:
        f0 = f1 = 0
        for x in nums:
            f0, f1 = f1, max(f1, f0 + x)
        return f1
```

```java [sol-Java]
class Solution {
    public int rob(int[] nums) {
        int f0 = 0;
        int f1 = 0;
        for (int x : nums) {
            int newF = Math.max(f1, f0 + x);
            f0 = f1;
            f1 = newF;
        }
        return f1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rob(vector<int>& nums) {
        int f0 = 0, f1 = 0;
        for (int x : nums) {
            int new_f = max(f1, f0 + x);
            f0 = f1;
            f1 = new_f;
        }
        return f1;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int rob(int* nums, int numsSize) {
    int f0 = 0, f1 = 0;
    for (int i = 0; i < numsSize; i++) {
        int new_f = MAX(f1, f0 + nums[i]);
        f0 = f1;
        f1 = new_f;
    }
    return f1;
}
```

```go [sol-Go]
func rob(nums []int) int {
    f0, f1 := 0, 0
    for _, x := range nums {
        f0, f1 = f1, max(f1, f0+x)
    }
    return f1
}
```

```js [sol-JavaScript]
var rob = function(nums) {
    let f0 = 0, f1 = 0;
    for (const x of nums) {
        [f0, f1] = [f1, Math.max(f1, f0 + x)]
    }
    return f1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rob(nums: Vec<i32>) -> i32 {
        let mut f0 = 0;
        let mut f1 = 0;
        for x in nums {
            let new_f = f1.max(f0 + x);
            f0 = f1;
            f1 = new_f;
        }
        f1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。其中 $n$ 是 $\textit{nums}$ 的长度。
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

本题来自 `一、入门 DP / §1.2 打家劫舍 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.2 打家劫舍 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
