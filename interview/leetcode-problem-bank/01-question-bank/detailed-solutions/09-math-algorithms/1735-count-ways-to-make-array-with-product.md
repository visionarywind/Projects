# 1735. 生成乘积数组的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-ways-to-make-array-with-product/
- 题目 slug：`count-ways-to-make-array-with-product`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.3 质因数分解
- 难度分：2500
- 外部题解来源：https://leetcode.cn/problems/count-ways-to-make-array-with-product/solutions/2713481/tu-jie-zhi-yin-zi-fen-jie-fang-qiu-wen-t-fboo/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】质因数分解+放球问题（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/count-ways-to-make-array-with-product/solutions/2713481/tu-jie-zhi-yin-zi-fen-jie-fang-qiu-wen-t-fboo/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-zhi-yin-zi-fen-jie-fang-qiu-wen-t-fboo`
- topic id：`2713481`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

![LC1735.png](https://pic.leetcode.cn/1709650044-xoCYiR-LC1735.png)

如何编程实现？首先讲讲简单的做法，更高效的做法见后面的「写法二」。

## 如何分解质因子

从 $i=2$ 开始枚举，如果 $k$ 能被 $i$ 整除就不断除 $i$，直到 $k$ 不能被 $i$ 整除为止，统计除 $i$ 的次数，作为公式中的 $e$。

什么时候停止枚举呢？如果 $i^2 > k$，继续向后枚举是不会出现 $k$ 被 $i$ 整除的情况的。这可以用反证法证明：假设存在 $i$，满足 $i^2>k$ 且 $k$ 能被 $i$ 整除，那么 $k$ 也能被 $\dfrac{k}{i}$ 整除，注意到 $\dfrac{k}{i}<i$，但我们已经处理完小于 $i$ 的因子了，不会出现 $k$ 仍可以被一个小于 $i$ 的数整除的情况，矛盾。所以当 $i^2 > k$ 时可以停止枚举。

循环结束后，如果 $k>1$，说明还有一个质因子为 $k$。

## 如何计算组合数

对于从 $n$ 个物品中选择 $m$ 个物品的方案数 $C(n,m)$，可以用「选或不选」来思考，对于第 $n$ 个物品：

- 不选：问题变成从 $n-1$ 个物品中选择 $m$ 个物品的方案数 $C(n-1,m)$。
- 选：问题变成从 $n-1$ 个物品中选择 $m-1$ 个物品的方案数 $C(n-1,m-1)$。

所以 $C(n,m) = C(n-1,m) + C(n-1,m-1)$。

初始值：$C(n,0) = 1$。

对于本题，由于 $2^{13} < 10^4 < 2^{14}$，我们可以预处理 $n\le 10^4 + 13-1$ 和 $m \le 13$ 的组合数。

```py [sol-Python3]
MOD = 1_000_000_007

# 预处理组合数
c = [[0] * 14 for _ in range(10013)]
c[0][0] = 1
for i in range(1, 10013):
    c[i][0] = 1
    for j in range(1, 14):
        c[i][j] = (c[i - 1][j] + c[i - 1][j - 1]) % MOD

class Solution:
    def waysToFillArray(self, queries: List[List[int]]) -> List[int]:
        ans = []
        for n, k in queries:
            res = 1
            i = 2
            while i * i <= k:
                if k % i == 0:  # i 是 k 的质因子
                    e = 0
                    while k % i == 0:
                        e += 1  # 统计有多少个质因子 i
                        k //= i
                    res = res * c[e + n - 1][e] % MOD
                i += 1
            if k > 1:  # 还剩下一个质因子
                res = res * n % MOD
            ans.append(res)
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int[][] C = new int[10013][14];

    static {
        // 预处理组合数
        C[0][0] = 1;
        for (int i = 1; i < 10013; i++) {
            C[i][0] = 1;
            for (int j = 1; j < 14; j++) {
                C[i][j] = (C[i - 1][j] + C[i - 1][j - 1]) % MOD;
            }
        }
    }

    public int[] waysToFillArray(int[][] queries) {
        int[] ans = new int[queries.length];
        for (int idx = 0; idx < queries.length; idx++) {
            int[] q = queries[idx];
            int n = q[0];
            int k = q[1];
            long res = 1;
            for (int i = 2; i * i <= k; i++) {
                if (k % i == 0) { // i 是 k 的质因子
                    int e = 1;
                    for (k /= i; k % i == 0; k /= i) {
                        e++; // 统计有多少个质因子 i
                    }
                    res = res * C[e + n - 1][e] % MOD;
                }
            }
            if (k > 1) { // 还剩下一个质因子
                res = res * n % MOD;
            }
            ans[idx] = (int) res;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
int C[10013][14];

// 预处理组合数
auto init = [] {
    C[0][0] = 1;
    for (int i = 1; i < 10013; i++) {
        C[i][0] = 1;
        for (int j = 1; j < 14; j++) {
            C[i][j] = (C[i - 1][j] + C[i - 1][j - 1]) % MOD;
        }
    }
    return 0;
}();

class Solution {
public:
    vector<int> waysToFillArray(vector<vector<int>> &queries) {
        vector<int> ans;
        for (auto &q : queries) {
            int n = q[0], k = q[1];
            long long res = 1;
            for (int i = 2; i * i <= k; i++) {
                if (k % i == 0) { // i 是 k 的质因子
                    int e = 1;
                    for (k /= i; k % i == 0; k /= i) {
                        e++; // 统计有多少个质因子 i
                    }
                    res = res * C[e + n - 1][e] % MOD;
                }
            }
            if (k > 1) { // 还剩下一个质因子
                res = res * n % MOD;
            }
            ans.push_back(res);
        }
        return ans;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

var c [10013][14]int

// 预处理组合数
func init() {
    c[0][0] = 1
    for i := 1; i < len(c); i++ {
        c[i][0] = 1
        for j := 1; j < len(c[i]); j++ {
            c[i][j] = (c[i-1][j] + c[i-1][j-1]) % mod
        }
    }
}

func waysToFillArray(queries [][]int) []int {
    ans := make([]int, len(queries))
    for idx, q := range queries {
        n, k := q[0], q[1]
        res := 1
        for i := 2; i*i <= k; i++ {
            if k%i == 0 { // i 是 k 的质因子
                e := 1
                for k /= i; k%i == 0; k /= i {
                    e++ // 统计有多少个质因子 i
                }
                res = res * c[e+n-1][e] % mod
            }
        }
        if k > 1 { // 还剩下一个质因子
            res = res * n % mod
        }
        ans[idx] = res
    }
    return ans
}
```

```js [sol-JavaScript]
const MOD = 1_000_000_007n;

// 预处理组合数
const c = Array.from({length: 10013}, () => Array(14).fill(0n));
c[0][0] = 1n;
for (let i = 1; i < 10013; i++) {
    c[i][0] = 1n;
    for (let j = 1; j < 14; j++) {
        c[i][j] = (c[i - 1][j] + c[i - 1][j - 1]) % MOD;
    }
}

var waysToFillArray = function(queries) {
    const ans = Array(queries.length);
    for (let i = 0; i < queries.length; i++) {
        let [n, k] = queries[i];
        let res = 1n;
        for (let i = 2; i * i <= k; i++) {
            if (k % i === 0) { // i 是 k 的质因子
                let e = 1;
                for (k /= i; k % i === 0; k /= i) {
                    e++; // 统计有多少个质因子 i
                }
                res = res * c[e + n - 1][e] % MOD;
            }
        }
        if (k > 1) { // 还剩下一个质因子
            res = res * BigInt(n) % MOD;
        }
        ans[i] = Number(res);
    }
    return ans;
};
```

```rust [sol-Rust]
const MOD: i64 = 1_000_000_007;
static mut initialized: bool = false;
static mut c: [[i64; 14]; 10013] = [[0; 14]; 10013];

// 预处理组合数
unsafe fn init_once() {
    if initialized {
        return;
    }
    initialized = true;
    c[0][0] = 1;
    for i in 1..10013 {
        c[i][0] = 1;
        for j in 1..14 {
            c[i][j] = (c[i - 1][j] + c[i - 1][j - 1]) % MOD;
        }
    }
}

impl Solution {
    pub fn ways_to_fill_array(queries: Vec<Vec<i32>>) -> Vec<i32> {
        unsafe { init_once(); }
        let mut ans = Vec::with_capacity(queries.len());
        for q in queries {
            let n = q[0] as usize;
            let mut k = q[1];
            let mut res = 1i64;
            let mut i = 2;
            while i * i <= k {
                if k % i == 0 { // i 是 k 的质因子
                    let mut e = 0;
                    while k % i == 0 {
                        e += 1; // 统计有多少个质因子 i
                        k /= i;
                    }
                    unsafe { res = res * c[e + n - 1][e] % MOD; }
                }
                i += 1;
            }
            if k > 1 { // 还剩下一个质因子
                res = res * n as i64 % MOD;
            }
            ans.push(res as i32);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：预处理的时间为 $\mathcal{O}((N+\log K)\log K)$，其中 $N=10^4, K=10^4$。回答单个询问的时间为 $\mathcal{O}(\sqrt k)$。
- 空间复杂度：$\mathcal{O}((N+\log K)\log K)$。返回值不计入。

## 写法二

竞赛算法，供学有余力的同学参考。

优化 1：预处理阶乘及其逆元后，利用公式 $C(n,m) = \dfrac{n!}{m!(n-m)!}$ 计算组合数。关于除法取模的计算方法，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

优化 2：预处理每个数的最小质因子（LPF，least prime factor），从而快速分解质因数。这可以用埃氏筛或者欧拉筛来做，见 [筛法](https://oi-wiki.org/math/number-theory/sieve/)。下面代码用的埃氏筛。

```py [sol-Python3]
MOD = 1_000_000_007
MX = 10013
MX_K = 10001

f = [0] * MX  # f[i] = i!
f[0] = 1
for i in range(1, MX):
    f[i] = f[i - 1] * i % MOD

inv_f = [0] * MX  # inv_f[i] = i!^-1
inv_f[-1] = pow(f[-1], -1, MOD)
for i in range(MX - 1, 0, -1):
    inv_f[i - 1] = inv_f[i] * i % MOD

lpf = [0] * MX_K  # i 的最小质因子是 lpf[i]
for i in range(2, MX_K):
    if lpf[i] == 0:  # i 是质数
        for j in range(i, MX_K, i):
            if lpf[j] == 0:
                lpf[j] = i  # j 的最小质因子是 i

def comb(n: int, m: int) -> int:
    return f[n] * inv_f[m] * inv_f[n - m] % MOD

class Solution:
    def waysToFillArray(self, queries: List[List[int]]) -> List[int]:
        ans = []
        for n, k in queries:
            res = 1
            while k > 1:
                p = lpf[k]
                e = 0
                while k % p == 0:
                    e += 1
                    k //= p
                res = res * comb(e + n - 1, e) % MOD
            ans.append(res)
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 10013;
    private static final int MX_K = 10001;

    private static final long[] F = new long[MX]; // f[i] = i!
    private static final long[] INV_F = new long[MX]; // inv_f[i] = i!^-1
    private static final int[] LPF = new int[MX_K]; // i 的最小质因子是 lpf[i]

    static {
        F[0] = 1;
        for (int i = 1; i < MX; i++) {
            F[i] = F[i - 1] * i % MOD;
        }

        INV_F[MX - 1] = pow(F[MX - 1], MOD - 2);
        for (int i = MX - 1; i > 0; i--) {
            INV_F[i - 1] = INV_F[i] * i % MOD;
        }

        for (int i = 2; i < MX_K; i++) {
            if (LPF[i] == 0) { // i 是质数
                for (int j = i; j < MX_K; j += i) {
                    if (LPF[j] == 0) {
                        LPF[j] = i; // j 的最小质因子是 i
                    }
                }
            }
        }
    }

    public int[] waysToFillArray(int[][] queries) {
        int[] ans = new int[queries.length];
        for (int i = 0; i < queries.length; i++) {
            int[] q = queries[i];
            int n = q[0];
            int k = q[1];
            long res = 1;
            while (k > 1) {
                int p = LPF[k];
                int e = 1;
                for (k /= p; k % p == 0; k /= p) {
                    e++;
                }
                res = res * comb(e + n - 1, e) % MOD;
            }
            ans[i] = (int) res;
        }
        return ans;
    }

    private long comb(int n, int m) {
        return F[n] * INV_F[m] % MOD * INV_F[n - m] % MOD;
    }

    private static long pow(long x, int n) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 10013;
const int MX_K = 10001;

long long F[MX]; // F[i] = i!
long long INV_F[MX]; // INV_F[i] = i!^-1
int LPF[MX_K]; // i 的最小质因子是 LPF[i]

long long pow(long long x, int n) {
    long long res = 1;
    for (; n; n /= 2) {
        if (n % 2) {
            res = res * x % MOD;
        }
        x = x * x % MOD;
    }
    return res;
}

auto init = [] {
    F[0] = 1;
    for (int i = 1; i < MX; i++) {
        F[i] = F[i - 1] * i % MOD;
    }

    INV_F[MX - 1] = pow(F[MX - 1], MOD - 2);
    for (int i = MX - 1; i; i--) {
        INV_F[i - 1] = INV_F[i] * i % MOD;
    }

    for (int i = 2; i < MX_K; i++) {
        if (LPF[i] == 0) { // i 是质数
            for (int j = i; j < MX_K; j += i) {
                if (LPF[j] == 0) {
                    LPF[j] = i; // j 的最小质因子是 i
                }
            }
        }
    }
    return 0;
}();

int comb(int n, int m) {
    return F[n] * INV_F[m] % MOD * INV_F[n - m] % MOD;
}

class Solution {
public:
    vector<int> waysToFillArray(vector<vector<int>> &queries) {
        vector<int> ans;
        for (auto &q: queries) {
            int n = q[0], k = q[1];
            long long res = 1;
            while (k > 1) {
                int p = LPF[k];
                int e = 1;
                for (k /= p; k % p == 0; k /= p) {
                    e++;
                }
                res = res * comb(e + n - 1, e) % MOD;
            }
            ans.push_back(res);
        }
        return ans;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const mx = 10013
const mxK = 10001

var f [mx]int    // f[i] = i!
var invF [mx]int // invF[i] = i!^-1
var lpf [mxK]int // i 的最小质因子是 lpf[i]

func init() {
    f[0] = 1
    for i := 1; i < mx; i++ {
        f[i] = f[i-1] * i % mod
    }

    invF[mx-1] = pow(f[mx-1], mod-2)
    for i := mx - 1; i > 0; i-- {
        invF[i-1] = invF[i] * i % mod
    }

    for i := 2; i < mxK; i++ {
        if lpf[i] == 0 { // i 是质数
            for j := i; j < mxK; j += i {
                if lpf[j] == 0 {
                    lpf[j] = i // j 的最小质因子是 i
                }
            }
        }
    }
}

func comb(n, m int) int {
    return f[n] * invF[m] % mod * invF[n-m] % mod
}

func waysToFillArray(queries [][]int) []int {
    ans := make([]int, len(queries))
    for i, q := range queries {
        n, k := q[0], q[1]
        res := 1
        for k > 1 {
            p := lpf[k]
            e := 1
            for k /= p; lpf[k] == p; k /= p {
                e++
            }
            res = res * comb(e+n-1, e) % mod
        }
        ans[i] = res
    }
    return ans
}

func pow(x, n int) int {
    res := 1
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            res = res * x % mod
        }
        x = x * x % mod
    }
    return res
}
```

```js [sol-JavaScript]
const MOD = 1_000_000_007n;
const MX = 10013;
const MX_K = 10001;

const F = Array(MX); // f[i] = i!
const INV_F = Array(MX); // inv_f[i] = i!^-1
const LPF = Array(MX_K); // i 的最小质因子是 lpf[i]

function pow(x, n) {
    let res = 1n;
    for (; n; n = Math.floor(n / 2)) {
        if (n % 2) {
            res = res * x % MOD;
        }
        x = x * x % MOD;
    }
    return res;
}

F[0] = 1n;
for (let i = 1; i < MX; i++) {
    F[i] = F[i - 1] * BigInt(i) % MOD;
}

INV_F[MX - 1] = pow(F[MX - 1], Number(MOD) - 2);
for (let i = MX - 1; i > 0; i--) {
    INV_F[i - 1] = INV_F[i] * BigInt(i) % MOD;
}

for (let i = 2; i < MX_K; i++) {
    if (LPF[i] === undefined) { // i 是质数
        for (let j = i; j < MX_K; j += i) {
            if (LPF[j] === undefined) {
                LPF[j] = i; // j 的最小质因子是 i
            }
        }
    }
}

function comb(n, m) {
    return F[n] * INV_F[m] % MOD * INV_F[n - m] % MOD;
}

function waysToFillArray(queries) {
    const ans = new Array(queries.length);
    for (let i = 0; i < queries.length; i++) {
        let [n, k] = queries[i];
        let res = 1n;
        while (k > 1) {
            const p = LPF[k];
            let e = 1;
            for (k /= p; k % p === 0; k /= p) {
                e++;
            }
            res = res * comb(e + n - 1, e) % MOD;
        }
        ans[i] = res;
    }
    return ans;
}
```

```rust [sol-Rust]
const MOD: i64 = 1_000_000_007;
const MX: usize = 10013;
const MX_K: usize = 10001;

static mut initialized: bool = false;
static mut fac: [i64; MX] = [0; MX]; // f[i] = i!
static mut inv_f: [i64; MX] = [0; MX]; // inv_f[i] = i!^-1
static mut lpf: [usize; MX_K] = [0; MX_K]; // i 的最小质因子是 lpf[i]

unsafe fn init_once() {
    if initialized {
        return;
    }
    initialized = true;

    fac[0] = 1;
    for i in 1..MX {
        fac[i] = fac[i - 1] * i as i64 % MOD;
    }

    inv_f[MX - 1] = pow(fac[MX - 1], MOD as i32 - 2);
    for i in (1..MX).rev() {
        inv_f[i - 1] = inv_f[i] * i as i64 % MOD;
    }

    for i in 2..MX_K {
        if lpf[i] == 0 { // i 是质数
            for j in (i..MX_K).step_by(i) {
                if lpf[j] == 0 {
                    lpf[j] = i; // j 的最小质因子是 i
                }
            }
        }
    }
}

fn pow(mut x: i64, mut n: i32) -> i64 {
    let mut res = 1;
    while n > 0 {
        if n % 2 > 0 {
            res = res * x % MOD;
        }
        x = x * x % MOD;
        n /= 2;
    }
    res
}

unsafe fn comb(n: usize, m: usize) -> i64 {
    fac[n] * inv_f[m] % MOD * inv_f[n - m] % MOD
}

impl Solution {
    pub fn ways_to_fill_array(queries: Vec<Vec<i32>>) -> Vec<i32> {
        unsafe { init_once(); }
        let mut ans = Vec::with_capacity(queries.len());
        for q in queries {
            let n = q[0] as usize;
            let mut k = q[1] as usize;
            let mut res = 1;
            while k > 1 {
                unsafe {
                    let p = lpf[k];
                    let mut e = 0;
                    while k % p == 0 {
                        e += 1;
                        k /= p;
                    }
                    res = res * comb(e + n - 1, e) % MOD;
                }
            }
            ans.push(res as i32);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：预处理的时间为 $\mathcal{O}(N+K\log \log K)$，其中 $N=10^4, K=10^4$。回答单个询问的时间为 $\mathcal{O}(\log k)$，因为 $k$ 有 $\mathcal{O}(\log k)$ 个质因子。
- 空间复杂度：$\mathcal{O}(N+K)$。返回值不计入。

注：如果用欧拉筛计算 LPF，则预处理的时间复杂度为 $\mathcal{O}(N+K)$。

## 放球问题

- [1641. 统计字典序元音字符串的数目](https://leetcode.cn/problems/count-sorted-vowel-strings/) 1519
- [1621. 大小为 K 的不重叠线段的数目](https://leetcode.cn/problems/number-of-sets-of-k-non-overlapping-line-segments/) 2198
- [2338. 统计理想数组的数目](https://leetcode.cn/problems/count-the-number-of-ideal-arrays/) 2615
- [CF1931G](https://codeforces.com/contest/1931/problem/G) & [视频讲解](https://www.bilibili.com/video/BV1p6421g736/)

## 预处理最小质因子 LPF

- [2709. 最大公约数遍历](https://leetcode.cn/problems/greatest-common-divisor-traversal/) 2172
- [1998. 数组的最大公因数排序](https://leetcode.cn/problems/gcd-sort-of-an-array/) 2429

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

本题来自 `一、数论 / §1.3 质因数分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.3 质因数分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
