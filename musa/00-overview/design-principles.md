# 设计原则

- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 证据状态：已确认与推断分开；未执行构建和硬件运行

## 结论

源码体现出五个稳定原则：C ABI 与 C++ 对象解耦、Context 作为资源归属边界、命令异步化、HAL 隔离硬件差异、旧 ABI 通过包装保持兼容。前四项由源码结构和调用点确认；“设计原则”这一表述是基于实现的归纳。

## 原则清单

1. **入口薄、核心厚**：`muapi*` 负责参数/句柄检查和结果转换，实际资源操作下沉到 `Platform`、`Context`、`Memory`、`Stream`。[src/driver/mu_memory.cpp:271-320] [src/musa/core/context.cpp:1037-1099]
2. **显式资源归属**：Context 的 `CriticalBase` 保存 stream、event、graph、memory、module 等集合，并在销毁时释放客户端资源。[src/musa/core/context.cpp:41-475]
3. **异步统一为 Command**：Kernel、copy、memset、graph launch 都经由 `Stream::Cmd*` 创建/排队，提交线程再调用 `Command::Submit`。[src/musa/core/context.cpp:625-673] [src/musa/core/stream.cpp:1048-1220]
4. **硬件差异由 HAL 吸收**：core 只依赖 HAL 接口，`halM3d` 再链接 `m3d/scpc/util`。[src/musa/core/CMakeLists.txt:1-11] [src/hal/m3d/CMakeLists.txt:36-59]
5. **兼容性优先**：v1 API 多以 v2 API 的窄包装实现，导出表按领域分组。[src/driver/mu_context.cpp:184-221] [src/driver/mu_entry.cpp:1323-1660]

## 需要谨慎解释的地方

- 线程安全不是所有对象都由同一把锁保护；已看到 `ReadLockedAccessor`、`WriteLockedAccessor`、提交锁和 per-thread stream，具体锁序需要逐函数审计。[src/musa/core/context.cpp:552-613] [src/musa/core/stream.cpp:1048-1100]
- HAL 到内核驱动的最终副作用仍依赖私有 M3D 子模块，当前文档不把“调用成功”当作硬件验证。
