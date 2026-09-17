# 3008. 找出数组中的美丽下标 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-beautiful-indices-in-the-given-array-ii/
- 题目 slug：`find-beautiful-indices-in-the-given-array-ii`
- 来源专题：字符串
- 来源分类路径：一、KMP（前缀的后缀）
- 难度分：2016
- 外部题解来源：https://leetcode.cn/problems/find-beautiful-indices-in-the-given-array-ii/solutions/2603695/kmper-fen-cha-zhao-by-endlesscheng-7bjm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种写法：KMP+二分查找/双指针（Python/Java/C++/Go）](https://leetcode.cn/problems/find-beautiful-indices-in-the-given-array-ii/solutions/2603695/kmper-fen-cha-zhao-by-endlesscheng-7bjm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kmper-fen-cha-zhao-by-endlesscheng-7bjm`
- topic id：`2603695`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

[视频讲解](https://www.bilibili.com/video/BV1zt4y1R7Tc/) 第四题。

## 前置知识

1. [我在知乎上对 KMP 算法的讲解](https://www.zhihu.com/question/21923021/answer/37475572)
2. [二分查找原理](https://www.bilibili.com/video/BV1AP41137w7/)

## 写法一：KMP+二分查找

1. 用 KMP 求出 $a$ 在 $s$ 中的所有出现位置，记作 $\textit{posA}$。
2. 用 KMP 求出 $b$ 在 $s$ 中的所有出现位置，记作 $\textit{posB}$。
3. 遍历 $\textit{posA}$ 中的下标 $i$，在 $\textit{posB}$ 中二分查找离 $i$ 最近的 $j$。如果 $|i-j|\le k$，则把 $i$ 加入答案。

```py [sol-Python3]
class Solution:
    def beautifulIndices(self, s: str, a: str, b: str, k: int) -> List[int]:
        pos_a = self.kmp(s, a)
        pos_b = self.kmp(s, b)

        ans = []
        for i in pos_a:
            bi = bisect_left(pos_b, i)
            if bi < len(pos_b) and pos_b[bi] - i <= k or \
               bi > 0 and i - pos_b[bi - 1] <= k:
                ans.append(i)
        return ans

    def kmp(self, text: str, pattern: str) -> List[int]:
        m = len(pattern)
        pi = [0] * m
        c = 0
        for i in range(1, m):
            v = pattern[i]
            while c and pattern[c] != v:
                c = pi[c - 1]
            if pattern[c] == v:
                c += 1
            pi[i] = c

        res = []
        c = 0
        for i, v in enumerate(text):
            while c and pattern[c] != v:
                c = pi[c - 1]
            if pattern[c] == v:
                c += 1
            if c == len(pattern):
                res.append(i - m + 1)
                c = pi[c - 1]
        return res
```

```java [sol-Java]
class Solution {
    public List<Integer> beautifulIndices(String s, String a, String b, int k) {
        char[] text = s.toCharArray();
        List<Integer> posA = kmp(text, a.toCharArray());
        List<Integer> posB = kmp(text, b.toCharArray());

        List<Integer> ans = new ArrayList<>();
        for (int i : posA) {
            int bi = lowerBound(posB, i);
            if (bi < posB.size() && posB.get(bi) - i <= k ||
                bi > 0 && i - posB.get(bi - 1) <= k) {
                ans.add(i);
            }
        }
        return ans;
    }

    private List<Integer> kmp(char[] text, char[] pattern) {
        int m = pattern.length;
        int[] pi = new int[m];
        int c = 0;
        for (int i = 1; i < m; i++) {
            char v = pattern[i];
            while (c > 0 && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            pi[i] = c;
        }

        List<Integer> res = new ArrayList<>();
        c = 0;
        for (int i = 0; i < text.length; i++) {
            char v = text[i];
            while (c > 0 && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            if (c == m) {
                res.add(i - m + 1);
                c = pi[c - 1];
            }
        }
        return res;
    }

    // 开区间写法
    // 请看 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(List<Integer> nums, int target) {
        int left = -1, right = nums.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = (left + right) >>> 1;
            if (nums.get(mid) < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> beautifulIndices(string s, string a, string b, int k) {
        vector<int> posA = kmp(s, a);
        vector<int> posB = kmp(s, b);

        vector<int> ans;
        for (int i: posA) {
            auto it = lower_bound(posB.begin(), posB.end(), i);
            if (it != posB.end() && *it - i <= k ||
                it != posB.begin() && i - *--it <= k) {
                ans.push_back(i);
            }
        }
        return ans;
    }

private:
    vector<int> kmp(string &text, string &pattern) {
        int m = pattern.length();
        vector<int> pi(m);
        int c = 0;
        for (int i = 1; i < m; i++) {
            char v = pattern[i];
            while (c && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            pi[i] = c;
        }

        vector<int> res;
        c = 0;
        for (int i = 0; i < text.length(); i++) {
            char v = text[i];
            while (c && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            if (c == m) {
                res.push_back(i - m + 1);
                c = pi[c - 1];
            }
        }
        return res;
    }
};
```

```go [sol-Go]
func beautifulIndices(s, a, b string, k int) (ans []int) {
	posA := kmp(s, a)
	posB := kmp(s, b)

	for _, i := range posA {
		bi := sort.SearchInts(posB, i)
		// 离 i 最近的 j 是 posB[bi] 或 posB[bi-1]
		if bi < len(posB) && posB[bi]-i <= k || bi > 0 && i-posB[bi-1] <= k {
			ans = append(ans, i)
		}
	}
	return
}

func kmp(text, pattern string) (pos []int) {
	m := len(pattern)
	pi := make([]int, m)
	cnt := 0
	for i := 1; i < m; i++ {
		v := pattern[i]
		for cnt > 0 && pattern[cnt] != v {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == v {
			cnt++
		}
		pi[i] = cnt
	}

	cnt = 0
	for i, v := range text {
		for cnt > 0 && pattern[cnt] != byte(v) {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == byte(v) {
			cnt++
		}
		if cnt == m {
			pos = append(pos, i-m+1)
			cnt = pi[cnt-1]
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二：KMP+双指针

由于 $\textit{posA}$ 和 $\textit{posB}$ 都是有序的，也可以用双指针做。

```py [sol-Python3]
class Solution:
    def beautifulIndices(self, s: str, a: str, b: str, k: int) -> List[int]:
        pos_a = self.kmp(s, a)
        pos_b = self.kmp(s, b)

        ans = []
        j, m = 0, len(pos_b)
        for i in pos_a:
            while j < m and pos_b[j] < i - k:
                j += 1
            if j < m and pos_b[j] <= i + k:
                ans.append(i)
        return ans

    def kmp(self, text: str, pattern: str) -> List[int]:
        m = len(pattern)
        pi = [0] * m
        c = 0
        for i in range(1, m):
            v = pattern[i]
            while c and pattern[c] != v:
                c = pi[c - 1]
            if pattern[c] == v:
                c += 1
            pi[i] = c

        res = []
        c = 0
        for i, v in enumerate(text):
            while c and pattern[c] != v:
                c = pi[c - 1]
            if pattern[c] == v:
                c += 1
            if c == len(pattern):
                res.append(i - m + 1)
                c = pi[c - 1]
        return res
```

```java [sol-Java]
class Solution {
    public List<Integer> beautifulIndices(String s, String a, String b, int k) {
        char[] text = s.toCharArray();
        List<Integer> posA = kmp(text, a.toCharArray());
        List<Integer> posB = kmp(text, b.toCharArray());

        List<Integer> ans = new ArrayList<>();
        int j = 0, m = posB.size();
        for (int i : posA) {
            while (j < m && posB.get(j) < i - k) {
                j++;
            }
            if (j < m && posB.get(j) <= i + k) {
                ans.add(i);
            }
        }
        return ans;
    }

    private List<Integer> kmp(char[] text, char[] pattern) {
        int m = pattern.length;
        int[] pi = new int[m];
        int c = 0;
        for (int i = 1; i < m; i++) {
            char v = pattern[i];
            while (c > 0 && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            pi[i] = c;
        }

        List<Integer> res = new ArrayList<>();
        c = 0;
        for (int i = 0; i < text.length; i++) {
            char v = text[i];
            while (c > 0 && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            if (c == m) {
                res.add(i - m + 1);
                c = pi[c - 1];
            }
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> beautifulIndices(string s, string a, string b, int k) {
        vector<int> posA = kmp(s, a);
        vector<int> posB = kmp(s, b);

        vector<int> ans;
        int j = 0, m = posB.size();
        for (int i : posA) {
            while (j < m && posB[j] < i - k) {
                j++;
            }
            if (j < m && posB[j] <= i + k) {
                ans.push_back(i);
            }
        }
        return ans;
    }

private:
    vector<int> kmp(string &text, string &pattern) {
        int m = pattern.length();
        vector<int> pi(m);
        int c = 0;
        for (int i = 1; i < m; i++) {
            char v = pattern[i];
            while (c && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            pi[i] = c;
        }

        vector<int> res;
        c = 0;
        for (int i = 0; i < text.length(); i++) {
            char v = text[i];
            while (c && pattern[c] != v) {
                c = pi[c - 1];
            }
            if (pattern[c] == v) {
                c++;
            }
            if (c == m) {
                res.push_back(i - m + 1);
                c = pi[c - 1];
            }
        }
        return res;
    }
};
```

```go [sol-Go]
func beautifulIndices(s, a, b string, k int) (ans []int) {
	posA := kmp(s, a)
	posB := kmp(s, b)

	j, m := 0, len(posB)
	for _, i := range posA {
		for j < m && posB[j] < i-k {
			j++
		}
		if j < m && posB[j] <= i+k {
			ans = append(ans, i)
		}
	}
	return
}

func kmp(text, pattern string) (pos []int) {
	m := len(pattern)
	pi := make([]int, m)
	cnt := 0
	for i := 1; i < m; i++ {
		v := pattern[i]
		for cnt > 0 && pattern[cnt] != v {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == v {
			cnt++
		}
		pi[i] = cnt
	}

	cnt = 0
	for i, v := range text {
		for cnt > 0 && pattern[cnt] != byte(v) {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == byte(v) {
			cnt++
		}
		if cnt == m {
			pos = append(pos, i-m+1)
			cnt = pi[cnt-1]
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。注意二重循环中 `j++` 的次数不会超过 $n$，所以二重循环的时间复杂度是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(n)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `一、KMP（前缀的后缀）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、KMP（前缀的后缀）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
