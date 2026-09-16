# E095. 专家版：LRU 缓存

## 元数据

- 难度：专家级
- 知识域：K11 高级数据结构与在线算法
- 标签：哈希表、双向链表、设计
- 公司标签：阿里, 腾讯 `[推断]`
- 面试频率：专题强化 `[推断]`
- 前置题：E094
- 关联题：E096
- 事实状态：[已确认] 经典算法与复杂度结论；[推断] 公司训练标签；[待验证] 除抽样编译外仍需按目标平台运行完整样例

## 1. 题目与约束

设计 LRU 缓存，支持 `get(key)` 和 `put(key,value)`，容量满时淘汰最近最少使用的键。

约束：要求平均 `O(1)` 操作；容量为正整数。 本题按专家追问处理：先澄清输入规模、在线/离线、是否允许修改数据、并发或持久化要求，再给出可验证方案。

## 2. 面试思路

每次访问后在线性容器中移动元素会导致 `O(n)`，不能满足高频访问场景。

优化切入点：哈希表负责按 key `O(1)` 定位节点，双向链表负责 `O(1)` 删除和移动节点；链表头表示最近使用，尾表示最久未使用。 面试表达时建议先说清楚状态含义，再说明为什么该状态足以覆盖所有答案。

## 3. 算法与正确性

算法步骤：用 `list<pair<int,int>>` 保存从新到旧的键值对，`unordered_map<int, list<...>::iterator>` 定位节点。访问或更新时把节点移动到头部；插入超容量时删除链表尾部。

正确性说明：每次 `get` 或 `put` 都把被访问键移动到表头，因此链表顺序始终等于从最近到最久的访问顺序。容量超限时尾部节点正是最近最少使用节点，删除它符合 LRU 规则。哈希表和链表同步维护，所以定位和更新正确。

## 4. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class LRUCache {
    int cap;
    list<pair<int,int>> items;
    unordered_map<int, list<pair<int,int>>::iterator> pos;
public:
    explicit LRUCache(int capacity) : cap(capacity) {}
    int get(int key) {
        auto it = pos.find(key);
        if (it == pos.end()) return -1;
        items.splice(items.begin(), items, it->second);
        return it->second->second;
    }
    void put(int key, int value) {
        auto it = pos.find(key);
        if (it != pos.end()) {
            it->second->second = value;
            items.splice(items.begin(), items, it->second);
            return;
        }
        items.push_front({key, value});
        pos[key] = items.begin();
        if ((int)items.size() > cap) {
            int old = items.back().first;
            items.pop_back();
            pos.erase(old);
        }
    }
};

int main() { LRUCache cache(2); cache.put(1, 1); return cache.get(1) == 1 ? 0 : 1; }
```

实现细节：代码使用标准库容器，不依赖平台扩展；若题目平台要求类名 `Solution`，可把 `solve` 函数移动到对应类中。面试手写时优先保证边界清晰，再考虑微优化。

## 5. 复杂度

`get`/`put` 平均时间 `O(1)`，空间 `O(capacity)`。 复杂度成立的前提是输入满足题目约束；如果输出本身很大，需要把输出规模计入空间和时间。

## 6. 例子推演

容量 2：`put(1,1), put(2,2), get(1), put(3,3)` 后，键 2 最久未使用被淘汰。

推演时不要只给最终答案，应说明关键变量、数据结构或状态如何变化；这能帮助面试官判断你是否真正理解算法。

## 7. 测试与调试

建议测试：更新已有 key；连续 get 改变新旧顺序；容量为 1；访问不存在 key 返回 -1。

调试切入点：先用最小样例确认基础状态，再用边界样例检查空输入、重复值、极端大小和溢出；若能写暴力解，可用随机数据做差分测试。

## 8. 面试追问

1. 如果输入规模扩大 10 倍，瓶颈会出现在时间、空间还是常数因子？
2. 如果要返回构造过程或所有答案，输出规模会如何改变复杂度？
3. 如果输入变成数据流，状态如何维护，如何恢复和回放？
4. 如果部署到多线程服务，缓存、共享状态和一致性如何处理？

## 9. 易错点与反例

- 只记住模板而不解释不变量，遇到约束变化时容易套错。
- 忽略空输入、重复值、负数、溢出或输出规模。
- 把未运行样例说成已验证；本题库中没有实际运行记录的结论统一保持 `[待验证]`。

## 10. 变体与迁移

本题可迁移到同知识域 `K11` 的相邻题：当题目仍满足相同的不变量、单调性、递推关系或连通性定义时，可复用该模式；一旦约束变为在线、并发、需要所有解或需要稳定顺序，就必须重新评估数据结构和复杂度。
