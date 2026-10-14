# K04 树、BST、递归遍历与序列化

## 核心问题

递归定义、层序/DFS、路径、最近公共祖先、序列化。学习目标是能识别题目中的结构、选择合适算法、证明复杂度并写出边界正确的 C++17 实现。

树是递归定义的数据结构：一棵树由根和若干子树构成。本域的通用方法是**把问题定义成"以某节点为根的子树上的子问题"**，先写清递归语义（函数返回什么、由左右子结果如何合并），再处理空节点这一递归基。BST 额外提供"中序有序"这一强性质，序列化则考察树的确定性编码。

## 训练顺序

1. 先完成对应入门题，建立术语和模板。
2. 再完成中等级变体，训练多约束组合。
3. 高级和专家题用于训练证明、在线约束、工程边界和追问。

## 核心模式

### 模式 1：深度优先遍历（前/中/后序）

三种序的区别只在"访问根"相对左右子树的时机。递归写法统一而清晰：

```cpp
#include <bits/stdc++.h>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int v = 0, TreeNode* l = nullptr, TreeNode* r = nullptr)
        : val(v), left(l), right(r) {}
};

void preorder(TreeNode* root, vector<int>& out) {   // 根 左 右
    if (!root) return;
    out.push_back(root->val);
    preorder(root->left, out);
    preorder(root->right, out);
}
void inorder(TreeNode* root, vector<int>& out) {    // 左 根 右
    if (!root) return;
    inorder(root->left, out);
    out.push_back(root->val);
    inorder(root->right, out);
}
void postorder(TreeNode* root, vector<int>& out) {  // 左 右 根
    if (!root) return;
    postorder(root->left, out);
    postorder(root->right, out);
    out.push_back(root->val);
}
```

**不变量**：进入某次调用时 `root` 指向待遍历子树的根；空节点直接返回是递归基。时间 O(n)，空间 O(h)（h 为树高，退化成链时 O(n)）。

### 模式 2：层序遍历（BFS）

用队列逐层扩展。区分层的关键是进入循环时先记录当前队列大小 `sz`，恰好处理 `sz` 个节点即为层边界。

```cpp
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> res;
    if (!root) return res;
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int sz = q.size();                 // 本层节点数，锁定层边界
        vector<int> level;
        for (int i = 0; i < sz; ++i) {
            TreeNode* n = q.front(); q.pop();
            level.push_back(n->val);
            if (n->left) q.push(n->left);
            if (n->right) q.push(n->right);
        }
        res.push_back(level);
    }
    return res;
}
```

时间 O(n)，空间 O(宽度)（最宽层节点数）。

### 模式 3：BST 性质

BST 满足：任一节点值 > 左子树所有值、< 右子树所有值。由此推出两个高频结论：

- **中序遍历 = 升序序列**（验证 BST、求第 k 小、转有序数组都靠它）。
- **验证 BST 必须携带上下界**，不能只比较父子（局部比较会漏判跨层违规）。

```cpp
bool isValidBST(TreeNode* root, long long lo = LLONG_MIN, long long hi = LLONG_MAX) {
    if (!root) return true;
    if (root->val <= lo || root->val >= hi) return false;
    return isValidBST(root->left, lo, root->val) &&
           isValidBST(root->right, root->val, hi);
}

int kthSmallest(TreeNode* root, int k) {   // 中序第 k 个
    stack<TreeNode*> st;
    TreeNode* cur = root;
    while (cur || !st.empty()) {
        while (cur) { st.push(cur); cur = cur->left; }
        cur = st.top(); st.pop();
        if (--k == 0) return cur->val;
        cur = cur->right;
    }
    return -1;
}
```

**验证不变量**：`isValidBST(node, lo, hi)` 断言 `node` 的值落在开区间 `(lo, hi)`，并把左子树的界收紧为 `(lo, node->val)`、右子树为 `(node->val, hi)`。用 `long long` 是为了容纳 `INT_MIN/INT_MAX` 边界。

### 模式 4：递归转迭代

