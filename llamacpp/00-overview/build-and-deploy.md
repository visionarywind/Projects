# 构建与部署

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
