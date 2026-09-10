# 跨模块：变更影响地图

- 证据状态：影响关系基于构建和调用边界；需按具体符号复核

| 变更 | 直接模块 | 必查模块/风险 |
|---|---|---|
| GE Graph/shape/模型格式 | GE | ACL 模型 API、Runtime Execute、算子包、Driver 设备能力 |
| GE Executor stream/event 资源 | GE | Runtime 生命周期、Driver queue/esched、异步测试 |
| ACL 公共 API/错误码 | ACL | Runtime 导出表、GE 调用者、应用 ABI、文档/测试 |
| Runtime C API/handle | Runtime | ACL 包装、GE Executor、Driver HAL、所有客户端 |
| KernelMemoryPool 策略/池大小 | Runtime | Kernel/Program load/free、Driver memory、碎片与资源限制测试 |
| SOMA Segment/重用/trim | Runtime | ACL stream-ordered API、AICPU `SomaMemMng`、HAL/SVM、异步失败回滚和统计测试 |
| 普通 memory flags/policy | Runtime | NpuDriver、HAL/SVM、SoC 产品分支、P2P/huge/Host 回归 |
| Runtime scheduler/memory | Runtime | Stream/Event、DFX、Driver queue、性能/并发回归 |
| HAL 函数/结构体 | Driver | Runtime 链接、SDK-driver、内核 ioctl、所有产品分支 |
| SOMA ioctl/VMM 协议 | Driver | Runtime async config、SVM V2/V3、AICPU/固件、free 失败恢复 |
| esched ioctl ABI | Driver | 用户态 SDK、内核模块、固件/设备调度 |
| SoC 条件编译/设备信息 | Driver/Runtime | ACL 平台缓存、GE 编译目标、产品测试矩阵 |

## 资源变更评估顺序

1. 先确认 API/wrapper、Runtime 内部对象和真正产生副作用的实现，而不是只改公共声明。
2. 对内存变更区分 KernelMemoryPool、SOMA 和普通 device/Host memory，分别检查 ownership、锁、异步完成和失败回滚。
3. 检查 CMake target、安装库、导出表、HAL/SDK-driver ABI、AICPU 参数和动态依赖。
4. 更新错误映射、生命周期文档、DFX 字段和测试矩阵。
5. 分别运行无设备 UT、跨仓集成和真实设备回归；未运行或缺设备时只记录静态证据和未验证边界。
6. 更新 `analysis-state.md` 及证据索引。
