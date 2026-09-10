# 技术债务

- 证据状态：待办事项来自当前覆盖缺口

## 代码和调用图

- 补齐 Runtime `Api::SetDevice`、Context、Stream、Memory、Task 到 Driver HAL 的静态路径。
- 按设备、内存、Queue、HDC、esched 分别建立 HAL→ioctl→内核映射。
- 补齐 GE Compiler Pass/Engine/Planner、V1/V2 和 ACL Model/Single-op 源码地图。

## 契约和验证

- 生成公共头文件、导出符号、错误码和结构体 ABI 清单。
- 形成 Context/Stream/Event/Buffer/Session 的线程安全和所有权表。
- 建立 Toolkit/Driver/Firmware/SoC 兼容矩阵。
- 建立无设备 mock、UT、跨仓集成和真实设备 CI 分层。

## 文档维护

每次源码版本变化更新 `analysis-state.md`、证据索引、调用链和变更影响地图；不保留无法追溯来源的“已确认”结论。
