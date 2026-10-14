# 2265. 统计值等于子树平均值的节点数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-nodes-equal-to-average-of-subtree/
- 题目 slug：`count-nodes-equal-to-average-of-subtree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：1473
- 外部题解来源：https://leetcode.cn/problems/count-nodes-equal-to-average-of-subtree/solutions/1477315/tong-ji-zi-shu-de-jie-dian-he-ji-jie-dia-va8t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计子树元素和 & 子树大小（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/count-nodes-equal-to-average-of-subtree/solutions/1477315/tong-ji-zi-shu-de-jie-dian-he-ji-jie-dia-va8t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-ji-zi-shu-de-jie-dian-he-ji-jie-dia-va8t`
- topic id：`1477315`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

本题二叉树递归入门题。不了解递归的同学请看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

为了求出子树平均值，我们需要知道子树的节点值之和 $\textit{sum}$，以及子树节点个数 $\textit{size}$，那么子树平均值就是 $\left\lfloor\dfrac{\textit{sum}}{\textit{size}}\right\rfloor$。

如果当前节点 $\textit{node}$ 满足 $\textit{node}.\textit{val} = \left\lfloor\dfrac{\textit{sum}}{\textit{size}}\right\rfloor$，把答案增加一。

```py [sol-Python3]
class Solution:
    def averageOfSubtree(self, root: TreeNode) -> int:
        ans = 0

        # 返回 node 子树的节点值之和、节点个数
        def dfs(node: TreeNode | None) -> tuple[int, int]:
            if node is None:
                return 0, 0
            left_sum, left_size = dfs(node.left)  # 拆解问题：递归计算左子树的信息
            right_sum, right_size = dfs(node.right)  # 拆解问题：递归计算右子树的信息
            sum_ = left_sum + right_sum + node.val  # node 子树的节点值之和
            size = left_size + right_size + 1  # node 子树的节点个数
            if node.val == sum_ // size:  # 题目要求下取整
                nonlocal ans
                ans += 1
            return sum_, size

        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans = 0;

    public int averageOfSubtree(TreeNode root) {
        dfs(root);
        return ans;
    }

    private int[] dfs(TreeNode node) {
        if (node == null) {
            return new int[]{0, 0};
        }
        int[] left = dfs(node.left); // 拆解问题：递归计算左子树的信息
        int[] right = dfs(node.right); // 拆解问题：递归计算右子树的信息
        int sum = left[0] + right[0] + node.val; // node 子树的节点值之和
        int size = left[1] + right[1] + 1; // node 子树的节点个数
        if (node.val == sum / size) { // 题目要求下取整
            ans++;
        }
        return new int[]{sum, size};
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int averageOfSubtree(TreeNode* root) {
        int ans = 0;

        // 返回 node 子树的节点值之和、节点个数
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> pair<int, int> {
            if (node == nullptr) {
                return {0, 0};
            }
            auto [left_sum, left_size] = dfs(node->left); // 拆解问题：递归计算左子树的信息
            auto [right_sum, right_size] = dfs(node->right); // 拆解问题：递归计算右子树的信息
            int sum = left_sum + right_sum + node->val; // node 子树的节点值之和
            int size = left_size + right_size + 1; // node 子树的节点个数
            if (node->val == sum / size) { // 题目要求下取整
                ans++;
            }
            return {sum, size};
        };

        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
typedef struct {
    int sum;
    int size;
} Pair;

Pair dfs(struct TreeNode* node, int* ans) {
    if (node == NULL) {
        return (Pair) {0, 0};
    }
    Pair left = dfs(node->left, ans); // 拆解问题：递归计算左子树的信息
    Pair right = dfs(node->right, ans); // 拆解问题：递归计算右子树的信息
    int sum = left.sum + right.sum + node->val; // node 子树的节点值之和
    int size = left.size + right.size + 1; // node 子树的节点个数
    if (node->val == sum / size) { // 题目要求下取整
        (*ans)++;
    }
    return (Pair) {sum, size};
}

int averageOfSubtree(struct TreeNode* root) {
    int ans = 0;
    dfs(root, &ans);
    return ans;
}
```

```go [sol-Go]
func averageOfSubtree(root *TreeNode) (ans int) {
	// 返回 node 子树的节点值之和、节点个数
	var dfs func(*TreeNode) (int, int)
	dfs = func(node *TreeNode) (int, int) {
		if node == nil {
			return 0, 0
		}
		leftSum, leftSize := dfs(node.Left)    // 拆解问题：递归计算左子树的信息
		rightSum, rightSize := dfs(node.Right) // 拆解问题：递归计算右子树的信息
		sum := leftSum + rightSum + node.Val   // node 子树的节点值之和
		size := leftSize + rightSize + 1       // node 子树的节点个数
		if node.Val == sum/size {              // 题目要求下取整
			ans++
		}
		return sum, size
	}

	dfs(root)
	return
}
```

```js [sol-JavaScript]
var averageOfSubtree = function(root) {
    let ans = 0;

    // 返回 node 子树的节点值之和、节点个数
    function dfs(node) {
        if (node === null) {
            return [0, 0];
        }
        const [left_sum, left_size] = dfs(node.left); // 拆解问题：递归计算左子树的信息
        const [right_sum, right_size] = dfs(node.right); // 拆解问题：递归计算右子树的信息
        const sum = left_sum + right_sum + node.val; // node 子树的节点值之和
        const size = left_size + right_size + 1; // node 子树的节点个数
        if (node.val === Math.floor(sum / size)) { // 题目要求下取整
            ans++;
        }
        return [sum, size];
    };

    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn average_of_subtree(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, ans: &mut i32) -> (i32, i32) {
            let Some(node) = node else {
                return (0, 0);
            };
            let node = node.borrow();
            let (left_sum, left_size) = dfs(&node.left, ans); // 拆解问题：递归计算左子树的信息
            let (right_sum, right_size) = dfs(&node.right, ans); // 拆解问题：递归计算右子树的信息
            let sum = left_sum + right_sum + node.val; // node 子树的节点值之和
            let size = left_size + right_size + 1; // node 子树的节点个数
            if node.val == sum / size { // 题目要求下取整
                *ans += 1;
            }
            (sum, size)
        }

        let mut ans = 0;
        dfs(&root, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$。其中 $h$ 是二叉树的高度。

## 专题训练

见下面二叉树题单的「**§2.3 自底向上 DFS**」。

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
