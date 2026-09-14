# M10 执行流程

```text
nvmake include/源分类 → libcuda_test → per-test binary
→ generated test list → cuda_test dispatcher
→ subprocess stdout/stderr → DVS parse
→ aggregate score/exit code
```

构建失败在 make-time 暴露；runner 找不到 binary、没有 summary 或 failure/zero pass 时返回失败。Linux runner 还可能调用 sudo persistence mode 与 `nvidia-smi`。
