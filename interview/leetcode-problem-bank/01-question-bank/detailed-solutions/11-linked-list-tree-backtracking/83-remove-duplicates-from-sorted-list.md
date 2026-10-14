# 83. 删除排序链表中的重复元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-duplicates-from-sorted-list/
- 题目 slug：`remove-duplicates-from-sorted-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.2 删除节点
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-duplicates-from-sorted-list/solutions/2004062/ru-he-qu-zhong-yi-ge-shi-pin-jiang-tou-p-98g7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】如何去重？一个视频讲透！附题单！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-duplicates-from-sorted-list/solutions/2004062/ru-he-qu-zhong-yi-ge-shi-pin-jiang-tou-p-98g7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-qu-zhong-yi-ge-shi-pin-jiang-tou-p-98g7`
- topic id：`2004062`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

## 视频讲解

请看[【基础算法精讲 08】](https://www.bilibili.com/video/BV1VP4y1Q71e/)，制作不易，欢迎点赞~

```py [sol-Python3]
class Solution:
    def deleteDuplicates(self, head: Optional[ListNode]) -> Optional[ListNode]:
        if head is None:
            return None
        cur = head
        while cur.next:  # 看看下个节点……
            if cur.next.val == cur.val:  # 和我一样，删！
                cur.next = cur.next.next
            else:  # 和我不一样，移动到下个节点
                cur = cur.next
        return head
```

```java [sol-Java]
class Solution {
    public ListNode deleteDuplicates(ListNode head) {
        if (head == null) {
            return null;
        }
        ListNode cur = head;
        while (cur.next != null) { // 看看下个节点……
            if (cur.next.val == cur.val) { // 和我一样，删！
                cur.next = cur.next.next;
            } else { // 和我不一样，移动到下个节点
                cur = cur.next;
            }
        }
        return head;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* deleteDuplicates(ListNode* head) {
        if (head == nullptr) {
            return nullptr;
        }
        ListNode* cur = head;
        while (cur->next) { // 看看下个节点……
            if (cur->next->val == cur->val) { // 和我一样，删！
                cur->next = cur->next->next;
            } else { // 和我不一样，移动到下个节点
                cur = cur->next;
            }
        }
        return head;
    }
};
```

```cpp [sol-C++ 回收内存]
class Solution {
public:
    ListNode* deleteDuplicates(ListNode* head) {
        if (head == nullptr) {
            return nullptr;
        }
        ListNode* cur = head;
        while (cur->next) { // 看看下个节点……
            if (cur->next->val == cur->val) { // 和我一样，删！
                ListNode* nxt = cur->next;
                cur->next = nxt->next;
                delete nxt;
            } else { // 和我不一样，移动到下个节点
                cur = cur->next;
            }
        }
        return head;
    }
};
```

```c [sol-C]
struct ListNode* deleteDuplicates(struct ListNode* head) {
    if (head == NULL) {
        return NULL;
    }
    struct ListNode* cur = head;
    while (cur->next) { // 看看下个节点……
        if (cur->next->val == cur->val) { // 和我一样，删！
            cur->next = cur->next->next;
        } else { // 和我不一样，移动到下个节点
            cur = cur->next;
        }
    }
    return head;
}
```

```go [sol-Go]
func deleteDuplicates(head *ListNode) *ListNode {
    if head == nil {
        return nil
    }
    cur := head
    for cur.Next != nil { // 看看下个节点……
        if cur.Next.Val == cur.Val { // 和我一样，删！
            cur.Next = cur.Next.Next
        } else { // 和我不一样，移动到下个节点
            cur = cur.Next
        }
    }
    return head
}
```

```js [sol-JavaScript]
var deleteDuplicates = function(head) {
    if (head === null) {
        return null;
    }
    let cur = head;
    while (cur.next) { // 看看下个节点……
        if (cur.next.val === cur.val) { // 和我一样，删！
            cur.next = cur.next.next;
        } else { // 和我不一样，移动到下个节点
            cur = cur.next;
        }
    }
    return head;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn delete_duplicates(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        if head.is_none() {
            return None;
        }
        let mut cur = head.as_mut()?;
        while let Some(nxt) = cur.next.take() { // 取出来
            if (nxt.val == cur.val) {
                cur.next = nxt.next; // 删除 nxt
            } else {
                cur.next = Some(nxt); // 放回去
                cur = cur.next.as_mut()?;
            }
        }
        head
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为链表的长度。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

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
