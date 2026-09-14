# M09 行级分析

- `[src/icd_rel/icd.c:43-51]`：全局 vendor list 和一次性初始化入口。
- `[src/icd_rel/icd.c:63-208]`：动态库、extension、platform IDs、suffix、vendor node 和临时资源清理。
- `[src/icd_rel/icd_dispatch.c:43-90]`：platform IDs 参数校验、输出清零和 vendor 枚举。
- `[src/cl/cliglobals.c:58-118]`：CUI 初始化、配置、dispatch/platform 创建和失败清零。
- `[src/cl/cliobject.h:98-169]`：public object 首字段、ICD dispatch、public/internal refcount、parent/children 树和 destroy contract。
- `[src/cl/clcommandqueue.c:15-76,189-215]`：queue 以 context 为 parent 创建，CUI queue 失败回滚及 public retain/release。
- `[src/cl/clicontext.c:467-545]`：worker/callback thread、task graph、pinned memory、CUI context 和 GL TLS 的销毁顺序。
- `[src/cl/clevent.c:106-217]`：event submitted 等待、按 device marker 聚合、flush/wait 和 pinned memcpy 清理。
- `[src/cl/clgl.c:93-204]`：GL mem object 建立、resource register、image/size 查询和错误 unregister。
- `[src/cui/cuiextinterop.c:21-171]`：external handle open、semaphore/buffer memobj backing 及错误 free。

这些证据闭合了 host-side 对象和等待路径；图形驱动 fence、DMAL close、worker 实际消费和 GPU interop 完成语义仍未验证。
