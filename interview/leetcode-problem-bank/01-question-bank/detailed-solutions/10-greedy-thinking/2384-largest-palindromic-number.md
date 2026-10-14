# 2384. 最大回文数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-palindromic-number/
- 题目 slug：`largest-palindromic-number`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.2 回文串贪心
- 难度分：1636
- 外部题解来源：https://leetcode.cn/problems/largest-palindromic-number/solutions/1764473/tan-xin-by-endlesscheng-epjv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计 + 贪心](https://leetcode.cn/problems/largest-palindromic-number/solutions/1764473/tan-xin-by-endlesscheng-epjv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-by-endlesscheng-epjv`
- topic id：`1764473`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1md4y1P75q) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

```py [sol1-Python3]
class Solution:
    def largestPalindromic(self, num: str) -> str:
        cnt = Counter(num)
        if cnt['0'] == len(num):  # 特判最特殊的情况：num 全是 0
            return "0"

        s = ""
        for d in digits[:0:-1]:
            s += d * (cnt[d] // 2)
        if s:  # 如果填了数字，则可以填 0
            s += '0' * (cnt['0'] // 2)

        t = s[::-1]
        for d in digits[::-1]:
            if cnt[d] % 2:  # 还可以填一个变成奇回文串
                s += d
                break
        return s + t  # 添加镜像部分
```

```go [sol1-Go]
func largestPalindromic(num string) string {
	cnt := ['9' + 1]int{}
	for _, d := range num {
		cnt[d]++
	}
	if cnt['0'] == len(num) { // 特判最特殊的情况：num 全是 0
		return "0"
	}

	s := []byte{}
	for i := byte('9'); i > '0' || i == '0' && len(s) > 0; i-- { // 如果填了数字，则可以填 0
		s = append(s, strings.Repeat(string(i), cnt[i]/2)...)
	}

	j := len(s) - 1
	for i := byte('9'); i >= '0'; i-- {
		if cnt[i]&1 > 0 { // 还可以填一个变成奇回文串
			s = append(s, i)
			break
		}
	}
	for ; j >= 0; j-- { // 添加镜像部分
		s = append(s, s[j])
	}
	return string(s)
}
```
