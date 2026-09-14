# M01 执行流程

## 成功

```text
公开 API → 参数/输出检查 → TLS/context 检查 → CUI 对象操作 → 错误原样返回
```

## 失败

- 非法 flags/空输出/零长度：在 wrapper 返回，不创建资源。
- 初始化或 current context 不满足：`cuiInitCheckEx` 返回错误。
- global registration 失败：释放已创建 memobj 并解锁 context。
- launch 的 function/stream 属于不同 context：返回 invalid handle。

## 清理

wrapper 只清理本次局部创建且尚未转移所有权的对象；已注册 memobj、stream 或 graph 节点必须走对应 CUI 生命周期。证据：[src/api/apimem.c:52-118,231-339]、[src/api/apilaunch.c:223-301]。
