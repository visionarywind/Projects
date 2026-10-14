# 2807. 在链表中插入最大公约数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/insert-greatest-common-divisors-in-linked-list/
- 题目 slug：`insert-greatest-common-divisors-in-linked-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.3 插入节点
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/insert-greatest-common-divisors-in-linked-list/solutions/2592797/jian-ji-xie-fa-pythonjavacgojsrust-by-en-i1rn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/insert-greatest-common-divisors-in-linked-list/solutions/2592797/jian-ji-xie-fa-pythonjavacgojsrust-by-en-i1rn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavacgojsrust-by-en-i1rn`
- topic id：`2592797`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

遍历链表，在当前节点 $\textit{cur}$ 后面插入 $\textit{gcd}$ 节点，同时 $\textit{gcd}$ 节点指向 $\textit{cur}$ 的下一个节点。

插入后，$\textit{cur}$ 更新为 $\textit{cur}.\textit{next}.\textit{next}$，也就是 $\textit{cur}$ 原来的下一个节点，开始下一轮循环。

循环直到 $\textit{cur}$ 没有下一个节点为止。

```py [sol-Python3]
class Solution:
    def insertGreatestCommonDivisors(self, head: Optional[ListNode]) -> Optional[ListNode]:
        cur = head
        while cur.next:
            cur.next = ListNode(gcd(cur.val, cur.next.val), cur.next)
            cur = cur.next.next
        return head
```

```java [sol-Java]
class Solution {
    public ListNode insertGreatestCommonDivisors(ListNode head) {
        for (ListNode cur = head; cur.next != null; cur = cur.next.next) {
            cur.next = new ListNode(gcd(cur.val, cur.next.val), cur.next);
        }
        return head;
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int temp = a;
            a = b % a;
            b = temp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode *insertGreatestCommonDivisors(ListNode *head) {
        for (auto cur = head; cur->next; cur = cur->next->next) {
            cur->next = new ListNode(gcd(cur->val, cur->next->val), cur->next);
        }
        return head;
    }
};
```

```go [sol-Go]
func insertGreatestCommonDivisors(head *ListNode) (ans *ListNode) {
    for cur := head; cur.Next != nil; cur = cur.Next.Next {
        cur.Next = &ListNode{gcd(cur.Val, cur.Next.Val), cur.Next}
    }
    return head
}

func gcd(a, b int) int {
    for a != 0 {
        a, b = b%a, a
    }
    return b
}
```

```js [sol-JavaScript]
var insertGreatestCommonDivisors = function(head) {
    for (let cur = head; cur.next; cur = cur.next.next) {
        cur.next = new ListNode(gcd(cur.val, cur.next.val), cur.next);
    }
    return head;
};

function gcd(a, b) {
    while (a !== 0) {
        [a, b] = [b % a, a];
    }
    return b;
}
```

```rust [sol-Rust]
impl Solution {
    pub fn insert_greatest_common_divisors(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut cur = &mut head;
        while cur.as_ref().unwrap().next.is_some() {
            let x = cur.as_mut().unwrap();
            let next = x.next.take();
            x.next = Some(Box::new(ListNode {
                val: Self::gcd(x.val, next.as_ref().unwrap().val),
                next,
            }));
            cur = &mut cur.as_mut().unwrap().next.as_mut().unwrap().next;
        }
        head
    }

    fn gcd(mut a: i32, mut b: i32) -> i32 {
        while a != 0 {
            (a, b) = (b % a, a);
        }
        b
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 为链表长度，$U$ 为节点值的最大值。每次计算 `gcd` 需要 $\mathcal{O}(\log U)$ 的时间。
- 空间复杂度：$\mathcal{O}(1)$。返回值的空间不计入。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
