# 2810. 故障键盘

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/faulty-keyboard/
- 题目 slug：`faulty-keyboard`
- 来源专题：常用数据结构
- 来源分类路径：四、队列 / §4.3 双端队列
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/faulty-keyboard/solutions/2375152/shuang-duan-dui-lie-on-zuo-fa-by-endless-5pe1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[双端队列 O(n) 做法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/faulty-keyboard/solutions/2375152/shuang-duan-dui-lie-on-zuo-fa-by-endless-5pe1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shuang-duan-dui-lie-on-zuo-fa-by-endless-5pe1`
- topic id：`2375152`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**提示**：把反转看成是往字符串的头部添加字符。

具体来说：

- 如果当前处于「往字符串尾部添加字符」的状态，那么遇到 `i` 后，改成「往字符串头部添加字符」的状态。
- 如果当前处于「往字符串头部添加字符」的状态，那么遇到 `i` 后，改成「往字符串尾部添加字符」的状态。

这可以用**双端队列**实现。

不想用双端队列的话，可以参考下面 Go 和 JS 的实现。

附：[视频讲解](https://www.bilibili.com/video/BV1Yr4y1o7aP/)

```py [sol-Python3]
class Solution:
    def finalString(self, s: str) -> str:
        q = deque()
        tail = True
        for c in s:
            if c == 'i':
                tail = not tail  # 修改添加方向
            elif tail:  # 加尾部
                q.append(c)
            else:  # 加头部
                q.appendleft(c)
        return ''.join(q if tail else reversed(q))
```

```java [sol-Java]
class Solution {
    public String finalString(String s) {
        Deque<Character> q = new ArrayDeque<>();
        boolean tail = true;
        for (char c : s.toCharArray()) {
            if (c == 'i') {
                tail = !tail; // 修改添加方向
            } else if (tail) {
                q.addLast(c); // 加尾部
            } else {
                q.addFirst(c); // 加头部
            }
        }
        StringBuilder ans = new StringBuilder();
        for (char c : q) {
            ans.append(c);
        }
        if (!tail) {
            ans.reverse();
        }
        return ans.toString();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string finalString(string s) {
        deque<char> q;
        bool tail = true;
        for (char c : s) {
            if (c == 'i') tail = !tail; // 修改添加方向
            else if (tail) q.push_back(c); // 加尾部
            else q.push_front(c); // 加头部
        }
        return tail ? string(q.begin(), q.end()) : string(q.rbegin(), q.rend());
    }
};
```

```go [sol-Go]
func finalString(s string) string {
	q := [2][]rune{} // 两个 slice 背靠背，q[0] 向左，q[1] 向右
	dir := 1
	for _, c := range s {
		if c == 'i' {
			dir ^= 1 // 修改添加方向
		} else {
			q[dir] = append(q[dir], c)
		}
	}
	slices.Reverse(q[dir^1])
	return string(append(q[dir^1], q[dir]...))
}
```

```js [sol-JavaScript]
var finalString = function(s) {
    const q = [[], []] // 两个 list 背靠背，q[0] 向左，q[1] 向右
    let dir = 1;
    for (const c of s) {
        if (c === 'i') {
            dir ^= 1; // 修改添加方向
        } else {
            q[dir].push(c);
        }
    }
    return q[dir ^ 1].reverse().concat(q[dir]).join('');
};
```

```rust [sol-Rust]
use std::collections::VecDeque;

impl Solution {
    pub fn final_string(s: String) -> String {
        let mut q = VecDeque::new();
        let mut tail = true;
        for c in s.chars() {
            if c == 'i' {
                tail = !tail; // 修改添加方向
            } else if tail {
                q.push_back(c); // 加尾部
            } else {
                q.push_front(c); // 加头部
            }
        }
        if tail {
            q.iter().collect()
        } else {
            q.iter().rev().collect()
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `四、队列 / §4.3 双端队列`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、队列 / §4.3 双端队列`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
