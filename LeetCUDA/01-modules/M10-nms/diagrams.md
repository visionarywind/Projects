# M10 图示

```mermaid
flowchart LR
  I[boxes,scores] --> O[stable score order]
  O --> M[warp IoU bitmask]
  M --> R[one-block ordered resolve]
  R --> X[original indices]
```

证据：`[kernels/nms/nms.cu:126-189]`。
