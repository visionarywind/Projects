# M04 Driver 开发指南

## 修改前

- 确认变更属于 HAL、SDK-driver 还是内核接口。
- 查 `pkg_inc`、ioctl 头文件、设备节点和目标 SoC 的条件编译。
- 列出资源所有者、锁、DMA 映射、session 和异常清理路径。
- 评估 Runtime/ACL 依赖的 ABI 和错误码影响。

## 修改 SVM/cache 前

1. 先确认目标产品编译的是 V2 还是 V3，不能只看目录名或运行时日志猜测。
2. V2 变更同时检查 heap list、VA/size/mapped 多棵树、`total/size` 不变量和释放失败回滚。
3. V3 变更同时检查 cache flag、`ga_range/ga_area` 双索引、range 完整 idle 判断和 recycle segment。
4. 普通 `rtMalloc` cache 与 SOMA pool 分开评审；size/align/flag/NUMA 改动要同步 Runtime HAL ABI。
5. 修改底层 free、ioctl 或结构体时，保留原始错误、设备、VA、size、flag 和回滚日志。


- 保持用户态/内核态结构体布局和命令号兼容。
- 所有用户指针、长度、数量和设备 ID 先校验；避免整数溢出。
- 在 HDC/queue 生命周期中使用既有锁和状态检查，避免 active session 下释放。
- 不把平台特定代码无条件扩展到其他 SoC。

## 修改后

先运行静态检查和无硬件 UT，再在匹配 Driver/Firmware/NPU 上执行设备测试；记录内核日志、设备 ID、命令和错误码。任何未执行项应明确标注。
