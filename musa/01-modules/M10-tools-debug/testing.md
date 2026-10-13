# 工具与调试接口：测试策略

- 文档目的：tools debug：测试入口、缺口与建议用例（含未验证标注）。
- 适用范围：M10 模块的《工具与调试接口：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态检查

- 核对 `muInfo` 用到的 `muDeviceGetAttribute` 枚举值都存在于 `musa.h` 公共头文件；属性缓存 `GetDeviceProps` 中任何枚举缺失都会在首次调用时经 `checkMuErrors` 直接退出 [src/tools/muInfo.cpp:119-274]。
- 核对导出表分发：`Client` 枚举（9 值）与 switch 分支一一对应；未知 id 走默认回退 MUPTI 表 [src/driver/mu_entry.cpp:2164-2304]。
- 核对新增 accessor 前 `FillAccessors` 的哨兵契约：目标槽必须预置 `AccessorHint`，源列表不得有 0 空洞 [src/driver/mu_entry.cpp:2146-2155]。
- 核对 hook 存储：tracepoint 入口的 `ready`/`AccessorHint` 双检查 [src/driver/mupti/tracepoints.h:13-21] 与 `EnableMUptiDriver`/`DisableMUptiDriver` 的 release/acquire 配对 [src/driver/mupti/hooks.cpp:9-16]。
- 核对资源成对：GDB 侧 FIFO 创建/删除、`fork`/`waitpid`、shader/exchange 内存分配/释放 [src/gdb/musaGdb.cpp:30-56]、[src/driver/mugdb/tracepoints.cpp:85-103]。
- 运行 `git diff --check` 只验证本文档仓库的格式，不验证远端项目。

## 运行验证（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
# 专项：
./build/bin/muInfo
```

目标环境需要私有子模块、工具链、驱动和 GPU；本知识库没有声称上述命令成功。

## 集成测试要点（需目标机）

- `muInfo`：多设备环境下验证每个设备的 primary ctx retain/set current 与 peer 矩阵；单错误即退出意味着一次只能暴露一个失败。
- GDB：`MUSA_DEVICE_WAITS_ON_EXCEPTION=1` 控制 trap 时是否等待 attach；`Monitor` 收到 `BPH_COMM_STATE_TRAP_ENTER` 后打印 kernel 信息并 `raise(SIGINT)` [src/driver/mugdb/tracepoints.cpp:129-171]。
- MUPTI/MUASAN：命令执行路径上的 tracepoint 是否在 `ready` 置位后如期回调（memcpy [src/musa/core/command/memcpyCommand.cpp:58]、dispatch [src/musa/core/command/dispatchCommand.cpp:321]）。

模块专项用例见 `80-demos`（如 D01 `muInfo`）和远端 `tests` 文件。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（2 处，例：[src/driver/mu_entry.cpp:2164-2304]）
- `src/driver/mugdb/tracepoints.cpp`（2 处，例：[src/driver/mugdb/tracepoints.cpp:85-103]）
- `src/tools/muInfo.cpp`（1 处，例：[src/tools/muInfo.cpp:119-274]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
