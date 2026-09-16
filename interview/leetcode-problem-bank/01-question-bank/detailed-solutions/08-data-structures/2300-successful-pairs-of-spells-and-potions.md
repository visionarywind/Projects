# 2300. 咒语和药水的成功对数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/successful-pairs-of-spells-and-potions/
- 题目 slug：`successful-pairs-of-spells-and-potions`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/successful-pairs-of-spells-and-potions/solutions/1595712/by-endlesscheng-1kbp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：排序+二分 / 计数+值域后缀和（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/successful-pairs-of-spells-and-potions/solutions/1595712/by-endlesscheng-1kbp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-1kbp`
- topic id：`1595712`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：排序 + 二分查找

### 写法一：使用浮点数

问题相当于给你 $n$ 个询问，每次问 $\textit{spells}[i]$ 与 $\textit{potions}$ 中的多少个数相乘，结果 $\ge \textit{success}$。

对 $\textit{potions}$ 排序后，就可以二分查找了：

- 设 $j$ 是最小的满足 $\textit{potions}[j]\ge\dfrac{\textit{success}}{\textit{spells}[i]}$ 的下标。
- 由于数组已经排序，那么下标大于 $j$ 的数也同样满足不等式。
- 从 $j$ 到 $m-1$，一共有 $m-j$ 个满足不等式的数，其中 $m$ 是 $\textit{potions}$ 的长度。

有关二分查找的原理，请看[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

```py [sol-Python3]
class Solution:
    def successfulPairs(self, spells: List[int], potions: List[int], success: int) -> List[int]:
        potions.sort()
        m = len(potions)
        return [m - bisect_left(potions, success / x) for x in spells]
```

```java [sol-Java]
class Solution {
    public int[] successfulPairs(int[] spells, int[] potions, long success) {
        Arrays.sort(potions);
        for (int i = 0; i < spells.length; i++) {
            spells[i] = potions.length - lowerBound(potions, (double) success / spells[i]);
        }
        return spells;
    }

