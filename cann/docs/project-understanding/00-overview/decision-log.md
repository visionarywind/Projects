# 总览：决策记录

## D-001：按四仓库划分模块

- 决策：将 GE、ACL、Runtime、Driver 分为 M01–M04。
- 依据：四个目录为独立 Git 仓库，且各自有独立构建入口和职责。
- 状态：已确认。

## D-002：以 ACL/GE → Runtime → Driver 为主架构

- 决策：把 API、图编译、设备运行时和驱动分开描述。
- 依据：仓库 README 与 CMake 分层；Runtime/Driver 的子目录组织 `[runtime/src/CMakeLists.txt:13-32]`、`[driver/src/CMakeLists.txt:9-30]`。
- 状态：已确认主边界，动态链接细节待补。

## D-003：区分“源码证据”和“架构推断”

- 决策：每篇文档标注证据状态，并对无法运行硬件验证的内容明确说明。
- 依据：当前环境无匹配 NPU/固件，且部分 Runtime→Driver 实现不在已追踪范围。
- 状态：已确认并持续执行。

## D-004：不把未执行命令写成验证结果

- 决策：构建、安装、UT、样例只记录仓库给出的入口，除非实际运行并保留结果。
- 状态：已确认。

## 待决事项

- V1/V2 自动选择与 Sink 启用条件。
- Runtime `Api::SetDevice` 到 HAL 的具体实现路径。
- Driver 用户态 HAL、内核 IOCTL 和设备节点的完整映射。
