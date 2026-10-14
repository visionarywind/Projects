# 2223. 构造字符串的总得分和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-scores-of-built-strings/
- 题目 slug：`sum-of-scores-of-built-strings`
- 来源专题：字符串
- 来源分类路径：二、Z 函数（后缀的前缀）
- 难度分：2220
- 外部题解来源：https://leetcode.cn/problems/sum-of-scores-of-built-strings/solutions/1390110/on-kuo-zhan-kmp-by-endlesscheng-9nhs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 扩展 KMP](https://leetcode.cn/problems/sum-of-scores-of-built-strings/solutions/1390110/on-kuo-zhan-kmp-by-endlesscheng-9nhs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-kuo-zhan-kmp-by-endlesscheng-9nhs`
- topic id：`1390110`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

题目求的就是扩展 KMP（Z 数组）的所有元素之和。

指路 -> https://oi-wiki.org/string/z-func/

```Python [sol1-Python3]
class Solution:
    def sumScores(self, s: str) -> int:
        n = len(s)
        z = [0] * n
        ans, l, r = n, 0, 0
        for i in range(1, n):
            z[i] = max(min(z[i - l], r - i + 1), 0)  # 注：不用 min max，拆开用 < > 比较会更快（仅限于 Python）
            while i + z[i] < n and s[z[i]] == s[i + z[i]]:
                l, r = i, i + z[i]
                z[i] += 1
            ans += z[i]
        return ans
```

```java [sol1-Java]
class Solution {
    public long sumScores(String s) {
        var n = s.length();
        var z = new int[n];
        long ans = n;
        for (int i = 1, l = 0, r = 0; i < n; i++) {
            z[i] = Math.max(Math.min(z[i - l], r - i + 1), 0);
            while (i + z[i] < n && s.charAt(z[i]) == s.charAt(i + z[i])) {
                l = i;
                r = i + z[i];
                z[i]++;
            }
            ans += z[i];
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long sumScores(string s) {
        int n = s.length();
        long ans = n;
        vector<int> z(n);
        for (int i = 1, l = 0, r = 0; i < n; ++i) {
            z[i] = max(min(z[i - l], r - i + 1), 0);
            while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
                l = i;
                r = i + z[i];
                ++z[i];
            }
            ans += z[i];
        }
        return ans;
    }
};
```

```go [sol1-Go]
func sumScores(s string) int64 {
	n := len(s)
	z := make([]int, n)
	ans := n
	for i, l, r := 1, 0, 0; i < n; i++ {
		z[i] = max(min(z[i-l], r-i+1), 0)
		for i+z[i] < n && s[z[i]] == s[i+z[i]] {
			l, r = i, i+z[i]
			z[i]++
		}
		ans += z[i]
	}
	return int64(ans)
}

func min(a, b int) int { if a > b { return b }; return a }
func max(a, b int) int { if a < b { return b }; return a }
```
