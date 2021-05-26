#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// 关键字表置初始值
string keyword[30] = {"#", "begin", "if", "then", "while", "do", "end", "", "", "",
                      "letter(letter|digit)*", "digitdigit*", "", "+", "-", "*", "/",
                      ":", ":=", "", "<", "<>", "<=", ">", ">=", "=", ";", "(", ")"};
string Cppkeyword[100] = {"#", "标识符", "整数", "实数", "字符常量", "+", "-", "*", "/", "<",
                          "<=", "==", "!=", ">", ">=", "&", "&&", "||", "=", "(",
                          ")", "[", "]", "{", "}", ":", ";", ",", "void", "int",
                          "float", "char", "if", "else", "while", "do", "for"};
class word
{
public:
    int syn{};
    string token;
};

// 处理单词的函数
word letterAnalysis(const string &subCode)
{
    word item;
    if (subCode.substr(0, 5) == "begin")
    {
        item.syn = 1;
    }
    else if (subCode.substr(0, 2) == "if")
    {
        item.syn = 2;
    }
    else if (subCode.substr(0, 4) == "then")
    {
        item.syn = 3;
    }
    else if (subCode.substr(0, 5) == "while")
    {
        item.syn = 4;
    }
    else if (subCode.substr(0, 2) == "do")
    {
        item.syn = 5;
    }
    else if (subCode.substr(0, 3) == "end")
    {
        item.syn = 6;
    }
    else
    {
        // 如果是其它单词，找到第一个不是字母的位置，截取之前的单词，置于10号位
        for (int i = 0; i < subCode.length(); ++i)
        {
            if (!(subCode[i] >= 'a' && subCode[i] <= 'z'))
            {
                item.syn = 10; //10号位固定非字符
                keyword[item.syn] = subCode.substr(0, i);
                break;
            }
        }
    }
    item.token = keyword[item.syn];
    return item;
}

// 处理数字的函数
word numberAnalysis(string subCode)
{
    word item;
    item.syn = 11;
    for (int i = 0; i < subCode.length(); ++i)
    {
        // 截取到第一个非数字字符
        if (!(subCode[i] >= '0' && subCode[i] <= '9'))
        {
            keyword[item.syn] = subCode.substr(0, i); //11号位固定数字?
            break;
        }
    }
    item.token = keyword[item.syn];
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
        item.syn = 13;
        break;
    case '-':
        item.syn = 14;
        break;
    case '*':
        item.syn = 15;
        break;
    case '/':
        item.syn = 16;
        break;
    case ':':
        if (subCode[1] == '=')
        {
            item.syn = 18;
        }
        else
        {
            item.syn = 17;
        }
        break;
    case '<':
        if (subCode[1] == '>')
        {
            item.syn = 21;
        }
        else if (subCode[1] == '=')
        {
            item.syn = 22;
        }
        else
        {
            item.syn = 20;
        }
        break;
    case '>':
        if (subCode[1] == '=')
        {
            item.syn = 24;
        }
        else
        {
            item.syn = 23;
        }
        break;
    case '=':
        item.syn = 25;
        break;
    case ';':
        item.syn = 26;
        break;
    case '(':
        item.syn = 27;
        break;
    case ')':
        item.syn = 28;
        break;
        /*default:
        item.syn = 0;
        break;*/
    }
    item.token = keyword[item.syn];
    return item;
}

// 词法分析
word *scanner(const string &code)
{ //if a=1;
    word lexicalTable[1000];
    int tableLen = 0;
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
        lexicalTable[tableLen++] = item;
        cout << "(" << item.syn << "," << item.token << ")" << endl;
    }
    return lexicalTable;
}

int main()
{
    string code;
    ifstream myfile("data.txt");
    if (!myfile.is_open())
    {
        cout << "未成功打开文件" << endl;
    }

    while (getline(myfile, code)) //按行读取文件，可读取空格
    {
        // cout << code << endl;
        scanner(code);
    }

    return 0;
}
