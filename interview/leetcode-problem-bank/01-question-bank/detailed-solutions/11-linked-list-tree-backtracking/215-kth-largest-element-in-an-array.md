# 215. 数组中的第K个最大元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kth-largest-element-in-an-array/
- 题目 slug：`kth-largest-element-in-an-array`
- 来源专题：链表、树与回溯
- 来源分类路径：五、其他递归/分治
- 难度分：912
- 外部题解来源：https://leetcode.cn/problems/kth-largest-element-in-an-array/solutions/3799769/on-kuai-su-xuan-ze-suan-fa-pythonjavaccg-lh7c/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 快速选择算法，附常见问题解答（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/kth-largest-element-in-an-array/solutions/3799769/on-kuai-su-xuan-ze-suan-fa-pythonjavaccg-lh7c/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-kuai-su-xuan-ze-suan-fa-pythonjavaccg-lh7c`
- topic id：`3799769`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

## 核心思路

第 $k$ 大元素在升序数组中的下标是 $n-k$。

1. 在 $\textit{nums}$ 中随机选择一个基准元素 $\textit{pivot}$。关于为什么要随机，见文末答疑。
2. 划分 $\textit{nums}$。通过交换，把 $<\textit{pivot}$ 的元素放在 $\textit{pivot}$ 的左侧，把 $\ge \textit{pivot}$ 的元素放在 $\textit{pivot}$ 的右侧。如此划分可以让我们粗略地排序 $\textit{nums}$。划分后，$\textit{pivot}$ 此刻的位置就等于 $\textit{pivot}$ 在升序数组中的位置。
3. 设 $\textit{pivot}$ 在 $\textit{nums}$ 中的下标为 $i$。
    - 如果 $i = n-k$，那么答案就是 $\textit{pivot}$。
    - 如果 $i > n-k$，说明答案在 $\textit{pivot}$ 左侧，我们在其中寻找，回到第一步。
    - 如果 $i < n-k$，说明答案在 $\textit{pivot}$ 右侧，我们在其中寻找，回到第一步。
    - 这类似 [二分查找](https://www.bilibili.com/video/BV1AP41137w7/)，只要我们每次能把问题的规模缩小一半，就可以用 $\mathcal{O}(n)$ 时间解决（见复杂度分析）。
    - 问题规模缩小后，相当于在 $\textit{nums}$ 的一个子数组中，继续划分子数组，寻找答案。

然而，如果按照 $<\textit{pivot}$ 和 $\ge\textit{pivot}$ 划分数组，这个做法会在数组包含大量重复元素时，划分后的 $i$ 往往是子数组第一个元素的下标，算法会退化至 $\mathcal{O}(n^2)$。

**解决办法**：修改第二步，把 $<$ 改成 $\le$，也就是把 $\le \textit{pivot}$ 的元素放在 $\textit{pivot}$ 的左侧，把 $\ge\textit{pivot}$ 的元素放在 $\textit{pivot}$ 的右侧。特别地，如果子数组所有元素都相同，这样做可以完美地返回子数组的中心下标（见代码），避免复杂度退化。

具体要如何交换元素？实现细节见代码注释。

```py [sol-Python3]
class Solution:
    def partition(self, nums: List[int], left: int, right: int) -> int:
        """
        在子数组 [left, right] 中随机选择一个基准元素 pivot
        根据 pivot 重新排列子数组 [left, right]
        重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
        返回 pivot 在重新排列后的 nums 中的下标
        特别地，如果子数组的所有元素都等于 pivot，我们会返回子数组的中心下标，避免退化
        """

        # 1. 在子数组 [left, right] 中随机选择一个基准元素 pivot
        i = randint(left, right)
        pivot = nums[i]
        # 把 pivot 与子数组第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
        nums[i], nums[left] = nums[left], nums[i]

        # 2. 相向双指针遍历子数组 [left + 1, right]
        # 循环不变量：在循环过程中，子数组的数据分布始终如下图
        # [ pivot | <=pivot | 尚未遍历 | >=pivot ]
        #   ^                 ^     ^         ^
        #   left              i     j         right

        i, j = left + 1, right
        while True:
            while i <= j and nums[i] < pivot:
                i += 1
            # 此时 nums[i] >= pivot

            while i <= j and nums[j] > pivot:
                j -= 1
            # 此时 nums[j] <= pivot

            if i >= j:
                break

            # 维持循环不变量
            nums[i], nums[j] = nums[j], nums[i]
            i += 1
            j -= 1

        # 循环结束后
        # [ pivot | <=pivot | >=pivot ]
        #   ^             ^   ^     ^
        #   left          j   i     right

        # 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
        # 为什么与 j 交换？
        # 如果与 i 交换，可能会出现 i = right + 1 的情况，已经下标越界了，无法交换
        # 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
        # 与 j 交换，即使 j = left，交换也不会出错
        nums[left], nums[j] = nums[j], nums[left]

        # 交换后
        # [ <=pivot | pivot | >=pivot ]
        #               ^
        #               j

        # 返回 pivot 的下标
        return j

    def findKthLargest(self, nums: list[int], k: int) -> int:
        n = len(nums)
        target_index = n - k  # 第 k 大元素在升序数组中的下标是 n - k
        left, right = 0, n - 1  # 闭区间
        while True:
            i = self.partition(nums, left, right)
            if i == target_index:
                # 找到第 k 大元素
                return nums[i]
            if i > target_index:
                # 第 k 大元素在 [left, i - 1] 中
                right = i - 1
            else:
                # 第 k 大元素在 [i + 1, right] 中
                left = i + 1
```

```java [sol-Java]
class Solution {
    private static final Random rand = new Random();

    public int findKthLargest(int[] nums, int k) {
        int n = nums.length;
        int targetIndex = n - k; // 第 k 大元素在升序数组中的下标是 n - k
        int left = 0;
        int right = n - 1; // 闭区间
        while (true) {
            int i = partition(nums, left, right);
            if (i == targetIndex) {
                // 找到第 k 大元素
                return nums[i];
            }
            if (i > targetIndex) {
                // 第 k 大元素在 [left, i - 1] 中
                right = i - 1;
            } else {
                // 第 k 大元素在 [i + 1, right] 中
                left = i + 1;
            }
        }
    }

    // 在子数组 [left, right] 中随机选择一个基准元素 pivot
    // 根据 pivot 重新排列子数组 [left, right]
    // 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
    // 返回 pivot 在重新排列后的 nums 中的下标
    // 特别地，如果子数组的所有元素都等于 pivot，我们会返回子数组的中心下标，避免退化
    private int partition(int[] nums, int left, int right) {
        // 1. 在子数组 [left, right] 中随机选择一个基准元素 pivot
        int i = left + rand.nextInt(right - left + 1);
        int pivot = nums[i];
        // 把 pivot 与子数组第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
        swap(nums, i, left);

        // 2. 相向双指针遍历子数组 [left + 1, right]
        // 循环不变量：在循环过程中，子数组的数据分布始终如下图
        // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
        //   ^                 ^     ^         ^
        //   left              i     j         right

        i = left + 1;
        int j = right;
        while (true) {
            while (i <= j && nums[i] < pivot) {
                i++;
            }
            // 此时 nums[i] >= pivot

            while (i <= j && nums[j] > pivot) {
                j--;
            }
            // 此时 nums[j] <= pivot

            if (i >= j) {
                break;
            }

            // 维持循环不变量
            swap(nums, i, j);
            i++;
            j--;
        }

        // 循环结束后
        // [ pivot | <=pivot | >=pivot ]
        //   ^             ^   ^     ^
        //   left          j   i     right

        // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
        // 为什么与 j 交换？
        // 如果与 i 交换，可能会出现 i = right + 1 的情况，已经下标越界了，无法交换
        // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
        // 与 j 交换，即使 j = left，交换也不会出错
        swap(nums, left, j);

        // 交换后
        // [ <=pivot | pivot | >=pivot ]
        //               ^
        //               j

        // 返回 pivot 的下标
        return j;
    }

    // 交换 nums[i] 与 nums[j]
    private void swap(int[] nums, int i, int j) {
        int tmp = nums[i];
        nums[i] = nums[j];
        nums[j] = tmp;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 在子数组 [left, right] 中随机选择一个基准元素 pivot
    // 根据 pivot 重新排列子数组 [left, right]
    // 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
    // 返回 pivot 在重新排列后的 nums 中的下标
    // 特别地，如果子数组的所有元素都等于 pivot，我们会返回子数组的中心下标，避免退化
    int partition(vector<int>& nums, int left, int right) {
        // 1. 在子数组 [left, right] 中随机选择一个基准元素 pivot
        int i = left + rand() % (right - left + 1);
        int pivot = nums[i];
        // 把 pivot 与子数组第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
        swap(nums[i], nums[left]);

        // 2. 相向双指针遍历子数组 [left + 1, right]
        // 循环不变量：在循环过程中，子数组的数据分布始终如下图
        // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
        //   ^                 ^     ^         ^
        //   left              i     j         right

        i = left + 1;
        int j = right;
        while (true) {
            while (i <= j && nums[i] < pivot) {
                i++;
            }
            // 此时 nums[i] >= pivot

            while (i <= j && nums[j] > pivot) {
                j--;
            }
            // 此时 nums[j] <= pivot

            if (i >= j) {
                break;
            }

            // 维持循环不变量
            swap(nums[i], nums[j]);
            i++;
            j--;
        }

        // 循环结束后
        // [ pivot | <=pivot | >=pivot ]
        //   ^             ^   ^     ^
        //   left          j   i     right

        // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
        // 为什么与 j 交换？
        // 如果与 i 交换，可能会出现 i = right + 1 的情况，已经下标越界了，无法交换
        // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
        // 与 j 交换，即使 j = left，交换也不会出错
        swap(nums[left], nums[j]);

        // 交换后
        // [ <=pivot | pivot | >=pivot ]
        //               ^
        //               j

        // 返回 pivot 的下标
        return j;
    }

public:
    int findKthLargest(vector<int>& nums, int k) {
        srand(time(NULL));
        int n = nums.size();
        int target_index = n - k; // 第 k 大元素在升序数组中的下标是 n - k
        int left = 0, right = n - 1; // 闭区间
        while (true) {
            int i = partition(nums, left, right);
            if (i == target_index) {
                // 找到第 k 大元素
                return nums[i];
            }
            if (i > target_index) {
                // 第 k 大元素在 [left, i - 1] 中
                right = i - 1;
            } else {
                // 第 k 大元素在 [i + 1, right] 中
                left = i + 1;
            }
        }
    }
};
```

```c [sol-C]
#define SWAP(a, b) do { int tmp = (a); (a) = (b); (b) = tmp; } while (0)

// 在子数组 [left, right] 中随机选择一个基准元素 pivot
// 根据 pivot 重新排列子数组 [left, right]
// 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
// 返回 pivot 在重新排列后的 nums 中的下标
// 特别地，如果子数组的所有元素都等于 pivot，我们会返回子数组的中心下标，避免退化
int partition(int* nums, int left, int right) {
    // 1. 在子数组 [left, right] 中随机选择一个基准元素 pivot
    int i = left + rand() % (right - left + 1);
    int pivot = nums[i];
    // 把 pivot 与子数组第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
    SWAP(nums[i], nums[left]);

    // 2. 相向双指针遍历子数组 [left + 1, right]
    // 循环不变量：在循环过程中，子数组的数据分布始终如下图
    // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
    //   ^                 ^     ^         ^
    //   left              i     j         right

    i = left + 1;
    int j = right;
    while (true) {
        while (i <= j && nums[i] < pivot) {
            i++;
        }
        // 此时 nums[i] >= pivot

        while (i <= j && nums[j] > pivot) {
            j--;
        }
        // 此时 nums[j] <= pivot

        if (i >= j) {
            break;
        }

        // 维持循环不变量
        SWAP(nums[i], nums[j]);
        i++;
        j--;
    }

    // 循环结束后
    // [ pivot | <=pivot | >=pivot ]
    //   ^             ^   ^     ^
    //   left          j   i     right

    // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
    // 为什么与 j 交换？
    // 如果与 i 交换，可能会出现 i = right + 1 的情况，已经下标越界了，无法交换
    // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
    // 与 j 交换，即使 j = left，交换也不会出错
    SWAP(nums[left], nums[j]);

    // 交换后
    // [ <=pivot | pivot | >=pivot ]
    //               ^
    //               j

    // 返回 pivot 的下标
    return j;
}

int findKthLargest(int* nums, int numsSize, int k) {
    srand(time(NULL));
    int target_index = numsSize - k; // 第 k 大元素在升序数组中的下标是 n - k
    int left = 0, right = numsSize - 1; // 闭区间
    while (true) {
        int i = partition(nums, left, right);
        if (i == target_index) {
            // 找到第 k 大元素
            return nums[i];
        }
        if (i > target_index) {
            // 第 k 大元素在 [left, i - 1] 中
            right = i - 1;
        } else {
            // 第 k 大元素在 [i + 1, right] 中
            left = i + 1;
        }
    }
}
```

```go [sol-Go]
// 在子数组 [left, right] 中随机选择一个基准元素 pivot
// 根据 pivot 重新排列子数组 [left, right]
// 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
// 返回 pivot 在重新排列后的 nums 中的下标
// 特别地，如果子数组的所有元素都等于 pivot，我们会返回子数组的中心下标，避免退化
func partition(nums []int, left int, right int) int {
    // 1. 在子数组 [left, right] 中随机选择一个基准元素 pivot
    i := left + rand.Intn(right-left+1)
    pivot := nums[i]
    // 把 pivot 与子数组第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
    nums[i], nums[left] = nums[left], nums[i]

    // 2. 相向双指针遍历子数组 [left + 1, right]
    // 循环不变量：在循环过程中，子数组的数据分布始终如下图
    // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
    //   ^                 ^     ^         ^
    //   left              i     j         right

    i, j := left+1, right
    for {
        for i <= j && nums[i] < pivot {
            i++
        }
        // 此时 nums[i] >= pivot

        for i <= j && nums[j] > pivot {
            j--
        }
        // 此时 nums[j] <= pivot

        if i >= j {
            break
        }

        // 维持循环不变量
        nums[i], nums[j] = nums[j], nums[i]
        i++
        j--
    }

    // 循环结束后
    // [ pivot | <=pivot | >=pivot ]
    //   ^             ^   ^     ^
    //   left          j   i     right

    // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
    // 为什么与 j 交换？
    // 如果与 i 交换，可能会出现 i = right + 1 的情况，已经下标越界了，无法交换
    // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
    // 与 j 交换，即使 j = left，交换也不会出错
    nums[left], nums[j] = nums[j], nums[left]

    // 交换后
    // [ <=pivot | pivot | >=pivot ]
    //               ^
    //               j

    // 返回 pivot 的下标
    return j
}

func findKthLargest(nums []int, k int) int {
    n := len(nums)
    targetIndex := n - k  // 第 k 大元素在升序数组中的下标是 n - k
    left, right := 0, n-1 // 闭区间
    for {
        i := partition(nums, left, right)
        if i == targetIndex {
            // 找到第 k 大元素
            return nums[i]
        }
        if i > targetIndex {
            // 第 k 大元素在 [left, i - 1] 中
            right = i - 1
        } else {
            // 第 k 大元素在 [i + 1, right] 中
            left = i + 1
        }
    }
}
```

```js [sol-JavaScript]
// 在子数组 [left, right] 中随机选择一个基准元素 pivot
// 根据 pivot 重新排列子数组 [left, right]
// 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
// 返回 pivot 在重新排列后的 nums 中的下标
// 特别地，如果子数组的所有元素都等于 pivot，我们会返回子数组的中心下标，避免退化
function partition(nums, left, right) {
    // 1. 在子数组 [left, right] 中随机选择一个基准元素 pivot
    const idx = left + Math.floor(Math.random() * (right - left + 1));
    const pivot = nums[idx];
    // 把 pivot 与子数组第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
    [nums[idx], nums[left]] = [nums[left], nums[idx]];

    // 2. 相向双指针遍历子数组 [left + 1, right]
    // 循环不变量：在循环过程中，子数组的数据分布始终如下图
    // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
    //   ^                 ^     ^         ^
    //   left              i     j         right

    let i = left + 1, j = right;
    while (true) {
        while (i <= j && nums[i] < pivot) {
            i++;
        }
        // 此时 nums[i] >= pivot

        while (i <= j && nums[j] > pivot) {
            j--;
        }
        // 此时 nums[j] <= pivot

        if (i >= j) {
            break;
        }

        // 维持循环不变量
        [nums[i], nums[j]] = [nums[j], nums[i]];
        i++;
        j--;
    }

    // 循环结束后
    // [ pivot | <=pivot | >=pivot ]
    //   ^             ^   ^     ^
    //   left          j   i     right

    // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
    // 为什么与 j 交换？
    // 如果与 i 交换，可能会出现 i = right + 1 的情况，已经下标越界了，无法交换
    // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
    // 与 j 交换，即使 j = left，交换也不会出错
    [nums[left], nums[j]] = [nums[j], nums[left]];

    // 返回 pivot 的下标
    return j;
}

var findKthLargest = function(nums, k) {
    const n = nums.length;
    const targetIndex = n - k; // 第 k 大元素在升序数组中的下标是 n - k
    let left = 0, right = n - 1; // 闭区间
    while (true) {
        const i = partition(nums, left, right);
        if (i === targetIndex) {
            // 找到第 k 大元素
            return nums[i];
        }
        if (i > targetIndex) {
            // 第 k 大元素在 [left, i - 1] 中
            right = i - 1;
        } else {
            // 第 k 大元素在 [i + 1, right] 中
            left = i + 1;
        }
    }
};
```

```rust [sol-Rust]
use rand::Rng;

impl Solution {
    // 在子数组 [left, right] 中随机选择一个基准元素 pivot
    // 根据 pivot 重新排列子数组 [left, right]
    // 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
    // 返回 pivot 在重新排列后的 nums 中的下标
    // 特别地，如果子数组的所有元素都等于 pivot，我们会返回子数组的中心下标，避免退化
    fn partition(nums: &mut [i32], left: usize, right: usize) -> usize {
        // 1. 在子数组 [left, right] 中随机选择一个基准元素 pivot
        let mut rng = rand::thread_rng();
        let i = left + rng.gen_range(0..=right - left);
        let pivot = nums[i];
        // 把 pivot 与子数组第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
        nums.swap(i, left);

        // 2. 相向双指针遍历子数组 [left + 1, right]
        // 循环不变量：在循环过程中，子数组的数据分布始终如下图
        // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
        //   ^                 ^     ^         ^
        //   left              i     j         right

        let mut i = left + 1;
        let mut j = right;
        loop {
            while i <= j && nums[i] < pivot {
                i += 1;
            }
            // 此时 nums[i] >= pivot

            while i <= j && nums[j] > pivot {
                j -= 1;
            }
            // 此时 nums[j] <= pivot

            if i >= j {
                break;
            }

            // 维持循环不变量
            nums.swap(i, j);
            i += 1;
            j -= 1;
        }

        // 循环结束后
        // [ pivot | <=pivot | >=pivot ]
        //   ^             ^   ^     ^
        //   left          j   i     right

        // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
        // 为什么与 j 交换？
        // 如果与 i 交换，可能会出现 i = right + 1 的情况，已经下标越界了，无法交换
        // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
        // 与 j 交换，即使 j = left，交换也不会出错
        nums.swap(left, j);

        // 交换后
        // [ <=pivot | pivot | >=pivot ]
        //               ^
        //               j

        // 返回 pivot 的下标
        j
    }

    pub fn find_kth_largest(mut nums: Vec<i32>, k: i32) -> i32 {
        let n = nums.len();
        let target_index = n - k as usize; // 第 k 大元素在升序数组中的下标是 n - k
        let mut left = 0;
        let mut right = n - 1; // 闭区间
        loop {
            let i = Self::partition(&mut nums, left, right);
            if i == target_index {
                // 找到第 k 大元素
                return nums[i];
            }
            if i > target_index {
                // 第 k 大元素在 [left, i - 1] 中
                right = i - 1;
            } else {
                // 第 k 大元素在 [i + 1, right] 中
                left = i + 1;
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：期望 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。在平均情况下，第一次划分（partition）需要处理 $n$ 个元素，第二次平均 $\dfrac{n}{2}$，第三次平均 $\dfrac{n}{4}$，依此类推。所以期望时间复杂度为 $\mathcal{O}\left(n + \dfrac{n}{2} + \dfrac{n}{4} +\cdots \right) = \mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 答疑

**问**：如果不随机选择基准元素 $\textit{pivot}$，会发生什么？

**答**：比如子数组是有序的，且我们每次都选子数组的第一个（或者最后一个）元素作为 $\textit{pivot}$，那么按照算法，$j$ 不变或者移动到最左边，划分是**最不均匀**的，算法会退化至 $\mathcal{O}(n^2)$。随机选 $\textit{pivot}$ 能使划分在期望意义上是均匀的（$j$ 移动到子数组的中间），保证算法的期望时间复杂度为 $\mathcal{O}(n)$。

**问**：代码中的 `nums[i] < pivot` 和 `nums[i] > pivot` 能否改成 `nums[i] <= pivot` 和 `nums[i] >= pivot`？

**答**：这个做法会在子数组所有元素相同时，划分后的 $j$ 是子数组最后一个元素的下标，是**最不均匀**划分，算法会退化至 $\mathcal{O}(n^2)$。

**问**：代码中的 `i <= j` 能否改成 `i < j`？

**答**：这会算错。来看一个例子 $\textit{nums} = [2,1,3]$，$\textit{pivot}=2$。左指针 $i=1$ 移动到 $i=2$，右指针 $j=2$ 因为不满足 `i < j` 的条件，无法移动。此时我们交换 $2$ 和 $\textit{nums}[j]= 3$，得到 $[3,1,2]$，返回 $j=2$。然而 $j=2$ 左侧有大于 $\textit{pivot}=2$ 的元素，划分失败。

如果写成 `i <= j`，那么最终 $i=2$，$j=1$。此时我们交换 $2$ 和 $\textit{nums}[j]=1$，得到 $[1,2,3]$，返回 $j=1$。这样的划分就是正确的。

## 附：库函数写法

```cpp [sol-C++]
class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        ranges::nth_element(nums, nums.end() - k);
        return nums[nums.size() - k];
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_kth_largest(mut nums: Vec<i32>, k: i32) -> i32 {
        let i = nums.len() - k as usize;
        *nums.select_nth_unstable(i).1
    }
}
```

## 关联题目

如果你理解了划分的过程，那么**快速排序算法**最难的内容也就理解了。读者可以趁热打铁，完成如下题目：

- [912. 排序数组](https://leetcode.cn/problems/sort-an-array/)

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

## 本地原创解析

### 1. 题意重述

本题来自 `五、其他递归/分治`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、其他递归/分治`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
