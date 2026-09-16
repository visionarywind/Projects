# 1177. 构建回文串检测

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/can-make-palindrome-from-substring/
- 题目 slug：`can-make-palindrome-from-substring`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.4 状态压缩前缀和
- 难度分：1848
- 外部题解来源：https://leetcode.cn/problems/can-make-palindrome-from-substring/solutions/2309725/yi-bu-bu-you-hua-cong-qian-zhui-he-dao-q-yh5p/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一步步优化！从前缀和到前缀异或和（附题单！）](https://leetcode.cn/problems/can-make-palindrome-from-substring/solutions/2309725/yi-bu-bu-you-hua-cong-qian-zhui-he-dao-q-yh5p/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-bu-bu-you-hua-cong-qian-zhui-he-dao-q-yh5p`
- topic id：`2309725`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、思考

回文意味着什么？

题目允许重新排列字母，那么以字母 $\texttt{a}$ 为例，它要如何排列？偶数个 $\texttt{a}$ 要如何排列？奇数个 $\texttt{a}$ 要如何排列？

什么情况下**一定**要替换字母？要替换多少个？

如何快速求出子串中每种字母的个数？

## 二、解惑

回文意味着从左往右第 $i$ 个字母和从右往左第 $i$ 个字母是相同的。（回文串关于回文中心是对称的。）

如果有偶数个 $\texttt{a}$，那么可以均分成两部分，分别放置在字符串的中心对称位置上。例如有 $4$ 个 $\texttt{a}$，可以在字符串的最左边放置 $2$ 个 $\texttt{a}$，最右边放置 $2$ 个 $\texttt{a}$，这样字符串中的 $\texttt{a}$ 是回文的。其它字母如果出现偶数次，也同理。

如果有奇数个 $\texttt{a}$，多出的一个 $\texttt{a}$ 要单独拿出来讨论：

- 假如只有 $\texttt{a}$ 出现奇数次，其它字母都出现偶数次。此时字符串的长度一定是奇数，那么可以把多出的这个 $\texttt{a}$ 放在字符串的中心，我们仍然可以得到一个回文串，无需替换任何字母。
- 如果有两种字母出现奇数次（假设是字母 $\texttt{a},\texttt{b}$），由于多出的一个 $\texttt{a}$ 和一个 $\texttt{b}$ 无法组成回文串，可以把一个 $\texttt{b}$ 改成 $\texttt{a}$（或者把一个 $\texttt{a}$ 改成 $\texttt{b}$），这样 $\texttt{a}$ 和 $\texttt{b}$ 就都出现偶数次了。
- 如果有三种字母出现奇数次（假设是字母 $\texttt{a},\texttt{b},\texttt{c}$），把一个 $\texttt{b}$ 改成 $\texttt{c}$，就转换成只有 $\texttt{a}$ 出现奇数次的情况了。
- 一般地，如果有 $m$ 种字母出现奇数次，只需修改其中 $\left\lfloor\dfrac{m}{2}\right\rfloor$ 个字母。换句话说，如果第 $i$ 次询问有 $\left\lfloor\dfrac{m}{2}\right\rfloor\le k$，那么 $\textit{answer}[i]$ 为真，反之为假。

最后要解决的问题是，如何快速求出子串中每种字母的个数？

可以创建 $26$ 个**前缀和**数组，分别统计每种字母。以字母 $\texttt{a}$ 为例，在计算前缀和时，如果 $s[i]=\texttt{a}$ 就视作 $1$，否则视作 $0$。

> 关于前缀和的讲解，见本文末尾。

## 三、算法（优化前）

1. 预处理 $s$ 的长为 $i$ 的前缀中，每种字母各出现多少次。为方便后续优化，这里用 $n\times 26$ 的二维数组 $\textit{sum}$ 存储前缀和，其中 $\textit{sum}[i+1][j]$ 表示从 $s[0]$ 到 $s[i]$ 中，字母表的第 $j$ 个小写字母的出现次数。
2. 对于 $\textit{queries}[i]$，利用前缀和计算出每种字母出现次数，统计有多少种字母出现奇数次，设为 $m$。如果 $\left\lfloor\dfrac{m}{2}\right\rfloor\le k$，那么 $\textit{answer}[i]$ 为真，反之为假。

