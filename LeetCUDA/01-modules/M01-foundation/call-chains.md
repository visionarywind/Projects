# M01 调用链

```text
notes-v2.cu -> phase test/bench -> base/common helper
 -> stage-specific kernel or PTX helper
 -> synchronize/check -> compare/free
```

完整消费者列表见 `[kernels/interview/README.md:6-16]`；具体 helper 符号链待继续核对。
