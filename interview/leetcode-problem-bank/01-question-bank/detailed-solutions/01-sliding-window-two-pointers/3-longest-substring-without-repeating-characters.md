# 3. 无重复字符的最长子串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-substring-without-repeating-characters/
- 题目 slug：`longest-substring-without-repeating-characters`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/longest-substring-without-repeating-characters/solutions/1959540/xia-biao-zong-suan-cuo-qing-kan-zhe-by-e-iaks/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】一个视频讲透滑动窗口！附题单！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/longest-substring-without-repeating-characters/solutions/1959540/xia-biao-zong-suan-cuo-qing-kan-zhe-by-e-iaks/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xia-biao-zong-suan-cuo-qing-kan-zhe-by-e-iaks`
- topic id：`1959540`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 视频讲解

请看 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)，制作不易，欢迎点赞关注~

## 写法一：哈希表（整型数组）

```py [sol-Python3]
class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        ans = left = 0
        cnt = defaultdict(int)  # 维护从下标 left 到下标 right 的字符及其出现次数
        for right, c in enumerate(s):
            cnt[c] += 1
            while cnt[c] > 1:  # 窗口内有重复字母
                cnt[s[left]] -= 1  # 移除窗口左端点字母
                left += 1  # 缩小窗口
            ans = max(ans, right - left + 1)  # 更新窗口长度最大值
        return ans
