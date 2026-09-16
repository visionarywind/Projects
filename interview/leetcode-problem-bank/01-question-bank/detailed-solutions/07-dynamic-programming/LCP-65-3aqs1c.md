# LCP 65. 舒适的湿度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/3aqs1c/
- 题目 slug：`3aqs1c`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/3aqs1c/solutions/1847193/by-endlesscheng-fu9b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数形结合 + 巧妙地定义状态（Python/Java/C++/Go）](https://leetcode.cn/problems/3aqs1c/solutions/1847193/by-endlesscheng-fu9b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-fu9b`
- topic id：`1847193`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

个人赛五道题目的 [视频讲解](https://www.bilibili.com/video/BV1zN4y1K762) 已出炉，欢迎点赞三连，在评论区分享你对这场比赛的看法~

---

数形结合更好理解，推荐先看视频哦，下面整理了视频中讲的重点。

1. 画折线图，问题转换成最小化折线图中最大值与最小值的差。
2. 定义 $f[i][j]$ 表示考虑 $\textit{operate}$ 的前 $i$ 个数，其中某些数字变成负数后，折线图最右端点到折线图最下端点的纵坐标距离为 $j$ 时，折线图中最大值与最小值的差的最小值（请注意：状态定义中的 $j$ 不是坐标，是到下界的相对距离）。
3. 设 $x=\textit{operate}[i]$，分类讨论（**下面的等号表示左值和右值取 $\min$ 后赋给左值**）：
   - 取正号，折线图往上走：$f[i][j+x] = \max(f[i-1][j],j+x)$；
   - 取负号，折线图往下走，且纵坐标没有小于最下端点的纵坐标：$f[i][j-x] = f[i-1][j]$；
   - 取负号，折线图往下走，且纵坐标小于最下端点的纵坐标，那么产生了一个新的最下端点，按照定义：$f[i][0] = f[i-1][j]-j+x$。
4. 初始值 $f[0][0] = 0$，其余为 $+\infty$。
5. 答案为 $\min(f[n-1])$。
6. 代码实现时，用滚动数组优化空间。

```py [sol1-Python3]
class Solution:
    def unSuitability(self, operate: List[int]) -> int:
        mx = max(operate) * 2
        pre = [0] + [inf] * mx
        for x in operate:
            f = [inf] * (mx + 1)
            for j, dis in enumerate(pre):
                if dis == inf: continue  # 无效的长度（无法组成）
                if j + x <= mx: f[j + x] = min(f[j + x], max(dis, j + x))
                if j >= x: f[j - x] = min(f[j - x], dis)
                else: f[0] = min(f[0], dis - j + x)
            pre = f
        return min(pre)
```

```java [sol1-Java]
class Solution {
    public int unSuitability(int[] operate) {
        var mx = Arrays.stream(operate).max().orElseThrow() * 2 + 1;
        int[] pre = new int[mx], f = new int[mx];
        Arrays.fill(pre, Integer.MAX_VALUE);
        pre[0] = 0;
        for (var x : operate) {
            Arrays.fill(f, Integer.MAX_VALUE);
            for (var j = 0; j < mx; ++j) {
                var dis = pre[j];
                if (dis == Integer.MAX_VALUE) continue; // 无效的长度（无法组成）
                if (j + x < mx) f[j + x] = Math.min(f[j + x], Math.max(dis, j + x));
                if (j >= x) f[j - x] = Math.min(f[j - x], dis);
                else f[0] = Math.min(f[0], dis - j + x);
            }
            var tmp = pre;
            pre = f;
            f = tmp;
        }
        return Arrays.stream(pre).min().orElseThrow();
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int unSuitability(vector<int> &operate) {
        int mx = *max_element(operate.begin(), operate.end()) * 2 + 1;
        int pre[mx], f[mx];
        memset(pre, 0x3f, sizeof(pre));
        pre[0] = 0;
        for (int x : operate) {
            memset(f, 0x3f, sizeof(f));
            for (int j = 0; j < mx; ++j) {
                int dis = pre[j];
                if (dis == 0x3f3f3f3f) continue; // 无效的长度（无法组成）
                if (j + x < mx) f[j + x] = min(f[j + x], max(dis, j + x));
                if (j >= x) f[j - x] = min(f[j - x], dis);
                else f[0] = min(f[0], dis - j + x);
            }
            memcpy(pre, f, sizeof(f));
        }
        return *min_element(pre, pre + mx);
    }
};
```

```go [sol1-Go]
func unSuitability(operate []int) int {
	const inf = math.MaxInt32
	mx := 0
	for _, x := range operate {
		mx = max(mx, x)
	}
	mx *= 2
	pre := make([]int, mx+1)
	for i := range pre {
		pre[i] = inf
	}
	pre[0] = 0
	f := make([]int, mx+1)
	for _, x := range operate {
		for i := range f {
			f[i] = inf
		}
		for j, dis := range pre {
			if pre[j] == inf { // 无效的长度（无法组成）
				continue
			}
			if j+x <= mx {
				f[j+x] = min(f[j+x], max(dis, j+x))
			}
			if j >= x {
				f[j-x] = min(f[j-x], dis)
			} else {
				f[0] = min(f[0], dis-j+x)
			}
		}
		pre, f = f, pre
	}
	ans := inf
	for _, x := range pre {
		ans = min(ans, x)
	}
	return ans
}

func min(a, b int) int { if b < a { return b }; return a }
func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(nU)$，其中 $n$ 为 $\textit{operate}$ 的长度，$U=max(\textit{operate})$。
- 空间复杂度：$O(U)$。

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
