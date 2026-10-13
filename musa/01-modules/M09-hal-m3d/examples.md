# HAL/M3D 适配：例子

- 文档目的：hal m3d：用法示例与边界/错误输入说明。
- 适用范围：M09 模块的《HAL/M3D 适配：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


本页是**源码阅读例子**，不是已运行的程序；每个步骤都给出可核对的行号。

## 例 1：从零建立平台

```text
Hal::CreatePlatform(createInfo, &pPlatform)      src/hal/m3d/lib.cpp:6-13
  new M3d::Platform()
  Platform::Init(createInfo)                     src/hal/m3d/platform.cpp:102-114
    CreatePlatform -> IM3d::CreatePlatform       src/hal/m3d/platform.cpp:116-126
    CreateDevices                                src/hal/m3d/platform.cpp:128-195
      EnumerateDevices -> 每个 device new+Init   src/hal/m3d/platform.cpp:131-184
      CreateDevicesP2pTopoMatrix                 src/hal/m3d/platform.cpp:193-194
  成功 -> *ppPlatform，失败 -> delete platform    src/hal/m3d/lib.cpp:9-12
```

阅读要点：`Init` 内部把 `CreatePlatform` 的失败直接 `printf` 后返回，`CreateDevices` 的设备数为 0 会被判成 `errorNoDevice` [src/hal/m3d/platform.cpp:128-135]。

## 例 2：一次普通分配的落点

```text
Core: Hal().GetMemMgr()->Allocate(allocInfo, &offset, &pHalMemory)
  -> MemMgr::Allocate                            src/hal/m3d/memMgr.cpp:81-147
     校验 size / 溢出 / totalGlobalMem           src/hal/m3d/memMgr.cpp:85-96
     m_PoolRefs.Get(MakeKey(poolInfo))           src/hal/m3d/memMgr.cpp:116
     未命中 -> CreatePoolNoLock                  src/hal/m3d/memMgr.cpp:132
     pPool->FullAllocate                         src/hal/m3d/memMgr.cpp:144
```

同样的 `Allocate` 在 Core 侧有多个调用点，例如设备内存创建路径就直接取 `GetMemMgr()` [src/musa/core/device.cpp:1257-1265]。阅读要点：`ppMemoryPool` 为空时选池完全由五字段键决定，键不一致会新建池而不是报错 [src/hal/m3d/memMgr.cpp:107-133]。

## 例 3：录一条命令并提交

```text
Device::CreateCmdPool / CreateCmdBuffer         src/hal/m3d/device.cpp:94-114
CmdBuffer::Begin  Initial->Recording             src/hal/m3d/cmdBuffer.cpp:145-197
CmdBuffer::Cmd*   写入 m_M3dCmdBuffer
CmdBuffer::End    Recording->Executable          src/hal/m3d/cmdBuffer.cpp:199-211
Queue::Submit     转 IM3d::IQueue->Submit        src/hal/m3d/queue.cpp:182-357
```

阅读要点：`Begin` 会顺带 `CmdBindBorderColorPalette`，compute 与 universal 两类 engine 都会走到这一步 [src/hal/m3d/cmdBuffer.cpp:188-195]；`Queue::Submit` 对 user queue 一律拒绝带 fence 的提交 [src/hal/m3d/queue.cpp:182-188]。

## 例 4：核对一处"看起来安全"的查找

按 [`risks-and-debt.md`](risks-and-debt.md) 的记录，`MemMgr::GetPool`/`FindPool` 都对返回值直接 `->m_Value`，而 `SplayTree` 的 `Find` 对缺失键返回空指针 [src/hal/m3d/memMgr.cpp:36-44]。这个例子说明本模块的用法：**读到"直接解引用查找结果"的写法时，先回到 `src/util/utilSplayTree.h` 确认返回契约，再判断是否可信**。

## 不是什么

- 以上流程没有在本环境编译或运行过，行号只证明代码形态如此。
- `src/hal/m3d/m3d` 内部如何响应 `CreatePlatform`、`Submit`，本模块未读取。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（2 处，例：[src/hal/m3d/memMgr.cpp:107-133]）
- `src/hal/m3d/platform.cpp`（1 处，例：[src/hal/m3d/platform.cpp:128-135]）
- `src/musa/core/device.cpp`（1 处，例：[src/musa/core/device.cpp:1257-1265]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
