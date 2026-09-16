# 3027. 人员站位的方案数 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-number-of-ways-to-place-people-ii/
- 题目 slug：`find-the-number-of-ways-to-place-people-ii`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶
- 难度分：2020
- 外部题解来源：https://leetcode.cn/problems/find-the-number-of-ways-to-place-people-ii/solutions/2630655/on2-you-ya-mei-ju-by-endlesscheng-z86d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[利用排序简化问题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-the-number-of-ways-to-place-people-ii/solutions/2630655/on2-you-ya-mei-ju-by-endlesscheng-z86d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on2-you-ya-mei-ju-by-endlesscheng-z86d`
- topic id：`2630655`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设矩形的左上角为 $(x_1,y_1)$，右下角为 $(x_2,y_2)$。

根据题意，合法矩形需要满足如下三个要求：

1. $x_1\le x_2$。
2. $y_1\ge y_2$。
3. 矩形内部或边界上没有其他点。

为方便计算，先把 $\textit{points}$ 按照横坐标**从小到大**排序（横坐标相同呢？后面会说明）。排序后，第一个要求在枚举时自动成立。

然后外层循环枚举 $\textit{points}[i] = (x_1,y_1)$；内层循环从 $i+1$ 开始，枚举 $\textit{points}[j] = (x_2,y_2)$，跳过 $y_2 > y_1$ 的点。这样就满足了第二个要求。

最后来处理第三个要求，其等价于：

- 对于下标在闭区间 $[i+1,j-1]$ 中的每个点，纵坐标要么大于 $y_1$，要么小于 $y_2$。如果不满足，那么这个点就在矩形内部或边界上了。

纵坐标大于 $y_1$ 的点，我们已经跳过了，所以只需满足纵坐标小于 $y_2$。

枚举到 $\textit{points}[j] = (x_2,y_2)$ 时，之前遍历过的点，纵坐标都必须小于 $y_2$。难道要再遍历一遍 $[i+1,j-1]$？不需要，只要这些点的纵坐标的**最大值**小于 $y_2$，那么这些点的纵坐标就都小于 $y_2$（$\mathcal{O}(1)$ 时间获取 $\mathcal{O}(n)$ 信息）。所以只需维护遍历过的点的纵坐标的最大值 $\textit{maxY}$。如果发现 $y_2> \textit{maxY}$，就找到了一个合法矩形，把答案加一。

最后来说，对于横坐标相同的点，要怎么处理。

比如 $\textit{points} = [(1,3),(1,2),(1,1)]$，其中有两个满足要求的矩形：$(1,3)$ 为左上角，$(1,2)$ 为右下角的矩形；$(1,2)$ 为左上角，$(1,1)$ 为右下角的矩形。注意 $(1,3)$ 为左上角，$(1,1)$ 为右下角的矩形包含 $(1,2)$，不满足要求。

由于内层循环中的 $j$ 是从 $i+1$ 开始枚举的，所以在横坐标相同时，要按照纵坐标**从大到小**排序。这也保证了横坐标相同时，我们总是会先遍历到纵坐标更大的点。在上面的例子中，遍历到 $(1,1)$ 之前，一定会先遍历到 $(1,2)$。这样对于 $(1,3)$ 为左上角，$(1,1)$ 为右下角的矩形，我们就能正确地判断出这个矩形是不合法的。

**优化**：如果在循环中发现 $\textit{maxY} = y_1$，那么不存在满足要求的 $y_2$，此时可以退出循环。

[视频讲解](https://www.bilibili.com/video/BV14C411r7nN/) 第四题。

```py [sol-Python3]
class Solution:
    def numberOfPairs(self, points: List[List[int]]) -> int:
        points.sort(key=lambda p: (p[0], -p[1]))  # x 升序，y 降序
        ans = 0
        for i, (_, y1) in enumerate(points):
            max_y = -inf
            for (_, y2) in points[i + 1:]:
                if y1 >= y2 > max_y:
                    max_y = y2
                    ans += 1
                if max_y == y1:  # 优化
                    break
        return ans
```

```java [sol-Java]
class Solution {
    public int numberOfPairs(int[][] points) {
        // x 升序，y 降序
        Arrays.sort(points, (a, b) -> a[0] != b[0] ? a[0] - b[0] : b[1] - a[1]);
        int ans = 0;
        for (int i = 0; i < points.length; i++) {
            int y1 = points[i][1];
            int maxY = Integer.MIN_VALUE;
            for (int j = i + 1; j < points.length && maxY < y1; j++) {
                int y2 = points[j][1];
                if (y2 <= y1 && y2 > maxY) {
                    maxY = y2;
                    ans++;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfPairs(vector<vector<int>>& points) {
        // x 升序，y 降序
        ranges::sort(points, {}, [](auto& p) { return pair(p[0], -p[1]); });
        int ans = 0, n = points.size();
        for (int i = 0; i < n; i++) {
            int y1 = points[i][1];
            int max_y = INT_MIN;
            for (int j = i + 1; j < n && max_y < y1; j++) {
                int y2 = points[j][1];
                if (y2 <= y1 && y2 > max_y) {
                    max_y = y2;
                    ans++;
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* p, const void* q) {
    int* a = *(int**)p;
    int* b = *(int**)q;
    // x 升序，y 降序
    return a[0] != b[0] ? a[0] - b[0] : b[1] - a[1];
}

int numberOfPairs(int** points, int pointsSize, int* pointsColSize) {
    qsort(points, pointsSize, sizeof(int*), cmp);
    int ans = 0;
    for (int i = 0; i < pointsSize; i++) {
        int y1 = points[i][1];
        int max_y = INT_MIN;
        for (int j = i + 1; j < pointsSize && max_y < y1; j++) {
            int y2 = points[j][1];
            if (y2 <= y1 && y2 > max_y) {
                max_y = y2;
                ans++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func numberOfPairs(points [][]int) (ans int) {
	// x 升序，y 降序
	slices.SortFunc(points, func(a, b []int) int { return cmp.Or(a[0]-b[0], b[1]-a[1]) })
	for i, p := range points {
		y1 := p[1]
		maxY := math.MinInt
		for _, q := range points[i+1:] {
			y2 := q[1]
			if y2 <= y1 && y2 > maxY {
				maxY = y2
				ans++
			}
			if maxY == y1 { // 优化
				break
			}
		}
	}
	return
}
```

```js [sol-JavaScript]
var numberOfPairs = function(points) {
    points.sort((a, b) => a[0] - b[0] || b[1] - a[1]); // x 升序，y 降序
    const n = points.length;
    let ans = 0;
    for (let i = 0; i < n; i++) {
        const y1 = points[i][1];
        let max_y = -Infinity;
        for (let j = i + 1; j < n && max_y < y1; j++) {
            const y2 = points[j][1];
            if (y2 <= y1 && y2 > max_y) {
                max_y = y2;
                ans++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn number_of_pairs(mut points: Vec<Vec<i32>>) -> i32 {
        points.sort_unstable_by_key(|p| (p[0], -p[1])); // x 升序，y 降序
        let mut ans = 0;
        for (i, p) in points.iter().enumerate() {
            let y1 = p[1];
            let mut max_y = i32::MIN;
            for q in &points[i + 1..] {
                let y2 = q[1];
                if y2 <= y1 && y2 > max_y {
                    max_y = y2;
                    ans += 1;
                }
                if max_y == y1 { // 优化
                    break;
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{points}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。Python 可以用循环代替切片。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
