# 1172. 餐盘栈

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/dinner-plate-stacks/
- 题目 slug：`dinner-plate-stacks`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.2 进阶
- 难度分：2110
- 外部题解来源：https://leetcode.cn/problems/dinner-plate-stacks/solutions/2248707/yu-qi-wei-hu-di-yi-ge-wei-man-zhan-bu-ru-sphs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[与其维护第一个未满栈，不如维护所有未满栈（Python/Java/C++/Go）](https://leetcode.cn/problems/dinner-plate-stacks/solutions/2248707/yu-qi-wei-hu-di-yi-ge-wei-man-zhan-bu-ru-sphs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-qi-wei-hu-di-yi-ge-wei-man-zhan-bu-ru-sphs`
- topic id：`2248707`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

试想一下，假如一开始连续执行了很多次 $\texttt{push}$ 操作，就会得到一排整整齐齐的栈；然后再执行一些 $\texttt{popAtStack}$ 操作，随机挑选 $\textit{index}$，就会把这些栈弄得参差不齐。

> 注：如果一个栈清空了，那么它会继续占据着这个位置，它右侧的栈不会挤过来。

这个时候再交替执行 $\texttt{push}$ 和 $\texttt{popAtStack}$，「从左往右第一个没有满的栈」就没有什么规律可言了。如果把第一个未满栈填满，就要暴力寻找下一个未满栈了。如何优化？

格局打开：**与其维护第一个未满栈，不如维护所有未满栈。**

假设可以用一个数据结构来维护这些未满栈（的下标），看看需要哪些操作：

- 对于 $\texttt{push}$ 来说，需要知道这些下标中的最小值。如果入栈后，栈满了，那么这个栈就不能算作未满栈，此时这个最小下标就需要从数据结构中移除。
- 对于 $\texttt{popAtStack}$ 来说，如果操作的是一个满栈，操作后就得到了一个未满栈，那么就往数据结构中添加这个栈的下标。
- 对于 $\texttt{pop}$ 来说，它等价于 $\texttt{popAtStack}(\textit{lastIndex})$，其中 $\textit{lastIndex}$ 是最后一个非空栈的下标。

所以我们需要一个支持添加元素、查询最小值和移除最小值的数据结构。**最小堆，就你了。**

此外还需要一个列表 $\textit{stacks}$，它的每个元素都是一个栈。上面说的 $\textit{lastIndex}$ 就是 $\textit{stacks}$ 的长度减一。

如何维护 $\textit{stacks}$ 呢？

- 如果 $\texttt{push}$ 时最小堆为空，说明所有栈都是满的，那么就需要向 $\textit{stacks}$ 的末尾添加一个新的栈。如果 $\textit{capacity}>1$，就把这个新的未满栈的下标入堆。
- 如果 $\texttt{popAtStack}$ 操作的是最后一个栈，且操作后栈为空，那么就从 $\textit{stacks}$ 中移除最后一个栈。如果移除后 $\textit{stacks}$ 的最后一个栈也是空的，就不断移除直到 $\textit{stacks}$ 为空或者最后一个栈不为空。**细节**：需要同步移除最小堆中的下标吗？其实不需要，如果在 $\texttt{push}$ 时发现堆顶的下标大于等于 $\textit{stacks}$ 的长度，把整个堆清空即可。（这个技巧叫懒删除。）

此外，如果 $\texttt{popAtStack}$ 操作越界或者操作的栈为空，则返回 $-1$。

### 答疑

**问**：懒删除是否会导致堆中有重复的下标？

**答**：不会有重复下标。假设重复下标是在 $\texttt{push}$ 时插入的，说明此前所有栈都是满的，堆中残留的下标必然都大于等于 $\textit{stacks}$ 的长度，但这种情况下 $\texttt{push}$ 会清空堆，不会导致重复下标。假设重复下标是在 $\texttt{popAtStack}$ 时插入的，这只会发生在 $\textit{stacks}[\textit{index}]$ 是满栈的情况下，而 $\texttt{push}$ 保证在满栈时会去掉这个下标，所以也不会导致重复下标。

