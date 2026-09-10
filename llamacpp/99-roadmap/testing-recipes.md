# 测试配方

## C++

```bash
ctest --test-dir build -N
ctest --test-dir build -L main --output-on-failure
ctest --test-dir build -R '^test-jinja$' --output-on-failure
```

CTest label 默认是 `main`，模型/fixture 测试可能需要下载或先生成 dummy models。[tests/CMakeLists.txt:19-46] [tests/CMakeLists.txt:201-257]

## Backend

```bash
cmake --build build --target test-backend-ops -j$(nproc)
./build/bin/test-backend-ops -b CPU
```

命令来源已确认，未运行。

## Server

```bash
python3 -m pip install -r tools/server/tests/requirements.txt
cd tools/server/tests
./tests.sh unit/test_chat_completion.py::test_invalid_chat_completion_req
```

`tests.sh` 没参数时默认排除 slow marker，有参数时将参数直接交给 pytest。[tools/server/tests/tests.sh:7-20]

## Python/UI

```bash
python3 -m unittest discover ./gguf-py -v
cd tools/ui && npm run check && npm run lint && npm run test
```

以上命令都只完成静态来源确认；本批没有测试通过结果。

---

**页尾：** “注册”“可构建”“运行通过”是三种不同状态。
