# 350. 两个数组的交集 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/intersection-of-two-arrays-ii/
- 题目 slug：`intersection-of-two-arrays-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.1 双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/intersection-of-two-arrays-ii/solutions/3056041/yi-ci-bian-li-jian-ji-xie-fa-fu-jin-jie-szdb1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历，简洁写法，附进阶问题解答（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/intersection-of-two-arrays-ii/solutions/3056041/yi-ci-bian-li-jian-ji-xie-fa-fu-jin-jie-szdb1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-ci-bian-li-jian-ji-xie-fa-fu-jin-jie-szdb1`
- topic id：`3056041`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

**前置题目**：[349. 两个数组的交集](https://leetcode.cn/problems/intersection-of-two-arrays/)。请先完成这题，并阅读 [我的题解](https://leetcode.cn/problems/intersection-of-two-arrays/solutions/3053420/yi-ci-bian-li-pythonjavacgojsrust-by-end-jstx/)。

本题只需把哈希集合改成哈希表，其中哈希表的 value 表示 key 的出现次数。

在 $\textit{nums}_2[i]$ 加入答案列表的同时，把 $\textit{nums}_2[i]$ 在哈希表中的出现次数减一。如果出现次数变成 $0$，后面遍历到相同的元素时，不加入答案列表。

```py [sol-Python3]
class Solution:
    def intersect(self, nums1: List[int], nums2: List[int]) -> List[int]:
        cnt = Counter(nums1)
        ans = []
        for x in nums2:
            if cnt[x] > 0:
                cnt[x] -= 1
                ans.append(x)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] intersect(int[] nums1, int[] nums2) {
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums1) {
            cnt.merge(x, 1, Integer::sum); // cnt[x]++
        }
        List<Integer> ans = new ArrayList<>();
        for (int x : nums2) {
            int c = cnt.getOrDefault(x, 0);
            if (c > 0) {
                cnt.put(x, c - 1);
                ans.add(x);
            }
        }
        // 由于返回值是 int[]，需要额外遍历一次
        return ans.stream().mapToInt(i -> i).toArray();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        unordered_map<int, int> cnt;
        for (int x : nums1) {
            cnt[x]++;
        }
        vector<int> ans;
        for (int x : nums2) {
            if (cnt[x] > 0) {
                cnt[x]--;
                ans.push_back(x);
            }
        }
        return ans;
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        unordered_multiset<int> st(nums1.begin(), nums1.end());
        vector<int> ans;
        for (int x : nums2) {
            auto it = st.find(x);
            if (it != st.end()) {
                st.erase(it);
                ans.push_back(x);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func intersect(nums1, nums2 []int) (ans []int) {
    cnt := map[int]int{}
    for _, x := range nums1 {
        cnt[x]++
    }
    for _, x := range nums2 {
        if cnt[x] > 0 {
            cnt[x]--
            ans = append(ans, x)
        }
    }
    return
}
```

```js [sol-JavaScript]
var intersect = function(nums1, nums2) {
    const cnt = new Map();
    for (const x of nums1) {
        cnt.set(x, (cnt.get(x) ?? 0) + 1);
    }
    const ans = [];
    for (const x of nums2) {
        const c = cnt.get(x) ?? 0;
        if (c > 0) {
            cnt.set(x, c - 1);
            ans.push(x);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn intersect(nums1: Vec<i32>, nums2: Vec<i32>) -> Vec<i32> {
        let mut cnt = HashMap::new();
        for x in nums1 {
            *cnt.entry(x).or_insert(0) += 1;
        }
        let mut ans = vec![];
        for x in nums2 {
            if let Some(c) = cnt.get_mut(&x) {
                if *c > 0 {
                    *c -= 1;
                    ans.push(x);
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{nums}_1$ 的长度，$m$ 是 $\textit{nums}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 进阶问题

**问**：如果给定的数组已经排好序呢？你将如何优化你的算法？

**答**：用双指针解决，具体见下面的双指针写法。空间复杂度优化至 $\mathcal{O}(1)$。

**问**：如果 $\textit{nums}_1$ 的大小比 $\textit{nums}_2$ 小，哪种方法更优？

**答**：这里讨论哈希表做法。可以把长度小的数组转成哈希表，这样可以做到 $\mathcal{O}(\min(n,m))$ 的空间复杂度。

**问**：如果 $\textit{nums}_2$ 的元素存储在磁盘上，内存是有限的，并且你不能一次加载所有的元素到内存中，你该怎么办？

**答**：用一个小型缓冲区（buffer）一边读数据一边遍历数据。这等价于问 $\textit{nums}_2$ 是一个**流**（Stream）的情况要怎么做。由于我们写的是一次遍历的代码，所以已经符合这个要求。

## 附：双指针写法

```py [sol-Python3]
class Solution:
    def intersect(self, nums1: List[int], nums2: List[int]) -> List[int]:
        # 如果保证 nums1 和 nums2 是有序的，排序可以去掉
        nums1.sort()
        nums2.sort()

        ans = []
        i = j = 0
        while i < len(nums1) and j < len(nums2):
            x, y = nums1[i], nums2[j]
            if x < y:
                i += 1
            elif x > y:
                j += 1
            else:
                ans.append(x)
                i += 1
                j += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] intersect(int[] nums1, int[] nums2) {
        // 如果保证 nums1 和 nums2 是有序的，排序可以去掉
        Arrays.sort(nums1);
        Arrays.sort(nums2);

        List<Integer> ans = new ArrayList<>();
        int i = 0;
        int j = 0;
        while (i < nums1.length && j < nums2.length) {
            int x = nums1[i];
            int y = nums2[j];
            if (x < y) {
                i++;
            } else if (x > y) {
                j++;
            } else {
                ans.add(x);
                i++;
                j++;
            }
        }

        // 由于返回值是 int[]，需要额外遍历一次
        return ans.stream().mapToInt(x -> x).toArray();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        // 如果保证 nums1 和 nums2 是有序的，排序可以去掉
        ranges::sort(nums1);
        ranges::sort(nums2);

        vector<int> ans;
        int i = 0, j = 0;
        while (i < nums1.size() && j < nums2.size()) {
            int x = nums1[i], y = nums2[j];
            if (x < y) {
                i++;
            } else if (x > y) {
                j++;
            } else {
                ans.push_back(x);
                i++;
                j++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func intersect(nums1, nums2 []int) (ans []int) {
    // 如果保证 nums1 和 nums2 是有序的，排序可以去掉
    slices.Sort(nums1)
    slices.Sort(nums2)

    i, j := 0, 0
    for i < len(nums1) && j < len(nums2) {
        x, y := nums1[i], nums2[j]
        if x < y {
            i++
        } else if x > y {
            j++
        } else {
            ans = append(ans, x)
            i++
            j++
        }
    }
    return
}
```

```js [sol-JavaScript]
var intersect = function(nums1, nums2) {
    // 如果保证 nums1 和 nums2 是有序的，排序可以去掉
    nums1.sort((a, b) => a - b);
    nums2.sort((a, b) => a - b);

    const ans = [];
    let i = 0, j = 0;
    while (i < nums1.length && j < nums2.length) {
        const x = nums1[i], y = nums2[j];
        if (x < y) {
            i++;
        } else if (x > y) {
            j++;
        } else {
            ans.push(x);
            i++;
            j++;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn intersect(mut nums1: Vec<i32>, mut nums2: Vec<i32>) -> Vec<i32> {
        // 如果保证 nums1 和 nums2 是有序的，排序可以去掉
        nums1.sort_unstable();
        nums2.sort_unstable();

        let mut ans = vec![];
        let mut i = 0;
        let mut j = 0;
        while i < nums1.len() && j < nums2.len() {
            let x = nums1[i];
            let y = nums2[j];
            if x < y {
                i += 1;
            } else if x > y {
                j += 1;
            } else {
                ans.push(x);
                i += 1;
                j += 1;
            }
        }
        ans
    }
}
```

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
