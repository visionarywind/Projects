# 2183. 统计可以被 K 整除的下标对数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-array-pairs-divisible-by-k/
- 题目 slug：`count-array-pairs-divisible-by-k`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.5 因子
- 难度分：2246
- 外部题解来源：https://leetcode.cn/problems/count-array-pairs-divisible-by-k/solutions/1278721/tong-ji-yin-zi-chu-xian-ci-shu-by-endles-t5k8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计因子出现次数（Python/Java/C++/Go）](https://leetcode.cn/problems/count-array-pairs-divisible-by-k/solutions/1278721/tong-ji-yin-zi-chu-xian-ci-shu-by-endles-t5k8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-ji-yin-zi-chu-xian-ci-shu-by-endles-t5k8`
- topic id：`1278721`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

#### 提示 1

枚举 $\textit{nums}[j]$，求有多少个符合条件的 $\textit{nums}[i]$。

#### 提示 2

对于一个固定的 $\textit{nums}[j]$，$\textit{nums}[i]$ 必须是某个数 $x$ 的倍数。显然 $\textit{nums}[i]$ 可以是 $x=k$ 的倍数，但为了找到所有的 $\textit{nums}[i]$，$x$ 应当尽可能地小，这个最小的 $x$ 是多少？

---

#### 方法一：统计每个数的因子

从因子的角度考虑。如果 $\textit{nums}[j]$ 和 $k$ 有一些公因子，那么可以从 $k$ 中除去这些公因子，这样 $x$ 会变小，那么除去 $\text{GCD}(\textit{nums}[j],k)$ 是最优的，因此 $\textit{nums}[i]$ 必须是 $x=\dfrac{k}{\text{GCD}(\textit{nums}[j],k)}$ 的倍数。

如何统计是 $x$ 的倍数的 $\textit{nums}[i]$ 的个数呢？我们可以预处理每个数的所有因子，用一个哈希表 $\textit{cnt}$ 统计 $\textit{nums}[j]$ 前面每个数的每个因子的出现次数，这样对于一个固定的 $\textit{nums}[j]$，符合条件的 $\textit{nums}[i]$ 的个数就是 $\textit{cnt}[x]$。累加所有个数即为答案。

注：$[1,10^5]$ 内 $83160$ 的因子个数最多，有 $128$ 个。


```Python [sol1-Python3]
MX = 100001
divisors = [[] for _ in range(MX)]
for i in range(1, MX):  # 预处理每个数的所有因子，时间复杂度 O(MlogM)，M=1e5
    for j in range(i, MX, i):
        divisors[j].append(i)

class Solution:
    def countPairs(self, nums: List[int], k: int) -> int:
        ans = 0
        cnt = Counter()
        for v in nums:
            ans += cnt[k / gcd(v, k)]
            for d in divisors[v]:
                cnt[d] += 1
        return ans
```

```java [sol1-Java]
class Solution {
    final static int mx = 100001;
    static ArrayList<ArrayList<Integer>> divisors = new ArrayList<>(mx);
    static {
        for (var i = 0; i < mx; i++)
            divisors.add(new ArrayList<>());
        for (var i = 1; i < mx; ++i) // 预处理每个数的所有因子，时间复杂度 O(MlogM)，M=1e5
            for (var j = i; j < mx; j += i)
                divisors.get(j).add(i);
    }

    public long countPairs(int[] nums, int k) {
        var ans = 0L;
        var cnt = new HashMap<Integer, Integer>();
        for (var v : nums) {
            ans += cnt.getOrDefault(k / gcd(v, k), 0);
            for (var d : divisors.get(v))
                cnt.put(d, cnt.getOrDefault(d, 0) + 1);
        }
        return ans;
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```C++ [sol1-C++]
const int mx = 100001;
vector<int> divisors[mx];

int init = []() { // 预处理每个数的所有因子，时间复杂度 O(MlogM)，M=1e5
    for (int i = 1; i < mx; ++i)
        for (int j = i; j < mx; j += i)
            divisors[j].push_back(i);
    return 0;
}();

class Solution {
public:
    long long countPairs(vector<int> &nums, int k) {
        long long ans = 0;
        unordered_map<int, int> cnt;
        for (int v : nums) {
            ans += cnt[k / gcd(v, k)];
            for (int d : divisors[v])
                ++cnt[d];
        }
        return ans;
    }
};
```

```go [sol1-Go]
const mx int = 1e5
var divisors [mx + 1][]int

func init() { // 预处理每个数的所有因子，时间复杂度 O(MlogM)，M=1e5
	for i := 1; i <= mx; i++ {
		for j := i; j <= mx; j += i {
			divisors[j] = append(divisors[j], i)
		}
	}
}

func countPairs(nums []int, k int) (ans int64) {
	cnt := map[int]int{}
	for _, v := range nums {
		ans += int64(cnt[k/gcd(v, k)])
		for _, d := range divisors[v] {
			cnt[d]++
		}
	}
	return
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```


#### 方法二：统计 $k$ 的因子

注意到 $x$ 是 $k$ 的因子，因此我们可以将方法一中「统计 $v$ 的因子」改为「统计 $v$ 是 $k$ 的哪些因子的倍数」，这可以通过枚举 $k$ 的所有因子来判断。

```Python [sol2-Python3]
class Solution:
    def countPairs(self, nums: List[int], k: int) -> int:
        divisors = []  
        d = 1
        while d * d <= k:  # 预处理 k 的所有因子
            if k % d == 0:
                divisors.append(d)
                if d * d < k:
                    divisors.append(k / d)
            d += 1
        ans = 0
        cnt = Counter()
        for v in nums:
            ans += cnt[k / gcd(v, k)]
            for d in divisors:
                if v % d == 0:
                    cnt[d] += 1
        return ans
```

```java [sol2-Java]
class Solution {
    public long countPairs(int[] nums, int k) {
        var divisors = new ArrayList<Integer>(); // 预处理 k 的所有因子
        for (var d = 1; d * d <= k; d++) {
            if (k % d == 0) {
                divisors.add(d);
                if (d * d < k) divisors.add(k / d);
            }
        }
        var ans = 0L;
        var cnt = new HashMap<Integer, Integer>();
        for (var v : nums) {
            ans += cnt.getOrDefault(k / gcd(v, k), 0);
            for (var d : divisors)
                if (v % d == 0)
                    cnt.put(d, cnt.getOrDefault(d, 0) + 1);
        }
        return ans;
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```C++ [sol2-C++]
class Solution {
public:
    long long countPairs(vector<int> &nums, int k) {
        vector<int> divisors; 
        for (int d = 1; d * d <= k; ++d) { // 预处理 k 的所有因子
            if (k % d == 0) {
                divisors.push_back(d);
                if (d * d < k) divisors.push_back(k / d);
            }
        }
        long long ans = 0;
        unordered_map<int, int> cnt;
        for (int v : nums) {
            ans += cnt[k / gcd(v, k)];
            for (int d : divisors)
                if (v % d == 0) ++cnt[d];
        }
        return ans;
    }
};
```

```go [sol2-Go]
func countPairs(nums []int, k int) (ans int64) {
	divisors := []int{} 
	for d := 1; d*d <= k; d++ { // 预处理 k 的所有因子
		if k%d == 0 {
			divisors = append(divisors, d)
			if d*d < k {
				divisors = append(divisors, k/d)
			}
		}
	}
	cnt := map[int]int{}
	for _, v := range nums {
		ans += int64(cnt[k/gcd(v, k)])
		for _, d := range divisors {
			if v%d == 0 {
				cnt[d]++
			}
		}
	}
	return
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.5 因子`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.5 因子`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
