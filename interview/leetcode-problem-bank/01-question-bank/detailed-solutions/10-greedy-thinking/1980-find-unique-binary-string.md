# 1980. 找出不同的二进制字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-unique-binary-string/
- 题目 slug：`find-unique-binary-string`
- 来源专题：贪心与思维
- 来源分类路径：六、构造题
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-unique-binary-string/solutions/951165/go-jian-ji-xie-fa-by-endlesscheng-mcwc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力枚举 / 康托对角线（Python/Java/C++/Go）](https://leetcode.cn/problems/find-unique-binary-string/solutions/951165/go-jian-ji-xie-fa-by-endlesscheng-mcwc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-jian-ji-xie-fa-by-endlesscheng-mcwc`
- topic id：`951165`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

## 方法一：暴力枚举

把 $\textit{nums}$ 中的字符串转成二进制整数，保存到一个哈希集合中。

枚举 $\textit{ans} = 0,1,2,\ldots$ 直到 $\textit{ans}$ 不在哈希集合中，即为答案。

方法二告诉我们，满足要求的答案是一定存在的。

```py [sol-Python3]
class Solution:
    def findDifferentBinaryString(self, nums: List[str]) -> str:
        st = {int(s, 2) for s in nums}

        ans = 0
        while ans in st:
            ans += 1

        n = len(nums)
        return f"{ans:0{n}b}"
```

```java [sol-Java]
class Solution {
    public String findDifferentBinaryString(String[] nums) {
        Set<Integer> set = new HashSet<>();
        for (String s : nums) {
            set.add(Integer.parseInt(s, 2));
        }

        int ans = 0;
        while (set.contains(ans)) {
            ans++;
        }

        String bin = Integer.toBinaryString(ans);
        return "0".repeat(nums.length - bin.length()) + bin;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string findDifferentBinaryString(vector<string>& nums) {
        unordered_set<int> st;
        for (auto& s : nums) {
            st.insert(stoi(s, nullptr, 2));
        }

        int ans = 0;
        while (st.contains(ans)) {
            ans++;
        }

        int n = nums.size();
        return bitset<32>(ans).to_string().substr(32 - n);
    }
};
```

```go [sol-Go]
func findDifferentBinaryString(nums []string) string {
	n := len(nums)
	has := make(map[int]bool, n)
	for _, s := range nums {
		x, _ := strconv.ParseInt(s, 2, 64)
		has[int(x)] = true
	}

	ans := 0
	for has[ans] {
		ans++
	}

	return fmt.Sprintf("%0*b", n, ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。把长为 $n$ 的字符串转成整数需要 $\mathcal{O}(n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：康托对角线

这个方法灵感来自数学家康托关于「实数是不可数无限」的证明。

例如 $\textit{nums} = [\texttt{111}, \texttt{011}, \texttt{000}]$。我们可以构造一个字符串 $\textit{ans}$，满足：

- $\textit{ans}[0] = \texttt{0} \ne \textit{nums}[0][0]$。
- $\textit{ans}[1] = \texttt{0} \ne \textit{nums}[1][1]$。
- $\textit{ans}[2] = \texttt{1} \ne \textit{nums}[2][2]$。

$\textit{ans} = \texttt{001}$ 和每个 $\textit{nums}[i]$ **都至少有一个字符不同**，满足题目要求。

一般地，令 $\textit{ans}[i] = \textit{nums}[i][i]\oplus 1$，即可满足要求。其中 $\oplus$ 是异或运算。

```py [sol-Python3]
class Solution:
    def findDifferentBinaryString(self, nums: List[str]) -> str:
        ans = [''] * len(nums)
        for i, s in enumerate(nums):
            ans[i] = '1' if s[i] == '0' else '0'
        return ''.join(ans)
```

```java [sol-Java]
class Solution {
    public String findDifferentBinaryString(String[] nums) {
        int n = nums.length;
        char[] ans = new char[n];
        for (int i = 0; i < n; i++) {
            ans[i] = (char) (nums[i].charAt(i) ^ 1);
        }
        return new String(ans);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string findDifferentBinaryString(vector<string>& nums) {
        int n = nums.size();
        string ans(n, 0);
        for (int i = 0; i < n; i++) {
            ans[i] = nums[i][i] ^ 1;
        }
        return ans;
    }
};
```

```go [sol-Go]
func findDifferentBinaryString(nums []string) string {
	ans := make([]byte, len(nums))
	for i, s := range nums {
		ans[i] = s[i] ^ 1
	}
	return string(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。注意这个方法没有遍历整个字符串，只访问了每个字符串的其中一个字符。
- 空间复杂度：$\mathcal{O}(1)$，返回值不计入。

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

本题来自 `六、构造题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、构造题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
