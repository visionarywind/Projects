# M02 测试与验证

## 静态覆盖

- `basic_sanity.cu` 通过初始化、设备枚举和 context 操作覆盖 M02 的 globals/TLS/context 主线（静态确认：[tests/cuda_test/basic_sanity.cu:49-100,181-195]）。
- `dispatch_main.cpp` 在测试入口初始化 testutils，并在 `--listGpus` 模式下只展示设备、不执行测试（静态确认：[tests/cuda_test/dispatch_main.cpp:22-50]）。
- context 创建 wrapper 覆盖非法 device、NULL 输出、CUI 创建失败和 TLS push 失败后的 context 销毁（静态确认：[src/api/apictx.c:38-95]）。

## 应覆盖的错误场景

| 场景 | 预期观察 |
|---|---|
| 未初始化时调用需要 context 的 API | `cuiInitCheckEx` 返回初始化错误 |
| TLS 没有 current context | context 检查失败或按 API 契约处理 |
| context sticky error | 后续检查观察并返回 sticky error |
| context 创建后 TLS push 失败 | 新 context 被销毁，不留下半绑定状态 |
| 初始化中途 manager 创建失败 | 按反向顺序释放已初始化 manager |

## 验证状态

本工作区未运行 nvmake、`cuda_test`、GPU、debugger 或任何外部 driver 命令；上述内容是源码静态覆盖，不是执行结果。
