# 735. 小行星碰撞

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/asteroid-collision/
- 题目 slug：`asteroid-collision`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.3 邻项消除
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/asteroid-collision/solutions/3938065/yong-zhan-wei-hu-xiao-xing-xing-pythonja-llqq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用栈维护小行星（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/asteroid-collision/solutions/3938065/yong-zhan-wei-hu-xiao-xing-xing-pythonja-llqq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-zhan-wei-hu-xiao-xing-xing-pythonja-llqq`
- topic id：`3938065`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

从左到右遍历这些小行星，向右的小行星会和向左的小行星碰撞。

遍历到一颗向左的小行星时，我们需要找到左边最近的未爆炸的小行星。这可以用一个栈维护。

如果当前小行星向右，那么直接入栈，继续向后遍历 $\textit{asteroids}$。

如果当前小行星向左，设其大小为 $x$，分类讨论：

- 如果栈顶的小行星向右，且大小 $\le x$，那么栈顶的小行星爆炸，弹出栈顶。
- 如果栈顶的小行星向右，且大小 $\ge x$，那么 $x$ 爆炸。
- 如此循环，直到 $x$ 爆炸，或者没有向右的小行星。

循环结束后，如果 $x$ 没有爆炸，那么把 $x$ 入栈。

最后，栈中剩余元素就是剩余的小行星。

```py [sol-Python3]
class Solution:
    def asteroidCollision(self, asteroids: List[int]) -> List[int]:
        st = []
        for x in asteroids:
            if x > 0:  # x 向右
                st.append(x)
                continue
            while st and st[-1] > 0:  # 栈顶小行星向右
                top = st[-1]
                if top <= -x:  # 栈顶小行星爆炸
                    st.pop()
                if top >= -x:  # x 爆炸
                    break
            else:  # while 没有 break，说明 x 没有爆炸
                st.append(x)
        return st
```

```java [sol-Java]
class Solution {
    public int[] asteroidCollision(int[] asteroids) {
        int[] st = new int[asteroids.length];
        int top = -1; // 栈顶下标
        next:
        for (int x : asteroids) {
            if (x > 0) { // x 向右
                st[++top] = x;
                continue;
            }
            while (top >= 0 && st[top] > 0) { // 栈顶小行星向右
                int t = st[top];
                if (t <= -x) { // 栈顶小行星爆炸
                    top--;
                }
                if (t >= -x) { // x 爆炸
                    continue next; // 遍历下一颗小行星
                }
            }
            // x 没有爆炸
            st[++top] = x;
        }
        return Arrays.copyOf(st, top + 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> asteroidCollision(vector<int>& asteroids) {
        vector<int> st;
        for (int x : asteroids) {
            if (x > 0) { // x 向右
                st.push_back(x);
                continue;
            }
            bool ok = true;
            while (!st.empty() && st.back() > 0) { // 栈顶小行星向右
                int top = st.back();
                if (top <= -x) { // 栈顶小行星爆炸
                    st.pop_back();
                }
                if (top >= -x) { // x 爆炸
                    ok = false;
                    break;
                }
            }
            if (ok) { // x 没有爆炸
                st.push_back(x);
            }
        }
        return st;
    }
};
```

```c [sol-C]
int* asteroidCollision(int* asteroids, int asteroidsSize, int* returnSize) {
    int* st = malloc(asteroidsSize * sizeof(int));
    int top = -1; // 栈顶下标

    for (int i = 0; i < asteroidsSize; i++) {
        int x = asteroids[i];
        if (x > 0) { // x 向右
            st[++top] = x;
            continue;
        }
        int ok = true;
        while (top >= 0 && st[top] > 0) { // 栈顶小行星向右
            int t = st[top];
            if (t <= -x) { // 栈顶小行星爆炸
                top--;
            }
            if (t >= -x) { // x 爆炸
                ok = false;
                break;
            }
        }
        if (ok) { // x 没有爆炸
            st[++top] = x;
        }
    }

    *returnSize = top + 1;
    return st;
}
```

```go [sol-Go]
func asteroidCollision(asteroids []int) (st []int) {
next:
	for _, x := range asteroids {
		if x > 0 { // x 向右
			st = append(st, x)
			continue
		}
		for len(st) > 0 && st[len(st)-1] > 0 { // 栈顶小行星向右
			top := st[len(st)-1]
			if top <= -x { // 栈顶小行星爆炸
				st = st[:len(st)-1]
			}
			if top >= -x { // x 爆炸
				continue next // 遍历下一颗小行星
			}
		}
		// x 没有爆炸
		st = append(st, x)
	}
	return
}
```

```js [sol-JavaScript]
var asteroidCollision = function(asteroids) {
    const st = [];
    for (const x of asteroids) {
        if (x > 0) { // x 向右
            st.push(x);
            continue;
        }
        let ok = true;
        while (st.length > 0 && st[st.length - 1] > 0) { // 栈顶小行星向右
            const top = st[st.length - 1];
            if (top <= -x) { // 栈顶小行星爆炸
                st.pop();
            }
            if (top >= -x) { // x 爆炸
                ok = false;
                break;
            }
        }
        if (ok) { // x 没有爆炸
            st.push(x);
        }
    }
    return st;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn asteroid_collision(asteroids: Vec<i32>) -> Vec<i32> {
        let mut st = vec![];
        for x in asteroids {
            if x > 0 { // x 向右
                st.push(x);
                continue;
            }
            let mut ok = true;
            while let Some(&top) = st.last() {
                if top < 0 { // 栈顶小行星向左
                    break;
                }
                if top <= -x { // 栈顶小行星爆炸
                    st.pop();
                }
                if top >= -x { // x 爆炸
                    ok = false;
                    break;
                }
            }
            if ok { // x 没有爆炸
                st.push(x);
            }
        }
        st
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{asteroids}$ 的长度。虽然我们写了个二重循环，但每个元素至多入栈出栈各一次，所以二重循环的**总**循环次数是 $\mathcal{O}(n)$ 的，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§3.3 邻项消除**」。

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
