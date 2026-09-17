# 912. 排序数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sort-an-array/
- 题目 slug：`sort-an-array`
- 来源专题：链表、树与回溯
- 来源分类路径：五、其他递归/分治
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sort-an-array/solutions/3799866/onlogn-kuai-su-pai-xu-fen-zhi-xie-fa-you-njpq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(nlogn) 快速排序+有序优化，附常见问题解答（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sort-an-array/solutions/3799866/onlogn-kuai-su-pai-xu-fen-zhi-xie-fa-you-njpq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`onlogn-kuai-su-pai-xu-fen-zhi-xie-fa-you-njpq`
- topic id：`3799866`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

请先学习相对更简单的快速选择算法，重点掌握**划分**（partition）的原理。见 [215. 数组中的第K个最大元素](https://leetcode.cn/problems/kth-largest-element-in-an-array/)，[我的题解](https://leetcode.cn/problems/kth-largest-element-in-an-array/solutions/3799769/on-kuai-su-xuan-ze-suan-fa-pythonjavaccg-lh7c/)。

下面描述快速排序的**核心思想**：

1. 定义一个递归函数 $\text{sort}(\textit{left},\textit{right})$，表示把子数组 $[\textit{left},\textit{right}]$ 升序排序。
2. 划分子数组 $[\textit{left},\textit{right}]$，划分方法同 215 题。设划分后，基准元素 $\textit{pivot}$ 的下标为 $i$。
    - 在 $\textit{pivot}$ 左侧的元素，继续递归排序，即递归调用 $\text{sort}(\textit{left},i-1)$。
    - 在 $\textit{pivot}$ 右侧的元素，继续递归排序，即递归调用 $\text{sort}(i+1,\textit{right})$。
3. 递归边界：如果 $\textit{left}\ge \textit{right}$，说明子数组为空或者只剩一个元素，此时无需排序，直接返回。
4. 递归入口：$\text{sort}(0,n-1)$。

**一个简单有效的优化**：如果子数组已是升序，直接返回。

## 答疑

**问**：如果不随机选择基准元素 $\textit{pivot}$，会发生什么？

**答**：比如子数组是降序，且我们每次都选子数组的第一个（或者最后一个）元素作为 $\textit{pivot}$，那么按照算法，$j$ 不变或者移动到最左边，划分是**最不均匀**的，算法会退化至 $\mathcal{O}(n^2)$。随机选 $\textit{pivot}$ 能使划分在期望意义上是均匀的（$j$ 移动到子数组的中间），保证算法的期望时间复杂度为 $\mathcal{O}(n\log n)$。

**问**：代码中的 `nums[i] < pivot` 和 `nums[i] > pivot` 能否改成 `nums[i] <= pivot` 和 `nums[i] >= pivot`？

**答**：这个做法会在子数组所有元素相同时，划分后的 $j$ 是子数组最后一个元素的下标，是**最不均匀**划分，算法会退化至 $\mathcal{O}(n^2)$。

**问**：代码中的 `i <= j` 能否改成 `i < j`？

**答**：这会算错。来看一个例子 $\textit{nums} = [2,1,3]$，$\textit{pivot}=2$。左指针 $i=1$ 移动到 $i=2$，右指针 $j=2$ 因为不满足 `i < j` 的条件，无法移动。此时我们交换 $2$ 和 $\textit{nums}[j]= 3$，得到 $[3,1,2]$，返回 $j=2$。然而 $j=2$ 左侧有大于 $\textit{pivot}=2$ 的元素，划分失败。

如果写成 `i <= j`，那么最终 $i=2$，$j=1$。此时我们交换 $2$ 和 $\textit{nums}[j]=1$，得到 $[1,2,3]$，返回 $j=1$。这样的划分就是正确的。

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

    def sortArray(self, nums: List[int]) -> List[int]:
        # 快速排序子数组 [left, right]
        def quick_sort(left: int, right: int) -> None:
            # 优化：如果子数组已是升序，直接返回
            # 也可以写 if all(nums[i] <= nums[i + 1] for i in range(left, right)): return
            ordered = True
            for i in range(left, right):
                if nums[i] > nums[i + 1]:
                    ordered = False
                    break
            if ordered:
                return

            i = self.partition(nums, left, right)  # 划分子数组
            quick_sort(left, i - 1)   # 排序在 pivot 左侧的元素
            quick_sort(i + 1, right)  # 排序在 pivot 右侧的元素

        quick_sort(0, len(nums) - 1)
        return nums
```

```java [sol-Java]
class Solution {
    private static final Random rand = new Random();

    public int[] sortArray(int[] nums) {
        quickSort(nums, 0, nums.length - 1);
        return nums;
    }

    // 快速排序子数组 [left, right]
    private void quickSort(int[] nums, int left, int right) {
        // 优化：如果子数组已是升序，直接返回
        boolean ordered = true;
        for (int i = left; i < right; i++) {
            if (nums[i] > nums[i + 1]) {
                ordered = false;
                break;
            }
        }
        if (ordered) {
            return;
        }

        int i = partition(nums, left, right); // 划分子数组
        quickSort(nums, left, i - 1);  // 排序在 pivot 左侧的元素
        quickSort(nums, i + 1, right); // 排序在 pivot 右侧的元素
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

```java [sol-Java 写法二]
class Solution {
    private static final int INSERTION_THRESHOLD = 24;
    private static final Random rand = new Random();

    public int[] sortArray(int[] nums) {
        quickSort(nums, 0, nums.length - 1);
        return nums;
    }

    // 快速排序子数组 [left, right]
    private void quickSort(int[] nums, int left, int right) {
        // 优化：如果子数组已是升序，直接返回
        boolean ordered = true;
        for (int i = left; i < right; i++) {
            if (nums[i] > nums[i + 1]) {
                ordered = false;
                break;
            }
        }
        if (ordered) {
            return;
        }

        // 当子数组长度不超过 INSERTION_THRESHOLD 时，切换为插入排序
        if (right - left + 1 <= INSERTION_THRESHOLD) {
            insertionSort(nums, left, right);
            return;
        }

        int i = partition(nums, left, right); // 划分子数组
        quickSort(nums, left, i - 1);  // 排序在 pivot 左侧的元素
        quickSort(nums, i + 1, right); // 排序在 pivot 右侧的元素
    }

    // 插入排序子数组 [left, right]
    private void insertionSort(int[] nums, int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            // 把 x = nums[i] 插到已排序区间 [left, i - 1] 中
            int x = nums[i];
            int j = i - 1;
            while (j >= left && nums[j] > x) {
                nums[j + 1] = nums[j];
                j--;
            }
            // 循环结束后，原本 > x 的数全部右移一位，nums[j + 1] 空出来，填入 x
            nums[j + 1] = x;
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

    // 快速排序子数组 [left, right]
    void quick_sort(vector<int>& nums, int left, int right) {
        // 优化：如果子数组已是升序，直接返回
        // STL 写法：is_sorted(nums.begin() + left, nums.begin() + right + 1)
        bool ordered = true;
        for (int i = left; i < right; i++) {
            if (nums[i] > nums[i + 1]) {
                ordered = false;
                break;
            }
        }
        if (ordered) {
            return;
        }

        int i = partition(nums, left, right); // 划分子数组
        quick_sort(nums, left, i - 1);  // 排序在 pivot 左侧的元素
        quick_sort(nums, i + 1, right); // 排序在 pivot 右侧的元素
   }

public:
    vector<int> sortArray(vector<int>& nums) {
        srand(time(NULL));
        quick_sort(nums, 0, (int) nums.size() - 1);
        return nums;
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

// 快速排序子数组 [left, right]
void quickSort(int* nums, int left, int right) {
    // 优化：如果子数组已是升序，直接返回
    bool ordered = true;
    for (int i = left; i < right; i++) {
        if (nums[i] > nums[i + 1]) {
            ordered = false;
            break;
        }
    }
    if (ordered) {
        return;
    }

    int i = partition(nums, left, right); // 划分子数组
    quickSort(nums, left, i - 1);  // 排序在 pivot 左侧的元素
    quickSort(nums, i + 1, right); // 排序在 pivot 右侧的元素
}

int* sortArray(int* nums, int numsSize, int* returnSize) {
    srand(time(NULL));
    quickSort(nums, 0, numsSize - 1);
    *returnSize = numsSize;
    return nums;
}
```

```go [sol-Go]
// 在 nums 中随机选择一个基准元素 pivot
// 根据 pivot 重新排列 nums
// 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
// 返回 pivot 在重新排列后的 nums 中的下标
// 特别地，如果数组的所有元素都等于 pivot，我们会返回数组的中心下标，避免退化
func partition(nums []int) int {
    // 1. 在 nums 中随机选择一个基准元素 pivot
    n := len(nums)
    i := rand.Intn(n)
    pivot := nums[i]
    // 把 pivot 与第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
    nums[i], nums[0] = nums[0], nums[i]

    // 2. 相向双指针遍历 [1, n-1]
    // 循环不变量：在循环过程中，数组的数据分布始终如下图
    // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
    //   ^                 ^     ^         ^
    //   0                 i     j         n-1

    i, j := 1, n-1
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
    //   0             j   i     n-1

    // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
    // 为什么与 j 交换？
    // 如果与 i 交换，可能会出现 i = n 的情况，已经下标越界了，无法交换
    // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
    // 与 j 交换，即使 j = 0，交换也不会出错
    nums[0], nums[j] = nums[j], nums[0]

    // 交换后
    // [ <=pivot | pivot | >=pivot ]
    //               ^
    //               j

    // 返回 pivot 的下标
    return j
}

// 快速排序 nums
func sortArray(nums []int) []int {
    // 优化：如果 nums 已是升序，直接返回
    if slices.IsSorted(nums) {
        return nums
    }

    i := partition(nums)  // 划分 nums
    sortArray(nums[:i])   // 排序在 pivot 左侧的元素
    sortArray(nums[i+1:]) // 排序在 pivot 右侧的元素
    return nums
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

// 快速排序子数组 [left, right]
function quickSort(nums, left, right) {
    // 优化：如果子数组已是升序，直接返回
    let ordered = true;
    for (let i = left; i < right; i++) {
        if (nums[i] > nums[i + 1]) {
            ordered = false;
            break;
        }
    }
    if (ordered) {
        return;
    }

    const i = partition(nums, left, right); // 划分子数组
    quickSort(nums, left, i - 1);  // 排序在 pivot 左侧的元素
    quickSort(nums, i + 1, right); // 排序在 pivot 右侧的元素
}

var sortArray = function(nums) {
    quickSort(nums, 0, nums.length - 1);
    return nums;
};
```

```rust [sol-Rust]
use rand::Rng;

impl Solution {
    // 在 nums 中随机选择一个基准元素 pivot
    // 根据 pivot 重新排列 nums
    // 重新排列后，<= pivot 的元素都在 pivot 的左侧，>= pivot 的元素都在 pivot 的右侧
    // 返回 pivot 在重新排列后的 nums 中的下标
    // 特别地，如果数组的所有元素都等于 pivot，我们会返回数组的中心下标，避免退化
    fn partition(nums: &mut [i32]) -> usize {
        // 1. 在 nums 中随机选择一个基准元素 pivot
        let n = nums.len();
        let mut rng = rand::thread_rng();
        let i = rng.gen_range(0..n);
        let pivot = nums[i];
        // 把 pivot 与第一个元素交换，避免 pivot 干扰后续划分，从而简化实现逻辑
        nums.swap(i, 0);

        // 2. 相向双指针遍历 [1, n-1]
        // 循环不变量：在循环过程中，数组的数据分布始终如下图
        // [ pivot | <=pivot | 尚未遍历 | >=pivot ]
        //   ^                 ^     ^         ^
        //   0                 i     j         n-1

        let mut i = 1;
        let mut j = n - 1;
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
        //   0             j   i     n-1

        // 3. 把 pivot 与 nums[j] 交换，完成划分（partition）
        // 为什么与 j 交换？
        // 如果与 i 交换，可能会出现 i = n 的情况，已经下标越界了，无法交换
        // 另一个原因是如果 nums[i] > pivot，交换会导致一个大于 pivot 的数出现在子数组最左边，不是有效划分
        // 与 j 交换，即使 j = 0，交换也不会出错
        nums.swap(0, j);

        // 交换后
        // [ <=pivot | pivot | >=pivot ]
        //               ^
        //               j

        // 返回 pivot 的下标
        j
    }

    // 快速排序 nums
    fn quick_sort(nums: &mut [i32]) {
        // 优化：如果 nums 已是升序，直接返回
        if nums.windows(2).all(|w| w[0] <= w[1]) {
            return;
        }

        let i = Self::partition(nums);
        Self::quick_sort(&mut nums[..i]); // 排序在 pivot 左侧的元素
        Self::quick_sort(&mut nums[i + 1..]); // 排序在 pivot 右侧的元素
    }

    pub fn sort_array(mut nums: Vec<i32>) -> Vec<i32> {
        Self::quick_sort(&mut nums);
        nums
    }
}
```

#### 复杂度分析

- 期望时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。在平均情况下，可以视作均匀划分（partition）。递归深度为 $\mathcal{O}(\log n)$，每一层会涉及 $\mathcal{O}(n)$ 个元素的比较和交换，所以时间复杂度等于一个高为 $\mathcal{O}(\log n)$，底边长为 $\mathcal{O}(n)$ 的矩形的面积，即 $\mathcal{O}(n\log n)$。
- 期望空间复杂度：$\mathcal{O}(\log n)$。递归需要期望 $\mathcal{O}(\log n)$ 的栈开销。

**额外优化 1**：当子数组长度小于某个阈值时，切换为**插入排序**。参考 Java 写法二。

**额外优化 2**：当递归深度超过某个阈值时，切换为稳定的堆排序，避免退化到 $\mathcal{O}(n^2)$。留给感兴趣的读者实现。

## 总结

|   | 快速选择  |  快速排序  |
|---|---|---|
| 题目 | [215. 数组中的第K个最大元素](https://leetcode.cn/problems/kth-largest-element-in-an-array/) | [912. 排序数组](https://leetcode.cn/problems/sort-an-array/) |
| 应用场景  | 寻找数组第 $k$ 小 | 排序整个数组 |
| 算法分类 | 减治法 | 分治法 |
| 核心思想 | 每次划分只保留包含目标元素的子数组 | 每次把数组划分为左右两部分并递归排序 |
| 核心区别 | 只关注目标元素所在子数组，忽略另一个子数组 | 左右子数组都要排序 |
| 时间复杂度  | 期望 $\mathcal{O}(n)$ | 期望 $\mathcal{O}(n\log n)$  |
| 空间复杂度  | $\mathcal{O}(1)$ | 期望 $\mathcal{O}(\log n)$ |

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
