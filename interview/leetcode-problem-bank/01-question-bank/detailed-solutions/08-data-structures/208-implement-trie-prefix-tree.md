# 208. 实现 Trie (前缀树)

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/implement-trie-prefix-tree/
- 题目 slug：`implement-trie-prefix-tree`
- 来源专题：常用数据结构
- 来源分类路径：六、字典树（trie） / §6.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/implement-trie-prefix-tree/solutions/2993894/cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从二叉树到二十六叉树（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/implement-trie-prefix-tree/solutions/2993894/cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4`
- topic id：`2993894`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 思路

> “若我俩都走过同样的路，则我俩心意相通。”

假设字符串里面只有 $\texttt{a}$ 和 $\texttt{b}$ 两种字符。

从左到右遍历字符串，把 $\texttt{a}$ 视作往左走，把 $\texttt{b}$ 视作往右走。怎么记录路径？怎么区分不同的路径？用二叉树记录。

- $\texttt{insert}$：例如先插入字符串 $\texttt{aab}$，相当于生成了一条移动方向为「左-左-右」的路径。标记最后一个节点为终止节点。再插入字符串 $\texttt{aabb}$，相当于生成了一条移动方向为「左-左-右-右」的路径。标记最后一个节点为终止节点。
- $\texttt{search}$：例如查找字符串 $\texttt{aab}$，相当于查找二叉树中是否存在一条移动方向为「左-左-右」的路径，且最后一个节点是终止节点。
- $\texttt{startsWith}$：例如查找前缀 $\texttt{aa}$，相当于查找二叉树中是否存在一条移动方向为「左-左」的路径，无其他要求。

![lc208.png](https://pic.leetcode.cn/1732008195-ouktYd-lc208.png){:width=500}

推广到 $26$ 种字母，其实就是一棵 $26$ 叉树，对于 $26$ 叉树的每个节点，可以用哈希表，或者长为 $26$ 的数组来存储子节点。

## 算法

- 初始化：创建一棵 $26$ 叉树，一开始只有一个根节点 $\textit{root}$。$26$ 叉树的每个节点包含一个长为 $26$ 的儿子节点列表 $\textit{son}$，以及一个布尔值 $\textit{end}$，表示是否为终止节点。
- $\texttt{insert}$：
    1. 遍历字符串 $\textit{word}$，同时用一个变量 $\textit{cur}$ 表示当前在 $26$ 叉树的哪个节点，初始值为 $\textit{root}$。
    2. 如果 $\textit{word}[i]$ 不是 $\textit{cur}$ 的儿子，那么创建一个新的节点 $\textit{node}$ 作为 $\textit{cur}$ 的儿子。如果 $\textit{word}[i]=\texttt{a}$，那么把 $\textit{node}$ 记录到 $\textit{cur}$ 的 $\textit{son}[0]$ 中。如果 $\textit{word}[i]=\texttt{b}$，那么把 $\textit{node}$ 记录到 $\textit{cur}$ 的 $\textit{son}[1]$ 中。依此类推。
    3. 更新 $\textit{cur}$ 为儿子列表中的相应节点。
    4. 遍历结束，把 $\textit{cur}$ 的 $\textit{end}$ 标记为 $\texttt{true}$。
- $\texttt{search}$ 和 $\texttt{startsWith}$ 可以复用同一个函数 $\textit{find}$：
    1. 遍历字符串 $\textit{word}$，同时用一个变量 $\textit{cur}$ 表示当前在 $26$ 叉树的哪个节点，初始值为 $\textit{root}$。
    2. 如果 $\textit{word}[i]$ 不是 $\textit{cur}$ 的儿子，返回 $0$。$\texttt{search}$ 和 $\texttt{startsWith}$ 收到 $0$ 之后返回 $\texttt{false}$。
    3. 更新 $\textit{cur}$ 为儿子列表中的相应节点。
    4. 遍历结束，如果 $\textit{cur}$ 的 $\textit{end}$ 是 $\texttt{false}$，返回 $1$，否则返回 $2$。$\texttt{search}$ 如果收到的是 $2$，返回 $\texttt{true}$，否则返回 $\texttt{false}$。$\texttt{startsWith}$ 如果收到的是非 $0$ 数字，返回 $\texttt{true}$，否则返回 $\texttt{false}$。

```py [sol-Python3 dict]
class Node:
    __slots__ = 'son', 'end'

    def __init__(self):
        self.son = {}
        self.end = False

