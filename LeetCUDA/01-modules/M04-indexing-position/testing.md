# M04 测试

- embedding：空索引、重复索引、边界/越界索引、不同 embedding width。
- histogram：空输入、负值/最大 bin、重复写入，并用 racecheck。
- transpose：方阵、非方阵、非 tile 倍数和连续性。
- RoPE：偶数维、不同位置、频率参数和 dtype。

使用 PyTorch 索引/transpose/数学 reference；命令当前未执行。
