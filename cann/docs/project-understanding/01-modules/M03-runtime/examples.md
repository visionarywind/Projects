# M03 Runtime 示例与构建

- 证据状态：命令来自仓库文档/AGENTS；本环境未执行

## 构建

```bash
bash install_deps.sh
python3 download_3rd_party.py
bash build.sh
```

Runtime README/AGENTS 还提供测试和示例入口；实际依赖安装、工具链和设备环境需先确认。

## 调试一个设备选择失败

1. 在 `aclrtSetDeviceImpl` 或 `rtSetDevice` 记录 device ID 和返回码。
2. 继续到 `Api::SetDevice` 内部实现，确认 Context、设备节点和 Driver 错误。
3. 保存 Runtime 扩展错误、Driver 日志和 `npu-smi` 信息。
4. 分离“参数/状态错误”和“设备/固件错误”，不要盲目重试。

无硬件时只能确认编译和门面 UT，不能确认实际设备成功。
