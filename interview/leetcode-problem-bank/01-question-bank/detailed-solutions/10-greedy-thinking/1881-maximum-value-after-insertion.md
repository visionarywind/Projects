# 1881. 插入后的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-value-after-insertion/
- 题目 slug：`maximum-value-after-insertion`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1381
- 外部题解来源：https://leetcode.cn/problems/maximum-value-after-insertion/solutions/803003/tan-xin-sao-miao-yi-bian-xun-zhao-cha-ru-yxj8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心，扫描一遍寻找插入位置](https://leetcode.cn/problems/maximum-value-after-insertion/solutions/803003/tan-xin-sao-miao-yi-bian-xun-zhao-cha-ru-yxj8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-sao-miao-yi-bian-xun-zhao-cha-ru-yxj8`
- topic id：`803003`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

- 对于正数，插入位置左侧的数字都必须 $\ge x$
- 对于负数，插入位置左侧的数字都必须 $\le x$

```go
func maxValue(n string, x int) string {
	y := byte('0' + x)
	i := 0
	if n[0] != '-' {
		for ; i < len(n) && n[i] >= y; i++ {}
	} else {
		for i = 1; i < len(n) && n[i] <= y; i++ {}
	}
	return n[:i] + string(y) + n[i:]
}
```
