#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include "../Util/util.cpp"
using namespace std;

struct term
{
    int termType; //移进项目、待约项目、规约项目
    string leftPart;
    vector<string> rightPart;
    //想要把term作为自定义类型插入set中必须有以下的比较函数
    bool operator<(const term &b) const //find和count均支持，const关键字必不可少
    {                                   //随便怎么排序
        return true;
    }
};

set<term> statusSet[50];
vector<term> statusVec[50];

void initGrammar()
{
    grammar.push_back("S'->S");
    grammar.push_back("S->BB");
    grammar.push_back("B->aB");
    grammar.push_back("B->b");
}

void constructStatusSet()
{
    int statusNum = 0;
    term tmpTerm;
    string X;
    vector<string> Y;
    for (int i = 0; i < grammar.size(); i++)
    {
        split(grammar[i], X, Y);
        //if(X=="S'")
        //  tmpTerm.termType =1;//接收状态
        if (VT2int.count(Y[0]) != 0) //S->·bBB，移进项目
            tmpTerm.termType = 2;
        else if (VN2int.count(Y[0]) != 0) //S->b·BB,待约项目
            tmpTerm.termType = 3;

        else if (VN2int.count(Y[0]) != 0)
        {                         //S->bBB·
            tmpTerm.termType = 3; //规约项目
        }
        //Y.insert();
        if (find(Y.begin(), Y.end(), "·") == Y.end()) //没有活前缀"·"
        {
            cout << "hello" << endl;
            Y.insert(Y.begin(), "·");
        }
        tmpTerm.leftPart = X;
        tmpTerm.rightPart = Y;
        statusSet[0].insert(tmpTerm);
    }
    //输出
    for (auto it = statusSet[0].begin(); it != statusSet[0].end(); it++)
    {
        cout << it->leftPart << "->";
        for (int j = 0; j < it->rightPart.size(); j++)
        {
            cout << it->rightPart[j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    /*
    LR(0)分析
    1、构造初始化文法
    2、识别所有终结符和非终结符?
    3、将文法转为新的数据结构
    4、构造各个状态集
    */
    initGrammar(); //初始化文法
    readVnAndVt(); //读取文法中所有的VN和VT
    constructStatusSet();

    return 0;
}