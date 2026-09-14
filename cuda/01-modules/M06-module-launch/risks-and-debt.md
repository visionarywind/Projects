# M06 风险与技术债

| 风险 | 证据 | 影响 |
|---|---|---|
| 参数 metadata 缺失 | `[src/api/apilaunch.c:141-147]` | 现代 kernelParams 被拒绝 |
| packed/unpacked 双路径 | `[src/api/apilaunch.c:155-205]` | 兼容复制和生命周期复杂 |
| capture 失败使 graph invalidated | `[src/api/apilaunch.c:270-278]` | 后续 API 需观察 capture invalidated |
| launch 异步引用 | `[src/cui/cuilaunch.c:163-218]` | function/module/memobj 不能过早释放 |
| module unload 反向清理 | shared ELF、function、syscall、UVM、CNP cache 和 tools 状态需按顺序撤销 | 部分失败/重复 unload 的观察点跨多个所有权域 |
| graph 动态 context | launch stream 可能不在 instantiate context 集合，且 API stream 指针临时替换 | 锁数组、marker 和 UVM rollback 必须保持一致 |

**深度状态**：API launch、参数、tracking、setup、module/ELF/JIT、graph instantiate/launch/unload 已完成静态主线；架构 QMD/ABI 具体字段、compiler/gpgpucomp 和设备执行仍未闭合。

