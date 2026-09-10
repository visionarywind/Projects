# M07 开发指南

## 修改前

1. 先确定目标变体和真实 export；不要只改名字相近的 `.cu`。
2. 记录 `[B,H,N,D]` layout、Br/Bc、stage、padding、accumulator 和架构宏。
3. 检查 `QKV_seqlen % max(Br,Bc)==0` 约束和 D dispatch。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-946]

## 修改后

1. 对 torch/SDPA/reference 做 correctness；
2. 覆盖整 tile、尾 tile、D dispatch、stage 1/2；
3. 检查 shared alias、barrier、shuffle mask；
4. 再用 sanitizer 和 Nsight；
5. 更新 source-map、line-level、performance、risk 和 evidence-index。

当前仓库没有统一 GPU CI；命令是否可运行未验证。`setup.py` 和 `tools/utils.py` 为空，主要构建入口仍是 `flash_attn_mma.py` 的动态 load，这一事实不能被文档入口混淆。
