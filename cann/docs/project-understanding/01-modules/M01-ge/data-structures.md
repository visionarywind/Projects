# M01 GE 数据结构与生命周期

- 对应源码版本：`47020afc8`
- 证据状态：核心概念已确认；部分内部布局待补

## 主要对象

| 对象 | 职责 | 生命周期/所有权 |
|---|---|---|
| `SessionManager` | 管理 GE 全局 Session | GE 初始化创建，Finalize 清理 |
| `InnerSession` | 保存会话状态并处理图 API | `GeSession::Impl` 通过 `shared_ptr` 持有 `[ge/api/session/session/ge_session_impl.cc:34-71]` |
| `Graph`/`Node` | 图和算子语义 | AddGraph 后由 Session/图对象管理 |
| `Tensor`/`Attribute` | 数据和算子元信息 | Graph/Model 或调用方按 API 约定持有 |
| `DataAnchor`/`CtrlAnchor` | 数据边和控制边 | 随图结构建立和销毁 |
| V2 Executor | 模型加载、执行和卸载 | Init → Loaded → Init `[ge/runtime/v2/core/model_v2_executor.cc:201-257]` |
| Stream/Event/Notify allocator | 执行资源 | 外部注入或 Executor 管理；Execute 返回时解除绑定 |

## 图与执行对象

```text
Session
 └─ Graph
     ├─ Node
     │   ├─ input/output DataAnchor
     │   └─ CtrlAnchor
     └─ Tensor/Attribute metadata
          ↓ compile
      OM / ExecuteGraph
          ↓ load
      Executor + Runtime resources
```

GE 架构文档确认 Graph、Node、Tensor 和 Anchor 的语义 `[ge/docs/zh/design/architecture.md:76-93]`。Memory Planner、EnginePartitioner 和完整模型内部布局仍需继续追踪。

## 关键不变量

- Graph 编译前必须是合法且完整的图。
- Executor 只有 Loaded 状态可执行。
- Execute 期间输入/输出地址、Stream、Event 和 allocator 必须有效。
- UnLoad 必须对称释放主图、初始化/反初始化图及默认资源。
