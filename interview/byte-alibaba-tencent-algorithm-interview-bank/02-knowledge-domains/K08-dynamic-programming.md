# K08 动态规划

## 核心问题

- **状态与转移**：用"状态定义 + 转移方程 + 初始化 + 遍历顺序 + 答案"五步法把指数搜索压成多项式递推。
- **无后效性**：当前状态一旦确定，后续只依赖它而不依赖到达它的具体路径，这是能用 DP 的前提。
- **线性 DP**：爬楼梯、打家劫舍、最长递增子序列等一维递推。
- **区间 DP**：以区间长度递增枚举，求括号匹配、回文、矩阵链乘、戳气球一类"合并子区间"的问题。
- **背包**：0-1 背包、完全背包、多重背包及其滚动数组写法。
- **状态压缩 DP**：以位掩码表示集合，解决 TSP、子集划分、棋盘覆盖。
- **DP 优化**：滚动数组省空间，单调队列/前缀和优化转移省时间。

学习目标是能识别题目中的重叠子问题、写出正确转移并给出边界正确的 C++17 实现。

## 训练顺序

1. 先用爬楼梯、打家劫舍建立"定义状态—写转移—滚动变量"的肌肉记忆。
2. 再学背包三种形态，重点记牢"0-1 倒序、完全正序"这一条决定性的循环方向。
3. 接着练区间 DP，掌握"按长度枚举、按分割点转移"的骨架。
4. 然后做状态压缩 DP，理解 `mask` 表示集合、`mask` 的子集枚举。
5. 最后做 DP 优化：滚动数组、单调队列、前缀和加速，并对比优化前后复杂度。

## 核心模式

### 1. 五步法与线性 DP（爬楼梯）

```cpp
int climbStairs(int n) {
    if (n <= 2) return n;                    // f(1)=1, f(2)=2
    long long a = 1, b = 2;                  // a=f(i-2), b=f(i-1)
    for (int i = 3; i <= n; ++i) {
        long long c = a + b;                 // f(i)=f(i-1)+f(i-2)
        a = b;
        b = c;
    }
    return (int)b;
}
```

状态：`f(k)` 为到第 `k` 级的方案数。转移：`f(k)=f(k-1)+f(k-2)`。不变量：第 `i` 轮开始时 `a=f(i-2)、b=f(i-1)`。复杂度：时间 `O(n)`，滚动后空间 `O(1)`。

### 2. 最长递增子序列（LIS，`O(n log n)`）

```cpp
int lis(const vector<int>& a) {
    vector<int> tails;                       // tails[k]=长度 k+1 的递增子序列的最小结尾
    for (int x : a) {
        auto it = lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;                        // 用更小的结尾替换，strictly increasing
    }
    return (int)tails.size();                // tails 长度即 LIS 长度
}
```

不变量：`tails` 严格递增，`tails[k]` 是长度为 `k+1` 的所有递增子序列中可能的最小结尾。复杂度 `O(n log n)`。（注意：`tails` 本身不是某条 LIS，只用于求长度；要还原序列需额外记录前驱。）

### 3. 0-1 背包（每件最多一次，倒序内层）

```cpp
int knapsack01(const vector<int>& w, const vector<int>& v, int W) {
    vector<int> dp(W + 1, 0);
    for (int i = 0; i < (int)w.size(); ++i)
        for (int c = W; c >= w[i]; --c)      // 倒序：保证第 i 件只用一次
            dp[c] = max(dp[c], dp[c - w[i]] + v[i]);
    return dp[W];
}
```

不变量：处理完前 `i` 件后，`dp[c]` 是容量 `c` 下的最大价值。复杂度：时间 `O(nW)`、空间 `O(W)`。

### 4. 完全背包（每件可重复，正序内层）

```cpp
int knapsackComplete(const vector<int>& w, const vector<int>& v, int W) {
    vector<int> dp(W + 1, 0);
    for (int i = 0; i < (int)w.size(); ++i)
        for (int c = w[i]; c <= W; ++c)      // 正序：允许同一件被多次选用
            dp[c] = max(dp[c], dp[c - w[i]] + v[i]);
    return dp[W];
}
```

不变量同 0-1 背包，但正序遍历使 `dp[c-w[i]]` 可能已包含第 `i` 件，等价于"可重复选"。复杂度：`O(nW)` 时间、`O(W)` 空间。

> 关键记忆：**0-1 倒序，完全正序**。方向只差一个循环写法，结果却完全不同，是最高频的失分点。

### 5. 区间 DP 骨架

```cpp
// dp[i][j]：区间 [i, j] 的最优值
vector<vector<int>> dp(n, vector<int>(n, 0));
for (int len = 2; len <= n; ++len)                 // 按长度从小到大
    for (int i = 0; i + len - 1 < n; ++i) {
        int j = i + len - 1;
        dp[i][j] = INT_MAX;                        // 或按题意初值
        for (int k = i; k < j; ++k)                // 枚举分割点
            dp[i][j] = min(dp[i][j], dp[i][k] + dp[k + 1][j] + cost(i, j));
    }
```

