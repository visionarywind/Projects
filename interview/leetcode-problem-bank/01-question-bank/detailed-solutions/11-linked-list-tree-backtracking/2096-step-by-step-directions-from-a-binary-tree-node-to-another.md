# 2096. 从二叉树一个节点到另一个节点每一步的方向

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/step-by-step-directions-from-a-binary-tree-node-to-another/
- 题目 slug：`step-by-step-directions-from-a-binary-tree-node-to-another`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.8 最近公共祖先
- 难度分：1805
- 外部题解来源：https://leetcode.cn/problems/step-by-step-directions-from-a-binary-tree-node-to-another/solutions/1139430/bfs-zuo-fa-by-endlesscheng-dfcf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 求路径（Python/Java/C++/C/Go）](https://leetcode.cn/problems/step-by-step-directions-from-a-binary-tree-node-to-another/solutions/1139430/bfs-zuo-fa-by-endlesscheng-dfcf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfs-zuo-fa-by-endlesscheng-dfcf`
- topic id：`1139430`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

首先，用 DFS 求出从根节点到 $\textit{startValue}$ 的移动路径，以及从根节点到 $\textit{destValue}$ 的移动路径。

例如：

- 从根节点到 $\textit{startValue}$ 的移动路径为 $\texttt{RRLL}$。
- 从根节点到 $\textit{destValue}$ 的移动路径为 $\texttt{RRRL}$。

两条路径的最长公共前缀为 $\texttt{RR}$，然后分道扬镳。根据最近公共祖先的定义，分道扬镳的位置就是最近公共祖先。

- 从最近公共祖先到 $\textit{startValue}$ 的移动路径为 $\texttt{LL}$。
- 从最近公共祖先到 $\textit{destValue}$ 的移动路径为 $\texttt{RL}$。

所以从 $\textit{startValue}$ 到 $\textit{destValue}$ 的移动路径为：

1. 从 $\textit{startValue}$ 往上走两步到最近公共祖先，即 $\texttt{UU}$。
2. 从最近公共祖先到 $\textit{destValue}$，即 $\texttt{RL}$。
3. 拼接得到 $\texttt{UURL}$。

## 写法一：两次 DFS

```py [sol-Python3]
class Solution:
    def getDirections(self, root: Optional[TreeNode], startValue: int, destValue: int) -> str:
        path = []

        # dfs 返回是否找到了 target
        def dfs(node: Optional[TreeNode], target: int) -> bool:
            if node is None:
                return False

            # 找到 target
            if node.val == target:
                return True

            # 往左找 target
            path.append('L')
            if dfs(node.left, target):
                return True

            # 往右找 target
            path[-1] = 'R'  # 直接覆盖，无需恢复现场
            if dfs(node.right, target):
                return True

            path.pop()  # 恢复现场
            return False

        dfs(root, startValue)
        path_to_start = path.copy()

        path.clear()
        dfs(root, destValue)
        path_to_dest = path

        i = 0
        while i < min(len(path_to_start), len(path_to_dest)) and path_to_start[i] == path_to_dest[i]:
            i += 1

        return 'U' * (len(path_to_start) - i) + ''.join(path_to_dest[i:])
```

```java [sol-Java]
class Solution {
    public String getDirections(TreeNode root, int startValue, int destValue) {
        StringBuilder path = new StringBuilder();

        // dfs 返回是否找到了 target
        dfs(root, startValue, path);
        String pathToStart = path.toString();

        path.setLength(0);
        dfs(root, destValue, path);
        StringBuilder pathToDest = path;

        int i = 0;
        while (i < Math.min(pathToStart.length(), pathToDest.length()) && pathToStart.charAt(i) == pathToDest.charAt(i)) {
            i++;
        }

        return "U".repeat(pathToStart.length() - i) + pathToDest.substring(i);
    }

    // dfs 返回是否找到了 target
    private boolean dfs(TreeNode node, int target, StringBuilder path) {
        if (node == null) {
            return false;
        }

        // 找到 target
        if (node.val == target) {
            return true;
        }

        // 往左找 target
        path.append('L');
        if (dfs(node.left, target, path)) {
            return true;
        }

        // 往右找 target
        path.setCharAt(path.length() - 1, 'R'); // 直接覆盖，无需恢复现场
        if (dfs(node.right, target, path)) {
            return true;
        }

        path.setLength(path.length() - 1); // 恢复现场
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string getDirections(TreeNode* root, int startValue, int destValue) {
        string path;

        // dfs 返回是否找到了 target
        auto dfs = [&](this auto&& dfs, TreeNode* node, int target) -> bool {
            if (node == nullptr) {
                return false;
            }

            // 找到 target
            if (node->val == target) {
                return true;
            }

            // 往左找 target
            path += 'L';
            if (dfs(node->left, target)) {
                return true;
            }

            // 往右找 target
            path.back() = 'R'; // 直接覆盖，无需恢复现场
            if (dfs(node->right, target)) {
                return true;
            }

            path.pop_back(); // 恢复现场
            return false;
        };

        dfs(root, startValue);
        string path_to_start = path;

        path.clear();
        dfs(root, destValue);
        string path_to_dest = move(path);

        int i = 0;
        while (i < min(path_to_start.size(), path_to_dest.size()) && path_to_start[i] == path_to_dest[i]) {
            i++;
        }

        return string(path_to_start.size() - i, 'U') + path_to_dest.substr(i);
    }
};
```

