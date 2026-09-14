# M05 接口契约

- `cuiStreamCreate`：成功返回带 public handle 的 active stream；失败不得泄露 QMD/semaphore/UVM registration。
- `streamBeginPushWithFlags`：取得 channel/push state；调用者必须配对 `streamEndPush`。
- `channelInit`：成功后 channel 才进入 manager；失败负责 backend/semaphore/CV cleanup。
- `ctxMarkerGetStatus`：查询完成状态；`COMPLETED_BY_GPU` 才允许 detached reclaim。
- QMD 轮换：每次 launch 取得当前 index 并递增取模，不可绕过 pool 状态。