```py [sol-Python3]
class Solution:
    def canMakePaliQueries(self, s: str, queries: List[List[int]]) -> List[bool]:
        sum = [[0] * 26]
        for c in s:
            sum.append(sum[-1].copy())
            sum[-1][ord(c) - ord('a')] += 1

        ans = []
        for left, right, k in queries:
            m = 0
            for sl, sr in zip(sum[left], sum[right + 1]):
                m += (sr - sl) % 2  # 奇数+1，偶数+0
            ans.append(m // 2 <= k)
        return ans
```

```java [sol-Java]
class Solution {
    public List<Boolean> canMakePaliQueries(String s, int[][] queries) {
        int n = s.length();
        var sum = new int[n + 1][26];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i].clone();
            sum[i + 1][s.charAt(i) - 'a']++;
        }

        var ans = new ArrayList<Boolean>(queries.length); // 预分配空间
        for (var q : queries) {
            int left = q[0], right = q[1], k = q[2], m = 0;
            for (int j = 0; j < 26; j++)
                m += (sum[right + 1][j] - sum[left][j]) % 2; // 奇数+1，偶数+0
            ans.add(m / 2 <= k);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> canMakePaliQueries(string s, vector<vector<int>> &queries) {
        int n = s.length(), q = queries.size();
        vector<array<int, 26>> sum(n + 1);
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i];
            sum[i + 1][s[i] - 'a']++;
        }

        vector<bool> ans(q);
        for (int i = 0; i < q; i++) {
            auto &query = queries[i];
            int left = query[0], right = query[1], k = query[2], m = 0;
            for (int j = 0; j < 26; j++)
                m += (sum[right + 1][j] - sum[left][j]) % 2; // 奇数+1，偶数+0
            ans[i] = m / 2 <= k;
        }
        return ans;
    }
};
```

