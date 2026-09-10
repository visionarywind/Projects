# M03 调用链

```text
Python entry -> load()/binding -> row/block launch
 -> local register computation
 -> warp shuffle/shared reduction
 -> normalization or softmax writeback
 -> synchronize/reference compare
```

这是由目录和 interview 阶段归纳的模式；每个小算子的实际函数名、axis 分支和错误处理需逐文件确认。基础阶段范围证据为 `[kernels/interview/notes-v2.cu:11-15]`。
