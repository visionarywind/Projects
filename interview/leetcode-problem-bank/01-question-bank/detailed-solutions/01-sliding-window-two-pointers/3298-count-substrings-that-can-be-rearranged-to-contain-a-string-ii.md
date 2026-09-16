# 3298. 统计重新排列后包含另一个字符串的子字符串数目 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-substrings-that-can-be-rearranged-to-contain-a-string-ii/
- 题目 slug：`count-substrings-that-can-be-rearranged-to-contain-a-string-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.2 越长越合法
- 难度分：1909
- 外部题解来源：https://leetcode.cn/problems/count-substrings-that-can-be-rearranged-to-contain-a-string-ii/solutions/2925828/on-hua-dong-chuang-kou-qiu-ge-shu-python-0x7a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 滑动窗口求个数（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/count-substrings-that-can-be-rearranged-to-contain-a-string-ii/solutions/2925828/on-hua-dong-chuang-kou-qiu-ge-shu-python-0x7a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-hua-dong-chuang-kou-qiu-ge-shu-python-0x7a`
- topic id：`2925828`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

下文把 $\textit{word}_1$ 和 $\textit{word}_2$ 简称为 $s$ 和 $t$。

由于子串可以重排，只要子串可以涵盖（见 76 题）字符串 $t$，那么子串就可以通过重排，使得 $t$ 是子串的前缀。

所以本题是 [76. 最小覆盖子串](https://leetcode.cn/problems/minimum-window-substring/) 的求个数版本，做法都是**滑动窗口**，请看 [我的题解](https://leetcode.cn/problems/minimum-window-substring/solutions/2713911/liang-chong-fang-fa-cong-o52mn-dao-omnfu-3ezz/)。

滑动窗口的内层循环结束时，右端点**固定**在 $\textit{right}$，左端点在 $0,1,2,\ldots,\textit{left}-1$ 的所有子串都是合法的，这一共有 $\textit{left}$ 个，把 $\textit{left}$ 加入答案。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1WRtDejEjD/) 第三+四题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def validSubstringCount(self, s: str, t: str) -> int:
        if len(s) < len(t):
            return 0

        # t 的字母出现次数与 s 的字母出现次数之差
        diff = defaultdict(int)  # 也可以用 Counter(t)，但是会慢很多
        for c in t:
            diff[c] += 1

        # 窗口内有 less 个字母的出现次数比 t 的少
        less = len(diff)

        ans = left = 0
        for c in s:
            diff[c] -= 1
            if diff[c] == 0:
                # c 移入窗口后，窗口内 c 的出现次数和 t 的一样
                less -= 1
            while less == 0:  # 窗口符合要求
                if diff[s[left]] == 0:
                    # s[left] 移出窗口之前，检查出现次数，
                    # 如果窗口内 s[left] 的出现次数和 t 的一样，
                    # 那么 s[left] 移出窗口后，窗口内 s[left] 的出现次数比 t 的少
                    less += 1
                diff[s[left]] += 1
                left += 1
            ans += left
        return ans