    // 返回 nums 中的第一个大于等于 target 的元素下标
    private int lowerBound(int[] nums, double target) {
        int left = -1, right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] <= target
            // nums[right] > target
            int mid = (left + right) >>> 1; // 比 left+(right-left)/2 更快的写法
            if (nums[mid] >= target) {
                right = mid; // 二分范围缩小到 (left, mid)
            } else {
                left = mid; // 二分范围缩小到 (mid, right)
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> successfulPairs(vector<int>& spells, vector<int>& potions, long long success) {
        ranges::sort(potions);
        for (int& x : spells) { // 原地修改
            x = potions.end() - ranges::lower_bound(potions, 1.0 * success / x);
        }
        return spells;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

// 返回 nums 中的第一个大于等于 target 的元素下标
int lowerBound(int* nums, int numsSize, double target) {
    int left = -1, right = numsSize; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        int mid = left + (right - left) / 2;
        if (nums[mid] >= target) {
            right = mid; // 二分范围缩小到 (left, mid)
        } else {
            left = mid; // 二分范围缩小到 (mid, right)
        }
    }
    return right;
}

int* successfulPairs(int* spells, int spellsSize, int* potions, int potionsSize, long long success, int* returnSize) {
    qsort(potions, potionsSize, sizeof(int), cmp);
    for (int i = 0; i < spellsSize; i++) {
        spells[i] = potionsSize - lowerBound(potions, potionsSize, 1.0 * success / spells[i]);
    }
    *returnSize = spellsSize;
    return spells;
}
```

```go [sol-Go]
func successfulPairs(spells, potions []int, success int64) []int {
    slices.Sort(potions)
    for i, x := range spells {
        target := float64(success) / float64(x)
        j := sort.Search(len(potions), func(j int) bool { return float64(potions[j]) >= target })
        spells[i] = len(potions) - j
    }
    return spells
}
```

```js [sol-JS]
var successfulPairs = function(spells, potions, success) {
    potions.sort((a, b) => a - b);
    for (let i = 0; i < spells.length; i++) {
        const target = success / spells[i];
        spells[i] = potions.length - lowerBound(potions, target);
    }
    return spells;
};

var lowerBound = function(nums, target) {
    let left = -1, right = nums.length; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    return right;
}
```

```js [sol-JS lodash]
var successfulPairs = function(spells, potions, success) {
    potions.sort((a, b) => a - b);
    for (let i = 0; i < spells.length; i++) {
        const target = success / spells[i];
        spells[i] = potions.length - _.sortedIndex(potions, target);
    }
    return spells;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn successful_pairs(mut spells: Vec<i32>, mut potions: Vec<i32>, success: i64) -> Vec<i32> {
        potions.sort_unstable();
        let last = potions[potions.len() - 1] as i64;
        for x in spells.iter_mut() {
            let target = success as f64 / *x as f64;
            let j = potions.partition_point(|&x| (x as f64) < target);
            *x = (potions.len() - j) as i32;
        }
        spells
    }
}
```

### 写法二：不使用浮点数

浮点数有舍入误差，如果数据范围更大，上面的做法就不一定正确了。

更好的做法是，避免使用浮点数，只使用整数计算。一方面可以保证正确性，另一方面整数运算比浮点运算快。

对于正整数，$xy\ge\textit{success}$ 等价于 $y\ge\left\lceil\dfrac{\textit{success}}{x}\right\rceil$。

为方便二分，可以利用如下恒等式： 

$$
\left\lceil\dfrac{a}{b}\right\rceil = \left\lfloor\dfrac{a+b-1}{b}\right\rfloor = \left\lfloor\dfrac{a-1}{b}\right\rfloor + 1
$$

证明见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

根据上式，我们有

$$
y\ge\left\lceil\dfrac{\textit{success}}{x}\right\rceil = \left\lfloor\dfrac{\textit{success}-1}{x}\right\rfloor + 1
$$ 

也可以写成

$$
y>\left\lfloor\dfrac{\textit{success}-1}{x}\right\rfloor
$$

为什么不等式一定要这样变形？好处是只需要在二分之前做一次除法，避免在二分循环内计算乘法，效率更高。另外的好处是部分语言可以直接调用库函数二分。

```py [sol-Python3]
class Solution:
    def successfulPairs(self, spells: List[int], potions: List[int], success: int) -> List[int]:
        potions.sort()
        m = len(potions)
        success -= 1  # 提前减一，避免在循环中反复减一
        return [m - bisect_right(potions, success // x) for x in spells]
```

```java [sol-Java]
class Solution {
    public int[] successfulPairs(int[] spells, int[] potions, long success) {
        Arrays.sort(potions);
        for (int i = 0; i < spells.length; i++) {
            long target = (success - 1) / spells[i];
            if (target < potions[potions.length - 1]) {
                // 这样写每次二分就只用比两个 int 的大小，避免把 potions 中的元素转成 long 比较
                spells[i] = potions.length - upperBound(potions, (int) target);
            } else {
                spells[i] = 0;
            }
        }
        return spells;
    }

    // 返回 nums 中的第一个大于 target 的元素下标
    private int upperBound(int[] nums, int target) {
        int left = -1, right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] <= target
            // nums[right] > target
            int mid = (left + right) >>> 1; // 比 left+(right-left)/2 更快的写法
            if (nums[mid] > target) {
                right = mid; // 二分范围缩小到 (left, mid)
            } else {
                left = mid; // 二分范围缩小到 (mid, right)
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> successfulPairs(vector<int>& spells, vector<int>& potions, long long success) {
        ranges::sort(potions);
        for (int& x : spells) { // 原地修改
            long long target = (success - 1) / x;
            if (target < potions.back()) {
                // 这样写每次二分就只用比两个 int 的大小，避免把 potions 中的元素转成 long long 比较
                x = potions.end() - ranges::upper_bound(potions, (int) target);
            } else {
                x = 0;
            }
        }
        return spells;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

// 返回 nums 中的第一个大于 target 的元素下标
int upperBound(int* nums, int numsSize, int target) {
    int left = -1, right = numsSize; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        int mid = left + (right - left) / 2;
        if (nums[mid] > target) {
            right = mid; // 二分范围缩小到 (left, mid)
        } else {
            left = mid; // 二分范围缩小到 (mid, right)
        }
    }
    return right;
}

int* successfulPairs(int* spells, int spellsSize, int* potions, int potionsSize, long long success, int* returnSize) {
    qsort(potions, potionsSize, sizeof(int), cmp);
    for (int i = 0; i < spellsSize; i++) {
        long long target = (success - 1) / spells[i];
        if (target < potions[potionsSize - 1]) {
            // 这样写每次二分就只用比两个 int 的大小，避免把 potions 中的元素转成 long long 比较
            spells[i] = potionsSize - upperBound(potions, potionsSize, target);
        } else {
            spells[i] = 0;
        }
    }
    *returnSize = spellsSize;
    return spells;
}
```

```go [sol-Go]
func successfulPairs(spells, potions []int, success int64) []int {
	slices.Sort(potions)
	for i, x := range spells {
		spells[i] = len(potions) - sort.SearchInts(potions, (int(success)-1)/x+1)
	}
	return spells
}
```

```js [sol-JS]
var successfulPairs = function(spells, potions, success) {
    potions.sort((a, b) => a - b);
    for (let i = 0; i < spells.length; i++) {
        const target = Math.ceil(success / spells[i]);
        spells[i] = potions.length - lowerBound(potions, target);
    }
    return spells;
};

var lowerBound = function(nums, target) {
    let left = -1, right = nums.length; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    return right;
}
```

```js [sol-JS lodash]
var successfulPairs = function(spells, potions, success) {
    potions.sort((a, b) => a - b);
    for (let i = 0; i < spells.length; i++) {
        const target = Math.ceil(success / spells[i]);
        spells[i] = potions.length - _.sortedIndex(potions, target);
    }
    return spells;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn successful_pairs(mut spells: Vec<i32>, mut potions: Vec<i32>, success: i64) -> Vec<i32> {
        potions.sort_unstable();
        let last = potions[potions.len() - 1] as i64;
        for x in spells.iter_mut() {
            let target = (success - 1) / *x as i64;
            if target < last { // 防止 i64 转成 i32 截断（这样不需要把 potions 中的数转成 i64 比较）
                let j = potions.partition_point(|&x| x <= target as i32);
                *x = (potions.len() - j) as i32;
            } else {
                *x = 0;
            }
        }
        spells
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m)\log m)$，其中 $n$ 为 $\textit{spells}$ 的长度，$m$ 为 $\textit{potions}$ 的长度。排序 $\mathcal{O}(m\log m)$。二分 $n$ 次，每次 $\mathcal{O}(\log m)$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销，仅用到若干额外变量。

## 方法二：计数 + 值域后缀和

方法一得出的结论是，统计满足 $\textit{potions}[j]\ge \left\lfloor\dfrac{\textit{success}-1}{\textit{spell}[i]}\right\rfloor + 1$ 的药水的个数。

比如 $\textit{potions}=[1,2,2,3,5,5,5]$，要计算 $\ge 2$ 的药水的个数，我们可以统计每个数出现了多少次，记在一个 $\textit{cnt}$ 数组中。在这个例子中，$\textit{cnt}=[0,1,2,1,0,3]$，比如 $\textit{cnt}[5]=3$ 表示 $5$ 出现了 $3$ 次。

那么计算 $\textit{cnt}[2] + \textit{cnt}[3] + \textit{cnt}[4] + \textit{cnt}[5]=2+1+0+3=6$，就是 $\ge 2$ 的药水的个数。

但这样太慢了。如何加速？

借鉴 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 的思想，我们可以倒着遍历 $\textit{cnt}$，原地计算 $\textit{cnt}$ 的**后缀和**，把 $\textit{cnt}[i]$ 更新为 $\ge i$ 的药水的个数。上面的 $\textit{cnt}$ 可以更新为 $[7,7,6,4,3,3]$。比如 $\textit{cnt}[2] = 6$ 表示 $\ge 2$ 的药水的个数。

```py [sol-Python3]
class Solution:
    def successfulPairs(self, spells: List[int], potions: List[int], success: int) -> List[int]:
        mx = max(potions)
        cnt = [0] * (mx + 1)
        for y in potions:
            cnt[y] += 1  # 统计每种药水的出现次数

        # 计算 cnt 的后缀和
        for i in range(mx - 1, -1, -1):
            cnt[i] += cnt[i + 1]
        # 计算完毕后，cnt[i] 就是 potions 值 >= i 的药水个数

        for i, x in enumerate(spells):
            low = (success - 1) // x + 1
            spells[i] = cnt[low] if low <= mx else 0
        return spells
```

```java [sol-Java]
class Solution {
    public int[] successfulPairs(int[] spells, int[] potions, long success) {
        int mx = 0;
        for (int y : potions) {
            mx = Math.max(mx, y);
        }

        int[] cnt = new int[mx + 1];
        for (int y : potions) {
            cnt[y]++; // 统计每种药水的出现次数
        }

        // 计算 cnt 的后缀和
        for (int i = mx - 1; i >= 0; i--) {
            cnt[i] += cnt[i + 1];
        }
        // 计算完毕后，cnt[i] 就是 potions 值 >= i 的药水个数

        for (int i = 0; i < spells.length; i++) {
            long low = (success - 1) / spells[i] + 1;
            spells[i] = low <= mx ? cnt[(int) low] : 0;
        }
        return spells;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> successfulPairs(vector<int>& spells, vector<int>& potions, long long success) {
        int mx = ranges::max(potions);
        vector<int> cnt(mx + 1);
        for (int y : potions) {
            cnt[y]++; // 统计每种药水的出现次数
        }

        // 计算 cnt 的后缀和
        for (int i = mx - 1; i >= 0; i--) {
            cnt[i] += cnt[i + 1];
        }
        // 计算完毕后，cnt[i] 就是 potions 值 >= i 的药水个数

        for (int& x : spells) {
            long long low = (success - 1) / x + 1;
            x = low <= mx ? cnt[low] : 0;
        }
        return spells;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int* successfulPairs(int* spells, int spellsSize, int* potions, int potionsSize, long long success, int* returnSize) {
    int mx = 0;
    for (int i = 0; i < potionsSize; i++) {
        mx = MAX(mx, potions[i]);
    }

    int* cnt = calloc(mx + 1, sizeof(int));
    for (int i = 0; i < potionsSize; i++) {
        cnt[potions[i]]++; // 统计每种药水的出现次数
    }

    // 计算 cnt 的后缀和
    for (int i = mx - 1; i >= 0; i--) {
        cnt[i] += cnt[i + 1];
    }
    // 计算完毕后，cnt[i] 就是 potions 值 >= i 的药水个数

    for (int i = 0; i < spellsSize; i++) {
        long long low = (success - 1) / spells[i] + 1;
        spells[i] = low <= mx ? cnt[low] : 0;
    }

    *returnSize = spellsSize;
    free(cnt);
    return spells;
}
```

```go [sol-Go]
func successfulPairs(spells, potions []int, success int64) []int {
	mx := slices.Max(potions)
	cnt := make([]int, mx+1)
	for _, y := range potions {
		cnt[y]++ // 统计每种药水的出现次数
	}

	// 计算 cnt 的后缀和
	for i := mx - 1; i >= 0; i-- {
		cnt[i] += cnt[i+1]
	}
	// 计算完毕后，cnt[i] 就是 potions 值 >= i 的药水个数

	for i, x := range spells {
		low := (int(success)-1)/x + 1
		if low <= mx {
			spells[i] = cnt[low]
		} else {
			spells[i] = 0
		}
	}
	return spells
}
```

```js [sol-JavaScript]
var successfulPairs = function(spells, potions, success) {
    const mx = Math.max(...potions);
    const cnt = Array(mx + 1).fill(0);
    for (const y of potions) {
        cnt[y]++; // 统计每种药水的出现次数
    }

    // 计算 cnt 的后缀和
    for (let i = mx - 1; i >= 0; i--) {
        cnt[i] += cnt[i + 1];
    }
    // 计算完毕后，cnt[i] 就是 potions 值 >= i 的药水个数

    for (let i = 0; i < spells.length; i++) {
        const low = Math.ceil(success / spells[i]);
        spells[i] = low <= mx ? cnt[low] : 0;
    }
    return spells;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn successful_pairs(mut spells: Vec<i32>, potions: Vec<i32>, success: i64) -> Vec<i32> {
        let mx = *potions.iter().max().unwrap() as usize;
        let mut cnt = vec![0; mx + 1];
        for y in potions {
            cnt[y as usize] += 1; // 统计每种药水的出现次数
        }

        // 计算 cnt 的后缀和
        for i in (0..mx).rev() {
            cnt[i] += cnt[i + 1];
        }
        // 计算完毕后，cnt[i] 就是 potions 值 >= i 的药水个数

        let success = success as usize;
        for x in spells.iter_mut() {
            let low = (success - 1) / *x as usize + 1;
            *x = if low <= mx { cnt[low] } else { 0 };
        }
        spells
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m+U)$，其中 $n$ 为 $\textit{spells}$ 的长度，$m$ 为 $\textit{potions}$ 的长度，$U=\max(\textit{potions})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 思考题

把乘法改成**异或**要怎么做？

这题是 [1803. 统计异或值在范围内的数对有多少](https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/)，做法见 [我的题解](https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/solution/bu-hui-zi-dian-shu-zhi-yong-ha-xi-biao-y-p2pu/)。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.5 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.5 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
