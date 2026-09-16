# 1298. 你能从盒子里获得的最大糖果数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-candies-you-can-get-from-boxes/
- 题目 slug：`maximum-candies-you-can-get-from-boxes`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.1 深度优先搜索（DFS）
- 难度分：1825
- 外部题解来源：https://leetcode.cn/problems/maximum-candies-you-can-get-from-boxes/solutions/3683782/dfs-xie-fa-pythonjavacgo-by-endlesscheng-g6wx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-candies-you-can-get-from-boxes/solutions/3683782/dfs-xie-fa-pythonjavacgo-by-endlesscheng-g6wx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dfs-xie-fa-pythonjavacgo-by-endlesscheng-g6wx`
- topic id：`3683782`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

为简化处理逻辑，把 $\textit{status}[i]=1$ 当作我们有第 $i$ 个盒子的钥匙。

遍历 $\textit{initialBoxes}$，从 $x=\textit{initialBoxes}[i]$ 开始 DFS：

- 把 $\textit{candies}[x]$ 加到答案中。
- 遍历 $\textit{keys}[x]$ 中的钥匙 $y$，如果我们有对应的盒子，那么就打开盒子，继续递归 $y$。（这说明我们先找到盒子，然后找到钥匙）
- 遍历 $\textit{containedBoxes}[x]$ 中的盒子 $y$，如果我们有对应的钥匙，那么就打开盒子，继续递归 $y$。（这说明我们先找到钥匙，然后找到盒子）

代码实现时，用数组 $\textit{hasKey}$ 记录我们有哪些钥匙，用数组 $\textit{hasBox}$ 记录我们有哪些盒子。

⚠**注意**：在 DFS 的过程中，可能会因为找到了钥匙，打开在 $\textit{initialBoxes}$ 中的盒子。比如从 $\textit{initialBoxes}[0]$ 开始 DFS，我们可能会在 DFS 中打开 $\textit{initialBoxes}[1]$。DFS 结束，向后遍历到 $\textit{initialBoxes}[1]$，这个 $\textit{initialBoxes}[1]$ 是不能重复 DFS 的。为避免重复 DFS，可以把访问过的元素做个标记，比如用 $\textit{vis}$ 数组。或者直接修改 $\textit{hasBox}$，标记这个盒子没有找到。

### 答疑

**问**：这样 DFS，会不会漏算？比如找到钥匙但还没有找到盒子，或者找到盒子但还没有找到钥匙。

**答**：不会漏算。「找到盒子」和「找到钥匙」一定有个先后顺序。如果找到钥匙但还没有找到盒子，那么后面找到盒子时，就会递归这个盒子；如果找到盒子但还没有找到钥匙，那么后面找到钥匙时，就会递归对应的盒子。

