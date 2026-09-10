# M05 调用链

```text
sgemm.py -> load(sgemm*.cu)
 -> exported launcher
 -> block maps BM×BN output tile
 -> cooperative A/B shared loads
 -> __syncthreads
 -> K-loop register accumulation
 -> C global store
 -> Python synchronize/reference/TFLOPS
```

具体导出函数因实现而异；上述链条由代表 SGEMM 源码和 benchmark 组织确认。[kernels/sgemm/sgemm.cu:36-166]。
