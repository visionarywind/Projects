# 1960. 两个回文子字符串长度的最大乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-product-of-the-length-of-two-palindromic-substrings/
- 题目 slug：`maximum-product-of-the-length-of-two-palindromic-substrings`
- 来源专题：字符串
- 来源分类路径：三、Manacher 算法（回文串）
- 难度分：2691
- 外部题解来源：https://leetcode.cn/problems/maximum-product-of-the-length-of-two-palindromic-substrings/solutions/928047/go-ma-la-che-suan-fa-fu-xiang-xi-zhu-shi-98xx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 马拉车算法，附详细注释](https://leetcode.cn/problems/maximum-product-of-the-length-of-two-palindromic-substrings/solutions/928047/go-ma-la-che-suan-fa-fu-xiang-xi-zhu-shi-98xx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-ma-la-che-suan-fa-fu-xiang-xi-zhu-shi-98xx`
- topic id：`928047`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

```go
func maxProduct(s string) int64 {
	n := len(s)

	// 将 s 改造为 t，这样就不需要分 len(s) 的奇偶来讨论了，因为新串 t 的每个回文子串都是奇回文串（都有回文中心）
	// s 和 t 的下标转换关系：
	// (si+1)*2 = ti
	// ti/2-1 = si
	t := append(make([]byte, 0, n*2+3), '^')
	for _, c := range s {
		t = append(t, '#', byte(c))
	}
	t = append(t, '#', '$')

	// 定义一个奇回文串的半长度=(长度+1)/2，即保留回文中心，去掉一侧后的剩余字符串的长度
	// halfLen[i] 表示在 t 上的以 t[i] 为回文中心的最长回文子串的半长度
	// 即 [i-halfLen[i]+1,i+halfLen[i]-1] 是 t 上的一个回文子串
	halfLen := make([]int, len(t)-2)
	halfLen[1] = 1
	// r 表示当前右边界下标最大的回文子串的右边界下标+1
	// mid 为该回文子串的中心位置，二者的关系为 r=mid+halfLen[mid]
	for i, mid, r := 2, 1, 0; i < len(halfLen); i++ { // 循环的起止位置对应着原串的首尾字符
		hl := 1
		if i < r {
			// 记 i 关于 mid 的对称位置 i'=mid*2-i
			// 若以 i' 为中心的最长回文子串范围超出了以 mid 为中心的回文串的范围（即 i+halfLen[i'] >= r）
			// 则 halfLen[i] 应先初始化为已知的半长度 r-i，然后再继续暴力匹配
			// 否则 halfLen[i] 与 halfLen[i'] 相等
			hl = min(halfLen[mid*2-i], r-i)
		}
		// 暴力扩展
		// 算法的复杂度取决于这部分执行的次数
		// 由于扩展之后 r 必然会更新（右移），且扩展的的次数就是 r 右移的次数
		// 因此算法的复杂度和 t 的长度成正比
		for ; t[i-hl] == t[i+hl]; hl++ {}
		if i+hl > r {
			mid, r = i, i+hl
		}
		halfLen[i] = hl
	}

	// 计算两个数组，其中
	// startPL[i] 表示以 s[i] 为首字母的最长奇回文子串的长度
	// endPL[i]   表示以 s[i] 为尾字母的最长奇回文子串的长度
	startPL := make([]int, n)
	endPL := make([]int, n)
	for i := 2; i < len(halfLen); i += 2 { // 由于求的是奇数长度，写成 i+=2，改成 i++ 就是不限奇偶
		// t 中回文子串的长度为 hl*2-1
		// 由于其中 # 的数量总是比字符的数量多 1
		// 因此其在 s 中对应的回文子串的长度为 hl-1
		// 由于 t 中回文子串的首尾字母一定是 #，再根据上面说的下标转换关系，可以得到其在 s 中对应的回文子串的区间：
		// [(i-hl)/2, (i+hl)/2-2]
		hl := halfLen[i]
		left, right := (i-hl)/2, (i+hl)/2-2
		startPL[left] = max(startPL[left], hl-1)
		endPL[right] = max(endPL[right], hl-1)
	}
	for i := 1; i < n; i++ {
		startPL[i] = max(startPL[i], startPL[i-1]-2) // startPL[i] 还可能是一个更长的回文串缩短后的结果
	}
	for i := n - 2; i >= 0; i-- {
		startPL[i] = max(startPL[i], startPL[i+1]) // 变成后缀最大值
	}
	for i := n - 2; i >= 0; i-- {
		endPL[i] = max(endPL[i], endPL[i+1]-2) // endPL[i] 还可能是一个更长的回文串缩短后的结果
	}
	for i := 1; i < n; i++ {
		endPL[i] = max(endPL[i], endPL[i-1]) // 变成前缀最大值
	}

	ans := 0
	for i := 1; i < n; i++ {
		ans = max(ans, endPL[i-1]*startPL[i])
	}
	return int64(ans)
}

func min(a, b int) int { if a > b { return b }; return a }
func max(a, b int) int { if a < b { return b }; return a }
```

由于本题只要求奇回文串，所以不需要将 $s$ 改造，故上述代码可以化简如下：

```go
func maxProduct(s string) int64 {
	n := len(s)
	halfLen := make([]int, n)
	for i, mid, r := 0, 0, 0; i < n; i++ {
		hl := 1
		if i < r {
			hl = min(halfLen[mid*2-i], r-i)
		}
		for ; i >= hl && i+hl < n && s[i-hl] == s[i+hl]; hl++ {}
		if i+hl > r {
			mid, r = i, i+hl
		}
		halfLen[i] = hl
	}

	startPL := make([]int, n)
	endPL := make([]int, n)
	for i, hl := range halfLen {
		left, right := i-hl+1, i+hl-1
		startPL[left] = max(startPL[left], hl*2-1)
		endPL[right] = max(endPL[right], hl*2-1)
	}
	for i := 1; i < n; i++ {
		startPL[i] = max(startPL[i], startPL[i-1]-2)
	}
	for i := n - 2; i >= 0; i-- {
		startPL[i] = max(startPL[i], startPL[i+1])
	}
	for i := n - 2; i >= 0; i-- {
		endPL[i] = max(endPL[i], endPL[i+1]-2)
	}
	for i := 1; i < n; i++ {
		endPL[i] = max(endPL[i], endPL[i-1])
	}

	ans := 0
	for i := 1; i < n; i++ {
		ans = max(ans, endPL[i-1]*startPL[i])
	}
	return int64(ans)
}

func min(a, b int) int { if a > b { return b }; return a }
func max(a, b int) int { if a < b { return b }; return a }
```
