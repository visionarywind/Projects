# M03 行级审计

- 文档目的：修改 wait/signal 协议时的检查清单。
- 证据状态：当前 HEAD 静态整理。
- 最后更新：2026-09-10
- 前置阅读：[M03 README](README.md)
- 后续阅读：[跨模块错误边界](../../90-cross-module/error-boundaries.md)

| 检查点 | 证据 | 失败后果 |
|---|---|---|
| 自旋只消耗有限次数 | `lightweightsemaphore.h:290-323` | CPU 忙等或延迟变化 |
| count 与平台等待配对 | `290-360` | permit 丢失/虚假唤醒处理错误 |
| signal 使用 release | `421-430` | 数据可见性协议被削弱 |
| bulk wait 恢复数量 | `326-360` | batch 计数错误 |
| timeout 使用正确时钟/宏 | 平台实现分支 | 超时语义漂移 |
| 析构无 waiter | blocking queue 约束 | 访问已释放 semaphore |

## 验证

需要至少覆盖：立即成功、阻塞后 signal、timeout、bulk、重复 signal、消费者停止时 shutdown。未执行前均标为未验证。