```

```java [sol-Java]
class Solution {
    public int lengthOfLongestSubstring(String S) {
        char[] s = S.toCharArray(); // 转换成 char[] 加快效率（忽略带来的空间消耗）
        int n = s.length;
        int ans = 0;
        int left = 0;
        int[] cnt = new int[128]; // 也可以用 HashMap<Character, Integer>，这里为了效率用的数组
        for (int right = 0; right < n; right++) {
            char c = s[right];
            cnt[c]++;
            while (cnt[c] > 1) { // 窗口内有重复字母
                cnt[s[left]]--; // 移除窗口左端点字母
                left++; // 缩小窗口
            }
            ans = Math.max(ans, right - left + 1); // 更新窗口长度最大值
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int n = s.length(), ans = 0, left = 0;
        unordered_map<char, int> cnt; // 维护从下标 left 到下标 right 的字符
        for (int right = 0; right < n; right++) {
            char c = s[right];
            cnt[c]++;
            while (cnt[c] > 1) { // 窗口内有重复字母
                cnt[s[left]]--; // 移除窗口左端点字母
                left++; // 缩小窗口
            }
            ans = max(ans, right - left + 1); // 更新窗口长度最大值
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int lengthOfLongestSubstring(char* s) {
    int ans = 0, left = 0;
    int cnt[128] = {}; // 也可以用哈希表，这里为了效率用的数组
    for (int right = 0; s[right]; right++) {
        char c = s[right];
        cnt[c]++;
        while (cnt[c] > 1) { // 窗口内有重复字母
            cnt[s[left]]--; // 移除窗口左端点字母
            left++; // 缩小窗口
        }
        ans = MAX(ans, right - left + 1); // 更新窗口长度最大值
    }
    return ans;
}
```

```go [sol-Go]
func lengthOfLongestSubstring(s string) (ans int) {
    cnt := [128]int{} // 也可以用 map，这里为了效率用的数组
    left := 0
    for right, c := range s {
        cnt[c]++
        for cnt[c] > 1 { // 窗口内有重复字母
            cnt[s[left]]-- // 移除窗口左端点字母
            left++ // 缩小窗口
        }
        ans = max(ans, right-left+1) // 更新窗口长度最大值
    }
    return
}
```

```js [sol-JavaScript]
var lengthOfLongestSubstring = function(s) {
    let ans = 0;
    let left = 0;
    const cnt = new Map(); // 维护从下标 left 到下标 right 的字符
    for (let right = 0; right < s.length; right++) {
        const c = s[right];
        cnt.set(c, (cnt.get(c) ?? 0) + 1);
        while (cnt.get(c) > 1) { // 窗口内有重复字母
            cnt.set(s[left], cnt.get(s[left]) - 1); // 移除窗口左端点字母
            left++; // 缩小窗口
        }
        ans = Math.max(ans, right - left + 1); // 更新窗口长度最大值
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn length_of_longest_substring(s: String) -> i32 {
        let s = s.as_bytes();
        let mut ans = 0;
        let mut left = 0;
        let mut cnt = [0; 128]; // 也可以用哈希表，这里为了效率用的数组
        for (right, &c) in s.iter().enumerate() {
            let c = c as usize;
            cnt[c] += 1;
            while cnt[c] > 1 { // 窗口内有重复字母
                cnt[s[left] as usize] -= 1; // 移除窗口左端点字母
                left += 1; // 缩小窗口
            }
            ans = ans.max(right - left + 1); // 更新窗口长度最大值
        }
        ans as _
    }
}
```

## 写法二：哈希集合（布尔数组）

```py [sol-Python3]
class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        ans = left = 0
        window = set()  # 维护从下标 left 到下标 right 的字符
        for right, c in enumerate(s):
            # 如果窗口内已经包含 c，那么再加入一个 c 会导致窗口内有重复元素
            # 所以要在加入 c 之前，先移出窗口内的 c
            while c in window:  # 窗口内有 c
                window.remove(s[left])
                left += 1  # 缩小窗口
            window.add(c)  # 加入 c
            ans = max(ans, right - left + 1)  # 更新窗口长度最大值
        return ans
```

```java [sol-Java]
class Solution {
    public int lengthOfLongestSubstring(String S) {
        char[] s = S.toCharArray(); // 转换成 char[] 加快效率（忽略带来的空间消耗）
        int n = s.length;
        int ans = 0;
        int left = 0;
        boolean[] has = new boolean[128]; // 也可以用 HashSet<Character>，这里为了效率用的数组
        for (int right = 0; right < n; right++) {
            char c = s[right];
            // 如果窗口内已经包含 c，那么再加入一个 c 会导致窗口内有重复元素
            // 所以要在加入 c 之前，先移出窗口内的 c
            while (has[c]) { // 窗口内有 c
                has[s[left]] = false;
                left++; // 缩小窗口
            }
            has[c] = true; // 加入 c
            ans = Math.max(ans, right - left + 1); // 更新窗口长度最大值
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        int n = s.length(), ans = 0, left = 0;
        unordered_set<char> window; // 维护从下标 left 到下标 right 的字符
        for (int right = 0; right < n; right++) {
            char c = s[right];
            // 如果窗口内已经包含 c，那么再加入一个 c 会导致窗口内有重复元素
            // 所以要在加入 c 之前，先移出窗口内的 c
            while (window.contains(c)) { // 窗口内有 c
                window.erase(s[left]);
                left++; // 缩小窗口
            }
            window.insert(c); // 加入 c
            ans = max(ans, right - left + 1); // 更新窗口长度最大值
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int lengthOfLongestSubstring(char* s) {
    int ans = 0, left = 0;
    bool has[128] = {}; // 也可以用哈希集合，这里为了效率用的数组
    for (int right = 0; s[right]; right++) {
        char c = s[right];
        // 如果窗口内已经包含 c，那么再加入一个 c 会导致窗口内有重复元素
        // 所以要在加入 c 之前，先移出窗口内的 c
        while (has[c]) { // 窗口内有 c
            has[s[left]] = false;
            left++; // 缩小窗口
        }
        has[c] = true; // 加入 c
        ans = MAX(ans, right - left + 1); // 更新窗口长度最大值
    }
    return ans;
}
```

```go [sol-Go]
func lengthOfLongestSubstring(s string) (ans int) {
    window := [128]bool{} // 也可以用 map，这里为了效率用的数组
    left := 0
    for right, c := range s {
        // 如果窗口内已经包含 c，那么再加入一个 c 会导致窗口内有重复元素
        // 所以要在加入 c 之前，先移出窗口内的 c
        for window[c] { // 窗口内有 c
            window[s[left]] = false
            left++ // 缩小窗口
        }
        window[c] = true // 加入 c
        ans = max(ans, right-left+1) // 更新窗口长度最大值
    }
    return
}
```

```js [sol-JavaScript]
var lengthOfLongestSubstring = function(s) {
    let ans = 0;
    let left = 0;
    const window = new Set(); // 维护从下标 left 到下标 right 的字符
    for (let right = 0; right < s.length; right++) {
        const c = s[right];
        // 如果窗口内已经包含 c，那么再加入一个 c 会导致窗口内有重复元素
        // 所以要在加入 c 之前，先移出窗口内的 c
        while (window.has(c)) { // 窗口内有 c
            window.delete(s[left]);
            left++; // 缩小窗口
        }
        window.add(c); // 加入 c
        ans = Math.max(ans, right - left + 1); // 更新窗口长度最大值
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn length_of_longest_substring(s: String) -> i32 {
        let s = s.as_bytes();
        let mut ans = 0;
        let mut left = 0;
        let mut window = [false; 128]; // 也可以用哈希集合，这里为了效率用的数组
        for (right, &c) in s.iter().enumerate() {
            let c = c as usize;
            // 如果窗口内已经包含 c，那么再加入一个 c 会导致窗口内有重复元素
            // 所以要在加入 c 之前，先移出窗口内的 c
            while window[c] { // 窗口内有 c
                window[s[left] as usize] = false;
                left += 1; // 缩小窗口
            }
            window[c] = true; // 加入 c
            ans = ans.max(right - left + 1); // 更新窗口长度最大值
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。注意 $\textit{left}$ 至多增加 $n$ 次，所以整个二重循环至多循环 $\mathcal{O}(n)$ 次。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$，其中 $|\Sigma|$ 为字符集合的大小，本题中字符均为 ASCII 字符，所以 $|\Sigma|\le 128$。

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

本题来自 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
