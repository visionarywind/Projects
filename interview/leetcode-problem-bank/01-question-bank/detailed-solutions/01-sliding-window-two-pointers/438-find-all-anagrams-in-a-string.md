# 438. 找到字符串中所有字母异位词

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-all-anagrams-in-a-string/
- 题目 slug：`find-all-anagrams-in-a-string`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-all-anagrams-in-a-string/solutions/2969498/liang-chong-fang-fa-ding-chang-hua-chuan-14pd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：定长滑窗/不定长滑窗（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-all-anagrams-in-a-string/solutions/2969498/liang-chong-fang-fa-ding-chang-hua-chuan-14pd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-ding-chang-hua-chuan-14pd`
- topic id：`2969498`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

设 $n$ 是 $p$ 的长度。本题有两种做法：

1. **定长滑窗**。枚举 $s$ 的所有长为 $n$ 的子串 $t$，如果 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同，那么 $t$ 是 $p$ 的异位词。
2. **不定长滑窗**。枚举子串 $t$ 的右端点，如果发现 $t$ 其中一种字母的出现次数大于 $p$ 的这种字母的出现次数，则增大 $t$ 的左端点（缩小窗口）。如果发现 $t$ 的长度等于 $p$ 的长度，则说明 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同（如果出现次数 $t$ 的小于 $p$ 的，不可能长度一样），那么 $t$ 是 $p$ 的异位词。

## 方法一：定长滑窗

原理请看[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)。

用滑动窗口枚举 $s$ 的所有长为 $n$ 的子串 $t$。在滑的同时，维护 $t$ 的每种字母的出现次数。

如果 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同，那么 $t$ 是 $p$ 的异位词，把 $t$ 左端点下标加入答案。

```py [sol-Python3]
# 请选择 Python3 提交代码，而不是 Python
class Solution:
    def findAnagrams(self, s: str, p: str) -> List[int]:
        cnt_p = Counter(p)  # 统计 p 的每种字母的出现次数
        cnt_s = Counter()  # 统计 s 的长为 len(p) 的子串 t 的每种字母的出现次数
        ans = []

        for right, c in enumerate(s):
            cnt_s[c] += 1  # 右端点字母进入窗口

            left = right - len(p) + 1
            if left < 0:  # 窗口长度不足 len(p)
                continue

            if cnt_s == cnt_p:  # t 和 p 的每种字母的出现次数都相同
                ans.append(left)  # t 左端点下标加入答案

            cnt_s[s[left]] -= 1  # 左端点字母离开窗口

        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> findAnagrams(String s, String p) {
        // 统计 p 的每种字母的出现次数
        int[] cntP = new int[26];
        for (char c : p.toCharArray()) {
            cntP[c - 'a']++; // 统计 p 的字母
        }

        List<Integer> ans = new ArrayList<>();
        int[] cntS = new int[26]; // 统计 s 的长为 p.length() 的子串 t 的每种字母的出现次数
        for (int right = 0; right < s.length(); right++) {
            cntS[s.charAt(right) - 'a']++; // 右端点字母进入窗口
            int left = right - p.length() + 1;
            if (left < 0) { // 窗口长度不足 p.length()
                continue;
            }
            if (Arrays.equals(cntS, cntP)) { // t 和 p 的每种字母的出现次数都相同
                ans.add(left); // t 左端点下标加入答案
            }
            cntS[s.charAt(left) - 'a']--; // 左端点字母离开窗口
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findAnagrams(string s, string p) {
        // 统计 p 的每种字母的出现次数
        array<int, 26> cnt_p{}; 
        for (char c : p) {
            cnt_p[c - 'a']++;
        }

        vector<int> ans;
        array<int, 26> cnt_s{}; // 统计 s 的长为 p.size() 的子串 t 的每种字母的出现次数
        for (int right = 0; right < s.size(); right++) {
            cnt_s[s[right] - 'a']++; // 右端点字母进入窗口
            int left = right - p.size() + 1;
            if (left < 0) { // 窗口长度不足 p.size()
                continue;
            }
            if (cnt_s == cnt_p) { // t 和 p 的每种字母的出现次数都相同
                ans.push_back(left); // t 左端点下标加入答案
            }
            cnt_s[s[left] - 'a']--; // 左端点字母离开窗口
        }
        return ans;
    }
};
```

```c [sol-C]
int* findAnagrams(char* s, char* p, int* returnSize) {
    // 统计 p 的每种字母的出现次数
    int cnt_p[26] = {}; 
    int n = 0;
    for (; p[n]; n++) {
        cnt_p[p[n] - 'a']++;
    }

    int* ans = malloc(strlen(s) * sizeof(int));
    *returnSize = 0;
    int cnt_s[26] = {}; // 统计 s 的长为 n 的子串 t 的每种字母的出现次数
    for (int right = 0; s[right]; right++) {
        cnt_s[s[right] - 'a']++; // 右端点字母进入窗口
        int left = right - n + 1;
        if (left < 0) { // 窗口长度不足 n
            continue;
        }
        if (memcmp(cnt_s, cnt_p, sizeof(cnt_s)) == 0) { // t 和 p 的每种字母的出现次数都相同
            ans[(*returnSize)++] = left; // t 左端点下标加入答案
        }
        cnt_s[s[left] - 'a']--; // 左端点字母离开窗口
    }
    return ans;
}
```

```go [sol-Go]
func findAnagrams(s, p string) (ans []int) {
    // 统计 p 的每种字母的出现次数
    cntP := [26]int{} 
    for _, c := range p {
        cntP[c-'a']++
    }

    cntS := [26]int{} // 统计 s 的长为 len(p) 的子串 t 的每种字母的出现次数
    for right, c := range s {
        cntS[c-'a']++ // 右端点字母进入窗口
        left := right - len(p) + 1
        if left < 0 { // 窗口长度不足 len(p)
            continue
        }
        if cntS == cntP { // t 和 p 的每种字母的出现次数都相同
            ans = append(ans, left) // t 左端点下标加入答案
        }
        cntS[s[left]-'a']-- // 左端点字母离开窗口
    }
    return
}
```

```js [sol-JavaScript]
var findAnagrams = function(s, p) {
    // 统计 p 的每种字母的出现次数
    const cntP = new Array(26).fill(0); 
    for (const c of p) {
        cntP[c.charCodeAt() - 'a'.charCodeAt()]++;
    }

    const ans = [];
    const cntS = new Array(26).fill(0); // 统计 s 的长为 len(p) 的子串 t 的每种字母的出现次数
    for (let right = 0; right < s.length; right++) {
        cntS[s[right].charCodeAt() - 'a'.charCodeAt()]++; // 右端点字母进入窗口
        const left = right - p.length + 1;
        if (left < 0) { // 窗口长度不足 len(p)
            continue;
        }
        if (_.isEqual(cntS, cntP)) { // t 和 p 的每种字母的出现次数都相同
            ans.push(left); // t 左端点下标加入答案
        }
        cntS[s[left].charCodeAt() - 'a'.charCodeAt()]--; // 左端点字母离开窗口
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_anagrams(s: String, p: String) -> Vec<i32> {
        // 统计 p 的每种字母的出现次数
        let mut cnt_p = [0; 26]; 
        for c in p.bytes() {
            cnt_p[(c - b'a') as usize] += 1;
        }

        let s = s.as_bytes();
        let mut ans = vec![];
        let mut cnt_s = [0; 26]; // 统计 s 的长为 p.len() 的子串 t 的每种字母的出现次数
        for (right, &c) in s.iter().enumerate() {
            cnt_s[(c - b'a') as usize] += 1; // 右端点字母进入窗口
            let left = right - p.len() + 1;
            if left >= s.len() { // 窗口长度不足 p.len()
                continue;
            }
            if cnt_s == cnt_p { // t 和 p 的每种字母的出现次数都相同
                ans.push(left as i32); // t 左端点下标加入答案
            }
            cnt_s[(s[left] - b'a') as usize] -= 1; // 左端点字母离开窗口
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(|\Sigma|m+n)$，其中 $m$ 是 $s$ 的长度，$n$ 是 $p$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。返回值不计入。

**注**：可以优化到 $\mathcal{O}(m+n)$ 或者 $\mathcal{O}(m)$，做法见我的 [76. 最小覆盖子串的题解](https://leetcode.cn/problems/minimum-window-substring/solutions/2713911/liang-chong-fang-fa-cong-o52mn-dao-omnfu-3ezz/)。

## 方法二：不定长滑窗

**前置知识**：[滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

枚举子串 $t$ 的右端点，如果发现 $t$ 其中一种字母的出现次数**大于** $p$ 的这种字母的出现次数，则右移 $t$ 的左端点（缩小窗口）。如果发现 $t$ 的长度等于 $p$ 的长度，则说明 $t$ 的每种字母的出现次数，等于 $p$ 的每种字母的出现次数，即 $t$ 是 $p$ 的异位词。

**证明**：内层循环结束后，$t$ 的每种字母的出现次数，都小于等于 $p$ 的每种字母的出现次数。如果 $t$ 的其中一种字母的出现次数比 $p$ 的小，那么 $t$ 的长度必然小于 $p$ 的长度。所以只要 $t$ 的长度等于 $p$ 的长度，就说明 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同，$t$ 是 $p$ 的异位词，把 $t$ 左端点下标加入答案。

代码实现时，可以把 $\textit{cntS}$ 和 $\textit{cntP}$ 合并成一个 $\textit{cnt}$：

- 对于 $p$ 的字母 $c$，把 $\textit{cnt}[p]$ 加一。
- 对于 $t$ 的字母 $c$，把 $\textit{cnt}[c]$ 减一。
- 如果 $\textit{cnt}[c]<0$，说明窗口中的字母 $c$ 的个数比 $p$ 的多，右移左端点。

### 答疑

**问**：为什么内层循环只判断了字母 $c$ 的出现次数，而不是每种字母的出现次数？

**答**：如果字母 $c$ 进入窗口后，窗口不合法（某个 $\textit{cnt}[x] < 0$），那么罪魁祸首是谁？由于在之前的循环中，我们已经把窗口变成合法的了，所以只能是刚进入窗口的字母 $c$ 导致窗口不合法，其余字母都满足 $\textit{cnt}[x] \ge 0$，所以只需判断字母 $c$ 的出现次数。

```py [sol-Python3]
# 请选择 Python3 提交代码，而不是 Python
class Solution:
    def findAnagrams(self, s: str, p: str) -> List[int]:
        cnt = Counter(p)  # 统计 p 的每种字母的出现次数
        ans = []

        left = 0
        for right, c in enumerate(s):
            cnt[c] -= 1  # 右端点字母进入窗口
            while cnt[c] < 0:  # 字母 c 太多了
                cnt[s[left]] += 1  # 左端点字母离开窗口
                left += 1
            if right - left + 1 == len(p):  # t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.append(left)  # t 左端点下标加入答案

        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> findAnagrams(String s, String p) {
        // 统计 p 的每种字母的出现次数
        int[] cnt = new int[26]; 
        for (char c : p.toCharArray()) {
            cnt[c - 'a']++;
        }

        List<Integer> ans = new ArrayList<>();
        int left = 0;
        for (int right = 0; right < s.length(); right++) {
            int c = s.charAt(right) - 'a';
            cnt[c]--; // 右端点字母进入窗口
            while (cnt[c] < 0) { // 字母 c 太多了
                cnt[s.charAt(left) - 'a']++; // 左端点字母离开窗口
                left++;
            }
            if (right - left + 1 == p.length()) { // t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.add(left); // t 左端点下标加入答案
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findAnagrams(string s, string p) {
        // 统计 p 的每种字母的出现次数
        int cnt[26]{}; 
        for (char c : p) {
            cnt[c - 'a']++;
        }

        vector<int> ans;
        int left = 0;
        for (int right = 0; right < s.size(); right++) {
            int c = s[right] - 'a';
            cnt[c]--; // 右端点字母进入窗口
            while (cnt[c] < 0) { // 字母 c 太多了
                cnt[s[left] - 'a']++; // 左端点字母离开窗口
                left++; 
            }
            if (right - left + 1 == p.size()) { // t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.push_back(left); // t 左端点下标加入答案
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int* findAnagrams(char* s, char* p, int* returnSize) {
    // 统计 p 的每种字母的出现次数
    int cnt[26] = {};
    int n = 0;
    for (; p[n]; n++) {
        cnt[p[n] - 'a']++;
    }
    
    int* ans = malloc(strlen(s) * sizeof(int));
    *returnSize = 0;
    int left = 0;
    for (int right = 0; s[right]; right++) {
        int c = s[right] - 'a';
        cnt[c]--; // 右端点字母进入窗口
        while (cnt[c] < 0) { // 字母 c 太多了
            cnt[s[left] - 'a']++; // 左端点字母离开窗口
            left++;
        }
        if (right - left + 1 == n) { // t 和 p 的每种字母的出现次数都相同（证明见上）
            ans[(*returnSize)++] = left; // t 左端点下标加入答案
        }
    }
    return ans;
}
```

```go [sol-Go]
func findAnagrams(s, p string) (ans []int) {
    // 统计 p 的每种字母的出现次数
    cnt := [26]int{}
    for _, c := range p {
        cnt[c-'a']++
    }

    left := 0
    for right, c := range s {
        c -= 'a'
        cnt[c]-- // 右端点字母进入窗口
        for cnt[c] < 0 { // 字母 c 太多了
            cnt[s[left]-'a']++ // 左端点字母离开窗口
            left++
        }
        if right-left+1 == len(p) { // t 和 p 的每种字母的出现次数都相同（证明见上）
            ans = append(ans, left) // t 左端点下标加入答案
        }
    }
    return
}
```

```js [sol-JavaScript]
var findAnagrams = function(s, p) {
    // 统计 p 的每种字母的出现次数
    const cnt = new Array(26).fill(0); 
    for (const c of p) {
        cnt[c.charCodeAt() - 'a'.charCodeAt()]++;
    }

    const ans = [];
    let left = 0;
    for (let right = 0; right < s.length; right++) {
        const c = s[right].charCodeAt() - 'a'.charCodeAt();
        cnt[c]--; // 右端点字母进入窗口
        while (cnt[c] < 0) { // 字母 c 太多了
            cnt[s[left].charCodeAt() - 'a'.charCodeAt()]++; // 左端点字母离开窗口
            left++;
        }
        if (right - left + 1 === p.length) { // t 和 p 的每种字母的出现次数都相同（证明见上）
            ans.push(left); // t 左端点下标加入答案
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_anagrams(s: String, p: String) -> Vec<i32> {
        // 统计 p 的每种字母的出现次数
        let mut cnt = [0; 26]; 
        for c in p.bytes() {
            cnt[(c - b'a') as usize] += 1; // 统计 p 的字母
        }

        let s = s.as_bytes();
        let mut ans = vec![];
        let mut left = 0;
        for (right, &c) in s.iter().enumerate() {
            let c = (c - b'a') as usize;
            cnt[c] -= 1; // 右端点字母进入窗口
            while cnt[c] < 0 { // 字母 c 太多了
                cnt[(s[left] - b'a') as usize] += 1; // 左端点字母离开窗口
                left += 1;
            }
            if right - left + 1 == p.len() { // t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.push(left as i32); // t 左端点下标加入答案
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n)$，其中 $m$ 是 $s$ 的长度，$n$ 是 $p$ 的长度。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $m$ 次，所以滑窗的时间复杂度为 $\mathcal{O}(m)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$，其中 $|\Sigma|=26$ 是字符集合的大小。返回值不计入。

**注**：如果特判 $m<n$ 的情况（直接返回空列表），则时间复杂度为 $\mathcal{O}(m)$。

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

## 核心思路

设 $n$ 是 $p$ 的长度。本题有两种做法：

1. **定长滑窗**。枚举 $s$ 的所有长为 $n$ 的子串 $t$，如果 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同，那么 $t$ 是 $p$ 的异位词。
2. **不定长滑窗**。枚举子串 $t$ 的右端点，如果发现 $t$ 其中一种字母的出现次数大于 $p$ 的这种字母的出现次数，则增大 $t$ 的左端点（缩小窗口）。如果发现 $t$ 的长度等于 $p$ 的长度，则说明 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同（如果出现次数 $t$ 的小于 $p$ 的，不可能长度一样），那么 $t$ 是 $p$ 的异位词。

## 方法一：定长滑窗

原理请看[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)。

用滑动窗口枚举 $s$ 的所有长为 $n$ 的子串 $t$。在滑的同时，维护 $t$ 的每种字母的出现次数。

如果 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同，那么 $t$ 是 $p$ 的异位词，把 $t$ 左端点下标加入答案。

```py [sol-Python3]
# 请选择 Python3 提交代码，而不是 Python
class Solution:
    def findAnagrams(self, s: str, p: str) -> List[int]:
        cnt_p = Counter(p)  # 统计 p 的每种字母的出现次数
        cnt_s = Counter()  # 统计 s 的长为 len(p) 的子串 t 的每种字母的出现次数
        ans = []

        for right, c in enumerate(s):
            cnt_s[c] += 1  # 右端点字母进入窗口

            left = right - len(p) + 1
            if left < 0:  # 窗口长度不足 len(p)
                continue

            if cnt_s == cnt_p:  # t 和 p 的每种字母的出现次数都相同
                ans.append(left)  # t 左端点下标加入答案

            cnt_s[s[left]] -= 1  # 左端点字母离开窗口

        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> findAnagrams(String s, String p) {
        // 统计 p 的每种字母的出现次数
        int[] cntP = new int[26];
        for (char c : p.toCharArray()) {
            cntP[c - 'a']++; // 统计 p 的字母
        }

        List<Integer> ans = new ArrayList<>();
        int[] cntS = new int[26]; // 统计 s 的长为 p.length() 的子串 t 的每种字母的出现次数
        for (int right = 0; right < s.length(); right++) {
            cntS[s.charAt(right) - 'a']++; // 右端点字母进入窗口
            int left = right - p.length() + 1;
            if (left < 0) { // 窗口长度不足 p.length()
                continue;
            }
            if (Arrays.equals(cntS, cntP)) { // t 和 p 的每种字母的出现次数都相同
                ans.add(left); // t 左端点下标加入答案
            }
            cntS[s.charAt(left) - 'a']--; // 左端点字母离开窗口
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findAnagrams(string s, string p) {
        // 统计 p 的每种字母的出现次数
        array<int, 26> cnt_p{}; 
        for (char c : p) {
            cnt_p[c - 'a']++;
        }

        vector<int> ans;
        array<int, 26> cnt_s{}; // 统计 s 的长为 p.size() 的子串 t 的每种字母的出现次数
        for (int right = 0; right < s.size(); right++) {
            cnt_s[s[right] - 'a']++; // 右端点字母进入窗口
            int left = right - p.size() + 1;
            if (left < 0) { // 窗口长度不足 p.size()
                continue;
            }
            if (cnt_s == cnt_p) { // t 和 p 的每种字母的出现次数都相同
                ans.push_back(left); // t 左端点下标加入答案
            }
            cnt_s[s[left] - 'a']--; // 左端点字母离开窗口
        }
        return ans;
    }
};
```

```c [sol-C]
int* findAnagrams(char* s, char* p, int* returnSize) {
    // 统计 p 的每种字母的出现次数
    int cnt_p[26] = {}; 
    int n = 0;
    for (; p[n]; n++) {
        cnt_p[p[n] - 'a']++;
    }

    int* ans = malloc(strlen(s) * sizeof(int));
    *returnSize = 0;
    int cnt_s[26] = {}; // 统计 s 的长为 n 的子串 t 的每种字母的出现次数
    for (int right = 0; s[right]; right++) {
        cnt_s[s[right] - 'a']++; // 右端点字母进入窗口
        int left = right - n + 1;
        if (left < 0) { // 窗口长度不足 n
            continue;
        }
        if (memcmp(cnt_s, cnt_p, sizeof(cnt_s)) == 0) { // t 和 p 的每种字母的出现次数都相同
            ans[(*returnSize)++] = left; // t 左端点下标加入答案
        }
        cnt_s[s[left] - 'a']--; // 左端点字母离开窗口
    }
    return ans;
}
```

```go [sol-Go]
func findAnagrams(s, p string) (ans []int) {
    // 统计 p 的每种字母的出现次数
    cntP := [26]int{} 
    for _, c := range p {
        cntP[c-'a']++
    }

    cntS := [26]int{} // 统计 s 的长为 len(p) 的子串 t 的每种字母的出现次数
    for right, c := range s {
        cntS[c-'a']++ // 右端点字母进入窗口
        left := right - len(p) + 1
        if left < 0 { // 窗口长度不足 len(p)
            continue
        }
        if cntS == cntP { // t 和 p 的每种字母的出现次数都相同
            ans = append(ans, left) // t 左端点下标加入答案
        }
        cntS[s[left]-'a']-- // 左端点字母离开窗口
    }
    return
}
```

```js [sol-JavaScript]
var findAnagrams = function(s, p) {
    // 统计 p 的每种字母的出现次数
    const cntP = new Array(26).fill(0); 
    for (const c of p) {
        cntP[c.charCodeAt() - 'a'.charCodeAt()]++;
    }

    const ans = [];
    const cntS = new Array(26).fill(0); // 统计 s 的长为 len(p) 的子串 t 的每种字母的出现次数
    for (let right = 0; right < s.length; right++) {
        cntS[s[right].charCodeAt() - 'a'.charCodeAt()]++; // 右端点字母进入窗口
        const left = right - p.length + 1;
        if (left < 0) { // 窗口长度不足 len(p)
            continue;
        }
        if (_.isEqual(cntS, cntP)) { // t 和 p 的每种字母的出现次数都相同
            ans.push(left); // t 左端点下标加入答案
        }
        cntS[s[left].charCodeAt() - 'a'.charCodeAt()]--; // 左端点字母离开窗口
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_anagrams(s: String, p: String) -> Vec<i32> {
        // 统计 p 的每种字母的出现次数
        let mut cnt_p = [0; 26]; 
        for c in p.bytes() {
            cnt_p[(c - b'a') as usize] += 1;
        }

        let s = s.as_bytes();
        let mut ans = vec![];
        let mut cnt_s = [0; 26]; // 统计 s 的长为 p.len() 的子串 t 的每种字母的出现次数
        for (right, &c) in s.iter().enumerate() {
            cnt_s[(c - b'a') as usize] += 1; // 右端点字母进入窗口
            let left = right - p.len() + 1;
            if left >= s.len() { // 窗口长度不足 p.len()
                continue;
            }
            if cnt_s == cnt_p { // t 和 p 的每种字母的出现次数都相同
                ans.push(left as i32); // t 左端点下标加入答案
            }
            cnt_s[(s[left] - b'a') as usize] -= 1; // 左端点字母离开窗口
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(|\Sigma|m+n)$，其中 $m$ 是 $s$ 的长度，$n$ 是 $p$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。返回值不计入。

**注**：可以优化到 $\mathcal{O}(m+n)$ 或者 $\mathcal{O}(m)$，做法见我的 [76. 最小覆盖子串的题解](https://leetcode.cn/problems/minimum-window-substring/solutions/2713911/liang-chong-fang-fa-cong-o52mn-dao-omnfu-3ezz/)。

## 方法二：不定长滑窗

**前置知识**：[滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

枚举子串 $t$ 的右端点，如果发现 $t$ 其中一种字母的出现次数**大于** $p$ 的这种字母的出现次数，则右移 $t$ 的左端点（缩小窗口）。如果发现 $t$ 的长度等于 $p$ 的长度，则说明 $t$ 的每种字母的出现次数，等于 $p$ 的每种字母的出现次数，即 $t$ 是 $p$ 的异位词。

**证明**：内层循环结束后，$t$ 的每种字母的出现次数，都小于等于 $p$ 的每种字母的出现次数。如果 $t$ 的其中一种字母的出现次数比 $p$ 的小，那么 $t$ 的长度必然小于 $p$ 的长度。所以只要 $t$ 的长度等于 $p$ 的长度，就说明 $t$ 的每种字母的出现次数，和 $p$ 的每种字母的出现次数都相同，$t$ 是 $p$ 的异位词，把 $t$ 左端点下标加入答案。

代码实现时，可以把 $\textit{cntS}$ 和 $\textit{cntP}$ 合并成一个 $\textit{cnt}$：

- 对于 $p$ 的字母 $c$，把 $\textit{cnt}[p]$ 加一。
- 对于 $t$ 的字母 $c$，把 $\textit{cnt}[c]$ 减一。
- 如果 $\textit{cnt}[c]<0$，说明窗口中的字母 $c$ 的个数比 $p$ 的多，右移左端点。

### 答疑

**问**：为什么内层循环只判断了字母 $c$ 的出现次数，而不是每种字母的出现次数？

**答**：如果字母 $c$ 进入窗口后，窗口不合法（某个 $\textit{cnt}[x] < 0$），那么罪魁祸首是谁？由于在之前的循环中，我们已经把窗口变成合法的了，所以只能是刚进入窗口的字母 $c$ 导致窗口不合法，其余字母都满足 $\textit{cnt}[x] \ge 0$，所以只需判断字母 $c$ 的出现次数。

```py [sol-Python3]
# 请选择 Python3 提交代码，而不是 Python
class Solution:
    def findAnagrams(self, s: str, p: str) -> List[int]:
        cnt = Counter(p)  # 统计 p 的每种字母的出现次数
        ans = []

        left = 0
        for right, c in enumerate(s):
            cnt[c] -= 1  # 右端点字母进入窗口
            while cnt[c] < 0:  # 字母 c 太多了
                cnt[s[left]] += 1  # 左端点字母离开窗口
                left += 1
            if right - left + 1 == len(p):  # t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.append(left)  # t 左端点下标加入答案

        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> findAnagrams(String s, String p) {
        // 统计 p 的每种字母的出现次数
        int[] cnt = new int[26]; 
        for (char c : p.toCharArray()) {
            cnt[c - 'a']++;
        }

        List<Integer> ans = new ArrayList<>();
        int left = 0;
        for (int right = 0; right < s.length(); right++) {
            int c = s.charAt(right) - 'a';
            cnt[c]--; // 右端点字母进入窗口
            while (cnt[c] < 0) { // 字母 c 太多了
                cnt[s.charAt(left) - 'a']++; // 左端点字母离开窗口
                left++;
            }
            if (right - left + 1 == p.length()) { // t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.add(left); // t 左端点下标加入答案
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findAnagrams(string s, string p) {
        // 统计 p 的每种字母的出现次数
        int cnt[26]{}; 
        for (char c : p) {
            cnt[c - 'a']++;
        }

        vector<int> ans;
        int left = 0;
        for (int right = 0; right < s.size(); right++) {
            int c = s[right] - 'a';
            cnt[c]--; // 右端点字母进入窗口
            while (cnt[c] < 0) { // 字母 c 太多了
                cnt[s[left] - 'a']++; // 左端点字母离开窗口
                left++; 
            }
            if (right - left + 1 == p.size()) { // t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.push_back(left); // t 左端点下标加入答案
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int* findAnagrams(char* s, char* p, int* returnSize) {
    // 统计 p 的每种字母的出现次数
    int cnt[26] = {};
    int n = 0;
    for (; p[n]; n++) {
        cnt[p[n] - 'a']++;
    }
    
    int* ans = malloc(strlen(s) * sizeof(int));
    *returnSize = 0;
    int left = 0;
    for (int right = 0; s[right]; right++) {
        int c = s[right] - 'a';
        cnt[c]--; // 右端点字母进入窗口
        while (cnt[c] < 0) { // 字母 c 太多了
            cnt[s[left] - 'a']++; // 左端点字母离开窗口
            left++;
        }
        if (right - left + 1 == n) { // t 和 p 的每种字母的出现次数都相同（证明见上）
            ans[(*returnSize)++] = left; // t 左端点下标加入答案
        }
    }
    return ans;
}
```

```go [sol-Go]
func findAnagrams(s, p string) (ans []int) {
    // 统计 p 的每种字母的出现次数
    cnt := [26]int{}
    for _, c := range p {
        cnt[c-'a']++
    }

    left := 0
    for right, c := range s {
        c -= 'a'
        cnt[c]-- // 右端点字母进入窗口
        for cnt[c] < 0 { // 字母 c 太多了
            cnt[s[left]-'a']++ // 左端点字母离开窗口
            left++
        }
        if right-left+1 == len(p) { // t 和 p 的每种字母的出现次数都相同（证明见上）
            ans = append(ans, left) // t 左端点下标加入答案
        }
    }
    return
}
```

```js [sol-JavaScript]
var findAnagrams = function(s, p) {
    // 统计 p 的每种字母的出现次数
    const cnt = new Array(26).fill(0); 
    for (const c of p) {
        cnt[c.charCodeAt() - 'a'.charCodeAt()]++;
    }

    const ans = [];
    let left = 0;
    for (let right = 0; right < s.length; right++) {
        const c = s[right].charCodeAt() - 'a'.charCodeAt();
        cnt[c]--; // 右端点字母进入窗口
        while (cnt[c] < 0) { // 字母 c 太多了
            cnt[s[left].charCodeAt() - 'a'.charCodeAt()]++; // 左端点字母离开窗口
            left++;
        }
        if (right - left + 1 === p.length) { // t 和 p 的每种字母的出现次数都相同（证明见上）
            ans.push(left); // t 左端点下标加入答案
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_anagrams(s: String, p: String) -> Vec<i32> {
        // 统计 p 的每种字母的出现次数
        let mut cnt = [0; 26]; 
        for c in p.bytes() {
            cnt[(c - b'a') as usize] += 1; // 统计 p 的字母
        }

        let s = s.as_bytes();
        let mut ans = vec![];
        let mut left = 0;
        for (right, &c) in s.iter().enumerate() {
            let c = (c - b'a') as usize;
            cnt[c] -= 1; // 右端点字母进入窗口
            while cnt[c] < 0 { // 字母 c 太多了
                cnt[(s[left] - b'a') as usize] += 1; // 左端点字母离开窗口
                left += 1;
            }
            if right - left + 1 == p.len() { // t 和 p 的每种字母的出现次数都相同（证明见上）
                ans.push(left as i32); // t 左端点下标加入答案
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n)$，其中 $m$ 是 $s$ 的长度，$n$ 是 $p$ 的长度。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $m$ 次，所以滑窗的时间复杂度为 $\mathcal{O}(m)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$，其中 $|\Sigma|=26$ 是字符集合的大小。返回值不计入。

**注**：如果特判 $m<n$ 的情况（直接返回空列表），则时间复杂度为 $\mathcal{O}(m)$。

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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