```

```java [sol-Java]
class Solution {
    public long validSubstringCount(String S, String T) {
        if (S.length() < T.length()) {
            return 0;
        }

        char[] s = S.toCharArray();
        char[] t = T.toCharArray();
        int[] diff = new int[26]; // t 的字母出现次数与 s 的字母出现次数之差
        for (char c : t) {
            diff[c - 'a']++;
        }

        // 统计窗口内有多少个字母的出现次数比 t 的少
        int less = 0;
        for (int d : diff) {
            if (d > 0) {
                less++;
            }
        }

        long ans = 0;
        int left = 0;
        for (char c : s) {
            diff[c - 'a']--;
            if (diff[c - 'a'] == 0) {
                // c 移入窗口后，窗口内 c 的出现次数和 t 的一样
                less--;
            }
            while (less == 0) { // 窗口符合要求
                char outChar = s[left++]; // 准备移出窗口的字母
                if (diff[outChar - 'a'] == 0) {
                    // outChar 移出窗口之前检查出现次数，
                    // 如果窗口内 outChar 的出现次数和 t 的一样，
                    // 那么 outChar 移出窗口后，窗口内 outChar 的出现次数比 t 的少
                    less++;
                }
                diff[outChar - 'a']++;
            }
            ans += left;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long validSubstringCount(string s, string t) {
        if (s.length() < t.length()) {
            return 0;
        }

        int diff[26]{}; // t 的字母出现次数与 s 的字母出现次数之差
        for (char c : t) {
            diff[c - 'a']++;
        }

        // 统计窗口内有多少个字母的出现次数比 t 的少
        int less = 0;
        for (int d : diff) {
            if (d > 0) {
                less++;
            }
        }

        long long ans = 0;
        int left = 0;
        for (char c : s) {
            diff[c - 'a']--;
            if (diff[c - 'a'] == 0) {
                // c 移入窗口后，窗口内 c 的出现次数和 t 的一样
                less--;
            }
            while (less == 0) { // 窗口符合要求
                char out_char = s[left++] - 'a'; // 准备移出窗口的字母
                if (diff[out_char] == 0) {
                    // out_char 移出窗口之前，检查出现次数，
                    // 如果窗口内 out_char 的出现次数和 t 的一样，
                    // 那么 out_char 移出窗口后，窗口内 out_char 的出现次数比 t 的少
                    less++;
                }
                diff[out_char]++;
            }
            ans += left;
        }
        return ans;
    }
};
```

```c [sol-C]
long long validSubstringCount(char* s, char* t) {
    int diff[26] = {}; // t 的字母出现次数与 s 的字母出现次数之差
    for (int i = 0; t[i]; i++) {
        diff[t[i] - 'a']++;
    }

    // 统计窗口内有多少个字母的出现次数比 t 的少
    int less = 0;
    for (int i = 0; i < 26; i++) {
        if (diff[i] > 0) {
            less++;
        }
    }

    long long ans = 0;
    int left = 0;
    for (int i = 0; s[i]; i++) {
        diff[s[i] - 'a']--;
        if (diff[s[i] - 'a'] == 0) {
            // s[i] 移入窗口后，窗口内 s[i] 的出现次数和 t 的一样
            less--;
        }
        while (less == 0) { // 窗口符合要求
            char out_char = s[left++] - 'a'; // 准备移出窗口的字母
            if (diff[out_char] == 0) {
                // out_char 移出窗口之前，检查出现次数，
                // 如果窗口内 out_char 的出现次数和 t 的一样，
                // 那么 out_char 移出窗口后，窗口内 out_char 的出现次数比 t 的少
                less++;
            }
            diff[out_char]++;
        }
        ans += left;
    }
    return ans;
}
```

```go [sol-Go]
func validSubstringCount(s, t string) (ans int64) {
	if len(s) < len(t) {
		return 0
	}

	diff := [26]int{} // t 的字母出现次数与 s 的字母出现次数之差
	for _, c := range t {
		diff[c-'a']++
	}

	// 统计窗口内有多少个字母的出现次数比 t 的少
	less := 0
	for _, d := range diff {
		if d > 0 {
			less++
		}
	}

	left := 0
	for _, c := range s {
		diff[c-'a']--
		if diff[c-'a'] == 0 {
			// c 移入窗口后，窗口内 c 的出现次数和 t 的一样
			less--
		}
		for less == 0 { // 窗口符合要求
			if diff[s[left]-'a'] == 0 {
                // s[left] 移出窗口之前，检查出现次数，
                // 如果窗口内 s[left] 的出现次数和 t 的一样，
                // 那么 s[left] 移出窗口后，窗口内 s[left] 的出现次数比 t 的少
				less++
			}
			diff[s[left]-'a']++
			left++
		}
		ans += int64(left)
	}
	return
}
```

```js [sol-JavaScript]
var validSubstringCount = function(s, t) {
    if (s.length < t.length) {
        return 0;
    }

    const diff = Array(26).fill(0); // t 的字母出现次数与 s 的字母出现次数之差
    for (const c of t) {
        diff[c.charCodeAt(0) - 'a'.charCodeAt(0)]++;
    }

    // 统计窗口内有多少个字母的出现次数比 t 的少
    let less = 0;
    for (const d of diff) {
        if (d > 0) {
            less++;
        }
    }

    let ans = 0;
    let left = 0;
    for (const ch of s) {
        const c = ch.charCodeAt(0) - 'a'.charCodeAt(0);
        diff[c]--;
        if (diff[c] === 0) {
            // c 移入窗口后，窗口内 c 的出现次数和 t 的一样
            less--;
        }
        while (less === 0) { // 窗口符合要求
            const outChar = s[left++].charCodeAt(0) - 'a'.charCodeAt(0);
            if (diff[outChar] === 0) {
                // outChar 移出窗口之前，检查出现次数，
                // 如果窗口内 outChar 的出现次数和 t 的一样，
                // 那么 outChar 移出窗口后，窗口内 outChar 的出现次数比 t 的少
                less++;
            }
            diff[outChar]++;
        }
        ans += left;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn valid_substring_count(s: String, t: String) -> i64 {
        if s.len() < t.len() {
            return 0;
        }

        let mut diff = vec![0; 26]; // t 的字母出现次数与 s 的字母出现次数之差
        for c in t.bytes() {
            diff[(c - b'a') as usize] += 1;
        }

        // 统计窗口内有多少个字母的出现次数比 t 的少
        let mut less = diff.iter().filter(|&&d| d > 0).count() as i32;

        let mut ans = 0;
        let mut left = 0;
        let s = s.as_bytes();
        for c in s {
            let c = (c - b'a') as usize;
            diff[c] -= 1;
            if diff[c] == 0 {
                // c 移入窗口后，窗口内 c 的出现次数和 t 的一样
                less -= 1;
            }
            while less == 0 { // 窗口符合要求
                let out_char = (s[left] - b'a') as usize; // 准备移出窗口的字母
                if diff[out_char] == 0 {
                    // out_char 移出窗口之前，检查出现次数，
                    // 如果窗口内 out_char 的出现次数和 t 的一样，
                    // 那么 out_char 移出窗口后，窗口内 out_char 的出现次数比 t 的少
                    less += 1;
                }
                diff[out_char] += 1;
                left += 1;
            }
            ans += left;
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+|\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|=26$ 是字符集合的大小。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $n$ 次，所以总的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

更多相似题目，见下面滑动窗口题单中的「**§2.3 求子数组个数**」，例如 [2962. 统计最大元素出现至少 K 次的子数组](https://leetcode.cn/problems/count-subarrays-where-max-element-appears-at-least-k-times/) 等。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.2 越长越合法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.2 越长越合法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
