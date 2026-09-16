# 1125. 最小的必要团队

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-sufficient-team/
- 题目 slug：`smallest-sufficient-team`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP
- 难度分：2251
- 外部题解来源：https://leetcode.cn/problems/smallest-sufficient-team/solutions/2214387/zhuang-ya-0-1-bei-bao-cha-biao-fa-vs-shu-qode/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[状压 0-1 背包，查表法 vs 刷表法（Python/Java/C++/Go）](https://leetcode.cn/problems/smallest-sufficient-team/solutions/2214387/zhuang-ya-0-1-bei-bao-cha-biao-fa-vs-shu-qode/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuang-ya-0-1-bei-bao-cha-biao-fa-vs-shu-qode`
- topic id：`2214387`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：动态规划、记忆化搜索、0-1 背包

1. [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)
2. [0-1 背包与完全背包【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)

## 前置知识：集合论与位运算

集合可以用二进制表示，二进制从低到高第 $i$ 位为 $1$ 表示 $i$ 在集合中，为 $0$ 表示 $i$ 不在集合中。例如集合 $\{0,2,3\}$ 对应的二进制数为 $1101_{(2)}$。

本题中用到的位运算技巧：

1. 将元素 $x$ 变成集合 $\{x\}$，即 `1 << x`。
2. 判断元素 $x$ 是否在集合 $A$ 中，即 `((A >> x) & 1) == 1`。
3. 计算两个集合 $A,B$ 的并集 $A\cup B$，即 `A | B`。例如 `110 | 11 = 111`。
4. 计算 $A \setminus  B$，表示从集合 $A$ 中去掉在集合 $B$ 中的元素，即 `A & ~B`。例如 `110 & ~11 = 100`。
5. 全集 $U=\{0,1,\cdots,n-1\}$，即 `(1 << n) - 1`。

更多内容请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

## 一、转换成 0-1 背包问题

重新描述一遍问题：从 $\textit{people}$ 中选择一些元素（技能集合），这些技能集合的并集等于 $\textit{reqSkills}$，要求选的元素个数尽量少。

把 $\textit{people}[i]$ 看成物品（集合），$\textit{reqSkills}$ 看成背包容量（目标集合），本题就是一个集合版本的 0-1 背包问题。

为方便计算，先把 $\textit{reqSkills}$ 中的每个字符串映射到其下标上，记到一个哈希表 $\textit{sid}$ 中。然后把每个 $\textit{people}[i]$ 通过映射转换成数字集合，再压缩成一个二进制数。

例如示例 1，把 `"java","nodejs","reactjs"` 分别映射到 $0,1,2$ 上，那么 $\textit{people}[0],\textit{people}[1],\textit{people}[2]$ 按照这种映射关系就转换成集合 $\{0\},\{1\},\{1,2\}$，对应的二进制数分别为 $1_{(2)}, 10_{(2)}, 110_{(2)}$。那么选择集合 $\{0\}$ 和 $\{1,2\}$，它俩的并集为 $\{0,1,2\}$，满足题目要求。这等价于选择二进制数 $1_{(2)}$ 和 $110_{(2)}$，它俩的或运算的结果是 $111_{(2)}$，就对应着集合 $\{0,1,2\}$。

类似 0-1 背包，定义 $\textit{dfs}(i, j)$ 表示从前 $i$ 个集合中选择一些集合，并集包含 $j$，至少需要选择的集合个数。

分类讨论：

- 不选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j)$。
- 选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j\setminus\textit{people}[i])+1$。这是因为选了第 $i$ 个集合 $\textit{people}[i]$ 后，就不需要再包含 $\textit{people}[i]$ 中的任意元素了，所以直接去掉，去计算包含 $j\setminus\textit{people}[i]$ 至少要选多少个集合。
- 取最小值，即 $\textit{dfs}(i, j) = \min(\textit{dfs}(i-1, j),\textit{dfs}(i-1, j\setminus\textit{people}[i])+1)$。

由于本题还需要输出具体方案，为了方便存储，我们可以把人员编号集合也用二进制数表示。这是因为本题 $\textit{people}$ 的长度不超过 $60$，可以压缩到一个 $64$ 位整数中。如果长度大于 $64$ 呢？更加通用的做法我在 [1092. 最短公共超序列（题解）](https://leetcode.cn/problems/shortest-common-supersequence/solution/cong-di-gui-dao-di-tui-jiao-ni-yi-bu-bu-auy8z/)中详细介绍了，留作练习，欢迎把代码贴在评论区。

修改后，$\textit{dfs}(i, j)$ 定义成从前 $i$ 个集合中选择一些集合，并集包含 $j$，所选择的最小下标集合：

- 不选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j)$。
- 选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j\setminus\textit{people}[i])\cup \{i\}$。
- 取这两个集合中大小最小的。
- 递归边界：如果 $j=\varnothing$，返回 $\varnothing$。如果 $i<0$，返回全集 $U=\{0,1,\cdots,n-1\}$（也可以再多加一个 $n$，不过由于题目保证答案存在，这样就够了）。
- 上述是用集合的语言描述的，代码中用位运算实现。

