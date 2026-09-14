# D01 Demo 注册表

| ID | 入口 | 覆盖 | 状态 |
|---|---|---|---|
| D01 | `tests/cuda_test/basic_sanity.cu` | init、device、context、memory、module/function、kernel、CNP | 静态确认，未运行 |

D01 是源码级追踪样例，不是已通过的 GPU 测试报告。

## 覆盖缺口

当前没有专门覆盖 `cuStreamBeginCapture` → `cuGraphInstantiate` → `cuGraphLaunch` → `cuGraphExecDestroy` 的 Graph Demo。M06 的 [Graph 资源生命周期](../01-modules/M06-module-launch/graph-resource-lifecycle.md) 提供静态轨迹和待补测试矩阵；在 nvmake/GPU 环境可用前，不把该路径标记为端到端已验证。
