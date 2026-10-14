# 2102. 序列顺序查询

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sequentially-ordinal-rank-tracker/
- 题目 slug：`sequentially-ordinal-rank-tracker`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）
- 难度分：2159
- 外部题解来源：https://leetcode.cn/problems/sequentially-ordinal-rank-tracker/solutions/1152448/qiao-miao-li-yong-cha-xun-de-te-shu-xing-7eyg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[巧妙利用查询的特殊性](https://leetcode.cn/problems/sequentially-ordinal-rank-tracker/solutions/1152448/qiao-miao-li-yong-cha-xun-de-te-shu-xing-7eyg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qiao-miao-li-yong-cha-xun-de-te-shu-xing-7eyg`
- topic id：`1152448`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

为了高效插入数据，我们可以用平衡树来维护所有景点。

注意到每次 `get` 后仅会将查询次数加一，我们可以用一个迭代器（指针）$\textit{cur}$ 指向查询需要返回的元素，这样对于查询操作，每次查询结束后将 $\textit{cur}$ 移至其下一个元素。

对于添加操作，如果添加的景点排在 $\textit{cur}$ 前面，那么为了保证 $\textit{cur}$ 仍然指向第 $i$ 好的景点，我们将 $\textit{cur}$ 移至其前一个元素，否则不移动 $\textit{cur}$。

代码实现时，可以在初始化时插入一个哨兵元素，从而简化判断逻辑。

```C++ [sol1-C++]
class SORTracker {
    set<pair<int, string>> s;
    set<pair<int, string>>::iterator cur;

public:
    SORTracker() {
        s.emplace(0, ""); // 哨兵
        cur = s.begin();
    }

    void add(string name, int score) {
        pair<int, string> p = {-score, name};
        s.emplace(p);
        if (p < *cur) --cur;
    }

    string get() {
        return cur++->second;
    }
};
```

```go [sol1-Go]
type pair struct {
	score int
	name  string
}

func compare(x, y interface{}) int {
	a, b := x.(pair), y.(pair)
	if a.score > b.score || a.score == b.score && a.name < b.name {
		return -1
	}
	return 1
}

type SORTracker struct {
	*redblacktree.Tree
	cur redblacktree.Iterator
}

func Constructor() SORTracker {
	root := redblacktree.NewWith(compare)
	root.Put(pair{}, nil) // 哨兵
	return SORTracker{root, root.IteratorAt(root.Left())}
}

func (t *SORTracker) Add(name string, score int) {
	p := pair{score, name}
	t.Put(p, nil)
	if compare(p, t.cur.Key()) < 0 {
		t.cur.Prev() // 移动至前一个元素
	}
}

func (t *SORTracker) Get() string {
	name := t.cur.Key().(pair).name
	t.cur.Next() // 移动至下一个元素
	return name
}
```

Python 用户可以直接用 `SortedList` 模拟。

```python [sol2-Python3]
from sortedcontainers import SortedList

class SORTracker:
    def __init__(self):
        self.d = SortedList()
        self.i = 0

    def add(self, name: str, score: int) -> None:
        self.d.add((-score, name))

    def get(self) -> str:
        self.i += 1
        return self.d[self.i - 1][1]
```
