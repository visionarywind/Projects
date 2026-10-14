# 2182. 构造限制重复的字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/construct-string-with-repeat-limit/
- 题目 slug：`construct-string-with-repeat-limit`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1680
- 外部题解来源：https://leetcode.cn/problems/construct-string-with-repeat-limit/solutions/1278723/cong-da-dao-xiao-tan-xin-by-endlesscheng-b7ob/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法：从大到小贪心](https://leetcode.cn/problems/construct-string-with-repeat-limit/solutions/1278723/cong-da-dao-xiao-tan-xin-by-endlesscheng-b7ob/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-da-dao-xiao-tan-xin-by-endlesscheng-b7ob`
- topic id：`1278723`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

统计 $s$ 中每个字母的个数 $\textit{cnt}$。要使字典序最大，应优先选择最大的字母 $i$，选择 $\min(\textit{cnt}[i], \textit{repeatLimit})$ 个。

如果字母 $i$ 已被选完，那么继续选择下一个较小的字母。

如果字母 $i$ 未被选完，为了不让字母 $i$ 连续出现次数超过 $\textit{repeatLimit}$，我们可以选择一个较小的字母放在后面，然后再继续选择字母 $i$。

```go [sol1-goto]
func repeatLimitedString(s string, repeatLimit int) string {
	cnt := [26]int{}
	for _, b := range s {
		cnt[b-'a']++
	}
	ans := []byte{}
	for i := 25; i >= 0; i-- { // 从大到小填字母
	next:
		for j := 0; j < repeatLimit && cnt[i] > 0; j++ { // 填充 min(repeatLimit, cnt[i]) 个字母 i
			cnt[i]--
			ans = append(ans, 'a'+byte(i))
		}
		if cnt[i] == 0 { // i 用完了，找下一个更小的字母
			continue
		}
		for j := i - 1; j >= 0; j-- { // i 没用完，插入一个字母 j，这样可以继续填 i
			if cnt[j] > 0 {
				cnt[j]--
				ans = append(ans, 'a'+byte(j))
				goto next
			}
		}
	}
	return string(ans)
}
```

```go [sol1-非 goto]
func repeatLimitedString(s string, repeatLimit int) string {
	cnt := [26]int{}
	for _, b := range s {
		cnt[b-'a']++
	}
	ans := []byte{}
	for i := 25; i >= 0; i-- { // 从大到小填字母
		k := i - 1
		for {
			for j := 0; j < repeatLimit && cnt[i] > 0; j++ { // 填充 min(repeatLimit, cnt[i]) 个字母 i
				cnt[i]--
				ans = append(ans, 'a'+byte(i))
			}
			if cnt[i] == 0 { // i 用完了，找下一个更小的字母
				break
			}
			for k >= 0 && cnt[k] == 0 {
				k--
			}
			if k < 0 {
				break
			}
			// i 没用完，插入一个字母 k，这样可以继续填 i
			cnt[k]--
			ans = append(ans, 'a'+byte(k))
		}
	}
	return string(ans)
}
```
