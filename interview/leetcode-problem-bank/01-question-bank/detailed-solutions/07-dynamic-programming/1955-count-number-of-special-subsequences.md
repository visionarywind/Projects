# 1955. 统计特殊子序列的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-number-of-special-subsequences/
- 题目 slug：`count-number-of-special-subsequences`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.3 进阶
- 难度分：2125
- 外部题解来源：https://leetcode.cn/problems/count-number-of-special-subsequences/solutions/908427/dong-tai-gui-hua-by-endlesscheng-4onu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划](https://leetcode.cn/problems/count-number-of-special-subsequences/solutions/908427/dong-tai-gui-hua-by-endlesscheng-4onu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-by-endlesscheng-4onu`
- topic id：`908427`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

定义：
- $f[i][0]$ 表示前 $i$ 项得到的全 $0$ 子序列个数
- $f[i][1]$ 表示前 $i$ 项得到的先 $0$ 后 $1$ 的子序列个数
- $f[i][2]$ 表示前 $i$ 项得到的特殊子序列个数

遍历数组 $\textit{nums}$，对于 $f[i][j]$，若 $j \neq \textit{nums}[i]$，则直接从前一项转移过来，即 $f[i][j]=f[i-1][j]$。

若 $j = \textit{nums}[i]$ 则需要分类计算：

对于 $f[i][0]$，当遇到 $0$ 时，有选或不选两种方案，不选 $0$ 时有 $f[i][0] = f[i-1][0]$，选 $0$ 时，可以单独组成一个子序列，也可以与前面的 $0$ 组合，因此有 $f[i][0] = f[i-1][0] + 1$，两者相加得 $f[i][0] = 2\cdot f[i-1][0] + 1$。

对于 $f[i][1]$，当遇到 $1$ 时，有选或不选两种方案，不选 $1$ 时有 $f[i][1] = f[i-1][1]$，选 $1$ 时，可以单独与前面的 $0$ 组成一个子序列，也可以与前面的 $1$ 组合，因此有 $f[i][1] = f[i-1][1] + f[i-1][0]$，两者相加得 $f[i][1] = 2\cdot f[i-1][1] + f[i-1][0]$。

$f[i][2]$ 和 $f[i][1]$ 类似，有 $f[i][2] = 2\cdot f[i-1][2] + f[i-1][1]$。

最后答案为 $f[n-1][2]$。

代码实现时，可以把第一维压缩掉。

```go
const mod int = 1e9 + 7

func countSpecialSubsequences(nums []int) int {
	f := [3]int{}
	for _, v := range nums {
		if v == 0 {
			f[0] = (f[0]*2 + 1) % mod
		} else if v == 1 {
			f[1] = (f[1]*2 + f[0]) % mod
		} else {
			f[2] = (f[2]*2 + f[1]) % mod
		}
	}
	return f[2]
}
```
