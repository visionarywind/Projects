# 3085. 成为 K 特殊字符串需要删除的最少字符数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-deletions-to-make-string-k-special/
- 题目 slug：`minimum-deletions-to-make-string-k-special`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.6 先枚举，再贪心
- 难度分：1765
- 外部题解来源：https://leetcode.cn/problems/minimum-deletions-to-make-string-k-special/solutions/2692077/kao-lu-zui-duo-bao-liu-duo-shao-ge-zi-mu-qttz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[先枚举，再贪心 + 滑动窗口优化（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-deletions-to-make-string-k-special/solutions/2692077/kao-lu-zui-duo-bao-liu-duo-shao-ge-zi-mu-qttz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kao-lu-zui-duo-bao-liu-duo-shao-ge-zi-mu-qttz`
- topic id：`2692077`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

看题目的三个示例：

- 示例 1 没有删除字母 $\texttt{b}$，且 $\texttt{b}$ 是删除后出现次数最少的字母（之一）。
- 示例 2 没有删除字母 $\texttt{b}$，且 $\texttt{b}$ 是删除后出现次数最少的字母。
- 示例 3 没有删除字母 $\texttt{a}$，且 $\texttt{a}$ 是删除后出现次数最少的字母。

**猜想**：删除字母后，在所有出现次数最少的字母中，一定存在没被删除过的字母。

**证明**：反证法，假设所有出现次数最少的字母，都至少被删除一个。我们把这些字母的出现次数都增加一，即出现次数的最小值加一，仍然满足题目「出现次数绝对差 $\le k$」的要求，但总删除次数更少了，矛盾。故猜想成立。

统计 $\textit{word}$ 每种字母的出现次数，记到数组（或者哈希表）$\textit{cnt}$ 中。

枚举 $\textit{cnt}[i]$ 作为删除字母后，出现次数的最小值。

删除后，字母的出现次数有三种情况：

1. 如果出现次数小于 $\textit{cnt}[i]$，全删，出现次数变成 $0$。这样的字母不参与「出现次数绝对差 $\le k$」的计算。
2. 如果出现次数在闭区间 $[\textit{cnt}[i],\textit{cnt}[i]+k]$ 中，无需删除，出现次数不变。
3. 如果出现次数超过 $\textit{cnt}[i]+k$，需要减小至 $\textit{cnt}[i]+k$。

换句话说：

- 出现次数 $< \textit{cnt}[i]$ 的字母，全部删除。
- 出现次数 $\ge \textit{cnt}[i]$ 的字母 $j$，保留 $\min(\textit{cnt}[j], \textit{cnt}[i] + k)$ 个。累加保留的字母个数，取最大值，得到最多保留的字母个数 $\textit{maxSave}$。

最后，用 $\textit{word}$ 的长度，减去 $\textit{maxSave}$，即为需要删除的字母个数。

代码实现时，为方便计算哪些字母的出现次数小于 $\textit{cnt}[i]$，哪些大于等于 $\textit{cnt}[i]$，把 $\textit{cnt}$ 排序。

