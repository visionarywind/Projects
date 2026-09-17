# 2. 两数相加

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/add-two-numbers/
- 题目 slug：`add-two-numbers`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.8 合并链表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/add-two-numbers/solutions/2327008/dong-hua-jian-ji-xie-fa-cong-di-gui-dao-oe0di/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【动画】简洁写法！从递归到迭代！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/add-two-numbers/solutions/2327008/dong-hua-jian-ji-xie-fa-cong-di-gui-dao-oe0di/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-hua-jian-ji-xie-fa-cong-di-gui-dao-oe0di`
- topic id：`2327008`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 方法一：递归

<![lc2-1.png](https://pic.leetcode.cn/1688220185-RQeZVa-lc2-1.png),![lc2-2.png](https://pic.leetcode.cn/1688220185-MxyXvT-lc2-2.png),![lc2-3.png](https://pic.leetcode.cn/1688220185-cbYoMw-lc2-3.png),![lc2-4.png](https://pic.leetcode.cn/1688220185-VoWMSh-lc2-4.png),![lc2-5.png](https://pic.leetcode.cn/1688220185-UwxODb-lc2-5.png),![lc2-6.png](https://pic.leetcode.cn/1688220185-tNovMh-lc2-6.png),![lc2-7.png](https://pic.leetcode.cn/1688220185-NbApIF-lc2-7.png),![lc2-8.png](https://pic.leetcode.cn/1688220335-FmbsUK-lc2-8.png)>

把虚线内要计算的内容，可以理解为一个和原问题相似的，规模更小的子问题，所以非常适合用**递归**解决。

每次把两个节点值 $l_1.\textit{val},\ l_2.\textit{val}$ 与进位值 $\textit{carry}$ 相加，除以 $10$ 的余数即为当前节点需要保存的数位，除以 $10$ 的商即为新的进位值。

一遇到递归就头晕？请看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

不了解链表的同学请看[【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)。

### 写法一：创建新节点

```py [sol-Python3]
class Solution:
    # l1 和 l2 为当前遍历的节点，carry 为进位
    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode], carry=0) -> Optional[ListNode]:
        if l1 is None and l2 is None and carry == 0:  # 递归边界
            return None

        s = carry
        if l1:
            s += l1.val  # 累加进位与节点值
            l1 = l1.next
        if l2:
            s += l2.val
            l2 = l2.next

        # s 除以 10 的余数为当前节点值，商为进位
        return ListNode(s % 10, self.addTwoNumbers(l1, l2, s // 10))
```

```java [sol-Java]
class Solution {
    public ListNode addTwoNumbers(ListNode l1, ListNode l2) {
        return addTwo(l1, l2, 0);
    }

    // l1 和 l2 为当前遍历的节点，carry 为进位
    private ListNode addTwo(ListNode l1, ListNode l2, int carry) {
        if (l1 == null && l2 == null && carry == 0) { // 递归边界
            return null;
        }

        int s = carry;
        if (l1 != null) {
            s += l1.val; // 累加进位与节点值
            l1 = l1.next;
        }
        if (l2 != null) {
            s += l2.val;
            l2 = l2.next;
        }

        // s 除以 10 的余数为当前节点值，商为进位
        return new ListNode(s % 10, addTwo(l1, l2, s / 10));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    // l1 和 l2 为当前遍历的节点，carry 为进位
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2, int carry = 0) {
        if (l1 == nullptr && l2 == nullptr && carry == 0) { // 递归边界
            return nullptr;
        }

        int s = carry;
        if (l1) {
            s += l1->val; // 累加进位与节点值
            l1 = l1->next;
        }
        if (l2) {
            s += l2->val;
            l2 = l2->next;
        }

        // s 除以 10 的余数为当前节点值，商为进位
        return new ListNode(s % 10, addTwoNumbers(l1, l2, s / 10));
    }
};
```

```c [sol-C]
// l1 和 l2 为当前遍历的节点，carry 为进位
struct ListNode* addTwo(struct ListNode* l1, struct ListNode* l2, int carry) {
    if (l1 == NULL && l2 == NULL && carry == 0) { // 递归边界
        return NULL;
    }

    int s = carry;
    if (l1) {
        s += l1->val; // 累加进位与节点值
        l1 = l1->next;
    }
    if (l2) {
        s += l2->val;
        l2 = l2->next;
    }

    struct ListNode* node = malloc(sizeof(struct ListNode));
    // s 除以 10 的余数为当前节点值，商为进位
    node->val = s % 10;
    node->next = addTwo(l1, l2, s / 10);
    return node;
}

struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    return addTwo(l1, l2, 0);
}
```

```go [sol-Go]
// l1 和 l2 为当前遍历的节点，carry 为进位
func addTwo(l1 *ListNode, l2 *ListNode, carry int) *ListNode {
    if l1 == nil && l2 == nil && carry == 0 { // 递归边界
        return nil
    }

    s := carry
    if l1 != nil {
        s += l1.Val // 累加进位与节点值
        l1 = l1.Next
    }
    if l2 != nil {
        s += l2.Val
        l2 = l2.Next
    }

    // s 除以 10 的余数为当前节点值，商为进位
    return &ListNode{s % 10, addTwo(l1, l2, s/10)}
}

func addTwoNumbers(l1 *ListNode, l2 *ListNode) *ListNode {
    return addTwo(l1, l2, 0)
}
```

```js [sol-JavaScript]
// l1 和 l2 为当前遍历的节点，carry 为进位
var addTwoNumbers = function(l1, l2, carry = 0) {
    if (l1 === null && l2 === null && carry === 0) { // 递归边界
        return null;
    }

    let s = carry;
    if (l1) {
        s += l1.val; // 累加进位与节点值
        l1 = l1.next;
    }
    if (l2) {
        s += l2.val;
        l2 = l2.next;
    }

    // s 除以 10 的余数为当前节点值，商为进位
    return new ListNode(s % 10, addTwoNumbers(l1, l2, Math.floor(s / 10)));
};
```

```rust [sol-Rust]
impl Solution {
    pub fn add_two_numbers(l1: Option<Box<ListNode>>, l2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        Self::add_two(l1, l2, 0)
    }

    // l1 和 l2 为当前遍历的节点，carry 为进位
    fn add_two(mut l1: Option<Box<ListNode>>, mut l2: Option<Box<ListNode>>, carry: i32) -> Option<Box<ListNode>> {
        if l1.is_none() && l2.is_none() && carry == 0 { // 递归边界
            return None;
        }

        let mut sum = carry;
        if let Some(node) = l1 {
            sum += node.val; // 累加进位与节点值
            l1 = node.next;
        }
        if let Some(node) = l2 {
            sum += node.val;
            l2 = node.next;
        }

        // sum 除以 10 的余数为当前节点值，商为进位
        Some(Box::new(ListNode {
            val: sum % 10,
            next: Self::add_two(l1, l2, sum / 10),
        }))
    }
}
```

### 写法二：原地修改

代码实现时，有一个简化代码的小技巧：如果递归中发现 $l_2$ 的长度比 $l_1$ 更长，那么可以交换 $l_1$ 和 $l_2$，保证 $l_1$ 不是空节点，从而简化代码逻辑。

```py [sol-Python3]
class Solution:
    # l1 和 l2 为当前遍历的节点，carry 为进位
    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode], carry=0) -> Optional[ListNode]:
        if l1 is None and l2 is None:  # 递归边界
            return ListNode(carry) if carry else None  # 如果进位了，就额外创建一个节点
        if l1 is None:  # 如果 l1 是空的，那么此时 l2 一定不是空节点
            l1, l2 = l2, l1  # 交换 l1 与 l2，保证 l1 非空，从而简化代码
        s = carry + l1.val + (l2.val if l2 else 0)  # 节点值和进位加在一起
        l1.val = s % 10  # 每个节点保存一个数位（直接修改原链表）
        l1.next = self.addTwoNumbers(l1.next, l2.next if l2 else None, s // 10)  # 进位
        return l1
```

```java [sol-Java]
class Solution {
    public ListNode addTwoNumbers(ListNode l1, ListNode l2) {
        return addTwo(l1, l2, 0);
    }

    // l1 和 l2 为当前遍历的节点，carry 为进位
    private ListNode addTwo(ListNode l1, ListNode l2, int carry) {
        if (l1 == null && l2 == null) { // 递归边界
            return carry != 0 ? new ListNode(carry) : null; // 如果进位了，就额外创建一个节点
        }
        if (l1 == null) { // 如果 l1 是空的，那么此时 l2 一定不是空节点
            l1 = l2;
            l2 = null; // 交换 l1 与 l2，保证 l1 非空，从而简化代码
        }
        int sum = carry + l1.val + (l2 != null ? l2.val : 0); // 节点值和进位加在一起
        l1.val = sum % 10; // 每个节点保存一个数位（直接修改原链表）
        l1.next = addTwo(l1.next, (l2 != null ? l2.next : null), sum / 10); // 进位
        return l1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    // l1 和 l2 为当前遍历的节点，carry 为进位
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2, int carry = 0) {
        if (l1 == nullptr && l2 == nullptr) { // 递归边界
            return carry ? new ListNode(carry) : nullptr; // 如果进位了，就额外创建一个节点
        }
        if (l1 == nullptr) { // 如果 l1 是空的，那么此时 l2 一定不是空节点
            swap(l1, l2); // 交换 l1 与 l2，保证 l1 非空，从而简化代码
        }
        int sum = carry + l1->val + (l2 ? l2->val : 0); // 节点值和进位加在一起
        l1->val = sum % 10; // 每个节点保存一个数位（直接修改原链表）
        l1->next = addTwoNumbers(l1->next, (l2 ? l2->next : nullptr), sum / 10); // 进位
        return l1;
    }
};
```

```c [sol-C]
// l1 和 l2 为当前遍历的节点，carry 为进位
struct ListNode* addTwo(struct ListNode* l1, struct ListNode* l2, int carry) {
    if (l1 == NULL && l2 == NULL) { // 递归边界
        if (carry) {
            // 如果进位了，就额外创建一个节点
            struct ListNode* node = malloc(sizeof(struct ListNode));
            node->val = carry;
            node->next = NULL;
            return node;
        }
        return NULL;
    }
    if (l1 == NULL) { // 如果 l1 是空的，那么此时 l2 一定不是空节点
        // 交换 l1 与 l2，保证 l1 非空，从而简化代码
        l1 = l2;
        l2 = NULL;
    }
    int sum = carry + l1->val + (l2 ? l2->val : 0); // 节点值和进位加在一起
    l1->val = sum % 10; // 每个节点保存一个数位（直接修改原链表）
    l1->next = addTwo(l1->next, (l2 ? l2->next : NULL), sum / 10); // 进位
    return l1;
}

struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    return addTwo(l1, l2, 0);
}
```

```go [sol-Go]
// l1 和 l2 为当前遍历的节点，carry 为进位
func addTwo(l1, l2 *ListNode, carry int) *ListNode {
    if l1 == nil && l2 == nil { // 递归边界
        if carry != 0 {
            return &ListNode{Val: carry} // 如果进位了，就额外创建一个节点
        }
        return nil
    }
    if l1 == nil { // 如果 l1 是空的，那么此时 l2 一定不是空节点
        l1, l2 = l2, l1 // 交换 l1 与 l2，保证 l1 非空，从而简化代码
    }
    sum := carry + l1.Val // 节点值和进位加在一起
    if l2 != nil {
        sum += l2.Val // 节点值和进位加在一起
        l2 = l2.Next // 下一个节点
    }
    l1.Val = sum % 10 // 每个节点保存一个数位（直接修改原链表）
    l1.Next = addTwo(l1.Next, l2, sum/10) // 进位
    return l1
}

