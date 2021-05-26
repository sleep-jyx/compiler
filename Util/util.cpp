#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
using namespace std;
// 关键字表置初始值
string Cppkeyword[100] = {"#", "标识符(变量名)", "整数", "实数", "字符常量", "+", "-", "*", "/", "<",
                          "<=", "==", "!=", ">", ">=", "&", "&&", "||", "=", "(",
                          ")", "[", "]", "{", "}", ":", ";", ",", "void", "int",
                          "float", "char", "if", "else", "while", "do", "for", "include", "iostream", "using",
                          "namespace", "std", "main", "return", "null"};

vector<string> grammar;          //存储文法
map<string, int> VN2int, VT2int; //VN、VT映射为下标索引
int symbolNum = 0;

string getVn(string grammar)
{ //获取文法中的非终结符
    if (grammar[1] == '\'')
    { //带'的非终结符,如 E',T'
        return grammar.substr(0, 2);
    }
    else
    { //不带'的正常非终结符，如E，T
        return grammar.substr(0, 1);
    }
}

string getVt(string grammar)
{ //获取文法中的终结符
    //Cppkeyword[1] = "i";
    for (int k = 0; k <= 44; k++)
    {
        string Vt = grammar.substr(0, Cppkeyword[k].length());
        if (Vt == Cppkeyword[k])
        {
            return Vt;
        }
    }
    //如果运行到这里，说明这个终结符不是关键词表里的，认为小写字母也属于终结符
    if (grammar[0] >= 'a' && grammar[0] <= 'z')
    {
        return grammar.substr(0, 1);
    }
}
void readVnAndVt()
{
    //扫描一个产生式，识别所有的非终结符和终结符
    for (int i = 0; i < grammar.size(); i++)
    {
        for (int j = 0; j < grammar[i].length(); j++)
        {
            if (grammar[i][j] >= 'A' && grammar[i][j] <= 'Z')
            { //非终结符一般大写
                string Vn = getVn(grammar[i].substr(j, 2));
                if (VN2int[Vn] == 0)
                    VN2int[Vn] = ++symbolNum;
                j = j + Vn.length() - 1;
            }
            else if (grammar[i].substr(j, 2) == "->")
            {
                j = j + 2 - 1;
            }
            else
            { //扫描产生式右部的可能的终结符(关键词表)
                string Vt = getVt(grammar[i].substr(j, grammar[i].length() - j));
                if (VT2int[Vt] == 0)
                    //该终结符第一次出现,将该终结符映射为下标索引
                    VT2int[Vt] = ++symbolNum;
                j = j + Vt.length() - 1;
            }
        }
    }

    cout << "非终结符VN:" << endl;
    for (auto it = VN2int.begin(); it != VN2int.end(); it++)
    {
        cout << "索引下标:" << it->second << "\t名称：" << it->first << endl;
    }
    cout << "终结符VT:" << endl;
    for (auto it = VT2int.begin(); it != VT2int.end(); it++)
    {
        cout << "索引下标:" << it->second << "\t名称：" << it->first << endl;
    }
}

vector<string> splitGrammarIntoYi(string rightGrama)
{ //将产生式的右部(左部->右部拆分)：X->Y1Y2...Yk
    vector<string> Y;
    for (int j = 0; j < rightGrama.length(); j++)
    {
        if (rightGrama[j] >= 'A' && rightGrama[j] <= 'Z')
        { //非终结符
            string Vn = getVn(rightGrama.substr(j, 2));
            Y.push_back(Vn);
            j = j + Vn.length() - 1;
        }
        else
        { //终结符
            string Vt = getVt(rightGrama.substr(j, rightGrama.length() - j));
            Y.push_back(Vt);
            j = j + Vt.length() - 1;
        }
    }
    return Y;
}

void split(string grama, string &X, vector<string> &Y)
{
    int delimiterIndex = grama.find("->");
    X = grama.substr(0, delimiterIndex);
    string rightGrama = grama.substr(delimiterIndex + 2, grama.length() - delimiterIndex - 2);
    Y = splitGrammarIntoYi(rightGrama);
}