# M07 调用链

## 代表 tiling-QKV

```text
CLI
 -> flash_attn_mma.py:get_build_sources/get_build_cuda_cflags
 -> torch cpp_extension.load
 -> pybind/flash_attn.cc exported tiling_qkv function
 -> head-dim/stage dispatch
 -> global Q/K/V offsets
 -> shared Q/K loads (V aliases Q region)
 -> ldmatrix + HMMA QKᵀ
 -> row max/exp/sum, S→P
 -> V load + HMMA P@V
 -> online O rescale + 1/l normalize
 -> four-lane shuffle/vectorized O store
 -> Python reference/check/warmup/TFLOPS
```

代表实现证据：`[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:94-200,201-797]`；dispatch 证据：`[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-946]`。

## 资源、同步和错误

Q/K/V/O storage 由 Python/PyTorch 持有；shared/register 是 kernel invocation 资源。stage 1/2 的 wait/sync、Q→V alias、block early return 和整 tile assert 是实际同步/边界点。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:151-170,201-246,845-849]

当前所有 variant 的 binding rank/device/contiguous/launch-error 和 TMA descriptor 生命周期未统一核对，状态为部分完成。
