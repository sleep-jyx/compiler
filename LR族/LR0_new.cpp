#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include "../Util/util.cpp"
using namespace std;

struct term
{
    int termType; //移进项目、待约项目、规约项目
    string leftPart;
    vector<string> rightPart;
    int dotPos{-1}; //活前缀在右部的位置,初始化为-1

    bool operator==(const term &b) const
    {
        if (leftPart == b.leftPart && rightPart == b.rightPart)
            return true;
        return false;
    }
};
vector<term> statusSet[50]; //项集
int globalStatusNum = 1;
int actionTable[50][50]; //action表，行表示状态，列表示终结符
int gotoTable[50][50];   //goto表，行表示状态，列表示非终结符

void initGrammar()
{
    //表达式文法
    grammar.push_back("E'->E");
    grammar.push_back("E->E+T");
    grammar.push_back("E->T");
    grammar.push_back("T->T*F");
    grammar.push_back("T->F");
    grammar.push_back("F->(E)");
    grammar.push_back("F->i");
}

int mergeSet()
{
    int flag = -1; //假定不可以进行合并
    for (int i = 0; i < globalStatusNum - 1; i++)
    {
        if (statusSet[globalStatusNum - 1].size() != statusSet[i].size())
            continue;
        flag = i; //可以和状态集i合并
        for (int j = 0; j < statusSet[globalStatusNum - 1].size(); j++)
        {
            if (!(statusSet[i][j] == statusSet[globalStatusNum - 1][j]))
            {
                flag = -1;
            }
        }
        if (flag != -1) //第一次遇到可以合并的集合就结束，不然等到后面会遇到项目数相等但状态集不同的情况
            return flag;
    }
    return -1;
}

void initI0()
{
    term firstTerm;
    string X;
    vector<string> Y;
    split(grammar[0], X, Y);
    if (firstTerm.dotPos == -1) //没有活前缀"·"
        firstTerm.dotPos = 0;
    firstTerm.leftPart = X;
    firstTerm.rightPart = Y;
    statusSet[0].push_back(firstTerm);
}

void closure(int statusNum)
{ //计算项集闭包
    term tmpTerm;
    string X;
    vector<string> Y;
    int dot;
    int size = statusSet[statusNum].size(); //因为求闭包会使得项目大小增长，故保存初始大小
    for (int i = 0; i < size; i++)
    {
        tmpTerm = statusSet[statusNum][i];
        X = tmpTerm.leftPart;
        Y = tmpTerm.rightPart;
        dot = tmpTerm.dotPos;
        if (dot == Y.size()) //规约项就不用下面的闭包步骤了
            continue;
        if (VN2int.count(Y[dot]) != 0)
        { //点后面是非终结符，S->·E,待约项目
            //再遍历文法，把左部为E的产生式都加入状态集
            queue<string> vn_q;
            vn_q.push(Y[dot]);
            while (!vn_q.empty())
            {
                string tmpX = vn_q.front();
                int existSameVnFlag = 0;
                for (int k = 0; k < grammar.size(); k++)
                {
                    term tmpTerm2;
                    string X2 = getVn(grammar[k].substr(0, 2));
                    if (X2 == tmpX)
                    {
                        existSameVnFlag = 1;
                        vector<string> Y2;
                        split(grammar[k], X2, Y2);
                        if (VT2int.count(Y2[0]) != 0) //S->·bBB，移进项目
                            tmpTerm2.termType = 2;
                        else if (VN2int.count(Y2[0]) != 0) //S->b·BB,待约项目
                        {
                            tmpTerm2.termType = 3;
                            if (Y2[0] != tmpX)
                                vn_q.push(Y2[0]);
                        }
                        if (tmpTerm2.dotPos == -1)
                            tmpTerm2.dotPos = 0; //加入活前缀
                        tmpTerm2.leftPart = X2;
                        tmpTerm2.rightPart = Y2;
                        statusSet[statusNum].push_back(tmpTerm2);
                    }
                }
                if (existSameVnFlag == 1)
                    vn_q.pop();
            }
        }
    }
}

