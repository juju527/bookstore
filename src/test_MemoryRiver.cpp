#include "MemoryRiver.hpp"  // 包含你的 MemoryRiver 类头文件
#include <cassert>
#include <cstdio>   // 用于删除测试文件
#include <string>
#include <cstring>

// 测试用结构体（满足 sizeof(T) >= sizeof(int)，可二进制拷贝）
struct TestData {
    int id;         // 唯一标识，方便验证
    char name[20];  // 固定大小字符数组（避免 std::string 动态内存问题）
    double score;   // 数值类型，验证精度

    // 构造函数：方便初始化
    TestData() : id(0), score(0.0) {
        memset(name, 0, sizeof(name));  // 清空字符串
    }

    TestData(int _id, const char* _name, double _score) : id(_id), score(_score) {
        strncpy(name, _name, sizeof(name)-1);  // 安全拷贝字符串
        name[sizeof(name)-1] = '\0';           // 确保字符串结束符
    }

    // 重载 == 运算符：方便对比对象是否一致
    bool operator==(const TestData& other) const {
        return id == other.id 
            && strcmp(name, other.name) == 0 
            && score == other.score;
    }

    // 辅助打印：方便调试
    friend std::ostream& operator<<(std::ostream& os, const TestData& data) {
        os << "id: " << data.id 
           << ", name: " << data.name 
           << ", score: " << data.score;
        return os;
    }
};

// 测试工具函数：检查文件是否存在
bool fileExists(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    return file.is_open();
}

// 测试用例：初始化测试
void testInitialise() {
    std::cout << "=== 测试初始化功能 ===" << std::endl;
    const std::string filename = "test_initialise.dat";

    // 确保测试文件不存在（避免残留）
    std::remove(filename.c_str());

    MemoryRiver<TestData> mr(filename);
    mr.initialise();

    // 验证文件创建成功
    assert(fileExists(filename) && "初始化失败：文件未创建");
    std::cout << "文件创建成功" << std::endl;

    // 验证 info 区初始值为 0（默认 info_len=2，两个 int 都为 0）
    int info1 = -1, info2 = -1;
    mr.get_info(info1, 1);  // 第1个info（空闲链表头）
    mr.get_info(info2, 2);  // 第2个info（预留）
    assert(info1 == 0 && "初始化失败：info[1] 不为 0");
    assert(info2 == 0 && "初始化失败：info[2] 不为 0");
    std::cout << "info 区初始值验证成功" << std::endl;

    std::remove(filename.c_str());
    std::cout << "初始化测试通过！\n" << std::endl;
}

// 测试用例：info 读写功能
void testInfoReadWrite() {
    std::cout << "=== 测试 info 读写功能 ===" << std::endl;
    const std::string filename = "test_info.dat";
    std::remove(filename.c_str());

    MemoryRiver<TestData> mr(filename);
    mr.initialise();

    // 写入 info
    int write_val1 = 100, write_val2 = 200;
    mr.write_info(write_val1, 1);
    mr.write_info(write_val2, 2);

    // 读取 info 并验证
    int read_val1 = -1, read_val2 = -1;
    mr.get_info(read_val1, 1);
    mr.get_info(read_val2, 2);
    assert(read_val1 == write_val1 && "info[1] 读写不一致");
    assert(read_val2 == write_val2 && "info[2] 读写不一致");
    std::cout << "info[1] 写入：" << write_val1 << "，读取：" << read_val1 << std::endl;
    std::cout << "info[2] 写入：" << write_val2 << "，读取：" << read_val2 << std::endl;

    std::remove(filename.c_str());
    std::cout << "info 读写测试通过！\n" << std::endl;
}

// 测试用例：基本 write + read + update 功能
void testBasicWriteReadUpdate() {
    std::cout << "=== 测试基本 write/read/update 功能 ===" << std::endl;
    const std::string filename = "test_basic.dat";
    std::remove(filename.c_str());

    MemoryRiver<TestData> mr(filename);
    mr.initialise();

    // 写入对象
    TestData t1(1, "Alice", 95.5), t2(2, "Bob", 88.0);
    int idx1 = mr.write(t1);
    int idx2 = mr.write(t2);
    std::cout << "写入 t1，index：" << idx1 << std::endl;
    std::cout << "写入 t2，index：" << idx2 << std::endl;

    // 验证 index 合理性（info 区占 2*4=8 字节，第一个对象从 8 开始，第二个从 8+sizeof(TestData) 开始）
    assert(idx1 == 8 && "t1 的 index 不符合预期");
    assert(idx2 == 8 + sizeof(TestData) && "t2 的 index 不符合预期");

    // 读取对象并验证
    TestData read_t1, read_t2;
    mr.read(read_t1, idx1);
    mr.read(read_t2, idx2);
    assert(read_t1 == t1 && "t1 读取数据不一致");
    assert(read_t2 == t2 && "t2 读取数据不一致");
    std::cout << "读取 t1：" << read_t1 << std::endl;
    std::cout << "读取 t2：" << read_t2 << std::endl;

    // 更新对象
    TestData t1_update(1, "Alice_Updated", 98.0);
    mr.update(t1_update, idx1);
    mr.read(read_t1, idx1);
    assert(read_t1 == t1_update && "t1 更新失败");
    std::cout << "更新后 t1：" << read_t1 << std::endl;

    std::remove(filename.c_str());
    std::cout << "基本 write/read/update 测试通过！\n" << std::endl;
}

