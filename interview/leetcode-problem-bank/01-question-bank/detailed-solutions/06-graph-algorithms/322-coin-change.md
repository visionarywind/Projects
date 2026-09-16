# 322. 零钱兑换

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/coin-change/
- 题目 slug：`coin-change`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.3 图论建模 + BFS 最短路
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/coin-change/solutions/2119065/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-21m5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：完全背包 / BFS 最短路（Python/Java/C++/Go）](https://leetcode.cn/problems/coin-change/solutions/2119065/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-21m5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-21m5`
- topic id：`2119065`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。如果这个视频对你有帮助，欢迎一键三连！

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

### 答疑

**问**：为什么不需要写 $\textit{dfs}(i - 1, c - \textit{coins}[i])$ 呢？选了一个就不再选了，这样不行吗？

**答**：其实我们已经考虑这种情况了，先「选一个」，递归到 $\textit{dfs}(i, c - \textit{coins}[i])$，在这个递归中再「不选」，就能递归到 $\textit{dfs}(i - 1, c - \textit{coins}[i])$ 了。也就是说，递归两次，我们就能表达出「选了一个就不再选」的逻辑。从这个例子，相信读者能体会到递归所蕴含的强大表达能力。

```py [sol-Python3]
class Solution:
    def coinChange(self, coins: List[int], amount: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, c: int) -> int:
            if i < 0:
                return 0 if c == 0 else inf
            if c < coins[i]:  # 只能不选
                return dfs(i - 1, c)
            # 不选 vs 继续选
            return min(dfs(i - 1, c), dfs(i, c - coins[i]) + 1)

        ans = dfs(len(coins) - 1, amount)
        return ans if ans < inf else -1
```

```java [sol-Java]
class Solution {
    public int coinChange(int[] coins, int amount) {
        int n = coins.length;
        int[][] memo = new int[n][amount + 1];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }

        int ans = dfs(n - 1, amount, coins, memo);
        return ans < Integer.MAX_VALUE / 2 ? ans : -1;
    }

    private int dfs(int i, int c, int[] coins, int[][] memo) {
        if (i < 0) {
            return c == 0 ? 0 : Integer.MAX_VALUE / 2; // 除 2 防止下面 + 1 溢出
        }
        if (memo[i][c] != -1) { // 之前计算过
            return memo[i][c];
        }
        if (c < coins[i]) { // 只能不选
            return memo[i][c] = dfs(i - 1, c, coins, memo);
        }
        // 不选 vs 继续选
        return memo[i][c] = Math.min(dfs(i - 1, c, coins, memo), dfs(i, c - coins[i], coins, memo) + 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        int n = coins.size();
        vector memo(n, vector<int>(amount + 1, -1)); // -1 表示没有计算过

        // lambda 递归函数
        auto dfs = [&](this auto&& dfs, int i, int c) -> int {
            if (i < 0) {
                return c == 0 ? 0 : INT_MAX / 2; // 除 2 防止下面 + 1 溢出
            }
            int& res = memo[i][c]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if (c < coins[i]) { // 只能不选
                return res = dfs(i - 1, c);
            }
            // 不选 vs 继续选
            return res = min(dfs(i - 1, c), dfs(i, c - coins[i]) + 1);
        };

        int ans = dfs(n - 1, amount);
        return ans < INT_MAX / 2 ? ans : -1;
    }
};
```

```go [sol-Go]
func coinChange(coins []int, amount int) int {
    n := len(coins)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, amount+1)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }

    var dfs func(int, int) int
    dfs = func(i, c int) (res int) {
        if i < 0 {
            if c == 0 {
                return 0
            }
            return math.MaxInt / 2 // 除 2 防止下面 + 1 溢出
        }
        p := &memo[i][c]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if c < coins[i] { // 只能不选
            return dfs(i-1, c)
        }
        return min(dfs(i-1, c), dfs(i, c-coins[i])+1) // 不选 vs 继续选
    }

    ans := dfs(n-1, amount)
    if ans < math.MaxInt/2 {
        return ans
    }
    return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\cdot\textit{amount})$。

## 二、1:1 翻译成递推

```py [sol-Python3]
class Solution:
    def coinChange(self, coins: List[int], amount: int) -> int:
        n = len(coins)
        f = [[inf] * (amount + 1) for _ in range(n + 1)]
        f[0][0] = 0
        for i, x in enumerate(coins):
            for c in range(amount + 1):
                if c < x:
                    f[i + 1][c] = f[i][c]
                else:
                    f[i + 1][c] = min(f[i][c], f[i + 1][c - x] + 1)
        ans = f[n][amount]
        return ans if ans < inf else -1
```

```java [sol-Java]
class Solution {
    public int coinChange(int[] coins, int amount) {
        int n = coins.length;
        int[][] f = new int[n + 1][amount + 1];
        Arrays.fill(f[0], Integer.MAX_VALUE / 2); // 除 2 防止下面 + 1 溢出
        f[0][0] = 0;
        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= amount; c++) {
                if (c < coins[i]) {
                    f[i + 1][c] = f[i][c];
                } else {
                    f[i + 1][c] = Math.min(f[i][c], f[i + 1][c - coins[i]] + 1);
                }
            }
        }
        int ans = f[n][amount];
        return ans < Integer.MAX_VALUE / 2 ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        int n = coins.size();
        vector f(n + 1, vector<int>(amount + 1, INT_MAX / 2)); // 除 2 防止下面 + 1 溢出
        f[0][0] = 0;
        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= amount; c++) {
                if (c < coins[i]) {
                    f[i + 1][c] = f[i][c];
                } else {
                    f[i + 1][c] = min(f[i][c], f[i + 1][c - coins[i]] + 1);
                }
            }
        }
        int ans = f[n][amount];
        return ans < INT_MAX / 2 ? ans : -1;
    }
};
```

```go [sol-Go]
func coinChange(coins []int, amount int) int {
    n := len(coins)
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, amount+1)
    }
    for j := range f[0] {
        f[0][j] = math.MaxInt / 2 // 除 2 防止下面 + 1 溢出
    }
    f[0][0] = 0
    for i, x := range coins {
        for c := 0; c <= amount; c++ {
            if c < x {
                f[i+1][c] = f[i][c]
            } else {
                f[i+1][c] = min(f[i][c], f[i+1][c-x]+1)
            }
        }
    }
    ans := f[n][amount]
    if ans < math.MaxInt/2 {
        return ans
    }
    return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\cdot\textit{amount})$。

