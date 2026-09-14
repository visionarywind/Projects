# M09 接口契约

- `clGetPlatformIDs`：初始化 loader、清零输出、枚举 vendor；无 vendor 返回 `CL_PLATFORM_NOT_FOUND_KHR`。
- `khrIcdVendorAdd`：library→extension address→platform IDs→suffix→vendor list。
- `cliGlobalsInitialize`：先 `cuiInit(CUI_API_OPENCL_NV)`，再 mutex 下创建配置、dispatch 和 platform。
- `CLIicdDispatchTable`：按固定版本/平台顺序存储函数指针，非支持入口用 NULL 占位。
- GL/D3D acquire/release：需额外验证 resource ownership 和同步。
