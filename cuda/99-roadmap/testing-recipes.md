# 测试配方

1. 静态核对 binary 在 case 前定义，确认 `.cu` device-link 和内部 fatbinary 宏。
2. 为每个改动覆盖成功、参数错误、context/资源错误和清理。
3. 运行环境具备后先 `--listGpus`，再使用明确的 case/模式执行。
4. 阅读 stdout/stderr 和 DVS summary，不把 waived 当 pass；零结果按 runner 失败。

本知识库未执行任何 nvmake、DVS、GPU 或系统权限命令。
