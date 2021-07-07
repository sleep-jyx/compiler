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
vector<term> statusSet[200]; //项集
int globalStatusNum = 1;
int actionTable[50][50]; //action表，行表示状态，列表示终结符
int gotoTable[50][50];   //goto表，行表示状态，列表示非终结符

void initGrammar()
{
    /*
    grammar.push_back("S'->S");
    grammar.push_back("S->L=R");
    grammar.push_back("S->R");
    grammar.push_back("L->*R");
    grammar.push_back("L->i");
    grammar.push_back("R->L");
    */
    /*
    grammar.push_back("S'->E");
    grammar.push_back("E->E+T");
    grammar.push_back("E->T");
    grammar.push_back("T->T*F");
    grammar.push_back("T->F");
    grammar.push_back("F->(E)");
    grammar.push_back("F->i");
    */
    grammar.push_back("S'->S");
    grammar.push_back("S->aS");
    grammar.push_back("S->b");
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
            map<string, int> VN_readed;
            vn_q.push(Y[dot]);
            VN_readed[Y[dot]]++;
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
                            if (VN_readed[Y2[0]] == 0)
                            {
                                vn_q.push(Y2[0]);
                                VN_readed[Y2[0]]++;
                            }
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
{ //由状态集statusNum读入一个符号symbol(vn或vt)，返回转移后的项集编号
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
                //再进行闭包计算
                closure(globalStatusNum);
            }
        }
    }
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

void printTable()
{
    //输出分析表
    cout << " \t";
    for (auto it = VT2int.begin(); it != VT2int.end(); it++)
        cout << it->first << " \t";
    for (auto it = VN2int.begin(); it != VN2int.end(); it++)
    { //goto表跳过增广文法的左部
        if (it->first == getVn(grammar[0].substr(0, 2)))
            continue;
        cout << it->first << "\t";
    }
    cout << endl;
    for (int i = 0; i < globalStatusNum; i++)
    {
        cout << i << "\t";
        for (auto it = VT2int.begin(); it != VT2int.end(); it++)
        { //action，移进(大100)、规约(大1000)、接受
            if (actionTable[i][it->second] >= 100 && actionTable[i][it->second] < 1000)
                cout << "s" << actionTable[i][it->second] - 100 << "\t";
            else if (actionTable[i][it->second] >= 1000 && actionTable[i][it->second] < 10000)
                cout << "r" << actionTable[i][it->second] - 1000 << "\t";
            else if (actionTable[i][it->second] == 10000)
                cout << "acc\t";
            else
                cout << actionTable[i][it->second] << " \t";
        }
        //cout << " \t";
        for (auto it = VN2int.begin(); it != VN2int.end(); it++)
        { //goto表跳过增广文法的左部
            if (it->first == getVn(grammar[0].substr(0, 2)))
                continue;
            cout << gotoTable[i][it->second] << "\t";
        }
        cout << endl;
    }
}

void constructStatusSet(int choice = 0)
{ //同步构造项集和分析表
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
            for (int ii = 0; ii < statusSet[curStatus].size(); ii++)
            {
                /****action表规约项构造****/
                if (statusSet[curStatus][ii].dotPos == statusSet[curStatus][ii].rightPart.size())
                { //判断该规约项是用哪个产生式规约的
                    term tmpTerm = statusSet[curStatus][ii];
                    string reduceTerm = tmpTerm.leftPart + "->";
                    for (int ii = 0; ii < tmpTerm.rightPart.size(); ii++)
                        reduceTerm = reduceTerm + tmpTerm.rightPart[ii];
                    int genNum = -1;
                    for (int ii = 0; ii < grammar.size(); ii++)
                        if (reduceTerm == grammar[ii])
                            genNum = ii;
                    //接受状态
                    if (genNum == 0)
                        actionTable[curStatus][VT2int["$"]] = 10000;
                    else
                    {
                        if (choice == 0)
                        { //LR(0)分析中只要某状态集中存在规约项，则action表中该行所有终结符用同一产生式规约
                            for (auto it = VT2int.begin(); it != VT2int.end(); it++)
                            {
                                if (actionTable[curStatus][it->second] != 0)
                                    cout << "状态" << curStatus << "规约" << it->first << "存在冲突" << endl;
                                actionTable[curStatus][it->second] = 1000 + genNum; //同样为避免编号冲突，规约项全体加1000
                            }
                        }
                        else
                        { //SLR分析中，只有规约产生式左部的follow集中的终结符才进行规约
                            for (auto it = follow[VN2int[tmpTerm.leftPart]].begin(); it != follow[VN2int[tmpTerm.leftPart]].end(); it++)
                            {
                                if (actionTable[curStatus][VT2int[*it]] != 0)
                                    cout << "状态" << curStatus << "规约" << *it << "存在冲突" << endl;
                                actionTable[curStatus][VT2int[*it]] = 1000 + genNum; //同样为避免编号冲突，规约项全体加1000
                            }
                        }
                    }
                }
                continue;
            }
            /****action表规约项填充完毕*****/
            curStatus++;
            symbolToRead.pop();
            continue;
        }
        int nextStatus = GOTO(curStatus, symbolToRead.front());
        cout << "I" << curStatus << "--" << symbolToRead.front() << "-->"
             << "I" << nextStatus;
        //action表移入项填充
        if (VT2int.count(symbolToRead.front()) != 0)
        {
            if (actionTable[curStatus][VT2int[symbolToRead.front()]] != 0)
                cout << "(状态" << curStatus << "移进" << symbolToRead.front() << "存在冲突)";
            actionTable[curStatus][VT2int[symbolToRead.front()]] = 100 + nextStatus;
        }
        else //goto表填充
            gotoTable[curStatus][VN2int[symbolToRead.front()]] = nextStatus;
        cout << endl;
        //新状态集入队列,指向已有的状态集的就不要入队列了
        if (nextStatus == globalStatusNum - 1)
        {
            symbolMap.clear();
            for (int ii = 0; ii < statusSet[nextStatus].size(); ii++)
            {
                //规约项就跳过
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
    printStatus(); //输出状态项集
    printTable();  //输出分析表
}

int main()
{
    initGrammar();         //初始化文法
    VT2int["$"] = 0;       //文法中没有$符号，人为增加该终结符
    readVnAndVt();         //读取文法中所有的VN和VT
    converge();            //构造first和follow集
    constructStatusSet(1); //默认LR(0)分析表构造，参数1构造SLR分析表

    return 0;
}
