# D02 Ray Data 与 Tune 资源协作

- 来源：`source/ray/doc/source/data/doc_code/key_concepts.py:4-24`。
- 证据状态：脚本和调用关系已静态确认；未运行。

## 选择理由

该仓库内的官方文档代码用 `ray.data.range(10).show()` 作为 Tune trainable 的最小 Data workload，并用 `ray.init(num_cpus=4)`、`max_concurrent_trials=3` 表示给 Dataset 留出稀疏 CPU。它比模型下载 demo 更适合源码级追踪，且不依赖外部模型文件。

## 源码步骤

1. `ray.init(num_cpus=4)` 进入 M06 启动/连接边界。[`key_concepts.py:11-12`]
2. Tune 将 `objective` 包装成带 `{"cpu": 1}` 的 trainable，并设置 `num_samples=1`、`max_concurrent_trials=3`。[`key_concepts.py:14-23`]
3. Trial 执行 `objective`，调用 `ray.data.range(10)` 建立 Dataset，再 `.show()` 触发读取/执行。[`key_concepts.py:7-9`]
4. Data 的 logical plan/blocks 进入 M08 executor；Tune 的 Trial/Controller 属于 M10；底层任务、ObjectRef、资源由 M01-M04 承接。[推断自已定位源码，未动态验证]
5. `tuner.fit()` 等待试验完成并返回结果；脚本未显式 `ray.shutdown()`，退出清理由运行时行为决定，未验证。[`key_concepts.py:17-24`]

## 关键数据与资源

| 数据/状态 | 所有者/作用 |
|---|---|
| `Dataset` | driver 侧 lazy plan，`.show()` 才触发执行 |
| blocks/ObjectRefs | Data executor 与 Object Store 间的中间数据 |
| Trial | Tune controller 的实验状态 |
| CPU slots | 4 个总槽位；最多 3 个 trial 预留 1 CPU，意图保留 Data 执行空间 |

## 异常与清理

- 资源配置过满可能导致 Dataset 执行等待；原脚本注释明确说将并发 trial 改为 4 可能出现 hang，但该现象未在本环境验证。[`key_concepts.py:14-16`]
- trainable 或 Data read/task 失败沿 Trial/Data 错误边界传播。
- `fit()` 返回/失败后由 Tune controller 清理 trial；具体 actor 与 object 清理需动态 trace。

## 源码映射

- M01/M02/M04：`@ray` task、CoreWorker、资源/lease。
- M08：`python/ray/data/dataset.py:270-8709`、`read_api.py:678-828`、`streaming_executor.py:107-740`。
- M10：`python/ray/tune/tuner.py:43-425`、`tune_controller.py:72-2034`、`trial.py:216-1081`。
- M06：`python/ray/_private/worker.py:1439-1505`。

## 运行状态

没有安装依赖、启动 Ray 集群或运行脚本；任何 stdout、耗时、调度顺序和清理日志均为未验证。`demo-registry.md` 只登记源码来源，不代表 CI 成功。
