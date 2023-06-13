# ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    std::vector<int> v{3, 1, 2, 4, 5};
    std::vector<int> res = Solution().quickSort(v);

    std::vector<int> cor{1, 2, 3, 4, 5};
    if (res.size() != cor.size())
    {
        std::cout << "用例1未通过！" << std::endl;
        return;
    }
    for (int i = 0; i < cor.size(); i++)
        if (cor[i] != res[i])
        {
            std::cout << "用例1未通过！" << std::endl;
            return;
        }

    std::cout << "用例1通过！" << std::endl;
}

void Test2()
{
    std::vector<int> v{7, 3, 4, 1, 6, 1, 2};
    std::vector<int> res = Solution().quickSort(v);

    std::vector<int> cor{1, 1, 2, 3, 4, 6, 7};
    if (res.size() != cor.size())
    {
        std::cout << "用例2未通过！" << std::endl;
        return;
    }
    for (int i = 0; i < cor.size(); i++)
        if (cor[i] != res[i])
        {
            std::cout << "用例2未通过！" << std::endl;
            return;
        }

    std::cout << "用例2通过！" << std::endl;
}

int main()
{
    Test1();
    Test2();

    return 0;
}