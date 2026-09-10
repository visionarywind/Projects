# 调试指南

## 构建和单测

用 Debug 构建：

```bash
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j$(nproc)
ctest --test-dir build-debug -L main --output-on-failure
```

测试 backend/operator 改动时单独构建并执行 `test-backend-ops`；当前均未运行。[CLAUDE.md:49-65]

## Server

```bash
DEBUG=1 ./tests.sh -s -v -x
DEBUG_EXTERNAL=1 ./tests.sh unit/test_chat_completion.py -v -x
```

前者显示步骤和 server verbose，后者让外部 debugger 启动 server；变量和行为由测试 README 确认。[tools/server/tests/README.md:42-46] [tools/server/tests/README.md:68-95]

## 观察边界

- 请求错误：看 `ex_wrapper` 的 400/500 映射。
- decode 错误：看 ret、KV 清理和 batch 缩小分支。
- context 初始化：看 flash-attn/cache 类型约束。
- backend：看 registry/device enumeration 和 scheduler status。

未提供未经执行的断点地址、性能数字或硬件结论。

---

**页尾：** 调试输出可能含 prompt/model 信息，分享日志前由操作者自行审查。
