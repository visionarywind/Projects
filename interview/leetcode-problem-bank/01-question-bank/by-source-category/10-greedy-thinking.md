# 贪心与思维

<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->

来源：[分享丨【算法题单】贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
生成时间：2026-09-16 10:17:56 +0800

本页按来源题单中的标题层级组织，只保存题目元数据和链接，不复制题面或题解。

## 一、贪心策略 / §1.1 从最小/最大开始贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2604 | 3074 | [3074. 重新分装苹果](https://leetcode.cn/problems/apple-redistribution-into-boxes/) | 1198 | external-index |
| 2605 | 3545 | [3545. 不同字符数量最多为 K 时的最少删除数](https://leetcode.cn/problems/minimum-deletions-for-at-most-k-distinct-characters/) | 1211 | external-index |
| 2606 | 3745 | [3745. 三元素表达式的最大值](https://leetcode.cn/problems/maximize-expression-of-three-elements/) | 1218 | external-index |
| 2607 | 2279 | [2279. 装满石头的背包的最大数量](https://leetcode.cn/problems/maximum-bags-with-full-capacity-of-rocks/) | 1249 | external-index |
| 2608 | 1833 | [1833. 雪糕的最大数量](https://leetcode.cn/problems/maximum-ice-cream-bars/) | 1253 | external-index |
| 2609 | 2144 | [2144. 打折购买糖果的最小开销](https://leetcode.cn/problems/minimum-cost-of-buying-candies-with-discount/) | 1261 | external-index |
| 2610 | 1005 | [1005. K 次取反后最大化的数组和](https://leetcode.cn/problems/maximize-sum-of-array-after-k-negations/) | 1275 | external-index |
| 2611 | 1481 | [1481. 不同整数的最少数目](https://leetcode.cn/problems/least-number-of-unique-integers-after-k-removals/) | 1284 | external-index |
| 2612 | 1403 | [1403. 非递增顺序的最小子序列](https://leetcode.cn/problems/minimum-subsequence-in-non-increasing-order/) | 1288 | external-index |
| 2613 | 3010 | [3010. 将数组分成最小总代价的子数组 I](https://leetcode.cn/problems/divide-an-array-into-subarrays-with-minimum-cost-i/) | 1292 | external-index |
| 2614 | 1338 | [1338. 数组大小减半](https://leetcode.cn/problems/reduce-array-size-to-the-half/) | 1303 | external-index |
| 2615 | 1710 | [1710. 卡车上的最大单元数](https://leetcode.cn/problems/maximum-units-on-a-truck/) | 1310 | external-index |
| 2616 | 3075 | [3075. 幸福值最大化的选择方案](https://leetcode.cn/problems/maximize-happiness-of-selected-children/) | 1326 | external-index |
| 2617 | 2554 | [2554. 从一个范围内选择最多整数 I](https://leetcode.cn/problems/maximum-number-of-integers-to-choose-from-a-range-i/) | 1333 | external-index |
| 2618 | 2126 | [2126. 摧毁小行星](https://leetcode.cn/problems/destroying-asteroids/) | 1335 | external-index |
| 2619 | 2587 | [2587. 重排数组以得到最大前缀分数](https://leetcode.cn/problems/rearrange-array-to-maximize-prefix-score/) | 1337 | external-index |
| 2620 | 976 | [976. 三角形的最大周长](https://leetcode.cn/problems/largest-perimeter-triangle/) | 1341 | external-index |
| 2621 | 1561 | [1561. 你可以获得的最大硬币数目](https://leetcode.cn/problems/maximum-number-of-coins-you-can-get/) | 1406 | external-index |
| 2622 | 3627 | [3627. 中位数之和的最大值](https://leetcode.cn/problems/maximum-median-sum-of-subsequences-of-size-3/) | 1561 | external-index |
| 2623 | 3462 | [3462. 提取至多 K 个元素的最大总和](https://leetcode.cn/problems/maximum-sum-with-at-most-k-elements/) | 1416 | external-index |
| 2624 | 2099 | [2099. 找到和最大的长度为 K 的子序列](https://leetcode.cn/problems/find-subsequence-of-length-k-with-the-largest-sum/) | 1447 | external-index |
| 2625 | 3301 | [3301. 高度互不相同的最大塔高和](https://leetcode.cn/problems/maximize-the-total-height-of-unique-towers/) | 1448 | external-index |
| 2626 | 945 | [945. 使数组唯一的最小增量](https://leetcode.cn/problems/minimum-increment-to-make-array-unique/) | 1448 | external-index |
| 2627 | 1846 | [1846. 减小和重新排列数组后的最大元素](https://leetcode.cn/problems/maximum-element-after-decreasing-and-rearranging/) | 1454 | external-index |
| 2628 | 3727 | [3727. 最大交替平方和](https://leetcode.cn/problems/maximum-alternating-sum-of-squares/) | 1455 | external-index |
| 2629 | 1647 | [1647. 字符频次唯一的最小删除次数](https://leetcode.cn/problems/minimum-deletions-to-make-character-frequencies-unique/) | 1510 | external-index |
| 2630 | 2971 | [2971. 找到最大周长的多边形](https://leetcode.cn/problems/find-polygon-with-the-largest-perimeter/) | 1521 | external-index |
| 2631 | 2178 | [2178. 拆分成最多数目的正偶数之和](https://leetcode.cn/problems/maximum-split-of-positive-even-integers/) | 1538 | external-index |
| 2632 | 954 | [954. 二倍数对数组](https://leetcode.cn/problems/array-of-doubled-pairs/) | 1548 | external-index |
| 2633 | 1578 | [1578. 使绳子变成彩色的最短时间](https://leetcode.cn/problems/minimum-time-to-make-rope-colorful/) | 1574 | external-index |
| 2634 | 2567 | [2567. 修改两个元素的最小分数](https://leetcode.cn/problems/minimum-score-by-changing-two-elements/) | 1609 | external-index |
| 2635 | 1509 | [1509. 三次操作后最大值与最小值的最小差](https://leetcode.cn/problems/minimum-difference-between-largest-and-smallest-value-in-three-moves/) | 1653 | external-index |
| 2636 | 2611 | [2611. 老鼠和奶酪](https://leetcode.cn/problems/mice-and-cheese/) | 1663 | external-index |
| 2637 | 3397 | [3397. 执行操作后不同元素的最大数量](https://leetcode.cn/problems/maximum-number-of-distinct-elements-after-operations/) | 1687 | external-index |
| 2638 | 3457 | [3457. 吃披萨](https://leetcode.cn/problems/eat-pizzas/) | 1704 | external-index |
| 2639 | 3767 | [3767. 选择 K 个任务的最大总分数](https://leetcode.cn/problems/maximize-points-after-choosing-k-tasks/) | 1704 | external-index |
| 2640 | LCP 40 | [LCP 40. 心算挑战](https://leetcode.cn/problems/uOAnQW/) | Unknown | external-index |
| 2641 | 1262 | [1262. 可被三整除的最大和](https://leetcode.cn/problems/greatest-sum-divisible-by-three/) | 1762 | external-index |
| 2642 | 948 | [948. 令牌放置](https://leetcode.cn/problems/bag-of-tokens/) | 1762 | external-index |
| 2643 | 1775 | [1775. 通过最少操作次数使数组的和相等](https://leetcode.cn/problems/equal-sum-arrays-with-minimum-number-of-operations/) | 1850 | external-index |
| 2644 | 2333 | [2333. 最小差值平方和](https://leetcode.cn/problems/minimum-sum-of-squared-difference/) | 2011 | external-index |
| 2645 | 3645 | [3645. 最优激活顺序得到的最大总和](https://leetcode.cn/problems/maximum-total-from-optimal-activation-order/) | 2019 | external-index |
| 2646 | 3947 | [3947. 购买最多物品数目 II](https://leetcode.cn/problems/maximum-number-of-items-from-sale-ii/) | 2215 | external-index |
| 2647 | 2141 | [2141. 同时运行 N 台电脑的最长时间](https://leetcode.cn/problems/maximum-running-time-of-n-computers/) | 2265 | external-index |
| 2648 | 1196 | [1196. 最多可以买到的苹果数量](https://leetcode.cn/problems/how-many-apples-can-you-put-into-the-basket/) | Unknown | external-index |
| 2649 | 2214 | [2214. 通关游戏所需的最低生命值](https://leetcode.cn/problems/minimum-health-to-beat-game/) | Unknown | external-index |
| 2650 | 3763 | [3763. 带阈值约束的最大总和](https://leetcode.cn/problems/maximum-total-sum-with-threshold-constraints/) | Unknown | external-index |
| 2651 | 3730 | [3730. 跳跃燃烧的最大卡路里](https://leetcode.cn/problems/maximum-calories-burnt-from-jumps/) | Unknown | external-index |
| 2652 | 3476 | [3476. 最大化任务分配的利润](https://leetcode.cn/problems/maximize-profit-from-task-assignment/) | Unknown | external-index |
| 2653 | 2098 | [2098. 长度为 K 的最大偶数和子序列](https://leetcode.cn/problems/subsequence-of-size-k-with-the-largest-even-sum/) | Unknown | external-index |
| 2654 | 2548 | [2548. 填满背包的最大价格](https://leetcode.cn/problems/maximum-price-to-fill-a-bag/) | Unknown | external-index |
| 2655 | 3119 | [3119. 最大数量的可修复坑洼](https://leetcode.cn/problems/maximum-number-of-potholes-that-can-be-fixed/) | Unknown | external-index |
| 2656 | 2557 | [2557. 从一个范围内选择最多整数 II](https://leetcode.cn/problems/maximum-number-of-integers-to-choose-from-a-range-ii/) | Unknown | external-index |
| 2657 | 1984 | [1984. 学生分数的最小差值](https://leetcode.cn/problems/minimum-difference-between-highest-and-lowest-of-k-scores/) | 1306 | external-index |
| 2658 | 624 | [624. 数组列表中的最大距离](https://leetcode.cn/problems/maximum-distance-in-arrays/) | Unknown | external-index |
| 2659 | 910 | [910. 最小差值 II](https://leetcode.cn/problems/smallest-range-ii/) | 2135 | external-index |
| 2660 | 2835 | [2835. 使子序列的和等于目标的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-form-subsequence-with-target-sum/) | 2207 | external-index |
| 2661 | 3366 | [3366. 最小数组和](https://leetcode.cn/problems/minimum-array-sum/) | 2900 | external-index |

## 一、贪心策略 / §1.2 单序列配对

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2662 | 561 | [561. 数组拆分](https://leetcode.cn/problems/array-partition/) | 1300 | external-index |
| 2663 | 1877 | [1877. 数组中最大数对和的最小值](https://leetcode.cn/problems/minimize-maximum-pair-sum-in-array/) | 1301 | external-index |
| 2664 | 881 | [881. 救生艇](https://leetcode.cn/problems/boats-to-save-people/) | 1530 | external-index |
| 2665 | 2592 | [2592. 最大化数组的伟大值](https://leetcode.cn/problems/maximize-greatness-of-an-array/) | 1569 | external-index |
| 2666 | 2576 | [2576. 求出最多标记下标](https://leetcode.cn/problems/find-the-maximum-number-of-marked-indices/) | 1843 | external-index |

## 一、贪心策略 / §1.3 双序列配对

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2667 | 2037 | [2037. 使每位学生都有座位的最少移动次数](https://leetcode.cn/problems/minimum-number-of-moves-to-seat-everyone/) | 1357 | external-index |
| 2668 | 455 | [455. 分发饼干](https://leetcode.cn/problems/assign-cookies/) | 1381 | external-index |
| 2669 | 2410 | [2410. 运动员和训练师的最大匹配数](https://leetcode.cn/problems/maximum-matching-of-players-with-trainers/) | 1381 | external-index |
| 2670 | 1433 | [1433. 检查一个字符串是否可以打破另一个字符串](https://leetcode.cn/problems/check-if-a-string-can-break-another-string/) | 1436 | external-index |
| 2671 | 870 | [870. 优势洗牌](https://leetcode.cn/problems/advantage-shuffle/) | 1648 | external-index |
| 2672 | 826 | [826. 安排工作以达到最大收益](https://leetcode.cn/problems/most-profit-assigning-work/) | 1709 | external-index |
| 2673 | 2449 | [2449. 使数组相似的最少操作次数](https://leetcode.cn/problems/minimum-number-of-operations-to-make-arrays-similar/) | 2076 | external-index |
| 2674 | 1889 | [1889. 装包裹的最小浪费空间](https://leetcode.cn/problems/minimum-space-wasted-from-packaging/) | 2214 | external-index |
| 2675 | 2561 | [2561. 重排水果](https://leetcode.cn/problems/rearranging-fruits/) | 2222 | external-index |
| 2676 | 2071 | [2071. 你可以安排的最多任务数目](https://leetcode.cn/problems/maximum-number-of-tasks-you-can-assign/) | 2648 | external-index |
| 2677 | 2323 | [2323. 完成所有工作的最短时间 II](https://leetcode.cn/problems/find-minimum-time-to-finish-all-jobs-ii/) | Unknown | external-index |

## 一、贪心策略 / §1.4 从最左/最右开始贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2678 | 3402 | [3402. 使每一列严格递增的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-make-columns-strictly-increasing/) | 1246 | external-index |
| 2679 | 3191 | [3191. 使二进制数组全部等于 1 的最少操作次数 I](https://leetcode.cn/problems/minimum-operations-to-make-binary-array-elements-equal-to-one-i/) | 1312 | external-index |
| 2680 | 1827 | [1827. 最少操作使数组递增](https://leetcode.cn/problems/minimum-operations-to-make-the-array-increasing/) | 1315 | external-index |
| 2681 | 2027 | [2027. 转换字符串的最少操作次数](https://leetcode.cn/problems/minimum-moves-to-convert-string/) | 1346 | external-index |
| 2682 | 605 | [605. 种花问题](https://leetcode.cn/problems/can-place-flowers/) | 1400 | external-index |
| 2683 | 3111 | [3111. 覆盖所有点的最少矩形数目](https://leetcode.cn/problems/minimum-rectangles-to-cover-points/) | 1401 | external-index |
| 2684 | 2957 | [2957. 消除相邻近似相等字符](https://leetcode.cn/problems/remove-adjacent-almost-equal-characters/) | 1430 | external-index |
| 2685 | 3192 | [3192. 使二进制数组全部等于 1 的最少操作次数 II](https://leetcode.cn/problems/minimum-operations-to-make-binary-array-elements-equal-to-one-ii/) | 1433 | external-index |
| 2686 | 1529 | [1529. 最少的后缀翻转次数](https://leetcode.cn/problems/minimum-suffix-flips/) | 3192 | external-index |
| 2687 | 2789 | [2789. 合并后数组中的最大元素](https://leetcode.cn/problems/largest-element-in-an-array-after-merge-operations/) | 1485 | external-index |
| 2688 | 3576 | [3576. 数组元素相等转换](https://leetcode.cn/problems/transform-array-to-all-equal-elements/) | 1489 | external-index |
| 2689 | 3849 | [3849. 重新排列后的最大按位异或值](https://leetcode.cn/problems/maximum-bitwise-xor-after-rearrangement/) | 1556 | external-index |
| 2690 | 1144 | [1144. 递减元素使数组呈锯齿状](https://leetcode.cn/problems/decrease-elements-to-make-array-zigzag/) | 1559 | external-index |
| 2691 | 3228 | [3228. 将 1 移动到末尾的最大操作次数](https://leetcode.cn/problems/maximum-number-of-operations-to-move-ones-to-the-end/) | 1593 | external-index |
| 2692 | 2086 | [2086. 喂食仓鼠的最小食物桶数](https://leetcode.cn/problems/minimum-number-of-food-buckets-to-feed-the-hamsters/) | 1623 | external-index |
| 2693 | 2571 | [2571. 将整数减少到零需要的最少操作数](https://leetcode.cn/problems/minimum-operations-to-reduce-an-integer-to-0/) | 1649 | external-index |
| 2694 | 3776 | [3776. 使循环数组余额非负的最少移动次数](https://leetcode.cn/problems/minimum-moves-to-balance-circular-array/) | 1740 | external-index |
| 2695 | 861 | [861. 翻转矩阵后的得分](https://leetcode.cn/problems/score-after-flipping-matrix/) | 1818 | external-index |
| 2696 | 3980 | [3980. 变换二进制字符串的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-transform-binary-string/) | 1846 | external-index |
| 2697 | 3326 | [3326. 使数组非递减的最少除法操作次数](https://leetcode.cn/problems/minimum-division-operations-to-make-array-non-decreasing/) | 1864 | external-index |
| 2698 | 955 | [955. 删列造序 II](https://leetcode.cn/problems/delete-columns-to-make-sorted-ii/) | 1876 | external-index |
| 2699 | 1536 | [1536. 排布二进制网格的最少交换次数](https://leetcode.cn/problems/minimum-swaps-to-arrange-a-binary-grid/) | 1881 | external-index |
| 2700 | 2673 | [2673. 使二叉树所有路径值相等的最小代价](https://leetcode.cn/problems/make-costs-of-paths-equal-in-a-binary-tree/) | 1917 | external-index |
| 2701 | 1488 | [1488. 避免洪水泛滥](https://leetcode.cn/problems/avoid-flood-in-the-city/) | 1974 | external-index |
| 2702 | 2366 | [2366. 将数组排序的最少替换次数](https://leetcode.cn/problems/minimum-replacements-to-sort-the-array/) | 2060 | external-index |
| 2703 | 2528 | [2528. 最大化城市的最小电量](https://leetcode.cn/problems/maximize-the-minimum-powered-city/) | 2236 | external-index |
| 2704 | 3449 | [3449. 最大化游戏分数的最小值](https://leetcode.cn/problems/maximize-the-minimum-game-score/) | 2748 | external-index |
| 2705 | 2422 | [2422. 使用合并操作将数组转换为回文序列](https://leetcode.cn/problems/merge-operations-to-turn-array-into-a-palindrome/) | Unknown | external-index |

## 一、贪心策略 / §1.5 划分型贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2706 | 1221 | [1221. 分割平衡字符串](https://leetcode.cn/problems/split-a-string-in-balanced-strings/) | 1220 | external-index |
| 2707 | 2405 | [2405. 子字符串的最优划分](https://leetcode.cn/problems/optimal-partition-of-string/) | 1355 | external-index |
| 2708 | 2294 | [2294. 划分数组使最大差为 K](https://leetcode.cn/problems/partition-array-such-that-maximum-difference-is-k/) | 1416 | external-index |
| 2709 | 2358 | [2358. 分组的最大数量](https://leetcode.cn/problems/maximum-number-of-groups-entering-a-competition/) | 1503 | external-index |
| 2710 | 2522 | [2522. 将字符串分割成值不超过 K 的子字符串](https://leetcode.cn/problems/partition-string-into-substrings-with-values-at-most-k/) | 1605 | external-index |
| 2711 | 3557 | [3557. 不相交子字符串的最大数量](https://leetcode.cn/problems/find-maximum-number-of-non-intersecting-substrings/) | 1720 | external-index |
| 2712 | 1546 | [1546. 和为目标值且不重叠的非空子数组的最大数目](https://leetcode.cn/problems/maximum-number-of-non-overlapping-subarrays-with-sum-equals-target/) | 1855 | external-index |
| 2713 | 2436 | [2436. 使子数组最大公约数大于一的最小分割数](https://leetcode.cn/problems/minimum-split-into-subarrays-with-gcd-greater-than-one/) | Unknown | external-index |
| 2714 | 2892 | [2892. 将相邻元素相乘后得到最小化数组](https://leetcode.cn/problems/minimizing-array-after-replacing-pairs-with-their-product/) | Unknown | external-index |

## 一、贪心策略 / §1.6 先枚举，再贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2715 | 2171 | [2171. 拿出最少数目的魔法豆](https://leetcode.cn/problems/removing-minimum-number-of-magic-beans/) | 1748 | external-index |
| 2716 | 3085 | [3085. 成为 K 特殊字符串需要删除的最少字符数](https://leetcode.cn/problems/minimum-deletions-to-make-string-k-special/) | 1765 | external-index |
| 2717 | 1727 | [1727. 重新排列后的最大子矩阵](https://leetcode.cn/problems/largest-submatrix-with-rearrangements/) | 1927 | external-index |
| 2718 | 2749 | [2749. 得到整数零需要执行的最少操作数](https://leetcode.cn/problems/minimum-operations-to-make-the-integer-zero/) | 2132 | external-index |
| 2719 | 2910 | [2910. 合法分组的最少组数](https://leetcode.cn/problems/minimum-number-of-groups-to-create-a-valid-assignment/) | 2132 | external-index |
| 2720 | 2234 | [2234. 花园的最大总美丽值](https://leetcode.cn/problems/maximum-total-beauty-of-the-gardens/) | 2562 | external-index |

## 一、贪心策略 / §1.7 交换论证法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2721 | 1877 | [1877. 数组中最大数对和的最小值](https://leetcode.cn/problems/minimize-maximum-pair-sum-in-array/) | 1301 | external-index |
| 2722 | 2895 | [2895. 最小处理时间](https://leetcode.cn/problems/minimum-processing-time/) | 1352 | external-index |
| 2723 | 1846 | [1846. 减小和重新排列数组后的最大元素](https://leetcode.cn/problems/maximum-element-after-decreasing-and-rearranging/) | 1454 | external-index |
| 2724 | 3457 | [3457. 吃披萨](https://leetcode.cn/problems/eat-pizzas/) | 1704 | external-index |
| 2725 | 1665 | [1665. 完成所有任务的最少初始能量](https://leetcode.cn/problems/minimum-initial-energy-to-finish-tasks/) | 1901 | external-index |
| 2726 | 3273 | [3273. 对 Bob 造成的最少伤害](https://leetcode.cn/problems/minimum-amount-of-damage-dealt-to-bob/) | 2013 | external-index |
| 2727 | 2136 | [2136. 全部开花的最早一天](https://leetcode.cn/problems/earliest-possible-day-of-full-bloom/) | 2033 | external-index |
| 2728 | 2561 | [2561. 重排水果](https://leetcode.cn/problems/rearranging-fruits/) | 2222 | external-index |
| 2729 | 179 | [179. 最大数](https://leetcode.cn/problems/largest-number/) | Unknown | external-index |
| 2730 | 3309 | [3309. 连接二进制表示可形成的最大数值](https://leetcode.cn/problems/maximum-possible-number-by-binary-concatenation/) | Unknown | external-index |
| 2731 | 3897 | [3897. 连接二进制片段得到的最大值](https://leetcode.cn/problems/maximum-value-of-concatenated-binary-segments/) | 1998 | external-index |
| 2732 | 2412 | [2412. 完成所有交易的初始最少钱数](https://leetcode.cn/problems/minimum-money-required-before-transactions/) | 2092 | external-index |

## 一、贪心策略 / §1.8 相邻不同

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2733 | 2335 | [2335. 装满杯子需要的最短总时长](https://leetcode.cn/problems/minimum-amount-of-time-to-fill-cups/) | 1360 | external-index |
| 2734 | 3659 | [3659. 数组元素分组](https://leetcode.cn/problems/partition-array-into-k-distinct-groups/) | 1440 | external-index |
| 2735 | 1753 | [1753. 移除石子的最大得分](https://leetcode.cn/problems/maximum-score-from-removing-stones/) | 1488 | external-index |
| 2736 | 767 | [767. 重构字符串](https://leetcode.cn/problems/reorganize-string/) | 1681 | external-index |
| 2737 | 1054 | [1054. 距离相等的条形码](https://leetcode.cn/problems/distant-barcodes/) | 1702 | external-index |
| 2738 | 2856 | [2856. 删除数对后的最小数组长度](https://leetcode.cn/problems/minimum-array-length-after-pair-removals/) | 1750 | external-index |
| 2739 | 1953 | [1953. 你可以工作的最大周数](https://leetcode.cn/problems/maximum-number-of-weeks-for-which-you-can-work/) | 1804 | external-index |
| 2740 | 3785 | [3785. 避免禁用值的最小交换次数](https://leetcode.cn/problems/minimum-swaps-to-avoid-forbidden-values/) | 2052 | external-index |
| 2741 | 3664 | [3664. 两个字母卡牌游戏](https://leetcode.cn/problems/two-letter-card-game/) | 2158 | external-index |
| 2742 | 3495 | [3495. 使数组元素都变为零的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-make-array-elements-zero/) | 2206 | external-index |
| 2743 | 3139 | [3139. 使数组中所有元素相等的最小开销](https://leetcode.cn/problems/minimum-cost-to-equalize-array/) | 2666 | external-index |
| 2744 | 621 | [621. 任务调度器](https://leetcode.cn/problems/task-scheduler/) | Unknown | external-index |
| 2745 | 358 | [358. K 距离间隔重排字符串](https://leetcode.cn/problems/rearrange-string-k-distance-apart/) | Unknown | external-index |
| 2746 | 984 | [984. 不含 AAA 或 BBB 的字符串](https://leetcode.cn/problems/string-without-aaa-or-bbb/) | Unknown | external-index |
| 2747 | 1405 | [1405. 最长快乐字符串](https://leetcode.cn/problems/longest-happy-string/) | 1821 | external-index |

## 一、贪心策略 / §1.9 反悔贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2748 | LCP 30 | [LCP 30. 魔塔游戏](https://leetcode.cn/problems/p0NxJO/) | 1900 | external-index |
| 2749 | 1642 | [1642. 可以到达的最远建筑](https://leetcode.cn/problems/furthest-building-you-can-reach/) | 1962 | external-index |
| 2750 | 630 | [630. 课程表 III](https://leetcode.cn/problems/course-schedule-iii/) | 2000 | external-index |
| 2751 | 871 | [871. 最低加油次数](https://leetcode.cn/problems/minimum-number-of-refueling-stops/) | 2074 | external-index |
| 2752 | 3362 | [3362. 零数组变换 III](https://leetcode.cn/problems/zero-array-transformation-iii/) | 2424 | external-index |
| 2753 | 2813 | [2813. 子序列最大优雅度](https://leetcode.cn/problems/maximum-elegance-of-a-k-length-subsequence/) | 2582 | external-index |
| 2754 | 3049 | [3049. 标记所有下标的最早秒数 II](https://leetcode.cn/problems/earliest-second-to-mark-indices-ii/) | 3111 | external-index |
| 2755 | 3711 | [3711. 不出现负余额的最大交易额](https://leetcode.cn/problems/maximum-transactions-without-negative-balance/) | Unknown | external-index |
| 2756 | 2599 | [2599. 使前缀和数组非负](https://leetcode.cn/problems/make-the-prefix-sum-non-negative/) | Unknown | external-index |

## 二、区间贪心 / §2.1 不相交区间

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2757 | 435 | [435. 无重叠区间](https://leetcode.cn/problems/non-overlapping-intervals/) | 1700 | external-index |
| 2758 | 646 | [646. 最长数对链](https://leetcode.cn/problems/maximum-length-of-pair-chain/) | 435 | external-index |
| 2759 | 1520 | [1520. 最多的不重叠子字符串](https://leetcode.cn/problems/maximum-number-of-non-overlapping-substrings/) | 2363 | external-index |
| 2760 | 3458 | [3458. 选择 K 个互不重叠的特殊子字符串](https://leetcode.cn/problems/select-k-disjoint-special-substrings/) | 1520 | external-index |

## 二、区间贪心 / §2.2 区间分组

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2761 | 2406 | [2406. 将区间分为最少组数](https://leetcode.cn/problems/divide-intervals-into-minimum-number-of-groups/) | 1713 | external-index |
| 2762 | 253 | [253. 会议室 II](https://leetcode.cn/problems/meeting-rooms-ii/) | Unknown | external-index |

## 二、区间贪心 / §2.3 区间选点

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2763 | 452 | [452. 用最少数量的箭引爆气球](https://leetcode.cn/problems/minimum-number-of-arrows-to-burst-balloons/) | 1700 | external-index |
| 2764 | 757 | [757. 设置交集大小至少为2](https://leetcode.cn/problems/set-intersection-size-at-least-two/) | 2379 | external-index |
| 2765 | 2589 | [2589. 完成所有任务的最少时间](https://leetcode.cn/problems/minimum-time-to-complete-all-tasks/) | 2381 | external-index |
| 2766 | LCP 32 | [LCP 32. 批量处理任务](https://leetcode.cn/problems/t3fKg1/) | Unknown | external-index |

## 二、区间贪心 / §2.4 区间覆盖

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2767 | 45 | [45. 跳跃游戏 II](https://leetcode.cn/problems/jump-game-ii/) | 1700 | external-index |
| 2768 | 1024 | [1024. 视频拼接](https://leetcode.cn/problems/video-stitching/) | 1746 | external-index |
| 2769 | 1326 | [1326. 灌溉花园的最少水龙头数目](https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/) | 1885 | external-index |

## 二、区间贪心 / §2.5 合并区间

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2770 | 56 | [56. 合并区间](https://leetcode.cn/problems/merge-intervals/) | Unknown | external-index |
| 2771 | 57 | [57. 插入区间](https://leetcode.cn/problems/insert-interval/) | Unknown | external-index |
| 2772 | 2848 | [2848. 与车相交的点](https://leetcode.cn/problems/points-that-intersect-with-cars/) | Unknown | external-index |
| 2773 | 55 | [55. 跳跃游戏](https://leetcode.cn/problems/jump-game/) | Unknown | external-index |
| 2774 | 763 | [763. 划分字母区间](https://leetcode.cn/problems/partition-labels/) | 1443 | external-index |
| 2775 | 3169 | [3169. 无需开会的工作日](https://leetcode.cn/problems/count-days-without-meetings/) | 1483 | external-index |
| 2776 | 3951 | [3951. 维持亮度的最小总能量](https://leetcode.cn/problems/minimum-energy-to-maintain-brightness/) | 1530 | external-index |
| 2777 | 3975 | [3975. 筛选忙碌区间](https://leetcode.cn/problems/filter-occupied-intervals/) | 1533 | external-index |
| 2778 | 3964 | [3964. 照亮道路的最少灯泡数](https://leetcode.cn/problems/minimum-lights-to-illuminate-a-road/) | 1572 | external-index |
| 2779 | 2580 | [2580. 统计将重叠区间合并成组的方案数](https://leetcode.cn/problems/count-ways-to-group-overlapping-ranges/) | 1632 | external-index |
| 2780 | 3394 | [3394. 判断网格图能否被切割成块](https://leetcode.cn/problems/check-if-grid-can-be-cut-into-sections/) | 1916 | external-index |
| 2781 | 2963 | [2963. 统计好分割方案的数目](https://leetcode.cn/problems/count-the-number-of-good-partitions/) | 1985 | external-index |
| 2782 | 2584 | [2584. 分割数组使乘积互质](https://leetcode.cn/problems/split-the-array-to-make-coprime-products/) | 2159 | external-index |
| 2783 | 616 | [616. 给字符串添加加粗标签](https://leetcode.cn/problems/add-bold-tag-in-string/) | Unknown | external-index |
| 2784 | 758 | [758. 字符串中的加粗单词](https://leetcode.cn/problems/bold-words-in-string/) | Unknown | external-index |
| 2785 | 3323 | [3323. 通过插入区间最小化连通组](https://leetcode.cn/problems/minimize-connected-groups-by-inserting-interval/) | Unknown | external-index |
| 2786 | 759 | [759. 员工空闲时间](https://leetcode.cn/problems/employee-free-time/) | Unknown | external-index |
| 2787 | 2655 | [2655. 寻找最大长度的未覆盖区间](https://leetcode.cn/problems/find-maximal-uncovered-ranges/) | Unknown | external-index |

## 二、区间贪心 / §2.6 其他区间贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2788 | 1288 | [1288. 删除被覆盖区间](https://leetcode.cn/problems/remove-covered-intervals/) | Unknown | external-index |
| 2789 | 2054 | [2054. 两个最好的不重叠活动](https://leetcode.cn/problems/two-best-non-overlapping-events/) | 1883 | external-index |
| 2790 | 1705 | [1705. 吃苹果的最大数目](https://leetcode.cn/problems/maximum-number-of-eaten-apples/) | 1930 | external-index |
| 2791 | 1353 | [1353. 最多可以参加的会议数目](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended/) | 2016 | external-index |

## 三、字符串贪心 / §3.1 字典序最小/最大

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2792 | 1323 | [1323. 6 和 9 组成的最大数字](https://leetcode.cn/problems/maximum-69-number/) | 1194 | external-index |
| 2793 | 4000 | [4000. 给定数位和的最大整数](https://leetcode.cn/problems/largest-integer-with-given-digit-sum/) | 1200 | external-index |
| 2794 | 3216 | [3216. 交换后字典序最小的字符串](https://leetcode.cn/problems/lexicographically-smallest-string-after-a-swap/) | 1243 | external-index |
| 2795 | 2697 | [2697. 字典序最小回文串](https://leetcode.cn/problems/lexicographically-smallest-palindrome/) | 1304 | external-index |
| 2796 | 3517 | [3517. 最小回文排列 I](https://leetcode.cn/problems/smallest-palindromic-rearrangement-i/) | 1357 | external-index |
| 2797 | 1881 | [1881. 插入后的最大值](https://leetcode.cn/problems/maximum-value-after-insertion/) | 1381 | external-index |
| 2798 | 2734 | [2734. 执行子串操作后的字典序最小字符串](https://leetcode.cn/problems/lexicographically-smallest-string-after-substring-operation/) | 1405 | external-index |
| 2799 | 1432 | [1432. 改变一个整数能得到的最大差值](https://leetcode.cn/problems/max-difference-you-can-get-from-changing-an-integer/) | 1427 | external-index |
| 2800 | 1946 | [1946. 子字符串突变后可能得到的最大整数](https://leetcode.cn/problems/largest-number-after-mutating-substring/) | 1445 | external-index |
| 2801 | 1663 | [1663. 具有给定数值的最小字符串](https://leetcode.cn/problems/smallest-string-with-a-given-numeric-value/) | 1461 | external-index |
| 2802 | 1328 | [1328. 破坏回文串](https://leetcode.cn/problems/break-a-palindrome/) | 1474 | external-index |
| 2803 | 2259 | [2259. 移除指定数字得到的最大结果](https://leetcode.cn/problems/remove-digit-from-number-to-maximize-result/) | Unknown | external-index |
| 2804 | 2566 | [2566. 替换一个数字后的最大差值](https://leetcode.cn/problems/maximum-difference-by-remapping-a-digit/) | Unknown | external-index |
| 2805 | 670 | [670. 最大交换](https://leetcode.cn/problems/maximum-swap/) | Unknown | external-index |
| 2806 | 3106 | [3106. 满足距离约束且字典序最小的字符串](https://leetcode.cn/problems/lexicographically-smallest-string-after-operations-with-constraint/) | 1515 | external-index |
| 2807 | 3723 | [3723. 数位平方和的最大值](https://leetcode.cn/problems/maximize-sum-of-squares-of-digits/) | 1537 | external-index |
| 2808 | 1053 | [1053. 交换一次的先前排列](https://leetcode.cn/problems/previous-permutation-with-one-swap/) | 1633 | external-index |
| 2809 | 2375 | [2375. 根据模式串构造最小数字](https://leetcode.cn/problems/construct-smallest-number-from-di-string/) | 1642 | external-index |
| 2810 | 2182 | [2182. 构造限制重复的字符串](https://leetcode.cn/problems/construct-string-with-repeat-limit/) | 1680 | external-index |
| 2811 | 738 | [738. 单调递增的数字](https://leetcode.cn/problems/monotone-increasing-digits/) | 1700 | external-index |
| 2812 | 3403 | [3403. 从盒子中找出字典序最大的字符串 I](https://leetcode.cn/problems/find-the-lexicographically-largest-string-from-the-box-i/) | 1762 | external-index |
| 2813 | 3170 | [3170. 删除星号以后字典序最小的字符串](https://leetcode.cn/problems/lexicographically-minimum-string-after-removing-stars/) | 1772 | external-index |
| 2814 | 1363 | [1363. 形成三的最大倍数](https://leetcode.cn/problems/largest-multiple-of-three/) | 1823 | external-index |
| 2815 | 3752 | [3752. 字典序最小和为目标值且绝对值是排列的数组](https://leetcode.cn/problems/lexicographically-smallest-negated-permutation-that-sums-to-target/) | 1827 | external-index |
| 2816 | 1754 | [1754. 构造字典序最大的合并字符串](https://leetcode.cn/problems/largest-merge-of-two-strings/) | 1829 | external-index |
| 2817 | 1202 | [1202. 交换字符串中的元素](https://leetcode.cn/problems/smallest-string-with-swaps/) | 1855 | external-index |
| 2818 | 2434 | [2434. 使用机器人打印字典序最小的字符串](https://leetcode.cn/problems/using-a-robot-to-print-the-lexicographically-smallest-string/) | 1953 | external-index |
| 2819 | 2948 | [2948. 交换得到字典序最小的数组](https://leetcode.cn/problems/make-lexicographically-smallest-array-by-swapping-elements/) | 2047 | external-index |
| 2820 | 1505 | [1505. 最多 K 次交换相邻数位后得到的最小整数](https://leetcode.cn/problems/minimum-possible-integer-after-at-most-k-adjacent-swaps-on-digits/) | 2337 | external-index |
| 2821 | 3474 | [3474. 字典序最小的生成字符串](https://leetcode.cn/problems/lexicographically-smallest-generated-string/) | 2605 | external-index |
| 2822 | 555 | [555. 分割连接字符串](https://leetcode.cn/problems/split-concatenated-strings/) | Unknown | external-index |
| 2823 | 3088 | [3088. 使字符串反回文](https://leetcode.cn/problems/make-string-anti-palindrome/) | Unknown | external-index |
| 2824 | 3720 | [3720. 大于目标字符串的最小字典序排列](https://leetcode.cn/problems/lexicographically-smallest-permutation-greater-than-target/) | 1958 | external-index |
| 2825 | 3734 | [3734. 大于目标字符串的最小字典序回文排列](https://leetcode.cn/problems/lexicographically-smallest-palindromic-permutation-greater-than-target/) | 2330 | external-index |
| 2826 | 2663 | [2663. 字典序最小的美丽字符串](https://leetcode.cn/problems/lexicographically-smallest-beautiful-string/) | 2416 | external-index |
| 2827 | 2048 | [2048. 下一个更大的数值平衡数](https://leetcode.cn/problems/next-greater-numerically-balanced-number/) | Unknown | external-index |
| 2828 | 3646 | [3646. 下一个特殊回文数](https://leetcode.cn/problems/next-special-palindrome-number/) | Unknown | external-index |
| 2829 | 3348 | [3348. 最小可整除数位乘积 II](https://leetcode.cn/problems/smallest-divisible-digit-product-ii/) | 3101 | external-index |

## 三、字符串贪心 / §3.2 回文串贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2830 | 409 | [409. 最长回文串](https://leetcode.cn/problems/longest-palindrome/) | 1250 | external-index |
| 2831 | 2697 | [2697. 字典序最小回文串](https://leetcode.cn/problems/lexicographically-smallest-palindrome/) | 1304 | external-index |
| 2832 | 3517 | [3517. 最小回文排列 I](https://leetcode.cn/problems/smallest-palindromic-rearrangement-i/) | 1357 | external-index |
| 2833 | 680 | [680. 验证回文串 II](https://leetcode.cn/problems/valid-palindrome-ii/) | 1400 | external-index |
| 2834 | 1328 | [1328. 破坏回文串](https://leetcode.cn/problems/break-a-palindrome/) | 1474 | external-index |
| 2835 | 1400 | [1400. 构造 K 个回文字符串](https://leetcode.cn/problems/construct-k-palindrome-strings/) | 1530 | external-index |
| 2836 | 2131 | [2131. 连接两字母单词得到的最长回文串](https://leetcode.cn/problems/longest-palindrome-by-concatenating-two-letter-words/) | 1557 | external-index |
| 2837 | 2384 | [2384. 最大回文数字](https://leetcode.cn/problems/largest-palindromic-number/) | 1636 | external-index |
| 2838 | 3035 | [3035. 回文字符串的最大数量](https://leetcode.cn/problems/maximum-palindromes-after-operations/) | 1857 | external-index |
| 2839 | 1616 | [1616. 分割两个字符串得到回文串](https://leetcode.cn/problems/split-two-strings-to-make-palindrome/) | 1868 | external-index |
| 2840 | 1147 | [1147. 段式回文](https://leetcode.cn/problems/longest-chunked-palindrome-decomposition/) | 1912 | external-index |
| 2841 | 2193 | [2193. 得到回文串的最少操作次数](https://leetcode.cn/problems/minimum-number-of-moves-to-make-palindrome/) | 2091 | external-index |
| 2842 | 3734 | [3734. 大于目标字符串的最小字典序回文排列](https://leetcode.cn/problems/lexicographically-smallest-palindromic-permutation-greater-than-target/) | 2330 | external-index |
| 2843 | 3646 | [3646. 下一个特殊回文数](https://leetcode.cn/problems/next-special-palindrome-number/) | Unknown | external-index |
| 2844 | 266 | [266. 回文排列](https://leetcode.cn/problems/palindrome-permutation/) | Unknown | external-index |
| 2845 | 2422 | [2422. 使用合并操作将数组转换为回文序列](https://leetcode.cn/problems/merge-operations-to-turn-array-into-a-palindrome/) | Unknown | external-index |
| 2846 | 1842 | [1842. 下个由相同数字构成的回文串](https://leetcode.cn/problems/next-palindrome-using-same-digits/) | Unknown | external-index |
| 2847 | 3088 | [3088. 使字符串反回文](https://leetcode.cn/problems/make-string-anti-palindrome/) | Unknown | external-index |

## 四、数学贪心 / §4.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2848 | 2160 | [2160. 拆分数位后四位数字的最小和](https://leetcode.cn/problems/minimum-sum-of-four-digit-number-after-splitting-digits/) | 1314 | external-index |
| 2849 | 2578 | [2578. 最小和分割](https://leetcode.cn/problems/split-with-minimum-sum/) | 1351 | external-index |
| 2850 | 2244 | [2244. 完成所有任务需要的最少轮数](https://leetcode.cn/problems/minimum-rounds-to-complete-all-tasks/) | 1372 | external-index |
| 2851 | 2870 | [2870. 使数组为空的最少操作次数](https://leetcode.cn/problems/minimum-number-of-operations-to-make-array-empty/) | 1392 | external-index |
| 2852 | 1217 | [1217. 玩筹码](https://leetcode.cn/problems/minimum-cost-to-move-chips-to-the-same-position/) | 1408 | external-index |
| 2853 | 3091 | [3091. 执行操作使数据元素之和大于等于 K](https://leetcode.cn/problems/apply-operations-to-make-sum-of-array-greater-than-or-equal-to-k/) | 1522 | external-index |
| 2854 | 397 | [397. 整数替换](https://leetcode.cn/problems/integer-replacement/) | Unknown | external-index |

## 四、数学贪心 / §4.2 乘积贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2855 | 628 | [628. 三个数的最大乘积](https://leetcode.cn/problems/maximum-product-of-three-numbers/) | Unknown | external-index |
| 2856 | 3732 | [3732. 一次替换后的三元素最大乘积](https://leetcode.cn/problems/maximum-product-of-three-elements-after-one-replacement/) | 1529 | external-index |
| 2857 | 1567 | [1567. 乘积为正数的最长子数组长度](https://leetcode.cn/problems/maximum-length-of-subarray-with-positive-product/) | 1710 | external-index |

## 四、数学贪心 / §4.3 排序不等式

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2858 | 4014 | [4014. 应用折扣后的最低总价](https://leetcode.cn/problems/minimum-total-price-after-applying-discounts/) | 1192 | external-index |
| 2859 | 3974 | [3974. K 个元素的最大总和](https://leetcode.cn/problems/maximum-total-sum-of-k-selected-elements/) | 1263 | external-index |
| 2860 | 2285 | [2285. 道路的最大总重要性](https://leetcode.cn/problems/maximum-total-importance-of-roads/) | 1496 | external-index |
| 2861 | 3016 | [3016. 输入单词需要的最少按键次数 II](https://leetcode.cn/problems/minimum-number-of-pushes-to-type-word-ii/) | 1534 | external-index |
| 2862 | 1402 | [1402. 做菜顺序](https://leetcode.cn/problems/reducing-dishes/) | 1679 | external-index |
| 2863 | 2931 | [2931. 购买物品的最大开销](https://leetcode.cn/problems/maximum-spending-after-buying-items/) | 1822 | external-index |
| 2864 | 1589 | [1589. 所有排列中的最大和](https://leetcode.cn/problems/maximum-sum-obtained-of-any-permutation/) | 1871 | external-index |
| 2865 | 1874 | [1874. 两个数组的最小乘积和](https://leetcode.cn/problems/minimize-product-sum-of-two-arrays/) | Unknown | external-index |
| 2866 | 2268 | [2268. 最少按键次数](https://leetcode.cn/problems/minimum-number-of-keypresses/) | 3016 | external-index |
| 2867 | 2561 | [2561. 重排水果](https://leetcode.cn/problems/rearranging-fruits/) | 2222 | external-index |
| 2868 | 3547 | [3547. 图中边值的最大和](https://leetcode.cn/problems/maximum-sum-of-edge-values-in-a-graph/) | Unknown | external-index |

## 四、数学贪心 / §4.4 均值不等式

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2869 | 3081 | [3081. 替换字符串中的问号使分数最小](https://leetcode.cn/problems/replace-question-marks-in-string-to-minimize-its-value/) | 1905 | external-index |
| 2870 | 1969 | [1969. 数组元素的最小非零乘积](https://leetcode.cn/problems/minimum-non-zero-product-of-the-array-elements/) | 1967 | external-index |
| 2871 | 2939 | [2939. 最大异或乘积](https://leetcode.cn/problems/maximum-xor-product/) | 2128 | external-index |
| 2872 | 3723 | [3723. 数位平方和的最大值](https://leetcode.cn/problems/maximize-sum-of-squares-of-digits/) | 1537 | external-index |
| 2873 | 2897 | [2897. 对数组执行操作使平方和最大](https://leetcode.cn/problems/apply-operations-on-array-to-maximize-sum-of-squares/) | 2301 | external-index |

## 四、数学贪心 / §4.5 中位数贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2874 | 462 | [462. 最小操作次数使数组元素相等 II](https://leetcode.cn/problems/minimum-moves-to-equal-array-elements-ii/) | Unknown | external-index |
| 2875 | 2033 | [2033. 获取单值网格的最小操作数](https://leetcode.cn/problems/minimum-operations-to-make-a-uni-value-grid/) | 1672 | external-index |
| 2876 | 2448 | [2448. 使数组相等的最小开销](https://leetcode.cn/problems/minimum-cost-to-make-array-equal/) | 2005 | external-index |
| 2877 | 2607 | [2607. 使子数组元素和相等](https://leetcode.cn/problems/make-k-subarray-sums-equal/) | 2071 | external-index |
| 2878 | 2967 | [2967. 使数组成为等数数组的最小代价](https://leetcode.cn/problems/minimum-cost-to-make-array-equalindromic/) | 2116 | external-index |
| 2879 | 1478 | [1478. 安排邮筒](https://leetcode.cn/problems/allocate-mailboxes/) | 2190 | external-index |
| 2880 | 2968 | [2968. 执行操作使频率分数最大](https://leetcode.cn/problems/apply-operations-to-maximize-frequency-score/) | 2444 | external-index |
| 2881 | 1703 | [1703. 得到连续 K 个 1 的最少相邻交换次数](https://leetcode.cn/problems/minimum-adjacent-swaps-for-k-consecutive-ones/) | 2467 | external-index |
| 2882 | 3762 | [3762. 使数组元素相等的最小操作次数](https://leetcode.cn/problems/minimum-operations-to-equalize-subarrays/) | 2497 | external-index |
| 2883 | 3086 | [3086. 拾起 K 个 1 需要的最少行动次数](https://leetcode.cn/problems/minimum-moves-to-pick-k-ones/) | 2673 | external-index |
| 2884 | LCP 24 | [LCP 24. 数字游戏](https://leetcode.cn/problems/5TxKeK/) | Unknown | external-index |
| 2885 | 3441 | [3441. 变成好标题的最少代价](https://leetcode.cn/problems/minimum-cost-good-caption/) | Unknown | external-index |
| 2886 | 296 | [296. 最佳的碰头地点](https://leetcode.cn/problems/best-meeting-point/) | Unknown | external-index |
| 2887 | 3937 | [3937. 使数组变为模交替数组的最少操作次数 I](https://leetcode.cn/problems/minimum-operations-to-make-array-modulo-alternating-i/) | Unknown | external-index |

## 四、数学贪心 / §4.6 归纳法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2888 | 2952 | [2952. 需要添加的硬币的最小数量](https://leetcode.cn/problems/minimum-number-of-coins-to-be-added/) | 1784 | external-index |
| 2889 | 330 | [330. 按要求补齐数组](https://leetcode.cn/problems/patching-array/) | 2952 | external-index |
| 2890 | 1798 | [1798. 你能构造出连续值的最大数目](https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/) | 1931 | external-index |

## 四、数学贪心 / §4.7 其他数学贪心

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2891 | 1414 | [1414. 和为 K 的最少斐波那契数字数目](https://leetcode.cn/problems/find-the-minimum-number-of-fibonacci-numbers-whose-sum-is-k/) | 1466 | external-index |
| 2892 | 3107 | [3107. 使数组中位数等于 K 的最少操作数](https://leetcode.cn/problems/minimum-operations-to-make-median-of-array-equal-to-k/) | 1605 | external-index |
| 2893 | 754 | [754. 到达终点数字](https://leetcode.cn/problems/reach-a-number/) | 2000 | external-index |
| 2894 | 1058 | [1058. 最小化舍入误差以满足目标](https://leetcode.cn/problems/minimize-rounding-error-to-meet-target/) | Unknown | external-index |

## 五、思维题 / §5.1 从特殊到一般

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2895 | 2745 | [2745. 构造最长的新字符串](https://leetcode.cn/problems/construct-the-longest-new-string/) | 1607 | external-index |
| 2896 | 2611 | [2611. 老鼠和奶酪](https://leetcode.cn/problems/mice-and-cheese/) | 1663 | external-index |
| 2897 | 1029 | [1029. 两地调度](https://leetcode.cn/problems/two-city-scheduling/) | 2611 | external-index |
| 2898 | 2645 | [2645. 构造有效字符串的最少插入数](https://leetcode.cn/problems/minimum-additions-to-make-valid-string/) | Unknown | external-index |
| 2899 | 2202 | [2202. K 次操作后最大化顶端元素](https://leetcode.cn/problems/maximize-the-topmost-element-after-k-moves/) | 1717 | external-index |
| 2900 | 2568 | [2568. 最小无法得到的或值](https://leetcode.cn/problems/minimum-impossible-or/) | 1754 | external-index |
| 2901 | 1702 | [1702. 修改后的最大二进制字符串](https://leetcode.cn/problems/maximum-binary-string-after-change/) | 1825 | external-index |
| 2902 | 3012 | [3012. 通过操作使数组长度最小](https://leetcode.cn/problems/minimize-length-of-array-using-operations/) | 1833 | external-index |
| 2903 | 2350 | [2350. 不可能得到的最短骰子序列](https://leetcode.cn/problems/shortest-impossible-sequence-of-rolls/) | 1961 | external-index |
| 2904 | 3660 | [3660. 跳跃游戏 IX](https://leetcode.cn/problems/jump-game-ix/) | 2187 | external-index |
| 2905 | 517 | [517. 超级洗衣机](https://leetcode.cn/problems/super-washing-machines/) | Unknown | external-index |
| 2906 | 2499 | [2499. 让数组不相等的最小总代价](https://leetcode.cn/problems/minimum-total-cost-to-make-arrays-unequal/) | 2633 | external-index |
| 2907 | 3357 | [3357. 最小化相邻元素的最大差值](https://leetcode.cn/problems/minimize-the-maximum-adjacent-element-difference/) | 3077 | external-index |
| 2908 | 3431 | [3431. 对数字排序的最小解锁下标](https://leetcode.cn/problems/minimum-unlocked-indices-to-sort-nums/) | Unknown | external-index |

## 五、思维题 / §5.2 脑筋急转弯

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2909 | 2733 | [2733. 既不是最小值也不是最大值](https://leetcode.cn/problems/neither-minimum-nor-maximum/) | 1148 | external-index |
| 2910 | 3432 | [3432. 统计元素和差值为偶数的分区方案](https://leetcode.cn/problems/count-partitions-with-even-sum-difference/) | 1200 | external-index |
| 2911 | 3875 | [3875. 构造奇偶一致的数组 I](https://leetcode.cn/problems/construct-uniform-parity-array-i/) | 1200 | external-index |
| 2912 | 1903 | [1903. 字符串中的最大奇数](https://leetcode.cn/problems/largest-odd-number-in-string/) | 1249 | external-index |
| 2913 | 3996 | [3996. 偶数次骑士移动](https://leetcode.cn/problems/even-number-of-knight-moves/) | 1260 | external-index |
| 2914 | 2549 | [2549. 统计桌面上的不同数字](https://leetcode.cn/problems/count-distinct-numbers-on-board/) | 1266 | external-index |
| 2915 | 3746 | [3746. 等量移除后的字符串最小长度](https://leetcode.cn/problems/minimum-string-length-after-balanced-removals/) | 1327 | external-index |
| 2916 | 2396 | [2396. 严格回文的数字](https://leetcode.cn/problems/strictly-palindromic-number/) | 1329 | external-index |
| 2917 | 1689 | [1689. 十-二进制数的最少数目](https://leetcode.cn/problems/partitioning-into-minimum-number-of-deci-binary-numbers/) | 1355 | external-index |
| 2918 | 3760 | [3760. 不同首字母的子字符串数目](https://leetcode.cn/problems/maximum-substrings-with-distinct-start/) | 1364 | external-index |
| 2919 | 3674 | [3674. 数组元素相等的最小操作次数](https://leetcode.cn/problems/minimum-operations-to-equalize-array/) | 1369 | external-index |
| 2920 | 3689 | [3689. 最大子数组总值 I](https://leetcode.cn/problems/maximum-total-subarray-value-i/) | 1371 | external-index |
| 2921 | 2078 | [2078. 两栋颜色不同且距离最远的房子](https://leetcode.cn/problems/two-furthest-houses-with-different-colors/) | Unknown | external-index |
| 2922 | 598 | [598. 区间加法 II](https://leetcode.cn/problems/range-addition-ii/) | 1400 | external-index |
| 2923 | 521 | [521. 最长特殊序列 Ⅰ](https://leetcode.cn/problems/longest-uncommon-subsequence-i/) | 1400 | external-index |
| 2924 | 3675 | [3675. 转换字符串的最小操作次数](https://leetcode.cn/problems/minimum-operations-to-transform-string/) | 1414 | external-index |
| 2925 | 3227 | [3227. 字符串元音游戏](https://leetcode.cn/problems/vowels-game-in-a-string/) | 1452 | external-index |
| 2926 | 3638 | [3638. 平衡装运的最大数量](https://leetcode.cn/problems/maximum-balanced-shipments/) | 1463 | external-index |
| 2927 | 3702 | [3702. 按位异或非零的最长子序列](https://leetcode.cn/problems/longest-subsequence-with-non-zero-bitwise-xor/) | 1489 | external-index |
| 2928 | 3810 | [3810. 变成目标数组的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-reach-target-array/) | 1492 | external-index |
| 2929 | 2419 | [2419. 按位与最大的最长子数组](https://leetcode.cn/problems/longest-subarray-with-maximum-bitwise-and/) | 1496 | external-index |
| 2930 | 3424 | [3424. 将数组变相同的最小代价](https://leetcode.cn/problems/minimum-cost-to-make-arrays-identical/) | 1503 | external-index |
| 2931 | 1992 | [1992. 找到所有的农场组](https://leetcode.cn/problems/find-all-groups-of-farmland/) | 1539 | external-index |
| 2932 | 1007 | [1007. 行相等的最少多米诺旋转](https://leetcode.cn/problems/minimum-domino-rotations-for-equal-row/) | 1541 | external-index |
| 2933 | 2811 | [2811. 判断是否能拆分数组](https://leetcode.cn/problems/check-if-it-is-possible-to-split-array/) | 1543 | external-index |
| 2934 | 3987 | [3987. 处理所有元素的成本](https://leetcode.cn/problems/minimum-total-cost-to-process-all-elements/) | 1550 | external-index |
| 2935 | 463 | [463. 岛屿的周长](https://leetcode.cn/problems/island-perimeter/) | Unknown | external-index |
| 2936 | 2211 | [2211. 统计道路上的碰撞次数](https://leetcode.cn/problems/count-collisions-on-a-road/) | 1581 | external-index |
| 2937 | 877 | [877. 石子游戏](https://leetcode.cn/problems/stone-game/) | 1590 | external-index |
| 2938 | 3207 | [3207. 与敌人战斗后的最大分数](https://leetcode.cn/problems/maximum-points-after-enemy-battles/) | 1591 | external-index |
| 2939 | 3828 | [3828. 删除子数组后的最终元素](https://leetcode.cn/problems/final-element-after-subarray-deletions/) | 1591 | external-index |
| 2940 | 3471 | [3471. 找出最大的几近缺失整数](https://leetcode.cn/problems/find-the-largest-almost-missing-integer/) | Unknown | external-index |
| 2941 | 2546 | [2546. 执行逐位运算使字符串相等](https://leetcode.cn/problems/apply-bitwise-operations-to-make-strings-equal/) | 1605 | external-index |
| 2942 | 1503 | [1503. 所有蚂蚁掉下来前的最后一刻](https://leetcode.cn/problems/last-moment-before-all-ants-fall-out-of-a-plank/) | 1619 | external-index |
| 2943 | 2860 | [2860. 让所有学生保持开心的分组方法数](https://leetcode.cn/problems/happy-students/) | 1626 | external-index |
| 2944 | 1332 | [1332. 删除回文子序列](https://leetcode.cn/problems/remove-palindromic-subsequences/) | 1629 | external-index |
| 2945 | 1975 | [1975. 最大方阵和](https://leetcode.cn/problems/maximum-matrix-sum/) | 1648 | external-index |
| 2946 | 1145 | [1145. 二叉树着色游戏](https://leetcode.cn/problems/binary-tree-coloring-game/) | 1741 | external-index |
| 2947 | 2087 | [2087. 网格图中机器人回家的最小代价](https://leetcode.cn/problems/minimum-cost-homecoming-of-a-robot-in-a-grid/) | 1744 | external-index |
| 2948 | 1297 | [1297. 子串的最大出现次数](https://leetcode.cn/problems/maximum-number-of-occurrences-of-a-substring/) | 1748 | external-index |
| 2949 | 3577 | [3577. 统计计算机解锁顺序排列数](https://leetcode.cn/problems/count-the-number-of-computer-unlocking-permutations/) | 1750 | external-index |
| 2950 | 3282 | [3282. 到达数组末尾的最大得分](https://leetcode.cn/problems/reach-end-of-array-with-max-score/) | 1772 | external-index |
| 2951 | 3644 | [3644. 排序排列](https://leetcode.cn/problems/maximum-k-to-sort-a-permutation/) | 1775 | external-index |
| 2952 | 2712 | [2712. 使所有字符相等的最小成本](https://leetcode.cn/problems/minimum-cost-to-make-all-characters-equal/) | 1791 | external-index |
| 2953 | 3148 | [3148. 矩阵中的最大得分](https://leetcode.cn/problems/maximum-difference-score-in-a-grid/) | 1820 | external-index |
| 2954 | 2311 | [2311. 小于等于 K 的最长二进制子序列](https://leetcode.cn/problems/longest-binary-subsequence-less-than-or-equal-to-k/) | 1840 | external-index |
| 2955 | 2332 | [2332. 坐上公交的最晚时间](https://leetcode.cn/problems/the-latest-time-to-catch-a-bus/) | 1841 | external-index |
| 2956 | 3961 | [3961. 设备评分的最大和](https://leetcode.cn/problems/maximize-sum-of-device-ratings/) | 1880 | external-index |
| 2957 | 2680 | [2680. 最大或值](https://leetcode.cn/problems/maximum-or/) | 1912 | external-index |
| 2958 | 2731 | [2731. 移动机器人](https://leetcode.cn/problems/movement-of-robots/) | 1923 | external-index |
| 2959 | 2556 | [2556. 二进制矩阵中翻转最多一次使路径不连通](https://leetcode.cn/problems/disconnect-path-in-a-binary-matrix-by-at-most-one-flip/) | 2369 | external-index |
| 2960 | 3353 | [3353. 最小总操作数](https://leetcode.cn/problems/minimum-total-operations/) | Unknown | external-index |
| 2961 | 1708 | [1708. 长度为 K 的最大子数组](https://leetcode.cn/problems/largest-subarray-length-k/) | Unknown | external-index |
| 2962 | 3596 | [3596. 最小花费路径交替方向 I](https://leetcode.cn/problems/minimum-cost-path-with-alternating-directions-i/) | Unknown | external-index |
| 2963 | 3496 | [3496. 最大化配对删除后的得分](https://leetcode.cn/problems/maximize-score-after-pair-deletions/) | Unknown | external-index |
| 2964 | 3125 | [3125. 使得按位与结果为 0 的最大数字](https://leetcode.cn/problems/maximum-number-that-makes-result-of-bitwise-and-zero/) | Unknown | external-index |
| 2965 | 1794 | [1794. 统计距离最小的子串对个数](https://leetcode.cn/problems/count-pairs-of-equal-substrings-with-minimum-difference/) | Unknown | external-index |

## 五、思维题 / §5.3 等价转化

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2966 | 453 | [453. 最小操作次数使数组元素相等](https://leetcode.cn/problems/minimum-moves-to-equal-array-elements/) | Unknown | external-index |
| 2967 | 3375 | [3375. 使数组的值全部为 K 的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-make-array-values-equal-to-k/) | 1383 | external-index |
| 2968 | 2840 | [2840. 判断通过操作能否让字符串相等 II](https://leetcode.cn/problems/check-if-strings-can-be-made-equal-with-operations-ii/) | 1486 | external-index |
| 2969 | 2914 | [2914. 使二进制字符串变美丽的最少修改次数](https://leetcode.cn/problems/minimum-number-of-changes-to-make-binary-string-beautiful/) | 1480 | external-index |
| 2970 | 3365 | [3365. 重排子字符串以形成目标字符串](https://leetcode.cn/problems/rearrange-k-substrings-to-form-target-string/) | 1514 | external-index |
| 2971 | 1657 | [1657. 确定两个字符串是否接近](https://leetcode.cn/problems/determine-if-two-strings-are-close/) | 1530 | external-index |
| 2972 | 3868 | [3868. 通过交换使数组相等的最小花费](https://leetcode.cn/problems/minimum-cost-to-equalize-arrays-using-swaps/) | 1579 | external-index |
| 2973 | 3857 | [3857. 拆分到 1 的最小总代价](https://leetcode.cn/problems/minimum-cost-to-split-into-ones/) | Unknown | external-index |
| 2974 | 2551 | [2551. 将珠子放入背包中](https://leetcode.cn/problems/put-marbles-in-bags/) | 2042 | external-index |
| 2975 | 391 | [391. 完美矩形](https://leetcode.cn/problems/perfect-rectangle/) | Unknown | external-index |
| 2976 | 1585 | [1585. 检查字符串是否可以通过排序子字符串得到另一个字符串](https://leetcode.cn/problems/check-if-string-is-transformable-with-substring-sort-operations/) | 2333 | external-index |
| 2977 | 1040 | [1040. 移动石子直到连续 II](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/) | 2456 | external-index |
| 2978 | 1622 | [1622. 奇妙序列](https://leetcode.cn/problems/fancy-sequence/) | 2476 | external-index |
| 2979 | 249 | [249. 移位字符串分组](https://leetcode.cn/problems/group-shifted-strings/) | Unknown | external-index |
| 2980 | 49 | [49. 字母异位词分组](https://leetcode.cn/problems/group-anagrams/) | Unknown | external-index |
| 2981 | 1183 | [1183. 矩阵中 1 的最大数量](https://leetcode.cn/problems/maximum-number-of-ones/) | Unknown | external-index |

## 五、思维题 / §5.4 逆向思维

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2982 | 2139 | [2139. 得到目标值的最少行动次数](https://leetcode.cn/problems/minimum-moves-to-reach-target-score/) | 1417 | external-index |
| 2983 | 3779 | [3779. 得到互不相同元素的最少操作次数](https://leetcode.cn/problems/minimum-number-of-operations-to-have-distinct-elements/) | 1444 | external-index |
| 2984 | 3396 | [3396. 使数组元素互不相同所需的最少操作次数](https://leetcode.cn/problems/minimum-number-of-operations-to-make-elements-in-array-distinct/) | 3779 | external-index |
| 2985 | 3147 | [3147. 从魔法师身上吸取的最大能量](https://leetcode.cn/problems/taking-maximum-energy-from-the-mystic-dungeon/) | 1460 | external-index |
| 2986 | 869 | [869. 重新排序得到 2 的幂](https://leetcode.cn/problems/reordered-power-of-2/) | 1505 | external-index |
| 2987 | 1558 | [1558. 得到目标数组的最少函数调用次数](https://leetcode.cn/problems/minimum-numbers-of-function-calls-to-make-target-array/) | 1637 | external-index |
| 2988 | 554 | [554. 砖墙](https://leetcode.cn/problems/brick-wall/) | 1700 | external-index |
| 2989 | 780 | [780. 到达终点](https://leetcode.cn/problems/reaching-points/) | 1897 | external-index |
| 2990 | 417 | [417. 太平洋大西洋水流问题](https://leetcode.cn/problems/pacific-atlantic-water-flow/) | 1900 | external-index |
| 2991 | 991 | [991. 坏了的计算器](https://leetcode.cn/problems/broken-calculator/) | 1909 | external-index |
| 2992 | 2227 | [2227. 加密解密字符串](https://leetcode.cn/problems/encrypt-and-decrypt-strings/) | 1945 | external-index |
| 2993 | 1354 | [1354. 多次求和构造目标数组](https://leetcode.cn/problems/construct-target-array-with-multiple-sums/) | 2015 | external-index |
| 2994 | 2543 | [2543. 判断一个点是否可以到达](https://leetcode.cn/problems/check-if-point-is-reachable/) | 2221 | external-index |
| 2995 | 3307 | [3307. 找出第 K 个字符 II](https://leetcode.cn/problems/find-the-k-th-character-in-string-game-ii/) | 2232 | external-index |
| 2996 | 3614 | [3614. 用特殊操作处理字符串 II](https://leetcode.cn/problems/process-string-with-special-operations-ii/) | Unknown | external-index |
| 2997 | 3419 | [3419. 图的最大边权的最小值](https://leetcode.cn/problems/minimize-the-maximum-edge-weight-of-graph/) | 2243 | external-index |
| 2998 | 1611 | [1611. 使整数变为 0 的最少操作次数](https://leetcode.cn/problems/minimum-one-bit-operations-to-make-integers-zero/) | 2345 | external-index |
| 2999 | 3609 | [3609. 到达目标点的最小移动次数](https://leetcode.cn/problems/minimum-moves-to-reach-target-in-grid/) | 2419 | external-index |
| 3000 | 936 | [936. 戳印序列](https://leetcode.cn/problems/stamping-the-sequence/) | 2583 | external-index |
| 3001 | 782 | [782. 变为棋盘](https://leetcode.cn/problems/transform-to-chessboard/) | Unknown | external-index |
| 3002 | 683 | [683. K 个关闭的灯泡](https://leetcode.cn/problems/k-empty-slots/) | Unknown | external-index |
| 3003 | 3141 | [3141. 最大汉明距离](https://leetcode.cn/problems/maximum-hamming-distances/) | Unknown | external-index |
| 3004 | 3607 | [3607. 电网维护](https://leetcode.cn/problems/power-grid-maintenance/) | 1700 | external-index |
| 3005 | 2718 | [2718. 查询后矩阵的和](https://leetcode.cn/problems/sum-of-matrix-after-queries/) | 1769 | external-index |
| 3006 | 3639 | [3639. 变为活跃状态的最小时间](https://leetcode.cn/problems/minimum-time-to-activate-string/) | 1853 | external-index |
| 3007 | 1970 | [1970. 你能穿过矩阵的最后一天](https://leetcode.cn/problems/last-day-where-you-can-still-cross/) | 2124 | external-index |
| 3008 | 2382 | [2382. 删除操作后的最大子段和](https://leetcode.cn/problems/maximum-segment-sum-after-removals/) | 2136 | external-index |
| 3009 | 803 | [803. 打砖块](https://leetcode.cn/problems/bricks-falling-when-hit/) | 2765 | external-index |

## 五、思维题 / §5.5 贡献法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 3010 | 3871 | [3871. 统计范围内的逗号 II](https://leetcode.cn/problems/count-commas-in-range-ii/) | 1381 | external-index |
| 3011 | 2063 | [2063. 所有子字符串中的元音](https://leetcode.cn/problems/vowels-of-all-substrings/) | 1663 | external-index |
| 3012 | 979 | [979. 在二叉树中分配硬币](https://leetcode.cn/problems/distribute-coins-in-binary-tree/) | 1709 | external-index |
| 3013 | 1588 | [1588. 所有奇数长度子数组的和](https://leetcode.cn/problems/sum-of-all-odd-length-subarrays/) | Unknown | external-index |
| 3014 | 3771 | [3771. 探索地牢的得分](https://leetcode.cn/problems/total-score-of-dungeon-runs/) | 1981 | external-index |
| 3015 | 2477 | [2477. 到达首都的最少油耗](https://leetcode.cn/problems/minimum-fuel-cost-to-report-to-the-capital/) | 2012 | external-index |
| 3016 | 2681 | [2681. 英雄的力量](https://leetcode.cn/problems/power-of-heroes/) | 2060 | external-index |
| 3017 | 3855 | [3855. 给定范围内 K 位数字之和](https://leetcode.cn/problems/sum-of-k-digit-numbers-in-a-range/) | 2085 | external-index |
| 3018 | 3786 | [3786. 树组的交互代价总和](https://leetcode.cn/problems/total-sum-of-interaction-cost-in-tree-groups/) | 2139 | external-index |
| 3019 | 891 | [891. 子序列宽度之和](https://leetcode.cn/problems/sum-of-subsequence-widths/) | 2183 | external-index |
| 3020 | 3428 | [3428. 最多 K 个元素的子序列的最值之和](https://leetcode.cn/problems/maximum-and-minimum-sums-of-at-most-size-k-subsequences/) | 891 | external-index |
| 3021 | 3426 | [3426. 所有安放棋子方案的曼哈顿距离](https://leetcode.cn/problems/manhattan-distances-of-all-arrangements-of-pieces/) | 2443 | external-index |
| 3022 | 2763 | [2763. 所有子数组中不平衡数字之和](https://leetcode.cn/problems/sum-of-imbalance-numbers-of-all-subarrays/) | 2700 | external-index |

## 五、思维题 / §5.6 两次扫描

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 3023 | 135 | [135. 分发糖果](https://leetcode.cn/problems/candy/) | Unknown | external-index |
| 3024 | 3796 | [3796. 找到带限制序列的最大值](https://leetcode.cn/problems/find-maximum-value-in-a-constrained-sequence/) | 1833 | external-index |
| 3025 | 3494 | [3494. 酿造药水需要的最少总时间](https://leetcode.cn/problems/find-the-minimum-amount-of-time-to-brew-potions/) | 2042 | external-index |
| 3026 | 1840 | [1840. 最高建筑高度](https://leetcode.cn/problems/maximum-building-height/) | 2374 | external-index |

## 五、思维题 / §5.7 交换元素

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 3027 | 1202 | [1202. 交换字符串中的元素](https://leetcode.cn/problems/smallest-string-with-swaps/) | 1855 | external-index |
| 3028 | 1722 | [1722. 执行交换操作后的最小汉明距离](https://leetcode.cn/problems/minimize-hamming-distance-after-swap-operations/) | 1892 | external-index |
| 3029 | 3695 | [3695. 交换元素后的最大交替和](https://leetcode.cn/problems/maximize-alternating-sum-using-swaps/) | 1984 | external-index |
| 3030 | 2948 | [2948. 交换得到字典序最小的数组](https://leetcode.cn/problems/make-lexicographically-smallest-array-by-swapping-elements/) | 2047 | external-index |

## 五、思维题 / §5.8 分类讨论

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 3031 | 2525 | [2525. 根据规则将箱子分类](https://leetcode.cn/problems/categorize-box-according-to-criteria/) | 1301 | external-index |
| 3032 | 860 | [860. 柠檬水找零](https://leetcode.cn/problems/lemonade-change/) | Unknown | external-index |
| 3033 | 2165 | [2165. 重排数字的最小值](https://leetcode.cn/problems/smallest-value-of-the-rearranged-number/) | 1362 | external-index |
| 3034 | 2091 | [2091. 从数组中移除最大值和最小值](https://leetcode.cn/problems/removing-minimum-and-maximum-from-array/) | 1384 | external-index |
| 3035 | 1033 | [1033. 移动石子直到连续](https://leetcode.cn/problems/moving-stones-until-consecutive/) | 1421 | external-index |
| 3036 | 3876 | [3876. 构造奇偶一致的数组 II](https://leetcode.cn/problems/construct-uniform-parity-array-ii/) | 1444 | external-index |
| 3037 | 2918 | [2918. 数组的最小相等和](https://leetcode.cn/problems/minimum-equal-sum-of-two-arrays-after-replacing-zeros/) | 1526 | external-index |
| 3038 | 2591 | [2591. 将钱分给最多的儿童](https://leetcode.cn/problems/distribute-money-to-maximum-children/) | 1531 | external-index |
| 3039 | 2131 | [2131. 连接两字母单词得到的最长回文串](https://leetcode.cn/problems/longest-palindrome-by-concatenating-two-letter-words/) | 1557 | external-index |
| 3040 | 3789 | [3789. 采购的最小花费](https://leetcode.cn/problems/minimum-cost-to-acquire-required-items/) | 1580 | external-index |
| 3041 | 2844 | [2844. 生成特殊数字的最少操作](https://leetcode.cn/problems/minimum-operations-to-make-a-special-number/) | 1588 | external-index |
| 3042 | 1247 | [1247. 交换字符使得字符串相同](https://leetcode.cn/problems/minimum-swaps-to-make-strings-equal/) | 1597 | external-index |
| 3043 | 838 | [838. 推多米诺](https://leetcode.cn/problems/push-dominoes/) | 1638 | external-index |
| 3044 | 2170 | [2170. 使数组变成交替数组的最少操作数](https://leetcode.cn/problems/minimum-operations-to-make-the-array-alternating/) | 1663 | external-index |
| 3045 | 2202 | [2202. K 次操作后最大化顶端元素](https://leetcode.cn/problems/maximize-the-topmost-element-after-k-moves/) | 1717 | external-index |
| 3046 | 3922 | [3922. 使二进制字符串连贯的最少翻转次数](https://leetcode.cn/problems/minimum-flips-to-make-binary-string-coherent/) | 1759 | external-index |
| 3047 | 2982 | [2982. 找出出现至少三次的最长特殊子字符串 II](https://leetcode.cn/problems/find-longest-special-substring-that-occurs-thrice-ii/) | 1773 | external-index |
| 3048 | 3724 | [3724. 转换数组的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-transform-array/) | 1790 | external-index |
| 3049 | 3001 | [3001. 捕获黑皇后需要的最少移动次数](https://leetcode.cn/problems/minimum-moves-to-capture-the-queen/) | 1797 | external-index |
| 3050 | 2423 | [2423. 删除字符使频率相同](https://leetcode.cn/problems/remove-letter-to-equalize-frequency/) | Unknown | external-index |
| 3051 | 2934 | [2934. 最大化数组末位元素的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-maximize-last-elements-in-arrays/) | 1803 | external-index |
| 3052 | 3800 | [3800. 使二进制字符串相等的最小成本](https://leetcode.cn/problems/minimum-cost-to-make-two-binary-strings-equal/) | 1841 | external-index |
| 3053 | 2162 | [2162. 设置时间的最少代价](https://leetcode.cn/problems/minimum-cost-to-set-cooking-time/) | 1852 | external-index |
| 3054 | 3942 | [3942. 排序排列的最少操作数](https://leetcode.cn/problems/minimum-operations-to-sort-a-permutation/) | 1855 | external-index |
| 3055 | 3863 | [3863. 将一个字符串排序的最小操作次数](https://leetcode.cn/problems/minimum-operations-to-sort-a-string/) | 1860 | external-index |
| 3056 | 3440 | [3440. 重新安排会议得到最多空余时间 II](https://leetcode.cn/problems/reschedule-meetings-for-maximum-free-time-ii/) | 1998 | external-index |
| 3057 | 3886 | [3886. 可排序整数求和](https://leetcode.cn/problems/sum-of-sortable-integers/) | 1999 | external-index |
| 3058 | 1927 | [1927. 求和游戏](https://leetcode.cn/problems/sum-game/) | 2005 | external-index |
| 3059 | 2508 | [2508. 添加边使所有节点度数都为偶数](https://leetcode.cn/problems/add-edges-to-make-degrees-of-all-nodes-even/) | 2060 | external-index |
| 3060 | 3240 | [3240. 最少翻转次数使二进制矩阵回文 II](https://leetcode.cn/problems/minimum-number-of-flips-to-make-binary-grid-palindromic-ii/) | 2080 | external-index |
| 3061 | 3854 | [3854. 使数组奇偶交替的最少操作](https://leetcode.cn/problems/minimum-operations-to-make-array-parity-alternating/) | 2095 | external-index |
| 3062 | 3548 | [3548. 等和矩阵分割 II](https://leetcode.cn/problems/equal-sum-grid-partition-ii/) | 2245 | external-index |
| 3063 | 2029 | [2029. 石子游戏 IX](https://leetcode.cn/problems/stone-game-ix/) | 2277 | external-index |
| 3064 | 2468 | [2468. 根据限制分割消息](https://leetcode.cn/problems/split-message-based-on-limit/) | 2382 | external-index |
| 3065 | 420 | [420. 强密码检验器](https://leetcode.cn/problems/strong-password-checker/) | Unknown | external-index |
| 3066 | 3609 | [3609. 到达目标点的最小移动次数](https://leetcode.cn/problems/minimum-moves-to-reach-target-in-grid/) | 2419 | external-index |
| 3067 | 1040 | [1040. 移动石子直到连续 II](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/) | 2456 | external-index |
| 3068 | 335 | [335. 路径交叉](https://leetcode.cn/problems/self-crossing/) | Unknown | external-index |
| 3069 | 1330 | [1330. 翻转子数组得到最大的数组值](https://leetcode.cn/problems/reverse-subarray-to-maximize-array-value/) | 2482 | external-index |
| 3070 | 3197 | [3197. 包含所有 1 的最小矩形面积 II](https://leetcode.cn/problems/find-the-minimum-area-to-cover-all-ones-ii/) | 2541 | external-index |
| 3071 | 3366 | [3366. 最小数组和](https://leetcode.cn/problems/minimum-array-sum/) | Unknown | external-index |
| 3072 | 2499 | [2499. 让数组不相等的最小总代价](https://leetcode.cn/problems/minimum-total-cost-to-make-arrays-unequal/) | 2633 | external-index |
| 3073 | 3311 | [3311. 构造符合图结构的二维矩阵](https://leetcode.cn/problems/construct-2d-grid-matching-graph-layout/) | 2664 | external-index |
| 3074 | 3139 | [3139. 使数组中所有元素相等的最小开销](https://leetcode.cn/problems/minimum-cost-to-equalize-array/) | 2666 | external-index |
| 3075 | 3017 | [3017. 按距离统计房屋对数目 II](https://leetcode.cn/problems/count-the-number-of-houses-at-a-certain-distance-ii/) | 2709 | external-index |
| 3076 | 2983 | [2983. 回文串重新排列查询](https://leetcode.cn/problems/palindrome-rearrangement-queries/) | 2780 | external-index |
| 3077 | 3395 | [3395. 唯一中间众数子序列 I](https://leetcode.cn/problems/subsequences-with-a-unique-middle-mode-i/) | 2800 | external-index |
| 3078 | 3357 | [3357. 最小化相邻元素的最大差值](https://leetcode.cn/problems/minimize-the-maximum-adjacent-element-difference/) | 3077 | external-index |
| 3079 | 1900 | [1900. 最佳运动员的比拼回合](https://leetcode.cn/problems/the-earliest-and-latest-rounds-where-players-compete/) | Unknown | external-index |
| 3080 | LCP 48 | [LCP 48. 无限棋局](https://leetcode.cn/problems/fsa7oZ/) | Unknown | external-index |
| 3081 | LCP 21 | [LCP 21. 追逐游戏](https://leetcode.cn/problems/Za25hA/) | Unknown | external-index |

## 六、构造题

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 3082 | 1304 | [1304. 和为零的 N 个不同整数](https://leetcode.cn/problems/find-n-unique-integers-sum-up-to-zero/) | 1167 | external-index |
| 3083 | 3963 | [3963. 构造恰好一条路径的网格](https://leetcode.cn/problems/create-grid-with-exactly-one-path/) | 1311 | external-index |
| 3084 | 942 | [942. 增减字符串匹配](https://leetcode.cn/problems/di-string-match/) | 1444 | external-index |
| 3085 | 1968 | [1968. 构造元素不等于两相邻元素平均值的数组](https://leetcode.cn/problems/array-with-elements-not-equal-to-average-of-neighbors/) | 1499 | external-index |
| 3086 | 1253 | [1253. 重构 2 行二进制矩阵](https://leetcode.cn/problems/reconstruct-a-2-row-binary-matrix/) | 1506 | external-index |
| 3087 | 1317 | [1317. 将整数转换为两个无零整数的和](https://leetcode.cn/problems/convert-integer-to-the-sum-of-two-no-zero-integers/) | Unknown | external-index |
| 3088 | 3513 | [3513. 不同 XOR 三元组的数目 I](https://leetcode.cn/problems/number-of-unique-xor-triplets-i/) | 1663 | external-index |
| 3089 | 2182 | [2182. 构造限制重复的字符串](https://leetcode.cn/problems/construct-string-with-repeat-limit/) | 1680 | external-index |
| 3090 | 3592 | [3592. 硬币面值还原](https://leetcode.cn/problems/inverse-coin-change/) | 1701 | external-index |
| 3091 | 969 | [969. 煎饼排序](https://leetcode.cn/problems/pancake-sorting/) | 1800 | external-index |
| 3092 | 406 | [406. 根据身高重建队列](https://leetcode.cn/problems/queue-reconstruction-by-height/) | Unknown | external-index |
| 3093 | 1605 | [1605. 给定行和列的和求可行矩阵](https://leetcode.cn/problems/find-valid-matrix-given-row-and-column-sums/) | 1868 | external-index |
| 3094 | 2375 | [2375. 根据模式串构造最小数字](https://leetcode.cn/problems/construct-smallest-number-from-di-string/) | Unknown | external-index |
| 3095 | 324 | [324. 摆动排序 II](https://leetcode.cn/problems/wiggle-sort-ii/) | 2000 | external-index |
| 3096 | 3988 | [3988. 创建一个恰好有 K 条路径的网格图 I](https://leetcode.cn/problems/create-grid-with-exactly-k-paths-i/) | 2055 | external-index |
| 3097 | 667 | [667. 优美的排列 II](https://leetcode.cn/problems/beautiful-arrangement-ii/) | 2100 | external-index |
| 3098 | 2122 | [2122. 还原原数组](https://leetcode.cn/problems/recover-the-original-array/) | 2159 | external-index |
| 3099 | 1980 | [1980. 找出不同的二进制字符串](https://leetcode.cn/problems/find-unique-binary-string/) | Unknown | external-index |
| 3100 | 3680 | [3680. 生成赛程](https://leetcode.cn/problems/generate-schedule/) | 2378 | external-index |
| 3101 | 932 | [932. 漂亮数组](https://leetcode.cn/problems/beautiful-array/) | 2500 | external-index |
| 3102 | 2790 | [2790. 长度递增组的最大数目](https://leetcode.cn/problems/maximum-number-of-groups-with-increasing-length/) | 2620 | external-index |
| 3103 | 3311 | [3311. 构造符合图结构的二维矩阵](https://leetcode.cn/problems/construct-2d-grid-matching-graph-layout/) | 2664 | external-index |
| 3104 | 2573 | [2573. 找出对应 LCP 矩阵的字符串](https://leetcode.cn/problems/find-the-string-with-lcp/) | 2682 | external-index |
| 3105 | 1982 | [1982. 从子集的和还原数组](https://leetcode.cn/problems/find-array-given-subset-sums/) | 2872 | external-index |
| 3106 | 280 | [280. 摆动排序](https://leetcode.cn/problems/wiggle-sort/) | Unknown | external-index |
| 3107 | 484 | [484. 寻找排列](https://leetcode.cn/problems/find-permutation/) | Unknown | external-index |
| 3108 | 3990 | [3990. 创建一个恰好有 K 条路径的网格图 II](https://leetcode.cn/problems/create-grid-with-exactly-k-paths-ii/) | Unknown | external-index |

## 七、交互题

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 3109 | 278 | [278. 第一个错误的版本](https://leetcode.cn/problems/first-bad-version/) | Unknown | external-index |
| 3110 | 374 | [374. 猜数字大小](https://leetcode.cn/problems/guess-number-higher-or-lower/) | Unknown | external-index |
| 3111 | 1237 | [1237. 找出给定方程的正整数解](https://leetcode.cn/problems/find-positive-integer-solution-for-a-given-equation/) | Unknown | external-index |
| 3112 | 1095 | [1095. 山脉数组中查找目标值](https://leetcode.cn/problems/find-in-mountain-array/) | 1827 | external-index |
| 3113 | 843 | [843. 猜猜这个单词](https://leetcode.cn/problems/guess-the-word/) | 2078 | external-index |
| 3114 | 2728 | [2728. 计算一个环形街道上的房屋数量](https://leetcode.cn/problems/count-houses-in-a-circular-street/) | Unknown | external-index |
| 3115 | 277 | [277. 搜寻名人](https://leetcode.cn/problems/find-the-celebrity/) | Unknown | external-index |
| 3116 | 2782 | [2782. 唯一类别的数量](https://leetcode.cn/problems/number-of-unique-categories/) | Unknown | external-index |
| 3117 | 3064 | [3064. 使用按位查询猜测数字 I](https://leetcode.cn/problems/guess-the-number-using-bitwise-questions-i/) | Unknown | external-index |
| 3118 | 3094 | [3094. 使用按位查询猜测数字 II](https://leetcode.cn/problems/guess-the-number-using-bitwise-questions-ii/) | Unknown | external-index |
| 3119 | 1428 | [1428. 至少有一个 1 的最左端列](https://leetcode.cn/problems/leftmost-column-with-at-least-a-one/) | Unknown | external-index |
| 3120 | 702 | [702. 搜索长度未知的有序数组](https://leetcode.cn/problems/search-in-a-sorted-array-of-unknown-size/) | Unknown | external-index |
| 3121 | 1533 | [1533. 找到最大整数的索引](https://leetcode.cn/problems/find-the-index-of-the-large-integer/) | Unknown | external-index |
| 3122 | 2936 | [2936. 包含相等值数字块的数量](https://leetcode.cn/problems/number-of-equal-numbers-blocks/) | Unknown | external-index |
| 3123 | 1618 | [1618. 找出适应屏幕的最大字号](https://leetcode.cn/problems/maximum-font-to-fit-a-sentence-in-a-screen/) | Unknown | external-index |
| 3124 | 1538 | [1538. 找出隐藏数组中出现次数最多的元素](https://leetcode.cn/problems/guess-the-majority-in-a-hidden-array/) | Unknown | external-index |
| 3125 | 489 | [489. 扫地机器人](https://leetcode.cn/problems/robot-room-cleaner/) | Unknown | external-index |
| 3126 | 1778 | [1778. 未知网格中的最短路径](https://leetcode.cn/problems/shortest-path-in-a-hidden-grid/) | Unknown | external-index |
| 3127 | 1810 | [1810. 隐藏网格下的最小消耗路径](https://leetcode.cn/problems/minimum-path-cost-in-a-hidden-grid/) | Unknown | external-index |
| 3128 | 1274 | [1274. 矩形内船只的数目](https://leetcode.cn/problems/number-of-ships-in-a-rectangle/) | Unknown | external-index |

## 八、其他

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 3129 | 2833 | [2833. 距离原点最远的点](https://leetcode.cn/problems/furthest-point-from-origin/) | 1294 | external-index |
| 3130 | 2740 | [2740. 找出分区值](https://leetcode.cn/problems/find-the-value-of-the-partition/) | 1302 | external-index |
| 3131 | 2946 | [2946. 循环移位后的矩阵相似检查](https://leetcode.cn/problems/matrix-similarity-after-cyclic-shifts/) | 1406 | external-index |
| 3132 | 3523 | [3523. 非递减数组的最大长度](https://leetcode.cn/problems/make-array-non-decreasing/) | 1435 | external-index |
| 3133 | 781 | [781. 森林中的兔子](https://leetcode.cn/problems/rabbits-in-forest/) | 1453 | external-index |
| 3134 | 1041 | [1041. 困于环中的机器人](https://leetcode.cn/problems/robot-bounded-in-circle/) | 1521 | external-index |
| 3135 | 3587 | [3587. 最小相邻交换至奇偶交替](https://leetcode.cn/problems/minimum-adjacent-swaps-to-alternate-parity/) | 1548 | external-index |
| 3136 | 1864 | [1864. 构成交替字符串需要的最小交换次数](https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-binary-string-alternating/) | 1601 | external-index |
| 3137 | 1899 | [1899. 合并若干三元组以形成目标三元组](https://leetcode.cn/problems/merge-triplets-to-form-target-triplet/) | 1636 | external-index |
| 3138 | 2498 | [2498. 青蛙过河 II](https://leetcode.cn/problems/frog-jump-ii/) | 1759 | external-index |
| 3139 | 134 | [134. 加油站](https://leetcode.cn/problems/gas-station/) | Unknown | external-index |
| 3140 | 3443 | [3443. K 次修改后的最大曼哈顿距离](https://leetcode.cn/problems/maximum-manhattan-distance-after-k-changes/) | 1856 | external-index |
| 3141 | 3635 | [3635. 最早完成陆地和水上游乐设施的时间 II](https://leetcode.cn/problems/earliest-finish-time-for-land-and-water-rides-ii/) | 1870 | external-index |
| 3142 | 3002 | [3002. 移除后集合的最多元素数](https://leetcode.cn/problems/maximum-size-of-a-set-after-removals/) | 1917 | external-index |
| 3143 | 1733 | [1733. 需要教语言的最少人数](https://leetcode.cn/problems/minimum-number-of-people-to-teach/) | 1984 | external-index |
| 3144 | 659 | [659. 分割数组为连续子序列](https://leetcode.cn/problems/split-array-into-consecutive-subsequences/) | 2100 | external-index |
| 3145 | 2732 | [2732. 找到矩阵中的好子集](https://leetcode.cn/problems/find-a-good-subset-of-the-matrix/) | 2240 | external-index |
| 3146 | 3474 | [3474. 字典序最小的生成字符串](https://leetcode.cn/problems/lexicographically-smallest-generated-string/) | 2605 | external-index |
| 3147 | LCP 03 | [LCP 03. 机器人大冒险](https://leetcode.cn/problems/programmable-robot/) | Unknown | external-index |
| 3148 | LCP 26 | [LCP 26. 导航装置](https://leetcode.cn/problems/hSRGyL/) | Unknown | external-index |
| 3149 | LCP 70 | [LCP 70. 沙地治理](https://leetcode.cn/problems/XxZZjK/) | Unknown | external-index |
| 3150 | 2340 | [2340. 生成有效数组的最少交换次数](https://leetcode.cn/problems/minimum-adjacent-swaps-to-make-a-valid-array/) | Unknown | external-index |
| 3151 | 3460 | [3460. 最多删除一次后的最长公共前缀](https://leetcode.cn/problems/longest-common-prefix-after-at-most-one-removal/) | Unknown | external-index |
| 3152 | 2753 | [2753. 计算一个环形街道上的房屋数量 II](https://leetcode.cn/problems/count-houses-in-a-circular-street-ii/) | Unknown | external-index |
