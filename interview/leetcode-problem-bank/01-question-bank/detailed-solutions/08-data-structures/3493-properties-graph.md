# 3493. 属性图

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/properties-graph/
- 题目 slug：`properties-graph`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.1 基础
- 难度分：1565
- 外部题解来源：https://leetcode.cn/problems/properties-graph/solutions/3624345/bing-cha-ji-pythonjavacgo-by-endlesschen-xi0d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/properties-graph/solutions/3624345/bing-cha-ji-pythonjavacgo-by-endlesschen-xi0d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bing-cha-ji-pythonjavacgo-by-endlesschen-xi0d`
- topic id：`3624345`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

暴力枚举所有 $\textit{properties}[i]$ 和 $\textit{properties}[j]$，如果交集大小 $\ge k$，那么用并查集合并 $i$ 和 $j$。

初始连通块个数 $\textit{cc}=n$，每成功合并一次，就把 $\textit{cc}$ 减一。

完整的并查集模板，见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/)。

[本题视频讲解](https://www.bilibili.com/video/BV12eXYYVE5H/?t=2m21s)，欢迎点赞关注~

```py [sol-Python3]
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己
        self._fa = list(range(n))  # 代表元
        self.cc = n  # 连通块个数

    # 返回 x 所在集合的代表元
    # 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    def find(self, x: int) -> int:
        # 如果 fa[x] == x，则表示 x 是代表元
        if self._fa[x] != x:
            self._fa[x] = self.find(self._fa[x])  # fa 改成代表元
        return self._fa[x]

    # 把 from 所在集合合并到 to 所在集合中
    def merge(self, from_: int, to: int) -> None:
        x, y = self.find(from_), self.find(to)
        if x == y:  # from 和 to 在同一个集合，不做合并
            return
        self._fa[x] = y  # 合并集合。修改后就可以认为 from 和 to 在同一个集合了
        self.cc -= 1  # 合并后，连通块个数减少了 1

class Solution:
    def numberOfComponents(self, properties: List[List[int]], k: int) -> int:
        sets = list(map(set, properties))
        uf = UnionFind(len(properties))
        for i, a in enumerate(sets):
            for j, b in enumerate(sets[:i]):
                if len(a & b) >= k:
                    uf.merge(j, i)
        return uf.cc
```

```py [sol-Python3 位运算]
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己
        self._fa = list(range(n))  # 代表元
        self.cc = n  # 连通块个数

    # 返回 x 所在集合的代表元
    # 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    def find(self, x: int) -> int:
        # 如果 fa[x] == x，则表示 x 是代表元
        if self._fa[x] != x:
            self._fa[x] = self.find(self._fa[x])  # fa 改成代表元
        return self._fa[x]

    # 把 from 所在集合合并到 to 所在集合中
    def merge(self, from_: int, to: int) -> None:
        x, y = self.find(from_), self.find(to)
        if x == y:  # from 和 to 在同一个集合，不做合并
            return
        self._fa[x] = y  # 合并集合。修改后就可以认为 from 和 to 在同一个集合了
        self.cc -= 1  # 合并后，连通块个数减少了 1

class Solution:
    def numberOfComponents(self, properties: List[List[int]], k: int) -> int:
        sets = [reduce(or_, (1 << x for x in a)) for a in properties]
        uf = UnionFind(len(properties))
        for i, a in enumerate(sets):
            for j, b in enumerate(sets[:i]):
                if (a & b).bit_count() >= k:
                    uf.merge(j, i)
        return uf.cc
```

```java [sol-Java]
class UnionFind {
    private final int[] fa;
    public int cc; // 连通块个数

    UnionFind(int n) {
        fa = new int[n];
        cc = n;
        for (int i = 0; i < n; i++) {
            fa[i] = i;
        }
    }

    public int find(int x) {
        if (fa[x] != x) {
            fa[x] = find(fa[x]);
        }
        return fa[x];
    }

    public void merge(int from, int to) {
        int x = find(from);
        int y = find(to);
        if (x == y) {
            return;
        }
        fa[x] = y;
        cc--;
    }
}

class Solution {
    public int numberOfComponents(int[][] properties, int k) {
        int n = properties.length;
        int m = properties[0].length;
        Set<Integer>[] sets = new HashSet[n];
        Arrays.setAll(sets, i -> new HashSet<>(m));
        for (int i = 0; i < n; i++) {
            for (int x : properties[i]) {
                sets[i].add(x);
            }
        }

        UnionFind u = new UnionFind(n);
        for (int i = 0; i < n; i++) {
            Set<Integer> a = sets[i];
            for (int j = 0; j < i; j++) {
                int cnt = 0;
                for (int x : sets[j]) {
                    if (a.contains(x)) {
                        cnt++;
                    }
                }
                if (cnt >= k) {
                    u.merge(i, j);
                }
            }
        }
        return u.cc;
    }
}
```

```cpp [sol-C++]
class UnionFind {
    vector<int> fa;

public:
    int cc; // 连通块个数

    UnionFind(int n) : fa(n), cc(n) {
        ranges::iota(fa, 0);
    }

    int find(int x) {
        if (fa[x] != x) {
            fa[x] = find(fa[x]);
        }
        return fa[x];
    }

    void merge(int from, int to) {
        int x = find(from), y = find(to);
        if (x == y) {
            return;
        }
        fa[x] = y;
        cc--;
    }
};

class Solution {
public:
    int numberOfComponents(vector<vector<int>>& properties, int k) {
        int n = properties.size();
        vector<unordered_set<int>> sets(n);
        for (int i = 0; i < n; i++) {
            sets[i] = unordered_set(properties[i].begin(), properties[i].end());
        }

        UnionFind uf(n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                int cnt = 0;
                for (int x : sets[j]) {
                    if (sets[i].contains(x)) {
                        cnt++;
                    }
                }
                if (cnt >= k) {
                    uf.merge(i, j);
                }
            }
        }
        return uf.cc;
    }
};
```

```go [sol-Go]
type uf struct {
	fa []int
	cc int
}

func newUnionFind(n int) uf {
	fa := make([]int, n)
	for i := range fa {
		fa[i] = i
	}
	return uf{fa, n}
}

func (u uf) find(x int) int {
	if u.fa[x] != x {
		u.fa[x] = u.find(u.fa[x])
	}
	return u.fa[x]
}

func (u *uf) merge(from, to int) {
	x, y := u.find(from), u.find(to)
	if x == y {
		return
	}
	u.fa[x] = y
	u.cc--
}

func numberOfComponents(properties [][]int, k int) int {
	sets := make([]map[int]bool, len(properties))
	for i, a := range properties {
		sets[i] = map[int]bool{}
		for _, x := range a {
			sets[i][x] = true
		}
	}

	u := newUnionFind(len(properties))
	for i, a := range sets {
		for j, b := range sets[:i] {
			cnt := 0
			for x := range b {
				if a[x] {
					cnt++
				}
			}
			if cnt >= k {
				u.merge(i, j)
			}
		}
	}
	return u.cc
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2m)$ 或者 $\mathcal{O}(n^2m/w)$，其中 $n$ 是 $\textit{properties}$ 的长度，$m$ 是 $\textit{properties}[i]$ 的长度，$w$ 等于 $32$ 或 $64$。
- 空间复杂度：$\mathcal{O}(nm)$ 或者 $\mathcal{O}(nm/w)$。

更多相似题目，见下面数据结构题单中的「**并查集**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
