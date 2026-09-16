# 1047. 删除字符串中的所有相邻重复项

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/
- 题目 slug：`remove-all-adjacent-duplicates-in-string`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.3 邻项消除
- 难度分：1286
- 外部题解来源：https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/solutions/4006521/tao-lu-lin-xiang-xiao-chu-wen-ti-fu-ti-d-936f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【套路】邻项消除问题，附题单（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/solutions/4006521/tao-lu-lin-xiang-xiao-chu-wen-ti-fu-ti-d-936f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tao-lu-lin-xiang-xiao-chu-wen-ti-fu-ti-d-936f`
- topic id：`4006521`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**思考**：消除顺序是否影响最终结果？如果不影响，那么可以从左到右消除。

情况一：不重叠的相邻重复项。

设 $s = \cdots a_1a_2\cdots b_1b_2\cdots$ 其中 $a_1=a_2$ 且 $b_1=b_2$。$(a_1,a_2)$ 和 $(b_1,b_2)$ 谁先谁后消除，产生的新字符串是完全一致的。

情况二：重叠的相邻重复项。

设 $s = \cdots a_1a_2a_3\cdots$ 其中 $a_1=a_2=a_3$。如果先消除 $(a_1,a_2)$，则剩下 $a_3$；如果先消除 $(a_2,a_3)$，则剩下 $a_1$。由于 $a_1=a_3$，所以两种消除方式产生的新字符串是完全一致的。

所以不同的消除顺序不影响最终结果，我们可以从左到右消除。

**思考**：如何知道左侧最近未被消除的字母？

以 $s = \texttt{abbaca}$ 为例。先消除 $\texttt{bb}$，得到 $\texttt{a\_\_aca}$。当我们遍历到第二个 $\texttt{a}$ 时，如何**快速**知道左侧最近未被消除的字母是什么？

我们需要一个数据结构，支持 $\mathcal{O}(1)$ 时间查看与删除最近添加的字母。这个数据结构是**栈**。

用一个栈记录所有未被消除的字母。遍历到 $s[i]$ 时：

- 如果栈不为空且 $s[i]$ 与栈顶相同，那么可以消除 $s[i]$，同时移除栈顶。
- 否则 $s[i]$ 无法消除，把 $s[i]$ 入栈。

最终的栈就是剩下的字母，顺序为从栈底到栈顶。

```py [sol-Python3]
class Solution:
    def removeDuplicates(self, s: str) -> str:
        st = []
        for ch in s:
            if st and st[-1] == ch:
                st.pop()
            else:
                st.append(ch)
        return ''.join(st)
```

```java [sol-Java]
class Solution {
    public String removeDuplicates(String s) {
        char[] st = new char[s.length()]; // 用数组模拟栈
        int size = 0;
        for (char ch : s.toCharArray()) {
            if (size > 0 && st[size - 1] == ch) {
                size--;
            } else {
                st[size++] = ch;
            }
        }
        return new String(st, 0, size);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string removeDuplicates(string s) {
        string st;
        for (char ch : s) {
            if (!st.empty() && st.back() == ch) {
                st.pop_back();
            } else {
                st += ch;
            }
        }
        return st;
    }
};
```

```c [sol-C]
char* removeDuplicates(char* s) {
    int n = strlen(s);
    char* st = malloc((n + 1) * sizeof(char)); // 用数组模拟栈
    int size = 0;

    for (int i = 0; i < n; i++) {
        if (size > 0 && st[size - 1] == s[i]) {
            size--;
        } else {
            st[size++] = s[i];
        }
    }

    st[size] = '\0';
    return st;
}
```

```c [sol-C 原地]
char* removeDuplicates(char* s) {
    int size = 0;

    for (int i = 0; s[i]; i++) {
        if (size > 0 && s[size - 1] == s[i]) {
            size--;
        } else {
            s[size++] = s[i];
        }
    }

    s[size] = '\0';
    return s;
}
```

```go [sol-Go]
func removeDuplicates(s string) string {
	st := []byte{}
	for i := range s {
		if len(st) > 0 && st[len(st)-1] == s[i] {
			st = st[:len(st)-1]
		} else {
			st = append(st, s[i])
		}
	}
	return string(st)
}
```

```js [sol-JavaScript]
var removeDuplicates = function(s) {
    const st = [];
    for (const ch of s) {
        if (st.length && st[st.length - 1] === ch) {
            st.pop();
        } else {
            st.push(ch);
        }
    }
    return st.join("");
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_duplicates(s: String) -> String {
        let mut st = vec![];
        for ch in s.bytes() {
            if !st.is_empty() && st[st.len() - 1] == ch {
                st.pop();
            } else {
                st.push(ch);
            }
        }
        unsafe { String::from_utf8_unchecked(st) }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。原地做法见 C 语言写法二。

## 专题训练

见下面数据结构题单的「**§3.3 邻项消除**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.3 邻项消除`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.3 邻项消除`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
