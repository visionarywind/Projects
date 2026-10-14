# 2477. 到达首都的最少油耗

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-fuel-cost-to-report-to-the-capital/
- 题目 slug：`minimum-fuel-cost-to-report-to-the-capital`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.5 贡献法
- 难度分：2012
- 外部题解来源：https://leetcode.cn/problems/minimum-fuel-cost-to-report-to-the-capital/solutions/1981361/kao-lu-mei-tiao-bian-shang-zhi-shao-xu-y-uamv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】没有思路？一张图秒懂！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-fuel-cost-to-report-to-the-capital/solutions/1981361/kao-lu-mei-tiao-bian-shang-zhi-shao-xu-y-uamv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kao-lu-mei-tiao-bian-shang-zhi-shao-xu-y-uamv`
- topic id：`1981361`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

![LC2477-c.png](https://pic.leetcode.cn/1701652114-HoRVil-LC2477-c.png)

```py [sol-Python3]
class Solution:
    def minimumFuelCost(self, roads: List[List[int]], seats: int) -> int:
        g = [[] for _ in range(len(roads) + 1)]
        for x, y in roads:
            g[x].append(y)  # 记录每个点的邻居
            g[y].append(x)

        ans = 0
        def dfs(x: int, fa: int) -> int:
            size = 1
            for y in g[x]:
                if y != fa:  # 递归子节点，不能递归父节点
                    size += dfs(y, x)  # 统计子树大小
            if x:  # x 不是根节点
                nonlocal ans
                ans += (size - 1) // seats + 1  # ceil(size/seats)
            return size
        dfs(0, -1)
        return ans
```

```java [sol-Java]
class Solution {
    public long minimumFuelCost(int[][] roads, int seats) {
        int n = roads.length + 1;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : roads) {
            int x = e[0], y = e[1];
            g[x].add(y); // 记录每个点的邻居
            g[y].add(x);
        }

        dfs(0, -1, g, seats);
        return ans;
    }

    private long ans;

    private int dfs(int x, int fa, List<Integer>[] g, int seats) {
        int size = 1;
        for (int y : g[x]) {
            if (y != fa) { // 递归子节点，不能递归父节点
                size += dfs(y, x, g, seats); // 统计子树大小
            }
        }
        if (x > 0) { // x 不是根节点
            ans += (size - 1) / seats + 1; // ceil(size/seats)
        }
        return size;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumFuelCost(vector<vector<int>> &roads, int seats) {
        vector<vector<int>> g(roads.size() + 1);
        for (auto &e: roads) {
            int x = e[0], y = e[1];
            g[x].push_back(y); // 记录每个点的邻居
            g[y].push_back(x);
        }

        long long ans = 0;
        function<int(int, int)> dfs = [&](int x, int fa) -> int {
            int size = 1;
            for (int y: g[x]) {
                if (y != fa) { // 递归子节点，不能递归父节点
                    size += dfs(y, x); // 统计子树大小
                }
            }
            if (x) { // x 不是根节点
                ans += (size - 1) / seats + 1; // ceil(size/seats)
            }
            return size;
        };
        dfs(0, -1);
        return ans;
    }
};
```

```go [sol-Go]
func minimumFuelCost(roads [][]int, seats int) (ans int64) {
	g := make([][]int, len(roads)+1)
	for _, e := range roads {
		x, y := e[0], e[1]
		g[x] = append(g[x], y) // 记录每个点的邻居
		g[y] = append(g[y], x)
	}

	var dfs func(int, int) int
	dfs = func(x, fa int) int {
		size := 1
		for _, y := range g[x] {
			if y != fa { // 递归子节点，不能递归父节点
				size += dfs(y, x) // 统计子树大小
			}
		}
		if x > 0 { // x 不是根节点
			ans += int64((size-1)/seats + 1) // ceil(size/seats)
		}
		return size
	}
	dfs(0, -1)
	return
}
```

```js [sol-JavaScript]
var minimumFuelCost = function (roads, seats) {
    const g = Array(roads.length + 1).fill(null).map(() => []);
    for (const [x, y] of roads) {
        g[x].push(y); // 记录每个点的邻居
        g[y].push(x);
    }

    let ans = 0;
    function dfs(x, fa) {
        let size = 1;
        for (const y of g[x]) {
            if (y !== fa) { // 递归子节点，不能递归父节点
                size += dfs(y, x); // 统计子树大小
            }
        }
        if (x !== 0) { // x 不是根节点
            ans += Math.ceil(size / seats);
        }
        return size;
    }
    dfs(0, -1);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_fuel_cost(roads: Vec<Vec<i32>>, seats: i32) -> i64 {
        let mut g = vec![vec![]; roads.len() + 1];
        for e in &roads {
            let x = e[0] as usize;
            let y = e[1] as usize;
            g[x].push(y); // 记录每个点的邻居
            g[y].push(x);
        }
        let mut ans = 0i64;
        Self::dfs(0, 0, &g, seats, &mut ans);
        ans
    }

    fn dfs(x: usize, fa: usize, g: &Vec<Vec<usize>>, seats: i32, ans: &mut i64) -> i32 {
        let mut size = 1;
        for &y in &g[x] {
            if y != fa { // 递归子节点，不能递归父节点
                size += Self::dfs(y, x, g, seats, ans); // 统计子树大小
            }
        }
        if x != 0 { // x 不是根节点
            *ans += ((size - 1) / seats + 1) as i64; // ceil(size/seats)
        }
        size
    }
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{roads}$ 的长度。递归这棵树，每个节点至多访问一次。
- 空间复杂度：$O(n)$。

## 相似题目

- [979. 在二叉树中分配硬币](https://leetcode.cn/problems/distribute-coins-in-binary-tree/)，[我的题解](https://leetcode.cn/problems/distribute-coins-in-binary-tree/solution/tu-jie-mei-you-si-lu-jin-lai-miao-dong-p-vrni/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
