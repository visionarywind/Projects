# 3799. 单词方块 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/word-squares-ii/
- 题目 slug：`word-squares-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.5 排列型回溯
- 难度分：1606
- 外部题解来源：https://leetcode.cn/problems/word-squares-ii/solutions/3872301/pai-lie-xing-hui-su-pythonjavacgo-by-end-3i7x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排列型回溯（Python/Java/C++/Go）](https://leetcode.cn/problems/word-squares-ii/solutions/3872301/pai-lie-xing-hui-su-pythonjavacgo-by-end-3i7x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-lie-xing-hui-su-pythonjavacgo-by-end-3i7x`
- topic id：`3872301`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

前置题目：[46. 全排列](https://leetcode.cn/problems/permutations/)，视频讲解：[排列型回溯【基础算法精讲 16】](https://www.bilibili.com/video/BV1mY411D7f6/)。

按题目要求模拟即可。

注意为了保证答案有序，可以先对 $\textit{words}$ 从小到大排序，再枚举排列。

```py [sol-Python3]
class Solution:
    def wordSquares(self, words: List[str]) -> List[List[str]]:
        words.sort()  # 保证答案有序
        return [[top, left, right, bottom]
                for top, left, right, bottom in permutations(words, 4)
                if top[0] == left[0] and top[3] == right[0] and
                bottom[0] == left[3] and bottom[3] == right[3]]
```

```java [sol-Java]
class Solution {
    public List<List<String>> wordSquares(String[] words) {
        Arrays.sort(words); // 保证答案有序
        
        int[] path = new int[4];
        boolean[] onPath = new boolean[words.length];
        List<List<String>> ans = new ArrayList<>();

        dfs(words, 0, path, onPath, ans);
        return ans;
    }

    private void dfs(String[] words, int i, int[] path, boolean[] onPath, List<List<String>> ans) {
        if (i == 4) {
            String top = words[path[0]];
            String left = words[path[1]];
            String right = words[path[2]];
            String bottom = words[path[3]];
            if (top.charAt(0) == left.charAt(0) && top.charAt(3) == right.charAt(0)
                    && bottom.charAt(0) == left.charAt(3) && bottom.charAt(3) == right.charAt(3)) {
                ans.add(Arrays.asList(top, left, right, bottom));
            }
            return;
        }

        for (int j = 0; j < words.length; j++) {
            if (!onPath[j]) {
                path[i] = j; // 从没有选的下标中选一个
                onPath[j] = true; // 已选上
                dfs(words, i + 1, path, onPath, ans);
                onPath[j] = false; // 恢复现场
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<string>> wordSquares(vector<string>& words) {
        ranges::sort(words); // 保证答案有序

        int path[4];
        vector<int8_t> on_path(words.size());
        vector<vector<string>> ans;

        auto dfs = [&](this auto&& dfs, int i) -> void {
            if (i == 4) {
                string& top = words[path[0]];
                string& left = words[path[1]];
                string& right = words[path[2]];
                string& bottom = words[path[3]];
                if (top[0] == left[0] && top[3] == right[0] && bottom[0] == left[3] && bottom[3] == right[3]) {
                    ans.push_back({top, left, right, bottom});
                }
                return;
            }

            for (int j = 0; j < words.size(); j++) {
                if (!on_path[j]) {
                    path[i] = j; // 从没有选的下标中选一个
                    on_path[j] = true; // 已选上
                    dfs(i + 1);
                    on_path[j] = false; // 恢复现场
                }
            }
        };

        dfs(0);
        return ans;
    }
};
```

```go [sol-Go]
func wordSquares(words []string) (ans [][]string) {
	slices.Sort(words) // 保证答案有序

	path := [4]int{}
	onPath := make([]bool, len(words))

	var dfs func(int)
	dfs = func(i int) {
		if i == 4 {
			top := words[path[0]]
			left := words[path[1]]
			right := words[path[2]]
			bottom := words[path[3]]
			if top[0] == left[0] && top[3] == right[0] && bottom[0] == left[3] && bottom[3] == right[3] {
				ans = append(ans, []string{top, left, right, bottom})
			}
			return
		}

		for j, on := range onPath {
			if !on {
				path[i] = j      // 从没有选的下标中选一个
				onPath[j] = true // 已选上
				dfs(i + 1)
				onPath[j] = false // 恢复现场
			}
		}
	}

	dfs(0)
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(A_n^4) = \mathcal{O}(n^4)$，其中 $n$ 是 $\textit{words}$ 的长度。字符串的长度视作常量。
- 空间复杂度：$\mathcal{O}(1)$。排序的栈开销不计入，返回值不计入。

## 专题训练

见下面回溯题单的「**§4.5 排列型回溯**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.5 排列型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.5 排列型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
