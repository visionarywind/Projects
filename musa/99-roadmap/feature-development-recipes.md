# 功能开发配方

## 新增 API

1. 定义公共声明和 ABI 版本策略。
2. 在 `src/driver/mu_*.cpp` 做参数/context/句柄检查。
3. 调用已有 Context/Device/Stream 对象方法。
4. 更新 accessors/export symbols。
5. 添加静态失败路径和目标机回归。

## 新增资源类型

先确定所有者、tracker 索引、同步完成条件、HAL 表示、销毁顺序，再实现正常路径。

## 修改异步行为

同时审查 `ResolveDependencyAndQueueCommand`、Stream command lists、submit/wait threads、MUPTI/GDB observability；不能只改 API 返回点。
