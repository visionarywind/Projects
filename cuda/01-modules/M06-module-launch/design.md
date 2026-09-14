# M06 设计

M06 将 module/function 的持久镜像状态与每次 launch 的暂态 `CUIlaunchData` 分开。普通 launch 可以立即生成异步提交；capture 则只建立 graph node，后续实例化负责把必要 setup 保存为持久或 node data。

参数 metadata 是 compiler 与 driver ABI 的连接点；HAL 只消费已验证的通用 launch data。
