# 2117. 一个区间内所有数乘积的缩写

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/abbreviating-the-product-of-a-range/
- 题目 slug：`abbreviating-the-product-of-a-range`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：2477
- 外部题解来源：https://leetcode.cn/problems/abbreviating-the-product-of-a-range/solutions/1176795/chai-fen-cheng-si-ge-wen-ti-ji-suan-dui-6karq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[[更新高精度算法] 拆分成四个问题：计算对数 + 对 1e5 取模 + 计算尾零个数 + 判断剩余数字的长度](https://leetcode.cn/problems/abbreviating-the-product-of-a-range/solutions/1176795/chai-fen-cheng-si-ge-wen-ti-ji-suan-dui-6karq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chai-fen-cheng-si-ge-wen-ti-ji-suan-dui-6karq`
- topic id：`1176795`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

再更：力扣将数据范围由 $10^6$ 下调至 $10^4$，大大降低了本题的难度，暴力模拟即可通过此题。

```go
func abbreviateProduct(left, right int) string {
	s := new(big.Int).MulRange(int64(left), int64(right)).String()
	tz := len(s)
	s = strings.TrimRight(s, "0")
	tz -= len(s)
	if len(s) > 10 {
		return fmt.Sprintf("%s...%se%d", s[:5], s[len(s)-5:], tz)
	}
	return fmt.Sprintf("%se%d", s, tz)
}
```

---

以下为原题解及高精度修复后的题解。

将原问题拆分成如下四个问题：

- 计算 $\textit{pre}$；
- 计算 $\textit{suf}$；
- 计算 $C$；
- 判断剩余数字的长度是否超过 $10$。

#### 1. 计算 $\textit{pre}$

我们可以通过取以 $10$ 为底的对数的方式，将乘法转换成加法，即如下法则：

$$
a\cdot b = 10^{\log_{10}a}\cdot 10^{\log_{10}b} = 10^{\log_{10}a+\log_{10}b}
$$

记最后得到的指数为 $e$，则有

$$
\textit{pre} = \lfloor10^{e-\lfloor e \rfloor} \cdot 10000\rfloor = \lfloor10^{e-\lfloor e \rfloor + 4}\rfloor
$$

#### 2. 计算 $C$

先来看怎么计算尾零。这相当于求乘积中能分解出来的 $10$ 的个数。

由于 $10=2\cdot 5$，我们可以将所有整数分解质因子，那么分解出来的 $2$ 的幂次之和，以及 $5$ 的幂次之和，这两者的较小值就是最后乘积中能分解出来的 $10$ 的个数。

#### 3. 计算 $\textit{suf}$

我们可以将每个数字的所有因子 $2$ 和 $5$ 去掉，然后将剩下的数字相乘，由于我们只取末 $5$ 位，所以在乘法的过程中需要对 $10^5$ 取模。

由于可能会多去掉一些 $2$ 或 $5$，在遍历 $[\textit{left},\textit{right}]$ 结束后还需要再重新乘上多去掉的 $2$ 或 $5$。

#### 4. 判断剩余数字的长度是否超过 $10$

在上一条的计算过程中额外计算一个乘积 $\textit{mul}$，判断其是否会大于或等于 $10^{10}$。

```go
func abbreviateProduct(left, right int) string {
	e, cnt2, cnt5, suf, mul := 0.0, 0, 0, 1, 1
	update := func(x int) {
		suf = suf * x % 1e5
		if mul != -1 {
			mul *= x
			if mul >= 1e10 { // 长度超过 10
				mul = -1
			}
		}
	}

	for i := left; i <= right; i++ {
		e += math.Log10(float64(i))
		x := i
		tz := bits.TrailingZeros(uint(x)) // 因子 2 的个数
		cnt2 += tz
		x >>= tz
		for ; x%5 == 0; x /= 5 {
			cnt5++ // 因子 5 的个数
		}
		update(x)
	}
	cnt10 := min(cnt2, cnt5)
	for i := cnt10; i < cnt2; i++ {
		update(2) // 补上多拆出来的 2
	}
	for i := cnt10; i < cnt5; i++ {
		update(5) // 补上多拆出来的 5
	}

	if mul != -1 { // 不需要缩写
		return fmt.Sprintf("%de%d", mul, cnt10)
	}
	pre := int(math.Pow(10, e-math.Floor(e)+4))
	return fmt.Sprintf("%d...%05de%d", pre, suf, cnt10)
}

func min(a, b int) int { if a > b { return b }; return a }
```

然而上述解法并不能通过 [@hqztrue](/u/hqztrue/) 给出的数据。按照他那篇题解给出的建议，将计算 $\textit{pre}$ 的部分换成了高精度的写法。直接对每个数相乘，超过一定位数就截断。

```go
var lim, _ = new(big.Int).SetString(strings.Repeat("9", 200), 10)
var div, _ = new(big.Int).SetString("1"+strings.Repeat("0", 100), 10)

func abbreviateProduct(left, right int) string {
	cnt2, cnt5, suf, mul := 0, 0, 1, 1
	update := func(x int) {
		suf = suf * x % 1e5
		if mul != -1 {
			mul *= x
			if mul >= 1e10 { // 长度超过 10
				mul = -1
			}
		}
	}

	pre := big.NewInt(1)
	for i := left; i <= right; i++ {
		pre.Mul(pre, big.NewInt(int64(i)))
		if pre.Cmp(lim) > 0 { // 超过一定位数就截断
			pre.Quo(pre, div)
		}
		x := i
		tz := bits.TrailingZeros(uint(x)) // 因子 2 的个数
		cnt2 += tz
		x >>= tz
		for ; x%5 == 0; x /= 5 {
			cnt5++ // 因子 5 的个数
		}
		update(x)
	}
	cnt10 := min(cnt2, cnt5)
	for i := cnt10; i < cnt2; i++ {
		update(2) // 补上多拆出来的 2
	}
	for i := cnt10; i < cnt5; i++ {
		update(5) // 补上多拆出来的 5
	}

	if mul != -1 { // 不需要缩写
		return fmt.Sprintf("%de%d", mul, cnt10)
	}
	return fmt.Sprintf("%s...%05de%d", pre.String()[:5], suf, cnt10)
}

func min(a, b int) int { if a > b { return b }; return a }
```
