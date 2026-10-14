# 2457. 美丽整数的最小增量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-addition-to-make-integer-beautiful/
- 题目 slug：`minimum-addition-to-make-integer-beautiful`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1680
- 外部题解来源：https://leetcode.cn/problems/minimum-addition-to-make-integer-beautiful/solutions/1936349/tan-xin-by-endlesscheng-f7e4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 + 视频讲解](https://leetcode.cn/problems/minimum-addition-to-make-integer-beautiful/solutions/1936349/tan-xin-by-endlesscheng-f7e4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-by-endlesscheng-f7e4`
- topic id：`1936349`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

[视频讲解](https://www.bilibili.com/video/BV1Em4y1c7Hc) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

基本思路是，不断 $+1$ 直到产生进位，就可能让数位和变小。

代码实现时，可以直接计算每个数位进位后的结果。

比如 $467$，十位数进位为 $470$，百位数进位为 $500$，千位数进位为 $1000$（这一点在 $\textit{target}=1$ 时尤为重要）。

```py [sol1-Python3]
class Solution:
    def makeIntegerBeautiful(self, n: int, target: int) -> int:
        tail = 1
        while True:
            m = x = n + (tail - n % tail) % tail  # 进位后的数字
            s = 0
            while x:
                s += x % 10
                x //= 10
            if s <= target: return m - n
            tail *= 10
```

```go [sol1-Go]
func makeIntegerBeautiful(n int64, target int) int64 {
	for tail := int64(1); ; tail *= 10 {
		m := n + (tail-n%tail)%tail // 进位后的数字
		sum := 0
		for x := m; x > 0; x /= 10 {
			sum += int(x % 10)
		}
		if sum <= target {
			return m - n
		}
	}
}
```

#### 复杂度分析

- 时间复杂度：$O(\log^2 n)$。
- 空间复杂度：$O(1)$，仅用到若干变量。
