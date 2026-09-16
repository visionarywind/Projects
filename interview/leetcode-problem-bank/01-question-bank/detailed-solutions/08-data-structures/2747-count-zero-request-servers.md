# 2747. 统计没有收到请求的服务器数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-zero-request-servers/
- 题目 slug：`count-zero-request-servers`
- 来源专题：常用数据结构
- 来源分类路径：专题：离线算法
- 难度分：2405
- 外部题解来源：https://leetcode.cn/problems/count-zero-request-servers/solutions/2320098/chi-xian-hua-dong-chuang-kou-pythonjavac-b573/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[离线+滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/count-zero-request-servers/solutions/2320098/chi-xian-hua-dong-chuang-kou-pythonjavac-b573/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chi-xian-hua-dong-chuang-kou-pythonjavac-b573`
- topic id：`2320098`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

为方便回答询问，可以把 $\textit{logs}$ 的时间和询问都从小到大排序。对于询问，为了不打乱顺序，可以创建一个下标数组对其排序。

由于询问的窗口大小是固定的，所以可以用**滑动窗口**（双指针）来算，维护窗口内的各个服务器收到了多少次请求 $\textit{cnt}$，以及没有收到请求的服务器数目 $\textit{outOfRange}$。

具体见[【双周赛 107】](https://www.bilibili.com/video/BV1am4y1a7Zi/)第四题讲解，欢迎点赞投币！

```py [sol-Python3]
class Solution:
    def countServers(self, n: int, logs: List[List[int]], x: int, queries: List[int]) -> List[int]:
        logs.sort(key=lambda p: p[1])  # 按照 time 排序
        ans = [0] * len(queries)
        cnt = [0] * (n + 1)
        out_of_range = n
        left = right = 0
        for qi, q in sorted(enumerate(queries), key=lambda p: p[1]):
            while right < len(logs) and logs[right][1] <= q:  # 进入窗口
                i = logs[right][0]
                if cnt[i] == 0: out_of_range -= 1
                cnt[i] += 1
                right += 1
            while left < len(logs) and logs[left][1] < q - x:  # 离开窗口
                i = logs[left][0]
                cnt[i] -= 1
                if cnt[i] == 0: out_of_range += 1
                left += 1
            ans[qi] = out_of_range
        return ans
```

```java [sol-Java]
class Solution {
    public int[] countServers(int n, int[][] logs, int x, int[] queries) {
        int nq = queries.length;
        var id = new Integer[nq];
        for (int i = 0; i < nq; i++) id[i] = i;
        Arrays.sort(id, (i, j) -> queries[i] - queries[j]);
        Arrays.sort(logs, (a, b) -> a[1] - b[1]); // 按照 time 排序

        int[] ans = new int[nq], cnt = new int[n + 1];
        int outOfRange = n, left = 0, right = 0;
        for (int i : id) {
            while (right < logs.length && logs[right][1] <= queries[i]) // 进入窗口
                if (cnt[logs[right++][0]]++ == 0)
                    outOfRange--;
            while (left < logs.length && logs[left][1] < queries[i] - x) // 离开窗口
                if (--cnt[logs[left++][0]] == 0)
                    outOfRange++;
            ans[i] = outOfRange;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> countServers(int n, vector<vector<int>> &logs, int x, vector<int> &queries) {
        int nq = queries.size(), id[nq], cnt[n + 1];
        memset(cnt, 0, sizeof(cnt));
        iota(id, id + nq, 0);
        sort(id, id + nq, [&](int i, int j) {
            return queries[i] < queries[j];
        });
        sort(logs.begin(), logs.end(), [](const auto &a, const auto &b) {
            return a[1] < b[1]; // 按照 time 排序
        });

        vector<int> ans(nq);
        int out_of_range = n, left = 0, right = 0;
        for (int i: id) {
            while (right < logs.size() && logs[right][1] <= queries[i]) // 进入窗口
                if (cnt[logs[right++][0]]++ == 0)
                    out_of_range--;
            while (left < logs.size() && logs[left][1] < queries[i] - x) // 离开窗口
                if (--cnt[logs[left++][0]] == 0)
                    out_of_range++;
            ans[i] = out_of_range;
        }
        return ans;
    }
};
```

```go [sol-Go]
func countServers(n int, logs [][]int, x int, queries []int) []int {
	type pair struct{ q, i int }
	qs := make([]pair, len(queries))
	for i, q := range queries {
		qs[i] = pair{q, i}
	}
	sort.Slice(qs, func(i, j int) bool { return qs[i].q < qs[j].q })
	sort.Slice(logs, func(i, j int) bool { return logs[i][1] < logs[j][1] }) // 按照 time 排序

	ans := make([]int, len(queries))
	cnt := make([]int, n+1)
	outOfRange, left, right := n, 0, 0
	for _, p := range qs {
		for ; right < len(logs) && logs[right][1] <= p.q; right++ { // 进入窗口
			i := logs[right][0]
			if cnt[i] == 0 {
				outOfRange--
			}
			cnt[i]++
		}
		for ; left < len(logs) && logs[left][1] < p.q-x; left++ { // 离开窗口
			i := logs[left][0]
			cnt[i]--
			if cnt[i] == 0 {
				outOfRange++
			}
		}
		ans[p.i] = outOfRange
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + m\log m+ q\log q)$，其中 $m$ 为 $\textit{logs}$ 的长度，$q$ 为 $\textit{queries}$ 的长度。用数组统计出现次数，复杂度要加 $n$。
- 空间复杂度：$\mathcal{O}(n+q)$。

#### 思考题

如果询问的区间长度不固定，要怎么做？也就是输入的询问是 $[\textit{left}_i, \textit{right}_i]$。

## 本地原创解析

### 1. 题意重述

本题来自 `专题：离线算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：离线算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
