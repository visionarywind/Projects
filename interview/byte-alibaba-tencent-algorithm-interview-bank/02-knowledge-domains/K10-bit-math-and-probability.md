# K10 位运算、数学、概率、组合与数论

## 核心问题

- **位运算技巧**：异或抵消、`lowbit` 取最低位、位计数、判断 2 的幂、枚举子集，是"线性时间 + 常数空间"题的常用武器。
- **快速幂**：把 `O(e)` 的幂运算降到 `O(log e)`，并用于矩阵快速幂与模逆元。
- **GCD / LCM**：欧几里得辗转相除，配合"先除后乘"防止溢出。
- **质数**：埃氏筛、线性筛、质因数分解。
- **组合数**：帕斯卡递推、阶乘 + 逆元、`mod` 为质数时的费马小定理。
- **概率与期望**：用线性期望拆分指标变量，用独立事件相乘，用鸽巢原理证明必然存在性。

学习目标是能识别可用位运算/数论简化的结构、写出防溢出的 C++17 实现并说明复杂度。

## 训练顺序

1. 先掌握位运算四件套（异或、`lowbit`、`popcount`、枚举子集），做"只出现一次的数字"打底。
2. 再学快速幂与欧几里得，理解 `O(log)` 分治的本质。
3. 然后做质数筛与质因数分解，明确各筛法的复杂度与适用上限。
4. 接着做组合数与取模，重点记"逆元 -> 除法变乘法"。
5. 最后做概率/期望题与鸽巢原理证明题，训练"用线性期望拆指标"的思维。

## 核心模式

### 1. 位运算技巧

```cpp
int  lowbit(int x)   { return x & (-x); }              // 最低位 1 及其后的 0
bool isPow2(int x)   { return x > 0 && (x & (x - 1)) == 0; }
int  popcount(unsigned x) { return __builtin_popcount(x); }   // GCC/Clang 内建

// 枚举 mask 的所有非空子集 sub（含 mask 本身，不含空集）
for (int sub = mask; sub; sub = (sub - 1) & mask) { /* 使用 sub */ }
```

不变量：`x & (x-1)` 消去最低位的 1；`(sub-1) & mask` 生成按位递减的下一个子集。复杂度：`lowbit/popcount` 为 `O(1)`；枚举所有子集的总代价为 `O(3^k)`（`k` 为全集位数）。

### 2. 异或抵消（只出现一次的数字）

```cpp
int singleNumber(const vector<int>& nums) {
    int acc = 0;                             // 0 是异或的单位元
    for (int x : nums) acc ^= x;             // 成对元素两两抵消
    return acc;
}
```

不变量：`acc` 等于已扫描前缀的异或。性质：`a^a=0`、`a^0=a`、交换律与结合律。复杂度：时间 `O(n)`、空间 `O(1)`。

### 3. 快速幂（模幂）

```cpp
long long modpow(long long b, long long e, long long mod) {
    long long r = 1 % mod;                   // 处理 mod == 1 的退化
    b %= mod;
    while (e > 0) {
        if (e & 1) r = r * b % mod;          // 当前二进制位为 1，乘入
        b = b * b % mod;                     // 底数平方
        e >>= 1;                             // 指数右移
    }
    return r;
}
```

不变量：循环中 `r · b^e` 始终等于初始的 `b₀^{e₀}`（对 mod 取模）。复杂度：`O(log e)` 时间、`O(1)` 空间。

### 4. GCD / LCM

```cpp
long long gcd(long long a, long long b) {
    while (b) { a %= b; swap(a, b); }
    return a;                                // gcd(a, 0) = a
}
long long lcm(long long a, long long b) {
    return a / gcd(a, b) * b;                // 先除后乘，避免 a*b 溢出
}
```

不变量：`gcd(a,b) = gcd(b, a mod b)`。复杂度：`O(log min(a,b))`。注意 `lcm` 必须先除后乘。

### 5. 线性筛（质数）

```cpp
vector<int> primes;
vector<char> isComposite;
void linearSieve(int n) {
    isComposite.assign(n + 1, 0);
    for (int i = 2; i <= n; ++i) {
        if (!isComposite[i]) primes.push_back(i);
        for (int p : primes) {
            if ((long long)i * p > n) break;  // 防溢出
            isComposite[i * p] = 1;
            if (i % p == 0) break;            // 每个合数只被其最小质因子筛一次
        }
    }
}
```

不变量：进入第 `i` 轮时，`isComposite` 已正确标记所有 `≤ n` 的合数中最小质因子 `≤ i` 者。复杂度：`O(n)` 时间（每个合数恰好标记一次）、`O(n)` 空间。埃氏筛为 `O(n log log n)`，实现更短但常数略大。

### 6. 组合数（阶乘 + 逆元，`mod` 为质数）

