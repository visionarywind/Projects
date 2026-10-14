# 1044. 最长重复子串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-duplicate-substring/
- 题目 slug：`longest-duplicate-substring`
- 来源专题：字符串
- 来源分类路径：四、字符串哈希
- 难度分：2429
- 外部题解来源：https://leetcode.cn/problems/longest-duplicate-substring/solutions/862743/on-hou-zhui-shu-zu-zuo-fa-by-endlesschen-rqkp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(N) 后缀数组做法](https://leetcode.cn/problems/longest-duplicate-substring/solutions/862743/on-hou-zhui-shu-zu-zuo-fa-by-endlesschen-rqkp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-hou-zhui-shu-zu-zuo-fa-by-endlesschen-rqkp`
- topic id：`862743`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 13:14:52 +0800

```go
func longestDupSubstring(s string) string {
	// 求出后缀数组和高度数组
	n := len(s)
	sa := *(*[]int32)(unsafe.Pointer(reflect.ValueOf(suffixarray.New([]byte(s))).Elem().FieldByName("sa").Field(0).UnsafeAddr()))
	rank := make([]int, n)
	for i := range rank {
		rank[sa[i]] = i
	}
	height := make([]int, n)
	h := 0
	for i, rk := range rank {
		if h > 0 {
			h--
		}
		if rk > 0 {
			for j := int(sa[rk-1]); i+h < n && j+h < n && s[i+h] == s[j+h]; h++ {
			}
		}
		height[rk] = h
	}
	
	// 高度数组中的最大值对应的就是最长的重复子串
	idx, maxH := 0, 0
	for i, h := range height {
		if h > maxH {
			idx, maxH = i, h
		}
	}
	return s[sa[idx] : int(sa[idx])+maxH]
}
```
