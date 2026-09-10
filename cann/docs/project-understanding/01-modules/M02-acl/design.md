# M02 ACL 设计

- 对应源码版本：`d26308d3`
- 证据状态：包装层已确认；完整内部对象模型部分未知

## 定位

ACL 提供稳定的应用级接口，将用户参数转换成 Runtime 可接受的句柄、buffer、模型和执行请求。`acl/CMakeLists.txt` 显示其实现链接 Runtime、HAL、MetaDef 和 DFX 等能力 `[acl/CMakeLists.txt:110-126,167-190]`。

## 典型分层

```text
ACL public C API
  -> acl*Impl wrapper
  -> rt* Runtime API
  -> Runtime Api::Instance()
  -> Driver/HAL
```

设备 API 的实现直接调用 `rtSetDevice`，失败时映射错误并记录统计 `[acl/runtime/device.cpp:47-59]`。这表明 ACL 的设备层主要承担参数/日志/错误适配，而非实现设备调度。

## 状态与所有权

应用需在使用设备资源前完成 ACL 初始化和设备选择；Context、Stream、Event、DataBuffer、Model 等对象有各自创建和销毁 API。异步提交后，输入 buffer、输出 buffer、callback 和关联 stream 必须保持有效；这是调用者责任，精确 API 例外需查公共头文件。

## 设计取舍

统一应用接口降低上层接入成本，但跨 ACL/Runtime 的错误转换、句柄 ABI 和版本耦合增加排查成本。该代价是从 CMake 依赖和包装实现可确认的结果；更深层动机标为推断。