```py [sol-Python3]
class Solution:
    def maxCandies(self, status: List[int], candies: List[int], keys: List[List[int]], containedBoxes: List[List[int]], initialBoxes: List[int]) -> int:
        ans = 0
        has_key = status  # 把开着的盒子当作有钥匙
        has_box = [False] * len(status)
        for x in initialBoxes:
            has_box[x] = True

        def dfs(x: int) -> None:
            nonlocal ans
            ans += candies[x]
            has_box[x] = False  # 避免找到钥匙后重新访问开着的盒子

            # 找到钥匙，打开盒子（说明我们先找到盒子，然后找到钥匙）
            for y in keys[x]:
                has_key[y] = True
                if has_box[y]:
                    dfs(y)

            # 找到盒子，使用钥匙（说明我们先找到钥匙，然后找到盒子）
            for y in containedBoxes[x]:
                has_box[y] = True
                if has_key[y]:
                    dfs(y)

        for x in initialBoxes:
            if has_key[x] and has_box[x]:  # 注意 dfs 中会修改 has_box
                dfs(x)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans = 0;

    public int maxCandies(int[] status, int[] candies, int[][] keys, int[][] containedBoxes, int[] initialBoxes) {
        int[] hasKey = status; // 把开着的盒子当作有钥匙
        boolean[] hasBox = new boolean[status.length];
        for (int x : initialBoxes) {
            hasBox[x] = true;
        }

        for (int x : initialBoxes) {
            if (hasBox[x] && hasKey[x] == 1) { // 注意 dfs 中会修改 hasBox
                dfs(x, candies, keys, containedBoxes, hasKey, hasBox);
            }
        }
        return ans;
    }

    private void dfs(int x, int[] candies, int[][] keys, int[][] containedBoxes, int[] hasKey, boolean[] hasBox) {
        ans += candies[x];
        hasBox[x] = false; // 避免找到钥匙后重新访问开着的盒子

        // 找到钥匙，打开盒子（说明我们先找到盒子，然后找到钥匙）
        for (int y : keys[x]) {
            hasKey[y] = 1;
            if (hasBox[y]) {
                dfs(y, candies, keys, containedBoxes, hasKey, hasBox);
            }
        }

        // 找到盒子，使用钥匙（说明我们先找到钥匙，然后找到盒子）
        for (int y : containedBoxes[x]) {
            hasBox[y] = true;
            if (hasKey[y] == 1) {
                dfs(y, candies, keys, containedBoxes, hasKey, hasBox);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxCandies(vector<int>& status, vector<int>& candies, vector<vector<int>>& keys, vector<vector<int>>& containedBoxes, vector<int>& initialBoxes) {
        int ans = 0;
        auto& has_key = status; // 把开着的盒子当作有钥匙
        vector<uint8_t> has_box(status.size());
        for (int x : initialBoxes) {
            has_box[x] = true;
        }

        auto dfs = [&](this auto&& dfs, int x) -> void {
            ans += candies[x];
            has_box[x] = false; // 避免找到钥匙后重新访问开着的盒子

            // 找到钥匙，打开盒子（说明我们先找到盒子，然后找到钥匙）
            for (int y : keys[x]) {
                has_key[y] = true;
                if (has_box[y]) {
                    dfs(y);
                }
            }

            // 找到盒子，使用钥匙（说明我们先找到钥匙，然后找到盒子）
            for (int y : containedBoxes[x]) {
                has_box[y] = true;
                if (has_key[y]) {
                    dfs(y);
                }
            }
        };

        for (int x : initialBoxes) {
            if (has_key[x] && has_box[x]) { // 注意 dfs 中会修改 has_box
                dfs(x);
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int maxCandies(int* status, int statusSize, int* candies, int candiesSize, int** keys, int keysSize, int* keysColSize, int** containedBoxes, int containedBoxesSize, int* containedBoxesColSize, int* initialBoxes, int initialBoxesSize) {
    int ans = 0;
    int* has_key = status; // 把开着的盒子当作有钥匙
    bool* has_box = calloc(statusSize, sizeof(bool));
    for (int i = 0; i < initialBoxesSize; i++) {
        has_box[initialBoxes[i]] = true;
    }

    void dfs(int x) {
        ans += candies[x];
        has_box[x] = false; // 避免找到钥匙后重新访问开着的盒子

        // 找到钥匙，打开盒子（说明我们先找到盒子，然后找到钥匙）
        for (int i = 0; i < keysColSize[x]; i++) {
            int y = keys[x][i];
            has_key[y] = true;
            if (has_box[y]) {
                dfs(y);
            }
        }
    
        // 找到盒子，使用钥匙（说明我们先找到钥匙，然后找到盒子）
        for (int i = 0; i < containedBoxesColSize[x]; i++) {
            int y = containedBoxes[x][i];
            has_box[y] = true;
            if (has_key[y]) {
                dfs(y);
            }
        }
    }

    for (int i = 0; i < initialBoxesSize; i++) {
        int x = initialBoxes[i];
        if (has_key[x] && has_box[x]) { // 注意 dfs 中会修改 has_box
            dfs(x);
        }
    }

    free(has_box);
    return ans;
}
```

