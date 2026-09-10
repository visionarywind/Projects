# 跨模块错误边界

- 对应源码版本：`main` / `0983c65`。
- 证据状态：错误类别已从代表入口确认；不同小算子的检查深度不一致。
- 最后更新：2026-09-10

## 错误分层

```text
环境/编译 -> Python 参数与 tensor 契约 -> binding 检查
         -> kernel launch/runtime -> 数值 reference -> 性能解释
```

| 边界 | 当前行为 | 主要风险 |
|---|---|---|
| Python → extension | `load()` 编译，错误通常在 import/编译阶段暴露 | ABI、nvcc、cache、架构不匹配 |
| tensor → binding | elementwise 主要检查 dtype；NMS 检查 dtype/device/dim/长度 | 非 contiguous、shape 不一致、错误 device |
| binding → kernel | 普通模块不统一检查 launch error；interview 有 `check()` | 异步错误延迟、非法访问 |
| kernel → reference | Python/CPU/torchvision 对拍 | 坐标语义、精度、stable tie 差异 |
| benchmark → 结论 | warmup/synchronize 后计时 | 未同步导致数字不可比 |

证据：`kernels/elementwise/elementwise.cu:134-146`；`kernels/nms/nms.cu:126-150`；`kernels/interview/notes-v2.cu:82-97`。

## NMS 特例

GPU NMS 与 CPU `hard_nms` 的 IoU 面积语义不同：GPU 使用连续坐标，CPU 使用 `+1` 像素面积；因此 CPU 参考不能无条件作为同一语义的 oracle。[kernels/nms/nms.cu:36-57]；[kernels/nms/nms.cc:4-40]

## 调试顺序

先确认环境/编译目标，再缩小输入和运行 correctness，之后启用同步与 sanitizer，最后才分析 TFLOPS。报告中必须区分“源码推断”“仓库历史输出”和“本机实测”。

## 未解决问题

没有统一异常类型、错误码、日志格式或 stream 检查 policy；跨模块修复应在受影响入口逐一补测。
