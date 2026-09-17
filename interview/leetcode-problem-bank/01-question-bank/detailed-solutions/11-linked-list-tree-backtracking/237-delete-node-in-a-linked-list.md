# 237. 删除链表中的节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/delete-node-in-a-linked-list/
- 题目 slug：`delete-node-in-a-linked-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.2 删除节点
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/delete-node-in-a-linked-list/solutions/2004056/ru-he-shan-chu-jie-dian-liu-fen-zhong-ga-x3kn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯+简洁写法（Python/Java/C++/C/Go/JS）](https://leetcode.cn/problems/delete-node-in-a-linked-list/solutions/2004056/ru-he-shan-chu-jie-dian-liu-fen-zhong-ga-x3kn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-shan-chu-jie-dian-liu-fen-zhong-ga-x3kn`
- topic id：`2004056`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

**视频讲解**：请看[【基础算法精讲 08】](https://www.bilibili.com/video/BV1VP4y1Q71e/)，制作不易，欢迎点赞~

要想直接删除节点 $\textit{node}$，必须修改 $\textit{node}$ 的前一个节点的 $\textit{next}$ 为 $\textit{node}.\textit{next}$。

比如示例 1，把 $4$ 的 $\textit{next}$ 改成 $1$，就把 $5$ 删掉了。

![lc237.jpg](https://pic.leetcode.cn/1754026046-bsBSjL-lc237.jpg){:width=350px}

然而，本题只告诉我们当前节点 $\textit{node}$，我们不知道 $\textit{node}$ 的前一个节点是谁。难道就没法删除了吗？

注意题目的要求：我们只需让链表「看上去」删除了 $\textit{node}$。比如按顺序打印这个链表的值，就是 $4,1,9$ 这三个节点值，就可以认为我们成功删除了 $5$。

比如示例 1，我们可以先把第二个节点的值**替换**为第三个节点的值，得到 $4\to 1\to 1\to 9$；然后删除第三个节点，得到 $4\to 1\to 9$。

具体来说，先把 $\textit{node}.\textit{val}$ 更新成 $\textit{node}.\textit{next}.\textit{val}$，然后删除 $\textit{node}$ 的下一个节点，也就是把 $\textit{node}.\textit{next}$ 更新成 $\textit{node}.\textit{next}.\textit{next}$。

**注**：如果语言支持指针操作，可以通过**解引用**直接把 $\textit{node}$ 更新成下一个节点。

```py [sol-Python3]
class Solution:
    def deleteNode(self, node: ListNode) -> None:
        node.val = node.next.val
        node.next = node.next.next
```

```java [sol-Java]
class Solution {
    public void deleteNode(ListNode node) {
        node.val = node.next.val;
        node.next = node.next.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    void deleteNode(ListNode* node) {
        *node = *node->next;
    }
};
```

```cpp [sol-C++ 回收内存]
class Solution {
public:
    void deleteNode(ListNode* node) {
        auto nxt = node->next;
        *node = *nxt;
        delete nxt;
    }
};
```

```c [sol-C]
void deleteNode(struct ListNode* node) {
    *node = *node->next;
}
```

```c [sol-C 回收内存]
void deleteNode(struct ListNode* node) {
    struct ListNode* nxt = node->next;
    *node = *nxt;
    free(nxt);
}
```

```go [sol-Go]
func deleteNode(node *ListNode) {
    *node = *node.Next
}
```

```js [sol-JS]
var deleteNode = function(node) {
    node.val = node.next.val;
    node.next = node.next.next;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `一、链表 / §1.2 删除节点`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.2 删除节点`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
