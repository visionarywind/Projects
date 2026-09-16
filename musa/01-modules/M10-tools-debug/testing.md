# 工具与调试接口：测试策略

## 静态检查

- 核对入口是否出现在导出/accessor 表。
- 核对失败路径是否阻止半初始化对象进入 tracker。
- 核对资源创建和销毁是否成对。
- 运行 `git diff --check` 只验证本文档仓库的格式，不验证远端项目。

## 运行验证（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

目标环境需要私有子模块、工具链、驱动和 GPU；本知识库没有声称上述命令成功。模块专项用例见 `80-demos` 和远端 `tests` 文件。
