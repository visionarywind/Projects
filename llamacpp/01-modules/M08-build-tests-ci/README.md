# M08 - 构建、测试与 CI

> 状态：首版构建/测试入口分析。

## 构建编排

顶层 CMake 先加入 ggml、src、vendor，再按选项加入 common/tests/examples/tools/app/mtmd。[CMakeLists.txt:204-258] CTest helper 用 `llama_build` 创建 executable，用 `llama_test` 注册测试和 label，fixtures 用于生成 dummy models 后供 state tests 使用。[tests/CMakeLists.txt:1-117] [tests/CMakeLists.txt:201-257]

## 常用验证

```bash
cmake -B build
cmake --build build --config Release -j$(nproc)
ctest --test-dir build -L main --output-on-failure
cmake --build build --target test-backend-ops -j$(nproc)
```

## 验证记录

- `cmake -B build -DLLAMA_BUILD_UI=OFF && cmake --build build --target llama-server llama-cli -j$(nproc)`：通过（CPU/x86_64）。
- `ctest --test-dir build -N`：已执行，仅列出测试；因测试 target 尚未构建，输出包含找不到 executable 的 CTest 提示，未执行测试用例。
- `python3 -m unittest discover ./gguf-py -v`：失败，`tests.test_gguf_reader_validation` 导入时缺少 `pytest`（`ModuleNotFoundError`）。
- server pytest、UI tests、`test-backend-ops`：未执行。

上述结果更新了“命令来源已确认”和“实际执行结果”的区别。

## CI 边界

CPU CI 常关闭 `GGML_NATIVE` 并开启 fatal warnings；backend CI 需要匹配硬件；server CI 先构建 server 再运行 `tools/server/tests/tests.sh`。完整 self-hosted 命令见 `ci/README.md`，不是无依赖的单机 smoke test。

## 修改影响

CMake option 会改变 target、链接库和运行时 backend registry；测试 fixture/label 改动可能影响顺序和模型下载。不要未经批准在 `tests/*` 新增测试文件。[AGENTS.md:83-86]

---

**页尾：** 测试“已注册”不代表测试“已通过”。
