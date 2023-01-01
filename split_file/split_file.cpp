#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>

namespace fs = std::filesystem;

/// <summary>
/// 分而治之/哈希映射
/// </summary>
/// <param name="in_file">输入文件</param>
/// <param name="num">文件分割数量</param>
/// <param name="out_path">输出路径</param>
void split_file(std::string in_file, int num, std::string out_path = "out")
{
    system("chcp 65001");//解决cout输出中文乱码

    std::ifstream fin(in_file, std::ios::in);
    if (!fin)
    {
        std::cout << "文件打开失败\n";
        return;
    }

    if (not fs::exists(out_path))
    {
        fs::create_directories(out_path);
    }

    std::hash<int> h;//初始化hash

    std::vector<std::ofstream> file_list;
    for (int i = 0; i < num; i++)
    {
        std::ofstream fout(out_path + "/" + std::to_string(i) + ".txt");
        file_list.push_back(std::move(fout));//在C++11中，具体的流对象是可移动的。但是，要利用这一点，需要传递临时对象或允许移动对象。
    }

    while (!fin.eof()) {
        std::string line;
        std::getline(fin, line);//按行读取文件
        auto m = h(std::stoi(line)) % num;//求每行内容的哈希值的模
        file_list[m] << line << "\n";
    }

    //关闭所有文件对象
    for (auto& f : file_list)
    {
        f.close();
    }

    fin.close();
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser parser("split_file");

    parser.add_argument("in_file")
        .help("输入文件");
    parser.add_argument("-n", "--num")
        .help("文件分割数量")
        .default_value(100)
        .scan<'i', int>();
    parser.add_argument("-o", "--out_path")
        .help("输出路径")
        .default_value("out");

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    split_file(parser.get("in_file"), parser.get<int>("num"), parser.get("out_path"));
}
