# K02：设备发现、拓扑、能力协商与兼容

## 边界

本知识域处理“一个任务能否在目标设备上安全运行”的准入问题：设备身份、静态能力、版本配套、拓扑属性和动态资源水位。它不负责任务在设备上的执行语义（K05），也不负责内存池内部结构（K04），但会引用它们的约束作为准入条件。

本域解决：

1. 设备发现的真实副作用，而不是把 device ID 当索引；
2. 静态能力、版本矩阵和拓扑属性的采集与判定；
3. 动态资源水位与快照时效性；
4. 准入失败的结构化原因与降级路径。

本域不负责：

- 任务的提交与完成（K03/K05）；
- 内存分配的具体路径选择（K04）；
- 跨卡/跨节点通信的完成语义（K08）；
- 运行时二次分配的失败处理（K04/K11）。

与相邻域的接口：K02 输出 `capability_snapshot` 和 `admission_result`；K03 负责准入通过后的对象创建与绑定；K05 负责准入通过后的完成判定；K04/K06 负责动态配额和池层面的二次校验。

## 核心机制

### 1. 设备身份不是能力

设备 ID 只是索引。`rtSetDevice` 不是简单写入 device ID：它 retain Primary Context，把引用对象写入线程局部容器，校验 Context/default Stream，设置 SatMode，清除显式 current Context 并发出设备状态回调。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:45`、`cann/01-modules/M03-runtime/call-chains.md:14-31`。

完整控制流：

```text
rtSetDevice(devId)
  -> ApiImpl::SetDevice
  -> Runtime::PrimaryContextRetain(devId)
  -> InnerThreadLocalContainer::SetCurRef(primary-context RefObject)
  -> 校验 Context / DefaultStream
  -> Device::SetSatMode(...)
  -> 清除显式 current Context
  -> DeviceStateCallbackManager::Notify(SET_POST)
```

因此 ACL/GE 在调用设备 API 后拿到的通常不是“一个整数状态”，而是 Runtime 管理的 Primary Context 引用。[已确认] 同一逻辑编号在不同节点可能对应不同型号或软件栈。

### 2. 分层准入

把准入拆成四层，逐层判定：

1. **身份层**：设备是否存在、可访问；驱动与 Runtime 是否可加载；Primary Context 是否可取得；
2. **能力层**：dtype、shape、算子、内存和队列能力是否支持；
3. **拓扑层**：跨卡、跨 NUMA、跨节点时链路是否可用；
4. **资源层**：此刻是否有足够内存、队列、带宽和配额。

判定式：

```text
admit(task, device) = static_compatible
                    ∧ topology_compatible
                    ∧ dynamic_budget_available
```

任何一项为假都不能把任务标记为可运行。

### 3. 版本配套与跨仓 ABI

CANN 由 GE、ACL、Runtime、Driver 四个仓库组成，可独立构建和发布。好处是解耦，代价是跨仓版本配套和 ABI 兼容要求高，错误码、头文件和二进制符号需要同步；并且没有设备时只能做有限验证。[已确认] 来源：`cann/00-overview/architecture.md:50-54`。

这决定能力协商必须覆盖：

- 头文件与共享库版本是否配套；
- 模型格式、设备固件和工具链是否兼容；
- 公共错误码映射是否一致（见 K09）。

驱动侧进一步区分 HAL 与 SDK-driver：`driver/src/CMakeLists.txt` 将构建拆成 `ascend_hal`、`sdk_driver` 和可选 `custom`，`DRIVER_COMPAT` 只构建 HAL。[已确认] 来源：`cann/01-modules/M04-driver/design.md:16`。这意味着“驱动可用”本身是个分层结论，不能一句话概括。

### 4. 拓扑与属性编码

拓扑不是抽象描述，而是被编码进分配与传输请求的 flags：

- `rtMalloc` 经过 `ApiImpl::DevMalloc` 解析 memory type、policy、advise、module/device config 并按 32-byte 对齐；Driver 按 online/offline/AICPU-scheduler 选择路径，并把页类型、HBM/DDR/P2P、节点和 module ID 编进 HAL flags，最终调用 `halMemAlloc`。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:80`。
- Driver V3 `cache_init` 根据 host/device、P2P、huge-page 组合创建多类 allocator，并按 `(devid, flag)` 建立全局 allocator 表。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:67-71`。
- SHMEM 初始化后端为 P2P/RDMA/SDMA 建立 host/device heap base 指针数组，说明传输路径与后端类型相关。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:87-93`。

