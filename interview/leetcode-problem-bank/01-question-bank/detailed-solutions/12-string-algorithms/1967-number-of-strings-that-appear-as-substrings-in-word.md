# 1967. 作为子字符串出现在单词中的字符串数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-strings-that-appear-as-substrings-in-word/
- 题目 slug：`number-of-strings-that-appear-as-substrings-in-word`
- 来源专题：字符串
- 来源分类路径：七、AC 自动机
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-strings-that-appear-as-substrings-in-word/solutions/936638/mo-ni-by-endlesscheng-chru/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力 / AC 自动机（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-strings-that-appear-as-substrings-in-word/solutions/936638/mo-ni-by-endlesscheng-chru/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ni-by-endlesscheng-chru`
- topic id：`936638`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 13:14:52 +0800

## 方法一：暴力

遍历 $\textit{patterns}$。如果 $\textit{patterns}[i]$ 在 $\textit{word}$ 中，把答案增加一。

```py [sol-Python3]
class Solution:
    def numOfStrings(self, patterns: List[str], word: str) -> int:
        return sum(pattern in word for pattern in patterns)
```

```java [sol-Java]
class Solution {
    public int numOfStrings(String[] patterns, String word) {
        int ans = 0;
        for (String pattern : patterns) {
            if (word.contains(pattern)) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numOfStrings(vector<string>& patterns, string word) {
        int ans = 0;
        for (auto& pattern : patterns) {
            if (word.contains(pattern)) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func numOfStrings(patterns []string, word string) (ans int) {
	for _, pattern := range patterns {
		if strings.Contains(word, pattern) {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nL)$，其中 $n$ 是 $\textit{word}$ 的长度，$L$ 是 $\textit{patterns}$ 中所有字符串的长度之和。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：AC 自动机

原理见 [OI Wiki](https://oi-wiki.org/string/ac-automaton/)。学习之前推荐先阅读 [字典树原理讲解](https://leetcode.cn/problems/implement-trie-prefix-tree/solutions/2993894/cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4/) 和 [KMP 原理讲解](https://www.zhihu.com/question/21923021/answer/37475572)。

⚠**注**：本题数据范围小，AC 自动机运行时间比暴力慢。读者可以在 [P3808 AC 自动机（简单版）](https://www.luogu.com.cn/problem/P3808) 提交数据范围更大的版本。

```py [sol-Python3]
class Node:
    __slots__ = 'son', 'fail', 'last', 'cnt'

    def __init__(self):
        self.son = [None] * 26
        self.fail = None  # 当 node.son[i] 失配时，node.fail.son[i] 即为下一个待匹配节点（等于 root 则表示没有匹配）
        self.last = None  # 后缀链接（suffix link），用来快速跳到一定是某个模式串末尾的节点（等于 root 则表示匹配结束）
        self.cnt = 0  # node 是 cnt 个模式串的末尾


class AhoCorasick:
    def __init__(self):
        self.root = Node()

    # 把模式串 pattern 插入 AC 自动机（代码和字典树一样）
    def put(self, pattern: str) -> None:
        cur = self.root
        for ch in pattern:
            i = ord(ch) - ord('a')
            if cur.son[i] is None:
                cur.son[i] = Node()
            cur = cur.son[i]
        cur.cnt += 1

    # BFS，构建 AC 自动机的 fail 和 last，方便快速查询
    def build_fail(self) -> None:
        self.root.fail = self.root.last = self.root

        q = deque()
        for i, son in enumerate(self.root.son):
            if son is None:
                self.root.son[i] = self.root
                continue
            son.fail = son.last = self.root  # 第一层的 fail 都指向根节点
            q.append(son)

        # BFS
        while q:
            cur = q.popleft()
            for i, son in enumerate(cur.son):
                if son is None:
                    # 把虚拟子节点 cur.son[i] 设置为 cur.fail.son[i]
                    # 方便失配时直接跳到下一个可能匹配的位置（但不一定是某个模式串的末尾）
                    cur.son[i] = cur.fail.son[i]
                    continue
                son.fail = cur.fail.son[i]  # 计算失配位置
                # 沿着 last 往上走，可以直接跳到一定是某个模式串末尾的节点（如果跳到 root 表示匹配结束）
                son.last = son.fail if son.fail.cnt else son.fail.last
                q.append(son)


class Solution:
    def numOfStrings(self, patterns: List[str], word: str) -> int:
        ac = AhoCorasick()
        for pattern in patterns:
            ac.put(pattern)
        ac.build_fail()

        ord_a = ord('a')
        cur = ac.root
        ans = 0
        for ch in word:
            cur = cur.son[ord(ch) - ord_a]  # 如果没有匹配，相当于移动到 fail 的 son[ord(ch)-ord_a]
            match_node = cur
            while match_node.cnt >= 0:
                ans += match_node.cnt
                match_node.cnt = -1  # 避免重复统计
                match_node = match_node.last  # 可能匹配更短的模式串，要继续在 last 链上找
        return ans
```

```java [sol-Java]
class Node {
    Node[] son = new Node[26];
    Node fail; // 当 node.son[i] 失配时，node.fail.son[i] 即为下一个待匹配节点（等于 root 则表示没有匹配）
    Node last; // 后缀链接（suffix link），用来快速跳到一定是某个模式串末尾的节点（等于 root 则表示匹配结束）
    int cnt; // node 是 cnt 个模式串的末尾
}

class AhoCorasick {
    Node root = new Node();

    // 把模式串 pattern 插入 AC 自动机（代码和字典树一样）
    void put(String pattern) {
        Node cur = root;
        for (char ch : pattern.toCharArray()) {
            ch -= 'a';
            if (cur.son[ch] == null) {
                cur.son[ch] = new Node();
            }
            cur = cur.son[ch];
        }
        cur.cnt++;
    }

    // BFS，构建 AC 自动机的 fail 和 last，方便快速查询
    void buildFail() {
        root.fail = root.last = root;

        Queue<Node> q = new ArrayDeque<>();
        for (int i = 0; i < root.son.length; i++) {
            Node son = root.son[i];
            if (son == null) {
                root.son[i] = root;
                continue;
            }
            son.fail = son.last = root; // 第一层的 fail 都指向根节点
            q.add(son);
        }

        // BFS
        while (!q.isEmpty()) {
            Node cur = q.poll();
            for (int i = 0; i < 26; i++) {
                Node son = cur.son[i];
                if (son == null) {
                    // 把虚拟子节点 cur.son[i] 设置为 cur.fail.son[i]
                    // 方便失配时直接跳到下一个可能匹配的位置（但不一定是某个模式串的末尾）
                    cur.son[i] = cur.fail.son[i];
                    continue;
                }
                son.fail = cur.fail.son[i]; // 计算失配位置
                // 沿着 last 往上走，可以直接跳到一定是某个模式串末尾的节点（如果跳到 root 表示匹配结束）
                son.last = son.fail.cnt > 0 ? son.fail : son.fail.last;
                q.add(son);
            }
        }
    }
}

class Solution {
    public int numOfStrings(String[] patterns, String word) {
        AhoCorasick ac = new AhoCorasick();
        for (String pattern : patterns) {
            ac.put(pattern);
        }
        ac.buildFail();

        Node cur = ac.root;
        int ans = 0;
        for (char ch : word.toCharArray()) {
            cur = cur.son[ch - 'a']; // 如果没有匹配，相当于移动到 fail 的 son[ch-'a']
            // 可能匹配更短的模式串，要继续在 last 链上找
            for (Node match = cur; match.cnt >= 0; match = match.last) {
                ans += match.cnt;
                match.cnt = -1; // 避免重复统计
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
struct Node {
    Node* son[26]{};
    Node* fail; // 当 node.son[i] 失配时，node.fail.son[i] 即为下一个待匹配节点（等于 root 则表示没有匹配）
    Node* last; // 后缀链接（suffix link），用来快速跳到一定是某个模式串末尾的节点（等于 root 则表示匹配结束）
    int cnt = 0; // node 是 cnt 个模式串的末尾
};

struct AhoCorasick {
    Node* root = new Node();

    // 把模式串 pattern 插入 AC 自动机（代码和字典树一样）
    void put(string& pattern) {
        Node* cur = root;
        for (char ch : pattern) {
            ch -= 'a';
            if (cur->son[ch] == nullptr) {
                cur->son[ch] = new Node();
            }
            cur = cur->son[ch];
        }
        cur->cnt++;
    }

    // BFS，构建 AC 自动机的 fail 和 last，方便快速查询
    void build_fail() {
        root->fail = root->last = root;

        queue<Node*> q;
        for (auto& son : root->son) {
            if (son == nullptr) {
                son = root;
                continue;
            }
            son->fail = son->last = root; // 第一层的 fail 都指向根节点
            q.push(son);
        }

        // BFS
        while (!q.empty()) {
            Node* cur = q.front();
            q.pop();
            for (int i = 0; i < 26; i++) {
                auto& son = cur->son[i];
                if (son == nullptr) {
                    // 把虚拟子节点 cur.son[i] 设置为 cur.fail.son[i]
                    // 方便失配时直接跳到下一个可能匹配的位置（但不一定是某个模式串的末尾）
                    son = cur->fail->son[i];
                    continue;
                }
                son->fail = cur->fail->son[i]; // 计算失配位置
                // 沿着 last 往上走，可以直接跳到一定是某个模式串末尾的节点（如果跳到 root 表示匹配结束）
                son->last = son->fail->cnt ? son->fail : son->fail->last;
                q.push(son);
            }
        }
    }
};

class Solution {
public:
    int numOfStrings(vector<string>& patterns, string word) {
        AhoCorasick ac;
        for (auto& pattern : patterns) {
            ac.put(pattern);
        }
        ac.build_fail();

        Node* cur = ac.root;
        int ans = 0;
        for (char ch : word) {
            cur = cur->son[ch - 'a']; // 如果没有匹配，相当于移动到 fail 的 son[ch-'a']
            // 可能匹配更短的模式串，要继续在 last 链上找
            for (Node* match = cur; match->cnt >= 0; match = match->last) {
                ans += match->cnt;
                match->cnt = -1; // 避免重复统计
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type node struct {
	son  [26]*node
	fail *node // 当 node.son[i] 失配时，node.fail.son[i] 即为下一个待匹配节点（等于 root 则表示没有匹配）
	last *node // 后缀链接（suffix link），用来快速跳到一定是某个模式串末尾的节点（等于 root 则表示匹配结束）
	cnt  int   // node 是 cnt 个模式串的末尾
}

type acam struct {
	root *node
}

// 把模式串 pattern 插入 AC 自动机（代码和字典树一样）
func (ac *acam) put(pattern string) {
	cur := ac.root
	for _, ch := range pattern {
		ch -= 'a'
		if cur.son[ch] == nil {
			cur.son[ch] = &node{}
		}
		cur = cur.son[ch]
	}
	cur.cnt++
}

// BFS，构建 AC 自动机的 fail 和 last，方便快速查询
func (ac *acam) buildFail() {
	ac.root.fail = ac.root
	ac.root.last = ac.root

	q := []*node{}
	for i, son := range ac.root.son[:] {
		if son == nil {
			ac.root.son[i] = ac.root
			continue
		}
		son.fail = ac.root // 第一层的 fail 都指向根节点
		son.last = ac.root
		q = append(q, son)
	}

	// BFS
	for len(q) > 0 {
		cur := q[0]
		q = q[1:]
		for i, son := range cur.son[:] {
			if son == nil {
				// 把虚拟子节点 cur.son[i] 设置为 cur.fail.son[i]
				// 方便失配时直接跳到下一个可能匹配的位置（但不一定是某个模式串的末尾）
				cur.son[i] = cur.fail.son[i]
				continue
			}
			son.fail = cur.fail.son[i] // 计算失配位置
			if son.fail.cnt > 0 {
				son.last = son.fail
			} else {
				// 沿着 last 往上走，可以直接跳到一定是某个模式串末尾的节点（如果跳到 root 表示匹配结束）
				son.last = son.fail.last
			}
			q = append(q, son)
		}
	}
}

func numOfStrings(patterns []string, word string) (ans int) {
	ac := &acam{root: &node{}}
	for _, pattern := range patterns {
		ac.put(pattern)
	}
	ac.buildFail()

	cur := ac.root
	for _, ch := range word {
		cur = cur.son[ch-'a'] // 如果没有匹配，相当于移动到 fail 的 son[ch-'a']
		// 可能匹配更短的模式串，要继续在 last 链上找
		for match := cur; match.cnt >= 0; match = match.last {
			ans += match.cnt
			match.cnt = -1 // 避免重复统计
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L|\Sigma| + n)$，其中 $n$ 是 $\textit{word}$ 的长度，$L$ 是 $\textit{patterns}$ 中所有字符串的长度之和，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(L|\Sigma| + n)$。

## 专题训练

见下面字符串题单的「**七、AC 自动机**」。

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

本题来自 `七、AC 自动机`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、AC 自动机`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
