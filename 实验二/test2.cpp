#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include "../Util/parse.cpp"
using namespace std;
class Rule
{
public:
    string rule[50];  //文法规则
    word *sentence;   //经过词法分析后的句子
    int front = 0;    //读头下标
    word currentWord; //当前读头读到的单词
    Rule()
    {
        rule[0] = "E->TE'";
        rule[1] = "E'->+TE'|null";
        rule[2] = "T->FT'";
        rule[3] = "T'->*FT'|null";
        rule[4] = "F->(E)|i";
    }
    void readword();
    void E();
    void E_();
    void T();
    void T_();
    void F();
};
void Rule::readword()
{
    if (front >= lexicalTableLen)
    {
        cout << "——语法分析完毕——" << endl;
        return;
    }
    currentWord.token = sentence[front].token;
    currentWord.syn = sentence[front].syn;
    printf("\t当前读头:%d \tsyn:%d \ttoken:%s \n", front, currentWord.syn, currentWord.token.c_str());
    //cout << "当前读头：" << front << " 对应单词：" << currentWord.token << " " << currentWord.syn << endl;
    front++; //读头移动
}
void Rule::E()
{
    T();
    E_();
}

void Rule::E_()
{
    if (currentWord.token == "+")
    {
        readword();
        T();
        E_();
    };
}

void Rule::T()
{
    F();
    T_();
}
void Rule::T_()
{
    if (currentWord.token == "*")
    {
        readword();
        F();
        T_();
    }
}

void Rule::F()
{
    if (currentWord.syn == 1)
    { //读到的是变量，syn号为1
        readword();
    }
    else if (currentWord.token == "(")
    {
        readword();
        E();
        if (currentWord.token == ")")
        {
            readword();
        }
        else
            cout << "error:缺少')'" << endl;
    }
    else
        cout << "缺少因子" << endl;
}
int main()
{
    Rule r;
    string code;
    ifstream myfile("ex2Data.txt");
    while (getline(myfile, code)) //按行读取文件，可读取空格
    {
        scanner(code);
    }
    // 词法分析结束，分析结果存储在lexicalTable中
    r.sentence = lexicalTable;
    /*
    for (int i = 0; i < lexicalTableLen; i++)
    {
        printf("\t%d\t%s\n", r.sentence[i].syn, r.sentence[i].token.c_str());
        //cout << r.sentence[i].syn << " " << r.sentence[i].token << endl;
    }
    */
    cout << "——————语法分析(递归下降法)——————" << endl;
    r.readword();
    r.E(); //文法开始
    if (r.front < lexicalTableLen)
        cout << "未能识别" << endl;
    else
        cout << "成功识别" << endl;
    return 0;
}
