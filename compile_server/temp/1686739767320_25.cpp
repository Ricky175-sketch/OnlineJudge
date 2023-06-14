#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class Solution {
public:
    bool isPalindrome(int x) {
        // 请将你的代码写在这里

        return false;
    }
};
# ifndef COMPILER_ONLINE
#include "header.cpp"
# endif

void Test1()
{
    bool ret = Solution().isPalindrome(121);
    if (ret == true)
        std::cout << "用例1通过！" << std::endl;
    else
        std::cout << "用例1未通过！" << std::endl;
}

void Test2()
{
    bool ret = Solution().isPalindrome(-10);
    if (ret == false)
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
