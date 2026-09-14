# M01 测试、示例与开发指南

## 现有测试证据

- `tests/cuda_test/basic_sanity.cu` 同时使用 Runtime API、Driver 内部对象和 CUI 导出符号，验证初始化、context、内存和 launch（静态确认：[tests/cuda_test/basic_sanity.cu:49-100,169-237]）。
- `tests/cuda_test/dispatch_main.cpp` 的 `--listGpus` 会列出设备并停止测试；与 `--forceRun` 的组合才适合实际执行（静态确认：[tests/cuda_test/dispatch_main.cpp:22-50]）。
- `tests/makefile.nvmk` 规定先构建 `libcuda_test` 再构建 `cuda_test`（静态确认：[tests/makefile.nvmk:3-18]）。构建未执行。

## 修改配方

1. 新增 API：先在 `inc/cuda.h` 建立声明/版本契约，再实现 `src/api` wrapper，最后检查 generated API 输入、`.def` 和 `cuda.nvmk`。
2. 修改参数宽度：同时核对 v1/v2 common 实现及输出转换；用 `basic_sanity` 或专门 API 测试检查错误路径。
3. 新增 PTDS/PTSZ 入口：确认宏展开、stream handle type 和导出符号一致。
4. 修改 wrapper 错误：保留输出参数初始化和局部回滚，不把未验证的内部对象暴露给调用者。

## 未验证命令

源码记录的 nvmake/MODS 测试命令依赖外部树、工具链和 GPU；本知识库未执行这些命令，因此没有运行结果。
