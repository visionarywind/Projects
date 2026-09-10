# M10 调用链

```text
nms.py lib.nms -> dtype/device/rank checks
 -> scores.sort(stable descending)
 -> boxes.index_select.contiguous
 -> zero keep/mask
 -> IoU mask launch
 -> one-block resolve launch
 -> CPU keep/order copy
 -> original int64 index tensor on boxes.device
```

证据：`[kernels/nms/nms.cu:126-189]`。
