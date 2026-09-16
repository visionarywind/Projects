# LCP 63. 弹珠游戏

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/EXvqDp/
- 题目 slug：`EXvqDp`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/EXvqDp/solutions/1847059/mei-ju-by-endlesscheng-5wzf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[暴力枚举模拟](https://leetcode.cn/problems/EXvqDp/solutions/1847059/mei-ju-by-endlesscheng-5wzf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-by-endlesscheng-5wzf`
- topic id：`1847059`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

个人赛五道题目的 [视频讲解](https://www.bilibili.com/video/BV1zN4y1K762) 已出炉，欢迎点赞三连，在评论区分享你对这场比赛的看法~

---

由于路径是唯一的，一个入口只会对应一个唯一的出口；一个出口+进入出口的方向，可以找到唯一的入口。

因此，从不同入口出发的弹珠，走过的路径在同一个格子上是不会重叠的（方向相反不算重叠），且路径不存在环（可以画图理解）。

枚举所有入口，模拟即可。不需要 DFS，写个循环就行。

```py [sol1-Python3]
DIRS = ((0, 1), (1, 0), (0, -1), (-1, 0))  # 右下左上（顺时针）

class Solution:
    def ballGame(self, num: int, plate: List[str]) -> List[List[int]]:
        m, n = len(plate), len(plate[0])

        def check(x: int, y: int, d: int) -> bool:
            left = num
            while plate[x][y] != 'O':
                if left == 0: return False  # 无剩余步数
                if plate[x][y] == 'W':   d = (d + 3) % 4  # 逆时针
                elif plate[x][y] == 'E': d = (d + 1) % 4  # 顺时针
                x += DIRS[d][0]
                y += DIRS[d][1]
                if not (0 <= x < m and 0 <= y < n): return False  # 出界
                left -= 1
            return True

        ans = []
        for j in range(1, n - 1):
            if plate[0][j] == '.' and check(0, j, 1): ans.append([0, j])  # 上边
            if plate[-1][j] == '.' and check(m - 1, j, 3): ans.append([m - 1, j])  # 下边
        for i in range(1, m - 1):
            if plate[i][0] == '.' and check(i, 0, 0): ans.append([i, 0])  # 左边
            if plate[i][-1] == '.' and check(i, n - 1, 2): ans.append([i, n - 1])  # 右边
        return ans
```

```go [sol1-Go]
var dirs = []struct{ x, y int }{{0, 1}, {1, 0}, {0, -1}, {-1, 0}} // 右下左上（顺时针）

func ballGame(num int, plate []string) (ans [][]int) {
	m, n := len(plate), len(plate[0])
	check := func(x, y, d int) bool {
		for left := num; plate[x][y] != 'O'; left-- {
			if left == 0 { // 无剩余步数
				return false
			}
			if plate[x][y] == 'W' { // 逆时针
				d = (d + 3) % 4
			} else if plate[x][y] == 'E' { // 顺时针
				d = (d + 1) % 4
			}
			x += dirs[d].x
			y += dirs[d].y
			if x < 0 || x >= m || y < 0 || y >= n { // 从另一边出去了
				return false
			}
		}
		return true
	}
	for j := 1; j < n-1; j++ {
		if plate[0][j] == '.' && check(0, j, 1) {
			ans = append(ans, []int{0, j})
		}
		if plate[m-1][j] == '.' && check(m-1, j, 3) {
			ans = append(ans, []int{m - 1, j})
		}
	}
	for i := 1; i < m-1; i++ {
		if plate[i][0] == '.' && check(i, 0, 0) {
			ans = append(ans, []int{i, 0})
		}
		if plate[i][n-1] == '.' && check(i, n-1, 2) {
			ans = append(ans, []int{i, n - 1})
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$O(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{plate}$ 的行数和列数。每个状态（横坐标，纵坐标，方向）至多被访问一次，总共有 $4mn$ 个状态。
- 空间复杂度：$O(1)$。返回值不计入。

## 本地原创解析

### 1. 题意重述

本题来自 `一、网格图 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `一、网格图 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
