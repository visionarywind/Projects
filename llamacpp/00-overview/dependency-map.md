# 依赖地图

## 构建依赖

```text
ggml -> llama -> common -> {tests, examples, tools}
                         -> app
server-context -> llama-server-impl -> llama-server
mtmd -> llama + ggml
ui assets -> llama-server-impl
```

**已确认：** 顶层 CMake 的条件 `add_subdirectory` 给出上述大致顺序。[CMakeLists.txt:210-258] **已确认：** server impl 链接 context、UI、httplib；server executable 链接 impl。[tools/server/CMakeLists.txt:36-67] **已确认：** mtmd 链接 ggml、llama，并明确不链接 llama-common。[tools/mtmd/CMakeLists.txt:83-115] 

## 依赖类型

| 类型 | 例子 | 是否等于链接依赖 |
|---|---|---|
| 编译/链接 | CMake target、vendor、线程库 | 是 |
| 源码 | `LLM_ARCH_*`、模型类、模板文件 | 否 |
| 运行时 | backend registry、设备、HTTP queue | 否 |
| 数据 | GGUF、词表、cache、模型文件 | 否 |
| 生命周期 | backend -> model -> context -> task/result | 否 |

## 解耦意图

- M01 不知道上层请求语义，只执行 tensor graph。
- M02 对外提供稳定 C API，隐藏 model/context 实现。
- M03 把应用语义（模板、grammar、下载）留在 runtime 外。
- M04 以 task/result 隔离 HTTP 与推理线程。
- M06 不依赖 common，便于单独打包。[tools/mtmd/CMakeLists.txt:131-163]

---

**页尾：** 具体第三方版本以各自 CMake、package 和 requirements 为准，本文不重复供应商清单。
