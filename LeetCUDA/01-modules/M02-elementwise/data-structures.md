# M02 数据结构与生命周期

输入输出 tensor 由 Python/PyTorch 管理，kernel 借用 data pointer；pack kernel 在寄存器中保存 `float4`/half pack 临时值，写回调用者提供的 `c`。不应把 pack 临时对象或裸指针跨异步调用保存。