func addTwoNumbers(l1, l2 *ListNode) *ListNode {
    return addTwo(l1, l2, 0)
}
```

```js [sol-JavaScript]
// l1 和 l2 为当前遍历的节点，carry 为进位
var addTwoNumbers = function(l1, l2, carry = 0) {
    if (l1 === null && l2 === null) { // 递归边界
        return carry ? new ListNode(carry) : null; // 如果进位了，就额外创建一个节点
    }
    if (l1 === null) { // 如果 l1 是空的，那么此时 l2 一定不是空节点
        [l1, l2] = [l2, l1]; // 交换 l1 与 l2，保证 l1 非空，从而简化代码
    }
    const sum = carry + l1.val + (l2 ? l2.val : 0); // 节点值和进位加在一起
    l1.val = sum % 10; // 每个节点保存一个数位（直接修改原链表）
    l1.next = addTwoNumbers(l1.next, (l2 ? l2.next : null), Math.floor(sum / 10)); // 进位
    return l1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn add_two_numbers(l1: Option<Box<ListNode>>, l2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        Self::add_two(l1, l2, 0)
    }

    // l1 和 l2 为当前遍历的节点，carry 为进位
    fn add_two(l1: Option<Box<ListNode>>, l2: Option<Box<ListNode>>, carry: i32) -> Option<Box<ListNode>> {
        match (l1, l2) {
            (None, None) => { // 递归边界
                if carry == 0 {
                    return None;
                }
                // 进位了，额外创建一个节点
                Some(Box::new(ListNode::new(carry)))
            }
            (None, Some(node2)) => Self::add_two(Some(node2), None, carry), // 交换，保证 l1 非空
            (Some(mut node1), None) => {
                let sum = node1.val + carry; // 节点值和进位加在一起
                node1.val = sum % 10; // 每个节点保存一个数位（直接修改原链表）
                node1.next = Self::add_two(node1.next.take(), None, sum / 10); // 进位
                Some(node1)
            }
            (Some(mut node1), Some(mut node2)) => {
                let sum = node1.val + node2.val + carry; // 节点值和进位加在一起
                node1.val = sum % 10; // 每个节点保存一个数位（直接修改原链表）
                node1.next = Self::add_two(node1.next.take(), node2.next.take(), sum / 10); // 进位
                Some(node1)
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $l_1$ 长度和 $l_2$ 长度的最大值。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：迭代

首先请看如何遍历一个链表，代码框架如下：

```py [sol-Python3]
# 遍历链表 l1
while l1:  # 从链表头节点开始向后遍历，直到遇到空节点
    print(l1.val)  # 当前节点值
    l1 = l1.next  # 准备遍历下一个节点
```

```java [sol-Java]
// 遍历链表 l1
while (l1 != null) { // 从链表头节点开始向后遍历，直到遇到空节点
    System.out.println(l1.val); // 当前节点值
    l1 = l1.next; // 准备遍历下一个节点
}
```

```cpp [sol-C++]
// 遍历链表 l1
while (l1) { // 从链表头节点开始向后遍历，直到遇到空节点
    cout << l1->val << endl; // 当前节点值
    l1 = l1->next; // 准备遍历下一个节点
}
```

```c [sol-C]
// 遍历链表 l1
while (l1) { // 从链表头节点开始向后遍历，直到遇到空节点
    printf("%d\n", l1->val); // 当前节点值
    l1 = l1->next; // 准备遍历下一个节点
}
```

```go [sol-Go]
// 遍历链表 l1
for l1 != nil { // 从链表头节点开始向后遍历，直到遇到空节点
    fmt.Println(l1.Val) // 当前节点值
    l1 = l1.Next // 准备遍历下一个节点
}
```

```js [sol-JavaScript]
// 遍历链表 l1
while (l1) { // 从链表头节点开始向后遍历，直到遇到空节点
    console.log(l1.val); // 当前节点值
    l1 = l1.next; // 准备遍历下一个节点
}
```

```rust [sol-Rust]
// 遍历链表 l1
let mut cur = &l1; // 这样写，下面 let Some(node) = cur 不会转移 l1 中节点的所有权
while let Some(node) = cur { // 从链表头节点开始向后遍历，直到遇到空节点
    println!("{}", node.val); // 当前节点值
    cur = &node.next; // 准备遍历下一个节点
}
```

迭代的思路是，初始化答案为一个「空链表」，每次循环，向该链表末尾添加一个节点（保存一个数位）。

循环即遍历链表 $l_1$ 和 $l_2$，每次把两个节点值 $l_1.\textit{val},\ l_2.\textit{val}$ 与进位值 $\textit{carry}$ 相加，除以 $10$ 的余数即为当前节点需要保存的数位，除以 $10$ 的商即为新的进位值。

需要注意的是，在第一次循环时，我们无法往一个空节点的末尾添加节点。这里的技巧是，创建一个**哨兵节点**（dummy node），当成初始的「空链表」。循环结束后，哨兵节点的下一个节点就是最终要返回的链表头节点。

```py [sol-Python3]
class Solution:
    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
        cur = dummy = ListNode()  # 哨兵节点
        carry = 0  # 进位值
        while l1 or l2 or carry:  # 有一个不是空节点，或者还有进位，就继续迭代
            s = carry
            if l1:
                s += l1.val  # 节点值和进位加在一起
                l1 = l1.next  # 下一个节点
            if l2:
                s += l2.val  # 节点值和进位加在一起
                l2 = l2.next  # 下一个节点
            cur.next = ListNode(s % 10)  # 每个节点保存一个数位
            carry = s // 10  # 新的进位
            cur = cur.next  # 下一个节点
        return dummy.next  # 哨兵节点的下一个节点就是头节点
```

```java [sol-Java]
class Solution {
    public ListNode addTwoNumbers(ListNode l1, ListNode l2) {
        ListNode dummy = new ListNode(); // 哨兵节点
        ListNode cur = dummy;
        int carry = 0; // 进位值
        while (l1 != null || l2 != null || carry != 0) { // 有一个不是空节点，或者还有进位，就继续迭代
            int sum = carry;
            if (l1 != null) {
                sum += l1.val; // 节点值和进位加在一起
                l1 = l1.next; // 下一个节点
            }
            if (l2 != null) {
                sum += l2.val; // 节点值和进位加在一起
                l2 = l2.next; // 下一个节点
            }
            cur = cur.next = new ListNode(sum % 10); // 每个节点保存一个数位
            carry = sum / 10; // 新的进位
        }
        return dummy.next; // 哨兵节点的下一个节点就是头节点
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode dummy; // 哨兵节点
        ListNode* cur = &dummy;
        int carry = 0; // 进位值
        while (l1 || l2 || carry) { // 有一个不是空节点，或者还有进位，就继续迭代
            int sum = carry;
            if (l1) {
                sum += l1->val; // 节点值和进位加在一起
                l1 = l1->next; // 下一个节点
            }
            if (l2) {
                sum += l2->val; // 节点值和进位加在一起
                l2 = l2->next; // 下一个节点
            }  
            cur = cur->next = new ListNode(sum % 10); // 每个节点保存一个数位
            carry = sum / 10; // 新的进位
        }
        return dummy.next; // 哨兵节点的下一个节点就是头节点
    }
};
```

```c [sol-C]
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode dummy; // 哨兵节点
    struct ListNode* cur = &dummy;
    int carry = 0; // 进位值
    while (l1 || l2 || carry) { // 有一个不是空节点，或者还有进位，就继续迭代
        int sum = carry;
        if (l1) {
            sum += l1->val; // 节点值和进位加在一起
            l1 = l1->next; // 下一个节点
        }
        if (l2) {
            sum += l2->val; // 节点值和进位加在一起
            l2 = l2->next; // 下一个节点
        }  
        cur = cur->next = malloc(sizeof(struct ListNode)); 
        cur->val = sum % 10; // 每个节点保存一个数位
        carry = sum / 10; // 新的进位
    }
    cur->next = NULL; // 注意最后一个节点是 malloc 出来的，next 不一定是 NULL，需要手动置为 NULL
    return dummy.next; // 哨兵节点的下一个节点就是头节点
}
```

```go [sol-Go]
func addTwoNumbers(l1, l2 *ListNode) *ListNode {
    dummy := ListNode{} // 哨兵节点
    cur := &dummy
    carry := 0 // 进位值
    for l1 != nil || l2 != nil || carry != 0 { // 有一个不是空节点，或者还有进位，就继续迭代
        sum := carry
        if l1 != nil {
            sum += l1.Val // 节点值和进位加在一起
            l1 = l1.Next // 下一个节点
        }
        if l2 != nil {
            sum += l2.Val // 节点值和进位加在一起
            l2 = l2.Next // 下一个节点
        }
        cur.Next = &ListNode{Val: sum % 10} // 每个节点保存一个数位
        carry = sum / 10 // 新的进位
        cur = cur.Next // 下一个节点
    }
    return dummy.Next // 哨兵节点的下一个节点就是头节点
}
```

```js [sol-JavaScript]
var addTwoNumbers = function(l1, l2) {
    const dummy = new ListNode(); // 哨兵节点
    let cur = dummy;
    let carry = 0; // 进位值
    while (l1 || l2 || carry) {
        let sum = carry;
        if (l1) {
            sum += l1.val; // 节点值和进位加在一起
            l1 = l1.next; // 下一个节点
        }
        if (l2) {
            sum += l2.val; // 节点值和进位加在一起
            l2 = l2.next; // 下一个节点
        }
        cur = cur.next = new ListNode(sum % 10); // 每个节点保存一个数位
        carry = Math.floor(sum / 10); // 新的进位
    }
    return dummy.next; // 哨兵节点的下一个节点就是头节点
};
```

```rust [sol-Rust]
impl Solution {
    pub fn add_two_numbers(mut l1: Option<Box<ListNode>>, mut l2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut dummy = ListNode::new(0); // 哨兵节点
        let mut cur = &mut dummy;
        let mut carry = 0; // 进位值
        while l1.is_some() || l2.is_some() || carry != 0 {
            let mut sum = carry;
            if let Some(node) = l1 {
                sum += node.val; // 节点值和进位加在一起
                l1 = node.next; // 下一个节点
            }
            if let Some(node) = l2 {
                sum += node.val; // 节点值和进位加在一起
                l2 = node.next; // 下一个节点
            }
            cur.next = Some(Box::new(ListNode::new(sum % 10))); // 每个节点保存一个数位
            carry = sum / 10; // 新的进位
            cur = cur.next.as_mut()?; // 下一个节点
        }
        dummy.next // 哨兵节点的下一个节点就是头节点
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $l_1$ 的长度和 $l_2$ 的长度的最大值。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 思考题

本题的链表是从数字的最低位开始的，如果改成从最高位开始，要怎么做呢？

- [206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/)
- [445. 两数相加 II](https://leetcode.cn/problems/add-two-numbers-ii/)

更多链表题目，见下面的链表题单。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、链表 / §1.8 合并链表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.8 合并链表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