// 测试用例：空间回收（Delete + 空闲块复用）
void testSpaceRecycling() {
    std::cout << "=== 测试空间回收（Delete + 复用）功能 ===" << std::endl;
    const std::string filename = "test_recycle.dat";
    std::remove(filename.c_str());

    MemoryRiver<TestData> mr(filename);
    mr.initialise();

    // 第一步：写入 3 个对象，记录 index
    TestData t1(1, "A", 90.0), t2(2, "B", 80.0), t3(3, "C", 70.0);
    int idx1 = mr.write(t1);
    int idx2 = mr.write(t2);
    int idx3 = mr.write(t3);
    std::cout << "初始写入：idx1=" << idx1 << ", idx2=" << idx2 << ", idx3=" << idx3 << std::endl;
    assert(idx1 == 8 && idx2 == 8+sizeof(TestData) && idx3 == 8+2*sizeof(TestData) && "初始 index 异常");

    // 第二步：删除 idx1 和 idx3，加入空闲链表
    mr.Delete(idx1);
    mr.Delete(idx3);
    std::cout << "删除 idx1 和 idx3" << std::endl;

    // 验证空闲链表头（info[1]）是否为 idx3（头插法，最后删除的 idx3 成为链表头）
    int free_head = -1;
    mr.get_info(free_head, 1);
    std::cerr<<free_head<<" "<<idx3<<std::endl;
    assert(free_head == idx3 && "空闲链表头错误（应指向最后删除的 idx3）");
    std::cout << "空闲链表头：" << free_head << "（预期：" << idx3 << "）" << std::endl;

    // 第三步：写入新对象 t4，应复用空闲链表头（idx3）
    TestData t4(4, "D", 60.0);
    int idx4 = mr.write(t4);
    std::cout << "写入 t4，index：" << idx4 << "（预期：" << idx3 << "）" << std::endl;
    assert(idx4 == idx3 && "未复用 idx3");

    // 验证 t4 数据正确
    TestData read_t4;
    mr.read(read_t4, idx4);
    std::cerr<<read_t4<<std::endl;
    assert(read_t4 == t4 && "复用块读取数据错误");
    std::cout << "复用 idx3 读取 t4：" << read_t4 << std::endl;

    // 第四步：再次写入 t5，应复用下一个空闲块（idx1）
    TestData t5(5, "E", 50.0);
    int idx5 = mr.write(t5);
    std::cout << "写入 t5，index：" << idx5 << "（预期：" << idx1 << "）" << std::endl;
    assert(idx5 == idx1 && "未复用 idx1");

    // 验证 t5 数据正确
    TestData read_t5;
    mr.read(read_t5, idx5);
    assert(read_t5 == t5 && "复用块读取数据错误");
    std::cout << "复用 idx1 读取 t5：" << read_t5 << std::endl;

    // 第五步：无空闲块时，写入 t6 应追加到末尾
    TestData t6(6, "F", 40.0);
    int idx6 = mr.write(t6);
    int expected_idx6 = 8 + 3*sizeof(TestData);  // 原 idx3 已复用，新末尾是 8+3*sizeof(TestData)
    std::cout << "写入 t6，index：" << idx6 << "（预期：" << expected_idx6 << "）" << std::endl;
    assert(idx6 == expected_idx6 && "无空闲块时未追加到末尾");

    std::remove(filename.c_str());
    std::cout << "空间回收测试通过！\n" << std::endl;
}

// 测试用例：边界情况（无效 index 处理）
void testEdgeCases() {
    std::cout << "=== 测试边界情况 ===" << std::endl;
    const std::string filename = "test_edge.dat";
    std::remove(filename.c_str());

    MemoryRiver<TestData> mr(filename);
    mr.initialise();

    // 测试 1：读取/更新/删除 无效 index（0、负数、info 区地址）
    TestData t(1, "Test", 90.0);
    mr.read(t, 0);    // 无效 index，无崩溃
    mr.update(t, -5); // 无效 index，无崩溃
    mr.Delete(4);     // info 区地址（4 字节，小于 8），无崩溃
    std::cout << "无效 index 操作无崩溃" << std::endl;

    // 测试 2：info 区超出范围写入
    mr.write_info(100, 3);  // info_len=2，写入 n=3 无效，无崩溃
    int info3 = -1;
    mr.get_info(info3, 3);  // 读取无效 info，值不变
    assert(info3 == -1 && "超出 info_len 写入未被忽略");
    std::cout << "超出 info_len 操作被正确忽略" << std::endl;

    std::remove(filename.c_str());
    std::cout << "边界情况测试通过！\n" << std::endl;
}

int main() {
    try {
        testInitialise();
        testInfoReadWrite();
        testBasicWriteReadUpdate();
        testSpaceRecycling();  // 核心测试
        testEdgeCases();

        std::cout << "========================" << std::endl;
        std::cout << "所有测试用例全部通过！" << std::endl;
        std::cout << "========================" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "测试失败：" << e.what() << std::endl;
        return 1;
    }

    return 0;
}