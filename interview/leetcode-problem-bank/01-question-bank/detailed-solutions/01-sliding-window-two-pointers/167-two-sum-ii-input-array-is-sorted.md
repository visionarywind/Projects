# 167. 两数之和 II - 输入有序数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/two-sum-ii-input-array-is-sorted/
- 题目 slug：`two-sum-ii-input-array-is-sorted`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/two-sum-ii-input-array-is-sorted/solutions/1968341/san-shu-zhi-he-bu-hui-xie-xiang-xiang-sh-6wbq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】双指针想不明白？一个视频讲透！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/two-sum-ii-input-array-is-sorted/solutions/1968341/san-shu-zhi-he-bu-hui-xie-xiang-xiang-sh-6wbq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-shu-zhi-he-bu-hui-xie-xiang-xiang-sh-6wbq`
- topic id：`1968341`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看 [相向双指针【基础算法精讲】](https://www.bilibili.com/video/BV1bP411c7oJ/)，欢迎点赞关注~

⚠**注意**：虽然题目说下标从 $1$ 开始，但输入的 $\textit{numbers}$ 数组的下标仍然是从 $0$ 开始的。我们应当按照从 $0$ 开始的下标写代码，只是在最后返回的时候，把下标加一。

⚠**注意**：题目保证答案一定存在。

```py [sol-Python3]
class Solution:
    def twoSum(self, numbers: List[int], target: int) -> List[int]:
        left = 0
        right = len(numbers) - 1
        while True:
            s = numbers[left] + numbers[right]
            if s == target:
                return [left + 1, right + 1]  # 题目要求下标从 1 开始
            if s > target:
                right -= 1
            else:
                left += 1
```

```java [sol-Java]
class Solution {
    public int[] twoSum(int[] numbers, int target) {
        int left = 0;
        int right = numbers.length - 1;
        while (true) {
            int s = numbers[left] + numbers[right];
            if (s == target) {
                return new int[]{left + 1, right + 1}; // 题目要求下标从 1 开始
            }
            if (s > target) { 
                right--;
            } else {
                left++;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
        int left = 0, right = numbers.size() - 1;
        while (true) {
            int s = numbers[left] + numbers[right];
            if (s == target) {
                return {left + 1, right + 1}; // 题目要求下标从 1 开始
            }
            s > target ? right-- : left++;
        }
    }
};
```

```c [sol-C]
int* twoSum(int* numbers, int numbersSize, int target, int* returnSize) {
    int left = 0, right = numbersSize - 1;
    while (true) {
        int s = numbers[left] + numbers[right];
        if (s == target) {
            *returnSize = 2;
            int* ans = malloc(2 * sizeof(int));
            ans[0] = left + 1;
            ans[1] = right + 1; // 题目要求下标从 1 开始
            return ans;
        }
        s > target ? right-- : left++;
    }
}
```

```go [sol-Go]
func twoSum(numbers []int, target int) []int {
    left, right := 0, len(numbers)-1
    for {
        s := numbers[left] + numbers[right]
        if s == target {
            return []int{left + 1, right + 1} // 题目要求下标从 1 开始
        }
        if s > target {
            right--
        } else {
            left++
        }
    }
}
```

```js [sol-JavaScript]
var twoSum = function(numbers, target) {
    let left = 0, right = numbers.length - 1;
    while (true) {
        let s = numbers[left] + numbers[right];
        if (s === target) {
            return [left + 1, right + 1]; // 题目要求下标从 1 开始
        }
        s > target ? right-- : left++;
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn two_sum(numbers: Vec<i32>, target: i32) -> Vec<i32> {
        let mut left = 0;
        let mut right = numbers.len() - 1;
        loop {
            let s = numbers[left] + numbers[right];
            if s == target {
                return vec![left as i32 + 1, right as i32 + 1]; // 题目要求下标从 1 开始
            } else if s > target {
                right -= 1;
            } else {
                left += 1;
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{numbers}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

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
