# 682. 棒球比赛

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/baseball-game/
- 题目 slug：`baseball-game`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/baseball-game/solutions/2861078/yong-lie-biao-mo-ni-pythonjavaccgojsrust-pj1t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用列表模拟（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/baseball-game/solutions/2861078/yong-lie-biao-mo-ni-pythonjavaccgojsrust-pj1t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-lie-biao-mo-ni-pythonjavaccgojsrust-pj1t`
- topic id：`2861078`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

遍历 $\textit{operations}$，用列表（或者栈）模拟题目的四个操作：

1. 操作一，把 $x$ 加到列表末尾。
2. 操作二，把列表的后两项之和加到列表末尾。
3. 操作三，把列表最后一项的两倍加到列表末尾。
4. 操作四，去掉列表最后一项。

遍历结束后，返回列表中的元素之和。

```py [sol-Python3]
class Solution:
    def calPoints(self, operations: List[str]) -> int:
        st = []
        for op in operations:
            if op == '+':
                st.append(st[-2] + st[-1])
            elif op == 'D':
                st.append(st[-1] * 2)
            elif op == 'C':
                st.pop()
            else:
                st.append(int(op))
        return sum(st)
```

```java [sol-Java]
class Solution {
    public int calPoints(String[] operations) {
        List<Integer> st = new ArrayList<>();
        for (String op : operations) {
            switch (op.charAt(0)) {
                case '+':
                    st.add(st.get(st.size() - 2) + st.get(st.size() - 1));
                    break;
                case 'D':
                    st.add(st.get(st.size() - 1) * 2);
                    break;
                case 'C':
                    st.remove(st.size() - 1);
                    break;
                default:
                    st.add(Integer.parseInt(op));
            }
        }
        int sum = 0;
        for (int x : st) {
            sum += x;
        }
        return sum;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int calPoints(String[] operations) {
        int[] st = new int[operations.length];
        int m = 0;
        for (String op : operations) {
            switch (op.charAt(0)) {
                case '+':
                    st[m] = st[m - 2] + st[m - 1];
                    m++;
                    break;
                case 'D':
                    st[m] = st[m - 1] * 2;
                    m++;
                    break;
                case 'C':
                    m--;
                    break;
                default:
                    st[m++] = Integer.parseInt(op);
            }
        }
        int sum = 0;
        for (int i = 0; i < m; i++) {
            sum += st[i];
        }
        return sum;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int calPoints(vector<string>& operations) {
        vector<int> st;
        for (auto& op : operations) {
            switch (op[0]) {
                case '+':
                    st.push_back(st[st.size() - 2] + st.back());
                    break;
                case 'D':
                    st.push_back(st.back() * 2);
                    break;
                case 'C':
                    st.pop_back();
                    break;
                default:
                    st.push_back(stoi(op));
            }
        }
        return reduce(st.begin(), st.end());
    }
};
```

```c [sol-C]
int calPoints(char** operations, int operationsSize) {
    int* st = malloc(operationsSize * sizeof(int));
    int m = 0;
    for (int i = 0; i < operationsSize; i++) {
        switch (operations[i][0]) {
            case '+':
                st[m] = st[m - 2] + st[m - 1];
                m++;
                break;
            case 'D':
                st[m] = st[m - 1] * 2;
                m++;
                break;
            case 'C':
                m--;
                break;
            default:
                st[m++] = atoi(operations[i]);
        }
    }
    int sum = 0;
    for (int i = 0; i < m; i++) {
        sum += st[i];
    }
    free(st);
    return sum;
}
```

```go [sol-Go]
func calPoints(operations []string) (sum int) {
    st := []int{}
    for _, op := range operations {
        switch op[0] {
        case '+':
            st = append(st, st[len(st)-2]+st[len(st)-1])
        case 'D':
            st = append(st, st[len(st)-1]*2)
        case 'C':
            st = st[:len(st)-1]
        default:
            x, _ := strconv.Atoi(op)
            st = append(st, x)
        }
    }
    for _, x := range st {
        sum += x
    }
    return
}
```

```js [sol-JavaScript]
var calPoints = function(operations) {
    const st = [];
    for (const op of operations) {
        switch (op) {
            case "+":
                st.push(st[st.length - 2] + st[st.length - 1]);
                break;
            case "D":
                st.push(st[st.length - 1] * 2);
                break;
            case "C":
                st.pop();
                break;
            default:
                st.push(parseInt(op));
        }
    }
    return _.sum(st);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn cal_points(operations: Vec<String>) -> i32 {
        let mut st = vec![];
        for op in operations {
            match op.as_bytes()[0] {
                b'+' => st.push(st[st.len() - 2] + st[st.len() - 1]),
                b'D' => st.push(st[st.len() - 1] * 2),
                b'C' => { st.pop(); }
                _ => st.push(op.parse::<i32>().unwrap())
            }
        }
        st.iter().sum()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{operations}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面数据结构题单中的「**栈**」。

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
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
