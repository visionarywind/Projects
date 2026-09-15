# M03 调用链、源码地图与风险

- 文档目的：解释 01-modules/M03-device-hal/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-device-hal/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 设备链

```text
cuDeviceGet / cuDeviceGetCount
 → API device wrapper
 → globals.devices / remap table
 → cuiDevice* 查询
```

设备初始化阶段：

```text
globalsInit / device discovery
 → CUdev.state.ccArch
 → InitDeviceArch
 → deviceInitArchKepler/…/Ampere
 → CUdev->hal function table
```

`InitDeviceArch` 的架构候选及编译条件有直接证据：[src/cui/hal/hal.c:35-79]；当前构建具体宏值未知。

## 文件地图

| 文件/目录 | 角色 |
|---|---|
| `src/cui/devmgr.c` | 设备表、过滤、排序、UUID/NVAPI/MPS |
| `src/cui/device.c` | 通用 device 状态与操作 |
| `src/cui/cuidevice.c` | Driver API 对接的设备查询 |
| `src/cui/hal/hal.c` | 架构分派 |
| `src/cui/hal/*` | 架构具体 device/launch/mem/QMD |
| `src/cui/dmal/common_*` | DMAL 通用连接 |
| `src/cui/dmal/rm/*` | RM backend |
| `src/cui/dmal/mrm/*` | MRM backend |
| `src/cui/dmal/wddm/*` | WDDM backend |
| `src/cui/dmal/amod/*` | amodel backend |

## 风险

- 架构代码由编译宏裁剪；未启用的 case 在二进制中不存在，静态阅读不能代表目标构建。
- ordinal 重排和 `CUDA_VISIBLE_DEVICES`/MPS 筛选可能改变设备索引；测试不能硬编码物理顺序。
- DMAL 到外部 RM/NVRM 是未完成链路，需匹配外部树和构建日志验证。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
