# 2296. 设计一个文本编辑器

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/design-a-text-editor/
- 题目 slug：`design-a-text-editor`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.6 对顶栈
- 难度分：1912
- 外部题解来源：https://leetcode.cn/problems/design-a-text-editor/solutions/1536149/lian-biao-mo-ni-pythonjavacgo-by-endless-egw4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[对顶栈 / Splay 树（Python/Java/C++/Go）](https://leetcode.cn/problems/design-a-text-editor/solutions/1536149/lian-biao-mo-ni-pythonjavacgo-by-endless-egw4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`lian-biao-mo-ni-pythonjavacgo-by-endless-egw4`
- topic id：`1536149`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：对顶栈

创建左右两个栈，头对头（栈顶对栈顶），光标的左右移动就相当于两个栈中的数据来回倒（左手倒右手，右手倒左手）。

对于插入和删除操作，直接在左边那个栈上入栈出栈。

```py [sol-Python3]
class TextEditor:
    def __init__(self):
        self.left = []  # 光标左侧字符
        self.right = []  # 光标右侧字符

    def addText(self, text: str) -> None:
        self.left.extend(text)  # 入栈

    def deleteText(self, k: int) -> int:
        pre = len(self.left)  # 删除之前的栈大小
        del self.left[-k:]  # 出栈
        return pre - len(self.left)  # 减去删除之后的栈大小

    def text(self) -> str:
        return ''.join(self.left[-10:])  # 光标左边至多 10 个字符

    def cursorLeft(self, k: int) -> str:
        while k and self.left:
            self.right.append(self.left.pop())  # 左手倒右手
            k -= 1
        return self.text()

    def cursorRight(self, k: int) -> str:
        while k and self.right:
            self.left.append(self.right.pop())  # 右手倒左手
            k -= 1
        return self.text()
```

```java [sol-Java]
class TextEditor {
    private final StringBuilder left = new StringBuilder(); // 光标左侧字符
    private final StringBuilder right = new StringBuilder(); // 光标右侧字符

    public void addText(String text) {
        left.append(text); // 入栈
    }

    public int deleteText(int k) {
        k = Math.min(k, left.length());
        left.setLength(left.length() - k); // 出栈
        return k;
    }

    public String cursorLeft(int k) {
        while (k > 0 && !left.isEmpty()) {
            right.append(left.charAt(left.length() - 1)); // 左手倒右手
            left.setLength(left.length() - 1);
            k--;
        }
        return text();
    }

    public String cursorRight(int k) {
        while (k > 0 && !right.isEmpty()) {
            left.append(right.charAt(right.length() - 1)); // 右手倒左手
            right.setLength(right.length() - 1);
            k--;
        }
        return text();
    }

    private String text() {
        // 光标左边至多 10 个字符
        return left.substring(Math.max(left.length() - 10, 0));
    }
}
```

```cpp [sol-C++]
class TextEditor {
    string left, right; // 光标左侧、右侧字符

    string text() {
        // 光标左边至多 10 个字符
        return left.substr(max((int) left.size() - 10, 0));
    }

public:
    void addText(string text) {
        left += text; // 入栈
    }

    int deleteText(int k) {
        k = min(k, (int) left.length());
        left.resize(left.length() - k); // 出栈
        return k;
    }

    string cursorLeft(int k) {
        while (k && !left.empty()) {
            right += left.back(); // 左手倒右手
            left.pop_back();
            k--;
        }
        return text();
    }

    string cursorRight(int k) {
        while (k && !right.empty()) {
            left += right.back(); // 右手倒左手
            right.pop_back();
            k--;
        }
        return text();
    }
};
```

```go [sol-Go]
type TextEditor struct {
    left, right []byte // 光标左侧、右侧字符
}

func Constructor() TextEditor {
    return TextEditor{}
}

func (t *TextEditor) AddText(text string) {
    t.left = append(t.left, text...) // 入栈
}

func (t *TextEditor) DeleteText(k int) int {
    k = min(k, len(t.left))
    t.left = t.left[:len(t.left)-k] // 出栈
    return k
}

func (t *TextEditor) text() string {
    // 光标左边至多 10 个字符
    return string(t.left[max(len(t.left)-10, 0):])
}

func (t *TextEditor) CursorLeft(k int) string {
    for k > 0 && len(t.left) > 0 {
        t.right = append(t.right, t.left[len(t.left)-1]) // 左手倒右手
        t.left = t.left[:len(t.left)-1]
        k--
    }
    return t.text()
}

func (t *TextEditor) CursorRight(k int) string {
    for k > 0 && len(t.right) > 0 {
        t.left = append(t.left, t.right[len(t.right)-1]) // 右手倒左手
        t.right = t.right[:len(t.right)-1]
        k--
    }
    return t.text()
}
```