int GOTO(int statusNum, string symbol)
{ //由状态集statusNum读入一个符号symbol(vn或vt)
    int size = statusSet[statusNum].size();
    for (int i = 0; i < size; i++)
    {
        vector<string> right = statusSet[statusNum][i].rightPart;
        int dotPos = statusSet[statusNum][i].dotPos;
        //symbol是vn或者vt都可
        if (dotPos < right.size() && symbol == right[dotPos])
        {
            term tmpTerm = statusSet[statusNum][i];
            tmpTerm.dotPos = tmpTerm.dotPos + 1; //活前缀后移一位
            dotPos = tmpTerm.dotPos;
            //如果后移一位之后发现成为了规约项目,则加入新项目集
            if (tmpTerm.dotPos == tmpTerm.rightPart.size())
            {
                tmpTerm.termType = 4;
                statusSet[globalStatusNum].push_back(tmpTerm);
            }
            else if (VT2int.count(tmpTerm.rightPart[dotPos]) != 0)
            { //活前缀不在最后，且紧随着一个终结符(移进项目)，加入新项目
                tmpTerm.termType = 2;
                statusSet[globalStatusNum].push_back(tmpTerm);
            }
            else if (VN2int.count(tmpTerm.rightPart[dotPos]) != 0)
            { //活前缀不在最后，且紧随着一个非终结符(待约项目)，将上一个状态集中的该非终结符产生式加入
                //先加入 S->B·B
                tmpTerm.termType = 3;
                statusSet[globalStatusNum].push_back(tmpTerm);
            }
        }
    }
    closure(globalStatusNum);
    globalStatusNum++;
    int flag = mergeSet();
    if (flag != -1) //可合并
    {
        statusSet[globalStatusNum - 1].clear(); //清空，以防万一
        globalStatusNum--;
        return flag;
    }
    else
        return globalStatusNum - 1;
}

void printStatus()
{
    //输出状态集
    for (int i = 0; i < globalStatusNum; i++)
    {
        cout << "┌───────────────┐" << endl;
        cout << "│      I" << i << "\t│" << endl;
        cout << "├───────────────┤" << endl;
        for (auto it = statusSet[i].begin(); it != statusSet[i].end(); it++)
        {
            cout << "│";
            cout << it->leftPart << "->";
            for (int j = 0; j < it->rightPart.size(); j++)
            {
                if (j == it->dotPos)
                    cout << "·";
                cout << it->rightPart[j];
            }
            if (it->rightPart.size() == it->dotPos)
                cout << "·";
            cout << "     \t│" << endl;
        }
        cout << "└───────────────┘" << endl;
    }
}

void constructStatusSet()
{
    initI0();
    closure(0);
    queue<string> symbolToRead;
    map<string, int> symbolMap;
    int curStatus = 0; //队列中当前项的状态

    for (int i = 0; i < statusSet[0].size(); i++)
    {
        string symbolStr = statusSet[0][i].rightPart[statusSet[0][i].dotPos];
        if (symbolMap[symbolStr] == 0)
        {
            symbolToRead.push(symbolStr);
            symbolMap[symbolStr]++;
        }
    }
    symbolToRead.push("sep"); //加入分隔项

    while (!symbolToRead.empty())
    {
        if (symbolToRead.front() == "sep")
        {
            curStatus++;
            symbolToRead.pop();
            continue;
        }
        int nextStatus = GOTO(curStatus, symbolToRead.front());
        cout << "I" << curStatus << "--" << symbolToRead.front() << "-->"
             << "I" << nextStatus << endl;
        //新状态集入队列,指向已有的状态集的就不要入队列了
        if (nextStatus == globalStatusNum - 1)
        {
            symbolMap.clear();
            for (int ii = 0; ii < statusSet[nextStatus].size(); ii++)
            {
                if (statusSet[nextStatus][ii].dotPos == statusSet[nextStatus][ii].rightPart.size())
                    continue;
                string symbolStr = statusSet[nextStatus][ii].rightPart[statusSet[nextStatus][ii].dotPos];
                if (symbolMap[symbolStr] == 0)
                {
                    symbolToRead.push(symbolStr);
                    symbolMap[symbolStr]++;
                }
            }
            symbolToRead.push("sep"); //引入分隔项
        }
        symbolToRead.pop();
    }
    printStatus();
}

int main()
{
    initGrammar();   //初始化文法
    VT2int["$"] = 0; //文法中没有$符号，人为增加该终结符
    readVnAndVt();   //读取文法中所有的VN和VT
    constructStatusSet();
    return 0;
}