```py [sol1-Python3]
class DinnerPlates:
    def __init__(self, capacity: int):
        self.capacity = capacity  # 栈的容量
        self.stacks = []  # 所有栈
        self.h = []  # 最小堆，保存未满栈的下标

    def push(self, val: int) -> None:
        if self.h and self.h[0] >= len(self.stacks):
            self.h = []  # 堆中都是越界下标，直接清空
        if self.h:  # 还有未满栈
            self.stacks[self.h[0]].append(val)  # 入栈
            if len(self.stacks[self.h[0]]) == self.capacity:  # 栈满了
                heappop(self.h)  # 从堆中去掉
        else:  # 所有栈都是满的
            self.stacks.append([val])  # 添加一个新的栈
            if self.capacity > 1:  # 新的栈没有满
                heappush(self.h, len(self.stacks) - 1)  # 入堆

    def pop(self) -> int:
        # 等价为 popAtStack 最后一个非空栈
        return self.popAtStack(len(self.stacks) - 1)

    def popAtStack(self, index: int) -> int:
        if index < 0 or index >= len(self.stacks) or len(self.stacks[index]) == 0:
            return -1  # 非法操作
        if len(self.stacks[index]) == self.capacity:  # 满栈
            heappush(self.h, index)  # 元素出栈后，栈就不满了，把下标入堆
        val = self.stacks[index].pop()
        while self.stacks and len(self.stacks[-1]) == 0:
            self.stacks.pop()  # 去掉末尾的空栈（懒删除，堆中下标在 push 时处理）
        return val
```

```java [sol1-Java]
class DinnerPlates {
    // 栈的容量
    private int capacity;
    // 所有栈
    private List<Deque<Integer>> stacks = new ArrayList<>();
    // 最小堆，保存未满栈的下标
    private PriorityQueue<Integer> idx = new PriorityQueue<>();

    public DinnerPlates(int capacity) {
        this.capacity = capacity;
    }

    public void push(int val) {
        if (!idx.isEmpty() && idx.peek() >= stacks.size())
            idx.clear(); // 堆中都是越界下标，直接清空
        if (idx.isEmpty()) { // 所有栈都是满的
            var st = new ArrayDeque<Integer>();
            st.push(val);
            stacks.add(st); // 添加一个新的栈
            if (capacity > 1) // 新的栈没有满
                idx.add(stacks.size() - 1); // 入堆
        } else { // 还有未满栈
            var st = stacks.get(idx.peek());
            st.push(val); // 入栈
            if (st.size() == capacity) // 栈满了
                idx.poll(); // 从堆中去掉
        }
    }

    public int pop() {
        // 等价为 popAtStack 最后一个非空栈
        return popAtStack(stacks.size() - 1);
    }

    public int popAtStack(int index) {
        if (index < 0 || index >= stacks.size() || stacks.get(index).isEmpty())
            return -1; // 非法操作
        var st = stacks.get(index);
        if (st.size() == capacity) // 满栈
            idx.add(index); // 元素出栈后，栈就不满了，把下标入堆
        int val = st.pop();
        // 去掉末尾的空栈（懒删除，堆中下标在 push 时处理）
        while (!stacks.isEmpty() && stacks.get(stacks.size() - 1).isEmpty())
            stacks.remove(stacks.size() - 1);
        return val;
    }
}
```

