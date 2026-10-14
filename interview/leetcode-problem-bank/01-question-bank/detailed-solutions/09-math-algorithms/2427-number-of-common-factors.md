# 2427. 公因子的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-common-factors/
- 题目 slug：`number-of-common-factors`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.5 因子
- 难度分：1172
- 外部题解来源：https://leetcode.cn/problems/number-of-common-factors/solutions/1864071/mei-ju-yin-zi-by-endlesscheng-v3fb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[计算 gcd(a,b) 的因子（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-common-factors/solutions/1864071/mei-ju-yin-zi-by-endlesscheng-v3fb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-yin-zi-by-endlesscheng-v3fb`
- topic id：`1864071`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

### 视频讲解

见[【周赛 313】](https://www.bilibili.com/video/BV1kd4y1q7fC)

### 思路

枚举因子，挨个判断能否整除 $a$ 和 $b$。

改进方案是枚举 $a$ 和 $b$ 的最大公因数的因子。

```py [sol1-Python3]
class Solution:
    def commonFactors(self, a: int, b: int) -> int:
        g = gcd(a, b)
        ans, i = 0, 1
        while i * i <= g:
            if g % i == 0:
                ans += 1  # i 是公因子
                if i * i < g:
                    ans += 1  # g/i 是公因子
            i += 1
        return ans
```

```java [sol1-Java]
class Solution {
    public int commonFactors(int a, int b) {
        int ans = 0, g = gcd(a, b);
        for (int i = 1; i * i <= g; ++i)
            if (g % i == 0) {
                ++ans; // i 是公因子
                if (i * i < g)
                    ++ans; // g/i 是公因子
            }
        return ans;
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int commonFactors(int a, int b) {
        int ans = 0, g = gcd(a, b);
        for (int i = 1; i * i <= g; ++i)
            if (g % i == 0) {
                ++ans; // i 是公因子
                if (i * i < g)
                    ++ans; // g/i 是公因子
            }
        return ans;
    }
};
```

```go [sol1-Go]
func commonFactors(a, b int) (ans int) {
	g := gcd(a, b)
	for i := 1; i*i <= g; i++ {
		if g%i == 0 {
			ans++ // i 是公因子
			if i*i < g {
				ans++ // g/i 是公因子
			}
		}
	}
	return
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

#### 复杂度分析

- 时间复杂度：$O(\sqrt{\min(a,b)})$。
- 空间复杂度：$O(1)$，仅用到若干变量。
