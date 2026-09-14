# M09 风险与技术债

| 风险 | 静态依据 | 后果 |
|---|---|---|
| dispatch table 漂移 | 256-entry 顺序填充 `[src/cl/cliicd.c:6-14,32-109]` | ABI 入口错位 |
| 平台选择歧义 | properties/device type/first vendor fallback `[src/icd_rel/icd_dispatch.c:217-247]` | context 落到错误 vendor |
| 动态库生命周期 | vendor add 临时 unload 与节点保存 `[src/icd_rel/icd.c:70-208]` | function pointer/platform 失效 |
| feature macro 差异 | OpenCL 1.1/1.2/2.0 条件 `[src/cl/cliicd.c:128-235]` | 入口在不同 binary 不同 |
| interop 链未闭合 | GL/D3D 文件存在但转换实现未全读 | 无法确认 sync/ownership |
| 多重引用计数 | `CLIobjectData` public/internal/tree `[src/cl/cliobject.h:98-169]` | public release 可能早于 CUI/GPU 完成或反之 |
| context 销毁顺序敏感 | thread/task graph/pinned memory/CUctx 顺序 `[src/cl/clicontext.c:467-545]` | 后台 worker 或异步 memcpy 使用已释放状态 |
| event fast path 旁路普通 wait | marker completed 分支手动 clear pinned tracking `[src/cl/clevent.c:182-213]` | 新增等待路径若漏清理会留下 pinned memory |
| external memobj owner 不明 | DMAL open 与 `memobjAlloc` `[src/cui/cuiextinterop.c:21-171]` | handle close、device pointer 和 backing 释放时机不清 |

**深度等级：中→深（host-side）。** ICD/vendor、public/internal object tree、context destroy、event marker aggregation、GL registration 和 external memobj creation 已覆盖；OpenCL enqueue 到 worker/CUI 的所有任务类型、D3D/GL fence 的 RM 语义、外部 handle close 和实际运行仍未闭合。
