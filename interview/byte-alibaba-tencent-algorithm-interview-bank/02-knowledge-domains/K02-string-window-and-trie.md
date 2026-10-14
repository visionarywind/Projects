# K02 字符串、窗口统计、Trie 与字符串匹配

## 核心问题

字符集、子串、字典树、KMP/Rolling Hash、编辑距离入口。学习目标是能识别题目中的结构、选择合适算法、证明复杂度并写出边界正确的 C++17 实现。

本域围绕"如何高效表示和比较字符串"展开：滑动窗口统计窗口内字符状态；Trie 用前缀共享把大量字符串查询降到 O(串长)；KMP/Z 算法用"已匹配信息"避免回溯；字符串哈希用整数指纹 O(1) 比较子串。

## 训练顺序

1. 先完成对应入门题，建立术语和模板。
2. 再完成中等级变体，训练多约束组合。
3. 高级和专家题用于训练证明、在线约束、工程边界和追问。

## 核心模式

### 模式 1：滑动窗口字符计数

维护窗口 `[l, r]` 内每个字符的频次，配合一个"当前窗口是否合法"的判据（如：重复字符数、已满足的字符种类数）。窗口收缩时同步减少频次。

```cpp
#include <bits/stdc++.h>
using namespace std;

// 最长无重复字符子串：窗口内字符互异
int lengthOfLongestSubstring(string s) {
    array<int, 128> last;
    last.fill(-1);                         // 字符 -> 最近下标（ASCII）
    int l = 0, ans = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        unsigned char c = s[r];
        if (last[c] >= l) l = last[c] + 1; // 重复字符在窗口内，收缩
        last[c] = r;
        ans = max(ans, r - l + 1);
    }
    return ans;
}
```

**不变量**：每轮结束时窗口 `[l, r]` 内无重复字符；`l` 单调不减，总时间 O(n)。若字符集非 ASCII，把 `array<int,128>` 换成 `unordered_map<char,int>`。

### 模式 2：Trie（字典树）插入与查询

Trie 用树上的边表示字符，从根到某节点的路径即一个前缀。插入与查询均与串长成正比，与词表规模无关，适合前缀检索、单词拼写、最大异或对（按二进制建树）。

```cpp
struct Trie {
    struct Node {
        int ch[26];                        // 子节点下标，-1 表示无
        bool end = false;                  // 是否为某单词结尾
        Node() { fill(begin(ch), end(ch), -1); }
    };
    vector<Node> t{Node()};                // 0 号为根

    void insert(const string& w) {
        int p = 0;
        for (char c : w) {
            int k = c - 'a';
            if (t[p].ch[k] == -1) {
                t[p].ch[k] = t.size();
                t.push_back(Node());
            }
            p = t[p].ch[k];
        }
        t[p].end = true;
    }
    bool search(const string& w) const {   // 整词匹配
        int p = 0;
        for (char c : w) {
            int k = c - 'a';
            if (t[p].ch[k] == -1) return false;
            p = t[p].ch[k];
        }
        return t[p].end;
    }
    bool startsWith(const string& pre) const {
        int p = 0;
        for (char c : pre) {
            int k = c - 'a';
            if (t[p].ch[k] == -1) return false;
            p = t[p].ch[k];
        }
        return true;
    }
};
```

空间 O(总字符数 × 字符集)。用 `array` 定长数组比 `map` 更快，但字符集大时浪费空间，可用哈希子节点折中。

### 模式 3：KMP（前缀函数）

KMP 的核心是 `pi[i]`：`pattern[0..i]` 的最长**真前缀**同时是其**后缀**的长度。失配时按 `pi` 回退，主串指针 `i` 永不回退，故匹配 O(n+m)。

```cpp
vector<int> prefixFunction(const string& s) {
    int n = s.size();
    vector<int> pi(n, 0);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;                         // 不变量：pi[i] 为最长 border 长度
    }
    return pi;
}
vector<int> kmpSearch(const string& text, const string& pat) {
    if (pat.empty()) return {};
    string cur = pat + '\x01' + text;      // 分隔符不在字符集内
    vector<int> pi = prefixFunction(cur);
    vector<int> res;
    int m = pat.size();
    for (int i = m + 1; i < (int)cur.size(); ++i)
        if (pi[i] == m) res.push_back(i - 2 * m);
    return res;
}
```

