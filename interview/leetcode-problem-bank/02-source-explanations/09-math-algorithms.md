# 数学算法

- 来源 URL：https://leetcode.cn/circle/discuss/IYT3ss/
- 来源标题：分享丨【算法题单】数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）
- 抓取时间：2026-09-17 16:46:02 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 数学算法

> 图：暴力？NO！数学做法，降维打击！

### 前言

本文全面整理了力扣上的数学相关题目，其中数论和组合数学的题目较多。
部分题目（尤其是组合数学）会涉及到取模，我写了一篇详细的取模教程，请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

### 一、数论 / §1.1 判断质数

如何判断一个正整数 $n$ 是不是质数？
只需判断 $n$ 能否能被 $\sqrt n$ 以内的某个大于 $1$ 的整数整除，如果不能则说明 $n$ 是质数。为什么？
**反证法**：如果 $n$ 不能被 $\sqrt n$ 以内的大于 $1$ 的整数整除，但可以被大于 $\sqrt n$ 的整数 $d\ (d<n)$ 整除，那么必然还有一个数 $\dfrac{n}{d}$ 也能整除 $n$。但是 $\dfrac{n}{d} < \dfrac{n}{\sqrt n} = \sqrt n$，说明存在一个 $\sqrt n$ 以内的大于 $1$ 的整数能整除 $n$，矛盾。
注意 $1$ 不是质数。
```py [sol-Python3]
# 时间复杂度 O(sqrt(n))
def is_prime(n: int) -> bool:
    for i in range(2, math.isqrt(n) + 1):
        if n % i == 0:
            return False
    return n >= 2  # 1 不是质数
```java [sol-Java]

### 一、数论 / §1.2 预处理质数（筛质数）

模板（埃氏筛）：
```py [sol-Python3]
# 时间复杂度 O(MX * log log MX)
MX = 1_000_001
is_prime = [False] * 2 + [True] * (MX - 2)  # 0 和 1 不是质数
primes = []
for i in range(2, MX):
    if is_prime[i]:
        primes.append(i)
        for j in range(i * i, MX, i):
            is_prime[j] = False  # j 是质数 i 的倍数
```java [sol-Java]

### 一、数论 / §1.3 质因数分解

**模板一**：预处理每个数的所有不同质因子。原理同埃氏筛。
```py [sol-Python3]
MX = 1_000_001  # **根据题目数据范围修改**
prime_factors = [[] for _ in range(MX)]
for i in range(2, MX):
    if not prime_factors[i]:  # i 是质数
        for j in range(i, MX, i):  # i 的倍数 j 有质因子 i
            prime_factors[j].append(i)
```java [sol-Java]
class Solution {
    private static final int MX = 1_000_001; // **根据题目数据范围修改**
    private static final List<Integer>[] primeFactors = new ArrayList[MX];

### 一、数论 / §1.5 因子

**模板一**：计算一个数的所有因子。
```py [sol-Python3]
# 返回正整数 n 的所有因子（未排序）
# 时间复杂度 O(√n)
def calc_divisors(n: int) -> list[int]:
    divisors = []
    for d in range(1, math.isqrt(n) + 1):
        if n % d == 0:
            divisors.append(d)
            if d * d < n:
                divisors.append(n // d)
    return divisors

### 一、数论 / §1.6 最大公约数（GCD）

部分语言的标准库没有 GCD 和 LCM，需要手写。推荐写迭代，比递归快一点。
```java [sol-Java]
class Solution {
    private long gcd(long a, long b) {
        while (a != 0) {
            long tmp = a;
            a = b % a;
            b = tmp;
        return b;
    // 推荐先除后乘，尽量避免溢出
    private long lcm(long a, long b) {
        return a / gcd(a, b) * b;

### 一、数论 / §1.11 莫比乌斯函数

数论中的容斥原理。

### 二、组合数学 / §2.2 组合计数

预处理阶乘及其逆元后，可以 $\mathcal{O}(1)$ 计算组合数，模板代码如下。原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。
```py [sol-Python3]
MOD = 1_000_000_007
MX = 100_001  # **根据题目数据范围修改**
fac = [0] * MX  # fac[i] = i!
fac[0] = 1
for i in range(1, MX):
    fac[i] = fac[i - 1] * i % MOD
inv_f = [0] * MX  # inv_f[i] = i!^-1
inv_f[-1] = pow(fac[-1], -1, MOD)
for i in range(MX - 1, 0, -1):
    inv_f[i - 1] = inv_f[i] * i % MOD

### 二、组合数学 / §2.3 容斥原理

部分题目有其他解法，难度分仅供参考。

### 二、组合数学 / §2.4 生成函数（母函数）

> 注：贡献法移至贪心与思维题单。

### 三、概率期望

- [九坤-04. 筹码游戏](https://leetcode.cn/contest/ubiquant2022/problems/I3Gm2h/)
**随机数据下显著更快的算法**：

### 五、计算几何 / §5.4 凸包

```py [sol-Python3]
class Vec:
    __slots__ = 'x', 'y'
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y
    def __sub__(self, b: "Vec") -> "Vec":
        return Vec(self.x - b.x, self.y - b.y)
    def det(self, b: "Vec") -> int:
        return self.x * b.y - self.y * b.x
# Andrew 算法，计算 points 的凸包（逆时针顺序）
# 时间复杂度 O(n log n)，其中 n = len(points)

### 七、杂项 / §7.1 回文数

从小到大枚举回文数的模板（从 $1$ 开始枚举）：
```py [sol-Python3]
def gen_palindrome() -> Iterator[int]:
    base = 1
    while True:
        # 生成奇数长度回文数，例如 base = 10，生成的范围是 101 ~ 999
        for i in range(base, base * 10):
            s = str(i)
            x = int(s + s[::-1][1:])
        # 生成偶数长度回文数，例如 base = 10，生成的范围是 1001 ~ 9999
        for i in range(base, base * 10):
            s = str(i)

### 七、杂项 / §7.4 卷积

另见本题单的「**§2.6 生成函数**」。

### 七、杂项 / §7.7 线性基

见 [位运算题单](https://leetcode.cn/circle/discuss/dHn9Vk/)。

### 七、杂项 / §7.10 其他

- [xh-10.2. 约瑟夫环问题](https://leetcode.cn/problems/W7yuXW/)

### 算法题单

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 一、数论 / §1.1 判断质数 | 更快的模板（写法二） | https://leetcode.cn/problems/sum-of-largest-prime-substrings/solutions/3685356/pan-duan-zhi-shu-pythonjavacgo-by-endles-0w8f/ | pending-fetch |
| 二、组合数学 / §2.2 组合计数 | 图解：多重集组合数 | https://leetcode.cn/problems/count-ways-to-make-array-with-product/solutions/2713481/tu-jie-zhi-yin-zi-fen-jie-fang-qiu-wen-t-fboo/ | pending-fetch |

## 本地原创增强解析

数学算法 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
