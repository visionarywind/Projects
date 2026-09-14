# M01 数据结构与行级分析

## 数据结构

- `CUdeviceptr_v1/v2`：公开 ABI 宽度差异；内部统一使用 `CUdeviceptr_v2`（静态确认：[src/cuda_types.h:29-34]）。
- `CUctx`、`CUIstream`、`CUfunc`：wrapper 通过 TLS/句柄转换获得，不能把公开 handle 直接当作任意对象指针使用。
- `CUmemdesc`：内存分配的属性载体；`type/owner/apiSource/location/mapDevice` 决定后续 memobj 行为（静态确认：[src/api/apimem.c:90-98]）。
- `CUIlaunchData`：block/grid/shared memory 与参数描述的 HAL 无关载体（静态确认：[src/api/apilaunch.c:135-179]）。

## 行级关键点

1. `[src/api/apimem.c:63-65]` 先清零输出指针，保证错误返回不暴露未初始化地址。
2. `[src/api/apimem.c:73-87]` 先做全局/context/capture 检查，再检查空指针和零大小。
3. `[src/api/apilaunch.c:239-247]` 强制 function 和 stream 来自同一 context，避免跨 context 资源提交。
4. `[src/api/apilaunch.c:252-286]` capture 分支在 graph 锁下创建节点；失败会 invalidate capture。
5. `[src/api/apistream.c:94-109]` context lock 释放后再做 QMD pool 登记，失败转入 stream detach。

## 并发边界

API wrapper 的 context lock 保护 context-owned 状态；stream pool 和 graph capture 有独立锁。具体锁顺序不能从函数名推断，应沿调用点验证（静态确认：[src/api/apistream.c:83-99]；更完整规则未知）。
