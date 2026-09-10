# 分析状态

- 文档目的：支持后续分批续作，记录已完成、未完成和证据置信度。
- 最后更新：2026-09-10
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

## 当前已确认

- 核心队列为 per-producer block 子队列加 producer list。
- enqueue 在元素构造完成后 release 发布 tail；dequeue 通过 head/optimistic 协议领取并显式析构。
- blocking wrapper 以成功入队 signal、等待后调用核心 dequeue。
- CMake 为 interface/export；legacy Make 构建测试和 benchmark。
- README 明确不保证跨 producer 全局顺序、linearizability 或 sequential consistency。

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

## 续作协议

每次续作：先确认 HEAD 是否变化；若变化，重新校验 `evidence-index.md` 行号；再完成一个模块或 Demo；最后只把真实执行的命令写入验证表，并更新本页日期和状态。