```py [sol1-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        mask = [0] * n
        for i, skills in enumerate(people):
            for s in skills:  # 把 skills 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s]

        @cache
        def dfs(i: int, j: int) -> int:
            if j == 0: return 0  # 背包已装满
            if i < 0: return (1 << n) - 1  # 没法装满背包，返回全集，这样下面比较集合大小会取更小的
            res = dfs(i - 1, j)  # 不选 mask[i]
            res2 = dfs(i - 1, j & ~mask[i]) | (1 << i)  # 选 mask[i]
            return res if res.bit_count() < res2.bit_count() else res2

        res = dfs(n - 1, (1 << len(req_skills)) - 1)
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol1-Java]
class Solution {
    private long all;
    private int[] mask;
    private long[][] memo;

    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size();
        mask = new int[n];
        for (int i = 0; i < n; ++i)
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid.get(s);

        int u = 1 << m;
        memo = new long[n][u];
        for (int i = 0; i < n; i++)
            Arrays.fill(memo[i], -1); // -1 表示还没有计算过
        all = (1L << n) - 1;
        long res = dfs(n - 1, u - 1);

        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }

    private long dfs(int i, int j) {
        if (j == 0) return 0; // 背包已装满
        if (i < 0) return all; // 没法装满背包，返回全集，这样下面比较集合大小会取更小的
        if (memo[i][j] != -1) return memo[i][j];
        long res = dfs(i - 1, j); // 不选 mask[i]
        long res2 = dfs(i - 1, j & ~mask[i]) | (1L << i); // 选 mask[i]
        return memo[i][j] = Long.bitCount(res) < Long.bitCount(res2) ? res : res2;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), mask[n];
        memset(mask, 0, sizeof(mask));
        for (int i = 0; i < n; ++i)
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s];

        int u = 1 << m;
        long long memo[n][u], all = (1LL << n) - 1;
        memset(memo, -1, sizeof(memo)); // -1 表示还没有计算过
        function<long long(int, int)> dfs = [&](int i, int j) -> long long {
            if (j == 0) return 0; // 背包已装满
            if (i < 0) return all; // 没法装满背包，返回全集，这样下面比较集合大小会取更小的
            auto &res = memo[i][j]; // 注意这里是引用，下面会直接修改 memo[i][j]
            if (res != -1) return res;
            auto r1 = dfs(i - 1, j); // 不选 mask[i]
            auto r2 = dfs(i - 1, j & ~mask[i]) | (1LL << i); // 选 mask[i]
            return res = __builtin_popcountll(r1) < __builtin_popcountll(r2) ? r1 : r2;
        };
        auto res = dfs(n - 1, u - 1);

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol1-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    mask := make([]int, n)
    for i, skills := range people {
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask[i] |= 1 << sid[s]
        }
    }

    u, all := 1<<m, 1<<n-1
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, u)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示还没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if j == 0 { // 背包已装满
            return 0
        }
        if i < 0 { // 没法装满背包，返回全集，这样下面比较集合大小会取更小的
            return all
        }
        p := &memo[i][j]
        if *p != -1 {
            return *p
        }
        r1 := dfs(i-1, j) // 不选 mask[i]
        r2 := dfs(i-1, j&^mask[i]) | 1<<i // 选 mask[i]
        if bits.OnesCount(uint(r1)) < bits.OnesCount(uint(r2)) {
            *p = r1
        } else {
            *p = r2
        }
        return *p
    }
    res := dfs(n-1, u-1)

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，记忆化搜索的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此记忆化搜索的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+n2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

> 注意 $T\ge S$，所以时间复杂度中忽略了 $S$。

## 二、1:1 翻译成递推

把 $\textit{dfs}$ 改成 $f$ 数组，把递归改成循环就好了。相当于原来是用递归计算每个状态 $(i,j)$，现在改用循环去计算每个状态 $(i,j)$。

由于需要处理 $i<0$，也就是需要 $f[-1]$ 这个状态，那么在 $f$ 的前面插入一个状态，$f[0]$ 就对应到 $i<0$ 的情况了，原来的 $f[i]$ 需要变成 $f[i+1]$。

### 答疑

**问**：为什么变慢了？

**答**：因为有很多状态是不需要计算的。比如集合 $\{0\}$ 和 $\{1,2\}$，在记忆化搜索中，是不会递归到 $j=\{0,1\}$ 这种集合的，而递推需要计算所有状态。

没关系，后面会优化。

```py [sol2-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        u = 1 << len(req_skills)
        # f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
        f = [[0] * u for _ in range(n + 1)]
        f[0] = [(1 << n) - 1] * u  # 对应记忆化搜索中的 if i < 0: return (1 << n) - 1
        f[0][0] = 0
        for i, skills in enumerate(people):
            mask = 0
            for s in skills:  # 把 skills 压缩成一个二进制数 mask
                mask |= 1 << sid[s]
            for j in range(1, u):
                res = f[i][j]  # 不选 mask
                res2 = f[i][j & ~mask] | (1 << i)  # 选 mask
                f[i + 1][j] = res if res.bit_count() < res2.bit_count() else res2
        res = f[-1][-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol2-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        // f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
        var f = new long[n + 1][u];
        Arrays.fill(f[0], (1L << n) - 1); // 对应记忆化搜索中的 if (i < 0) return all;
        f[0][0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask
                mask |= 1 << sid.get(s);
            for (int j = 1; j < u; ++j) {
                long res = f[i][j]; // 不选 mask
                long res2 = f[i][j & ~mask] | (1L << i); // 选 mask
                f[i + 1][j] = Long.bitCount(res) < Long.bitCount(res2) ? res : res2;
            }
        }

        long res = f[n][u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        // f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
        long long f[n + 1][u];
        fill(f[0], f[0] + u, (1LL << n) - 1); // 对应记忆化搜索中的 if (i < 0) return all;
        f[0][0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask |= 1 << sid[s];
            f[i + 1][0] = 0;
            for (int j = 1; j < u; ++j) {
                auto r1 = f[i][j]; // 不选 mask
                auto r2 = f[i][j & ~mask] | (1L << i); // 选 mask
                f[i + 1][j] = __builtin_popcountll(r1) < __builtin_popcountll(r2) ? r1 : r2;
            }
        }
        auto res = f[n][u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol2-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    u, all := 1<<m, 1<<n-1
    // f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, u)
    }
    for j := 1; j < u; j++ {
        f[0][j] = all // 对应记忆化搜索中的 if (i < 0) return all
    }
    for i, skills := range people {
        mask := 0
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask |= 1 << sid[s]
        }
        for j := 1; j < u; j++ {
            r1 := f[i][j] // 不选 mask[i]
            r2 := f[i][j&^mask] | 1<<i // 选 mask[i]
            if bits.OnesCount(uint(r1)) < bits.OnesCount(uint(r2)) {
                f[i+1][j] = r1
            } else {
                f[i+1][j] = r2
            }
        }
    }
    res := f[n][u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，递推的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此递推的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+n2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

## 三、空间优化

由于计算 $f[i+1]$ 只需要 $f[i]$，不需要下标更小的，所以只需要一个长为 $2^m$ 的数组。

实现时需要倒序循环 $j$，原理见 [0-1 背包与完全背包【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。

```py [sol3-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        u = 1 << len(req_skills)
        f = [(1 << n) - 1] * u
        f[0] = 0
        for i, skills in enumerate(people):
            mask = 0
            for s in skills:  # 把 skills 压缩成一个二进制数 mask
                mask |= 1 << sid[s]
            for j in range(u - 1, 0, -1):
                res = f[j]  # 不选 mask
                res2 = f[j & ~mask] | (1 << i)  # 选 mask
                f[j] = res if res.bit_count() < res2.bit_count() else res2
        res = f[-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol3-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        var f = new long[u];
        Arrays.fill(f, (1L << n) - 1);
        f[0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask
                mask |= 1 << sid.get(s);
            for (int j = u - 1; j > 0; --j) {
                long res = f[j]; // 不选 mask
                long res2 = f[j & ~mask] | (1L << i); // 选 mask
                f[j] = Long.bitCount(res) < Long.bitCount(res2) ? res : res2;
            }
        }

        long res = f[u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        long long f[u];
        fill(f, f + u, (1LL << n) - 1);
        f[0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask |= 1 << sid[s];
            for (int j = u - 1; j; --j) {
                auto r1 = f[j]; // 不选 mask
                auto r2 = f[j & ~mask] | (1L << i); // 选 mask
                f[j] = __builtin_popcountll(r1) < __builtin_popcountll(r2) ? r1 : r2;
            }
        }
        auto res = f[u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol3-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    u, all := 1<<m, 1<<n-1
    f := make([]int, u)
    for j := 1; j < u; j++ {
        f[j] = all // 对应记忆化搜索中的 if (i < 0) return all
    }
    for i, skills := range people {
        mask := 0
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask |= 1 << sid[s]
        }
        for j := u - 1; j > 0; j-- {
            r1 := f[j] // 不选 mask[i]
            r2 := f[j&^mask] | 1<<i // 选 mask[i]
            if bits.OnesCount(uint(r1)) > bits.OnesCount(uint(r2)) {
                f[j] = r2
            }
        }
    }
    res := f[u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，递推的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此递推的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

## 四、查表法 vs 刷表法

上面的做法算作查表法，意思是用其它状态更新当前状态。但是这种写法无法跳过无效的状态，在很多不必要的计算上浪费了大量时间。

我们还可以用当前状态去更新其它状态：从小到大遍历每个 $f[j]$，然后遍历 $\textit{mask}$，用 $f[j]$ 去更新 $f[j|\textit{mask}[i]]$。这种做法叫刷表法。

由于我们是从小到大遍历 $j$，并且更新也是更新到比 $j$ 更大的数上，如果 $f[j]$ 等于其初始值，说明它没有被更新过，也就说明 $j$ 无法由若干集合的并集得到，是无效状态，可以直接跳过。

```py [sol4-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        mask = [0] * n
        for i, skills in enumerate(people):
            for s in skills:  # 把 skills 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s]

        ALL = (1 << n) - 1
        u = 1 << len(req_skills)
        f = [ALL] * u  # f[j] 表示并集为 j 要选的最小 people 集合
        f[0] = 0
        for j in range(u - 1):  # f[u-1] 无需计算
            if f[j] == ALL: continue  # 无法更新其它状态，直接跳过
            for i, msk in enumerate(mask):
                if f[j].bit_count() + 1 < f[j | msk].bit_count():
                    f[j | msk] = f[j] | (1 << i)  # 刷表：用 f[j] 去更新其它状态

        res = f[-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol4-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size();
        var mask = new int[n];
        for (int i = 0; i < n; ++i)
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid.get(s);

        long all = (1L << n) - 1;
        int u = 1 << m;
        var f = new long[u]; // f[j] 表示并集为 j 至少要选的 people 集合
        Arrays.fill(f, all);
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < all)
                for (int i = 0; i < n; ++i)
                    if (Long.bitCount(f[j]) + 1 < Long.bitCount(f[j | mask[i]]))
                        f[j | mask[i]] = f[j] | (1L << i); // 刷表：用 f[j] 去更新其它状态

        long res = f[u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol4-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), mask[n];
        memset(mask, 0, sizeof(mask));
        for (int i = 0; i < n; ++i)
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s];

        int u = 1 << m;
        long long all = (1LL << n) - 1, f[u];
        fill(f, f + u, all);
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < all)
                for (int i = 0; i < n; ++i)
                    if (__builtin_popcountll(f[j]) + 1 < __builtin_popcountll(f[j | mask[i]]))
                        f[j | mask[i]] = f[j] | (1LL << i); // 刷表：用 f[j] 去更新其它状态
        auto res = f[u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol4-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    mask := make([]int, n)
    for i, skills := range people {
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask[i] |= 1 << sid[s]
        }
    }

    u, all := 1<<m, 1<<n-1
    f := make([]int, u)
    for j := 1; j < u; j++ {
        f[j] = all // 对应记忆化搜索中的 if (i < 0) return all
    }
    for j, fj := range f {
        if fj < all {
            for i, msk := range mask {
                if bits.OnesCount(uint(fj))+1 < bits.OnesCount(uint(f[j|msk])) {
                    f[j|msk] = fj | 1<<i // 刷表：用 f[j] 去更新其它状态
                }
            }
        }
    }
    res := f[u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 优化

由于计算二进制中 $1$ 的个数也比较耗时，新开一个数组 $\textit{ids}$ 单独记录压缩后的下标集合。这样 $f[j]$ 的含义就是并集为 $j$ 最少要选的集合个数了。

```py [sol5-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        mask = [0] * n
        for i, skills in enumerate(people):
            for s in skills:  # 把 skills 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s]

        u = 1 << len(req_skills)
        ids = [0] * u  # ids[j] 表示 f[j] 对应的 people 下标集合
        f = [inf] * u  # f[j] 表示并集为 j 至少要选的 people 个数
        f[0] = 0
        for j in range(u - 1):  # f[u-1] 无需计算
            if f[j] == inf: continue  # 无法更新其它状态，直接跳过
            for i, msk in enumerate(mask):
                if f[j] + 1 < f[j | msk]:
                    f[j | msk] = f[j] + 1  # 刷表：用 f[j] 去更新其它状态
                    ids[j | msk] = ids[j] | (1 << i)

        res = ids[-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol5-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size();
        var mask = new int[n];
        for (int i = 0; i < n; ++i)
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid.get(s);

        int u = 1 << m;
        var ids = new long[u]; // ids[j] 表示 f[j] 对应的 people 下标集合
        var f = new int[u]; // f[j] 表示并集为 j 至少要选的 people 个数
        Arrays.fill(f, Integer.MAX_VALUE);
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < Integer.MAX_VALUE)
                for (int i = 0; i < n; ++i)
                    if (f[j] + 1 < f[j | mask[i]]) {
                        f[j | mask[i]] = f[j] + 1; // 刷表：用 f[j] 去更新其它状态
                        ids[j | mask[i]] = ids[j] | (1L << i);
                    }

        long res = ids[u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol5-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), mask[n];
        memset(mask, 0, sizeof(mask));
        for (int i = 0; i < n; ++i)
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s];

        int u = 1 << m;
        long long ids[u]; memset(ids, 0, sizeof(ids));
        char f[u]; memset(f, 0x7f, sizeof(f));
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < 0x7f)
                for (int i = 0; i < n; ++i)
                    if (f[j] + 1 < f[j | mask[i]]) {
                        f[j | mask[i]] = f[j] + 1; // 刷表：用 f[j] 去更新其它状态
                        ids[j | mask[i]] = ids[j] | (1LL << i);
                    }
        auto res = ids[u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol5-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    mask := make([]int, n)
    for i, skills := range people {
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask[i] |= 1 << sid[s]
        }
    }

    u := 1 << m
    ids := make([]int, u)
    f := make([]int8, u)
    for j := 1; j < u; j++ {
        f[j] = math.MaxInt8
    }
    for j, fj := range f {
        if fj < math.MaxInt8 {
            for i, msk := range mask {
                if fj+1 < f[j|msk] {
                    f[j|msk] = fj + 1 // 刷表：用 f[j] 去更新其它状态
                    ids[j|msk] = ids[j] | 1<<i
                }
            }
        }
    }
    res := ids[u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，递推的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此递推的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+n+2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

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

## 前置知识：动态规划、记忆化搜索、0-1 背包

1. [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)
2. [0-1 背包与完全背包【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)

## 前置知识：集合论与位运算

集合可以用二进制表示，二进制从低到高第 $i$ 位为 $1$ 表示 $i$ 在集合中，为 $0$ 表示 $i$ 不在集合中。例如集合 $\{0,2,3\}$ 对应的二进制数为 $1101_{(2)}$。

本题中用到的位运算技巧：

1. 将元素 $x$ 变成集合 $\{x\}$，即 `1 << x`。
2. 判断元素 $x$ 是否在集合 $A$ 中，即 `((A >> x) & 1) == 1`。
3. 计算两个集合 $A,B$ 的并集 $A\cup B$，即 `A | B`。例如 `110 | 11 = 111`。
4. 计算 $A \setminus  B$，表示从集合 $A$ 中去掉在集合 $B$ 中的元素，即 `A & ~B`。例如 `110 & ~11 = 100`。
5. 全集 $U=\{0,1,\cdots,n-1\}$，即 `(1 << n) - 1`。

更多内容请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

## 一、转换成 0-1 背包问题

重新描述一遍问题：从 $\textit{people}$ 中选择一些元素（技能集合），这些技能集合的并集等于 $\textit{reqSkills}$，要求选的元素个数尽量少。

把 $\textit{people}[i]$ 看成物品（集合），$\textit{reqSkills}$ 看成背包容量（目标集合），本题就是一个集合版本的 0-1 背包问题。

为方便计算，先把 $\textit{reqSkills}$ 中的每个字符串映射到其下标上，记到一个哈希表 $\textit{sid}$ 中。然后把每个 $\textit{people}[i]$ 通过映射转换成数字集合，再压缩成一个二进制数。

例如示例 1，把 `"java","nodejs","reactjs"` 分别映射到 $0,1,2$ 上，那么 $\textit{people}[0],\textit{people}[1],\textit{people}[2]$ 按照这种映射关系就转换成集合 $\{0\},\{1\},\{1,2\}$，对应的二进制数分别为 $1_{(2)}, 10_{(2)}, 110_{(2)}$。那么选择集合 $\{0\}$ 和 $\{1,2\}$，它俩的并集为 $\{0,1,2\}$，满足题目要求。这等价于选择二进制数 $1_{(2)}$ 和 $110_{(2)}$，它俩的或运算的结果是 $111_{(2)}$，就对应着集合 $\{0,1,2\}$。

类似 0-1 背包，定义 $\textit{dfs}(i, j)$ 表示从前 $i$ 个集合中选择一些集合，并集包含 $j$，至少需要选择的集合个数。

分类讨论：

- 不选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j)$。
- 选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j\setminus\textit{people}[i])+1$。这是因为选了第 $i$ 个集合 $\textit{people}[i]$ 后，就不需要再包含 $\textit{people}[i]$ 中的任意元素了，所以直接去掉，去计算包含 $j\setminus\textit{people}[i]$ 至少要选多少个集合。
- 取最小值，即 $\textit{dfs}(i, j) = \min(\textit{dfs}(i-1, j),\textit{dfs}(i-1, j\setminus\textit{people}[i])+1)$。

由于本题还需要输出具体方案，为了方便存储，我们可以把人员编号集合也用二进制数表示。这是因为本题 $\textit{people}$ 的长度不超过 $60$，可以压缩到一个 $64$ 位整数中。如果长度大于 $64$ 呢？更加通用的做法我在 [1092. 最短公共超序列（题解）](https://leetcode.cn/problems/shortest-common-supersequence/solution/cong-di-gui-dao-di-tui-jiao-ni-yi-bu-bu-auy8z/)中详细介绍了，留作练习，欢迎把代码贴在评论区。

修改后，$\textit{dfs}(i, j)$ 定义成从前 $i$ 个集合中选择一些集合，并集包含 $j$，所选择的最小下标集合：

- 不选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j)$。
- 选第 $i$ 个集合：$\textit{dfs}(i, j) = \textit{dfs}(i-1, j\setminus\textit{people}[i])\cup \{i\}$。
- 取这两个集合中大小最小的。
- 递归边界：如果 $j=\varnothing$，返回 $\varnothing$。如果 $i<0$，返回全集 $U=\{0,1,\cdots,n-1\}$（也可以再多加一个 $n$，不过由于题目保证答案存在，这样就够了）。
- 上述是用集合的语言描述的，代码中用位运算实现。

```py [sol1-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        mask = [0] * n
        for i, skills in enumerate(people):
            for s in skills:  # 把 skills 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s]

        @cache
        def dfs(i: int, j: int) -> int:
            if j == 0: return 0  # 背包已装满
            if i < 0: return (1 << n) - 1  # 没法装满背包，返回全集，这样下面比较集合大小会取更小的
            res = dfs(i - 1, j)  # 不选 mask[i]
            res2 = dfs(i - 1, j & ~mask[i]) | (1 << i)  # 选 mask[i]
            return res if res.bit_count() < res2.bit_count() else res2

        res = dfs(n - 1, (1 << len(req_skills)) - 1)
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol1-Java]
class Solution {
    private long all;
    private int[] mask;
    private long[][] memo;

    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size();
        mask = new int[n];
        for (int i = 0; i < n; ++i)
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid.get(s);

        int u = 1 << m;
        memo = new long[n][u];
        for (int i = 0; i < n; i++)
            Arrays.fill(memo[i], -1); // -1 表示还没有计算过
        all = (1L << n) - 1;
        long res = dfs(n - 1, u - 1);

        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }

    private long dfs(int i, int j) {
        if (j == 0) return 0; // 背包已装满
        if (i < 0) return all; // 没法装满背包，返回全集，这样下面比较集合大小会取更小的
        if (memo[i][j] != -1) return memo[i][j];
        long res = dfs(i - 1, j); // 不选 mask[i]
        long res2 = dfs(i - 1, j & ~mask[i]) | (1L << i); // 选 mask[i]
        return memo[i][j] = Long.bitCount(res) < Long.bitCount(res2) ? res : res2;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), mask[n];
        memset(mask, 0, sizeof(mask));
        for (int i = 0; i < n; ++i)
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s];

        int u = 1 << m;
        long long memo[n][u], all = (1LL << n) - 1;
        memset(memo, -1, sizeof(memo)); // -1 表示还没有计算过
        function<long long(int, int)> dfs = [&](int i, int j) -> long long {
            if (j == 0) return 0; // 背包已装满
            if (i < 0) return all; // 没法装满背包，返回全集，这样下面比较集合大小会取更小的
            auto &res = memo[i][j]; // 注意这里是引用，下面会直接修改 memo[i][j]
            if (res != -1) return res;
            auto r1 = dfs(i - 1, j); // 不选 mask[i]
            auto r2 = dfs(i - 1, j & ~mask[i]) | (1LL << i); // 选 mask[i]
            return res = __builtin_popcountll(r1) < __builtin_popcountll(r2) ? r1 : r2;
        };
        auto res = dfs(n - 1, u - 1);

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol1-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    mask := make([]int, n)
    for i, skills := range people {
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask[i] |= 1 << sid[s]
        }
    }

    u, all := 1<<m, 1<<n-1
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, u)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示还没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if j == 0 { // 背包已装满
            return 0
        }
        if i < 0 { // 没法装满背包，返回全集，这样下面比较集合大小会取更小的
            return all
        }
        p := &memo[i][j]
        if *p != -1 {
            return *p
        }
        r1 := dfs(i-1, j) // 不选 mask[i]
        r2 := dfs(i-1, j&^mask[i]) | 1<<i // 选 mask[i]
        if bits.OnesCount(uint(r1)) < bits.OnesCount(uint(r2)) {
            *p = r1
        } else {
            *p = r2
        }
        return *p
    }
    res := dfs(n-1, u-1)

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，记忆化搜索的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此记忆化搜索的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+n2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

