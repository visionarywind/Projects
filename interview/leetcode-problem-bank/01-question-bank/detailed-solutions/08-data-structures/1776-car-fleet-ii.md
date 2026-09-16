# 1776. 车队 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/car-fleet-ii/
- 题目 slug：`car-fleet-ii`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.3 邻项消除
- 难度分：2531
- 外部题解来源：https://leetcode.cn/problems/car-fleet-ii/solutions/4023869/dao-xu-bian-li-pythonjavacgo-by-endlessc-49w7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[倒序遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/car-fleet-ii/solutions/4023869/dao-xu-bian-li-pythonjavacgo-by-endlessc-49w7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dao-xu-bian-li-pythonjavacgo-by-endlessc-49w7`
- topic id：`4023869`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设从左到右有四辆车 $A,B,C,D$。如果 $B$ 与 $C$ 的相遇用时比 $C$ 与 $D$ 的相遇用时还长，那么可以得到如下结论：

1. $B$ 一定不会与 $C$ 相遇（$C$ 已经与 $D$ 合并成一个车队，可以视作删除了 $C$）。
2. 对于在 $B$ 左边的车 $A$，就算 $A$ 的车速很快，它也必须先与 $B$ 相遇。而一旦 $A$ 与 $B$ 合并，合并后的车速就是 $B$ 的车速。由结论 1 可知，$A$ 不会再与 $C$ 相遇。
3. 综上，$C$ 对于 $B$ 及其左边的所有车来说都不再是有效的相遇对象，可以直接从候选车辆中删除。

> 注：$A$ 不一定先与 $B$ 相遇。可能先 $B$ 与 $D$ 相遇，然后 $A$ 与 $D$ 相遇。

具体地，倒着遍历 $\textit{cars}$，同时用栈维护遍历过的车的下标。

对于车 $i$ 与栈顶车 $j$：

- 如果 $i$ 比 $j$ 的车速更慢（或者相等），那么车 $i$ 无法与车 $j$ 相遇，弹出栈顶，继续循环。
- 如果 $\textit{answer}[j]\ne -1$ 且两车相遇用时 $>\textit{answer}[j]$，说明车 $j$ 在相遇前就已经合并（删除），弹出栈顶，继续循环。
- 否则，车 $i$ 与车 $j$ 的相遇早于（或等于）$\textit{answer}[j]$，那么 $\textit{answer}[i]$ 就是车 $i$ 与车 $j$ 的相遇用时，跳出循环。

如果栈为空也没算出 $\textit{answer}[i]$，则说明车 $i$ 不会与下一辆车相遇，$\textit{answer}[i]=-1$。

```py [sol-Python3]
class Solution:
    def getCollisionTimes(self, cars: List[List[int]]) -> List[float]:
        ans = [-1.0] * len(cars)
        st = []  # 栈底用时长，栈顶用时短
        for i in range(len(cars) - 1, -1, -1):
            p, v = cars[i]
            while st:
                j = st[-1]
                q, w = cars[j]
                if v > w:
                    t = (q - p) / (v - w)
                    if ans[j] < 0 or t <= ans[j]:  # 车 i 与车 j 相遇
                        ans[i] = t
                        break
                # 车 j 已合并（删除）
                st.pop()
            st.append(i)
        return ans
```

```java [sol-Java]
class Solution {
    public double[] getCollisionTimes(int[][] cars) {
        // 更快的写法见【Java 数组】
        double[] ans = new double[cars.length];
        ArrayDeque<Integer> st = new ArrayDeque<>(); // 栈底用时长，栈顶用时短

        for (int i = cars.length - 1; i >= 0; i--) {
            int p = cars[i][0];
            int v = cars[i][1];
            while (!st.isEmpty()) {
                int j = st.peek();
                int w = cars[j][1];
                if (v > w) {
                    double t = (double) (cars[j][0] - p) / (v - w);
                    if (ans[j] < 0 || t <= ans[j]) { // 车 i 与车 j 相遇
                        ans[i] = t;
                        break;
                    }
                }
                // 车 j 已合并（删除）
                st.pop();
            }
            if (st.isEmpty()) { // 车 i 不会与下一辆车相遇
                ans[i] = -1;
            }
            st.push(i);
        }

        return ans;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public double[] getCollisionTimes(int[][] cars) {
        int n = cars.length;
        double[] ans = new double[n];
        int[] st = new int[n];
        int top = -1;

        for (int i = n - 1; i >= 0; i--) {
            int p = cars[i][0];
            int v = cars[i][1];
            while (top >= 0) {
                int j = st[top];
                int w = cars[j][1];
                if (v > w) {
                    double t = (double) (cars[j][0] - p) / (v - w);
                    if (ans[j] < 0 || t <= ans[j]) { // 车 i 与车 j 相遇
                        ans[i] = t;
                        break;
                    }
                }
                // 车 j 已合并（删除）
                top--;
            }
            if (top < 0) { // 车 i 不会与下一辆车相遇
                ans[i] = -1;
            }
            st[++top] = i;
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<double> getCollisionTimes(vector<vector<int>>& cars) {
        vector<double> ans(cars.size(), -1);
        stack<int> st; // 栈底用时长，栈顶用时短
        for (int i = cars.size() - 1; i >= 0; i--) {
            int p = cars[i][0], v = cars[i][1];
            while (!st.empty()) {
                int j = st.top();
                int w = cars[j][1];
                if (v > w) {
                    double t = 1.0 * (cars[j][0] - p) / (v - w);
                    if (ans[j] < 0 || t <= ans[j]) { // 车 i 与车 j 相遇
                        ans[i] = t;
                        break;
                    }
                }
                // 车 j 已合并（删除）
                st.pop();
            }
            st.push(i);
        }
        return ans;
    }
};
```

```go [sol-Go]
func getCollisionTimes(cars [][]int) []float64 {
	ans := make([]float64, len(cars))
	st := []int{} // 栈底用时长，栈顶用时短
	for i := len(cars) - 1; i >= 0; i-- {
		p, v := cars[i][0], cars[i][1]
		for len(st) > 0 {
			j := st[len(st)-1]
			w := cars[j][1]
			if v > w {
				t := float64(cars[j][0]-p) / float64(v-w)
				if ans[j] < 0 || t <= ans[j] { // 车 i 与车 j 相遇
					ans[i] = t
					break
				}
			}
			// 车 j 已合并（删除）
			st = st[:len(st)-1]
		}
		if len(st) == 0 { // 车 i 不会与下一辆车相遇
			ans[i] = -1
		}
		st = append(st, i)
	}
	return ans
}
```

> **注**：如果想尽量避免浮点误差，可以把除法 $\dfrac{a}{b} \le \dfrac{c}{d}$ 改成等价的乘法 $ad \le bc$。

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{cars}$ 的长度。虽然我们写了个二重循环，但站在每个下标的视角看，一个下标在二重循环中最多入栈出栈各一次，因此**总**循环次数是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§3.3 邻项消除**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.3 邻项消除`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.3 邻项消除`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
