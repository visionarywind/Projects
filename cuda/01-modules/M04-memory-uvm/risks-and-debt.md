# M04 风险与技术债

| 项目 | 现状 | 影响 |
|---|---|---|
| 多层对象所有权 | `memobj`、`memblock`、manager 可分离 | 释放顺序和共享引用复杂 |
| 子分配策略 | 由 flags、size、HAL block size 和环境变量共同决定 | 同一请求可能走不同 backing 路径 |
| UVM 末端 | 依赖 `cuiuvm`、DAG 和外部内核接口 | 当前无法证明 fault 到 migration 的完整时序 |
| P2P/共享对象 | source/loopback/proxy 分支存在 | pointer lookup 和 unregister 容易取错对象 |
| 动态 UVM AL | `uvmInitAL` 按平台/全局选项选择 zero-copy、Mac、UVM8 或 KdLite | 仅读公共入口不能推断 fault/migration 和 channel 行为 |
| submemblock 特殊映射 | Kd8 处理 host-page split、non-managed heap 与 SKED/reflected memory | 页粒度、重叠检测和动态并行映射的组合分支难以仅靠单元测试覆盖 |
| P2P 引用与真实映射分离 | `CUpeerMap` 只维护 access refcount | bookkeeping 成功不等于 RM/设备 VA map 成功 |

**深度状态**：M04 已完成 API 分配/free、对象属性、子分配决策、UVM AL 选择、managed registration rollback、DAG/stream 状态和 P2P bookkeeping 的静态主线；待完成 fault/migration backend、IPC/external handle 端到端路径和各 DMAL memory backend。