分隔符必须不属于 `pat` 的字符集（否则边界匹配会串扰），这是实现层面的常见坑。Z 算法是等价思路：`z[i]` 为 `s` 与 `s[i..]` 的最长公共前缀，用 Z-box `[l, r]` 保持线性。

### 模式 4：字符串哈希（Rolling Hash）

用多项式哈希把子串映射为整数，实现 O(1) 子串比较。取模双哈希可把碰撞概率压到可忽略。

```cpp
struct RollingHash {
    static const long long M1 = 1000000007, M2 = 1000000009, B = 131;
    vector<long long> h1, h2, p1, p2;
    explicit RollingHash(const string& s) : h1(s.size()+1,0), h2(s.size()+1,0),
                                             p1(s.size()+1,1), p2(s.size()+1,1) {
        for (int i = 0; i < (int)s.size(); ++i) {
            h1[i+1] = (h1[i] * B + s[i]) % M1;  p1[i+1] = p1[i] * B % M1;
            h2[i+1] = (h2[i] * B + s[i]) % M2;  p2[i+1] = p2[i] * B % M2;
        }
    }
    pair<long long,long long> get(int l, int r) const {  // 子串 [l, r)
        long long a = (h1[r] - h1[l]*p1[r-l]) % M1; if (a < 0) a += M1;
        long long b = (h2[r] - h2[l]*p2[r-l]) % M2; if (b < 0) b += M2;
        return {a, b};
    }
};
```

**要点**：`B` 应大于字符集且与模数互质；减法后必须归一化到非负；单哈希存在被构造攻击的碰撞风险，竞赛/面试稳妥做法是双哈希或随机底数。

## 常见陷阱

- **KMP 分隔符冲突**：拼接 `pat + sep + text` 时，`sep` 必须不出现在 `pat` 中，否则会匹配出错误位置。
- **`prefixFunction` 从 i=1 起**：`pi[0]` 恒为 0；回退用 `j = pi[j-1]` 而非 `pi[j]`，差一位就会死循环或越界。
- **Trie 忘记 `end` 标记**：只判断路径存在会把前缀误判为整词（`search` 返回 true 而实际只存了更长单词）。
- **Trie 节点数组初始化**：`ch` 未全部置 -1 时，下标 0 被误当成有效子节点。
- **哈希取模为负**：C++ 中负数取模仍为负，比较前必须 `(x % M + M) % M`。
- **单哈希被卡**：底数固定且模数已知时，存在碰撞构造；用双哈希或随机底数。
- **ASCII 数组越界**：`array<int,128>` 遇到非 ASCII 字符会越界，需转 `unordered_map` 或先处理编码。
- **滑动窗口 `l = last+1` 与 `l = max(l, last+1)`**：不取 max 会让 `l` 回退，破坏单调性。

## 复杂度速查

| 模式 | 预处理 | 单次查询/匹配 | 空间 | 关键前提 |
|---|---|---|---|---|
| 滑窗字符计数 | — | O(n) 整体 | O(字符集) | 窗口状态可增量维护 |
| Trie 插入/查询 | O(总字符数) | O(串长) | O(总字符数 × 字符集) | 字符集有限 |
| KMP | O(m) 建 pi | O(n+m) | O(m) | 前缀函数 |
| Z 算法 | O(n+m) | O(n+m) | O(n+m) | Z-box 不回退 |
| Rolling Hash | O(n) | O(1) 比较 | O(n) | 模数为大素数，双哈希 |

## 题目索引

- `../01-question-bank/intermediate/I002-longest-substring-no-repeat.md`（最长无重复子串：滑窗字符计数）

跨域相关题：`K01`（滑动窗口通用模板）、`E002`（当"字符串"退化为"字符串集合的计数"时用哈希计数）。

**证据与来源：** `[已确认]` KMP 前缀函数、Z 算法、Trie 与多项式哈希均为公开经典算法结论；`[推断]` 训练顺序按难度归纳。样例未在目标 OJ 逐例运行，标记 `[待验证]`。
