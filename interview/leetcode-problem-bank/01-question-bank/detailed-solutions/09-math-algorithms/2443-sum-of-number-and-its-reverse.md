# 2443. 反转之后的数字和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-number-and-its-reverse/
- 题目 slug：`sum-of-number-and-its-reverse`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sum-of-number-and-its-reverse/solutions/1895714/bao-li-mei-ju-by-endlesscheng-p7b4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[暴力枚举](https://leetcode.cn/problems/sum-of-number-and-its-reverse/solutions/1895714/bao-li-mei-ju-by-endlesscheng-p7b4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bao-li-mei-ju-by-endlesscheng-p7b4`
- topic id：`1895714`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

[视频讲解](https://www.bilibili.com/video/BV1Ae4y1i7PM) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

一行写法（由于 Python 在处理反转时调用了底层的 C 库，这种写法实际比不用字符串的写法还要快）：

```py
class Solution:
    def sumOfNumberAndReverse(self, num: int) -> bool:
        return any(i + int(str(i)[::-1]) == num for i in range(0, num + 1))
```

不用字符串的写法：

```py [sol1-Python3]
class Solution:
    def sumOfNumberAndReverse(self, num: int) -> bool:
        for i in range(num + 1):
            rev, x = 0, i
            while x:
                rev = rev * 10 + x % 10
                x //= 10
            if i + rev == num:
                return True
        return False
```

```go [sol1-Go]
func sumOfNumberAndReverse(num int) bool {
	for i := 0; i <= num; i++ {
		rev := 0
		for x := i; x > 0; x /= 10 {
			rev = rev*10 + x%10
		}
		if i+rev == num {
			return true
		}
	}
	return false
}
```

#### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n=\textit{num}$。
- 空间复杂度：$O(1)$，仅用到若干变量。
