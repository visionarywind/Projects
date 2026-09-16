# 76. 最小覆盖子串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-window-substring/
- 题目 slug：`minimum-window-substring`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.2 越长越合法/求最短/最小
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/minimum-window-substring/solutions/2713911/liang-chong-fang-fa-cong-o52mn-dao-omnfu-3ezz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：从 O(52m+n) 到 O(m+n)，附题单（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-window-substring/solutions/2713911/liang-chong-fang-fa-cong-o52mn-dao-omnfu-3ezz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-cong-o52mn-dao-omnfu-3ezz`
- topic id：`2713911`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：滑动窗口

如果您不知道滑动窗口，推荐先看视频 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)，并完成 [209. 长度最小的子数组](https://leetcode.cn/problems/minimum-size-subarray-sum/) 作为本题的铺垫，因为这两题都属于「越长越合法」滑动窗口。

## 什么是「涵盖」

看示例 1，$s$ 的子串 $\texttt{BANC}$ 中每个字母的出现次数，都**大于等于** $t=\texttt{ABC}$ 中每个字母的出现次数，这就叫**涵盖**。

## 滑动窗口怎么滑

原理和 209 题一样，按照视频中的做法，我们枚举 $s$ 子串的右端点 $\textit{right}$（子串最后一个字母的下标），如果子串涵盖 $t$，就不断右移左端点 $\textit{left}$ 直到不涵盖为止。在移动过程中更新最短子串的左右端点。

具体来说：

1. 初始化 $\textit{ansLeft}=-1,\ \textit{ansRight}=m$，用来记录最短子串的左右端点，其中 $m$ 是 $s$ 的长度。
2. 用一个哈希表（或者数组）$\textit{cntT}$ 统计 $t$ 中每个字母的出现次数。
3. 初始化 $\textit{left}=0$，以及一个空哈希表（或者数组）$\textit{cntS}$，用来统计 $s$ 子串中每个字母的出现次数。
4. 遍历 $s$，设当前枚举的子串右端点为 $\textit{right}$，把 $s[\textit{right}]$ 的出现次数加一。
5. 遍历 $\textit{cntS}$ 中的每个字母及其出现次数，如果出现次数都大于等于 $\textit{cntT}$ 中的字母出现次数：
     1. 如果 $\textit{right}-\textit{left} < \textit{ansRight} - \textit{ansLeft}$，说明我们找到了更短的子串，更新 $\textit{ansLeft} = \textit{left},\  \textit{ansRight} = \textit{right}$。
     2. 把 $s[\textit{left}]$ 的出现次数减一。
     3. 左端点右移，即 $\textit{left}$ 加一。
     4. 重复上述三步，直到 $\textit{cntS}$ 有字母的出现次数小于 $\textit{cntT}$ 中该字母的出现次数为止。
6. 最后，如果 $\textit{ansLeft}<0$，说明没有找到符合要求的子串，返回空字符串，否则返回下标 $\textit{ansLeft}$ 到下标 $\textit{ansRight}$ 之间的子串。

由于本题大写字母和小写字母都有，为了方便，代码实现时可以直接创建大小为 $128$ 的数组，直接把 ASCII 值作为数组的下标。

## 优化前

```py [sol-Python3]
# 请选择 Python3 提交代码，而不是 Python
class Solution:
    def minWindow(self, s: str, t: str) -> str:
        cnt_s = Counter()  # s 子串字母的出现次数
        cnt_t = Counter(t)  # t 中字母的出现次数

        ans_left, ans_right = -1, len(s)
        left = 0

        for right, c in enumerate(s):  # 移动子串右端点
            cnt_s[c] += 1  # 右端点字母移入子串
            while cnt_s >= cnt_t:  # 涵盖
                if right - left < ans_right - ans_left:  # 找到更短的子串
                    ans_left, ans_right = left, right  # 记录此时的左右端点
                cnt_s[s[left]] -= 1  # 左端点字母移出子串
                left += 1

        return "" if ans_left < 0 else s[ans_left: ans_right + 1]
