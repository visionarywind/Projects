# M07 CI 矩阵

- 文档目的：记录 CI 已声明的环境，而不是把它扩展成未存在的发布流程。
- 证据状态：workflow 静态确认；当前工作站未执行。
- 最后更新：2026-09-10
- 前置阅读：[M07 README](README.md)
- 后续阅读：[构建总览](../../00-overview/build-and-deploy.md)

| 维度 | CI 路径 | 备注 |
|---|---|---|
| native | GNU build + `./bin/unittests --disable-prompt --iterations 1` | 单元 smoke |
| RISC-V | cross compile + `qemu-riscv64-static -L /usr/riscv64-linux-gnu` | 同类 unit smoke |
| fuzz | 未确认 CI 直接运行 | 不能据此声称覆盖 |
| benchmark | 未确认 CI 直接运行 | 需手动构建/运行 |
| CMake install | 配置文件存在；CI 是否运行需 workflow 证据 | 未扩大结论 |

CI 通过只说明该 workflow 中的步骤通过，不说明所有平台、场景或性能目标均通过。