递归的隐式调用栈可用显式栈替代，避免深树爆栈。中序迭代的核心不变量：`cur` 一路向左压栈，直到为空后弹出访问，再转向右子树。

```cpp
vector<int> inorderIterative(TreeNode* root) {
    vector<int> out;
    stack<TreeNode*> st;
    TreeNode* cur = root;
    while (cur || !st.empty()) {
        while (cur) { st.push(cur); cur = cur->left; }  // 走到最左
        cur = st.top(); st.pop();
        out.push_back(cur->val);                        // 访问
        cur = cur->right;                               // 转右子树
    }
    return out;
}
```

前序迭代只需把访问提前；后序可借"根右左"再反转，或用 `lastVisited` 标记。Morris 遍历进一步把空间降到 O(1)，代价是临时修改树指针（用线索还原）。

### 模式 5：序列化与反序列化

序列化要求**可唯一还原**。前序 + 空节点占位符（如 `#`）即可唯一确定一棵树，因为空标记划定了子树边界。层序同样可行（空节点也入列）。

```cpp
void serialize(TreeNode* root, string& out) {
    if (!root) { out += "#,"; return; }
    out += to_string(root->val) + ",";
    serialize(root->left, out);
    serialize(root->right, out);
}
TreeNode* deserialize(istringstream& in) {
    string tok;
    if (!getline(in, tok, ',')) return nullptr;
    if (tok == "#") return nullptr;
    TreeNode* node = new TreeNode(stoi(tok));
    node->left = deserialize(in);
    node->right = deserialize(in);
    return node;
}
```

**正确性**：前序中每个子树占据一段连续 token；`#` 标出空子树，递归消费 token 的顺序与原生成的顺序一致，故还原唯一。时间 O(n)，空间 O(n)（含分隔符）。

## 常见陷阱

- **验证 BST 只比父子**：`[10,5,15,null,null,6,20]` 中 6 在 15 左子树却小于 10，局部比较会误判合法，必须传上下界。
- **中序求第 k 小时 `k` 的语义**：`--k == 0` 与 `k-- == 0` 差一位，边界题高频出错。
- **层序忘记锁定 `sz`**：用 `while(!q.empty())` 直接 push 会把所有层混在一起。
- **递归基缺失或写错**：空节点必须显式返回，否则解引用空指针。
- **深树问爆栈**：退化成链时递归深度 O(n)，工程题应改用迭代或 Morris。
- **序列化未处理负数/多位数**：以逗号分隔即可；用单字符分隔会把负号和多位数拆错。
- **反序列化越界**：`getline` 失败后必须返回空，不能继续 `stoi`。
- **指针悬空**：迭代遍历中修改 `cur` 后仍用旧指针访问子树会出错。

## 复杂度速查

| 模式 | 时间 | 空间 | 关键前提 |
|---|---|---|---|
| DFS 前/中/后序（递归） | O(n) | O(h) | 递归定义 |
| 层序遍历（BFS） | O(n) | O(宽度) | 队列 + 层边界 |
| 验证 BST（界法） | O(n) | O(h) | 开区间上下界 |
| 中序第 k 小 | O(h + k) | O(h) | 中序有序 |
| 迭代中序 / Morris | O(n) | O(h) / O(1) | 显式栈 / 线索 |
| 序列化与反序列化 | O(n) | O(n) | 唯一编码 |

## 题目索引

本域在当前题库中**尚无独立题目文件**。可先用 `../00-overview/coverage-matrix.md` 的 `K04` 条目了解待补缺口，并从以下跨域题迁移思想：

- `../01-question-bank/advanced/A002-number-of-islands.md`（递归/显式栈遍历网格，是"树遍历"在一般图上的推广）
- `../01-question-bank/beginner/B002-climbing-stairs.md`（递归 → 递推 → 记忆化，训练"递归语义先行"的思维）

**证据与来源：** `[已确认]` 树的三种 DFS 序、层序、BST 中序性质与序列化唯一性均为公开经典结论；`[推断]` 训练顺序按难度归纳。样例未在目标 OJ 逐例运行，标记 `[待验证]`。