```cpp
const long long MOD = 1000000007LL;
vector<long long> fact, invFact;
void precompute(int n) {
    fact.assign(n + 1, 1);
    invFact.assign(n + 1, 1);
    for (int i = 1; i <= n; ++i) fact[i] = fact[i - 1] * i % MOD;
    invFact[n] = modpow(fact[n], MOD - 2, MOD);        // 费马小定理求逆元
    for (int i = n; i > 0; --i) invFact[i - 1] = invFact[i] * i % MOD;
}
long long comb(int n, int k) {
    if (k < 0 || k > n) return 0;
    return fact[n] * invFact[k] % MOD * invFact[n - k] % MOD;
}
```

不变量：`invFact[i] = (i!)^{-1} mod MOD`。前提：`MOD` 为质数且 `n < MOD`（否则阶乘含 `MOD` 因子，逆元不存在）。复杂度：预处理 `O(n)`，单次查询 `O(1)`。

### 7. 概率与期望（线性期望）

- **期望的线性性**：`E[X+Y] = E[X]+E[Y]`，不要求独立。对计数型变量，常把"总数"拆成每个位置/每个配对的指示变量之和。
- **独立事件**：相互独立时 `P(A∩B) = P(A)P(B)`，多步独立过程的联合概率相乘。
- **条件期望**：`E[X] = Σ E[X|Y=y]·P(Y=y)`，用于分情况。
- **鸽巢原理**：`n+1` 个物体放入 `n` 个盒子，至少一个盒子有 ≥2 个物体；用于证明"必然存在"和下界。

## 常见陷阱

- **负数取模**：C++ 中 `-1 % 3 == -1`，需要归一到 `((x % m) + m) % m`；同余题忽略这一点会得到负下标或错误分组。
- **整数溢出**：`a*b` 在乘之前就可能溢出；两数均为 `int` 时先转 `long long`，`1e18` 量级乘法则考虑 `__int128`。
- **有符号右移与 `lowbit`**：`x & (-x)` 依赖补码，对 `unsigned` 与 `int` 均成立，但对 `x=0` 返回 0；`>>` 对负数实现定义，涉及负数位运算时改用无符号。
- **`1 << n` 溢出**：`n >= 31` 时 `1 << n` 在 `int` 上溢出，应用 `1LL << n`。
- **快速幂底数为负/指数为 0**：`modpow` 需先 `b %= mod` 归一，`e=0` 时返回 `1 % mod`。
- **`lcm` 先乘后除**：`a*b/gcd(a,b)` 在大数下溢出，必须先 `a/gcd` 再乘 `b`。
- **筛法边界**：`i*p > n` 用 `int` 乘可能溢出，转 `long long`；`n < 2` 时素数列表为空，注意特判。
- **组合数逆元前提**：`MOD` 非质数时费马小定理不适用，需扩展欧几里得求逆或改用其他方法；`n >= MOD` 时阶乘法失效。
- **概率独立假设滥用**：不独立的事件不能直接相乘；考题常设"不放回抽样"破坏独立性。
- **期望用整数存**：概率/期望涉及浮点时，比较用误差容限而非 `==`，计数期望可保持整数。

## 复杂度速查

| 操作 | 时间 | 空间 | 备注 |
|---|---|---|---|
| `lowbit` / `popcount` | `O(1)` | `O(1)` | 内建函数 |
| 枚举所有子集 | `O(3^k)` | `O(1)` | `k` 为全集位数 |
| 快速幂 | `O(log e)` | `O(1)` | 模幂同样 |
| 矩阵快速幂 | `O(m^3 log e)` | `O(m^2)` | `m` 为矩阵阶 |
| GCD | `O(log min(a,b))` | `O(1)` | 辗转相除 |
| 埃氏筛 | `O(n log log n)` | `O(n)` | 简单 |
| 线性筛 | `O(n)` | `O(n)` | 每个合数标记一次 |
| 质因数分解 | `O(√n)` | `O(1)` | 试除到 `√n` |
| 组合数（预处理） | `O(n)` 预处理、`O(1)` 查询 | `O(n)` | 需 `MOD` 为质数 |
| 帕斯卡递推 | `O(n^2)` | `O(n^2)` | 不依赖逆元 |

说明：`e` 为指数，`k` 为位数，`n` 为规模。

## 题目索引

本域对应的真实题目文件：

- `../01-question-bank/beginner/B003-single-number.md` —— 异或抵消、线性时间常数空间，K10 的入门代表题，并含"出现三次逐位取模""两孤元素分组异或"两个进阶追问。

延伸方向（题面变体，非独立文件）：快速幂/矩阵快速幂（大数斐波那契）、质数筛（计数质数）、组合数取模（求组合数）、概率期望（生日悖论、期望步数），可在覆盖矩阵 `../00-overview/coverage-matrix.md` 的 `K10` 条目下继续补题。