class Trie:
    def __init__(self):
        self.root = Node()

    def insert(self, word: str) -> None:
        cur = self.root
        for c in word:
            if c not in cur.son:  # 无路可走？
                cur.son[c] = Node()  # 那就造路！
            cur = cur.son[c]
        cur.end = True

    def find(self, word: str) -> int:
        cur = self.root
        for c in word:
            if c not in cur.son:  # 道不同，不相为谋
                return 0
            cur = cur.son[c]
        # 走过同样的路（2=完全匹配，1=前缀匹配）
        return 2 if cur.end else 1

    def search(self, word: str) -> bool:
        return self.find(word) == 2

    def startsWith(self, prefix: str) -> bool:
        return self.find(prefix) != 0
```

```py [sol-Python3 list]
class Node:
    __slots__ = 'son', 'end'

    def __init__(self):
        self.son = [None] * 26
        self.end = False

class Trie:
    def __init__(self):
        self.root = Node()

    def insert(self, word: str) -> None:
        cur = self.root
        for c in word:
            c = ord(c) - ord('a')
            if cur.son[c] is None:  # 无路可走？
                cur.son[c] = Node()  # 那就造路！
            cur = cur.son[c]
        cur.end = True

    def find(self, word: str) -> int:
        cur = self.root
        for c in word:
            c = ord(c) - ord('a')
            if cur.son[c] is None:  # 道不同，不相为谋
                return 0
            cur = cur.son[c]
        # 走过同样的路（2=完全匹配，1=前缀匹配）
        return 2 if cur.end else 1

    def search(self, word: str) -> bool:
        return self.find(word) == 2

    def startsWith(self, prefix: str) -> bool:
        return self.find(prefix) != 0
```

```java [sol-Java]
class Trie {
    private static class Node {
        Node[] son = new Node[26];
        boolean end = false;
    }

    private final Node root = new Node();

    public void insert(String word) {
        Node cur = root;
        for (char c : word.toCharArray()) {
            c -= 'a';
            if (cur.son[c] == null) { // 无路可走？
                cur.son[c] = new Node(); // new 出来！
            }
            cur = cur.son[c];
        }
        cur.end = true;
    }

    public boolean search(String word) {
        return find(word) == 2;
    }

    public boolean startsWith(String prefix) {
        return find(prefix) != 0;
    }

    private int find(String word) {
        Node cur = root;
        for (char c : word.toCharArray()) {
            c -= 'a';
            if (cur.son[c] == null) { // 道不同，不相为谋
                return 0;
            }
            cur = cur.son[c];
        }
        // 走过同样的路（2=完全匹配，1=前缀匹配）
        return cur.end ? 2 : 1;
    }
}
```

```cpp [sol-C++]
struct Node {
    Node* son[26]{};
    bool end = false;
};

class Trie {
    Node* root = new Node();

    int find(string word) {
        Node* cur = root;
        for (char c : word) {
            c -= 'a';
            if (cur->son[c] == nullptr) { // 道不同，不相为谋
                return 0;
            }
            cur = cur->son[c];
        }
        // 走过同样的路（2=完全匹配，1=前缀匹配）
        return cur->end ? 2 : 1;
    }

    void destroy(Node* node) {
        if (node == nullptr) {
            return;
        }
        for (Node* son : node->son) {
            destroy(son);
        }
        delete node;
    }

public:
    ~Trie() {
        destroy(root);
    }

    void insert(string word) {
        Node* cur = root;
        for (char c : word) {
            c -= 'a';
            if (cur->son[c] == nullptr) { // 无路可走？
                cur->son[c] = new Node(); // new 出来！
            }
            cur = cur->son[c];
        }
        cur->end = true;
    }

    bool search(string word) {
        return find(word) == 2;
    }

    bool startsWith(string prefix) {
        return find(prefix) != 0;
    }
};
```

```c [sol-C]
#define ALPHABET_SIZE 26

typedef struct Node {
    struct Node* son[ALPHABET_SIZE];
    bool end;
} Node;

typedef struct {
    Node* root;
} Trie;

Trie* trieCreate() {
    Trie* obj = malloc(sizeof(Trie));
    obj->root = calloc(1, sizeof(Node));
    return obj;
}

void trieInsert(Trie* obj, char* word) {
    Node* cur = obj->root;
    for (int i = 0; word[i]; i++) {
        int c = word[i] - 'a';
        if (cur->son[c] == NULL) { // 无路可走？
            cur->son[c] = calloc(1, sizeof(Node)); // 那就造路！
        }
        cur = cur->son[c];
    }
    cur->end = true;
}

int trieFind(Trie* obj, char* word) {
    Node* cur = obj->root;
    for (int i = 0; word[i]; i++) {
        int c = word[i] - 'a';
        if (cur->son[c] == NULL) { // 道不同，不相为谋
            return 0;
        }
        cur = cur->son[c];
    }
    // 走过同样的路（2=完全匹配，1=前缀匹配）
    return cur->end ? 2 : 1;
}

