# 4030. 判断 ASCII 值回文

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-ascii-palindromic/
- 题目 slug：`check-ascii-palindromic`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：1165
- 外部题解来源：https://leetcode.cn/problems/check-ascii-palindromic/solutions/4016382/liang-chong-fang-fa-zi-fu-chuan-wei-yun-mqp94/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：字符串 / 位运算（Python/Java/C++/Go）](https://leetcode.cn/problems/check-ascii-palindromic/solutions/4016382/liang-chong-fang-fa-zi-fu-chuan-wei-yun-mqp94/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-zi-fu-chuan-wei-yun-mqp94`
- topic id：`4016382`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

下午两点 [B站@灵茶山艾府](https://space.bilibili.com/206214) 直播讲题，欢迎关注~

## 方法一：用字符串

```py [sol-Python3]
class Solution:
    def isPalindromic(self, s: str) -> bool:
        t = ''.join(f'{ord(ch):08b}' for ch in s)
        return t == t[::-1]
```

```java [sol-Java]
class Solution {
    public boolean isPalindromic(String s) {
        char[] chars = s.toCharArray();
        StringBuilder t = new StringBuilder(chars.length * 8);
        for (char ch : chars) {
            t.append(String.format("%8s", Integer.toBinaryString(ch)).replace(' ', '0'));
        }

        int n = t.length();
        for (int i = 0; i < n / 2; i++) {
            if (t.charAt(i) != t.charAt(n - 1 - i)) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPalindromic(string s) {
        string t;
        for (char ch : s) {
            for (int i = 7; i >= 0; i--) {
                t += '0' + (ch >> i & 1);
            }
        }

        int n = t.size();
        for (int i = 0; i < n / 2; i++) {
            if (t[i] != t[n - 1 - i]) {
                return false;
            }
        }
        return true;
    }
};
```

```go [sol-Go]
func isPalindromic(s string) bool {
	t := make([]byte, 0, len(s)*8)
	for _, ch := range s {
		t = append(t, fmt.Sprintf("%0*b", 8, ch)...)
	}

	n := len(t)
	for i := range n / 2 {
		if t[i] != t[n-1-i] {
			return false
		}
	}
	return true
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：不用字符串

只有 $\texttt{f},\texttt{n},\texttt{v}$ 反转后仍是小写字母的 ASCII 值。其中：

- $\texttt{f}$ 反转后是 $\texttt{f}$。
- $\texttt{n}$ 反转后是 $\texttt{v}$。
- $\texttt{v}$ 反转后是 $\texttt{n}$。

但这不是通用做法。

**通用做法**是用位运算反转一个二进制数，见 [190. 颠倒二进制位](https://leetcode.cn/problems/reverse-bits/)。

```py [sol-Python3]
m0 = 0b01010101
m1 = 0b00110011
m2 = 0b00001111

def reverseBits(n: int) -> int:
    n = n >> 1 & m0 | (n & m0) << 1  # 交换相邻位
    n = n >> 2 & m1 | (n & m1) << 2  # 两个两个交换
    return n >> 4 | (n & m2) << 4  # 交换高低 4 位

class Solution:
    def isPalindromic(self, s: str) -> bool:
        for i in range(len(s) // 2 + 1):
            if reverseBits(ord(s[i])) != ord(s[-1 - i]):
                return False
        return True
```

```java [sol-Java]
class Solution {
    private static final int m0 = 0b01010101;
    private static final int m1 = 0b00110011;
    private static final int m2 = 0b00001111;

    private int reverseBits(int n) {
        n = n >> 1 & m0 | (n & m0) << 1; // 交换相邻位
        n = n >> 2 & m1 | (n & m1) << 2; // 两个两个交换
        return n >> 4 | (n & m2) << 4;   // 交换高低 4 位
    }

    public boolean isPalindromic(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        for (int i = 0; i <= n / 2; i++) {
            if (reverseBits(s[i]) != s[n - 1 - i]) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPalindromic(string s) {
        int n = s.size();
        for (int i = 0; i <= n / 2; i++) {
            if (__builtin_bitreverse8(s[i]) != s[n - 1 - i]) {
                return false;
            }
        }
        return true;
    }
};
```

```go [sol-Go]
func isPalindromic(s string) bool {
	n := len(s)
	for i := range n/2 + 1 {
		if bits.Reverse8(s[i]) != s[n-1-i] {
			return false
		}
	}
	return true
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [190. 颠倒二进制位](https://leetcode.cn/problems/reverse-bits/)
- [3750. 最少反转次数得到翻转二进制字符串](https://leetcode.cn/problems/minimum-number-of-flips-to-reverse-binary-string/) 1289
- [3769. 二进制反射排序](https://leetcode.cn/problems/sort-integers-by-binary-reflection/) 1364

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
