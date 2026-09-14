# M06 执行流程

```text
module image → ELF/fatbinary/JIT parse → CUmod → CUfunc metadata
→ API parameter validation/pack → CUIlaunchData
→ capture: graph node
→ normal: setup → QMD/const bank → stream push
```

launch setup 失败不应留下未跟踪的 syscall、const bank 或 UVM running 状态；已进入 push 后统一经 `streamEndPush`，最后发 launch-end callback。
