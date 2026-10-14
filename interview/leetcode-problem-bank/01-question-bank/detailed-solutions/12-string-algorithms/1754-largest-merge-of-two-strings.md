# 1754. 构造字典序最大的合并字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-merge-of-two-strings/
- 题目 slug：`largest-merge-of-two-strings`
- 来源专题：字符串
- 来源分类路径：八、后缀数组/后缀自动机
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/largest-merge-of-two-strings/solutions/595542/hou-zhui-shu-zu-xian-xing-zuo-fa-by-endl-pedl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[后缀数组线性做法](https://leetcode.cn/problems/largest-merge-of-two-strings/solutions/595542/hou-zhui-shu-zu-xian-xing-zuo-fa-by-endl-pedl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hou-zhui-shu-zu-xian-xing-zuo-fa-by-endl-pedl`
- topic id：`595542`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 13:14:52 +0800

利用 $\textit{rank}$ 数组来代替原来的字符串比较逻辑，从而做到线性复杂度。

```go
func largestMerge(s, t string) string {
	n := len(s)
	sa := *(*[]int32)(unsafe.Pointer(reflect.ValueOf(suffixarray.New([]byte(s + "#" + t))).Elem().FieldByName("sa").Field(0).UnsafeAddr()))
	rank := make([]int, n+1+len(t))
	for i := range rank {
		rank[sa[i]] = i
	}
	ans := []byte{}
	i, j := 0, n+1
	for {
		if s == "" {
			ans = append(ans, t...)
			break
		}
		if t == "" {
			ans = append(ans, s...)
			break
		}
		if rank[i] > rank[j] { // s > t
			ans = append(ans, s[0])
			s = s[1:]
			i++
		} else {
			ans = append(ans, t[0])
			t = t[1:]
			j++
		}
	}
	return string(ans)
}
```

附：[后缀数组的介绍、求法及应用](https://oi-wiki.org/string/sa/)
