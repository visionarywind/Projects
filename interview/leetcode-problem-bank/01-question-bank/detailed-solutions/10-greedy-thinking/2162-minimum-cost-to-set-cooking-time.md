# 2162. 设置时间的最少代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-set-cooking-time/
- 题目 slug：`minimum-cost-to-set-cooking-time`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1852
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-set-cooking-time/solutions/1247077/fen-lei-tao-lun-by-endlesscheng-w66r/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论](https://leetcode.cn/problems/minimum-cost-to-set-cooking-time/solutions/1247077/fen-lei-tao-lun-by-endlesscheng-w66r/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-by-endlesscheng-w66r`
- topic id：`1247077`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

下文用 $s$ 表示 $\textit{targetSeconds}$。

- 如果 $s<100$，那么可以输入 $s$ 秒；
- 如果 $60\le s<6000$，那么可以输入 $\dfrac{s}{60}$ 分 $s\bmod 60$ 秒，注意这里秒数需要补前导零；
- 如果 $s\ge 100$ 且 $s\bmod 60<40$，那么我们可以借 $1$ 分钟给秒数，即输入 $\dfrac{s}{60}-1$ 分 $s\bmod 60+60$ 秒。
 
对于每种情况按照题意模拟，取所有情况的最小值。
 
```Python [sol1-Python3]
class Solution:
    def minCostSetTime(self, startAt: int, moveCost: int, pushCost: int, sec: int) -> int:
        def calc(s: str) -> int:
            cost = pushCost * len(s)
            cur = startAt
            for ch in s:
                if ord(ch) - ord('0') != cur:
                    cost += moveCost
                    cur = ord(ch) - ord('0')
            return cost

        ans = inf
        if 60 <= sec < 6000: ans = calc(f"{sec // 60}{sec % 60 :02}")
        if sec < 100: ans = min(ans, calc(str(sec)))  # 仅输入秒数
        elif sec % 60 < 40: ans = min(ans, calc(f"{sec // 60 - 1}{sec % 60 + 60}"))  # 借一分钟给秒数
        return ans
```

```go [sol1-Go]
func minCostSetTime(startAt, moveCost, pushCost, sec int) int {
	ans := math.MaxInt32
	calc := func(s string) {
		cost := pushCost * len(s)
		cur := startAt
		for _, ch := range s {
			if int(ch&15) != cur {
				cost += moveCost
				cur = int(ch & 15)
			}
		}
		if cost < ans { ans = cost }
	} 
	if sec >= 60 && sec < 6000 {
		calc(fmt.Sprintf("%d%02d", sec/60, sec%60))
	}
	if sec < 100 {
		calc(strconv.Itoa(sec)) // 仅输入秒数
	} else if sec%60 < 40 {
		calc(fmt.Sprintf("%d%d", sec/60-1, sec%60+60)) // 借一分钟给秒数
	}
	return ans
}
```
