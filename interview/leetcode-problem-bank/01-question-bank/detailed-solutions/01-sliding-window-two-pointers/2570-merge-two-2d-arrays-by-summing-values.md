# 2570. 合并两个二维数组 - 求和法

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-two-2d-arrays-by-summing-values/
- 题目 slug：`merge-two-2d-arrays-by-summing-values`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.1 双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/merge-two-2d-arrays-by-summing-values/solutions/2120224/xian-xing-zuo-fa-gui-bing-pai-xu-o1-e-wa-iy7u/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[线性做法：归并排序 + O(1) 额外空间](https://leetcode.cn/problems/merge-two-2d-arrays-by-summing-values/solutions/2120224/xian-xing-zuo-fa-gui-bing-pai-xu-o1-e-wa-iy7u/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xian-xing-zuo-fa-gui-bing-pai-xu-o1-e-wa-iy7u`
- topic id：`2120224`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

下午两点【biIibiIi@灵茶山艾府】直播讲题，记得关注哦~

---

做法和归并排序是一样的。

```py [sol1-Python3]
class Solution:
    def mergeArrays(self, a: List[List[int]], b: List[List[int]]) -> List[List[int]]:
        ans = []
        i, n = 0, len(a)
        j, m = 0, len(b)
        while True:
            if i == n:
                ans.extend(b[j:])
                return ans
            if j == m:
                ans.extend(a[i:])
                return ans
            if a[i][0] < b[j][0]:
                ans.append(a[i])
                i += 1
            elif a[i][0] > b[j][0]:
                ans.append(b[j])
                j += 1
            else:
                a[i][1] += b[j][1]
                ans.append(a[i])
                i += 1
                j += 1
```

```go [sol1-Go]
func mergeArrays(a, b [][]int) (ans [][]int) {
	i, n := 0, len(a)
	j, m := 0, len(b)
	for {
		if i == n {
			return append(ans, b[j:]...)
		}
		if j == m {
			return append(ans, a[i:]...)
		}
		if a[i][0] < b[j][0] {
			ans = append(ans, a[i])
			i++
		} else if a[i][0] > b[j][0] {
			ans = append(ans, b[j])
			j++
		} else {
			a[i][1] += b[j][1]
			ans = append(ans, a[i])
			i++
			j++
		}
	}
}
```

### 复杂度分析

- 时间复杂度：$O(n+m)$，其中 $n$ 为 $\textit{nums}_1$ 的长度，$m$ 为 $\textit{nums}_2$ 的长度。
- 空间复杂度：$O(1)$。不计入返回值，仅用到若干额外变量。
