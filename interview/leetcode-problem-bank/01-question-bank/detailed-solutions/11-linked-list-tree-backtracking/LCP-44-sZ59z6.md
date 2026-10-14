# LCP 44. 开幕式焰火

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sZ59z6/
- 题目 slug：`sZ59z6`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.1 遍历二叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sZ59z6/solutions/1016949/go-dfsha-xi-biao-by-endlesscheng-lqli/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二叉树 DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/sZ59z6/solutions/1016949/go-dfsha-xi-biao-by-endlesscheng-lqli/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-dfsha-xi-biao-by-endlesscheng-lqli`
- topic id：`1016949`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

遍历的同时用哈希集合统计节点值。

答案为哈希集合的大小。

```py [sol-Python3]
class Solution:
    def numColor(self, root: TreeNode) -> int:
        s = set()
        def dfs(node: Optional[ListNode]) -> None:
            if node is None:
                return
            s.add(node.val)
            dfs(node.left)
            dfs(node.right)
        dfs(root)
        return len(s)
```

```java [sol-Java]
class Solution {
    public int numColor(TreeNode root) {
        Set<Integer> s = new HashSet<>();
        dfs(root, s);
        return s.size();
    }

    private void dfs(TreeNode node, Set<Integer> s) {
        if (node == null) {
            return;
        }
        s.add(node.val);
        dfs(node.left, s);
        dfs(node.right, s);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numColor(TreeNode* root) {
        unordered_set<int> s;
        auto dfs = [&](auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) {
                return;
            }
            s.insert(node->val);
            dfs(dfs, node->left);
            dfs(dfs, node->right);
        };
        dfs(dfs, root);
        return s.size();
    }
};
```

```go [sol-Go]
func numColor(root *TreeNode) int {
    set := map[int]struct{}{}
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil {
            return
        }
        set[node.Val] = struct{}{}
        dfs(node.Left)
        dfs(node.Right)
    }
    dfs(root)
    return len(set)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面的二叉树题单。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
