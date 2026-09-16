# 1461. 检查一个字符串是否包含所有长度为 K 的二进制子串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-a-string-contains-all-binary-codes-of-size-k/
- 题目 slug：`check-if-a-string-contains-all-binary-codes-of-size-k`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：1504
- 外部题解来源：https://leetcode.cn/problems/check-if-a-string-contains-all-binary-codes-of-size-k/solutions/3902489/liang-chong-fang-fa-bao-li-wei-yun-suan-4wnph/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力 / 位运算（Python/Java/C++/Go）](https://leetcode.cn/problems/check-if-a-string-contains-all-binary-codes-of-size-k/solutions/3902489/liang-chong-fang-fa-bao-li-wei-yun-suan-4wnph/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-bao-li-wei-yun-suan-4wnph`
- topic id：`3902489`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴力

暴力枚举所有长为 $k$ 的子串，保存到一个哈希集合中。

如果最终哈希集合的大小恰好等于 $2^k$，那么说明**所有**长为 $k$ 的二进制串都在 $s$ 中。

```py [sol-Python3]
class Solution:
    def hasAllCodes(self, s: str, k: int) -> bool:
        st = {s[i - k: i] for i in range(k, len(s) + 1)}
        return len(st) == 1 << k
```

```java [sol-Java]
class Solution {
    public boolean hasAllCodes(String s, int k) {
        Set<String> set = new HashSet<>();
        for (int i = k; i <= s.length(); i++) {
            set.add(s.substring(i - k, i));
        }
        return set.size() == (1 << k);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool hasAllCodes(string s, int k) {
        unordered_set<string> st;
        for (int i = k; i <= s.size(); i++) {
            st.insert(s.substr(i - k, k));
        }
        return st.size() == (1 << k);
    }
};
```

```go [sol-Go]
func hasAllCodes(s string, k int) bool {
	set := map[string]struct{}{}
	for i := k; i <= len(s); i++ {
		set[s[i-k:i]] = struct{}{}
	}
	return len(set) == 1<<k
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n-k)k)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}((n-k)k)$。

## 方法二：位运算滑窗

把子串转成整数，保存到哈希集合或者布尔数组中。

**小优化**：如果循环过程中发现已经找到 $2^k$ 个不同的二进制数，可以提前返回 $\texttt{true}$。

```py [sol-Python3]
class Solution:
    def hasAllCodes(self, s: str, k: int) -> bool:
        MASK = (1 << k) - 1
        st = set()  # 更快的写法见另一份代码【Python3 列表】
        x = 0
        for i, ch in enumerate(s):
            # 把 ch 加到 x 的末尾：x 整体左移一位，然后或上 ch
            # &MASK 目的是去掉超出 k 的比特位
            x = (x << 1 & MASK) | int(ch)
            if i >= k - 1:
                st.add(x)
        return len(st) == 1 << k
```

```py [sol-Python3 列表]
class Solution:
    def hasAllCodes(self, s: str, k: int) -> bool:
        MASK = (1 << k) - 1
        has = [False] * (1 << k)
        cnt = x = 0
        for i, ch in enumerate(s):
            # 把 ch 加到 x 的末尾：x 整体左移一位，然后或上 ch
            # &MASK 目的是去掉超出 k 的比特位
            x = (x << 1 & MASK) | int(ch)
            if i < k - 1 or has[x]:
                continue
            has[x] = True
            cnt += 1
            if cnt == 1 << k:
                return True
        return False
```

```java [sol-Java]
class Solution {
    public boolean hasAllCodes(String s, int k) {
        final int MASK = (1 << k) - 1;
        boolean[] has = new boolean[1 << k];
        int cnt = 0;
        int x = 0;
        for (int i = 0; i < s.length() && cnt < (1 << k); i++) {
            char ch = s.charAt(i);
            // 把 ch 加到 x 的末尾：x 整体左移一位，然后或上 ch&1
            // &MASK 目的是去掉超出 k 的比特位
            x = (x << 1 & MASK) | (ch & 1);
            if (i >= k - 1 && !has[x]) {
                has[x] = true;
                cnt++;
            }
        }
        return cnt == (1 << k);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool hasAllCodes(string s, int k) {
        const int MASK = (1 << k) - 1;
        vector<int8_t> has(1 << k);
        int cnt = 0;
        int x = 0;
        for (int i = 0; i < s.size() && cnt < (1 << k); i++) {
            // 把 s[i] 加到 x 的末尾：x 整体左移一位，然后或上 s[i]&1
            // &MASK 目的是去掉超出 k 的比特位
            x = (x << 1 & MASK) | (s[i] & 1);
            if (i >= k - 1 && !has[x]) {
                has[x] = true;
                cnt++;
            }
        }
        return cnt == (1 << k);
    }
};
```

```go [sol-Go]
func hasAllCodes(s string, k int) bool {
	has := make([]bool, 1<<k)
	cnt := 0
	mask := 1<<k - 1
	x := 0
	for i, ch := range s {
		// 把 ch 加到 x 的末尾：x 整体左移一位，然后或上 ch&1
		// &mask 目的是去掉超出 k 的比特位
		x = x<<1&mask | int(ch&1)
		if i < k-1 || has[x] {
			continue
		}
		has[x] = true
		cnt++
		if cnt == 1<<k {
			return true
		}
	}
	return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n-k)$ 或 $\mathcal{O}(2^k)$，取决于实现。

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

本题来自 `一、定长滑动窗口 / §1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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
    int maxVowels(string s, int k) {
        auto isVowel = [](char c) {
            return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
        };
        int cur = 0, ans = 0;
        for (int i = 0; i < (int)s.size(); ++i) {
            cur += isVowel(s[i]);
            if (i >= k) cur -= isVowel(s[i - k]);
            if (i + 1 >= k) ans = max(ans, cur);
        }
        return ans;
    }
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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
