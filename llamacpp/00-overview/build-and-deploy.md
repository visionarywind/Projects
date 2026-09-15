# 构建与部署

- 文档目的：解释 00-overview/build-and-deploy.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/build-and-deploy.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 默认构建

CMake 在 standalone 模式默认开启 common、tests、tools、examples、server、app；默认单配置输出在 `${CMAKE_BINARY_DIR}/bin`，开发版本为 `0.4.0-dev`。[CMakeLists.txt:5-23] [CMakeLists.txt:40-49] [CMakeLists.txt:129-141]

```bash
cmake -B build
cmake --build build --config Release -j$(nproc)
cmake --build build --target llama-server -j$(nproc)
```

命令来源已确认于根 `CLAUDE.md` 和 README；本批未执行，状态：未验证。

## 后端配置

常见开关是 `GGML_CUDA`、`GGML_HIP`、`GGML_MUSA`、`GGML_METAL`、`GGML_SYCL`、`GGML_VULKAN`、`GGML_CANN`、`GGML_OPENCL`、`GGML_WEBGPU` 和 `GGML_OPENVINO`；ggml CMake 也控制动态 backend、native 优化和 sanitizer。[ggml/CMakeLists.txt:86-90] [ggml/CMakeLists.txt:123-138] [ggml/CMakeLists.txt:189-281]

## 安装/部署

顶层安装 `llama` 库、公共 headers、CMake package 和 pkg-config 文件；common 和 tools 是否安装由选项控制。[CMakeLists.txt:265-308] server 需要模型路径或 router 配置，并可选嵌入 UI；实际部署参数见 `tools/server/README.md`，本文未验证具体环境。

## CI

CPU CI 的典型配置为 `GGML_NATIVE=OFF`、fatal warnings 和 RPC；server CI 单独构建 server 后运行 pytest。完整 self-hosted runner 需要模型、Python、git-lfs、硬件和网络，故不把 CI 结果当作本地验证。

---

**页尾：** 任何平台结论必须注明 toolchain、backend 和是否真的执行。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