## 三、空间优化：两个数组（滚动数组）

```py [sol-Python3]
class Solution:
    def coinChange(self, coins: List[int], amount: int) -> int:
        n = len(coins)
        f = [[inf] * (amount + 1) for _ in range(2)]
        f[0][0] = 0
        for i, x in enumerate(coins):
            for c in range(amount + 1):
                if c < x:
                    f[(i + 1) % 2][c] = f[i % 2][c]
                else:
                    f[(i + 1) % 2][c] = min(f[i % 2][c], f[(i + 1) % 2][c - x] + 1)
        ans = f[n % 2][amount]
        return ans if ans < inf else -1
```

```java [sol-Java]
class Solution {
    public int coinChange(int[] coins, int amount) {
        int n = coins.length;
        int[][] f = new int[2][amount + 1];
        Arrays.fill(f[0], Integer.MAX_VALUE / 2);
        f[0][0] = 0;
        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= amount; c++) {
                if (c < coins[i]) {
                    f[(i + 1) % 2][c] = f[i % 2][c];
                } else {
                    f[(i + 1) % 2][c] = Math.min(f[i % 2][c], f[(i + 1) % 2][c - coins[i]] + 1);
                }
            }
        }
        int ans = f[n % 2][amount];
        return ans < Integer.MAX_VALUE / 2 ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        int n = coins.size();
        vector f(2, vector<int>(amount + 1, INT_MAX / 2));
        f[0][0] = 0;
        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= amount; c++) {
                if (c < coins[i]) {
                    f[(i + 1) % 2][c] = f[i % 2][c];
                } else {
                    f[(i + 1) % 2][c] = min(f[i % 2][c], f[(i + 1) % 2][c - coins[i]] + 1);
                }
            }
        }
        int ans = f[n % 2][amount];
        return ans < INT_MAX / 2 ? ans : -1;
    }
};
```

```go [sol-Go]
func coinChange(coins []int, amount int) int {
    n := len(coins)
    f := make([][]int, 2)
    for i := range f {
        f[i] = make([]int, amount+1)
    }
    for j := range f[0] {
        f[0][j] = math.MaxInt / 2
    }
    f[0][0] = 0
    for i, x := range coins {
        for c := 0; c <= amount; c++ {
            if c < x {
                f[(i+1)%2][c] = f[i%2][c]
            } else {
                f[(i+1)%2][c] = min(f[i%2][c], f[(i+1)%2][c-coins[i]]+1)
            }
        }
    }
    ans := f[n%2][amount]
    if ans < math.MaxInt/2 {
        return ans
    }
    return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(\textit{amount})$。

## 四、空间优化：一个数组

好比在一面墙上画画，原来这面墙画的是 $f[i]$，现在要画一副新的画，把原来的画**覆盖**掉，新的画叫做 $f[i+1]$。

在循环的过程中：

- 对于 $c<x$ 的状态，转移方程是 $f[i+1][c] = f[i][c]$，这说明原来画的内容保持不变，空间优化后是 $f[c] = f[c]$，这个赋值是多余的。所以可以从 $c=x$ 开始循环。
- 对于 $c\ge x$ 的状态，转移方程是 $f[i+1][c] = \min(f[i][c], f[i+1][c-x]+1)$，其中 $f[i][c]$ 就地取材，$f[i+1][c-x]$ 是新画的内容，从这面墙的下标 $c-x$ 处取到，所以空间优化后就是 $f[c] = \min(f[c], f[c - x] + 1)$。

关于先枚举物品还是先枚举体积的讨论，见 [377. 组合总和 Ⅳ 我的题解](https://leetcode.cn/problems/combination-sum-iv/solutions/2706336/ben-zhi-shi-pa-lou-ti-cong-ji-yi-hua-sou-y52j/) 中的「**答疑**」。

```py [sol-Python3]
class Solution:
    def coinChange(self, coins: List[int], amount: int) -> int:
        f = [0] + [inf] * amount
        for x in coins:
            for c in range(x, amount + 1):
                f[c] = min(f[c], f[c - x] + 1)
        ans = f[amount]
        return ans if ans < inf else -1
