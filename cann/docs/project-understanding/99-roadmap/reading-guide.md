# 开发实践：阅读指南

## 按问题选择入口

| 问题 | 阅读顺序 |
|---|---|
| 模型编译失败 | M01 README → GE architecture → Parser/Compiler → error boundaries |
| 模型运行失败 | M01 call-chains → M03 interfaces → M04 HDC/Queue |
| `aclrtSetDevice` 失败 | M02 line-level → M03 `api_c_device.cc` → `Api::SetDevice` → M04 DMS |
| Stream/Event/内存问题 | M02 data structures → M03 Runtime objects → M04 queue/通信 |
| 驱动/设备节点问题 | M04 source-map → Queue/HDC/esched → Runtime caller |
| 性能问题 | cross-module performance → GE planning → Runtime queue → Driver |

## 阅读原则

先读接口契约，再读实现和调用者；区分已确认源码事实、架构推断和未验证硬件行为；跨仓变更必须看 change-impact-map。
