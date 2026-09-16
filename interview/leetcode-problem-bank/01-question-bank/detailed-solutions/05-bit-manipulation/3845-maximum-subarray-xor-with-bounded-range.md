# 3845. 最大子数组异或值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-subarray-xor-with-bounded-range/
- 题目 slug：`maximum-subarray-xor-with-bounded-range`
- 来源专题：位运算
- 来源分类路径：五、试填法
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-subarray-xor-with-bounded-range/solutions/3903069/hua-dong-chuang-kou-dan-diao-dui-lie-qia-5sz6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：0-1 字典树 / 试填法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-subarray-xor-with-bounded-range/solutions/3903069/hua-dong-chuang-kou-dan-diao-dui-lie-qia-5sz6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-dong-chuang-kou-dan-diao-dui-lie-qia-5sz6`
- topic id：`3903069`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置题目/知识点

1. [1438. 绝对差不超过限制的最长连续子数组](https://leetcode.cn/problems/longest-continuous-subarray-with-absolute-diff-less-than-or-equal-to-limit/)
2. [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)
3. [421. 数组中两个数的最大异或值](https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/)

## 方法一：0-1 字典树（异或字典树）

「最大值与最小值之间的差值不超过 $k$」这个约束和 1438 题是一样的，用滑动窗口和单调队列解决。

子数组异或和可以用前缀和快速计算。设 $\textit{nums}$ 的**前缀异或和**数组为 $s$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

题目要求子数组的最大异或和，我们可以枚举子数组的右端点下标 $r$，问题变成：

- 计算子数组 $[i,r]$ 的最大异或和，其中 $i$ 在闭区间 $[l,r]$ 中，其中 $l$ 是滑动窗口的左端点。

这等价于：

- 计算 $s[r+1]$ 与 $i$ 在 $[l,r]$ 中的一个 $s[i]$ 的最大异或和。

这可以用 0-1 字典树解决，见 421 题。

[本题视频讲解](https://www.bilibili.com/video/BV1VgZ4BCETj/?t=26m12s)，欢迎点赞关注~

```py [sol-Python3]
WIDTH = 15  # nums[i] 二进制长度的最大值


class Node:
    __slots__ = 'son', 'leaf'

    def __init__(self):
        self.son = [None] * 2
        self.leaf = 0  # 子树叶子个数


class Trie:
    def __init__(self):
        self.root = Node()

    def put(self, val: int) -> Node:
        cur = self.root
        for i in range(WIDTH - 1, -1, -1):
            bit = val >> i & 1
            if cur.son[bit] is None:
                cur.son[bit] = Node()
            cur = cur.son[bit]
            cur.leaf += 1

    def delete(self, val: int) -> Node:
        cur = self.root
        for i in range(WIDTH - 1, -1, -1):
            cur = cur.son[val >> i & 1]
            cur.leaf -= 1  # 如果减成 0 了，说明子树是空的，可以理解成 cur is None

    def max_xor(self, val: int) -> int:
        cur = self.root
        ans = 0
        for i in range(WIDTH - 1, -1, -1):
            bit = val >> i & 1
            if cur.son[bit ^ 1] and cur.son[bit ^ 1].leaf:
                ans |= 1 << i
                bit ^= 1
            cur = cur.son[bit]
        return ans


class Solution:
    def maxXor(self, nums: list[int], k: int) -> int:
        pre = list(accumulate(nums, xor, initial=0))

        t = Trie()
        min_q = deque()
        max_q = deque()
        ans = left = 0
        for right, x in enumerate(nums):
            # 1. 入
            t.put(pre[right])

            while min_q and x <= nums[min_q[-1]]:
                min_q.pop()
            min_q.append(right)

            while max_q and x >= nums[max_q[-1]]:
                max_q.pop()
            max_q.append(right)

            # 2. 出
            while nums[max_q[0]] - nums[min_q[0]] > k:
                t.delete(pre[left])
                left += 1
                if min_q[0] < left:
                    min_q.popleft()
                if max_q[0] < left:
                    max_q.popleft()

            # 3. 更新答案
            ans = max(ans, t.max_xor(pre[right + 1]))
        return ans
```

```java [sol-Java]
class Trie {
    private static final int WIDTH = 15; // nums[i] 二进制长度的最大值

    private static class Node {
        Node[] son = new Node[2];
        int leaf; // 子树叶子个数
    }

    private final Node root = new Node();