```

```java [sol-Java]
class Solution {
    public int coinChange(int[] coins, int amount) {
        int[] f = new int[amount + 1];
        Arrays.fill(f, Integer.MAX_VALUE / 2);
        f[0] = 0;
        for (int x : coins) {
            for (int c = x; c <= amount; c++) {
                f[c] = Math.min(f[c], f[c - x] + 1);
            }
        }
        int ans = f[amount];
        return ans < Integer.MAX_VALUE / 2 ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        vector<int> f(amount + 1, INT_MAX / 2);
        f[0] = 0;
        for (int x : coins) {
            for (int c = x; c <= amount; c++) {
                f[c] = min(f[c], f[c - x] + 1);
            }
        }
        int ans = f[amount];
        return ans < INT_MAX / 2 ? ans : -1;
    }
};
```

```go [sol-Go]
func coinChange(coins []int, amount int) int {
    f := make([]int, amount+1)
    for i := range f {
        f[i] = math.MaxInt / 2
    }
    f[0] = 0
    for _, x := range coins {
        for c := x; c <= amount; c++ {
            f[c] = min(f[c], f[c-x]+1)
        }
    }
    ans := f[amount]
    if ans < math.MaxInt/2 {
        return ans
    }
    return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(\textit{amount})$。

## 五、BFS 最短路

设当前凑成的总金额为 $s$。添加一枚硬币 $\textit{coins}[i]$ 后，$s$ 变成了 $s + \textit{coins}[i]$。

把总金额当作节点编号，从 $s$ 到 $s + \textit{nums}[i]$ 连一条有向边，我们可以得到一张有向图。

本题相当于：

- 计算从起点 $0$ 到终点 $\textit{amount}$ 的**最短路长度**。

这可以用 **BFS** 解决。

下面代码用双数组实现 BFS，原理请看[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

```py [sol-Python3]
class Solution:
    def coinChange(self, coins: List[int], amount: int) -> int:
        q = [0]
        vis = [True] + [False] * amount
        step = 0

        while q:
            nxt = []
            for s in q:
                if s == amount:
                    return step
                for x in coins:
                    t = s + x
                    if t <= amount and not vis[t]:  # 之前没有访问过
                        vis[t] = True  # 避免重复访问
                        nxt.append(t)
            q = nxt
            step += 1

        return -1
```

```java [sol-Java]
class Solution {
    public int coinChange(int[] coins, int amount) {
        List<Integer> q = List.of(0);
        boolean[] vis = new boolean[amount + 1];
        vis[0] = true;

        for (int step = 0; !q.isEmpty(); step++) {
            List<Integer> nxt = new ArrayList<>();
            for (int s : q) {
                if (s == amount) {
                    return step;
                }
                for (int x : coins) {
                    if (s <= amount - x && !vis[s + x]) { // 之前没有访问过
                        vis[s + x] = true; // 避免重复访问
                        nxt.add(s + x);
                    }
                }
            }
            q = nxt;
        }

        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        vector<int> q = {0};
        vector<int8_t> vis(amount + 1);
        vis[0] = true;

        for (int step = 0; !q.empty(); step++) {
            auto tmp = move(q);
            for (int s : tmp) {
                if (s == amount) {
                    return step;
                }
                for (int x : coins) {
                    if (s <= amount - x && !vis[s + x]) { // 之前没有访问过
                        vis[s + x] = true; // 避免重复访问
                        q.push_back(s + x);
                    }
                }
            }
        }

        return -1;
    }
};
```

```go [sol-Go]
func coinChange(coins []int, amount int) int {
	q := []int{0}
	vis := make([]bool, amount+1)
	vis[0] = true

	for step := 0; len(q) > 0; step++ {
		nxt := []int{}
		for _, s := range q {
			if s == amount {
				return step
			}
			for _, x := range coins {
				t := s + x
				if t <= amount && !vis[t] { // 之前没有访问过
					vis[t] = true // 避免重复访问
					nxt = append(nxt, t)
				}
			}
		}
		q = nxt
	}

	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(\textit{amount})$。

## 专题训练

1. 动态规划题单的「**§3.2 完全背包**」。
2. 图论题单的「**§1.3 图论建模 + BFS 最短路**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、图的遍历 / §1.3 图论建模 + BFS 最短路`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.3 图论建模 + BFS 最短路`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
