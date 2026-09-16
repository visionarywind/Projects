# 2157. 字符串分组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/groups-of-strings/
- 题目 slug：`groups-of-strings`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.2 进阶
- 难度分：2499
- 外部题解来源：https://leetcode.cn/problems/groups-of-strings/solutions/1239405/bing-cha-ji-wei-yun-suan-by-endlesscheng-uejd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[哈希并查集 + 位运算（Python/Java/C++/Go）](https://leetcode.cn/problems/groups-of-strings/solutions/1239405/bing-cha-ji-wei-yun-suan-by-endlesscheng-uejd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bing-cha-ji-wei-yun-suan-by-endlesscheng-uejd`
- topic id：`1239405`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

由于字符串不含重复字母且只由小写字母组成，我们可以用一个二进制数来表示字符串，二进制数的第 $i$ 位为 $1$ 则表示第 $i$ 个小写字母出现在 $s$ 中。

枚举 $\textit{words}$ 中的字符串 $s$，并枚举 $s$ 通过添加、删除和替换操作得到的字符串 $t$，如果 $t$ 也在 $\textit{words}$ 中，则说明 $s$ 和 $t$ 可以分到同一组。我们可以用并查集来关联可以分到同一组的字符串。

遍历结束后，并查集中的集合个数就是 $\textit{words}$ 分组后的总组数，最大的集合即为字符串数目最多的组所包含的字符串数目。这可以在并查集合并的同时维护出来。

```py [sol-Python3]
class Solution:
    def groupStrings(self, words: List[str]) -> List[int]:
        # 并查集模板（哈希表写法）
        fa, size = {}, defaultdict(int)
        groups, max_size = len(words), 0
        def find(x: int) -> int:
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]
        def merge(x: int, y: int):
            nonlocal groups, max_size
            if y not in fa:
                return
            x, y = find(x), find(y)
            if x == y:
                return
            fa[x] = y
            size[y] += size[x]
            max_size = max(max_size, size[y])  # 维护答案
            groups -= 1

        for word in words:
            x = 0
            for ch in word:
                x |= 1 << (ord(ch) - ord('a'))  # 计算 word 的二进制表示
            fa[x] = x  # 添加至并查集
            size[x] += 1
            max_size = max(max_size, size[x])  # 维护答案
            if size[x] > 1:
                groups -= 1

        for x in fa:  # 枚举所有字符串（二进制表示）
            for i in range(26):
                merge(x, x ^ (1 << i))  # 添加或删除字符 i
                if (x >> i) & 1:
                    for j in range(26):
                        if ((x >> j) & 1) == 0:
                            merge(x, x ^ (1 << i) | (1 << j))  # 替换字符 i 为 j
        return [groups, max_size]
```

```java [sol-Java]
class Solution {
    // 并查集模板（哈希表写法）
    HashMap<Integer, Integer> fa = new HashMap<>();
    HashMap<Integer, Integer> size = new HashMap<>();
    int groups, maxSize;

    int find(int x) {
        if (fa.get(x) != x) {
            fa.put(x, find(fa.get(x)));
        }
        return fa.get(x);
    }

    void merge(int x, int y) {
        if (!fa.containsKey(y)) {
            return;
        }
        x = find(x);
        y = find(y);
        if (x == y) {
            return;
        }
        fa.put(x, y);
        size.put(y, size.get(y) + size.get(x));
        maxSize = Math.max(maxSize, size.get(y)); // 维护答案
        groups--;
    }

    public int[] groupStrings(String[] words) {
        groups = words.length;
        for (String word : words) {
            int x = 0;
            for (char c : word.toCharArray()) {
                x |= 1 << (c - 'a'); // 计算 word 的二进制表示
            }
            fa.put(x, x); // 添加至并查集
            size.put(x, size.getOrDefault(x, 0) + 1);
            maxSize = Math.max(maxSize, size.get(x)); // 维护答案
            if (size.get(x) > 1) {
                groups--;
            }
        }

        fa.forEach((x, fx) -> {
            for (int i = 0; i < 26; i++) {
                merge(x, x ^ (1 << i)); // 添加或删除字符 i
                if (((x >> i) & 1) == 1)
                    for (int j = 0; j < 26; j++)
                        if (((x >> j) & 1) == 0)
                            merge(x, x ^ (1 << i) | (1 << j)); // 替换字符 i 为 j
            }
        });
        return new int[]{groups, maxSize};
    }
}
```

```cpp [sol-C++]
class Solution {
    // 并查集模板（哈希表写法）
    unordered_map<int, int> fa, size;
    int groups, maxSize = 0;

    int find(int x) {
        return fa[x] != x ? fa[x] = find(fa[x]) : x;
    }

    void merge(int x, int y) {
        if (!fa.count(y)) return;
        x = find(x);
        y = find(y);
        if (x == y) return;
        fa[x] = y;
        size[y] += size[x];
        maxSize = max(maxSize, size[y]); // 维护答案
        --groups;
    }

public:
    vector<int> groupStrings(vector<string> &words) {
        groups = words.size();
        for (auto &word : words) {
            int x = 0;
            for (char ch : word)
                x |= 1 << (ch - 'a'); // 计算 word 的二进制表示
            fa[x] = x; // 添加至并查集
            ++size[x];
            maxSize = max(maxSize, size[x]); // 维护答案
            if (size[x] > 1) --groups;
        }

        for (auto &[x, _] : fa) { // 枚举所有字符串（二进制表示）
            for (int i = 0; i < 26; ++i) {
                merge(x, x ^ (1 << i)); // 添加或删除字符 i
                if ((x >> i) & 1) 
                    for (int j = 0; j < 26; ++j) 
                        if (((x >> j) & 1) == 0) 
                            merge(x, x ^ (1 << i) | (1 << j)); // 替换字符 i 为 j
            }
        }
        return {groups, maxSize};
    }
};
```

```go [sol-Go]
func groupStrings(words []string) (ans []int) {
	// 并查集模板（哈希表写法）
	fa := map[int]int{}
	size := map[int]int{}
	var find func(int) int
	find = func(x int) int {
		if fa[x] != x {
			fa[x] = find(fa[x])
		}
		return fa[x]
	}
	groups, maxSize := len(words), 0
	merge := func(x, y int) {
		if _, ok := fa[y]; !ok {
			return
		}
		x, y = find(x), find(y)
		if x == y {
			return
		}
		fa[x] = y
		size[y] += size[x]
		maxSize = max(maxSize, size[y]) // 维护答案
		groups--
	}

	for _, word := range words {
		x := 0
		for _, ch := range word {
			x |= 1 << (ch - 'a') // 计算 word 的二进制表示
		}
		fa[x] = x // 添加至并查集
		size[x]++
		maxSize = max(maxSize, size[x]) // 维护答案
		if size[x] > 1 {
			groups--
		}
	}

	for x := range fa { // 枚举所有字符串（二进制表示）
		for i := 0; i < 26; i++ {
			merge(x, x^1<<i) // 添加或删除字符 i
			if x>>i&1 == 1 {
				for j := 0; j < 26; j++ {
					if x>>j&1 == 0 {
						merge(x, x^1<<i|1<<j) // 替换字符 i 为 j
					}
				}
			}
		}
	}
	return []int{groups, maxSize}
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
