# 阅读路线

## 初学者

README → project-overview → runtime-model → D01 → M02 README → M03 README。

## 模型开发

M01 design/implementation → TransformerBlock forward → M02 schedule → M05 checkpoint。

## 训练系统开发

M03 implementation → M02 execution-flows → M05 optimizer/checkpoint → error-boundaries。

## 数据开发

M04 README → GPTDataset → sampler → `pretrain_gpt.get_batch` → shared-data-and-types。

## 推理开发

M06 README → StaticInferenceEngine → DynamicInferenceEngine → GPTInferenceWrapper → server 入口。

每条路线都应回到 `evidence-index.md` 核对源码版本和行号。