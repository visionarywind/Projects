# 依赖地图

## 编译依赖

```text
src/driver (shared musa_dynamic)
  -> musaCore (static)
      -> halM3d (static)
          -> m3d + scpc + util + dl/pthread/rt
src/tools/*.cpp -> musa_dynamic
unittest/*.cpp -> gtest + driver/core targets
```

证据：[src/driver/CMakeLists.txt:9-18] [src/musa/core/CMakeLists.txt:1-11] [src/hal/m3d/CMakeLists.txt:36-59] [src/tools/CMakeLists.txt:10-18]

## 运行时依赖

| 上游 | 下游 | 关系 | 证据状态 |
|---|---|---|---|
| `muapi*` | Platform/Context | 直接调用、句柄解析 | 已确认 |
| Context | Memory/Stream/Graph/Module | 创建、校验、销毁 | 已确认 |
| Stream | Command/HAL queue | 排队、提交、等待 | 已确认 |
| Core | HAL/M3D | C++ 接口调用 | 部分确认 |
| M3D | kernel driver/GPU | 私有实现最终副作用 | 未展开 |

## 风险边界

依赖图不能等价于调用图：CMake 的静态链接关系证明“可用”，不证明每个运行时路径都会访问所有下游对象。尤其 `muInfo` 只需设备查询，不会触发 kernel/graph 路径。
