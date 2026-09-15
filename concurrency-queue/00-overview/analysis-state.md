# 分析状态

- 文档目的：支持后续分批续作，记录已完成、未完成和证据置信度。
- 最后更新：2026-09-15
- 目标版本：`master` / `683b9e31ea15eb69f1b81cc1defc7850d5f20b71` (`v1.0.5-6-g683b9e3`)

## 批次状态

| 批次 | 内容 | 状态 |
|---|---|---|
| 0 | 版本、仓库盘点、规则检查 | 已完成静态确认 |
| 1 | 总览层和证据索引 | 已完成第一版 |
| 2 | M01 核心队列实现层 | 已完成主调用链；审计表待持续细化 |
| 3 | M02/M03 阻塞与信号量 | 已完成静态主链 |
| 4 | M04 C API | 已完成基本 ABI 链路；边界行为待验证 |
| 5 | M05–M07 | 已完成入口/职责盘点；native unit 构建和运行已验证，CMake/RISC-V 等待补充 |
| 6 | D01/D02 | D01 native smoke 已验证；D02 benchmark 未执行 |
| 7 | 跨模块和实践层 | 第一版完成；可继续添加调试案例 |
| 8 | 验收 | Markdown 相对链接检查已通过；行号和深度审计仍需持续维护 |
| 9 | 池化与资源管理专题 | 已完成 block pool/free list/producer 复用/析构闭环静态复核 |

## 当前已确认

- 核心队列为 per-producer block 子队列加 producer list。
- enqueue 在元素构造完成后 release 发布 tail；dequeue 通过 head/optimistic 协议领取并显式析构。
- blocking wrapper 以成功入队 signal、等待后调用核心 dequeue。
- CMake 为 interface/export；legacy Make 构建测试和 benchmark。
- README 明确不保证跨 producer 全局顺序、linearizability 或 sequential consistency。
- 已确认 block 请求顺序为 initial pool → global free list → 可选动态分配；`RECYCLE_ALLOCATED_BLOCKS` 决定 implicit 动态空块是立即释放还是留池复用。
- 已确认 token 销毁只把 explicit producer 标记为 inactive；producer、索引和其 block 的最终释放边界是 queue 析构。

## 已执行验证

- `make -C build tests`：成功，生成 native `build/bin/unittests` 和 `build/bin/fuzztests`。
- `./build/bin/unittests --help`：成功显示选项。
- `./build/bin/unittests --disable-prompt --iterations 1`：成功，输出中的全部测试通过。
- `./build/bin/unittests --disable-prompt --run test_threaded --iterations 1`：成功，选定测试通过。
- CMake configure/build/install：本轮未执行，工具调用权限被拒绝；不能据此判断 CMake 失败或成功。

## 未验证命令

fuzz executable、benchmark、CMake configure/build/install、RISC-V/QEMU 均未完成验证。`make tests` 已验证构建成功，但不等于 fuzz executable 已运行。不要把 CI 历史成功当作当前工作区运行结果。

## 未解决问题

5. CMake/RISC-V/QEMU、fuzz 和 benchmark 的实际环境验证。
6. C API 空 handle、空输出指针、异常穿过 C 边界和 value 所有权。
7. 不同平台 TLS/thread-exit notifier 的实际行为。
8. 所有目标架构上的 lock-free 属性和 semaphore 分支。
9. 完整 benchmark 输出及稳定性能边界。
10. 证据行号和文档覆盖率的持续自动验收。
11. 不同 `BLOCK_SIZE`、预分配容量和 recycle 策略的内存—吞吐曲线尚未实测。

## 本轮补充

- 新增 [池化与资源管理专题](../90-cross-module/pooling-and-resource-management.md)，集中说明 block、producer、索引、allocator 和 semaphore 的所有权与释放边界，并明确核心项目无 Graph runtime。
- 源码 HEAD 与原分析版本一致；本轮未修改 `source/concurrency-queue`。

## 续作协议

每次续作：先确认 HEAD 是否变化；若变化，重新校验 `evidence-index.md` 行号；再完成一个模块或 Demo；最后只把真实执行的命令写入验证表，并更新本页日期和状态。
