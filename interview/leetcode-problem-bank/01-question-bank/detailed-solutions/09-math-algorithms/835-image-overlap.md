# 835. 图像重叠

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/image-overlap/
- 题目 slug：`image-overlap`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.4 卷积
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/image-overlap/solutions/4022390/liang-chong-fang-fa-bao-li-mei-ju-juan-j-6bqq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力枚举 / 卷积（Python/Java/C++/Go）](https://leetcode.cn/problems/image-overlap/solutions/4022390/liang-chong-fang-fa-bao-li-mei-ju-juan-j-6bqq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-bao-li-mei-ju-juan-j-6bqq`
- topic id：`4022390`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

## 方法一：暴力枚举

![lc835.jpg](https://pic.leetcode.cn/1788650234-WQiJyd-lc835.jpg){:width=350px}

为方便描述，把 $\textit{img}_1$ 简记为 $A$，把 $\textit{img}_2$ 简记为 $B$。

如果把 $A$ 向下移动 $x$ 个单位，向右移动 $y$ 个单位（向上/向左则 $x,y$ 为负），那么 $A_{i,j}$ 对应着 $B_{i+x,\,j+y}$。例如，把 $A$ 向下移动 $1$ 个单位，向右移动 $1$ 个单位，那么 $A_{0,0}$ 对应 $B_{1,1}$。

移动后，重叠 $1$ 的个数为

$$
f(x,y) = \sum_{i=0}^{n-1}\sum_{j=0}^{n-1} A_{i,j}\cdot B_{i+x,\,j+y}
$$

> **注 1**：如果 $A_{i,j}$ 和 $B_{i+x,\,j+y}$ 都是 $1$，才能把计数器增加一，否则计数器不变。这等价于把计数器增加 $A_{i,j}\cdot B_{i+x,\,j+y}$。
> 
> **注 2**：超出下标范围的元素视作 $0$。

枚举 $x$ 和 $y$，答案为

$$
\max_{-(n-1)\le x,y\le n-1} f(x,y)
$$

代码实现时，由于 $0$ 对答案无贡献，可以只遍历满足 $0\le i < n$ 且 $0\le i+x< n$ 的 $i$，即 

$$
\max(-x,0)\le i < \min(n-x,n)
$$

对于 $j$，同理有

$$
\max(-y,0)\le j < \min(n-y,n)
$$

```py [sol-Python3]
class Solution:
    def largestOverlap(self, img1: List[List[int]], img2: List[List[int]]) -> int:
        n = len(img1)
        ans = 0
        for dx in range(1 - n, n):
            for dy in range(1 - n, n):
                cnt1 = 0
                for i in range(max(-dx, 0), min(n - dx, n)):
                    for j in range(max(-dy, 0), min(n - dy, n)):
                        # 两个数都是 1，才能让 cnt1 增加 1
                        cnt1 += img1[i][j] * img2[i + dx][j + dy]
                ans = max(ans, cnt1)
        return ans    
```

```java [sol-Java]
class Solution {
    public int largestOverlap(int[][] img1, int[][] img2) {
        int n = img1.length;
        int ans = 0;
        for (int dx = 1 - n; dx < n; dx++) {
            for (int dy = 1 - n; dy < n; dy++) {
                int cnt1 = 0;
                for (int i = Math.max(-dx, 0); i < Math.min(n - dx, n); i++) {
                    for (int j = Math.max(-dy, 0); j < Math.min(n - dy, n); j++) {
                        // 两个数都是 1，才能让 cnt1 增加 1
                        cnt1 += img1[i][j] * img2[i + dx][j + dy];
                    }
                }
                ans = Math.max(ans, cnt1);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int largestOverlap(vector<vector<int>>& img1, vector<vector<int>>& img2) {
        int n = img1.size();
        int ans = 0;
        for (int dx = 1 - n; dx < n; dx++) {
            for (int dy = 1 - n; dy < n; dy++) {
                int cnt1 = 0;
                for (int i = max(-dx, 0); i < min(n - dx, n); i++) {
                    for (int j = max(-dy, 0); j < min(n - dy, n); j++) {
                        // 两个数都是 1，才能让 cnt1 增加 1
                        cnt1 += img1[i][j] * img2[i + dx][j + dy];
                    }
                }
                ans = max(ans, cnt1);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func largestOverlap(img1, img2 [][]int) (ans int) {
	n := len(img1)
	for dx := 1 - n; dx < n; dx++ {
		for dy := 1 - n; dy < n; dy++ {
			cnt1 := 0
			for i := max(-dx, 0); i < min(n-dx, n); i++ {
				for j := max(-dy, 0); j < min(n-dy, n); j++ {
					// 两个数都是 1，才能让 cnt1 增加 1
					cnt1 += img1[i][j] * img2[i+dx][j+dy]
				}
			}
			ans = max(ans, cnt1)
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^4)$，其中 $n$ 是 $\textit{img}_i$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：二维卷积

> 本质上来说，我们计算的是 $A$ 和 $B$ 的互相关（cross-correlation）。

为了和卷积联系上，定义 $B$ 的翻转矩阵 $\widetilde{B}_{i,j} = B_{n-1-i,\,n-1-j}$。

计算 $A$ 和 $\widetilde{B}$ 的二维卷积

$$
C_{x,y} = \sum_{i=0}^{n-1}\sum_{j=0}^{n-1} A_{i,j}\cdot \widetilde{B}_{x-i,\,y-j}
$$

其中 $0\le x,y\le 2n-2$。超出下标范围的元素视作 $0$。

代入 $\widetilde{B}$ 的定义，上式变为

$$
C_{x,y} = \sum_{i=0}^{n-1}\sum_{j=0}^{n-1} A_{i,j}\cdot B_{n-1-x+i,\,n-1-y+j}
$$

令 $x' = n-1-x$，$y' = n-1-y$，上式变为

$$
C_{x,y} = \sum_{i=0}^{n-1}\sum_{j=0}^{n-1} A_{i,j}\cdot B_{i+x',\,j+y'} = f(x',y')
$$

其中 $-(n-1)\le x',y'\le n-1$。

上式表明，卷积矩阵 $C$ 恰好对应着所有 $(2n-1)^2$ 种平移情况，所以答案为 $C$ 中的最大值。

```py [sol-NumPy]
import numpy as np

class Solution:
    def largestOverlap(self, img1: List[List[int]], img2: List[List[int]]) -> int:
        a = np.array(img1, dtype=np.int8)
        b = np.array(img2, dtype=np.int8)
        b = np.flip(b)

        n = len(img1)
        shape = (n * 2 - 1, n * 2 - 1)
        fa = np.fft.fft2(a, shape)
        fb = np.fft.fft2(b, shape)
        conv = np.rint(np.fft.ifft2(fa * fb).real)

        return int(conv.max())
```

```py [sol-SciPy]
from scipy.signal import correlate2d

class Solution:
    def largestOverlap(self, img1: List[List[int]], img2: List[List[int]]) -> int:
        return int(correlate2d(img1, img2).max())
```

```go [sol-Go]
type fft struct {
	n        int
	omega    []complex128
	omegaInv []complex128
}

func newFFT(n int) *fft {
	omega := make([]complex128, n)
	omegaInv := make([]complex128, n)
	for i := range omega {
		sin, cos := math.Sincos(2 * math.Pi * float64(i) / float64(n))
		omega[i] = complex(cos, sin)
		omegaInv[i] = complex(cos, -sin)
	}
	return &fft{n, omega, omegaInv}
}

func (t *fft) transform(a, omega []complex128) {
	n := t.n
	for i, j := 0, 0; i < n; i++ {
		if i > j { // 保证同一对元素只交换一次
			a[i], a[j] = a[j], a[i]
		}
		for l := n / 2; ; l /= 2 {
			j ^= l
			if j >= l {
				break
			}
		}
	}
	for l := 2; l <= n; l *= 2 {
		m := l / 2
		for st := 0; st < n; st += l {
			b := a[st:]
			for i := range m {
				v := omega[n/l*i] * b[m+i]
				b[m+i] = b[i] - v
				b[i] += v
			}
		}
	}
}

func (t *fft) dft(a []complex128) {
	t.transform(a, t.omega)
}

func (t *fft) idft(a []complex128) {
	t.transform(a, t.omegaInv)
	cn := complex(float64(t.n), 0)
	for i := range a {
		a[i] /= cn
	}
}

func (t *fft) transform2(a [][]complex128, f func([]complex128)) {
	for _, row := range a {
		f(row)
	}

	n := len(a)
	tmp := make([]complex128, n)
	for j := range n {
		for i, row := range a {
			tmp[i] = row[j]
		}
		f(tmp)
		for i, row := range a {
			row[j] = tmp[i]
		}
	}
}

// 计算方阵 mat1 和方阵 mat2 的二维卷积
func conv2(mat1, mat2 [][]int) [][]int {
	n := len(mat1)
	n2 := n*2 - 1
	size := 1 << bits.Len(uint(n2))

	a := make([][]complex128, size)
	b := make([][]complex128, size)
	for i := range a {
		a[i] = make([]complex128, size)
		b[i] = make([]complex128, size)
	}
	for i, row := range mat1 {
		for j, x := range row {
			a[i][j] = complex(float64(x), 0)
			b[i][j] = complex(float64(mat2[i][j]), 0)
		}
	}

	f := newFFT(size)
	f.transform2(a, f.dft)
	f.transform2(b, f.dft)
	for i, row := range b {
		for j, x := range row {
			a[i][j] *= x
		}
	}
	f.transform2(a, f.idft)

	conv := make([][]int, n2)
	for i, row := range a[:n2] {
		conv[i] = make([]int, n2)
		for j, c := range row[:n2] {
			conv[i][j] = int(math.Round(real(c)))
		}
	}
	return conv
}

func largestOverlap(img1, img2 [][]int) (ans int) {
	for _, row := range img2 {
		slices.Reverse(row)
	}
	slices.Reverse(img2)

	conv := conv2(img1, img2)

	for _, row := range conv {
		ans = max(ans, slices.Max(row))
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log n)$，其中 $n$ 是 $\textit{img}_i$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n^2)$。

相关题目，见下面数学题单的「**§7.4 卷积**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `七、杂项 / §7.4 卷积`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.4 卷积`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