```cpp [sol1-C++]
class DinnerPlates {
    int capacity; // 栈的容量
    vector<stack<int>> stacks; // 所有栈
    priority_queue<int, vector<int>, greater<>> idx; // 未满栈的下标
public:
    DinnerPlates(int capacity) : capacity(capacity) {}

    void push(int val) {
        if (!idx.empty() && idx.top() >= stacks.size())
            while (!idx.empty()) idx.pop(); // 堆中都是越界下标，直接清空
        if (idx.empty()) { // 所有栈都是满的
            stack<int> st;
            st.push(val);
            stacks.emplace_back(st); // 添加一个新的栈
            if (capacity > 1) // 新的栈没有满
                idx.push(stacks.size() - 1); // 入堆
        } else { // 还有未满栈
            auto &st = stacks[idx.top()];
            st.push(val); // 入栈
            if (st.size() == capacity) // 栈满了
                idx.pop(); // 从堆中去掉
        }
    }

    int pop() {
        // 等价为 popAtStack 最后一个非空栈
        return popAtStack(stacks.size() - 1);
    }

    int popAtStack(int index) {
        if (index < 0 || index >= stacks.size() || stacks[index].empty())
            return -1; // 非法操作
        auto &st = stacks[index];
        if (st.size() == capacity) // 满栈
            idx.push(index); // 元素出栈后，栈就不满了，把下标入堆
        int val = st.top();
        st.pop();
        while (!stacks.empty() && stacks.back().empty())
            stacks.pop_back(); // 去掉末尾的空栈（懒删除，堆中下标在 push 时处理）
        return val;
    }
};
```

```go [sol1-Go]
type DinnerPlates struct {
    capacity int     // 栈的容量
    stacks   [][]int // 所有栈
    idx      hp      // 最小堆，保存未满栈的下标
}

func Constructor(capacity int) DinnerPlates {
    return DinnerPlates{capacity: capacity}
}

func (d *DinnerPlates) Push(val int) {
    if d.idx.Len() > 0 && d.idx.IntSlice[0] >= len(d.stacks) {
        d.idx = hp{} // 堆中都是越界下标，直接清空
    }
    if d.idx.Len() == 0 { // 所有栈都是满的
        d.stacks = append(d.stacks, []int{val}) // 添加一个新的栈
        if d.capacity > 1 { // 新的栈没有满
            heap.Push(&d.idx, len(d.stacks)-1) // 入堆
        }
    } else { // 还有未满栈
        i := d.idx.IntSlice[0]
        d.stacks[i] = append(d.stacks[i], val) // 入栈
        if len(d.stacks[i]) == d.capacity { // 栈满了
            heap.Pop(&d.idx) // 从堆中去掉
        }
    }
}

func (d *DinnerPlates) Pop() int {
    // 等价为 popAtStack 最后一个非空栈
    return d.PopAtStack(len(d.stacks) - 1)
}

func (d *DinnerPlates) PopAtStack(index int) int {
    if index < 0 || index >= len(d.stacks) || len(d.stacks[index]) == 0 {
        return -1 // 非法操作
    }
    if len(d.stacks[index]) == d.capacity { // 满栈
        heap.Push(&d.idx, index) // 元素出栈后，栈就不满了，把下标入堆
    }
    bk := len(d.stacks[index]) - 1
    val := d.stacks[index][bk]
    d.stacks[index] = d.stacks[index][:bk]
    for len(d.stacks) > 0 && len(d.stacks[len(d.stacks)-1]) == 0 {
        d.stacks = d.stacks[:len(d.stacks)-1] // 去掉末尾的空栈（懒删除，堆中下标在 push 时处理）
    }
    return val
}

type hp struct{ sort.IntSlice }
func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(q\log q)$，其中 $q$ 为操作次数。虽然在 $\texttt{popAtStack}$ 中有个循环，但是均摊考虑，被 $\texttt{popAtStack}$ 清空的栈，只贡献了一次循环，所以均摊来说 $\texttt{popAtStack}$ 的时间复杂度为 $\mathcal{O}(\log q)$。
- 空间复杂度：$\mathcal{O}(q)$。

### 思考题

添加方法 `bool pushAtStack(int index, int val)`（将 $\textit{val}$ 推入编号为 $\textit{index}$ 的栈，返回是否成功入栈），要怎么实现呢？欢迎在评论区发表你的思路。

（变形：如果 $\textit{index}$ 处的栈满了，则推入编号大于 $\textit{index}$ 的第一个未满栈。）

[往期每日一题题解](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

---

欢迎关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
