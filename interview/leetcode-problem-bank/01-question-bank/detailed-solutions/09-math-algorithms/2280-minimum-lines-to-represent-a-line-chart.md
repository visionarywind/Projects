# 2280. 表示一个折线图的最少线段数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-lines-to-represent-a-line-chart/
- 题目 slug：`minimum-lines-to-represent-a-line-chart`
- 来源专题：数学算法
- 来源分类路径：五、计算几何 / §5.1 点、线
- 难度分：1681
- 外部题解来源：https://leetcode.cn/problems/minimum-lines-to-represent-a-line-chart/solutions/1510444/pan-duan-san-dian-gong-xian-chao-jian-ji-0n0o/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[判断三点共线](https://leetcode.cn/problems/minimum-lines-to-represent-a-line-chart/solutions/1510444/pan-duan-san-dian-gong-xian-chao-jian-ji-0n0o/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pan-duan-san-dian-gong-xian-chao-jian-ji-0n0o`
- topic id：`1510444`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1RY4y157nW) 已出炉，讨论了一些浮点数的小知识，欢迎三连~

---

按照 $\textit{day}_i$ 排序后，如果相邻两条线段的斜率不同，那么必然需要一条新的线段。

代码实现时，可以假定第一天之前的斜率为 $\dfrac{1}{0}$，从而简化判断逻辑。

如果你用的是除法，要特别小心浮点数带来的影响：如果计算机无法精确表示一个浮点数（你可以理解成一个无限循环的小数），那么计算机会将其舍入到最近的一个能表示的浮点数上，两个本来不同的斜率，由于数值过于接近（比浮点精度还小），计算机会将其舍入到同一个浮点数上，这样本该不同的数值计算机会判定为相同。

改为乘法可以避免此问题。

```python [sol1-Python3]
class Solution:
    def minimumLines(self, stockPrices: List[List[int]]) -> int:
        stockPrices.sort(key=lambda x: x[0])  # 按照 day 排序
        ans, pre_dy, pre_dx = 0, 1, 0
        for (x1, y1), (x2, y2) in pairwise(stockPrices):
            dy, dx = y2 - y1, x2 - x1
            if dy * pre_dx != pre_dy * dx:  # 与上一条线段的斜率不同
                ans += 1
                pre_dy, pre_dx = dy, dx
        return ans
```

```go [sol1-Go]
func minimumLines(a [][]int) (ans int) {
	sort.Slice(a, func(i, j int) bool { return a[i][0] < a[j][0] }) // 按照 day 排序
	for i, preDY, preDX := 1, 1, 0; i < len(a); i++ {
		dy, dx := a[i][1]-a[i-1][1], a[i][0]-a[i-1][0]
		if dy*preDX != preDY*dx { // 与上一条线段的斜率不同
			ans++
			preDY, preDX = dy, dx
		}
	}
	return
}
```

由于斜率之差可以小到 $10^{-18}$ 量级，C++ 选手可以用 `long double` 来保证算出来的结果在误差允许范围内。

```cpp
class Solution {
public:
    int minimumLines(vector<vector<int>> &a) {
        sort(a.begin(), a.end(), [](auto &a, auto &b) { return a[0] < b[0]; });
        int ans = 0;
        long double pre_k = 2e9;
        for (int i = 1; i < a.size(); ++i) {
            long double k = (long double) (a[i][1] - a[i - 1][1]) / (a[i][0] - a[i - 1][0]);
            if (k != pre_k) {
                ans++;
                pre_k = k;
            }
        }
        return ans;
    }
};
```

## 本地原创解析

### 1. 题意重述

本题来自 `五、计算几何 / §5.1 点、线`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、计算几何 / §5.1 点、线`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
