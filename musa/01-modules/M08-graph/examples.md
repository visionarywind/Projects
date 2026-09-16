# Graph 执行：例子

## 静态最小例子

```text
入口调用
  1. 准备参数/句柄
  2. 通过 current context 或 Platform 找对象
  3. 调用核心方法
  4. 检查 MUresult
  5. 在生命周期结束前同步并释放
```

这是源码阅读模型，不是已运行的示例。可用 D01 `muInfo` 验证查询类路径，用 D02/D04 验证异步/Graph 路径。
