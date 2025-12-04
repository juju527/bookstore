#include "MemoryRiver.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>

// ========== 测试用类型定义 ==========
// 测试1：基础类型（int）
using TInt = int;

// 测试2：复杂结构体（包含多种POD类型）
struct ComplexData {
    int id;
    double score;
    char name[20];  // 用固定数组避免string的二进制读写问题
    bool is_valid;

    // 重载==便于断言比较
    bool operator==(const ComplexData& other) const {
        return id == other.id && 
               score == other.score && 
               std::string(name) == std::string(other.name) && 
               is_valid == other.is_valid;
    }
};

// 测试3：嵌套结构体
struct NestedData {
    int group_id;
    ComplexData item;
    int count;

    bool operator==(const NestedData& other) const {
        return group_id == other.group_id && 
               item == other.item && 
               count == other.count;
    }
};

// ========== 测试工具函数 ==========
// 检查文件是否存在
bool file_exists(const std::string& filename) {
    std::ifstream f(filename);
    return f.good();
}

// 删除测试文件
void cleanup(const std::string& filename) {
    if (file_exists(filename)) {
        std::remove(filename.c_str());
        std::cout << "[清理] 已删除测试文件: " << filename << std::endl;
    }
}

// ========== 具体测试用例 ==========
// 测试1：基础类型（int）的读写与更新
void test_basic_int() {
    std::cout << "\n===== 测试1：基础类型（int）操作 =====" << std::endl;
    const std::string filename = "test_basic_int.bin";
    cleanup(filename);

    // 初始化：info_len=3（头部3个int）
    MemoryRiver<TInt, 3> mr(filename);
    mr.initialise();

    // 测试头部信息读写
    int info1 = 123, info2 = 456, info3 = 789;
    mr.write_info(info1, 1);
    mr.write_info(info2, 2);
    mr.write_info(info3, 3);

    int read_info1, read_info2, read_info3;
    mr.get_info(read_info1, 1);
    mr.get_info(read_info2, 2);
    mr.get_info(read_info3, 3);
    std::cerr<<read_info3<<" "<<info3<<std::endl;
    assert(read_info1 == info1 && "头部信息1读取错误");
    assert(read_info2 == info2 && "头部信息2读取错误");
    assert(read_info3 == info3 && "头部信息3读取错误");
    std::cout << "✓ 头部信息读写通过" << std::endl;

    // 测试写入、读取、更新int
    std::vector<TInt> values = {10, 20, 30, 40, 50};
    std::vector<int> indexes;

    // 批量写入
    for (auto& val : values) {
        int idx = mr.write(val);
        indexes.push_back(idx);
        std::cout << "写入值 " << val << "，索引: " << idx << std::endl;
    }

    // 批量读取验证
    for (int i = 0; i < values.size(); ++i) {
        TInt read_val;
        mr.read(read_val, indexes[i]);
        assert(read_val == values[i] && "基础类型读取错误");
    }
    std::cout << "✓ 批量写入与读取通过" << std::endl;

    // 测试更新
    int update_idx = indexes[2];  // 第三个元素（30）
    TInt new_val = 300;
    mr.update(new_val, update_idx);
    TInt updated_val;
    mr.read(updated_val, update_idx);
    assert(updated_val == new_val && "基础类型更新错误");
    std::cout << "✓ 基础类型更新通过" << std::endl;

    cleanup(filename);
}