    public void put(int val) {
        Node cur = root;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            if (cur.son[bit] == null) {
                cur.son[bit] = new Node();
            }
            cur = cur.son[bit];
            cur.leaf++;
        }
    }

    public void del(int val) {
        Node cur = root;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            cur = cur.son[bit];
            cur.leaf--; // 如果减成 0 了，说明子树是空的，可以理解成 cur is None
        }
    }

    public int maxXor(int val) {
        Node cur = root;
        int ans = 0;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            if (cur.son[bit ^ 1] != null && cur.son[bit ^ 1].leaf > 0) {
                ans |= 1 << i;
                bit ^= 1;
            }
            cur = cur.son[bit];
        }
        return ans;
    }
}

class Solution {
    public int maxXor(int[] nums, int k) {
        int n = nums.length;
        int[] sum = new int[n + 1];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] ^ nums[i];
        }

        Trie t = new Trie();
        ArrayDeque<Integer> minQ = new ArrayDeque<>(); // 更快的写法见【Java 数组】
        ArrayDeque<Integer> maxQ = new ArrayDeque<>();
        int ans = 0;
        int left = 0;

        for (int right = 0; right < n; right++) {
            // 1. 入
            t.put(sum[right]);

            int x = nums[right];
            while (!minQ.isEmpty() && x <= nums[minQ.peekLast()]) {
                minQ.pollLast();
            }
            minQ.addLast(right);

            while (!maxQ.isEmpty() && x >= nums[maxQ.peekLast()]) {
                maxQ.pollLast();
            }
            maxQ.addLast(right);

            // 2. 出
            while (nums[maxQ.peekFirst()] - nums[minQ.peekFirst()] > k) {
                t.del(sum[left]);
                left++;
                if (minQ.peekFirst() < left) {
                    minQ.pollFirst();
                }
                if (maxQ.peekFirst() < left) {
                    maxQ.pollFirst();
                }
            }

            // 3. 更新答案
            ans = Math.max(ans, t.maxXor(sum[right + 1]));
        }

        return ans;
    }
}
```

```java [sol-Java 数组]
class Trie {
    private static final int WIDTH = 15; // nums[i] 二进制长度的最大值

    private static class Node {
        Node[] son = new Node[2];
        int leaf; // 子树叶子个数
    }

    private final Node root = new Node();

    public void put(int val) {
        Node cur = root;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            if (cur.son[bit] == null) {
                cur.son[bit] = new Node();
            }
            cur = cur.son[bit];
            cur.leaf++;
        }
    }

    public void del(int val) {
        Node cur = root;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            cur = cur.son[bit];
            cur.leaf--; // 如果减成 0 了，说明子树是空的，可以理解成 cur is None
        }
    }

    public int maxXor(int val) {
        Node cur = root;
        int ans = 0;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            if (cur.son[bit ^ 1] != null && cur.son[bit ^ 1].leaf > 0) {
                ans |= 1 << i;
                bit ^= 1;
            }
            cur = cur.son[bit];
        }
        return ans;
    }
}

class Solution {
    public int maxXor(int[] nums, int k) {
        int n = nums.length;
        int[] sum = new int[n + 1];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] ^ nums[i];
        }

        Trie t = new Trie();
        int[] minQ = new int[n];
        int[] maxQ = new int[n];
        int minHead = 0, minTail = -1;
        int maxHead = 0, maxTail = -1;
        int ans = 0;
        int left = 0;

        for (int right = 0; right < n; right++) {
            // 1. 入
            t.put(sum[right]);

            int x = nums[right];
            while (minHead <= minTail && x <= nums[minQ[minTail]]) {
                minTail--; // 右边出队
            }
            minQ[++minTail] = right; // 右边入队

            while (maxHead <= maxTail && x >= nums[maxQ[maxTail]]) {
                maxTail--; // 右边出队
            }
            maxQ[++maxTail] = right; // 右边入队

            // 2. 出
            while (nums[maxQ[maxHead]] - nums[minQ[minHead]] > k) {
                t.del(sum[left]);
                left++;
                if (minQ[minHead] < left) { // 队首不在窗口中
                    minHead++; // 左边出队
                }
                if (maxQ[maxHead] < left) { // 队首不在窗口中
                    maxHead++; // 左边出队
                }
            }

            // 3. 更新答案
            ans = Math.max(ans, t.maxXor(sum[right + 1]));
        }

        return ans;
    }
}
```

```cpp [sol-C++]
constexpr int WIDTH = 15; // nums[i] 二进制长度的最大值

