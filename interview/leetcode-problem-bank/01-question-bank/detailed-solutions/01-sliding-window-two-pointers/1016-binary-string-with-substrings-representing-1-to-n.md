# 1016. 子串能表示从 1 到 N 数字的二进制串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-string-with-substrings-representing-1-to-n/
- 题目 slug：`binary-string-with-substrings-representing-1-to-n`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-string-with-substrings-representing-1-to-n/solutions/2265097/san-chong-suan-fa-cong-bao-li-dao-you-hu-nmtq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种算法：从暴力到优化（Python/Java/C++/Go）](https://leetcode.cn/problems/binary-string-with-substrings-representing-1-to-n/solutions/2265097/san-chong-suan-fa-cong-bao-li-dao-you-hu-nmtq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-suan-fa-cong-bao-li-dao-you-hu-nmtq`
- topic id：`2265097`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 算法一

从小到大枚举 $[1,n]$ 内的数，转成二进制字符串，判断是否都在 $s$ 中。一旦枚举到不在 $s$ 中的数，就立刻返回 `false`。如果都在 $s$ 中就返回 `true`。

等等，$n$ 不是高达 $10^9$ 吗？为什么这样做不会超时？

请继续阅读。

```py [sol1-Python3]
class Solution:
    def queryString(self, s: str, n: int) -> bool:
        return all(bin(i)[2:] in s for i in range(1, n + 1))
```

```java [sol1-Java]
class Solution {
    public boolean queryString(String s, int n) {
        for (int i = 1; i <= n; i++)
            if (!s.contains(Integer.toBinaryString(i)))
                return false;
        return true;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    bool queryString(string s, int n) {
        for (int i = 1; i <= n; i++) {
            auto bin = bitset<32>(i).to_string();
            bin = bin.substr(bin.find('1'));
            if (s.find(bin) == string::npos)
                return false;
        }
        return true;
    }
};
```

```go [sol1-Go]
func queryString(s string, n int) bool {
    for i := 1; i <= n; i++ {
        if !strings.Contains(s, strconv.FormatUint(uint64(i), 2)) {
            return false
        }
    }
    return true
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\min(m,n)\cdot m\log \min(m,n))$，其中 $m$ 为 $s$ 的长度。由下面的「精确分析」可知，至多循环 $\mathcal{O}(\min(m,n))$ 次，所以二进制长度为 $\mathcal{O}(\log\min(m,n))$，每次在 $s$ 中查找需要 $\mathcal{O}(m\log \min(m,n))$ 的时间，所以总的时间复杂度为 $\mathcal{O}(\min(m,n)\cdot m\log \min(m,n))$。
- 空间复杂度：$\mathcal{O}(\log n)$。把数字转成二进制数需要 $\mathcal{O}(\log n)$ 的空间。

## 算法二

反过来想，把 $s$ 的子串都转成二进制数，如果数字在 $[1,n]$ 内，就保存到一个哈希表中。如果哈希表的大小最终为 $n$，就说明 $[1,n]$ 的二进制都在 $s$ 里面。

代码实现时，设当前枚举的子串对应的下标区间为 $[i,j]$，手动把这段子串转成二进制数。这里的技巧是，设当前得到的二进制数为 $x$，且下一个字符 $s[j+1]$ 为 $c$，那么将 $x$ 更新为 `(x << 1) | (c - '0')`，从而 $\mathcal{O}(1)$ 地计算出子串 $[i,j+1]$ 对应的的二进制数。

此外，可以跳过 $s[i]=\text{0}$ 的情况。例如 $s=\text{0110}$，从 $s[0]$ 开始和从 $s[1]$ 开始，得到的二进制数都是一样的。并且，由于保证从 $s[i]=\text{1}$ 开始枚举，二进制数的大小会指数增长，一旦 $x>n$，就停止枚举 $j$。所以对于固定的 $i$，至多枚举 $\mathcal{O}(\log n)$ 个 $j$。

```py [sol2-Python3]
class Solution:
    def queryString(self, s: str, n: int) -> bool:
        seen = set()
        s = list(map(int, s))  # 把 s[i] 全部转成 int
        for i, x in enumerate(s):
            if x == 0: continue  # 二进制数从 1 开始
            j = i + 1  # 下一个字符的下标
            while x <= n:
                seen.add(x)
                if j == len(s): break
                x = (x << 1) | s[j]  # 子串 s[i:j+1] 的二进制数
                j += 1
        return len(seen) == n
```

```java [sol2-Java]
class Solution {
    public boolean queryString(String S, int n) {
        var seen = new HashSet<Integer>();
        var s = S.toCharArray();
        for (int i = 0, m = s.length; i < m; ++i) {
            int x = s[i] - '0';
            if (x == 0) continue; // 二进制数从 1 开始
            for (int j = i + 1; x <= n; j++) {
                seen.add(x);
                if (j == m) break;
                x = (x << 1) | (s[j] - '0'); // 子串 [i,j] 的二进制数
            }
        }
        return seen.size() == n;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    bool queryString(string s, int n) {
        unordered_set<int> seen;
        for (int i = 0, m = s.length(); i < m; ++i) {
            int x = s[i] - '0';
            if (x == 0) continue; // 二进制数从 1 开始
            for (int j = i + 1; x <= n; j++) {
                seen.insert(x);
                if (j == m) break;
                x = (x << 1) | (s[j] - '0'); // 子串 [i,j] 的二进制数
            }
        }
        return seen.size() == n;
    }
};
```

```go [sol2-Go]
func queryString(s string, n int) bool {
    seen := map[int]struct{}{}
    for i, b := range s {
        x := int(b - '0')
        if x == 0 { // 二进制数从 1 开始
            continue
        }
        for j := i + 1; x <= n; j++ {
            seen[x] = struct{}{}
            if j == len(s) {
                break
            }
            x = x<<1 | int(s[j]-'0') // 子串 s[i:j+1] 的二进制数
        }
    }
    return len(seen) == n
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log n)$，其中 $m$ 为 $s$ 的长度。对于固定的 $i$，内层循环的次数为 $\mathcal{O}(\log n)$，所以总的循环次数为 $\mathcal{O}(m\log n)$。
- 空间复杂度：$\mathcal{O}(\min(m\log n, n))$，即哈希表所需要的空间。

> 注：如果调用库函数去转成二进制数，时间复杂度是 $\mathcal{O}(m\log^2 n)$。

## 精确分析：为什么「算法一」不会超时？

举例说明。如果 $n=7$，单看闭区间 $[4,7]$，有 $4$ 个互不相同的整数，它们的二进制长度均为 $3$。如果要让字符串 $s$ 包含这 $4$ 个数，$s$ 中至少要有 $4$ 个长为 $3$ 的互不相同的子串。考虑到这些子串可以有重叠部分，设 $s$ 的长度为 $m$，则应满足 $m\ge 3 + (4-1) = 6$，否则直接返回 `false`。（想象一个长为 $3$ 的滑动窗口在 $s$ 中滑动，至少要得到 $4$ 个子串。）

随着 $n$ 的变大，$m$ 的长度也应当随之变大。本题 $m$ 至多为 $1000$，而 $n$ 却高达 $10^9$。可以预见，当 $n$ 较大时，可以直接返回 `false`。如何精确地判断呢？

考虑到 $n+1$ 不一定是 $2$ 的幂，分两组讨论。

设 $n$ 的二进制长度为 $k+1$，那么：

- 区间 $[2^k, n]$ 内的二进制数的长度均为 $k+1$，这有 $n-2^k+1$ 个，所以应满足 $m\ge k+1+(n-2^k+1-1) = n-2^k+k+1$。
- 区间 $[2^{k-1},2^k-1]$ 内的二进制数的长度均为 $k$，这有 $2^{k-1}$ 个，所以应满足 $m\ge k+(2^{k-1}-1) = 2^{k-1}+k-1$。
- 注意，当 $n=1$ 时，$k=0$，此时区间 $[2^{k-1},2^k-1]$ 不存在。直接特判这种情况，返回 $s$ 是否包含 $1$。

> 更小的区间呢？
>
> 注意到，把区间 $[2^{k-1},2^k-1]$ 内的所有数字都右移一位，可以得到更小的区间 $[2^{k-2},2^{k-1}-1]$ 内的所有数字。
>
> 如果 $s$ 包含 $[2^{k-1},2^k-1]$ 内的所有二进制数，那么必然也包含 $[2^{k-2},2^{k-1}-1]$ 内的所有二进制数，对于更小的区间，也同样包含。
>
> 所以只需要考虑 $s$ 是否包含 $[2^{k-1},2^k-1]$ 和 $[2^k, n]$ 内的所有二进制数即可。

因此，如果

$$
m < \max(n-2^k+k+1,2^{k-1}+k-1)
$$

可以直接返回 `false`。

上式右边约为 $n/2$。换句话说，算法一的循环次数至多约为 $2m$。（对此有疑问的话，可以用反证法证明。）

> 注：由于 $m\le 1000$，根据上式，如果 $n\ge 2014$，可以直接返回 `false`。但不推荐这样写，根据 $n$ 和 $m$ 的值来判断更准确。

## 算法三

按照上面的分析：

1. 根据 $n$ 和 $m$ 的值来提前判断是否要返回 `false`。
2. 只需要考虑长为 $k$ 和 $k+1$ 的这两组二进制数 $s$ 是否都有，因此可以用长为 $k$ 和 $k+1$ 的滑动窗口实现，从而做到线性时间复杂度。
3. 进一步地，由于区间 $[2^k,n]$ 内的所有数右移一位可以得到区间 $[2^{k-1},\lfloor n/2\rfloor]$，所以对于 $[2^{k-1},2^k-1]$，只需从 $\lfloor n/2\rfloor+1$ 开始考虑。

```py [sol3-Python3]
class Solution:
    def queryString(self, s: str, n: int) -> bool:
        if n == 1:
            return '1' in s

        m = len(s)
        k = n.bit_length() - 1
        if m < max(n - (1 << k) + k + 1, (1 << (k - 1)) + k - 1):
            return False

        # 对于长为 k 的在 [lower, upper] 内的二进制数，判断这些数 s 是否都有
        def check(k: int, lower: int, upper: int) -> bool:
            if lower > upper: return True
            seen = set()
            mask = (1 << (k - 1)) - 1
            x = int(s[:k - 1], 2)
            for c in s[k - 1:]:
                # & mask 可以去掉最高比特位，从而实现滑窗的「出」
                # << 1 | int(c) 即为滑窗的「入」
                x = ((x & mask) << 1) | int(c)
                if lower <= x <= upper:
                    seen.add(x)
            return len(seen) == upper - lower + 1

        return check(k, n // 2 + 1, (1 << k) - 1) and check(k + 1, 1 << k, n)
```

```java [sol3-Java]
class Solution {
    public boolean queryString(String s, int n) {
        if (n == 1)
            return s.contains("1");

        int k = 31 - Integer.numberOfLeadingZeros(n); // n 的二进制长度减一
        if (s.length() < Math.max(n - (1 << k) + k + 1, (1 << (k - 1)) + k - 1))
            return false;

        return check(s, k, n / 2 + 1, (1 << k) - 1) && check(s, k + 1, 1 << k, n);
    }

    // 对于长为 k 的在 [lower, upper] 内的二进制数，判断这些数 s 是否都有
    private boolean check(String s, int k, int lower, int upper) {
        if (lower > upper) return true;
        var seen = new HashSet<Integer>();
        int mask = (1 << (k - 1)) - 1;
        int x = Integer.parseInt(s.substring(0, k - 1), 2);
        for (int i = k - 1, m = s.length(); i < m; i++) {
            // & mask 可以去掉最高比特位，从而实现滑窗的「出」
            // << 1 | (s.charAt(i) - '0') 即为滑窗的「入」
            x = ((x & mask) << 1) | (s.charAt(i) - '0');
            if (lower <= x && x <= upper)
                seen.add(x);
        }
        return seen.size() == upper - lower + 1;
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    bool queryString(string s, int n) {
        if (n == 1)
            return s.find('1') != string::npos;

        int m = s.length();
        int k = 31 - __builtin_clz(n); // n 的二进制长度减一
        if (m < max(n - (1 << k) + k + 1, (1 << (k - 1)) + k - 1))
            return false;

        // 对于长为 k 的在 [lower, upper] 内的二进制数，判断这些数 s 是否都有
        auto check = [&](int k, int lower, int upper) -> bool {
            if (lower > upper) return true;
            unordered_set<int> seen;
            int mask = (1 << (k - 1)) - 1;
            int x = stoi(s.substr(0, k - 1), nullptr, 2);
            for (int i = k - 1; i < m; i++) {
                // & mask 可以去掉最高比特位，从而实现滑窗的「出」
                // << 1 | (s[i] - '0') 即为滑窗的「入」
                x = ((x & mask) << 1) | (s[i] - '0');
                if (lower <= x && x <= upper)
                    seen.insert(x);
            }
            return seen.size() == upper - lower + 1;
        };

        return check(k, n / 2 + 1, (1 << k) - 1) && check(k + 1, 1 << k, n);
    }
};
```

```go [sol3-Go]
func queryString(s string, n int) bool {
    if n == 1 {
        return strings.Contains(s, "1")
    }

    m := len(s)
    k := bits.Len(uint(n)) - 1
    if m < max(n-1<<k+k+1, 1<<(k-1)+k-1) {
        return false
    }

    // 对于长为 k 的在 [lower, upper] 内的二进制数，判断这些数 s 是否都有
    check := func(k, lower, upper int) bool {
        if lower > upper {
            return true
        }
        seen := map[int]struct{}{}
        mask := 1<<(k-1) - 1
        v, _ := strconv.ParseUint(s[:k-1], 2, 64)
        x := int(v)
        for _, c := range s[k-1:] {
            // &mask 可以去掉最高比特位，从而实现滑窗的「出」
            // <<1 | int(c-'0') 即为滑窗的「入」
            x = x&mask<<1 | int(c-'0')
            if lower <= x && x <= upper {
                seen[x] = struct{}{}
            }
        }
        return len(seen) == upper-lower+1
    }

    return check(k, n/2+1, 1<<k-1) && check(k+1, 1<<k, n)
}

func max(a, b int) int { if a < b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m)$，其中 $m$ 为 $s$ 的长度。滑动窗口的时间复杂度为 $\mathcal{O}(m)$。
- 空间复杂度：$\mathcal{O}(\min(m,n))$，即哈希表所需要的空间。

---

[往期每日一题题解](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~

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
