# 895. 最大频率栈

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-frequency-stack/
- 题目 slug：`maximum-frequency-stack`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.2 进阶
- 难度分：2028
- 外部题解来源：https://leetcode.cn/problems/maximum-frequency-stack/solutions/1998430/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-oich/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【动画】没想明白？一个动画秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-frequency-stack/solutions/1998430/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-oich/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-oich`
- topic id：`1998430`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**核心思路**：把频率（出现次数）不同的元素，压入不同的栈中。每次出栈时，弹出含有频率最高元素的栈的栈顶。

同时，为了知道每个元素的频率，还需要用一个哈希表来实时维护。

下面的 PPT 动画展示了示例 1 是如何操作的。

<![895-1.png](https://pic.leetcode.cn/1669620727-ZOBhpc-895-1.png),![895-2.png](https://pic.leetcode.cn/1669620732-WHZMWG-895-2.png),![895-3.png](https://pic.leetcode.cn/1669620804-EivPMa-895-3.png),![895-4.png](https://pic.leetcode.cn/1669620804-dUoFoC-895-4.png),![895-5.png](https://pic.leetcode.cn/1669620804-ZJESco-895-5.png),![895-6.png](https://pic.leetcode.cn/1669620804-frhnfD-895-6.png),![895-7.png](https://pic.leetcode.cn/1669620804-VLSweF-895-7.png),![895-8.png](https://pic.leetcode.cn/1669620804-mignTz-895-8.png),![895-9.png](https://pic.leetcode.cn/1669620804-VAXKMm-895-9.png),![895-10.png](https://pic.leetcode.cn/1669620804-DFCvpK-895-10.png),![895-11.png](https://pic.leetcode.cn/1669620804-cOxrmr-895-11.png)>

> 注：代码实现时，为了简单地实现，当元素频率减为 $0$ 时，并没有从哈希表中删除元素。

#### 答疑

**问**：是否会出现**中间**的某个栈为空的情况？

**答**：不会。因为出栈一定是在元素频率最高的栈上发生的，即上面动画中最右侧的非空栈。

**问**：如果有多个相同频率的数字，怎么保证弹出的是一定最接近栈顶的那个数字？

**答**：因为这个做法本质上就是**把原始栈拆分成多个栈**，每个栈都存储着相同频率的数字（这里的频率指的是数字入栈时的频率），且保持了原有的入栈顺序，因此弹出的是一定最接近栈顶的那个数字。

```py [sol-Python3]
class FreqStack:
    def __init__(self):
        self.stacks = []  # 栈套栈，stacks 中的每个元素都是一个栈
        self.cnt = defaultdict(int)  # 每个 val 的出现次数

    def push(self, val: int) -> None:
        if self.cnt[val] == len(self.stacks):  # 这个元素的频率已经是目前最多的，现在又出现了一次
            self.stacks.append([val])  # 那么必须创建一个新栈
        else:
            self.stacks[self.cnt[val]].append(val)  # 否则就压入对应的栈
        self.cnt[val] += 1  # 更新频率

    def pop(self) -> int:
        val = self.stacks[-1].pop()  # 弹出最右侧栈的栈顶元素
        if not self.stacks[-1]:  # 弹出后，最右侧栈为空
            self.stacks.pop()  # 删除
        self.cnt[val] -= 1  # 更新频率
        return val
```

```java [sol-Java]
class FreqStack {
    // 栈套栈，stacks 中的每个元素都是一个栈
    private final List<Deque<Integer>> stacks = new ArrayList<>();

    // 每个 val 的出现次数
    private final Map<Integer, Integer> cnt = new HashMap<>();

    public void push(int val) {
        int c = cnt.getOrDefault(val, 0);
        if (c == stacks.size()) { // 这个元素的频率已经是目前最多的，现在又出现了一次
            stacks.add(new ArrayDeque<>()); // 那么必须创建一个新栈
        }
        stacks.get(c).push(val);
        cnt.put(val, c + 1); // 更新频率
    }

    public int pop() {
        int back = stacks.size() - 1;
        int val = stacks.get(back).pop(); // 弹出最右侧栈的栈顶
        if (stacks.get(back).isEmpty()) { // 栈为空
            stacks.remove(back); // 删除
        }
        cnt.merge(val, -1, Integer::sum); // 更新频率
        return val;
    }
}
```

```cpp [sol-C++]
class FreqStack {
    vector<stack<int>> stacks; // 栈套栈，stacks 中的每个元素都是一个栈
    unordered_map<int, int> cnt; // 每个 val 的出现次数
public:
    void push(int val) {
        if (cnt[val] == stacks.size()) { // 这个元素的频率已经是目前最多的，现在又出现了一次
            stacks.push_back({}); // 那么必须创建一个新栈
        }
        stacks[cnt[val]].push(val);
        cnt[val]++; // 更新频率
    }

    int pop() {
        int val = stacks.back().top(); // 弹出最右侧栈的栈顶
        stacks.back().pop();
        if (stacks.back().empty()) { // 栈为空
            stacks.pop_back(); // 删除
        }
        cnt[val]--; // 更新频率
        return val;
    }
};
```

```go [sol-Go]
type FreqStack struct {
    stacks [][]int     // 栈套栈，stacks 中的每个元素都是一个栈
    cnt    map[int]int // 每个 val 的出现次数
}

func Constructor() FreqStack {
    return FreqStack{cnt: map[int]int{}}
}

func (f *FreqStack) Push(val int) {
    c := f.cnt[val]
    if c == len(f.stacks) { // 这个元素的频率已经是目前最多的，现在又出现了一次
        f.stacks = append(f.stacks, []int{val}) // 那么必须创建一个新栈
    } else {
        f.stacks[c] = append(f.stacks[c], val) // 否则就压入对应的栈
    }
    f.cnt[val]++ // 更新频率
}

func (f *FreqStack) Pop() int {
    back := len(f.stacks) - 1
    st := f.stacks[back]
    bk := len(st) - 1
    val := st[bk] // 弹出最右侧栈的栈顶
    if bk == 0 { // 栈为空
        f.stacks = f.stacks[:back] // 删除
    } else {
        f.stacks[back] = st[:bk]
    }
    f.cnt[val]-- // 更新频率
    return val
}
```

#### 复杂度分析

- 时间复杂度：均为 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(q)$，其中 $q$ 为 $\text{push}$ 调用的次数。

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
