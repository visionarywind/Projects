# M09 测试与验证

- ICD loader 的 vendor 缺失、非法 `(num_entries, platforms, num_platforms)` 组合和 platform handle 校验由 dispatch 源码覆盖（静态确认：[src/icd_rel/icd_dispatch.c:43-108]）。
- dispatch table 的固定 256-entry 顺序及平台条件是 ABI 静态检查重点（静态确认：[src/cl/cliicd.c:6-14,24-144,146-235]）。
- OpenCL context/device/queue/memory、GL/D3D interop 的真实 vendor dispatch 需要运行时库和图形设备；本项目未执行。

“源码存在入口”不等于 vendor 能加载或对象操作成功，当前没有已验证的 OpenCL 结果。
