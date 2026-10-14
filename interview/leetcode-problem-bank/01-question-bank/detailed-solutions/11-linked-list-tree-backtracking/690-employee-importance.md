# 690. 员工的重要性

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/employee-importance/
- 题目 slug：`employee-importance`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.3 自底向上 DFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/employee-importance/solutions/2892882/ha-xi-biao-dfspythonjavacgojs-by-endless-rbx0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[哈希表+DFS（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/employee-importance/solutions/2892882/ha-xi-biao-dfspythonjavacgojs-by-endless-rbx0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ha-xi-biao-dfspythonjavacgojs-by-endless-rbx0`
- topic id：`2892882`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

首先，创建一个哈希表，把第 $i$ 个员工的 ID 映射到 $\textit{employees}[i]$ 对象上。

然后从 $\textit{id}$ 开始 DFS（题目保证员工组成了树形结构）：

1. 根据 $\textit{id}$ 从哈希表中获取到对应的员工对象 $e$。
2. 把 $e.\textit{importance}$ 加到返回值中。
3. 遍历 $e.\textit{subordinates}$，继续 DFS 下属的 ID，把 $\textit{dfs}(\textit{subId})$ 加到返回值中。

```py [sol-Python3]
class Solution:
    def getImportance(self, employees: List['Employee'], id: int) -> int:
        employees = {e.id: e for e in employees}
        def dfs(id: int) -> int:
            e = employees[id]
            return e.importance + sum(dfs(sub) for sub in e.subordinates)
        return dfs(id)
```

```java [sol-Java]
class Solution {
    public int getImportance(List<Employee> employees, int id) {
        Map<Integer, Employee> employeeMap = new HashMap<>(employees.size());
        for (Employee e : employees) {
            employeeMap.put(e.id, e);
        }
        return dfs(employeeMap, id);
    }

    private int dfs(Map<Integer, Employee> employeeMap, int id) {
        Employee e = employeeMap.get(id);
        int res = e.importance;
        for (int subId : e.subordinates) {
            res += dfs(employeeMap, subId);
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int getImportance(vector<Employee*>& employees, int id) {
        unordered_map<int, Employee*> employee_map;
        for (auto e : employees) {
            employee_map[e->id] = e;
        }
        auto dfs = [&](auto&& dfs, int id) -> int {
            auto e = employee_map[id];
            int res = e->importance;
            for (int sub_id : e->subordinates) {
                res += dfs(dfs, sub_id);
            }
            return res;
        };
        return dfs(dfs, id);
    }
};
```

```go [sol-Go]
func getImportance(employees []*Employee, id int) int {
    employeeMap := make(map[int]*Employee, len(employees))
    for _, e := range employees {
        employeeMap[e.Id] = e
    }
    var dfs func(int) int
    dfs = func(id int) int {
        e := employeeMap[id]
        res := e.Importance
        for _, subId := range e.Subordinates {
            res += dfs(subId)
        }
        return res
    }
    return dfs(id)
}
```

```js [sol-JavaScript]
var GetImportance = function(employees, id) {
    const employeeMap = new Map();
    for (const e of employees) {
        employeeMap.set(e.id, e)
    }
    function dfs(id) {
        const e = employeeMap.get(id);
        let res = e.importance;
        for (const subId of e.subordinates) {
            res += dfs(subId);
        }
        return res;
    }
    return dfs(id);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{employees}$ 的长度。由于员工组成了树形结构，每个员工至多访问一次。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面树题单中的「**一般树**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