属性编码意味着：拓扑兼容性必须在提交前判定，而不是等到分配或传输失败才发现。

### 5. 能力快照与时效性

能力快照的时间属性是这类系统最容易出错的地方：

```text
capability_snapshot = {
  collected_at, device_serial,
  kind, runtime_version, driver_version,
  supported_dtypes, memory_bytes,
  peer_devices, queue_limits, available
}
```

`memory_bytes` 和可用队列数会变化，因此快照不是永久真相，只能作为“提交前准入检查”。提交后仍需处理竞态：其他租户可能抢占资源、设备可能进入错误状态、拓扑链路可能失效。[推断] 稳定设计应将静态能力与动态水位分离，并在真正分配/提交处再次验证。

### 6. 结构化拒绝与降级

准入失败应返回结构化原因，而不是笼统的 `unavailable`：

```text
reasons = [device_unavailable,
           dtype_unsupported,
           insufficient_memory_snapshot,
           topology_not_supported,
           version_incompatible]
```

根据原因选择动作：换设备、排队等待、降级（转换 dtype、CPU/NPU 替代）或明确拒绝。降级必须记录原因并验证结果 oracle，因为降级会改变精度和正确率预算。

### 7. 设备热插拔与在线升级

设备热插拔要求准入链路处理“设备在运行时消失/出现”。这类问题与 K03 的生命周期和 K12 的演进交叉：

- 设备消失时，持有其 Context/Stream/内存的对象必须进入可诊断、可回收状态；
- 设备出现时，能力快照需要重新采集，不能复用旧快照；
- Driver 侧要求 HDC client 必须无活动 session 才能销毁。[已确认] 来源：`cann/90-cross-module/cross-module-call-chains.md:52`。

热插拔的具体行为依赖平台与固件，属于 `[待验证]`。

### 8. 准入链路的可读模型

把准入写成数据结构和判定函数，便于表驱动测试：

```python
@dataclass(frozen=True)
class Capability:
    device_id: int
    kind: str
    runtime_version: str
    supported_dtypes: frozenset[str]
    memory_bytes: int
    peer_devices: frozenset[int]
    available: bool

def admit(cap: Capability, req: dict) -> tuple[bool, list[str]]:
    reasons = []
    if not cap.available:
        reasons.append("device_unavailable")
    if req["dtype"] not in cap.supported_dtypes:
        reasons.append("dtype_unsupported")
    if req["bytes"] > cap.memory_bytes:
        reasons.append("insufficient_memory_snapshot")
    if req.get("peer") is not None and req["peer"] not in cap.peer_devices:
        reasons.append("topology_not_supported")
    return not reasons, reasons
```

生产实现还要校验版本兼容、权限、上下文创建和实时资源水位；上面代码只是能力协商的可读模型，不代表任一厂商 API 的完整实现。准入失败应返回结构化原因，并选择其他设备、排队等待、降级或明确拒绝。

### 9. 版本矩阵与 feature flag

能力协商不只看单个版本号，而是看组合是否配套。建议维护一个可检查的版本矩阵：

```text
compatible(GE, ACL, Runtime, Driver, firmware, toolkit, model_format)
  -> bool + 具体不匹配项
```

[已确认] 头文件、共享库、模型格式、设备固件和工具链必须来自兼容的 CANN 版本；Runtime README 明确要求源码版本配套。来源：`cann/00-overview/design-principles.md:24`。

feature flag 用于表达能力开关（例如某 SoC 是否支持某 allocation 路径），必须区分“设计接口存在”和“当前实现已启用”。一个典型例子是 SOMA 的 cached 段重用：接口和候选函数存在，但 `TryToReuse` 当前返回 `nullptr`，不能当成已启用能力。[已确认] 来源：`cann/01-modules/M03-runtime/memory-pool-analysis.md:27-29`。这条原则同样适用于能力协商：能力表应区分“声明支持”和“运行时可证”。

