# 27. 移除元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-element/
- 题目 slug：`remove-element`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.5 原地修改
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-element/solutions/2802809/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-72bn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用栈思考，原地实现（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-element/solutions/2802809/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-72bn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-72bn`
- topic id：`2802809`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

把 $\textit{nums}$ 视作一个**栈**，把不等于 $\textit{val}$ 的元素入栈，最后返回栈的大小。

1. 初始化栈的大小 $\textit{stackSize} = 0$。
2. 从左到右遍历 $\textit{nums}$，如果 $\textit{nums}[i]\ne \textit{val}$，把 $\textit{nums}[i]$ 入栈，也就是更新 $\textit{nums}[\textit{stackSize}] = \textit{nums}[i]$，然后把 $\textit{stackSize}$ 加一。
3. 遍历结束，返回 $\textit{stackSize}$。

```py [sol-Python3]
class Solution:
    def removeElement(self, nums: List[int], val: int) -> int:
        stack_size = 0
        for x in nums:
            if x != val:
                nums[stack_size] = x  # 把 x 入栈
                stack_size += 1
        return stack_size
```

```java [sol-Java]
class Solution {
    public int removeElement(int[] nums, int val) {
        int stackSize = 0;
        for (int x : nums) {
            if (x != val) {
                nums[stackSize++] = x; // 把 x 入栈
            }
        }
        return stackSize;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        int stack_size = 0;
        for (int x : nums) {
            if (x != val) {
                nums[stack_size++] = x; // 把 x 入栈
            }
        }
        return stack_size;
    }
};
```

```c [sol-C]
int removeElement(int* nums, int numsSize, int val) {
    int stackSize = 0;
    for (int i = 0; i < numsSize; i++) {
        if (nums[i] != val) {
            nums[stackSize++] = nums[i]; // 把 nums[i] 入栈
        }
    }
    return stackSize;
}
```

```go [sol-Go]
func removeElement(nums []int, val int) int {
    stackSize := 0 // 另一种写法见【Go 写法二】
    for _, x := range nums {
        if x != val {
            nums[stackSize] = x // 把 x 入栈
            stackSize++
        }
    }
    return stackSize
}
```

```go [sol-Go 写法二]
func removeElement(nums []int, val int) int {
    stk := nums[:0]
    for _, x := range nums {
        if x != val {
            stk = append(stk, x) // 把 x 入栈
        }
    }
    return len(stk)
}
```

```js [sol-JavaScript]
var removeElement = function(nums, val) {
    let stackSize = 0;
    for (const x of nums) {
        if (x !== val) {
            nums[stackSize++] = x; // 把 x 入栈
        }
    }
    return stackSize;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_element(nums: &mut Vec<i32>, val: i32) -> i32 {
        let mut stack_size = 0;
        for i in 0..nums.len() {
            if nums[i] != val {
                nums[stack_size] = nums[i]; // 把 nums[i] 入栈
                stack_size += 1;
            }
        }
        stack_size as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
