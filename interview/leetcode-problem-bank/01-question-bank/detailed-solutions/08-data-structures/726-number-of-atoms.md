# 726. 原子的数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-atoms/
- 题目 slug：`number-of-atoms`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.5 表达式解析
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-atoms/solutions/259424/golang-zhan-map-shuang-bai-by-endlesscheng/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Golang 栈+map 双百](https://leetcode.cn/problems/number-of-atoms/solutions/259424/golang-zhan-map-shuang-bai-by-endlesscheng/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`golang-zhan-map-shuang-bai-by-endlesscheng`
- topic id：`259424`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

理解清楚题意就能一遍过

```go
func isDigit(b byte) bool { return '0' <= b && b <= '9' }
func isLower(b byte) bool { return 'a' <= b && b <= 'z' }

func countOfAtoms(s string) (ans string) {
	stack := []map[string]int{{}}
	i, n := 0, len(s)
	parseNum := func() (v int) {
		if i == n || !isDigit(s[i]) {
			return 1
		}
		for ; i < n && isDigit(s[i]); i++ {
			v = v*10 + int(s[i]-'0')
		}
		return
	}
	for i < n {
		switch s[i] {
		case '(':
			stack = append(stack, map[string]int{})
			i++
		case ')':
			mp := stack[len(stack)-1]
			stack = stack[:len(stack)-1]
			i++
			v := parseNum()
			for s, c := range mp {
				stack[len(stack)-1][s] += c * v
			}
		default:
			st := i
			for i++; i < n && isLower(s[i]); i++ {
			}
			name := s[st:i]
			stack[len(stack)-1][name] += parseNum()
		}
	}
	mp := stack[0]
	ss := make([]string, 0, len(mp))
	for k := range mp {
		ss = append(ss, k)
	}
	sort.Strings(ss)
	for _, s := range ss {
		ans += s
		if v := mp[s]; v > 1 {
			ans += strconv.Itoa(v)
		}
	}
	return
}
```