```go [sol-Go]
func getDirections(root *TreeNode, startValue, destValue int) string {
	path := []byte{}
	// dfs 返回是否找到了 target
	var dfs func(*TreeNode, int) bool
	dfs = func(node *TreeNode, target int) bool {
		if node == nil {
			return false
		}

		// 找到 target
		if node.Val == target {
			return true
		}

		// 往左找 target
		path = append(path, 'L')
		if dfs(node.Left, target) {
			return true
		}

		// 往右找 target
		path[len(path)-1] = 'R' // 直接覆盖，无需恢复现场
		if dfs(node.Right, target) {
			return true
		}

		path = path[:len(path)-1] // 恢复现场
		return false
	}

	dfs(root, startValue)
	pathToStart := path

	path = []byte{}
	dfs(root, destValue)
	pathToDest := path

	i := 0
	for i < min(len(pathToStart), len(pathToDest)) && pathToStart[i] == pathToDest[i] {
		i++
	}

	return strings.Repeat("U", len(pathToStart)-i) + string(pathToDest[i:])
}
```

## 写法二：一次 DFS

```py [sol-Python3]
class Solution:
    def getDirections(self, root: Optional[TreeNode], startValue: int, destValue: int) -> str:
        path_to_start = path_to_dest = None
        path = []

        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:
                return

            nonlocal path_to_start, path_to_dest
            if node.val == startValue:
                path_to_start = path.copy()
            elif node.val == destValue:
                path_to_dest = path.copy()

            # 往左找
            path.append('L')
            dfs(node.left)

            # 往右找
            path[-1] = 'R'  # 直接覆盖，无需恢复现场
            dfs(node.right)
            path.pop()  # 恢复现场

        dfs(root)

        i = 0
        while i < min(len(path_to_start), len(path_to_dest)) and path_to_start[i] == path_to_dest[i]:
            i += 1

        return 'U' * (len(path_to_start) - i) + ''.join(path_to_dest[i:])
```

```java [sol-Java]
class Solution {
    private String pathToStart, pathToDest;

    public String getDirections(TreeNode root, int startValue, int destValue) {
        StringBuilder path = new StringBuilder();
        dfs(root, startValue, destValue, path);

        int i = 0;
        while (i < Math.min(pathToStart.length(), pathToDest.length()) && pathToStart.charAt(i) == pathToDest.charAt(i)) {
            i++;
        }

        return "U".repeat(pathToStart.length() - i) + pathToDest.substring(i);
    }

    private void dfs(TreeNode node, int startValue, int destValue, StringBuilder path) {
        if (node == null) {
            return;
        }

        if (node.val == startValue) {
            pathToStart = path.toString();
        } else if (node.val == destValue) {
            pathToDest = path.toString();
        }

        // 往左找
        path.append('L');
        dfs(node.left, startValue, destValue, path);

        // 往右找
        path.setCharAt(path.length() - 1, 'R'); // 直接覆盖，无需恢复现场
        dfs(node.right, startValue, destValue, path);
        path.setLength(path.length() - 1); // 恢复现场
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string getDirections(TreeNode* root, int startValue, int destValue) {
        string path_to_start, path_to_dest, path;

        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) {
                return;
            }

            if (node->val == startValue) {
                path_to_start = path;
            } else if (node->val == destValue) {
                path_to_dest = path;
            }

            // 往左找
            path += 'L';
            dfs(node->left);

            // 往右找
            path.back() = 'R'; // 直接覆盖，无需恢复现场
            dfs(node->right);
            path.pop_back(); // 恢复现场
        };

        dfs(root);

        int i = 0;
        while (i < min(path_to_start.size(), path_to_dest.size()) && path_to_start[i] == path_to_dest[i]) {
            i++;
        }

        return string(path_to_start.size() - i, 'U') + path_to_dest.substr(i);
    }
};
```

```go [sol-Go]
func getDirections(root *TreeNode, startValue, destValue int) string {
	var pathToStart, pathToDest string
	path := []byte{}
	var dfs func(*TreeNode)
	dfs = func(node *TreeNode) {
		if node == nil {
			return
		}

		if node.Val == startValue {
			pathToStart = string(path)
		} else if node.Val == destValue {
			pathToDest = string(path)
		}

		// 往左找
		path = append(path, 'L')
		dfs(node.Left)

		// 往右找
		path[len(path)-1] = 'R' // 直接覆盖，无需恢复现场
		dfs(node.Right)
		path = path[:len(path)-1] // 恢复现场
	}
	dfs(root)

	i := 0
	for i < min(len(pathToStart), len(pathToDest)) && pathToStart[i] == pathToDest[i] {
		i++
	}

	return strings.Repeat("U", len(pathToStart)-i) + pathToDest[i:]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
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

本题来自 `二、二叉树 / §2.8 最近公共祖先`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.8 最近公共祖先`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
