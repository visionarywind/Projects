# 滑动窗口与双指针

- 来源 URL：https://leetcode.cn/circle/discuss/0viNMK/
- 来源标题：分享丨【算法题单】滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）
- 抓取时间：2026-09-17 16:46:02 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 滑动窗口与双指针

如果你刚开始刷题，还不熟悉**基本编程语法**和**常用库函数**，推荐先刷力扣官方的入门题单：
- [「新」动计划 · 编程入门](https://leetcode.cn/studyplan/primers-list/)
有了一些简单题的积累，就可以开始刷我的题单啦~
下面的题目已按照难度分排序，右侧数字为难度分。
在刷题的过程中，如果遇到难度很大，题解都看不懂的题目，建议直接收藏，过段时间再来做。

### 一、定长滑动窗口 / §1.1 基础

[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)

### 二、不定长滑动窗口

不定长滑动窗口主要分为三类：求最长子数组，求最短子数组，求子数组个数。
> **注**：滑动窗口相当于在维护一个**队列**。右指针的移动可以视作**入队**，左指针的移动可以视作**出队**。

### 二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.1 越短越合法

一般要写 `ans += right - left + 1`。
内层循环结束后，$[\textit{left},\textit{right}]$ 这个子数组是满足题目要求的。由于子数组越短，越能满足题目要求，所以除了 $[\textit{left},\textit{right}]$，还有 $[\textit{left}+1,\textit{right}],[\textit{left}+2,\textit{right}],\ldots,[\textit{right},\textit{right}]$ 都是满足要求的。也就是说，当右端点**固定**在 $\textit{right}$ 时，左端点在 $\textit{left},\textit{left}+1,\textit{left}+2,\ldots,\textit{right}$ 的所有子数组都是满足要求的，这一共有 $\textit{right}-\textit{left}+1$ 个。
**思维扩展（选做）**

### 二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.2 越长越合法

一般要写 `ans += left`。
内层循环结束后，$[\textit{left},\textit{right}]$ 这个子数组是不满足题目要求的，但在退出循环之前的最后一轮循环，$[\textit{left}-1,\textit{right}]$ 是满足题目要求的。由于子数组越长，越能满足题目要求，所以除了 $[\textit{left}-1,\textit{right}]$，还有 $[\textit{left}-2,\textit{right}],[\textit{left}-3,\textit{right}],\ldots,[0,\textit{right}]$ 都是满足要求的。也就是说，当右端点**固定**在 $\textit{right}$ 时，左端点在 $0,1,2,\ldots,\textit{left}-1$ 的所有子数组都是满足要求的，这一共有 $\textit{left}$ 个。
我们关注的是 $\textit{left}-1$ 的合法性，而不是 $\textit{left}$。

### 二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.3 恰好型滑动窗口

例如，要计算有多少个元素和**恰好等于** $k$ 的子数组，可以把问题变成：
- 计算有多少个元素和 $\ge k$ 的子数组。
- 计算有多少个元素和 $> k$，也就是 $\ge k + 1$ 的子数组。
答案就是元素和 $\ge k$ 的子数组个数，减去元素和 $\ge k + 1$ 的子数组个数。这里把 $>$ 转换成 $\ge$，从而可以把滑窗逻辑封装成一个函数 $\texttt{solve}$，然后用 $\texttt{solve}(k) - \texttt{solve}(k+1)$ 计算，无需编写两份滑窗代码。
**总结**：「恰好」可以拆分成两个「至少」，也就是两个「越长越合法」的滑窗问题。
**注**：也可以把问题变成 $\le k$ 减去 $\le k-1$，即两个「至多」。可根据题目选择合适的变形方式。
**注**：也可以把两个滑动窗口合并起来，维护同一个右端点 $\textit{right}$ 和两个左端点 $\textit{left}_1$ 和 $\textit{left}_2$，我把这种写法叫做**三指针滑动窗口**。

### 二、不定长滑动窗口 / ⚠ 滑窗的内容到这里就结束了，可以去刷下一个题单。

刷题路线请看 [如何科学刷题](https://leetcode.cn/circle/discuss/RvFUtj/)。

### 三、单序列双指针 / §3.1 反转字符串

本质是相向双指针。

### 三、单序列双指针 / §3.2 相向双指针

两个指针 $\textit{left}=0,\ \textit{right}=n-1$，从数组的两端开始，向中间移动，这叫**相向双指针**。上面的滑动窗口相当于**同向双指针**。

### 三、单序列双指针 / §3.3 同向双指针

两个指针的移动方向相同（都向右，或者都向左）。

### 三、单序列双指针 / §3.4 背向双指针

两个指针从数组中的同一个位置出发，一个向左，另一个向右，背向移动。

### 六、分组循环

**适用场景**：按照题目要求，数组会被分割成若干组，每一组的判断/处理逻辑是相同的。
- 外层循环负责遍历组之前的准备工作（记录开始位置），和遍历组之后的统计工作（更新答案最大值）。
- 内层循环负责遍历组，找出这一组最远在哪结束。
这个写法的好处是，各个逻辑块分工明确，也不需要特判最后一组（易错点）。以我的经验，这个写法是所有写法中最不容易出 bug 的，推荐大家记住。

### 思考

做了一些题目后，请总结：滑动窗口和双指针的区别是什么？
欢迎在评论区发表你的做题总结。

### 关联题单

- **滑动窗口相关**：
   - [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 中的「**单调队列**」。
   - [位运算题单](https://leetcode.cn/circle/discuss/dHn9Vk/) 中的「**LogTrick**」。部分题目可以用滑动窗口+栈解决。
- **双指针相关**：
   - [贪心题单](https://leetcode.cn/circle/discuss/g6KTKL/) 中的「**单序列配对**」和「**双序列配对**」。

### 算法题单

如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 一、定长滑动窗口 / §1.1 基础 | 【套路】教你解决定长滑窗！适用于所有定长滑窗题目！ | https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/ | pending-fetch |
| 六、分组循环 | 例题讲解 | https://leetcode.cn/problems/longest-even-odd-subarray-with-threshold/solutions/2528771/jiao-ni-yi-ci-xing-ba-dai-ma-xie-dui-on-zuspx/ | pending-fetch |

## 本地原创增强解析

滑动窗口与双指针 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
