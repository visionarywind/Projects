# 开发实践：功能开发配方

## 新增 ACL/Runtime API

1. 定义公共 C ABI、前置状态、所有权和错误码。
2. 增加 ACL wrapper（若属于 ACL）和 Runtime C API/内部实现。
3. 更新导出表、Hook、CMake target、安装头文件和依赖。
4. 增加参数/句柄/错误映射/异步生命周期 UT。
5. 评估 GE、Driver 和应用兼容性。

## 修改 GE 执行行为

1. 确认 V1/V2 状态和模型格式影响。
2. 检查输入输出 shape、地址复用、Stream/Event/allocator。
3. 修改 Load/Execute/UnLoad 对称清理路径。
4. 增加同步、异步、失败和重复调用回归。

## 修改 Driver/HAL

1. 确认 HAL、SDK-driver、内核 ioctl 和产品宏边界。
2. 同步用户态/内核态结构布局和命令 ABI。
3. 审查用户指针、长度、DMA、锁和 session 生命周期。
4. 更新 Runtime 调用者、错误映射和设备矩阵。

详细影响见 `90-cross-module/change-impact-map.md`。