// 测试2：复杂结构体的操作
void test_complex_struct() {
    std::cout << "\n===== 测试2：复杂结构体操作 =====" << std::endl;
    const std::string filename = "test_complex_struct.bin";
    cleanup(filename);

    MemoryRiver<ComplexData, 2> mr(filename);
    mr.initialise();

    // 构造测试数据
    ComplexData data1 = {1, 95.5, "Alice", true};
    ComplexData data2 = {2, 88.0, "Bob", false};
    ComplexData data3 = {3, 99.9, "Charlie", true};

    // 写入
    int idx1 = mr.write(data1);
    int idx2 = mr.write(data2);
    int idx3 = mr.write(data3);
    std::cout << "写入结构体1，索引: " << idx1 << std::endl;
    std::cout << "写入结构体2，索引: " << idx2 << std::endl;
    std::cout << "写入结构体3，索引: " << idx3 << std::endl;

    // 读取验证
    ComplexData read_data1, read_data2, read_data3;
    mr.read(read_data1, idx1);
    mr.read(read_data2, idx2);
    mr.read(read_data3, idx3);
    assert(read_data1 == data1 && "复杂结构体1读取错误");
    assert(read_data2 == data2 && "复杂结构体2读取错误");
    assert(read_data3 == data3 && "复杂结构体3读取错误");
    std::cout << "✓ 复杂结构体读取通过" << std::endl;

    // 更新测试
    ComplexData new_data2 = {2, 92.5, "Bob Updated", true};
    mr.update(new_data2, idx2);
    ComplexData updated_data2;
    mr.read(updated_data2, idx2);
    assert(updated_data2 == new_data2 && "复杂结构体更新错误");
    std::cout << "✓ 复杂结构体更新通过" << std::endl;

    // 删除测试（覆盖验证）
    /*mr.Delete(idx3);
    ComplexData deleted_data3;
    mr.read(deleted_data3, idx3);
    // 验证是否被默认值覆盖（根据Delete实现）
    assert(deleted_data3.id == 0 && deleted_data3.score == 0.0 && "Delete操作未生效");
    std::cout << "✓ Delete操作通过" << std::endl;
    */
    cleanup(filename);
}

// 测试3：持久化验证（关闭后重新读取）
void test_persistence() {
    std::cout << "\n===== 测试3：文件持久化验证 =====" << std::endl;
    const std::string filename = "test_persistence.bin";
    cleanup(filename);

    // 第一步：写入数据并关闭
    {
        MemoryRiver<NestedData, 1> mr(filename);
        mr.initialise();

        NestedData nested_data = {
            100,
            {4, 85.0, "David", true},
            5
        };
        int idx = mr.write(nested_data);
        std::cout << "写入嵌套结构体，索引: " << idx << std::endl;

        // 写入头部信息
        mr.write_info(999, 1);
    }  // 作用域结束，mr析构（文件关闭）

    // 第二步：重新打开并读取
    {
        MemoryRiver<NestedData, 1> mr(filename);
        // 读取头部信息
        int head_val;
        mr.get_info(head_val, 1);
        assert(head_val == 999 && "持久化头部信息读取错误");
        std::cout << "✓ 持久化头部信息读取通过" << std::endl;

        // 读取嵌套结构体
        NestedData read_nested;
        int idx = sizeof(int) * 1;  // 头部1个int后的第一个位置（可通过之前的idx记录，这里直接计算）
        mr.read(read_nested, idx);

        NestedData expected = {
            100,
            {4, 85.0, "David", true},
            5
        };
        assert(read_nested == expected && "持久化结构体读取错误");
        std::cout << "✓ 持久化结构体读取通过" << std::endl;
    }

    cleanup(filename);
}

// 测试4：边界情况测试（空文件、索引越界等）
void test_edge_cases() {
    std::cout << "\n===== 测试4：边界情况 =====" << std::endl;
    const std::string filename = "test_edge.bin";
    cleanup(filename);

    MemoryRiver<TInt, 2> mr(filename);
    mr.initialise();

    // 测试空文件读取（索引为0，头部第一个int）
    int head0;
    mr.get_info(head0, 1);
    assert(head0 == 0 && "初始化头部值错误");
    std::cout << "✓ 空文件头部初始化通过" << std::endl;

    // 测试写入第一个对象（头部后的第一个位置）
    TInt first_val = 1000;
    int first_idx = mr.write(first_val);
    assert(first_idx == sizeof(int)*2 && "第一个对象索引错误（头部2个int）");
    std::cout << "✓ 第一个对象索引计算通过" << std::endl;

    // 测试索引越界读取（这里用try-catch，若未处理越界则提示）
    try {
        TInt dummy;
        mr.read(dummy, 99999);  // 无效索引
        std::cout << "⚠ 索引越界未触发错误（可根据需求添加越界检查）" << std::endl;
    } catch (...) {
        std::cout << "✓ 索引越界捕获成功" << std::endl;
    }

    cleanup(filename);
}

// ========== 主函数 ==========
int main() {
    std::cout << "===== 开始MemoryRiver类模板综合测试 =====" << std::endl;

    try {
        test_basic_int();
        test_complex_struct();
        test_persistence();
        test_edge_cases();

        std::cout << "\n===== 所有测试通过！=====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ 测试失败：" << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ 测试失败：未知异常" << std::endl;
        return 1;
    }

    return 0;
}