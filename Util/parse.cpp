#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;

// 关键字表置初始值
string Cppkeyword[100] = {"#", "标识符(变量名)", "整数", "实数", "字符常量", "+", "-", "*", "/", "<",
                          "<=", "==", "!=", ">", ">=", "&", "&&", "||", "=", "(",
                          ")", "[", "]", "{", "}", ":", ";", ",", "void", "int",
                          "float", "char", "if", "else", "while", "do", "for", "include", "iostream", "using",
                          "namespace", "std", "main", "return", "null"};
class word
{
public:
    int syn{};
    string token;
};
//存储词法分析结果
word lexicalTable[1000];
int lexicalTableLen = 0;

// 处理关键词和变量的函数
word letterAnalysis(const string &subCode)
{
    word item;
    int isKeyword = 0;
    for (int i = 28; i <= 43; i++)
    {
        if (subCode.substr(0, Cppkeyword[i].length()) == Cppkeyword[i])
        {
            item.syn = i;
            isKeyword = 1;
        }
    }
    /* 不用上述for循环的话就得如下一一列举，如果关键词数目较多，就要写很多低级代码
    if (subCode.substr(0, 4) == "void")
    {
        item.syn = 28;
    }
    else if (subCode.substr(0, 3) == "int")
    {
        item.syn = 29;
    }
    else if (subCode.substr(0, 5) == "float")
*/
    if (isKeyword == 0)
    {
        // 如果不是上述关键词，一律视为变量名
        for (int i = 0; i <= subCode.length(); ++i)
        { //找到第一个不是数字、字母、下划线的位置
            if (!(subCode[i] >= 'a' && subCode[i] <= 'z' || subCode[i] >= '0' && subCode[i] <= '9' || subCode[i] == '_'))
            {
                item.syn = 1; //1号位存储变量名
                Cppkeyword[item.syn] = subCode.substr(0, i);
                break;
            }
        }
    }
    item.token = Cppkeyword[item.syn];
    return item;
}

// 处理数字的函数
word numberAnalysis(string subCode)
{
    word item;
    for (int i = 0; i <= subCode.length(); ++i)
    {
        // 截取到第一个非数字和小数点字符
        if (!(subCode[i] >= '0' && subCode[i] <= '9' || subCode[i] == '.'))
        {
            string curNum = subCode.substr(0, i);
            if (curNum.find('.') == string::npos) //没读到'.'返回值为很大的数，若读到返回值是第一次出现的下标
                item.syn = 2;                     //2号位存整数
            else
                item.syn = 3; //3号位存实数

            Cppkeyword[item.syn] = curNum;
            break;
        }
    }
    item.token = Cppkeyword[item.syn];
    return item;
}

// 处理字符常量的函数
word strAnalysis(string subCode)
{ //"hello"<<endl;
    word item;
    int nextindex = subCode.find_first_of('"', 1); //找到第二个"出现的位置下标
    item.syn = 4;                                  //字符常量置为4
    Cppkeyword[item.syn] = subCode.substr(0, nextindex + 1);

    item.token = Cppkeyword[item.syn];
    return item;
}

// 处理字符的函数
word charAnalysis(string subCode)
{
    word item;
    switch (subCode[0])
    {
    case '#':
        item.syn = 0;
        break;
    case '+':
        item.syn = 5;
        break;
    case '-':
        item.syn = 6;
        break;
    case '*':
        item.syn = 7;
        break;
    case '/':
        item.syn = 8;
        break;
    case '<':
        if (subCode[1] == '=')
        {
            item.syn = 10;
        }
        else
        {
            item.syn = 9;
        }
        break;
    case '=':
        if (subCode[1] == '=')
        {
            item.syn = 11;
        }
        else
        {
            item.syn = 18;
        }
        break;
    case '!':
        if (subCode[1] == '=')
        {
            item.syn = 12;
        }
        break;

    case '>':
        if (subCode[1] == '=')
        {
            item.syn = 14;
        }
        else
        {
            item.syn = 13;
        }
        break;
    case '&':
        if (subCode[1] == '&')
        {
            item.syn = 16;
        }
        else
        {
            item.syn = 15;
        }
        break;
    case '|':
        if (subCode[1] == '|')
        {
            item.syn = 17;
        }
        break;

    case '(':
        item.syn = 19;
        break;
    case ')':
        item.syn = 20;
        break;
    case '[':
        item.syn = 21;
        break;
    case ']':
        item.syn = 22;
        break;
    case '{':
        item.syn = 23;
        break;
    case '}':
        item.syn = 24;
        break;
    case ':':
        item.syn = 25;
        break;
    case ';':
        item.syn = 26;
        break;
    case ',':
        item.syn = 27;
        break;
    }
    item.token = Cppkeyword[item.syn];
    return item;
}

// 词法分析
void scanner(const string &code)
{ //if a=1;

    for (int i = 0; i < code.length(); ++i)
    {
        word item;
        if (code[i] >= 'a' && code[i] <= 'z')
        {
            // 处理单词,假设句子是 if a=1;进行单词分析后返回“if”,i后移了两位，这点在该函数最后有做处理
            item = letterAnalysis(code.substr(i, code.length() - i + 1));
        }
        else if (code[i] >= '0' and code[i] <= '9')
        {
            // 处理数字
            item = numberAnalysis(code.substr(i, code.length() - i + 1));
        }
        else if (code[i] == ' ')
        {
            // 如果是空格，直接跳过
            continue;
        }
        else
        {
            // 处理特殊符号
            item = charAnalysis(code.substr(i, code.length() - i + 1));
        }
        i += int(item.token.length()) - 1;
        lexicalTable[lexicalTableLen++] = item; //词法处理完的结果存入lexicalTable中
        cout << "(" << item.syn << "," << item.token << ")" << endl;
    }
}