struct Node {
    Node* son[2]{};
    int leaf = 0; // 子树叶子个数
};

class Trie {
    // 注：我没有写析构函数，想写的同学可以自己补上
    Node* root = new Node();

public:
    void put(int val) {
        Node* cur = root;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            if (!cur->son[bit]) {
                cur->son[bit] = new Node();
            }
            cur = cur->son[bit];
            cur->leaf++;
        }
    }

    void del(int val) {
        Node* cur = root;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            cur = cur->son[bit];
            cur->leaf--; // 如果减成 0 了，说明子树是空的，可以理解成 cur is None
        }
    }

    int max_xor(int val) {
        Node* cur = root;
        int ans = 0;
        for (int i = WIDTH - 1; i >= 0; i--) {
            int bit = val >> i & 1;
            if (cur->son[bit ^ 1] && cur->son[bit ^ 1]->leaf) {
                ans |= 1 << i;
                bit ^= 1;
            }
            cur = cur->son[bit];
        }
        return ans;
    }
};

class Solution {
public:
    int maxXor(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> sum(n + 1);
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] ^ nums[i];
        }

        Trie t;
        deque<int> min_q, max_q;
        int ans = 0, left = 0;
        for (int right = 0; right < n; right++) {
            // 1. 入
            t.put(sum[right]);

            int x = nums[right];
            while (!min_q.empty() && x <= nums[min_q.back()]) {
                min_q.pop_back();
            }
            min_q.push_back(right);

            while (!max_q.empty() && x >= nums[max_q.back()]) {
                max_q.pop_back();
            }
            max_q.push_back(right);

            // 2. 出
            while (nums[max_q.front()] - nums[min_q.front()] > k) {
                t.del(sum[left]);
                left++;
                if (min_q.front() < left) {
                    min_q.pop_front();
                }
                if (max_q.front() < left) {
                    max_q.pop_front();
                }
            }

            // 3. 更新答案
            ans = max(ans, t.max_xor(sum[right + 1]));
        }
        return ans;
    }
};
```

```go [sol-Go]
const width = 15 // nums[i] 二进制长度的最大值

type node struct {
	son  [2]*node
	leaf int // 子树叶子个数
}

type trie struct {
	root *node
}

func newTrie() *trie {
	return &trie{&node{}}
}

func (t *trie) put(val int) {
	cur := t.root
	for i := width - 1; i >= 0; i-- {
		bit := val >> i & 1
		if cur.son[bit] == nil {
			cur.son[bit] = &node{}
		}
		cur = cur.son[bit]
		cur.leaf++
	}
}

func (t *trie) del(val int) {
	cur := t.root
	for i := width - 1; i >= 0; i-- {
		cur = cur.son[val>>i&1]
		cur.leaf-- // 如果减成 0 了，说明子树是空的，可以理解成 cur == nil
	}
}

func (t *trie) maxXor(val int) (ans int) {
	cur := t.root
	for i := width - 1; i >= 0; i-- {
		bit := val >> i & 1
		if cur.son[bit^1] != nil && cur.son[bit^1].leaf > 0 {
			ans |= 1 << i
			bit ^= 1
		}
		cur = cur.son[bit]
	}
	return
}