bool trieSearch(Trie* obj, char* word) {
    return trieFind(obj, word) == 2;
}

bool trieStartsWith(Trie* obj, char* prefix) {
    return trieFind(obj, prefix) != 0;
}

void trieFreeNode(Node* node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        trieFreeNode(node->son[i]);
    }
    free(node);
}

void trieFree(Trie* obj) {
    trieFreeNode(obj->root);
    free(obj);
}
```

```go [sol-Go]
type Node struct {
    son [26]*Node
    end bool
}

type Trie struct {
    root *Node
}

func Constructor() Trie {
    return Trie{&Node{}}
}

func (t Trie) Insert(word string) {
    cur := t.root
    for _, c := range word {
        c -= 'a'
        if cur.son[c] == nil { // 无路可走？
            cur.son[c] = &Node{} // 那就造路！
        }
        cur = cur.son[c]
    }
    cur.end = true
}

func (t Trie) find(word string) int {
    cur := t.root
    for _, c := range word {
        c -= 'a'
        if cur.son[c] == nil { // 道不同，不相为谋
            return 0
        }
        cur = cur.son[c]
    }
    // 走过同样的路（2=完全匹配，1=前缀匹配）
    if cur.end {
        return 2
    }
    return 1
}

func (t Trie) Search(word string) bool {
    return t.find(word) == 2
}

func (t Trie) StartsWith(prefix string) bool {
    return t.find(prefix) != 0
}
```

```js [sol-JS]
class Node {
    constructor() {
        this.son = Array(26).fill(null);
        this.end = false;
    }
}

class Trie {
    constructor() {
        this.root = new Node();
    }

    insert(word) {
        let cur = this.root;
        for (let c of word) {
            c = c.charCodeAt(0) - 'a'.charCodeAt(0);
            if (cur.son[c] === null) { // 无路可走？
                cur.son[c] = new Node(); // new 出来！
            }
            cur = cur.son[c];
        }
        cur.end = true;
    }

    #find(word) {
        let cur = this.root;
        for (let c of word) {
            c = c.charCodeAt(0) - 'a'.charCodeAt(0);
            if (cur.son[c] === null) { // 道不同，不相为谋
                return 0;
            }
            cur = cur.son[c];
        }
        // 走过同样的路（2=完全匹配，1=前缀匹配）
        return cur.end ? 2 : 1;
    }

    search(word) {
        return this.#find(word) === 2;
    }

    startsWith(prefix) {
        return this.#find(prefix) !== 0;
    }
}
```

```rust [sol-Rust]
struct Node {
    son: [Option<Box<Node>>; 26],
    end: bool,
}

impl Node {
    fn new() -> Self {
        Self {
            son: [const { None }; 26],
            end: false,
        }
    }
}

struct Trie {
    root: Node,
}

impl Trie {
    fn new() -> Self {
        Self {
            root: Node::new()
        }
    }

    fn insert(&mut self, word: String) {
        let mut cur = &mut self.root;
        for c in word.bytes() {
            let c = (c - b'a') as usize;
            if cur.son[c].is_none() { // 无路可走？
                cur.son[c] = Some(Box::new(Node::new())); // new 出来！
            }
            cur = cur.son[c].as_mut().unwrap();
        }
        cur.end = true;
    }

    fn find(&self, word: String) -> i32 {
        let mut cur = &self.root;
        for c in word.bytes() {
            let c = (c - b'a') as usize;
            if cur.son[c].is_none() { // 道不同，不相为谋
                return 0;
            }
            cur = cur.son[c].as_ref().unwrap();
        }
        // 走过同样的路（2=完全匹配，1=前缀匹配）
        if cur.end { 2 } else { 1 }
    }

    fn search(&self, word: String) -> bool {
        self.find(word) == 2
    }

    fn starts_with(&self, prefix: String) -> bool {
        self.find(prefix) != 0
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化为 $\mathcal{O}(1)$，$\texttt{insert}$ 为 $\mathcal{O}(n|\Sigma|)$，其余为 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{word}$ 的长度，$|\Sigma|=26$ 是字符集合的大小。注意创建一个节点需要 $\mathcal{O}(|\Sigma|)$ 的时间（如果用的是数组）。
- 空间复杂度：$\mathcal{O}(qn|\Sigma|)$。其中 $q$ 是 $\texttt{insert}$ 的调用次数。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `六、字典树（trie） / §6.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、字典树（trie） / §6.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
