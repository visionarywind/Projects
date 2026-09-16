# 30. 串联所有单词的子串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/substring-with-concatenation-of-all-words/
- 题目 slug：`substring-with-concatenation-of-all-words`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/substring-with-concatenation-of-all-words/solutions/3691292/30-ci-ding-chang-hua-dong-chuang-kou-pyt-5vgx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[30 次定长滑动窗口（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/substring-with-concatenation-of-all-words/solutions/3691292/30-ci-ding-chang-hua-dong-chuang-kou-pyt-5vgx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`30-ci-ding-chang-hua-dong-chuang-kou-pyt-5vgx`
- topic id：`3691292`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置题目

首先，你需要完成一些简单的**定长滑动窗口**题目，比如 [1456. 定长子串中元音的最大数目](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/)，详细讲解见[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)

然后完成 [567. 字符串的排列](https://leetcode.cn/problems/permutation-in-string/)，理解 [我的题解](https://leetcode.cn/problems/permutation-in-string/solutions/3747426/ding-chang-hua-dong-chuang-kou-cong-o26n-ulxp/) 中的优化技巧。

## 题意解读

设一个单词的长度为 $\textit{wordLen}$，由于题目保证 $\textit{words}$ 中所有单词长度都相同，所有单词长度之和（窗口长度）$\textit{windowLen} = \textit{wordLen}\cdot m$，其中 $m$ 是 $\textit{words}$ 的长度。

示例 1 的 $\textit{wordLen}=3$，窗口长度 $\textit{windowLen}=3\cdot 2=6$。

我们要在 $s$ 中找到一个长为 $\textit{windowLen}$ 的子串 $t$，满足：把 $t$ 均分成 $m$ 个长为 $\textit{wordLen}$ 的单词，统计每个单词的出现次数 $\textit{cnt}$（这是一个哈希表），以及 $\textit{words}$ 中每个单词的出现次数 $\textit{targetCnt}$（这也是一个哈希表），满足 $\textit{cnt} = \textit{targetCnt}$。

示例 1 的第一个长为 $6$ 的子串 $\texttt{barfoo} = \texttt{bar} + \texttt{foo}$，这两个单词的出现次数与 $\textit{words}$ 中每个单词的出现次数都一样，符合要求。（注意 $\textit{words}$ 中可能有重复的单词）

另一个子串 $\texttt{foobar} = \texttt{foo} + \texttt{bar}$，也符合要求。

把符合要求的子串左端点加入答案列表。

## 核心思路

示例 1 的 $s=\texttt{barfoothefoobarman}$，从第一个子串 $\texttt{barfoo}$ 到另一个子串 $\texttt{foothe}$，少了一个 $\texttt{bar}$，多了一个 $\texttt{the}$，中间的 $\texttt{foo}$ 是不变的，不需要重复统计。这种场景非常适合**定长滑动窗口**算法。

我们可以枚举第一个单词首字母是 $s[0]=\texttt{b}$ 还是 $s[1]=\texttt{a}$ 还是 $s[2]=\texttt{r}$。只需要枚举这 $3$ 个，因为 $s[3]=\texttt{f}$ 的情况在 $s[0]=\texttt{b}$ 的定长滑动窗口中计算了，其余 $s[4],s[5],\ldots$ 同理。

假设从 $i=0$ 开始滑窗，示例 1 依次遍历到的单词下标区间为 $[0,3),[3,6),[6,9),[9,12),[12,15),[15,18)$，把每个单词看成一个元素，我们相当于在如下列表中跑定长滑动窗口：

$$
[\texttt{bar},\texttt{foo},\texttt{the},\texttt{foo},\texttt{bar},\texttt{man}]
$$

一般地，我们需要跑 $\textit{wordLen}$ 次起点不同的定长滑动窗口，窗口左端点分别为：

- $0,\textit{wordLen},2\cdot \textit{wordLen},3\cdot \textit{wordLen},\ldots$
- $1,1+\textit{wordLen},1+2\cdot \textit{wordLen},1+3\cdot \textit{wordLen},\ldots$
- $2,2+\textit{wordLen},2+2\cdot \textit{wordLen},2+3\cdot \textit{wordLen},\ldots$
- ……
- $\textit{wordLen}-1,2\cdot\textit{wordLen}-1,3\cdot\textit{wordLen}-1,4\cdot\textit{wordLen}-1,\ldots$

⚠**注意**：无需考虑起点为 $\textit{wordLen}$ 的滑动窗口，它已经在起点为 $0$ 的滑动窗口中计算了。对于起点大于 $\textit{wordLen}$ 的滑动窗口也同理。

## 定长滑窗套路

我总结成三步：**入-更新-出**。

1. **入**：枚举进入窗口的单词 $\textit{inWord}$，把 $\textit{inWord}$ 的出现次数加一，如果加一之前 $\textit{inWord}$ 的出现次数等于其在 $\textit{targetCnt}$ 中的出现次数，那么加一之后就过多了，我们用一个变量 $\textit{overload}$ **统计过多的单词个数**。⚠**注意**：这包括不在 $\textit{words}$ 中的单词。
2. **更新**：更新答案。如果 $\textit{overload}=0$，那么符合要求，把窗口左端点加入答案。
3. **出**：窗口最左边的单词 $\textit{outWord}$ 离开窗口，减少 $\textit{outWord}$ 的出现次数。如果减少后 $\textit{outWord}$ 的出现次数等于其在 $\textit{targetCnt}$ 中的出现次数，那么 $\textit{outWord}$ 从「过多」变成「符合要求」，把 $\textit{overload}$ 减一。

以上三步适用于所有定长滑窗题目。

> 小优化：如果 $s$ 的长度小于 $\textit{windowLen}$，可以提前返回空列表。由于不影响实际运行时间，所以代码没有写这个优化。

```py [sol-Python3]
class Solution:
    def findSubstring(self, s: str, words: List[str]) -> List[int]:
        word_len = len(words[0])  # 一个单词的长度
        window_len = word_len * len(words)  # 所有单词的总长度，即窗口大小

        # 目标：窗口中的单词出现次数必须与 target_cnt 完全一致
        target_cnt = Counter(words)

        ans = []
        # 枚举第一个窗口的左端点，做 word_len 次起点不同的滑动窗口
        for start in range(word_len):
            cnt = defaultdict(int)
            overload = 0  # 统计过多的单词个数（包括不在 words 中的单词）
            # 枚举窗口最后一个单词的右开端点
            for right in range(start + word_len, len(s) + 1, word_len):
                # 1. in_word 进入窗口
                in_word = s[right - word_len: right]
                # 下面 cnt[in_word] += 1 后，in_word 的出现次数过多
                if cnt[in_word] == target_cnt[in_word]:
                    overload += 1
                cnt[in_word] += 1

                left = right - window_len  # 窗口第一个单词的左端点
                if left < 0:  # 窗口大小不足 window_len
                    continue

                # 2. 更新答案
                # 如果没有超出 target_cnt 的单词，那么也不会有少于 target_cnt 的单词
                if overload == 0:
                    ans.append(left)

                # 3. 窗口最左边的单词 out_word 离开窗口，为下一轮循环做准备
                out_word = s[left: left + word_len]
                cnt[out_word] -= 1
                if cnt[out_word] == target_cnt[out_word]:
                    overload -= 1

        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> findSubstring(String s, String[] words) {
        int wordLen = words[0].length(); // 一个单词的长度
        int windowLen = wordLen * words.length; // 所有单词的总长度，即窗口大小

        // 目标：窗口中的单词出现次数必须与 targetCnt 完全一致
        Map<String, Integer> targetCnt = new HashMap<>();
        for (String w : words) {
            targetCnt.merge(w, 1, Integer::sum); // targetCnt[w]++
        }

        List<Integer> ans = new ArrayList<>();
        // 枚举第一个窗口的左端点，做 wordLen 次起点不同的滑动窗口
        for (int start = 0; start < wordLen; start++) {
            Map<String, Integer> cnt = new HashMap<>();
            int overload = 0; // 统计过多的单词个数（包括不在 words 中的单词）
            // 枚举窗口最后一个单词的右开端点
            for (int right = start + wordLen; right <= s.length(); right += wordLen) {
                // 1. inWord 进入窗口
                String inWord = s.substring(right - wordLen, right);
                // 下面 cnt[inWord]++ 后，inWord 的出现次数过多
                if (cnt.getOrDefault(inWord, 0).equals(targetCnt.getOrDefault(inWord, 0))) {
                    overload++;
                }
                cnt.merge(inWord, 1, Integer::sum); // cnt[inWord]++

                int left = right - windowLen; // 窗口第一个单词的左端点
                if (left < 0) { // 窗口大小不足 windowLen
                    continue;
                }

                // 2. 更新答案
                // 如果没有超出 targetCnt 的单词，那么也不会有少于 targetCnt 的单词
                if (overload == 0) {
                    ans.add(left);
                }

                // 3. 窗口最左边的单词 outWord 离开窗口，为下一轮循环做准备
                String outWord = s.substring(left, left + wordLen);
                cnt.merge(outWord, -1, Integer::sum); // cnt[outWord]--
                if (cnt.get(outWord).equals(targetCnt.getOrDefault(outWord, 0))) {
                    overload--;
                }
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findSubstring(string s, vector<string>& words) {
        int word_len = words[0].size(); // 一个单词的长度
        int window_len = word_len * words.size(); // 所有单词的总长度，即窗口大小

        // 目标：窗口中的单词出现次数必须与 target_cnt 完全一致
        unordered_map<string, int> target_cnt;
        for (auto& w : words) {
            target_cnt[w]++;
        }

        vector<int> ans;
        // 枚举第一个窗口的左端点，做 word_len 次起点不同的滑动窗口
        for (int start = 0; start < word_len; start++) {
            unordered_map<string, int> cnt;
            int overload = 0; // 统计过多的单词个数（包括不在 words 中的单词）
            // 枚举窗口最后一个单词的右开端点
            for (int right = start + word_len; right <= s.size(); right += word_len) {
                // 1. in_word 进入窗口
                string in_word = s.substr(right - word_len, word_len);
                // 下面 cnt[in_word]++ 后，in_word 的出现次数过多
                if (cnt[in_word] == target_cnt[in_word]) {
                    overload++;
                }
                cnt[in_word]++;

                int left = right - window_len; // 窗口第一个单词的左端点
                if (left < 0) { // 窗口大小不足 window_len
                    continue;
                }

                // 2. 更新答案
                // 如果没有超出 target_cnt 的单词，那么也不会有少于 target_cnt 的单词
                if (overload == 0) {
                    ans.push_back(left);
                }

                // 3. 窗口最左边的单词 out_word 离开窗口，为下一轮循环做准备
                string out_word = s.substr(left, word_len);
                cnt[out_word]--;
                if (cnt[out_word] == target_cnt[out_word]) {
                    overload--;
                }
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func findSubstring(s string, words []string) (ans []int) {
    wordLen := len(words[0]) // 一个单词的长度
    windowLen := wordLen * len(words) // 所有单词的总长度，即窗口大小

    // 目标：窗口中的单词出现次数必须与 targetCnt 完全一致
    targetCnt := map[string]int{}
    for _, w := range words {
        targetCnt[w]++
    }

    // 枚举第一个窗口的左端点，做 wordLen 次起点不同的滑动窗口
    for start := range wordLen {
        cnt := map[string]int{}
        overload := 0 // 统计过多的单词个数（包括不在 words 中的单词）
        // 枚举窗口最后一个单词的右开端点
        for right := start + wordLen; right <= len(s); right += wordLen {
            // 1. inWord 进入窗口
            inWord := s[right-wordLen : right]
            // 下面 cnt[inWord]++ 后，inWord 的出现次数过多
            if cnt[inWord] == targetCnt[inWord] {
                overload++
            }
            cnt[inWord]++

            left := right - windowLen // 窗口第一个单词的左端点
            if left < 0 { // 窗口大小不足 windowLen
                continue
            }

            // 2. 更新答案
            // 如果没有超出 targetCnt 的单词，那么也不会有少于 targetCnt 的单词
            if overload == 0 {
                ans = append(ans, left)
            }

            // 3. 窗口最左边的单词 outWord 离开窗口，为下一轮循环做准备
            outWord := s[left : left+wordLen]
            cnt[outWord]--
            if cnt[outWord] == targetCnt[outWord] {
                overload--
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var findSubstring = function(s, words) {
    const wordLen = words[0].length; // 一个单词的长度
    const windowLen = wordLen * words.length; // 所有单词的总长度，即窗口大小

    // 目标：窗口中的单词出现次数必须与 targetCnt 完全一致
    const targetCnt = new Map();
    for (const w of words) {
        targetCnt.set(w, (targetCnt.get(w) ?? 0) + 1);
    }

    const ans = [];
    // 枚举第一个窗口的左端点，做 wordLen 次起点不同的滑动窗口
    for (let start = 0; start < wordLen; start++) {
        const cnt = new Map();
        let overload = 0; // 统计过多的单词个数（包括不在 words 中的单词）
        // 枚举窗口最后一个单词的右开端点
        for (let right = start + wordLen; right <= s.length; right += wordLen) {
            // 1. inWord 进入窗口
            const inWord = s.substring(right - wordLen, right);
            const cntIn = cnt.get(inWord) ?? 0;
            // 下面 cnt.set(inWord, cntIn + 1) 后，inWord 的出现次数过多
            if (cntIn === (targetCnt.get(inWord) ?? 0)) {
                overload++;
            }
            cnt.set(inWord, cntIn + 1);

            const left = right - windowLen; // 窗口第一个单词的左端点
            if (left < 0) { // 窗口大小不足 windowLen
                continue;
            }

            // 2. 更新答案
            // 如果没有超出 targetCnt 的单词，那么也不会有少于 targetCnt 的单词
            if (overload === 0) {
                ans.push(left);
            }

            // 3. 窗口最左边的单词 outWord 离开窗口，为下一轮循环做准备
            const outWord = s.substring(left, left + wordLen);
            const cntOut = cnt.get(outWord) - 1;
            cnt.set(outWord, cntOut);
            if (cntOut === (targetCnt.get(outWord) ?? 0)) {
                overload--;
            }
        }
    }

    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn find_substring(s: String, words: Vec<String>) -> Vec<i32> {
        let word_len = words[0].len(); // 一个单词的长度
        let window_len = word_len * words.len(); // 所有单词的总长度，即窗口大小

        // 目标：窗口中的单词出现次数必须与 target_cnt 完全一致
        let mut target_cnt = HashMap::new();
        for w in &words {
            *target_cnt.entry(w.as_str()).or_insert(0) += 1;
        }

        let mut ans = vec![];
        // 枚举第一个窗口的左端点，做 word_len 次起点不同的滑动窗口
        for start in 0..word_len {
            let mut cnt = HashMap::new();
            let mut overload = 0; // 统计过多的单词个数（包括不在 words 中的单词）
            // 枚举窗口最后一个单词的右开端点
            for right in (start + word_len..=s.len()).step_by(word_len) {
                // 1. in_word 进入窗口
                let in_word = &s[right - word_len..right];
                let e = cnt.entry(in_word).or_insert(0);
                // 下面 *e += 1 后，in_word 的出现次数过多
                if e == target_cnt.get(in_word).unwrap_or(&0) {
                    overload += 1;
                }
                *e += 1;

                if right < window_len { // 窗口大小不足 window_len
                    continue;
                }
                let left = right - window_len; // 窗口第一个单词的左端点

                // 2. 更新答案
                // 如果没有超出 target_cnt 的单词，那么也不会有少于 target_cnt 的单词
                if overload == 0 {
                    ans.push(left as i32);
                }

                // 3. 窗口最左边的单词 out_word 离开窗口，为下一轮循环做准备
                let out_word = &s[left..left + word_len];
                let e = cnt.entry(out_word).or_insert(0);
                *e -= 1;
                if e == target_cnt.get(out_word).unwrap_or(&0) {
                    overload -= 1;
                }
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m)\cdot \textit{wordLen})$，其中 $n$ 是 $s$ 的长度，$m$ 是 $\textit{words}$ 的长度，$wordLen$ 是 $\textit{words}[i]$ 的长度。初始化 $\textit{targetCnt}$ 需要 $\mathcal{O}(m\cdot \textit{wordLen})$ 的时间，做 $\mathcal{O}(\textit{wordLen})$ 次定长滑窗，每次 $\mathcal{O}(n)$ 时间。
- 空间复杂度：$\mathcal{O}(m\cdot \textit{wordLen})$。注意窗口长度为 $\mathcal{O}(m\cdot \textit{wordLen})$，所以 $\textit{cnt}$ 消耗的空间是 $\mathcal{O}(m\cdot \textit{wordLen})$。

更多相似题目，见下面滑动窗口题单的「**一、定长滑动窗口**」。

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
