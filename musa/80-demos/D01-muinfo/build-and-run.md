# D01：构建与运行

## 构建（未执行）

```bash
cmake -S . -B build
cmake --build build --target muInfo
LD_LIBRARY_PATH="$PWD/build/lib:$LD_LIBRARY_PATH" ./build/bin/muInfo
```

实际 target/output 目录应以目标源码生成结果为准；`src/tools/CMakeLists.txt` 明确建立可执行文件并链接 `${DRIVER_LIB_NAME}_dynamic`。[src/tools/CMakeLists.txt:10-18]

## 安装后运行（未执行）

```bash
./install.sh
muInfo
```

安装会写系统库和 bin 目录，需明确授权。[install.sh:83-105]
