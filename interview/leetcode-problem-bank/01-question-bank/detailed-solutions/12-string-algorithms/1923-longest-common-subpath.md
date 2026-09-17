# 1923. 最长公共子路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-common-subpath/
- 题目 slug：`longest-common-subpath`
- 来源专题：字符串
- 来源分类路径：四、字符串哈希
- 难度分：2661
- 外部题解来源：https://leetcode.cn/problems/longest-common-subpath/solutions/857732/hou-zhui-shu-zu-er-fen-da-an-by-endlessc-ocar/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[后缀数组+二分答案](https://leetcode.cn/problems/longest-common-subpath/solutions/857732/hou-zhui-shu-zu-er-fen-da-an-by-endlessc-ocar/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hou-zhui-shu-zu-er-fen-da-an-by-endlessc-ocar`
- topic id：`857732`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 13:14:52 +0800

![1.png](https://pic.leetcode.cn/1625455590-fymofI-1.png)


```go
func longestCommonSubpath(_ int, paths [][]int) (ans int) {
	a := []int{}
	minPathLen := int(1e9) // 二分右边界
	for _, p := range paths {
		minPathLen = min(minPathLen, len(p))
		a = append(a, 1e9) // 用一个不存在 paths 中的数拼接所有路径
		a = append(a, p...)
	}
	n, m := len(a), len(paths)

	// 标记每个元素属于哪条路径
	ids := make([]int, n)
	id := -1
	for i, v := range a {
		if v == 1e9 {
			id++
			ids[i] = m
		} else {
			ids[i] = id
		}
	}

	// 构建 a 的后缀数组和高度数组
	s := make([]byte, 0, n*4)
	for _, v := range a {
		s = append(s, byte(v>>24), byte(v>>16), byte(v>>8), byte(v))
	}
	// Go 自带后缀数组，由 SA-IS 算法实现，复杂度 O(n)
	_sa := *(*[]int32)(unsafe.Pointer(reflect.ValueOf(suffixarray.New(s)).Elem().FieldByName("sa").Field(0).UnsafeAddr()))
	sa := make([]int32, 0, n)
	for _, v := range _sa {
		if v&3 == 0 {
			sa = append(sa, v>>2)
		}
	}
	rank := make([]int, n)
	for i := range rank {
		rank[sa[i]] = i
	}
	height := make([]int, n)
	h := 0
	for i, rk := range rank {
		if h > 0 {
			h--
		}
		if rk > 0 {
			for j := int(sa[rk-1]); i+h < n && j+h < n && a[i+h] == a[j+h]; h++ {
			}
		}
		height[rk] = h
	}

	// 二分求答案
	return sort.Search(minPathLen, func(limit int) bool {
		limit++
		vis := make([]int, m)
		for i := 1; i < n; i++ {
			if height[i] < limit {
				continue
			}
			cnt := 0
			for st := i; i < n && height[i] >= limit; i++ {
				// 检查 sa[i] 和 sa[i-1]
				if j := ids[sa[i]]; j < m && vis[j] != st {
					vis[j] = st
					cnt++
				}
				if j := ids[sa[i-1]]; j < m && vis[j] != st {
					vis[j] = st
					cnt++
				}
			}
			if cnt == m {
				return false
			}
		}
		return true
	})
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `四、字符串哈希`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、字符串哈希`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
