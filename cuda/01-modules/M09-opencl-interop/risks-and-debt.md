# M09 风险与技术债

| 风险 | 静态依据 | 后果 |
|---|---|---|
| dispatch table 漂移 | 256-entry 顺序填充 `[src/cl/cliicd.c:6-14,32-109]` | ABI 入口错位 |
| 平台选择歧义 | properties/device type/first vendor fallback `[src/icd_rel/icd_dispatch.c:217-247]` | context 落到错误 vendor |
| 动态库生命周期 | vendor add 临时 unload 与节点保存 `[src/icd_rel/icd.c:70-208]` | function pointer/platform 失效 |
| feature macro 差异 | OpenCL 1.1/1.2/2.0 条件 `[src/cl/cliicd.c:128-235]` | 入口在不同 binary 不同 |
| interop 链未闭合 | GL/D3D 文件存在但转换实现未全读 | 无法确认 sync/ownership |

**深度等级：初步到中等。** ICD 和 vendor globals 已覆盖；OpenCL enqueue 到 CUI stream、graphics external memory 和跨 API 销毁仍待源码级闭合。