```

```java [sol-Java]
class Solution {
    public String minWindow(String S, String t) {
        int[] cntS = new int[128]; // s 子串字母的出现次数
        int[] cntT = new int[128]; // t 中字母的出现次数
        for (char c : t.toCharArray()) {
            cntT[c]++;
        }

        char[] s = S.toCharArray();
        int m = s.length;
        int ansLeft = -1;
        int ansRight = m;
        int left = 0;

        for (int right = 0; right < m; right++) { // 移动子串右端点
            cntS[s[right]]++; // 右端点字母移入子串
            while (isCovered(cntS, cntT)) { // 涵盖
                if (right - left < ansRight - ansLeft) { // 找到更短的子串
                    ansLeft = left; // 记录此时的左右端点
                    ansRight = right;
                }
                cntS[s[left]]--; // 左端点字母移出子串
                left++;
            }
        }

        return ansLeft < 0 ? "" : S.substring(ansLeft, ansRight + 1);
    }

    private boolean isCovered(int[] cntS, int[] cntT) {
        for (int i = 'A'; i <= 'Z'; i++) {
            if (cntS[i] < cntT[i]) {
                return false;
            }
        }
        for (int i = 'a'; i <= 'z'; i++) {
            if (cntS[i] < cntT[i]) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
    bool is_covered(int cnt_s[], int cnt_t[]) {
        for (int i = 'A'; i <= 'Z'; i++) {
            if (cnt_s[i] < cnt_t[i]) {
                return false;
            }
        }
        for (int i = 'a'; i <= 'z'; i++) {
            if (cnt_s[i] < cnt_t[i]) {
                return false;
            }
        }
        return true;
    }

public:
    string minWindow(string s, string t) {
        int cnt_s[128]{}; // s 子串字母的出现次数
        int cnt_t[128]{}; // t 中字母的出现次数
        for (char c : t) {
            cnt_t[c]++;
        }

        int m = s.size();
        int ans_left = -1, ans_right = m;
        int left = 0;

        for (int right = 0; right < m; right++) { // 移动子串右端点
            cnt_s[s[right]]++; // 右端点字母移入子串
            while (is_covered(cnt_s, cnt_t)) { // 涵盖
                if (right - left < ans_right - ans_left) { // 找到更短的子串
                    ans_left = left; // 记录此时的左右端点
                    ans_right = right;
                }
                cnt_s[s[left]]--; // 左端点字母移出子串
                left++;
            }
        }

        return ans_left < 0 ? "" : s.substr(ans_left, ans_right - ans_left + 1);
    }
};
```

```c [sol-C]
bool is_covered(int cnt_s[], int cnt_t[]) {
    for (int i = 'A'; i <= 'Z'; i++) {
        if (cnt_s[i] < cnt_t[i]) {
            return false;
        }
    }
    for (int i = 'a'; i <= 'z'; i++) {
        if (cnt_s[i] < cnt_t[i]) {
            return false;
        }
    }
    return true;
}

char* minWindow(char* s, char* t) {
    int cnt_s[128] = {}; // s 子串字母的出现次数
    int cnt_t[128] = {}; // t 中字母的出现次数
    for (int i = 0; t[i]; i++) {
        cnt_t[t[i]]++;
    }

    int ans_left = -1, ans_right = INT_MAX / 2;
    int left = 0;

    for (int right = 0; s[right]; right++) { // 移动子串右端点
        cnt_s[s[right]]++; // 右端点字母移入子串
        while (is_covered(cnt_s, cnt_t)) { // 涵盖
            if (right - left < ans_right - ans_left) { // 找到更短的子串
                ans_left = left; // 记录此时的左右端点
                ans_right = right;
            }
            cnt_s[s[left]]--; // 左端点字母移出子串
            left++;
        }
    }

    if (ans_left < 0) {
        return "";
    }
    s[ans_right + 1] = '\0';
    return s + ans_left;
}
```

```go [sol-Go]
func isCovered(cntS, cntT []int) bool {
    for i := 'A'; i <= 'Z'; i++ {
        if cntS[i] < cntT[i] {
            return false
        }
    }
    for i := 'a'; i <= 'z'; i++ {
        if cntS[i] < cntT[i] {
            return false
        }
    }
    return true
}

func minWindow(s, t string) string {
    var cntS, cntT [128]int
    for _, c := range t {
        cntT[c]++
    }

    ansLeft, ansRight := -1, len(s)
    left := 0

    for right, c := range s { // 移动子串右端点
        cntS[c]++ // 右端点字母移入子串
        for isCovered(cntS[:], cntT[:]) { // 涵盖
            if right-left < ansRight-ansLeft { // 找到更短的子串
                ansLeft, ansRight = left, right // 记录此时的左右端点
            }
            cntS[s[left]]-- // 左端点字母移出子串
            left++
        }
    }

    if ansLeft < 0 {
        return ""
    }
    return s[ansLeft : ansRight+1]
}
```

```js [sol-JavaScript]
function isCovered(cntS, cntT) {
    for (let i = 'A'.charCodeAt(0); i <= 'Z'.charCodeAt(0); i++) {
        if (cntS[i] < cntT[i]) {
            return false;
        }
    }
    for (let i = 'a'.charCodeAt(0); i <= 'z'.charCodeAt(0); i++) {
        if (cntS[i] < cntT[i]) {
            return false;
        }
    }
    return true;
}

var minWindow = function(s, t) {
    const cntS = Array(128).fill(0); // s 子串字母的出现次数
    const cntT = Array(128).fill(0); // t 中字母的出现次数
    for (const c of t) {
        cntT[c.codePointAt(0)]++;
    }

    const m = s.length;
    let ansLeft = -1, ansRight = m;
    let left = 0;

    for (let right = 0; right < m; right++) { // 移动子串右端点
        cntS[s[right].codePointAt(0)]++; // 右端点字母移入子串
        while (isCovered(cntS, cntT)) { // 涵盖
            if (right - left < ansRight - ansLeft) { // 找到更短的子串
                ansLeft = left; // 记录此时的左右端点
                ansRight = right;
            }
            cntS[s[left].codePointAt(0)]--; // 左端点字母移出子串
            left++;
        }
    }

    return ansLeft < 0 ? "" : s.substring(ansLeft, ansRight + 1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_window(S: String, t: String) -> String {
        fn is_covered(cnt_s: &[i32; 128], cnt_t: &[i32; 128]) -> bool {
            for i in b'A'..=b'Z' {
                if cnt_s[i as usize] < cnt_t[i as usize] {
                    return false;
                }
            }
            for i in b'a'..=b'z' {
                if cnt_s[i as usize] < cnt_t[i as usize] {
                    return false;
                }
            }
            true
        }

        let mut cnt_s = [0; 128]; // s 子串字母的出现次数
        let mut cnt_t = [0; 128]; // t 中字母的出现次数
        for c in t.bytes() {
            cnt_t[c as usize] += 1;
        }

        let s = S.as_bytes();
        let m = s.len();
        let mut ans_left = 0;
        let mut ans_right = m;
        let mut left = 0;

        for (right, &c) in s.iter().enumerate() { // 移动子串右端点
            cnt_s[c as usize] += 1; // 右端点字母移入子串
            while is_covered(&cnt_s, &cnt_t) { // 涵盖
                if right - left < ans_right - ans_left { // 找到更短的子串
                    ans_left = left; // 记录此时的左右端点
                    ans_right = right;
                }
                cnt_s[s[left] as usize] -= 1; // 左端点字母移出子串
                left += 1;
            }
        }

        if ans_right < m {
            S[ans_left..=ans_right].to_string()
        } else {
            String::new()
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(|\Sigma|m+n)$，其中 $m$ 是 $\textit{s}$ 的长度，$n$ 是 $\textit{t}$ 的长度，$|\Sigma|$ 是字符集合的大小，本题字符均为英文字母，所以 $|\Sigma|=52$。注意 $\textit{left}$ 只会增加不会减少，$\textit{left}$ 每增加一次，我们就花费 $\mathcal{O}(|\Sigma|)$ 的时间。因为 $\textit{left}$ 至多增加 $m$ 次，所以二重循环的时间复杂度为 $\mathcal{O}(|\Sigma|m)$，再算上统计 $t$ 字母出现次数的时间 $\mathcal{O}(n)$，总的时间复杂度为 $\mathcal{O}(|\Sigma|m+n)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。如果创建了大小为 $128$ 的数组，则 $|\Sigma|=128$。

## 优化

上面的代码每次都要花费 $\mathcal{O}(|\Sigma|)$ 的时间去判断是否涵盖，能不能优化到 $\mathcal{O}(1)$ 呢？

可以。用一个变量 $\textit{geCnt}$ 维护目前子串（窗口）中有 $\textit{geCnt}$ 种字母的出现次数大于等于 $t$ 中相应字母的出现次数。

设 $t$ 有 $\textit{kinds}$ 个不同的字母，那么「子串每种字母的出现次数都大于等于 $t$ 中相应字母的出现次数」等价于 $\textit{geCnt} = \textit{kinds}$。

如何维护 $\textit{geCnt}$ 呢？

为了方便实现，把 $\textit{cntS}$ 和 $\textit{cntT}$ 合并成一个 $\textit{diff}$，定义 $\textit{diff}[x] = \textit{cntS}[x] - \textit{cntT}[x]$。如果 $\textit{diff}[x] = 0$，就意味着窗口内字母 $x$ 的出现次数和 $t$ 的一样多。

- 如果字母 $x$ 进入窗口**后**，$\textit{diff}[x] = 0$，这意味着 $x$ 在子串和 $t$ 中的出现次数从 $<$ 变成了 $\ge$，那么把 $\textit{geCnt}$ 增加一。
- 如果字母 $x$ 离开窗口**前**，$\textit{diff}[x] = 0$，这意味着 $x$ 离开窗口后，$x$ 在子串和 $t$ 中的出现次数从 $\ge$ 变成了 $<$，那么把 $\textit{geCnt}$ 减少一。

⚠**注意**：不能在 $\textit{diff}[x] \ge 0$ 的时候就把 $\textit{geCnt}$ 增加一。这样写的话，对于同一个字母 $x$，$\textit{diff}[x]$ 等于 $0,1,2,\ldots$ 的时候都会让 $\textit{geCnt}$ 增加一，这就重复统计了。

```py [sol-Python3]
class Solution:
    def minWindow(self, s: str, t: str) -> str:
        # 注：defaultdict 比 Counter 快
        diff = defaultdict(int)  # 窗口每种字母个数 - t 每种字母个数
        for c in t:
            diff[c] -= 1
        kinds = len(diff)  # t 中有 kinds 种不同的字母

        ans_left, ans_right = -1, len(s)
        ge_cnt = 0  # 窗口内有 ge_cnt 种字母的出现次数 >= t 中相应字母的出现次数
        left = 0

        for right, c in enumerate(s):  # 移动子串右端点
            diff[c] += 1  # 右端点字母移入子串
            if diff[c] == 0:  # 原来窗口内 c 的出现次数比 t 的少，现在一样多
                ge_cnt += 1  # 从 < 变成 >=

            while ge_cnt == kinds:  # 涵盖：所有字母的出现次数都是 >=
                if right - left < ans_right - ans_left:  # 找到更短的子串
                    ans_left, ans_right = left, right  # 记录此时的左右端点

                x = s[left]  # 左端点字母
                if diff[x] == 0:
                    # x 移出窗口之前，检查出现次数，
                    # 如果窗口内 x 的出现次数和 t 一样，
                    # 那么 x 移出窗口后，窗口内 x 的出现次数比 t 的少
                    ge_cnt -= 1  # 从 >= 变成 <
                diff[x] -= 1  # 左端点字母移出子串
                left += 1

        return "" if ans_left < 0 else s[ans_left: ans_right + 1]
```

```java [sol-Java]
class Solution {
    public String minWindow(String S, String t) {
        int[] diff = new int[128]; // 窗口每种字母个数 - t 每种字母个数
        int kinds = 0;
        for (char c : t.toCharArray()) {
            if (diff[c] == 0) {
                kinds++; // 统计 t 有多少个不同的字母
            }
            diff[c]--;
        }

        char[] s = S.toCharArray();
        int m = s.length;
        int ansLeft = -1;
        int ansRight = m;
        int geCnt = 0; // 窗口内有 geCnt 种字母的出现次数 >= t 中相应字母的出现次数
        int left = 0;

        for (int right = 0; right < m; right++) { // 移动子串右端点
            char c = s[right]; // 右端点字母
            diff[c]++; // 右端点字母移入子串
            if (diff[c] == 0) { // 原来窗口内 c 的出现次数比 t 的少，现在一样多
                geCnt++; // 从 < 变成 >=
            }

            while (geCnt == kinds) { // 涵盖：所有字母的出现次数都是 >=
                if (right - left < ansRight - ansLeft) { // 找到更短的子串
                    ansLeft = left; // 记录此时的左右端点
                    ansRight = right;
                }

                char x = s[left]; // 左端点字母
                if (diff[x] == 0) {
                    // x 移出窗口之前，检查出现次数，
                    // 如果窗口内 x 的出现次数和 t 一样，
                    // 那么 x 移出窗口后，窗口内 x 的出现次数比 t 的少
                    geCnt--; // 从 >= 变成 <
                }
                diff[x]--; // 左端点字母移出子串
                left++;
            }
        }

        return ansLeft < 0 ? "" : S.substring(ansLeft, ansRight + 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string minWindow(string s, string t) {
        int diff[128]{}; // 窗口每种字母个数 - t 每种字母个数
        int kinds = 0;
        for (char c : t) {
            if (diff[c] == 0) {
                kinds++; // 统计 t 有多少个不同的字母
            }
            diff[c]--;
        }

        int m = s.size();
        int ans_left = -1, ans_right = m;
        int ge_cnt = 0; // 窗口内有 ge_cnt 种字母的出现次数 >= t 中相应字母的出现次数
        int left = 0;

        for (int right = 0; right < m; right++) { // 移动子串右端点
            char c = s[right]; // 右端点字母
            diff[c]++; // 右端点字母移入子串
            if (diff[c] == 0) { // 原来窗口内 c 的出现次数比 t 的少，现在一样多
                ge_cnt++; // 从 < 变成 >=
            }

            while (ge_cnt == kinds) { // 涵盖：所有字母的出现次数都是 >=
                if (right - left < ans_right - ans_left) { // 找到更短的子串
                    ans_left = left; // 记录此时的左右端点
                    ans_right = right;
                }

                char x = s[left]; // 左端点字母
                if (diff[x] == 0) {
                    // x 移出窗口之前，检查出现次数，
                    // 如果窗口内 x 的出现次数和 t 一样，
                    // 那么 x 移出窗口后，窗口内 x 的出现次数比 t 的少
                    ge_cnt--; // 从 >= 变成 <
                }
                diff[x]--; // 左端点字母移出子串
                left++;
            }
        }

        return ans_left < 0 ? "" : s.substr(ans_left, ans_right - ans_left + 1);
    }
};
```

```c [sol-C]
char* minWindow(char* s, char* t) {
    int diff[128] = {}; // 窗口每种字母个数 - t 每种字母个数
    int kinds = 0;
    for (int i = 0; t[i]; i++) {
         char c = t[i];
         if (diff[c] == 0) {
             kinds++; // 统计 t 有多少个不同的字母
         }
         diff[c]--;
    }

    int ans_left = -1, ans_right = INT_MAX / 2;
    int ge_cnt = 0; // 窗口内有 ge_cnt 种字母的出现次数 >= t 中相应字母的出现次数
    int left = 0;

    for (int right = 0; s[right]; right++) { // 移动子串右端点
        char c = s[right]; // 右端点字母
        diff[c]++; // 右端点字母移入子串
        if (diff[c] == 0) { // 原来窗口内 c 的出现次数比 t 的少，现在一样多
            ge_cnt++; // 从 < 变成 >=
        }

        while (ge_cnt == kinds) { // 涵盖：所有字母的出现次数都是 >=
            if (right - left < ans_right - ans_left) { // 找到更短的子串
                ans_left = left; // 记录此时的左右端点
                ans_right = right;
            }

            char x = s[left]; // 左端点字母
            if (diff[x] == 0) {
                // x 移出窗口之前，检查出现次数，
                // 如果窗口内 x 的出现次数和 t 一样，
                // 那么 x 移出窗口后，窗口内 x 的出现次数比 t 的少
                ge_cnt--; // 从 >= 变成 <
            }
            diff[x]--; // 左端点字母移出子串
            left++;
        }
    }

    if (ans_left < 0) {
        return "";
    } 
    s[ans_right + 1] = '\0';
    return s + ans_left;
}
```

```go [sol-Go]
func minWindow(s, t string) string {
    diff := [128]int{} // 窗口每种字母个数 - t 每种字母个数
    kinds := 0
    for _, c := range t {
        if diff[c] == 0 {
            kinds++ // 统计 t 有多少个不同的字母
        }
        diff[c]--
    }

    ansLeft, ansRight := -1, len(s)
    geCnt := 0 // 窗口内有 geCnt 种字母的出现次数 >= t 中相应字母的出现次数
    left := 0

    for right, c := range s { // 移动子串右端点
        diff[c]++ // 右端点字母移入子串
        if diff[c] == 0 { // 原来窗口内 c 的出现次数比 t 的少，现在一样多
            geCnt++ // 从 < 变成 >=
        }

        for geCnt == kinds { // 涵盖：所有字母的出现次数都是 >=
            if right-left < ansRight-ansLeft { // 找到更短的子串
                ansLeft = left // 记录此时的左右端点
                ansRight = right
            }

            x := s[left] // 左端点字母
            if diff[x] == 0 {
                // x 移出窗口之前，检查出现次数，
                // 如果窗口内 x 的出现次数和 t 一样，
                // 那么 x 移出窗口后，窗口内 x 的出现次数比 t 的少
                geCnt-- // 从 >= 变成 <
            }
            diff[x]-- // 左端点字母移出子串
            left++
        }
    }

    if ansLeft < 0 {
        return ""
    }
    return s[ansLeft : ansRight+1]
}
```

```js [sol-JavaScript]
var minWindow = function(s, t) {
    const diff = Array(128).fill(0); // 窗口每种字母个数 - t 每种字母个数
    let kinds = 0;
    for (let c of t) {
        c = c.codePointAt(0);
        if (diff[c] === 0) {
            kinds++; // 统计 t 有多少个不同的字母
        }
        diff[c]--;
    }

    const m = s.length;
    let ansLeft = -1, ansRight = m;
    let geCnt = 0; // 窗口内有 geCnt 种字母的出现次数 >= t 中相应字母的出现次数
    let left = 0;

    for (let right = 0; right < m; right++) { // 移动子串右端点
        const c = s[right].codePointAt(0); // 右端点字母
        diff[c]++; // 右端点字母移入子串
        if (diff[c] === 0) { // 原来窗口内 c 的出现次数比 t 的少，现在一样多
            geCnt++; // 从 < 变成 >=
        }

        while (geCnt === kinds) { // 涵盖：所有字母的出现次数都是 >=
            if (right - left < ansRight - ansLeft) { // 找到更短的子串
                ansLeft = left; // 记录此时的左右端点
                ansRight = right;
            }

            const x = s[left].codePointAt(0); // 左端点字母
            if (diff[x] === 0) {
                // x 移出窗口之前，检查出现次数，
                // 如果窗口内 x 的出现次数和 t 一样，
                // 那么 x 移出窗口后，窗口内 x 的出现次数比 t 的少
                geCnt--; // 从 >= 变成 <
            }
            diff[x]--; // 左端点字母移出子串
            left++;
        }
    }

    return ansLeft < 0 ? "" : s.substring(ansLeft, ansRight + 1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_window(S: String, t: String) -> String {
        let mut diff = [0; 128]; // 窗口每种字母个数 - t 每种字母个数
        let mut kinds = 0;
        for c in t.bytes() {
            let c = c as usize;
            if diff[c] == 0 {
                kinds += 1; // 统计 t 有多少个不同的字母
            }
            diff[c] -= 1;
        }

        let s = S.as_bytes();
        let m = s.len();
        let mut ans_left = 0;
        let mut ans_right = m;
        let mut ge_cnt = 0; // 窗口内有 ge_cnt 种字母的出现次数 >= t 中相应字母的出现次数
        let mut left = 0;

        for (right, &c) in s.iter().enumerate() { // 移动子串右端点
            let c = c as usize;
            diff[c] += 1; // 右端点字母移入子串
            if diff[c] == 0 { // 原来窗口内 c 的出现次数比 t 的少，现在一样多
                ge_cnt += 1; // 从 < 变成 >=
            }

            while ge_cnt == kinds { // 涵盖：所有字母的出现次数都是 >=
                if right - left < ans_right - ans_left { // 找到更短的子串
                    ans_left = left; // 记录此时的左右端点
                    ans_right = right;
                }

                let x = s[left] as usize; // 左端点字母
                if diff[x] == 0 {
                    // x 移出窗口之前，检查出现次数，
                    // 如果窗口内 x 的出现次数和 t 一样，
                    // 那么 x 移出窗口后，窗口内 x 的出现次数比 t 的少
                    ge_cnt -= 1; // 从 >= 变成 <
                }
                diff[x] -= 1; // 左端点字母移出子串
                left += 1;
            }
        }

        if ans_right < m {
            S[ans_left..=ans_right].to_string()
        } else {
            String::new()
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n)$ 或 $\mathcal{O}(m+n+|\Sigma|)$，其中 $m$ 是 $\textit{s}$ 的长度，$n$ 是 $\textit{t}$ 的长度，$|\Sigma|=128$。注意 $\textit{left}$ 只会增加不会减少，二重循环的时间复杂度为 $\mathcal{O}(m)$。使用哈希表写法的时间复杂度为 $\mathcal{O}(m+n)$，数组写法的时间复杂度为 $\mathcal{O}(m+n+|\Sigma|)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。无论 $m$ 和 $n$ 有多大，额外空间都不会超过 $\mathcal{O}(|\Sigma|)$。

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

本题来自 `二、不定长滑动窗口 / §2.2 越长越合法/求最短/最小`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.2 越长越合法/求最短/最小`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
