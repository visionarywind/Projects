# 测试配方

## 文档仓库可执行检查

```bash
git diff --check
```

## 远端项目参考命令（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 分层测试

- API：NULL、越界、无 context、legacy/v2。
- 生命周期：创建失败、重复释放、context reset/destroy。
- 异步：query/synchronize、跨 stream/event、错误延迟暴露。
- 硬件集成：多设备、peer、M3D queue/memory。
- Demo：D01 设备查询，D02 copy，D04/D05 graph。

所有依赖 GPU/私有子模块的结果必须标记实际环境和是否执行。
