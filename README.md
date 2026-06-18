# SJTU Ticket System

基于 B+ 树文件存储的火车票订票系统，模拟 12306 核心业务逻辑。支持用户管理、车次发布、余票查询、购票、候补购票、订单管理等完整功能。

## 主要功能

- **用户管理**：注册、登录、登出、查询/修改个人信息（权限分级）
- **车次管理**：添加、删除、发布车次；查询车次时刻表（含余票）
- **车票查询**：按时间或价格排序查询两地之间的车票（支持直达和中转）
- **购票与候补**：余票充足时直接购买，不足时可加入候补队列
- **订单管理**：查询历史订单（按时间倒序），支持退票（触发候补队列处理）
- **数据持久化**：所有数据存储于本地文件，支持 `clean` 清空所有数据

## 编译与运行

### 依赖
- C++17 编译器（GCC 7+ 或 Clang 5+）
- CMake 3.10+
- 无第三方库依赖（仅使用标准库，允许 `std::string`）

### 编译
```bash
git clone <repository>
cd SJTU-TicketSystem
mkdir build && cd build
cmake ..
make -j
```

生成可执行文件 `code`。

### 运行
程序通过标准输入输出交互，每行一条指令（格式见[需求文档](docs/要求.md)）。  
示例（使用测试数据）：

```bash
./code < test_data.txt > output.txt
```

指令格式：
```
[<timestamp>] <cmd> -<key1> <value1> -<key2> <value2> ...
```
输出第一行会包含 `[<timestamp>]` 回显。

### 清理数据
执行 `clean` 指令将删除所有用户、车次、订单数据，并重置文件。

## 文件结构

```
include/                     # 所有头文件和实现
├── BPlusTree.hpp            # B+ 树模板（节点、缓存、树）
├── BPlusTree_implement.hpp  # B+ 树模板实现
├── FileOperator.hpp         # 文件操作封装（使用 pread/pwrite）
├── FileOperator_implement.hpp
├── Map.hpp                  # 红黑树容器（用于登录列表等）
├── Vector.hpp               # 动态数组
├── Utilities.hpp            # 工具函数（日期/时间/排序/解析）
├── User.hpp / _implement    # 用户数据及操作
├── Train.hpp / _implement   # 车次数据及操作
├── Order.hpp / _implement   # 订单数据及操作
└── exceptions.hpp           # 异常定义
src/
└── code.cpp                 # 主程序入口
CMakeLists.txt
要求.md                       # 详细指令说明
README.md                    # 本文件
```

## 性能优化

本项目针对 25 MiB 内存限制进行了多项优化：

1. **差异化 B+ 树节点容量**：为大对象（`Train`）设置小节点（`N=8`），为小对象（`StationTimeTrain`）设置大节点（`N=60`），平衡内存与 I/O。
2. **内存缓存**：每个 B+ 树使用独立的 LRU 缓存（`MemoryRiver`），热点节点驻留内存，减少磁盘读。
3. **直接 I/O**：使用 `pread`/`pwrite` 系统调用替代 `fstream`，避免额外的 `lseek` 系统调用。
4. **批量内存操作**：节点内元素移动改用 `memmove`，节点构造用 `memset` 初始化。
5. **减少大对象拷贝**：提供 `findOne` 方法，直接返回单条记录，避免 `vector` 构造和拷贝。

## 注意事项

- 程序运行时会创建多个数据文件（`.dat`），请确保所在目录有写权限。
- 内存限制 25 MiB，若需调整 B+ 树参数（`N`、`M`），请重新计算缓存占用。
- WSL 环境下 `perf` 不可用，可使用 `valgrind` 或 `strace` 进行性能分析。
- 所有时间日期均为 2026 年 6~8 月，格式固定。

## 贡献与许可

本作业为上海交通大学《数据结构》课程项目，仅限课程使用。  
如有问题，请联系课程助教。
