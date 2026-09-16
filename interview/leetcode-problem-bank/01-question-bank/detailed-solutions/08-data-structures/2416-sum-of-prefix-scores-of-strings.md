# 2416. 字符串的前缀分数和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-prefix-scores-of-strings/
- 题目 slug：`sum-of-prefix-scores-of-strings`
- 来源专题：常用数据结构
- 来源分类路径：六、字典树（trie） / §6.1 基础
- 难度分：1725
- 外部题解来源：https://leetcode.cn/problems/sum-of-prefix-scores-of-strings/solutions/1831554/by-endlesscheng-ghfr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[字典树：两次遍历 / DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-prefix-scores-of-strings/solutions/1831554/by-endlesscheng-ghfr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-ghfr`
- topic id：`1831554`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1AP411p7pK) 已出炉，**包括本题字典树（trie）的原理**，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

根据题意，我们可以用字典树存储所有字符串，由于每个节点都是其子树节点的前缀，题干中的**分数**就是在字符串插入字典树的过程中，经过该节点的字符串个数，即以该节点为前缀的字符串的个数。

插入后，再次遍历每个字符串，在字典树上查找，累加路径上的分数之和就是答案。

```py [sol11-Python3]
class Solution:
    def sumPrefixScores(self, words: List[str]) -> List[int]:
        trie = lambda: defaultdict(trie)
        root = trie()
        for word in words:
            cur = root
            for c in word:
                cur = cur[c]
                cur[None] = cur.get(None, 0) + 1  # 更新所有前缀的分数

        ans = []
        for word in words:
            cur, sum = root, 0
            for c in word:
                cur = cur[c]
                sum += cur[None]  # 累加分数，即可得到答案
            ans.append(sum)
        return ans
```

```java [sol11-Java]
class Node {
    Node[] son = new Node[26];
    int score;
}

class Solution {
    public int[] sumPrefixScores(String[] words) {
        var root = new Node();
        for (var word : words) {
            var cur = root;
            for (var c : word.toCharArray()) {
                c -= 'a';
                if (cur.son[c] == null) cur.son[c] = new Node();
                cur = cur.son[c];
                ++cur.score; // 更新所有前缀的分数
            }
        }

        var n = words.length;
        var ans = new int[n];
        for (var i = 0; i < n; ++i) {
            var cur = root;
            for (var c : words[i].toCharArray()) {
                cur = cur.son[c - 'a'];
                ans[i] += cur.score; // 累加分数，即可得到答案
            }
        }
        return ans;
    }
}
```

```cpp [sol11-C++]
class Solution {
public:
    vector<int> sumPrefixScores(vector<string> &words) {
        struct Node {
            Node *son[26]{};
            int score = 0;
        };
        Node *root = new Node();
        for (auto &word : words) {
            auto cur = root;
            for (char c : word) {
                c -= 'a';
                if (cur->son[c] == nullptr) cur->son[c] = new Node();
                cur = cur->son[c];
                ++cur->score; // 更新所有前缀的分数
            }
        }

        int n = words.size();
        vector<int> ans(n);
        for (int i = 0; i < n; ++i) {
            auto cur = root;
            for (char c : words[i]) {
                cur = cur->son[c - 'a'];
                ans[i] += cur->score; // 累加分数，即可得到答案
            }
        }
        return ans;
    }
};
```

```go [sol11-Go]
func sumPrefixScores(words []string) []int {
    type node struct {
        son   [26]*node
        score int
    }
    root := &node{}
    for _, word := range words {
        cur := root
        for _, c := range word {
            c -= 'a'
            if cur.son[c] == nil {
                cur.son[c] = &node{}
            }
            cur = cur.son[c]
            cur.score++ // 更新所有前缀的分数
        }
    }

    ans := make([]int, len(words))
    for i, word := range words {
        cur := root
        for _, c := range word {
            cur = cur.son[c-'a']
            ans[i] += cur.score // 累加分数，即可得到答案
        }
    }
    return ans
}
```

