# M09 代表性例子

- 无 `platforms` 但有 `num_platforms`：loader 可只返回数量；不合法组合返回 `CL_INVALID_VALUE`。
- 非 Windows 构建仍为 D3D10 dispatch 保留 NULL 槽位，保持 table ABI 索引。
- `NVCL_IDLE_TIMEOUT_MS=0` 会让 worker 在 DAG 为空后立即进入等待状态；这是配置语义，未运行验证。
