# ifndef COMPILER_ONLINE
#include "header.cpp"
# endif

void Test1()
{
    bool ret = Solution().isPalindrome(121);
    if (ret)
        std::cout << "用例1通过！" << std::endl;
    else
        std::cout << "用例1未通过！" << std::endl;
}

void Test2()
{
    bool ret = Solution().isPalindrome(-10);
    if (!ret)
        std::cout << "用例2通过！" << std::endl;
    else
        std::cout << "用例2未通过！" << std::endl;
}

int main()
{
    Test1();
    Test2();

    return 0;
}