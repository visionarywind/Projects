# 跨模块接口契约

| 上游 | 下游 | 关键接口 | 前置条件 | 返回/副作用 | 状态 |
|---|---|---|---|---|---|
| Tool/App | Driver API | `muInit`、`muDeviceGet*` | 库可加载 | `MUresult`、输出参数 | 已确认 |
| Driver API | Platform | `Platform::Get().Init/GetDevice` | flags/ordinal 合法 | Platform/Device 状态 | 已确认 |
| Driver API | Context | `TlsCtxTop`、`Create*` | current context 有效 | Core resource | 已确认 |
| Context | Memory | `MemoryCreateInfo`、`Init` | Device/HAL 有效 | tracker + pointer | 已确认 |
| Context/Stream | Command | `Cmd*`、`ResolveDependencyAndQueueCommand` | stream 有效 | queued command | 已确认 |
| Core | HAL/M3D | `CreateQueue`、`CreateCmdBuffer`、`Allocate` | HAL device | 底层资源 | 部分确认 |

契约中“已确认”只表示源码存在并按此调用；不表示硬件实现或异常语义已完整验证。