[本题视频讲解](https://www.bilibili.com/video/BV1RH4y1W7DP/?t=5m37s)

## 优化前

```py [sol-Python3]
class Solution:
    def minimumDeletions(self, word: str, k: int) -> int:
        cnt = sorted(Counter(word).values())
        max_save = 0
        for i, base in enumerate(cnt):
            s = sum(min(c, base + k) for c in cnt[i:])  # 至多保留 base+k 个
            max_save = max(max_save, s)
        return len(word) - max_save
```

```py [sol-Python3 写法二]
class Solution:
    def minimumDeletions(self, word: str, k: int) -> int:
        cnt = sorted(Counter(word).values())
        max_save = max(sum(min(c, base + k) for c in cnt[i:])
                       for i, base in enumerate(cnt))
        return len(word) - max_save
```

```java [sol-Java]
class Solution {
    public int minimumDeletions(String word, int k) {
        int[] cnt = new int[26];
        for (char c : word.toCharArray()) {
            cnt[c - 'a']++;
        }
        Arrays.sort(cnt);

        int maxSave = 0;
        for (int i = 0; i < 26; i++) {
            int sum = 0;
            for (int j = i; j < 26; j++) {
                sum += Math.min(cnt[j], cnt[i] + k); // 至多保留 cnt[i]+k 个
            }
            maxSave = Math.max(maxSave, sum);
        }

        return word.length() - maxSave;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumDeletions(string word, int k) {
        int cnt[26]{};
        for (char c: word) {
            cnt[c - 'a']++;
        }
        ranges::sort(cnt);

        int max_save = 0;
        for (int i = 0; i < 26; i++) {
            int sum = 0;
            for (int j = i; j < 26; j++) {
                sum += min(cnt[j], cnt[i] + k); // 至多保留 cnt[i]+k 个
            }
            max_save = max(max_save, sum);
        }

        return word.size() - max_save;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int minimumDeletions(char* word, int k) {
    int cnt[26] = {};
    int n = 0;
    for (; word[n]; n++) {
        cnt[word[n] - 'a']++;
    }
    qsort(cnt, 26, sizeof(int), cmp);

    int max_save = 0;
    for (int i = 0; i < 26; i++) {
        int sum = 0;
        for (int j = i; j < 26; j++) {
            sum += MIN(cnt[j], cnt[i] + k); // 至多保留 cnt[i]+k 个
        }
        max_save = MAX(max_save, sum);
    }

    return n - max_save;
}
```

```go [sol-Go]
func minimumDeletions(word string, k int) int {
	cnt := make([]int, 26)
	for _, b := range word {
		cnt[b-'a']++
	}
	slices.Sort(cnt)

	maxSave := 0
	for i, base := range cnt {
		sum := 0
		for _, c := range cnt[i:] {
			sum += min(c, base+k) // 至多保留 base+k 个
		}
		maxSave = max(maxSave, sum)
	}

	return len(word) - maxSave
}
```

```js [sol-JavaScript]
var minimumDeletions = function(word, k) {
    const cnt = Array(26).fill(0);
    for (const c of word) {
        cnt[c.charCodeAt(0) - 97]++;
    }
    cnt.sort((a, b) => a - b);

    let maxSave = 0;
    for (let i = 0; i < 26; i++) {
        let sum = 0;
        for (let j = i; j < 26; j++) {
            sum += Math.min(cnt[j], cnt[i] + k); // 至多保留 cnt[i]+k 个
        }
        maxSave = Math.max(maxSave, sum);
    }

    return word.length - maxSave;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_deletions(word: String, k: i32) -> i32 {
        let mut cnt = [0; 26];
        for c in word.bytes() {
            cnt[(c - b'a') as usize] += 1;
        }
        cnt.sort_unstable();

        let mut max_save = 0;
        for (i, &base) in cnt.iter().enumerate() {
            let mut sum = 0;
            for &c in &cnt[i..] {
                sum += c.min(base + k); // 至多保留 base+k 个
            }
            max_save = max_save.max(sum);
        }

        word.len() as i32 - max_save
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + |\Sigma|^2)$，其中 $n$ 为 $\textit{word}$ 的长度，$|\Sigma|=26$ 为字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

## 优化

如果把 $\textit{word}$ 改成整数数组 $\textit{nums}$，值域范围大小 $|\Sigma|=10^5$，上面的做法就会超时。

时间复杂度更低的做法是，用一个 [滑动窗口](https://www.bilibili.com/video/BV1hd4y1r7Gq/) 维护出现次数不变的字母，即出现次数在闭区间 $[\textit{cnt}[i],\textit{cnt}[i]+k]$ 中的字母。由于 $\textit{cnt}[i]$ 越大，区间右端点也越大，所以可以滑动窗口。

维护窗口中的字母出现次数之和，记在变量 $s$ 中。

滑动窗口内层循环结束后，下标在 $[\textit{right},|\textit{cnt}|-1]$ 中的字母出现次数都要减小为 $\textit{cnt}[i]+k$，出现次数之和为

$$
(\textit{cnt}[i]+k)(|\textit{cnt}| - \textit{right})
$$

保留下来的字母总个数为

$$
s + (\textit{cnt}[i]+k)(|\textit{cnt}| - \textit{right})
$$

```py [sol-Python3]
class Solution:
    def minimumDeletions(self, word: str, k: int) -> int:
        cnt = sorted(Counter(word).values())
        max_save = s = right = 0
        for base in cnt:
            mx = base + k
            while right < len(cnt) and cnt[right] <= mx:
                s += cnt[right]
                right += 1
            # 现在 s 表示出现次数不变的字母个数之和
            # 再加上出现次数减少为 mx 的 len(cnt)-right 种字母，即为保留的字母总数
            max_save = max(max_save, s + mx * (len(cnt) - right))
            # 下一轮循环 base 全删
            s -= base
        return len(word) - max_save
```

```java [sol-Java]
class Solution {
    public int minimumDeletions(String word, int k) {
        final int SIGMA = 26;
        int[] cnt = new int[SIGMA];
        for (char c : word.toCharArray()) {
            cnt[c - 'a']++;
        }
        Arrays.sort(cnt);

        int maxSave = 0;
        int s = 0;
        int right = 0;
        for (int base : cnt) {
            while (right < SIGMA && cnt[right] <= base + k) {
                s += cnt[right];
                right++;
            }
            // 现在 s 表示出现次数不变的字母个数之和
            // 再加上出现次数减少为 base+k 的 SIGMA-right 种字母，即为保留的字母总数
            maxSave = Math.max(maxSave, s + (base + k) * (SIGMA - right));
            // 下一轮循环 base 全删
            s -= base;
        }

        return word.length() - maxSave;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumDeletions(string word, int k) {
        const int SIGMA = 26;
        int cnt[SIGMA]{};
        for (char c: word) {
            cnt[c - 'a']++;
        }
        ranges::sort(cnt);

        int max_save = 0, s = 0, right = 0;
        for (int base : cnt) {
            while (right < SIGMA && cnt[right] <= base + k) {
                s += cnt[right];
                right++;
            }
            // 现在 s 表示出现次数不变的字母个数之和
            // 再加上出现次数减少为 base+k 的 SIGMA-right 种字母，即为保留的字母总数
            max_save = max(max_save, s + (base + k) * (SIGMA - right));
            // 下一轮循环 base 全删
            s -= base;
        }

        return word.size() - max_save;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int minimumDeletions(char* word, int k) {
    #define SIGMA 26
    int cnt[SIGMA] = {};
    int n = 0;
    for (; word[n]; n++) {
        cnt[word[n] - 'a']++;
    }
    qsort(cnt, SIGMA, sizeof(int), cmp);

    int max_save = 0, s = 0, right = 0;
    for (int i = 0; i < SIGMA; i++) {
        int base = cnt[i];
        while (right < SIGMA && cnt[right] <= base + k) {
            s += cnt[right];
            right++;
        }
        // 现在 s 表示出现次数不变的字母个数之和
        // 再加上出现次数减少为 base+k 的 SIGMA-right 种字母，即为保留的字母总数
        max_save = MAX(max_save, s + (base + k) * (SIGMA - right));
        // 下一轮循环 base 全删
        s -= base;
    }

    return n - max_save;
}
```

```go [sol-Go]
func minimumDeletions(word string, k int) int {
	const sigma = 26
	cnt := [sigma]int{}
	for _, b := range word {
		cnt[b-'a']++
	}
	slices.Sort(cnt[:])

	var maxSave, s, right int
	for _, base := range cnt {
		for right < sigma && cnt[right] <= base+k {
			s += cnt[right]
			right++
		}
		// 现在 s 表示出现次数不变的字母个数之和
		// 再加上出现次数减少为 base+k 的 sigma-right 种字母，即为保留的字母总数
		maxSave = max(maxSave, s+(base+k)*(sigma-right))
		// 下一轮循环 base 全删
		s -= base
	}
	return len(word) - maxSave
}
```

```js [sol-JavaScript]
var minimumDeletions = function(word, k) {
    const SIGMA = 26;
    const cnt = Array(SIGMA).fill(0);
    for (const c of word) {
        cnt[c.charCodeAt(0) - 97]++;
    }
    cnt.sort((a, b) => a - b);

    let maxSave = 0, s = 0, right = 0;
    for (const base of cnt) {
        while (right < SIGMA && cnt[right] <= base + k) {
            s += cnt[right];
            right++;
        }
        // 现在 s 表示出现次数不变的字母个数之和
        // 再加上出现次数减少为 base+k 的 SIGMA-right 种字母，即为保留的字母总数
        maxSave = Math.max(maxSave, s + (base + k) * (SIGMA - right));
        // 下一轮循环 base 全删
        s -= base;
    }

    return word.length - maxSave;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_deletions(word: String, k: i32) -> i32 {
        let mut cnt = [0; 26];
        for c in word.bytes() {
            cnt[(c - b'a') as usize] += 1;
        }
        cnt.sort_unstable();

        let mut max_save = 0;
        let mut s = 0;
        let mut right = 0;
        for &base in &cnt {
            while right < cnt.len() && cnt[right] <= base + k {
                s += cnt[right];
                right += 1;
            }
            // 现在 s 表示出现次数不变的字母个数之和
            // 再加上出现次数减少为 base+k 的 cnt.len()-right 种字母，即为保留的字母总数
            max_save = max_save.max(s + (base + k) * (cnt.len() - right) as i32);
            // 下一轮循环 base 全删
            s -= base;
        }

        word.len() as i32 - max_save
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + |\Sigma|\log |\Sigma|)$，其中 $n$ 为 $\textit{word}$ 的长度，$|\Sigma|=26$ 为字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

## 相似题目

[2171. 拿出最少数目的魔法豆](https://leetcode.cn/problems/removing-minimum-number-of-magic-beans/)

更多相似题目，见贪心题单中的「**§1.6 先枚举，再贪心**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.6 先枚举，再贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.6 先枚举，再贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