> 注意 $T\ge S$，所以时间复杂度中忽略了 $S$。

## 二、1:1 翻译成递推

把 $\textit{dfs}$ 改成 $f$ 数组，把递归改成循环就好了。相当于原来是用递归计算每个状态 $(i,j)$，现在改用循环去计算每个状态 $(i,j)$。

由于需要处理 $i<0$，也就是需要 $f[-1]$ 这个状态，那么在 $f$ 的前面插入一个状态，$f[0]$ 就对应到 $i<0$ 的情况了，原来的 $f[i]$ 需要变成 $f[i+1]$。

### 答疑

**问**：为什么变慢了？

**答**：因为有很多状态是不需要计算的。比如集合 $\{0\}$ 和 $\{1,2\}$，在记忆化搜索中，是不会递归到 $j=\{0,1\}$ 这种集合的，而递推需要计算所有状态。

没关系，后面会优化。

```py [sol2-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        u = 1 << len(req_skills)
        # f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
        f = [[0] * u for _ in range(n + 1)]
        f[0] = [(1 << n) - 1] * u  # 对应记忆化搜索中的 if i < 0: return (1 << n) - 1
        f[0][0] = 0
        for i, skills in enumerate(people):
            mask = 0
            for s in skills:  # 把 skills 压缩成一个二进制数 mask
                mask |= 1 << sid[s]
            for j in range(1, u):
                res = f[i][j]  # 不选 mask
                res2 = f[i][j & ~mask] | (1 << i)  # 选 mask
                f[i + 1][j] = res if res.bit_count() < res2.bit_count() else res2
        res = f[-1][-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol2-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        // f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
        var f = new long[n + 1][u];
        Arrays.fill(f[0], (1L << n) - 1); // 对应记忆化搜索中的 if (i < 0) return all;
        f[0][0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask
                mask |= 1 << sid.get(s);
            for (int j = 1; j < u; ++j) {
                long res = f[i][j]; // 不选 mask
                long res2 = f[i][j & ~mask] | (1L << i); // 选 mask
                f[i + 1][j] = Long.bitCount(res) < Long.bitCount(res2) ? res : res2;
            }
        }

        long res = f[n][u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        // f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
        long long f[n + 1][u];
        fill(f[0], f[0] + u, (1LL << n) - 1); // 对应记忆化搜索中的 if (i < 0) return all;
        f[0][0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask |= 1 << sid[s];
            f[i + 1][0] = 0;
            for (int j = 1; j < u; ++j) {
                auto r1 = f[i][j]; // 不选 mask
                auto r2 = f[i][j & ~mask] | (1L << i); // 选 mask
                f[i + 1][j] = __builtin_popcountll(r1) < __builtin_popcountll(r2) ? r1 : r2;
            }
        }
        auto res = f[n][u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol2-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    u, all := 1<<m, 1<<n-1
    // f[i+1][j] 表示从前 i 个集合中选择一些集合，并集等于 j，需要选择的最小集合
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, u)
    }
    for j := 1; j < u; j++ {
        f[0][j] = all // 对应记忆化搜索中的 if (i < 0) return all
    }
    for i, skills := range people {
        mask := 0
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask |= 1 << sid[s]
        }
        for j := 1; j < u; j++ {
            r1 := f[i][j] // 不选 mask[i]
            r2 := f[i][j&^mask] | 1<<i // 选 mask[i]
            if bits.OnesCount(uint(r1)) < bits.OnesCount(uint(r2)) {
                f[i+1][j] = r1
            } else {
                f[i+1][j] = r2
            }
        }
    }
    res := f[n][u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，递推的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此递推的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+n2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

## 三、空间优化

由于计算 $f[i+1]$ 只需要 $f[i]$，不需要下标更小的，所以只需要一个长为 $2^m$ 的数组。

实现时需要倒序循环 $j$，原理见 [0-1 背包与完全背包【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。

```py [sol3-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        u = 1 << len(req_skills)
        f = [(1 << n) - 1] * u
        f[0] = 0
        for i, skills in enumerate(people):
            mask = 0
            for s in skills:  # 把 skills 压缩成一个二进制数 mask
                mask |= 1 << sid[s]
            for j in range(u - 1, 0, -1):
                res = f[j]  # 不选 mask
                res2 = f[j & ~mask] | (1 << i)  # 选 mask
                f[j] = res if res.bit_count() < res2.bit_count() else res2
        res = f[-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol3-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        var f = new long[u];
        Arrays.fill(f, (1L << n) - 1);
        f[0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask
                mask |= 1 << sid.get(s);
            for (int j = u - 1; j > 0; --j) {
                long res = f[j]; // 不选 mask
                long res2 = f[j & ~mask] | (1L << i); // 选 mask
                f[j] = Long.bitCount(res) < Long.bitCount(res2) ? res : res2;
            }
        }

        long res = f[u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), u = 1 << m;
        long long f[u];
        fill(f, f + u, (1LL << n) - 1);
        f[0] = 0;
        for (int i = 0; i < n; ++i) {
            int mask = 0;
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask |= 1 << sid[s];
            for (int j = u - 1; j; --j) {
                auto r1 = f[j]; // 不选 mask
                auto r2 = f[j & ~mask] | (1L << i); // 选 mask
                f[j] = __builtin_popcountll(r1) < __builtin_popcountll(r2) ? r1 : r2;
            }
        }
        auto res = f[u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol3-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    u, all := 1<<m, 1<<n-1
    f := make([]int, u)
    for j := 1; j < u; j++ {
        f[j] = all // 对应记忆化搜索中的 if (i < 0) return all
    }
    for i, skills := range people {
        mask := 0
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask |= 1 << sid[s]
        }
        for j := u - 1; j > 0; j-- {
            r1 := f[j] // 不选 mask[i]
            r2 := f[j&^mask] | 1<<i // 选 mask[i]
            if bits.OnesCount(uint(r1)) > bits.OnesCount(uint(r2)) {
                f[j] = r2
            }
        }
    }
    res := f[u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，递推的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此递推的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

## 四、查表法 vs 刷表法

上面的做法算作查表法，意思是用其它状态更新当前状态。但是这种写法无法跳过无效的状态，在很多不必要的计算上浪费了大量时间。

我们还可以用当前状态去更新其它状态：从小到大遍历每个 $f[j]$，然后遍历 $\textit{mask}$，用 $f[j]$ 去更新 $f[j|\textit{mask}[i]]$。这种做法叫刷表法。

由于我们是从小到大遍历 $j$，并且更新也是更新到比 $j$ 更大的数上，如果 $f[j]$ 等于其初始值，说明它没有被更新过，也就说明 $j$ 无法由若干集合的并集得到，是无效状态，可以直接跳过。

```py [sol4-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        mask = [0] * n
        for i, skills in enumerate(people):
            for s in skills:  # 把 skills 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s]

        ALL = (1 << n) - 1
        u = 1 << len(req_skills)
        f = [ALL] * u  # f[j] 表示并集为 j 要选的最小 people 集合
        f[0] = 0
        for j in range(u - 1):  # f[u-1] 无需计算
            if f[j] == ALL: continue  # 无法更新其它状态，直接跳过
            for i, msk in enumerate(mask):
                if f[j].bit_count() + 1 < f[j | msk].bit_count():
                    f[j | msk] = f[j] | (1 << i)  # 刷表：用 f[j] 去更新其它状态

        res = f[-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol4-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size();
        var mask = new int[n];
        for (int i = 0; i < n; ++i)
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid.get(s);

        long all = (1L << n) - 1;
        int u = 1 << m;
        var f = new long[u]; // f[j] 表示并集为 j 至少要选的 people 集合
        Arrays.fill(f, all);
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < all)
                for (int i = 0; i < n; ++i)
                    if (Long.bitCount(f[j]) + 1 < Long.bitCount(f[j | mask[i]]))
                        f[j | mask[i]] = f[j] | (1L << i); // 刷表：用 f[j] 去更新其它状态

        long res = f[u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol4-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), mask[n];
        memset(mask, 0, sizeof(mask));
        for (int i = 0; i < n; ++i)
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s];

        int u = 1 << m;
        long long all = (1LL << n) - 1, f[u];
        fill(f, f + u, all);
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < all)
                for (int i = 0; i < n; ++i)
                    if (__builtin_popcountll(f[j]) + 1 < __builtin_popcountll(f[j | mask[i]]))
                        f[j | mask[i]] = f[j] | (1LL << i); // 刷表：用 f[j] 去更新其它状态
        auto res = f[u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol4-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    mask := make([]int, n)
    for i, skills := range people {
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask[i] |= 1 << sid[s]
        }
    }

    u, all := 1<<m, 1<<n-1
    f := make([]int, u)
    for j := 1; j < u; j++ {
        f[j] = all // 对应记忆化搜索中的 if (i < 0) return all
    }
    for j, fj := range f {
        if fj < all {
            for i, msk := range mask {
                if bits.OnesCount(uint(fj))+1 < bits.OnesCount(uint(f[j|msk])) {
                    f[j|msk] = fj | 1<<i // 刷表：用 f[j] 去更新其它状态
                }
            }
        }
    }
    res := f[u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 优化

由于计算二进制中 $1$ 的个数也比较耗时，新开一个数组 $\textit{ids}$ 单独记录压缩后的下标集合。这样 $f[j]$ 的含义就是并集为 $j$ 最少要选的集合个数了。

```py [sol5-Python3]
class Solution:
    def smallestSufficientTeam(self, req_skills: List[str], people: List[List[str]]) -> List[int]:
        sid = {s: i for i, s in enumerate(req_skills)}  # 字符串映射到下标
        n = len(people)
        mask = [0] * n
        for i, skills in enumerate(people):
            for s in skills:  # 把 skills 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s]

        u = 1 << len(req_skills)
        ids = [0] * u  # ids[j] 表示 f[j] 对应的 people 下标集合
        f = [inf] * u  # f[j] 表示并集为 j 至少要选的 people 个数
        f[0] = 0
        for j in range(u - 1):  # f[u-1] 无需计算
            if f[j] == inf: continue  # 无法更新其它状态，直接跳过
            for i, msk in enumerate(mask):
                if f[j] + 1 < f[j | msk]:
                    f[j | msk] = f[j] + 1  # 刷表：用 f[j] 去更新其它状态
                    ids[j | msk] = ids[j] | (1 << i)

        res = ids[-1]
        return [i for i in range(n) if (res >> i) & 1]  # 所有在 res 中的下标
```

```java [sol5-Java]
class Solution {
    public int[] smallestSufficientTeam(String[] reqSkills, List<List<String>> people) {
        var sid = new HashMap<String, Integer>();
        int m = reqSkills.length;
        for (int i = 0; i < m; ++i)
            sid.put(reqSkills[i], i); // 字符串映射到下标

        int n = people.size();
        var mask = new int[n];
        for (int i = 0; i < n; ++i)
            for (var s : people.get(i)) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid.get(s);

        int u = 1 << m;
        var ids = new long[u]; // ids[j] 表示 f[j] 对应的 people 下标集合
        var f = new int[u]; // f[j] 表示并集为 j 至少要选的 people 个数
        Arrays.fill(f, Integer.MAX_VALUE);
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < Integer.MAX_VALUE)
                for (int i = 0; i < n; ++i)
                    if (f[j] + 1 < f[j | mask[i]]) {
                        f[j | mask[i]] = f[j] + 1; // 刷表：用 f[j] 去更新其它状态
                        ids[j | mask[i]] = ids[j] | (1L << i);
                    }

        long res = ids[u - 1];
        var ans = new int[Long.bitCount(res)];
        for (int i = 0, j = 0; i < n; ++i)
            if (((res >> i) & 1) > 0)
                ans[j++] = i; // 所有在 res 中的下标
        return ans;
    }
}
```

```cpp [sol5-C++]
class Solution {
public:
    vector<int> smallestSufficientTeam(vector<string> &req_skills, vector<vector<string>> &people) {
        unordered_map<string, int> sid;
        int m = req_skills.size();
        for (int i = 0; i < m; ++i)
            sid[req_skills[i]] = i; // 字符串映射到下标

        int n = people.size(), mask[n];
        memset(mask, 0, sizeof(mask));
        for (int i = 0; i < n; ++i)
            for (auto &s: people[i]) // 把 people[i] 压缩成一个二进制数 mask[i]
                mask[i] |= 1 << sid[s];

        int u = 1 << m;
        long long ids[u]; memset(ids, 0, sizeof(ids));
        char f[u]; memset(f, 0x7f, sizeof(f));
        f[0] = 0;
        for (int j = 0; j < u - 1; ++j) // f[u-1] 无需计算
            if (f[j] < 0x7f)
                for (int i = 0; i < n; ++i)
                    if (f[j] + 1 < f[j | mask[i]]) {
                        f[j | mask[i]] = f[j] + 1; // 刷表：用 f[j] 去更新其它状态
                        ids[j | mask[i]] = ids[j] | (1LL << i);
                    }
        auto res = ids[u - 1];

        vector<int> ans;
        for (int i = 0; i < n; ++i)
            if ((res >> i) & 1)
                ans.push_back(i); // 所有在 res 中的下标
        return ans;
    }
};
```

```go [sol5-Go]
func smallestSufficientTeam(reqSkills []string, people [][]string) (ans []int) {
    m := len(reqSkills)
    sid := make(map[string]int, m)
    for i, s := range reqSkills {
        sid[s] = i // 字符串映射到下标
    }

    n := len(people)
    mask := make([]int, n)
    for i, skills := range people {
        for _, s := range skills { // 把 skills 压缩成一个二进制数 mask[i]
            mask[i] |= 1 << sid[s]
        }
    }

    u := 1 << m
    ids := make([]int, u)
    f := make([]int8, u)
    for j := 1; j < u; j++ {
        f[j] = math.MaxInt8
    }
    for j, fj := range f {
        if fj < math.MaxInt8 {
            for i, msk := range mask {
                if fj+1 < f[j|msk] {
                    f[j|msk] = fj + 1 // 刷表：用 f[j] 去更新其它状态
                    ids[j|msk] = ids[j] | 1<<i
                }
            }
        }
    }
    res := ids[u-1]

    for i := 0; i < n; i++ {
        if res>>i&1 > 0 {
            ans = append(ans, i) // 所有在 res 中的下标
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(T+n2^m)$，其中 $T$ 为 $\textit{people}$ 中的字符串的长度之和，$n$ 为 $\textit{people}$ 的长度，$m$ 为 $\textit{reqSkills}$ 的长度。忽略比较字符串的时间。初始化数组 $\textit{mask}$ 需要 $O(T)$ 的时间。由于每个状态只会计算一次，递推的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数为 $O(n2^m)$，单个状态的计算时间为 $O(1)$，因此递推的时间复杂度为 $O(n2^m)$。
- 空间复杂度：$O(S+n+2^m)$，其中 $S$ 为 $\textit{reqSkills}$ 中的字符串的长度之和。

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

本题来自 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
