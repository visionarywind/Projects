# 160. 相交链表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/intersection-of-two-linked-lists/
- 题目 slug：`intersection-of-two-linked-lists`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.7 双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/intersection-of-two-linked-lists/solutions/2958778/tu-jie-yi-zhang-tu-miao-dong-xiang-jiao-m6tg1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】一张图秒懂相交链表！（Python/Java/C++/C/Go/JS）](https://leetcode.cn/problems/intersection-of-two-linked-lists/solutions/2958778/tu-jie-yi-zhang-tu-miao-dong-xiang-jiao-m6tg1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-yi-zhang-tu-miao-dong-xiang-jiao-m6tg1`
- topic id：`2958778`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

![lc160-3-c.png](https://pic.leetcode.cn/1729473968-TLOxoH-lc160-3-c.png)

具体算法如下：

1. 初始化两个指针 $p=\textit{headA},\ q=\textit{headB}$。
2. 不断循环，直到 $p=q$。
3. 每次循环，$p$ 和 $q$ 各向后走一步。具体来说，如果 $p$ 不是空节点，那么更新 $p$ 为 $p.\textit{next}$，否则更新 $p$ 为 $\textit{headB}$；如果 $q$ 不是空节点，那么更新 $q$ 为 $q.\textit{next}$，否则更新 $q$ 为 $\textit{headA}$。
4. 循环结束时，如果两条链表相交，那么此时 $p$ 和 $q$ 都在相交的起始节点处，返回 $p$；如果两条链表不相交，那么 $p$ 和 $q$ 都走到空节点，所以也可以返回 $p$，即空节点。

```py [sol-Python3]
class Solution:
    def getIntersectionNode(self, headA: ListNode, headB: ListNode) -> Optional[ListNode]:
        p, q = headA, headB
        while p is not q:
            p = p.next if p else headB
            q = q.next if q else headA
        return p
```

```java [sol-Java]
class Solution {
    public ListNode getIntersectionNode(ListNode headA, ListNode headB) {
        ListNode p = headA;
        ListNode q = headB;
        while (p != q) {
            p = p != null ? p.next : headB;
            q = q != null ? q.next : headA;
        }
        return p;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
        ListNode* p = headA;
        ListNode* q = headB;
        while (p != q) {
            p = p ? p->next : headB;
            q = q ? q->next : headA;
        }
        return p;
    }
};
```

```c [sol-C]
struct ListNode* getIntersectionNode(struct ListNode* headA, struct ListNode* headB) {
    struct ListNode* p = headA;
    struct ListNode* q = headB;
    while (p != q) {
        p = p ? p->next : headB;
        q = q ? q->next : headA;
    }
    return p;
}
```

```go [sol-Go]
func getIntersectionNode(headA, headB *ListNode) *ListNode {
    p, q := headA, headB
    for p != q {
        if p != nil {
            p = p.Next
        } else {
            p = headB
        }
        if q != nil {
            q = q.Next
        } else {
            q = headA
        }
    }
    return p
}
```

```js [sol-JavaScript]
var getIntersectionNode = function(headA, headB) {
    let p = headA, q = headB;
    while (p !== q) {
        p = p ? p.next : headB;
        q = q ? q.next : headA;
    }
    return p;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n)$，其中 $m$ 是第一条链表的长度，$n$ 是第二条链表的长度。除了交点，每个节点会被指针 $p$ 访问至多一次，每个节点会被指针 $q$ 访问至多一次。
- 空间复杂度：$\mathcal{O}(1)$。

更多相似题目，见下面的链表题单。

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
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
