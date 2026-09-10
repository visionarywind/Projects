# M07 示例与修改练习

## 示例 A：代表 tiling-QKV（未验证）

```bash
python3 kernels/flash-attn/flash_attn_mma.py --D 64 --B 1 --H 1 --N 128 --check
```

源码约束要求 sequence length 对代表 Br/Bc 整 tile；实际 `N=128` 是否匹配所选 dispatch、依赖和 GPU，需运行确认。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-946]

## 示例 B：边界实验

比较 `N` 为整 tile和非整 tile、`D` 为 64/128/256/其他值，记录 assert、dispatch、误差和 launch error。不能将异常结果解释为算法本身错误，需先判断是否违反源码契约。

## 修改练习

1. 跟踪 V alias Q shared region，增加同步前后注释并用 sanitizer 验证。
2. 记录 F16 与 F32 accumulator 的最大误差。
3. 改变 stage 1/2，比较 shared memory、时间和 occupancy。
4. 为代表 binding 增加显式 rank/device/contiguous 检查，再更新接口文档。

所有命令和练习当前未执行。