```go [sol-Go]
func maxCandies(status []int, candies []int, keys [][]int, containedBoxes [][]int, initialBoxes []int) (ans int) {
	hasKey := status // 把开着的盒子当作有钥匙
	hasBox := make([]bool, len(status))
	for _, x := range initialBoxes {
		hasBox[x] = true
	}

	var dfs func(int)
	dfs = func(x int) {
		ans += candies[x]
		hasBox[x] = false // 避免找到钥匙后重新访问开着的盒子

		// 找到钥匙，打开盒子（说明我们先找到盒子，然后找到钥匙）
		for _, y := range keys[x] {
			hasKey[y] = 1
			if hasBox[y] {
				dfs(y)
			}
		}

		// 找到盒子，使用钥匙（说明我们先找到钥匙，然后找到盒子）
		for _, y := range containedBoxes[x] {
			hasBox[y] = true
			if hasKey[y] == 1 {
				dfs(y)
			}
		}
	}

	for _, x := range initialBoxes {
		if hasBox[x] && hasKey[x] == 1 { // 注意 dfs 中会修改 hasBox
			dfs(x)
		}
	}
	return
}
```

```js [sol-JavaScript]
var maxCandies = function(status, candies, keys, containedBoxes, initialBoxes) {
    let ans = 0;
    const hasKey = status; // 把开着的盒子当作有钥匙
    const hasBox = Array(status.length).fill(false);
    for (const x of initialBoxes) {
        hasBox[x] = true;
    }

    function dfs(x) {
        ans += candies[x];
        hasBox[x] = false; // 避免找到钥匙后重新访问开着的盒子

        // 找到钥匙，打开盒子（说明我们先找到盒子，然后找到钥匙）
        for (const y of keys[x]) {
            hasKey[y] = true;
            if (hasBox[y]) {
                dfs(y);
            }
        }

        // 找到盒子，使用钥匙（说明我们先找到钥匙，然后找到盒子）
        for (const y of containedBoxes[x]) {
            hasBox[y] = true;
            if (hasKey[y]) {
                dfs(y);
            }
        }
    }

    for (const x of initialBoxes) {
        if (hasKey[x] && hasBox[x]) { // 注意 dfs 中会修改 hasBox
            dfs(x);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_candies(status: Vec<i32>, candies: Vec<i32>, keys: Vec<Vec<i32>>, contained_boxes: Vec<Vec<i32>>, initial_boxes: Vec<i32>) -> i32 {
        let mut has_key = status; // 把开着的盒子当作有钥匙
        let mut has_box = vec![false; has_key.len()];
        for &x in &initial_boxes {
            has_box[x as usize] = true;
        }

        fn dfs(x: usize, candies: &[i32], keys: &[Vec<i32>], contained_boxes: &[Vec<i32>], has_key: &mut [i32], has_box: &mut [bool], ans: &mut i32) {
            *ans += candies[x];
            has_box[x] = false; // 避免找到钥匙后重新访问开着的盒子

            // 找到钥匙，打开盒子（说明我们先找到盒子，然后找到钥匙）
            for &y in &keys[x] {
                let y = y as usize;
                has_key[y] = 1;
                if has_box[y] {
                    dfs(y, candies, keys, contained_boxes, has_key, has_box, ans);
                }
            }

            // 找到盒子，使用钥匙（说明我们先找到钥匙，然后找到盒子）
            for &y in &contained_boxes[x] {
                let y = y as usize;
                has_box[y] = true;
                if has_key[y] == 1 {
                    dfs(y, candies, keys, contained_boxes, has_key, has_box, ans);
                }
            }
        }

        let mut ans = 0;
        for x in initial_boxes {
            let x = x as usize;
            if has_box[x] && has_key[x] == 1 { // 注意 dfs 中会修改 has_box
                dfs(x, &candies, &keys, &contained_boxes, &mut has_key, &mut has_box, &mut ans);
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+L)$，其中 $n$ 是 $\textit{keys}$ 的长度，$L$ 是所有 $\textit{keys}[i]$ 的长度之和。可能不同的盒子中有相同的钥匙，遍历这些数据一共需要 $\mathcal{O}(n+L)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `一、图的遍历 / §1.1 深度优先搜索（DFS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.1 深度优先搜索（DFS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