#### 复杂度分析

- 时间复杂度：初始化 $\mathcal{O}(1)$，$\texttt{addText}$ 为 $\mathcal{O}(|\textit{text}|)$，其余 $\mathcal{O}(k)$。
- 空间复杂度：$\mathcal{O}(n)$。其中 $n$ 为所有 $\textit{text}$ 的长度之和。

## 方法二：Splay（选读）

如果 $k$ 很大，要怎么做？有没有复杂度和 $k$ 无关的算法？

可以用 [Splay](https://oi-wiki.org/ds/splay/) 模拟文本的添加和删除。感兴趣的同学可以查阅相关资料。

```go [sol-Go]
type node struct {
    ch  [2]*node
    sz  int
    key byte
}

// 设置如下返回值是为了方便使用 node 中的 ch 数组
func (o *node) cmpKth(k int) int {
    d := k - o.ch[0].size() - 1
    switch {
    case d < 0:
        return 0 // 左儿子
    case d > 0:
        return 1 // 右儿子
    default:
        return -1
    }
}

func (o *node) size() int {
    if o != nil {
        return o.sz
    }
    return 0
}

func (o *node) maintain() {
    o.sz = 1 + o.ch[0].size() + o.ch[1].size()
}

// 构建一棵中序遍历为 s 的 splay 树
// 比如，给你一个序列和一些修改操作，每次取出一段子区间，cut 掉然后 append 到末尾，输出完成所有操作后的最终序列：
//     我们可以 buildSplay(1,n)，每次操作调用两次 split 来 cut 区间，得到三棵子树 a b c
//     append 之后应该是 a c b，那么我们可以 a.merge(c.merge(b)) 来完成这一操作
//     注意 merge 后可能就不满足搜索树的性质了，但是没有关系，中序遍历的结果仍然是正确的，我们只要保证这一点成立，就能正确得到完成所有操作后的最终序列
func buildSplay(s string) *node {
    if s == "" {
        return nil
    }
    m := len(s) / 2
    o := &node{key: s[m]}
    o.ch[0] = buildSplay(s[:m])
    o.ch[1] = buildSplay(s[m+1:])
    o.maintain()
    return o
}

// 旋转，并维护子树大小
// d=0：左旋，返回 o 的右儿子
// d=1：右旋，返回 o 的左儿子
func (o *node) rotate(d int) *node {
    x := o.ch[d^1]
    o.ch[d^1] = x.ch[d]
    x.ch[d] = o
    o.maintain()
    x.maintain()
    return x
}

// 将子树 o（中序遍历）的第 k 个节点伸展到 o，并返回该节点
// 1 <= k <= o.size()
func (o *node) splay(k int) (kth *node) {
    d := o.cmpKth(k)
    if d < 0 {
        return o
    }
    k -= d * (o.ch[0].size() + 1)
    c := o.ch[d]
    if d2 := c.cmpKth(k); d2 >= 0 {
        c.ch[d2] = c.ch[d2].splay(k - d2*(c.ch[0].size()+1))
        if d2 == d {
            o = o.rotate(d ^ 1)
        } else {
            o.ch[d] = c.rotate(d)
        }
    }
    return o.rotate(d ^ 1)
}

// 分裂子树 o，把 o（中序遍历）的前 k 个节点放在 lo 子树，其余放在 ro 子树
// 返回的 lo 节点为 o（中序遍历）的第 k 个节点
// 1 <= k <= o.size()
// 特别地，k = o.size() 时 ro 为 nil
func (o *node) split(k int) (lo, ro *node) {
    lo = o.splay(k)
    ro = lo.ch[1]
    lo.ch[1] = nil
    lo.maintain()
    return
}

// 把子树 ro 合并进子树 o，返回合并前 o（中序遍历）的最后一个节点
// 相当于把 ro 的中序遍历 append 到 o 的中序遍历之后
// ro 可以为 nil，但 o 不能为 nil
func (o *node) merge(ro *node) *node {
    // 把最大节点伸展上来，这样会空出一个右儿子用来合并 ro
    o = o.splay(o.size())
    o.ch[1] = ro
    o.maintain()
    return o
}

type TextEditor struct {
    root *node
    cur  int
}

func Constructor() TextEditor { return TextEditor{} }

func (t *TextEditor) AddText(text string) {
    if t.cur == 0 {
        t.root = buildSplay(text).merge(t.root)
    } else {
        lo, ro := t.root.split(t.cur)
        t.root = lo.merge(buildSplay(text)).merge(ro)
    }
    t.cur += len(text)
}

func (t *TextEditor) DeleteText(k int) int {
    if t.cur == 0 {
        return 0
    }
    if t.cur <= k { // 左边全部删除
        _, t.root = t.root.split(t.cur)
        ans := t.cur
        t.cur = 0
        return ans
    } else {
        lo, ro := t.root.split(t.cur)
        t.cur -= k
        lo, _ = lo.split(t.cur) // 删除中间 k 个
        t.root = lo.merge(ro)
        return k
    }
}

func (t *TextEditor) text() string {
    if t.cur == 0 {
        return ""
    }
    k := max(t.cur-10, 0)
    t.root = t.root.splay(k + 1)
    ans := make([]byte, 1, t.cur-k)
    ans[0] = t.root.key
    var inorder func(*node) bool
    inorder = func(o *node) bool {
        if o == nil {
            return false
        }
        if inorder(o.ch[0]) || len(ans) == cap(ans) {
            return true
        }
        ans = append(ans, o.key)
        return inorder(o.ch[1])
    }
    inorder(t.root.ch[1])
    return string(ans)
}

func (t *TextEditor) CursorLeft(k int) string {
    t.cur = max(t.cur-k, 0)
    return t.text()
}

func (t *TextEditor) CursorRight(k int) string {
    t.cur = min(t.cur+k, t.root.size())
    return t.text()
}
```

#### 复杂度分析

- 时间复杂度：初始化 $\mathcal{O}(1)$，$\texttt{addText}$ 均摊 $\mathcal{O}(|\textit{text}| + \log n)$，其余均摊 $\mathcal{O}(\log n)$，其中 $n$ 是当前文本的长度之和。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目（对顶栈）

- [HDU 4699. Editor](http://acm.hdu.edu.cn/showproblem.php?pid=4699)

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

## 方法一：对顶栈

创建左右两个栈，头对头（栈顶对栈顶），光标的左右移动就相当于两个栈中的数据来回倒（左手倒右手，右手倒左手）。

对于插入和删除操作，直接在左边那个栈上入栈出栈。

```py [sol-Python3]
class TextEditor:
    def __init__(self):
        self.left = []  # 光标左侧字符
        self.right = []  # 光标右侧字符

    def addText(self, text: str) -> None:
        self.left.extend(text)  # 入栈

    def deleteText(self, k: int) -> int:
        pre = len(self.left)  # 删除之前的栈大小
        del self.left[-k:]  # 出栈
        return pre - len(self.left)  # 减去删除之后的栈大小

    def text(self) -> str:
        return ''.join(self.left[-10:])  # 光标左边至多 10 个字符

    def cursorLeft(self, k: int) -> str:
        while k and self.left:
            self.right.append(self.left.pop())  # 左手倒右手
            k -= 1
        return self.text()

    def cursorRight(self, k: int) -> str:
        while k and self.right:
            self.left.append(self.right.pop())  # 右手倒左手
            k -= 1
        return self.text()
```

```java [sol-Java]
class TextEditor {
    private final StringBuilder left = new StringBuilder(); // 光标左侧字符
    private final StringBuilder right = new StringBuilder(); // 光标右侧字符

    public void addText(String text) {
        left.append(text); // 入栈
    }

    public int deleteText(int k) {
        k = Math.min(k, left.length());
        left.setLength(left.length() - k); // 出栈
        return k;
    }

    public String cursorLeft(int k) {
        while (k > 0 && !left.isEmpty()) {
            right.append(left.charAt(left.length() - 1)); // 左手倒右手
            left.setLength(left.length() - 1);
            k--;
        }
        return text();
    }

    public String cursorRight(int k) {
        while (k > 0 && !right.isEmpty()) {
            left.append(right.charAt(right.length() - 1)); // 右手倒左手
            right.setLength(right.length() - 1);
            k--;
        }
        return text();
    }

    private String text() {
        // 光标左边至多 10 个字符
        return left.substring(Math.max(left.length() - 10, 0));
    }
}
```

```cpp [sol-C++]
class TextEditor {
    string left, right; // 光标左侧、右侧字符

    string text() {
        // 光标左边至多 10 个字符
        return left.substr(max((int) left.size() - 10, 0));
    }

public:
    void addText(string text) {
        left += text; // 入栈
    }

    int deleteText(int k) {
        k = min(k, (int) left.length());
        left.resize(left.length() - k); // 出栈
        return k;
    }

    string cursorLeft(int k) {
        while (k && !left.empty()) {
            right += left.back(); // 左手倒右手
            left.pop_back();
            k--;
        }
        return text();
    }

    string cursorRight(int k) {
        while (k && !right.empty()) {
            left += right.back(); // 右手倒左手
            right.pop_back();
            k--;
        }
        return text();
    }
};
```

```go [sol-Go]
type TextEditor struct {
    left, right []byte // 光标左侧、右侧字符
}

func Constructor() TextEditor {
    return TextEditor{}
}

func (t *TextEditor) AddText(text string) {
    t.left = append(t.left, text...) // 入栈
}

func (t *TextEditor) DeleteText(k int) int {
    k = min(k, len(t.left))
    t.left = t.left[:len(t.left)-k] // 出栈
    return k
}

func (t *TextEditor) text() string {
    // 光标左边至多 10 个字符
    return string(t.left[max(len(t.left)-10, 0):])
}

func (t *TextEditor) CursorLeft(k int) string {
    for k > 0 && len(t.left) > 0 {
        t.right = append(t.right, t.left[len(t.left)-1]) // 左手倒右手
        t.left = t.left[:len(t.left)-1]
        k--
    }
    return t.text()
}

func (t *TextEditor) CursorRight(k int) string {
    for k > 0 && len(t.right) > 0 {
        t.left = append(t.left, t.right[len(t.right)-1]) // 右手倒左手
        t.right = t.right[:len(t.right)-1]
        k--
    }
    return t.text()
}
```

#### 复杂度分析

- 时间复杂度：初始化 $\mathcal{O}(1)$，$\texttt{addText}$ 为 $\mathcal{O}(|\textit{text}|)$，其余 $\mathcal{O}(k)$。
- 空间复杂度：$\mathcal{O}(n)$。其中 $n$ 为所有 $\textit{text}$ 的长度之和。

## 方法二：Splay（选读）

如果 $k$ 很大，要怎么做？有没有复杂度和 $k$ 无关的算法？

可以用 [Splay](https://oi-wiki.org/ds/splay/) 模拟文本的添加和删除。感兴趣的同学可以查阅相关资料。

```go [sol-Go]
type node struct {
    ch  [2]*node
    sz  int
    key byte
}

// 设置如下返回值是为了方便使用 node 中的 ch 数组
func (o *node) cmpKth(k int) int {
    d := k - o.ch[0].size() - 1
    switch {
    case d < 0:
        return 0 // 左儿子
    case d > 0:
        return 1 // 右儿子
    default:
        return -1
    }
}

func (o *node) size() int {
    if o != nil {
        return o.sz
    }
    return 0
}

func (o *node) maintain() {
    o.sz = 1 + o.ch[0].size() + o.ch[1].size()
}

// 构建一棵中序遍历为 s 的 splay 树
// 比如，给你一个序列和一些修改操作，每次取出一段子区间，cut 掉然后 append 到末尾，输出完成所有操作后的最终序列：
//     我们可以 buildSplay(1,n)，每次操作调用两次 split 来 cut 区间，得到三棵子树 a b c
//     append 之后应该是 a c b，那么我们可以 a.merge(c.merge(b)) 来完成这一操作
//     注意 merge 后可能就不满足搜索树的性质了，但是没有关系，中序遍历的结果仍然是正确的，我们只要保证这一点成立，就能正确得到完成所有操作后的最终序列
func buildSplay(s string) *node {
    if s == "" {
        return nil
    }
    m := len(s) / 2
    o := &node{key: s[m]}
    o.ch[0] = buildSplay(s[:m])
    o.ch[1] = buildSplay(s[m+1:])
    o.maintain()
    return o
}

// 旋转，并维护子树大小
// d=0：左旋，返回 o 的右儿子
// d=1：右旋，返回 o 的左儿子
func (o *node) rotate(d int) *node {
    x := o.ch[d^1]
    o.ch[d^1] = x.ch[d]
    x.ch[d] = o
    o.maintain()
    x.maintain()
    return x
}

// 将子树 o（中序遍历）的第 k 个节点伸展到 o，并返回该节点
// 1 <= k <= o.size()
func (o *node) splay(k int) (kth *node) {
    d := o.cmpKth(k)
    if d < 0 {
        return o
    }
    k -= d * (o.ch[0].size() + 1)
    c := o.ch[d]
    if d2 := c.cmpKth(k); d2 >= 0 {
        c.ch[d2] = c.ch[d2].splay(k - d2*(c.ch[0].size()+1))
        if d2 == d {
            o = o.rotate(d ^ 1)
        } else {
            o.ch[d] = c.rotate(d)
        }
    }
    return o.rotate(d ^ 1)
}

// 分裂子树 o，把 o（中序遍历）的前 k 个节点放在 lo 子树，其余放在 ro 子树
// 返回的 lo 节点为 o（中序遍历）的第 k 个节点
// 1 <= k <= o.size()
// 特别地，k = o.size() 时 ro 为 nil
func (o *node) split(k int) (lo, ro *node) {
    lo = o.splay(k)
    ro = lo.ch[1]
    lo.ch[1] = nil
    lo.maintain()
    return
}

// 把子树 ro 合并进子树 o，返回合并前 o（中序遍历）的最后一个节点
// 相当于把 ro 的中序遍历 append 到 o 的中序遍历之后
// ro 可以为 nil，但 o 不能为 nil
func (o *node) merge(ro *node) *node {
    // 把最大节点伸展上来，这样会空出一个右儿子用来合并 ro
    o = o.splay(o.size())
    o.ch[1] = ro
    o.maintain()
    return o
}

type TextEditor struct {
    root *node
    cur  int
}

func Constructor() TextEditor { return TextEditor{} }

func (t *TextEditor) AddText(text string) {
    if t.cur == 0 {
        t.root = buildSplay(text).merge(t.root)
    } else {
        lo, ro := t.root.split(t.cur)
        t.root = lo.merge(buildSplay(text)).merge(ro)
    }
    t.cur += len(text)
}

func (t *TextEditor) DeleteText(k int) int {
    if t.cur == 0 {
        return 0
    }
    if t.cur <= k { // 左边全部删除
        _, t.root = t.root.split(t.cur)
        ans := t.cur
        t.cur = 0
        return ans
    } else {
        lo, ro := t.root.split(t.cur)
        t.cur -= k
        lo, _ = lo.split(t.cur) // 删除中间 k 个
        t.root = lo.merge(ro)
        return k
    }
}

func (t *TextEditor) text() string {
    if t.cur == 0 {
        return ""
    }
    k := max(t.cur-10, 0)
    t.root = t.root.splay(k + 1)
    ans := make([]byte, 1, t.cur-k)
    ans[0] = t.root.key
    var inorder func(*node) bool
    inorder = func(o *node) bool {
        if o == nil {
            return false
        }
        if inorder(o.ch[0]) || len(ans) == cap(ans) {
            return true
        }
        ans = append(ans, o.key)
        return inorder(o.ch[1])
    }
    inorder(t.root.ch[1])
    return string(ans)
}

func (t *TextEditor) CursorLeft(k int) string {
    t.cur = max(t.cur-k, 0)
    return t.text()
}

func (t *TextEditor) CursorRight(k int) string {
    t.cur = min(t.cur+k, t.root.size())
    return t.text()
}
```

#### 复杂度分析

- 时间复杂度：初始化 $\mathcal{O}(1)$，$\texttt{addText}$ 均摊 $\mathcal{O}(|\textit{text}| + \log n)$，其余均摊 $\mathcal{O}(\log n)$，其中 $n$ 是当前文本的长度之和。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目（对顶栈）

- [HDU 4699. Editor](http://acm.hdu.edu.cn/showproblem.php?pid=4699)

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

本题来自 `三、栈 / §3.6 对顶栈`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.6 对顶栈`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
