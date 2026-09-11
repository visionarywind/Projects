# 功能开发配方

## 新增模型层/spec

1. 修改/新增 M01 layer spec。
2. 确认 builder 分支和 config 字段。
3. 检查 TransformerBlock forward 参数兼容。
4. 更新 sharded_state_dict/checkpoint 兼容性。
5. 添加 focused unit tests。

## 新增 batch 字段

1. M04 dataset 生产字段。
2. `get_batch` 搬运和广播。
3. `forward_step_func` 传给模型。
4. M01 forward 签名或 `extra_block_kwargs` 消费。
5. 更新接口契约。

## 新增推理能力

1. M06 context/engine/request 字段。
2. wrapper/controller 路径。
3. GPTModel inference path。
4. server/offline example。
5. 容量和 KV cache 测试。