## 常见失效条件

- **把设备编号当能力。** 同一逻辑编号在不同节点可能代表不同型号或软件栈。
- **把总显存当可分配显存。** 碎片、对齐、缓存池和其他租户都会改变实际可用量（K04）。
- **只检查静态版本，不检查动态资源。** 启动时通过，稳态高峰仍可能 OOM 或排队超时。
- **把“枚举成功”当“可运行”。** 设备可枚举不等于 Primary Context 可取得、dtype 可支持。
- **快照长期缓存。** 用陈旧快照准入，误判容量或拓扑。
- **降级不记录原因。** 精度变化无法归因，结果 oracle 失效。
- **只看单卡显存不看拓扑。** 多卡任务忽略 P2P/互联/NUMA，导致集体通信退化或失败（K07/K08）。

## 关键不变量

1. **身份落地**：设备选择必须落到 Primary Context retain 和线程绑定，不能停留在 wrapper 层的整数状态。[已确认]
2. **静态与动态分离**：能力快照记录采集时间和版本；动态水位在分配/提交处二次校验。
3. **四项齐全**：`admit` 必须同时满足静态兼容、拓扑/权限可用、动态预算足够。
4. **属性端到端一致**：分配写入 HAL flags 的页类型、节点和 module，必须在释放时以同样的键回收。
5. **拒绝可解释**：拒绝原因集合与预设原因一致，不返回笼统错误。
6. **版本配套**：头文件、共享库、模型格式、固件和工具链来自兼容的 CANN 版本。[已确认]

## 与其他知识域的连接

- ← **K01**：准入是稳定性契约在提交前成立的前提。
- → **K03**：准入通过后创建/绑定 Context、Stream；Primary Context 的 retain 语义在此展开。
- → **K04**：动态水位、碎片、对齐和池选择决定“内存够不够”的真实答案。
- → **K05**：拓扑和 dtype 兼容通过后，仍需完成证据才能判定结果可读。
- → **K06**：能力层的队列/并发限制与配额和调度直接相关。
- → **K07/K08**：P2P、NUMA、RDMA/SDMA 后端来自本域采集的拓扑属性。
- → **K12**：设备热插拔和在线升级是准入链路在生产演进中的延伸。

## 题目索引

主域题目（`01-question-bank` 中 `知识域：K02`）：

- 入门：`HRS-B002`（提交前的能力协商）
- 进阶：`HRS-I002`（设备发现、拓扑采集与准入链路）、`HRS-I013`（设备热插拔与在线升级）
- 高级：`HRS-A002`（兼容矩阵与可控能力降级）
- 专家：`HRS-E002`（带证明义务的形式化兼容契约）

## 证据入口

- `cann/00-overview/architecture.md:50-54`：跨仓独立发布、ABI 兼容与无设备验证边界。
- `cann/90-cross-module/memory-and-resource-lifecycle.md:45,78-82`：`rtSetDevice` 副作用与 HAL flags 属性编码。
- `cann/01-modules/M03-runtime/call-chains.md:14-31`：设备选择完整控制流。
- `cann/90-cross-module/pooling-and-resource-management.md:67-71,87-93`：按 `(devid, flag)` 的 allocator 表与 SHMEM 后端。
- `cann/01-modules/M04-driver/design.md:14-45`：HAL/SDK-driver 分层与 ABI/安全校验。
- `cann/90-cross-module/cross-module-call-chains.md:42-52`：HDC client 无活动 session 才可销毁。
- `cann/00-overview/design-principles.md:22-27`：版本配套与异步调用者约束。

## 学习提示

先分清“身份”和“能力”：identity 回答设备是谁，capability 回答它此刻能做什么。再用四层准入（身份/能力/拓扑/资源）组装判定式，并用快照时效性检查自己是否把动态水位当成了静态事实。遇到多卡问题时，回到 K02 采集拓扑属性，再到 K07/K08 展开传输与通信语义。
