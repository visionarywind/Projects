# Stream 与 Command 调度：调用链

- 模块：M06 Stream / Command
- 证据状态：API 进入 Context/Stream、Command 入队、异步提交线程到 HAL queue submit 的骨架已静态确认；每类 Command 的 Build/Submit 细节仍需分文件展开。

## CHAIN-M06-COPY-ASYNC：`muMemcpyAsync` 到 copy command 入队

```text
muMemcpyAsync(dst, src, bytes, hStream)
  -> muapiMemcpyAsync                                      [src/driver/mu_memory.cpp:1016-1023]
     ├─ InitPlatform()
     ├─ GetMemcpy3DFrom1D(..., memcpy_default)
     └─ Context::GeneralMemcpy(TlsCtxTop(), hStream, copy3D, memBlocking)
        -> Context::GeneralMemcpy                         [src/musa/core/context.cpp:696-728]
           ├─ InfoStream(ctx, hStream)
           ├─ Context::CreateMemcpyNode(...)
           │  -> GraphMemcpyNode::Init/Update/CopyManagerSelect（待详解）
           │     [src/musa/core/context.cpp:2116-2189;
           │      src/musa/core/node/graphMemcpyNode.cpp:58-93,450-537]
           ├─ capture active -> Stream::CaptureNode
           └─ 非 capture -> Stream::CmdCopyMemory
              -> 创建 AsyncMemcpyCommand 或 SyncMemcpyCommand
              -> Context::ResolveDependencyAndQueueCommand
                 [src/musa/core/stream.cpp:696-727]
```

## CHAIN-M06-QUEUE：依赖解析和入队

```text
Context::ResolveDependencyAndQueueCommand(command, stream, blocking)
  [src/musa/core/context.cpp:1984-2045]
  ├─ 若默认流：依赖所有 blocking stream 的 LastCommand
  ├─ 若 barrier stream：依赖其它 stream
  ├─ 若普通 blocking stream：依赖默认流 LastCommand
  ├─ 合并 stream 当前依赖
  ├─ stream->QueueCommand(command)
  └─ blocking 或 pfmEnabled 时 commandRef->Wait()

Stream::QueueCommand(command)
  [src/musa/core/stream.cpp:1048-1111]
  ├─ 检查 asyncCapacity 和 sticky error
  ├─ command->ChoosePerfEngine(m_LastCommand)
  ├─ command->SetPrevCommand(m_LastCommand)
  ├─ m_LastCommand = command
  ├─ command 状态 created -> queued
  ├─ m_CommandList.push_back(command)
  └─ m_SubmitCv.notify_one()
```

## CHAIN-M06-ASYNC-SUBMIT：提交线程

```text
Stream::AsyncSubmit()
  [src/musa/core/stream.cpp:1154-1225]
  ├─ EngineSubmissionSchedule(engine)
  ├─ 为可 merge command 分配 submission id / MUpti 关联
  ├─ status = m_MergingList.front()->GetLastError()
  └─ status == SUCCESS -> m_MergingList.front()->Submit()
```

`Command` 基类负责依赖构建和等待：`Command::Build` 将依赖转换为 HAL semaphore wait；`Command::Wait` 通过 schedule 等待状态进入 completed/error。[src/musa/core/command/command.cpp:176-247]

## CHAIN-M06-HAL-QUEUE：HAL/M3D submit 边界

```text
Command 子类 Submit()
  -> SubmitToQueue(Hal::IQueue*, QueueSubmitInfo)
  -> Hal::M3d::Queue::Submit                               [src/hal/m3d/queue.cpp:178-355]
     ├─ 转换 wait/signal semaphore 到 IM3d queue semaphore
     ├─ 转换 Hal::ICmdBuffer* 到 IM3d::ICmdBuffer*
     ├─ 组装 IM3d::PerSubQueueSubmitInfo / MultiSubmitInfo
     ├─ 设置 serial/freeSchedule/exception settings
     └─ m_M3dQueue->Submit(m3dSubmitInfo)                  [src/hal/m3d/queue.cpp:348]
```

## 缺口

- `AsyncMemcpyCommand::Build/Submit`、`SyncMemcpyCommand::Build/Submit` 未在本页展开。
- `EngineSubmissionSchedule`、merge list 构建、command 状态 completed/error 设置路径需要继续深挖。
- M3D `IQueue::Submit` 之后到内核驱动/firmware 的路径尚未验证。
