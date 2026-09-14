# M07 代表性例子

`gpuIncrement` 在 `CNP_SUPPORTED()` 下递归执行 child launch；host 侧仍通过 stream/context synchronize 等待最终结果。该例说明 CNP 是 kernel/device service 与 M05/M06 异步资源的交叉点，但不证明当前环境支持 CNP。

`experiments/cnp/` 中的 cubin/spec 是历史实验资料，不能单独证明当前生成链。
