# M06 - mtmd 多模态

> 状态：首版构建和边界分析。

## 职责

`tools/mtmd` 构建公共 mtmd library，包含图像、音频和多个视觉/语音模型适配；它链接 ggml 和 llama，私有链接媒体/线程依赖。[tools/mtmd/CMakeLists.txt:15-35] [tools/mtmd/CMakeLists.txt:83-99]

## 解耦约束

CMake 明确检查 mtmd 不得链接 `llama-common`，并支持 `LLAMA_BUILD_MTMD` standalone packaging；视频支持要求 `LLAMA_SUBPROCESS`。[tools/mtmd/CMakeLists.txt:8-11] [tools/mtmd/CMakeLists.txt:131-163]

## Server 接入

completion handler 在带 mctx 的路径调用 `process_mtmd_prompt`，其它路径调用统一 prompt tokenizer；因此 mtmd 影响输入 token/embedding 数据，但 HTTP/task 仍通过 M04 传递。[tools/server/server-context.cpp:4291-4300]

## 未覆盖

各视觉架构 tensor、图片解码、ffmpeg subprocess 和 multimodal graph 尚未逐一追踪；需要真实图片/音频资产和对应模型后验证。

---

**页尾：** 不把 `mtmd` 误写为 server 私有组件。
