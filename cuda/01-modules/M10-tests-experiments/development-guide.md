# M10 开发指南

1. 新测试先加入对应 `tests.nvmk`，确认 binary 在 case 前定义。
2. `.cu` 测试核对 device-link、nvcc flags 和内部 fatbinary header 规避宏。
3. 每项测试覆盖成功、参数错误、context/资源错误和清理，不只断言 happy path。
4. runner 修改保持 stdout/stderr 独立消费、summary 去重和 no-result failure。
5. 实验记录工具链、目标架构、输入、产物与执行结果；当前所有执行均未验证。