另外一种写法是插入字符串后，DFS 这颗字典树，累加路径上的分数，就可以得到每个字符串的所有非空前缀的分数总和了。

代码实现时，由于可能有相同字符串，每个字符串对应的节点需要用一个列表存储该字符串在 $\textit{words}$ 中的所有下标。

```py [sol1-Python3]
class Node:
    __slots__ = 'son', 'ids', 'score'

    def __init__(self):
        self.son = defaultdict(Node)
        self.ids = []
        self.score = 0

class Solution:
    def sumPrefixScores(self, words: List[str]) -> List[int]:
        root = Node()
        for i, word in enumerate(words):
            cur = root
            for c in word:
                cur = cur.son[c]
                cur.score += 1  # 更新所有前缀的分数
            cur.ids.append(i)

        ans = [0] * len(words)
        def dfs(node: Node, sum: int) -> None:
            sum += node.score  # 累加分数，即可得到答案
            for i in node.ids:
                ans[i] = sum
            for child in node.son.values():
                if child:
                    dfs(child, sum)
        dfs(root, 0)
        return ans
```

```java [sol1-Java]
class Node {
    Node[] son = new Node[26];
    List<Integer> ids = new ArrayList<>();
    int score;
}

class Solution {
    int[] ans;

    private void dfs(Node node, int sum) {
        sum += node.score; // 累加分数，即可得到答案
        for (var i : node.ids)
            ans[i] += sum;
        for (var child : node.son)
            if (child != null)
                dfs(child, sum);
    }

    public int[] sumPrefixScores(String[] words) {
        var n = words.length;
        var root = new Node();
        for (int i = 0; i < n; i++) {
            var cur = root;
            for (var c : words[i].toCharArray()) {
                c -= 'a';
                if (cur.son[c] == null) cur.son[c] = new Node();
                cur = cur.son[c];
                ++cur.score; // 更新所有前缀的分数
            }
            cur.ids.add(i);
        }
        ans = new int[n];
        dfs(root, 0);
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
    struct Node {
        Node *son[26]{};
        vector<int> ids;
        int score = 0;
    };

    vector<int> ans;

    void dfs(Node *node, int sum) {
        sum += node->score; // 累加分数，即可得到答案
        for (int i : node->ids)
            ans[i] += sum;
        for (auto child : node->son)
            if (child != nullptr)
                dfs(child, sum);
    }

public:
    vector<int> sumPrefixScores(vector<string> &words) {
        int n = words.size();
        Node *root = new Node();
        for (int i = 0; i < n; ++i) {
            auto cur = root;
            for (char c : words[i]) {
                c -= 'a';
                if (cur->son[c] == nullptr) cur->son[c] = new Node();
                cur = cur->son[c];
                ++cur->score; // 更新所有前缀的分数
            }
            cur->ids.push_back(i);
        }
        ans.resize(n);
        dfs(root, 0);
        return ans;
    }
};
```

```go [sol1-Go]
func sumPrefixScores(words []string) []int {
	type node struct {
		son   [26]*node
		ids   []int
		score int
	}
	root := &node{}
	for i, word := range words {
		cur := root
		for _, c := range word {
			c -= 'a'
			if cur.son[c] == nil {
				cur.son[c] = &node{}
			}
			cur = cur.son[c]
			cur.score++ // 更新所有前缀的分数
		}
		cur.ids = append(cur.ids, i)
	}

	ans := make([]int, len(words))
	var dfs func(*node, int)
	dfs = func(node *node, sum int) {
		sum += node.score // 累加分数，即可得到答案
		for _, i := range node.ids {
			ans[i] = sum
		}
		for _, child := range node.son {
			if child != nil {
				dfs(child, sum)
			}
		}
	}
	dfs(root, 0)
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$O(L)$，其中 $L$ 为 $\textit{word}_i$ 的长度之和。
- 空间复杂度：$O(L)$。

## 本地原创解析

### 1. 题意重述

本题来自 `六、字典树（trie） / §6.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、字典树（trie） / §6.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
