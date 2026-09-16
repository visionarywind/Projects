# 阅读指南

## 30 分钟

README -> project-overview -> architecture -> module-registry -> D01。

## 半天

M02 API -> M03 Platform/Device -> M04 Context -> M05 Memory -> M06 Stream -> M07 Kernel -> M08 Graph -> M09 HAL。

## 阅读技巧

每条链都记录“入口、对象查找、状态变更、下游副作用、错误、清理、线程”。只看到 wrapper 时继续追 Core；进入 M3D 子模块后明确标注证据边界。