```go [sol-Go]
func canMakePaliQueries(s string, queries [][]int) []bool {
    sum := make([][26]int, len(s)+1)
    for i, c := range s {
        sum[i+1] = sum[i]
        sum[i+1][c-'a']++
    }

    ans := make([]bool, len(queries))
    for i, q := range queries {
        left, right, k, m := q[0], q[1], q[2], 0
        for j := 0; j < 26; j++ {
            m += (sum[right+1][j] - sum[left][j]) % 2 // 奇数+1，偶数+0
        }
        ans[i] = m/2 <= k
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+q)\cdot|\Sigma|)$，其中 $n$ 为 $s$ 的长度，$q$ 为 $\textit{queries}$ 的长度，$|\Sigma|$ 为字符集合的大小。本题中字符均为小写字母，所以 $|\Sigma|=26$。
- 空间复杂度：$\mathcal{O}(n\cdot|\Sigma|)$。返回值不计入。

## 四、一步步优化

由于只关心每种字母出现次数的奇偶性，所以不需要在前缀和中存储每种字母的出现次数，只需要保存每种字母出现次数的奇偶性。

为方便计算，用 $0$ 表示出现偶数次，用 $1$ 表示出现奇数次。

注意只有奇数减偶数，或者偶数减奇数，才能得到奇数。所以如果相减的结果不为 $0$（或者说相减的两个数不相等），就表示出现了奇数次。

```py [sol-Python3]
class Solution:
    def canMakePaliQueries(self, s: str, queries: List[List[int]]) -> List[bool]:
        sum = [[0] * 26]
        for c in s:
            sum.append(sum[-1].copy())
            sum[-1][ord(c) - ord('a')] += 1
            sum[-1][ord(c) - ord('a')] %= 2  # 偶数是 0

        ans = []
        for left, right, k in queries:
            m = 0
            for sl, sr in zip(sum[left], sum[right + 1]):
                m += sr != sl
            ans.append(m // 2 <= k)
        return ans
```

```java [sol-Java]
class Solution {
    public List<Boolean> canMakePaliQueries(String s, int[][] queries) {
        int n = s.length();
        var sum = new int[n + 1][26];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i].clone();
            sum[i + 1][s.charAt(i) - 'a']++;
            sum[i + 1][s.charAt(i) - 'a'] %= 2; // 偶数是 0
        }

        var ans = new ArrayList<Boolean>(queries.length); // 预分配空间
        for (var q : queries) {
            int left = q[0], right = q[1], k = q[2], m = 0;
            for (int j = 0; j < 26; j++)
                m += (sum[right + 1][j] != sum[left][j] ? 1 : 0);
            ans.add(m / 2 <= k);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> canMakePaliQueries(string s, vector<vector<int>> &queries) {
        int n = s.length(), q = queries.size();
        vector<array<int, 26>> sum(n + 1);
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i];
            sum[i + 1][s[i] - 'a']++;
            sum[i + 1][s[i] - 'a'] %= 2; // 偶数是 0
        }

        vector<bool> ans(q);
        for (int i = 0; i < q; i++) {
            auto &query = queries[i];
            int left = query[0], right = query[1], k = query[2], m = 0;
            for (int j = 0; j < 26; j++)
                m += sum[right + 1][j] != sum[left][j];
            ans[i] = m / 2 <= k;
        }
        return ans;
    }
};
```

```go [sol-Go]
func canMakePaliQueries(s string, queries [][]int) []bool {
    sum := make([][26]int, len(s)+1)
    for i, c := range s {
        sum[i+1] = sum[i]
        sum[i+1][c-'a']++
        sum[i+1][c-'a'] %= 2 // 偶数是 0
    }

    ans := make([]bool, len(queries))
    for i, q := range queries {
        left, right, k, m := q[0], q[1], q[2], 0
        for j := 0; j < 26; j++ {
            if sum[right+1][j] != sum[left][j] {
                m++
            }
        }
        ans[i] = m/2 <= k
    }
    return ans
}
```

由于异或运算满足 $1$ 和 $0$ 的结果是 $1$，而 $0$ 和 $0$，以及 $1$ 和 $1$ 的结果都是 $0$，所以可以用异或替换上面的减法。

```py [sol-Python3]
class Solution:
    def canMakePaliQueries(self, s: str, queries: List[List[int]]) -> List[bool]:
        sum = [[0] * 26]
        for c in s:
            sum.append(sum[-1].copy())
            sum[-1][ord(c) - ord('a')] ^= 1  # 奇数变偶数，偶数变奇数

        ans = []
        for left, right, k in queries:
            m = 0
            for sl, sr in zip(sum[left], sum[right + 1]):
                m += sr ^ sl
            ans.append(m // 2 <= k)
        return ans
```

```java [sol-Java]
class Solution {
    public List<Boolean> canMakePaliQueries(String s, int[][] queries) {
        int n = s.length();
        var sum = new int[n + 1][26];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i].clone();
            sum[i + 1][s.charAt(i) - 'a'] ^= 1; // 奇数变偶数，偶数变奇数
        }

        var ans = new ArrayList<Boolean>(queries.length); // 预分配空间
        for (var q : queries) {
            int left = q[0], right = q[1], k = q[2], m = 0;
            for (int j = 0; j < 26; j++)
                m += sum[right + 1][j] ^ sum[left][j];
            ans.add(m / 2 <= k);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> canMakePaliQueries(string s, vector<vector<int>> &queries) {
        int n = s.length(), q = queries.size();
        vector<array<int, 26>> sum(n + 1);
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i];
            sum[i + 1][s[i] - 'a'] ^= 1; // 奇数变偶数，偶数变奇数
        }

        vector<bool> ans(q);
        for (int i = 0; i < q; i++) {
            auto &query = queries[i];
            int left = query[0], right = query[1], k = query[2], m = 0;
            for (int j = 0; j < 26; j++)
                m += sum[right + 1][j] ^ sum[left][j];
            ans[i] = m / 2 <= k;
        }
        return ans;
    }
};
```

```go [sol-Go]
func canMakePaliQueries(s string, queries [][]int) []bool {
    sum := make([][26]int, len(s)+1)
    for i, c := range s {
        sum[i+1] = sum[i]
        sum[i+1][c-'a'] ^= 1 // 奇数变偶数，偶数变奇数
    }

    ans := make([]bool, len(queries))
    for i, q := range queries {
        left, right, k, m := q[0], q[1], q[2], 0
        for j := 0; j < 26; j++ {
            m += sum[right+1][j] ^ sum[left][j]
        }
        ans[i] = m/2 <= k
    }
    return ans
}
```

由于长为 $26$ 的数组中只存储 $0$ 和 $1$，可以压缩到一个二进制数中，二进制数从低到高第 $i$ 个比特存储着 $0$ 和 $1$ 的信息。

例如二进制 $10010$ 表示 $\texttt{b}$ 和 $\texttt{e}$ 出现奇数次，其余字母出现偶数次。

在计算前缀和时（准确地说是异或前缀和）：

- 修改 $\texttt{a}$ 出现次数的奇偶性，可以异或二进制 $1$；
- 修改 $\texttt{b}$ 出现次数的奇偶性，可以异或二进制 $10$；
- 修改 $\texttt{c}$ 出现次数的奇偶性，可以异或二进制 $100$；
- 依此类推。

此外，由于异或可以「并行计算」，对前缀和中的两个二进制数直接异或，便得到了子串中每种字母出现次数的奇偶性。再计算这个二进制数中的 $1$ 的个数，便得到了 $m$。

例如 $10010 \oplus 01110 = 11100$，说明有 $3$ 种字母出现奇数次。

```py [sol-Python3]
class Solution:
    def canMakePaliQueries(self, s: str, queries: List[List[int]]) -> List[bool]:
        sum = [0]
        for c in s:
            bit = 1 << (ord(c) - ord('a'))
            sum.append(sum[-1] ^ bit)  # 该比特对应字母的奇偶性：奇数变偶数，偶数变奇数

        ans = []
        for left, right, k in queries:
            m = (sum[left] ^ sum[right + 1]).bit_count()
            ans.append(m // 2 <= k)
        return ans
```

```java [sol-Java]
class Solution {
    public List<Boolean> canMakePaliQueries(String s, int[][] queries) {
        int n = s.length();
        var sum = new int[n + 1];
        for (int i = 0; i < n; i++) {
            int bit = 1 << (s.charAt(i) - 'a');
            sum[i + 1] = sum[i] ^ bit; // 该比特对应字母的奇偶性：奇数变偶数，偶数变奇数
        }

        var ans = new ArrayList<Boolean>(queries.length); // 预分配空间
        for (var q : queries) {
            int left = q[0], right = q[1], k = q[2];
            int m = Integer.bitCount(sum[right + 1] ^ sum[left]);
            ans.add(m / 2 <= k);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> canMakePaliQueries(string s, vector<vector<int>> &queries) {
        int n = s.length(), q = queries.size(), sum[n + 1];
        sum[0] = 0;
        for (int i = 0; i < n; i++) {
            int bit = 1 << (s[i] - 'a');
            sum[i + 1] = sum[i] ^ bit; // 该比特对应字母的奇偶性：奇数变偶数，偶数变奇数
        }

        vector<bool> ans(q); // 预分配空间
        for (int i = 0; i < q; i++) {
            auto &query = queries[i];
            int left = query[0], right = query[1], k = query[2];
            int m = __builtin_popcount(sum[right + 1] ^ sum[left]);
            ans[i] = m / 2 <= k;
        }
        return ans;
    }
};
```

```go [sol-Go]
func canMakePaliQueries(s string, queries [][]int) []bool {
    sum := make([]uint32, len(s)+1) // 节省一半空间
    for i, c := range s {
        bit := uint32(1) << (c - 'a')
        sum[i+1] = sum[i] ^ bit // 该比特对应字母的奇偶性：奇数变偶数，偶数变奇数
    }

    ans := make([]bool, len(queries))
    for i, q := range queries {
        left, right, k := q[0], q[1], q[2]
        m := bits.OnesCount32(sum[right+1] ^ sum[left])
        ans[i] = m/2 <= k
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+q)$，其中 $n$ 为 $\textit{nums}$ 的长度，$q$ 为 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

#### 答疑

**问**：怎么想到异或的？

**答**：异或可以视作「不进位加法（减法）」或者「模 $2$ 剩余系中的加法（减法）」，所以也常常用来解决一些和奇偶性有关的问题。

#### 相似题目（前缀和+异或）

- [1371. 每个元音包含偶数次的最长子字符串](https://leetcode.cn/problems/find-the-longest-substring-containing-vowels-in-even-counts/)
- [1542. 找出最长的超赞子字符串](https://leetcode.cn/problems/find-longest-awesome-substring/)
- [1915. 最美子字符串的数目](https://leetcode.cn/problems/number-of-wonderful-substrings/)，[题解](https://leetcode.cn/problems/number-of-wonderful-substrings/solution/qian-zhui-he-chang-jian-ji-qiao-by-endle-t57t/)

更多前缀和题目：

- [560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/)
- [974. 和可被 K 整除的子数组](https://leetcode.cn/problems/subarray-sums-divisible-by-k/)
- [1590. 使数组和能被 P 整除](https://leetcode.cn/problems/make-sum-divisible-by-p/)
- [523. 连续的子数组和](https://leetcode.cn/problems/continuous-subarray-sum/)
- [525. 连续数组](https://leetcode.cn/problems/contiguous-array/)

[往期每日一题题解（按 tag 分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

#### 附：前缀和

对于数组 $\textit{nums}$，定义它的前缀和 $\textit{s}[0]=0$，$\textit{s}[i+1] = \sum\limits_{j=0}^{i}\textit{nums}[j]$。

根据这个定义，有 $s[i+1]=s[i]+\textit{nums}[i]$。

例如 $\textit{nums}=[1,2,1,2]$，对应的前缀和数组为 $s=[0,1,3,4,6]$。

通过前缀和，我们可以把**子数组的元素和转换成两个前缀和的差**，即

$$
\sum_{j=\textit{left}}^{\textit{right}}\textit{nums}[j] = \sum\limits_{j=0}^{\textit{right}}\textit{nums}[j] - \sum\limits_{j=0}^{\textit{left}-1}\textit{nums}[j] = \textit{s}[\textit{right}+1] - \textit{s}[\textit{left}]
$$

例如 $\textit{nums}$ 的子数组 $[2,1,2]$ 的和就可以用 $s[4]-s[1]=6-1=5$ 算出来。

> 注：为方便计算，常用左闭右开区间 $[\textit{left},\textit{right})$ 来表示从 $\textit{nums}[\textit{left}]$ 到 $\textit{nums}[\textit{right}-1]$ 的子数组，此时子数组的和为 $\textit{s}[\textit{right}] - \textit{s}[\textit{left}]$，子数组的长度为 $\textit{right}-\textit{left}$。
>
> 注 2：$s[0]=0$ 表示一个空数组的元素和。为什么要额外定义它？想一想，如果要计算的子数组恰好是一个前缀（从 $\textit{nums}[0]$ 开始），你要用 $s[\textit{right}]$ 减去谁呢？通过定义 $s[0]=0$，任意子数组（包括前缀）都可以表示为两个前缀和的差。

---

欢迎点赞！关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.4 状态压缩前缀和`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.4 状态压缩前缀和`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