不变量：计算长度为 `len` 的区间时，所有更短区间已算好。复杂度：`O(n^3)` 时间、`O(n^2)` 空间。适用于"两个子区间合并成一个区间"的结构。

### 6. 状态压缩 DP（子集 / TSP）

```cpp
// dp[mask] 表示已选集合为 mask 时的最值
const int FULL = 1 << k;
vector<int> dp(FULL);
for (int mask = 1; mask < FULL; ++mask) {
    int low = __builtin_ctz(mask);               // 最低位所在元素
    int rest = mask ^ (1 << low);
    // 例如：dp[mask] = min over sub of dp[rest...]，按题意写
}
// 枚举 mask 的所有非空子集 sub
for (int sub = mask; sub; sub = (sub - 1) & mask) { /* 使用 sub */ }
```

不变量：`mask` 的每一位表示一个元素是否被选入集合。复杂度：状态数 `2^k`，每个状态转移代价决定总复杂度（TSP 为 `O(2^k·k^2)`）。空间 `O(2^k)`。

### 7. DP 优化：单调队列加速滑动窗口转移

```cpp
vector<int> maxSlidingWindow(const vector<int>& a, int k) {
    deque<int> dq;                               // 存下标，对应值单调递减
    vector<int> res;
    for (int i = 0; i < (int)a.size(); ++i) {
        while (!dq.empty() && dq.front() <= i - k) dq.pop_front();  // 出窗口
        while (!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();  // 维持单调
        dq.push_back(i);
        if (i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}
```

不变量：`dq` 内下标递增、对应值递减，队首是当前窗口最大值。用它可把形如 `dp[i] = min(dp[j] + a[i]) (i-k <= j < i)` 的转移从 `O(nk)` 降到 `O(n)`。复杂度：`O(n)` 时间、`O(k)` 空间。

## 常见陷阱

- **0-1 与完全背包循环方向写反**：0-1 用正序会变成"物品可重复选"，完全背包用倒序会退化成"每件只用一次"，是头号错误。
- **初始化错误**：求"恰好装满"时除 `dp[0]=0` 外其余应置 `-INF`；求"至多/不超过"时全置 `0`。用错会得到本该不可能的"合法"答案。滚动数组时初始化写在循环外，别被覆盖。
- **遍历顺序错**：区间 DP 必须按长度递增，长度从 0 或 1 起算要按题目定义（空区间/单元素）；写反会用到未计算的子问题。
- **下标越界/off-by-one**：`dp` 数组维度常需 `n+1`；`dp[i-1]` 在 `i=0` 时越界，需特判或错位定义。
- **状态定义不含关键信息**：如背包加了"选了几个"的约束却没把个数放进状态，导致无法区分。
- **重复计数**：组合型 DP（如零钱兑换的组合数）要固定物品遍历顺序在外层，避免把 `{1,2}` 与 `{2,1}` 当成两种方案。
- **忘记取模或溢出**：计数型 DP 结果常超出 `int`，需 `long long` 或按题意取模；`INF` 相加可能溢出，取 `INF = 总量上限 + 1` 或用 `long long`。
- **无后效性被破坏**：若当前决策影响后续可选项的"顺序"而非仅"状态"，单纯 DP 不成立，需换状态或换算法。
- **记忆化与递推混用不一致**：递归边界与递推初始化两处不一致会产生偏差，应二选一并保持同一套边界。

## 复杂度速查

| 模型 | 时间 | 空间 | 典型题 |
|---|---|---|---|
| 线性 DP（滚动变量） | `O(n)` | `O(1)` | 爬楼梯、打家劫舍 |
| 线性 DP（数组） | `O(n)` 或 `O(n^2)` | `O(n)` | 最大子段和 |
| LIS | `O(n log n)` | `O(n)` | 最长递增子序列 |
| 区间 DP | `O(n^3)` | `O(n^2)` | 最长回文子序列、戳气球 |
| 0-1 背包 | `O(nW)` | `O(W)`（滚动） | 子集和、分割等和 |
| 完全背包 | `O(nW)` | `O(W)`（滚动） | 零钱兑换、完全平方数 |
| 多重背包 | `O(W·Σ log c)` | `O(W)` | 二进制拆分 |
| 状态压缩 DP | `O(2^k · T)` | `O(2^k)` | TSP、子集划分 |
| 单调队列优化 | `O(n)` | `O(k)` | 滑动窗口最值、受限转移 |

说明：`W` 为容量，`c` 为某物品可用次数，`k` 为元素个数，`T` 为单状态转移代价。

## 题目索引

本域对应的真实题目文件：

- `../01-question-bank/beginner/B002-climbing-stairs.md` —— 线性 DP 与滚动数组入门，`f(n)=f(n-1)+f(n-2)` 的完整推导与边界分析。

延伸方向（题面变体，非独立文件）：打家劫舍（线性 DP）、最长递增子序列（`O(n log n)` 变形）、0-1/完全背包（分割等和、零钱兑换）、区间 DP（戳气球）、状态压缩（TSP），可在覆盖矩阵 `../00-overview/coverage-matrix.md` 的 `K08` 条目下继续补题。
