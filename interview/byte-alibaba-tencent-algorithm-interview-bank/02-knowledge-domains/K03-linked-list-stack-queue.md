# K03 链表、栈、队列与单调结构

## 核心问题

指针改写、栈匹配、队列模拟、单调栈/队列。学习目标是能识别题目中的结构、选择合适算法、证明复杂度并写出边界正确的 C++17 实现。

本域有两条主线：**链表**（指针操作，重点是反转与快慢指针，要求 O(1) 额外空间）和**栈/队列**（后进先出/先进先出的时序结构，重点是单调栈与单调队列，用"及时删除无效候选"换取线性时间）。

## 训练顺序

1. 先完成对应入门题，建立术语和模板。
2. 再完成中等级变体，训练多约束组合。
3. 高级和专家题用于训练证明、在线约束、工程边界和追问。

## 核心模式

### 模式 1：链表反转（迭代 + 头插）

反转的关键是用三个指针 `prev/cur/nxt` 逐个翻转 `next` 指向，全程 O(1) 空间。

```cpp
#include <bits/stdc++.h>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v = 0, ListNode* n = nullptr) : val(v), next(n) {}
};

ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* cur = head;
    while (cur) {
        ListNode* nxt = cur->next;         // 先保存后继，否则断链
        cur->next = prev;                  // 翻转
        prev = cur;                        // 前移
        cur = nxt;
    }
    return prev;                           // 新头
}
```

**不变量**：每轮开始时，`prev` 指向已反转段的头，`cur` 指向未处理段的头，且 `prev->...` 的链已正确反向。时间 O(n)，空间 O(1)。区间反转 `[left, right]` 用哨兵 `dummy` 定位前驱再套用同法。

### 模式 2：环检测（Floyd 快慢指针）

`slow` 每步走 1、`fast` 每步走 2。若存在环二者必相遇；相遇后从头与相遇点同步前进，再次相遇处即入环点。

```cpp
ListNode* detectCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {                // 相遇
            ListNode* p = head;
            while (p != slow) { p = p->next; slow = slow->next; }
            return p;                      // 入环点
        }
    }
    return nullptr;                        // 无环
}
```

**正确性要点**：`fast` 相对 `slow` 每轮缩短 1 个身位，故必相遇；相遇点到入环点的距离等于头到入环点的距离（模环长）。时间 O(n)，空间 O(1)。与之配套的是"找中点"：`fast` 到尾时 `slow` 在中点。

### 模式 3：单调栈（下一个更大元素）

栈内保持单调（递增或递减），新元素入栈前弹出所有破坏单调性的元素，被弹出元素即找到其"下一个更大/更小元素"。

```cpp
// 每个元素右侧第一个更大元素的下标，无则 -1
vector<int> nextGreater(vector<int>& a) {
    int n = a.size();
    vector<int> res(n, -1);
    stack<int> st;                         // 存下标，栈内值递减
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && a[st.top()] < a[i]) {
            res[st.top()] = i;             // a[i] 即其下一个更大元素
            st.pop();
        }
        st.push(i);
    }
    return res;
}
```

**不变量**：栈内下标对应的值单调递减；每个下标入栈、出栈各一次，故总时间 O(n)，空间 O(n)。柱状图最大矩形、接雨水、每日温度都是其变体。

### 模式 4：单调队列（滑动窗口最值）

用 `deque` 维护一个下标窗口，队首始终是当前窗口最大值。入队前从队尾弹出所有更小的值（它们不可能再成为任何后续窗口的最大值）。

```cpp
vector<int> maxSlidingWindow(vector<int>& a, int k) {
    deque<int> dq;                         // 存下标，对应值递减
    vector<int> res;
    for (int i = 0; i < (int)a.size(); ++i) {
        while (!dq.empty() && dq.front() <= i - k) dq.pop_front();  // 过期
        while (!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();  // 淘汰劣者
        dq.push_back(i);
        if (i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}
```

**不变量**：`dq` 中下标落在 `[i-k+1, i]` 且值严格递减，队首即窗口最大。每个下标入出队各一次，总时间 O(n)。

### 模式 5：栈模拟（括号匹配 / 表达式求值）

栈天然适合处理"最近未配对的左结构"。括号匹配遇到左括号入栈、右括号与栈顶配对待。

```cpp
bool isValid(string s) {
    stack<char> st;
    unordered_map<char, char> match{{')','('}, {']','['}, {'}','{'}};
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else {
            if (st.empty() || st.top() != match[c]) return false;
            st.pop();
        }
    }
    return st.empty();                     // 全部配对
}
```

时间 O(n)，空间 O(n)。中缀转后缀（调度场算法）与后缀求值都用同一个栈结构：求值时遇操作数入栈、遇运算符弹出两个操作数计算后回压。

## 常见陷阱

- **反转时丢失后继**：先 `cur->next = prev` 再想取 `cur->next` 已晚，务必先存 `nxt`。
- **快慢指针空指针**：`while (fast && fast->next)` 两个条件缺一不可；只判 `fast` 会在偶数长度链表上解引用空指针。
- **入环点推导记错**：相遇点在环内，必须用"头指针 + 相遇点同步走"求得入环点，而非直接返回相遇点。
- **单调栈用值而非下标**：需要区间距离或去重时，下标信息不可丢；只存值会丢失位置。
- **单调队列过期判断**：必须在入队前弹出 `<= i-k` 的队首，顺序写反会取到窗口外元素。
- **栈匹配忘记判空**：右括号到来时栈已空（如 `")"`），直接 `st.top()` 是未定义行为。
- **括号映射反向**：`match` 应以右括号为键查左括号；以左括号为键会匹配错误。
- **哨兵节点**：删除/插入头节点时不用 `dummy` 会引入大量分支，容易漏处理空链表。

## 复杂度速查

| 模式 | 时间 | 空间 | 关键前提 |
|---|---|---|---|
| 链表反转 | O(n) | O(1) | 三指针 |
| 环检测 / 找中点 | O(n) | O(1) | 快慢指针，步长 1/2 |
| 单调栈 | O(n) 均摊 | O(n) | 入出栈各一次 |
| 单调队列 | O(n) 均摊 | O(k) | 下标窗口，值单调 |
| 栈模拟匹配 | O(n) | O(n) | 最近配对结构 |
| 表达式求值 | O(n) | O(n) | 栈 + 运算符优先级 |

## 题目索引

- `../01-question-bank/intermediate/I001-valid-parentheses.md`（有效括号：栈匹配 + 哈希映射）

跨域相关题：`E001`（LRU 缓存：哈希表 + 双向链表，见 K11；可与本域链表指针操作互相印证）。

**证据与来源：** `[已确认]` 链表反转、Floyd 判环、单调栈/单调队列均为公开经典算法结论；`[推断]` 训练顺序按难度归纳。样例未在目标 OJ 逐例运行，标记 `[待验证]`。
