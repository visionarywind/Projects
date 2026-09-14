# M07 测试与开发指南

## 测试

- `basic_sanity:kernel` 是当前最直接的 CNP/内建 device launch 使用者；通过 `CNP_SUPPORTED` 和设备 capability 分支决定预期结果（静态确认：[tests/cuda_test/basic_sanity.cu:127-145,194-225]）。
- `bik/` 下有 kernel memcpy 和 memory utility 测试；文件存在但未执行。
- stream/channel 测试可间接验证 syscall/launch 产生的提交活动，但不单独证明 device-side ABI。

## 修改配方

1. 新 syscall：更新 CUI resource setup、tracking list、所有架构实现和生成构建输入。
2. 修改 CNP：同时核对 `cuicnp.c`、launch queue、stream/channel 提交、错误和 context destroy。
3. 修改生成 kernel：标明输入、生成器、目标架构和产物，不能只提交二进制。
4. 调试：先检查 launchData/QMD/constant-bank，再检查 device-side syscall buffer，最后到 backend/RM；当前均未运行验证。

## 未验证

需要外部 nvmake、compiler、GPU 和匹配驱动才能构建或执行本模块；本知识库未执行。