func maxXor(nums []int, k int) (ans int) {
	sum := make([]int, len(nums)+1)
	for i, x := range nums {
		sum[i+1] = sum[i] ^ x
	}

	t := newTrie()
	var minQ, maxQ []int
	left := 0
	for right, x := range nums {
		// 1. 入
		t.put(sum[right])

		for len(minQ) > 0 && x <= nums[minQ[len(minQ)-1]] {
			minQ = minQ[:len(minQ)-1]
		}
		minQ = append(minQ, right)

		for len(maxQ) > 0 && x >= nums[maxQ[len(maxQ)-1]] {
			maxQ = maxQ[:len(maxQ)-1]
		}
		maxQ = append(maxQ, right)

		// 2. 出
		for nums[maxQ[0]]-nums[minQ[0]] > k {
			t.del(sum[left])
			left++
			if minQ[0] < left {
				minQ = minQ[1:]
			}
			if maxQ[0] < left {
				maxQ = maxQ[1:]
			}
		}

		// 3. 更新答案
		ans = max(ans, t.maxXor(sum[right+1]))
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(n\log U)$。

## 方法二：试填法

请先阅读 [421 题我的题解](https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/solutions/2511644/tu-jie-jian-ji-gao-xiao-yi-tu-miao-dong-1427d/)。

对于本题，我们要计算 $s[r+1]$ 与 $i$ 在 $[l,r]$ 中的一个 $s[i]$ 的异或值，其高位是否等于目标值 $\textit{newAns}$。

运用 421 题的技巧，转化成判断是否存在一个 $s[i]$，其高位等于 $s[r+1]\oplus \textit{newAns}$ 的高位，且 $i$ 在 $[l,r]$ 中。

在 421 题中，我们只需记录是否存在前缀和；本题需要记录前缀和**最近一次出现的位置** $\textit{last}$。

为了避免反复滑动窗口，可以**预处理**一个数组 $\textit{lefts}$，表示当窗口右端点在 $\textit{right}$ 时，窗口左端点在 $\textit{lefts}[\textit{right}]$。

```py [sol-Python3]
class Solution:
    def maxXor(self, nums: list[int], k: int) -> int:
        # 预处理：当窗口右端点在 right 时，窗口左端点在 lefts[right]
        n = len(nums)
        lefts = [0] * n
        min_q = deque()
        max_q = deque()
        left = 0
        for right, x in enumerate(nums):
            # 1. 入
            while min_q and x <= nums[min_q[-1]]:
                min_q.pop()
            min_q.append(right)

            while max_q and x >= nums[max_q[-1]]:
                max_q.pop()
            max_q.append(right)

            # 2. 出
            while nums[max_q[0]] - nums[min_q[0]] > k:
                left += 1
                if min_q[0] < left:
                    min_q.popleft()
                if max_q[0] < left:
                    max_q.popleft()

            # 3. 记录此时的 left
            lefts[right] = left

        pre = list(accumulate(nums, xor, initial=0))

        # 试填法
        width = max(nums).bit_length()
        ans = 0
        for i in range(width - 1, -1, -1):
            last = [-1] * (1 << (width - i))
            last[0] = 0  # pre[0] = 0 的位置是 0
            ans <<= 1
            new_ans = ans | 1
            for right in range(n):
                s = pre[right + 1] >> i  # 去掉低位，只看高位
                if last[new_ans ^ s] >= lefts[right]:  # new_ans ^ s 存在，且在窗口内
                    ans = new_ans  # 最终答案第 i 位填 1
                    break
                last[s] = right + 1

        return ans
```

```java [sol-Java]
class Solution {
    public int maxXor(int[] nums, int k) {
        // 预处理：当窗口右端点在 right 时，窗口左端点在 lefts[right]
        // 顺带算出前缀异或和、nums 的最大值
        int n = nums.length;
        int[] lefts = new int[n];
        int[] sum = new int[n + 1];
        int mx = 0;

        int[] minQ = new int[n];
        int[] maxQ = new int[n];
        int minHead = 0, minTail = -1;
        int maxHead = 0, maxTail = -1;
        int left = 0;

        for (int right = 0; right < n; right++) {
            int x = nums[right];
            sum[right + 1] = sum[right] ^ x;
            mx = Math.max(mx, x);

            // 1. 入
            while (minHead <= minTail && x <= nums[minQ[minTail]]) {
                minTail--; // 右边出队
            }
            minQ[++minTail] = right; // 右边入队

            while (maxHead <= maxTail && x >= nums[maxQ[maxTail]]) {
                maxTail--; // 右边出队
            }
            maxQ[++maxTail] = right; // 右边入队

            // 2. 出
            while (nums[maxQ[maxHead]] - nums[minQ[minHead]] > k) {
                left++;
                if (minQ[minHead] < left) { // 队首不在窗口中
                    minHead++; // 左边出队
                }
                if (maxQ[maxHead] < left) { // 队首不在窗口中
                    maxHead++; // 左边出队
                }
            }

            // 3. 记录此时的 left
            lefts[right] = left;
        }

        // 试填法
        int width = 32 - Integer.numberOfLeadingZeros(mx);
        int[] last = new int[1 << width];
        int ans = 0;

        for (int i = width - 1; i >= 0; i--) {
            Arrays.fill(last, 0, 1 << (width - i), -1);
            last[0] = 0; // sum[0] = 0 的位置是 0
            ans <<= 1;
            int newAns = ans | 1;
            for (int right = 0; right < n; right++) {
                int s = sum[right + 1] >> i; // 去掉低位，只看高位
                if (last[newAns ^ s] >= lefts[right]) { // newAns ^ s 存在，且在窗口内
                    ans = newAns; // 最终答案第 i 位填 1
                    break;
                }
                last[s] = right + 1;
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxXor(vector<int>& nums, int k) {
        // 预处理：当窗口右端点在 right 时，窗口左端点在 lefts[right]
        // 顺带算出前缀异或和、nums 的最大值
        int n = nums.size();
        vector<int> lefts(n);
        vector<int> sum(n + 1);
        int mx = 0;
        deque<int> min_q, max_q;
        int left = 0;
        for (int right = 0; right < n; right++) {
            int x = nums[right];
            sum[right + 1] = sum[right] ^ x;
            mx = max(mx, x);

            // 1. 入
            while (!min_q.empty() && x <= nums[min_q.back()]) {
                min_q.pop_back();
            }
            min_q.push_back(right);

            while (!max_q.empty() && x >= nums[max_q.back()]) {
                max_q.pop_back();
            }
            max_q.push_back(right);

            // 2. 出
            while (nums[max_q.front()] - nums[min_q.front()] > k) {
                left++;
                if (min_q.front() < left) {
                    min_q.pop_front();
                }
                if (max_q.front() < left) {
                    max_q.pop_front();
                }
            }

            // 3. 记录此时的 left
            lefts[right] = left;
        }

        // 试填法
        int width = bit_width((uint32_t) mx);
        vector<int> last(1 << width);
        int ans = 0;
        for (int i = width - 1; i >= 0; i--) {
            fill(last.begin(), last.begin() + (1 << (width - i)), -1);
            last[0] = 0; // sum[0] = 0 的位置是 0
            ans <<= 1;
            int new_ans = ans | 1;
            for (int right = 0; right < n; right++) {
                int s = sum[right + 1] >> i; // 去掉低位，只看高位
                if (last[new_ans ^ s] >= lefts[right]) { // new_ans ^ s 存在，且在窗口内
                    ans = new_ans; // 最终答案第 i 位填 1
                    break;
                }
                last[s] = right + 1;
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func maxXor(nums []int, k int) (ans int) {
	// 预处理：当窗口右端点在 right 时，窗口左端点在 lefts[right]
	// 顺带算出前缀异或和、nums 的最大值
	n := len(nums)
	lefts := make([]int, n)
	sum := make([]int, len(nums)+1)
	mx := 0
	var minQ, maxQ []int
	left := 0
	for right, x := range nums {
		sum[right+1] = sum[right] ^ x
		mx = max(mx, x)

		// 1. 入
		for len(minQ) > 0 && x <= nums[minQ[len(minQ)-1]] {
			minQ = minQ[:len(minQ)-1]
		}
		minQ = append(minQ, right)

		for len(maxQ) > 0 && x >= nums[maxQ[len(maxQ)-1]] {
			maxQ = maxQ[:len(maxQ)-1]
		}
		maxQ = append(maxQ, right)

		// 2. 出
		for nums[maxQ[0]]-nums[minQ[0]] > k {
			left++
			if minQ[0] < left {
				minQ = minQ[1:]
			}
			if maxQ[0] < left {
				maxQ = maxQ[1:]
			}
		}

		// 3. 记录此时的 left
		lefts[right] = left
	}

	// 试填法
	width := bits.Len(uint(mx))
	last := make([]int, 1<<width)
	for i := width - 1; i >= 0; i-- {
		for j := range 1 << (width - i) {
			last[j] = -1
		}
		last[0] = 0 // sum[0] = 0 的位置是 0
		ans <<= 1
		newAns := ans | 1
		for right, l := range lefts {
			s := sum[right+1] >> i // 去掉低位，只看高位
			if last[newAns^s] >= l { // newAns^s 存在，且在窗口内
				ans = newAns // 最终答案第 i 位填 1
				break
			}
			last[s] = right + 1
		}
	}

	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(n + U)$，取决于 $\textit{last}$ 是哈希表还是数组。

## 专题训练

1. 数据结构题单的「**§4.4 单调队列**」和「**§6.4 0-1 字典树**」。
2. 位运算题单的「**五、试填法**」。

其中和本题相似的题目是 [2935. 找出强数对的最大异或值 II](https://leetcode.cn/problems/maximum-strong-pair-xor-ii/)。

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

## 本地原创解析

### 1. 题意重述

本题来自 `五、试填法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、试填法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
