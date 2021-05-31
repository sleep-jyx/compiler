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
    int dotPos{-1};             //活前缀在右部的位置,初始化为-1
    vector<string> subsequence; //LR1文法才用到的后继符,随便初始化一个字符串
    bool operator==(const term &b) const
    {
        if (leftPart == b.leftPart && rightPart == b.rightPart && dotPos == b.dotPos)
        {
            if (subsequence == b.subsequence)
                return true;
        }
        return false;
    }
};
vector<term> statusSet[50]; //项集
int globalStatusNum = 1;
int actionTable[50][50]; //action表，行表示状态，列表示终结符
int gotoTable[50][50];   //goto表，行表示状态，列表示非终结符

void initGrammar()
{
    //表达式文法,用LR(0)构造存在冲突，用SLR可以消除冲突
    /* grammar.push_back("E'->E");
    grammar.push_back("E->E+T");
    grammar.push_back("E->T");
    grammar.push_back("T->T*F");
    grammar.push_back("T->F");
    grammar.push_back("F->(E)");
    grammar.push_back("F->i");
    */
    //该文法使用SLR仍有冲突(哈工大mooc例子)，使用LR1分析验证正确
    /*
    grammar.push_back("S'->S");
    grammar.push_back("S->L=R");
    grammar.push_back("S->R");
    grammar.push_back("L->*R");
    grammar.push_back("L->i");
    grammar.push_back("R->L");
    */
    //龙书本科版，LR1例子,验证正确
    /*
    grammar.push_back("S'->S");
    grammar.push_back("S->CC");
    grammar.push_back("C->cC");
    grammar.push_back("C->d");
    */

    //赋值语句文法
    grammar.push_back("S'->A");
    grammar.push_back("A->i=E");
    grammar.push_back("E->@E");
    grammar.push_back("E->E+E");
    grammar.push_back("E->E-E");
    grammar.push_back("E->E*E");
    grammar.push_back("E->E/E");
    grammar.push_back("E->(E)");
    grammar.push_back("E->i");
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
    firstTerm.subsequence.push_back("$");
    statusSet[0].push_back(firstTerm);
}

//相对于LR0和SLR，LR1在closure这个函数中有大改动，几乎是重写
void closure(int statusNum)
{ //计算LR1文法的项集闭包
    queue<term> termQueue;
    for (int i = 0; i < statusSet[statusNum].size(); i++)
        termQueue.push(statusSet[statusNum][i]);
    while (!termQueue.empty())
    { //对项集I中每个项[A->a·B beta,alpha]
        term curTerm = termQueue.front();
        if (curTerm.dotPos == curTerm.rightPart.size()) //如果是规约项，跳过并弹出该项
        {
            termQueue.pop();
            continue;
        }
        string B = curTerm.rightPart[curTerm.dotPos];
        for (int j = 0; j < grammar.size(); j++)
        { //对增广文法G'中的每个产生式B->gamma
            if (B != getVn(grammar[j].substr(0, 2)))
                continue;
            //将[B->·gamma,b]加入集合I中，其中b是FIRST[beta alpha]中的终结符
            term newTerm;
            newTerm.dotPos = 0;
            split(grammar[j], newTerm.leftPart, newTerm.rightPart);
            //warning:好像没考虑B->null的情况，还是说已经考虑了(null看成终结符)
            if (VT2int.count(newTerm.rightPart[0]) != 0) //B->·bA，移进项目
                newTerm.termType = 2;
            else if (VN2int.count(newTerm.rightPart[0]) != 0) //B->b·A,待约项目
                newTerm.termType = 3;
            //找b
            string beta;
            vector<string> b;
            if (curTerm.dotPos == curTerm.rightPart.size() - 1)
            { //如果beta不存在，b即alpha
                b = curTerm.subsequence;
            }
            else if (VT2int.count(curTerm.rightPart[curTerm.dotPos + 1]) != 0)
            { //如果beta存在，b为first(beta)。特例：beta为终结符时，b就是beta
                b.push_back(curTerm.rightPart[curTerm.dotPos + 1]);
            }
            else
            { //遍历first(beta)中的终结符b
                beta = curTerm.rightPart[curTerm.dotPos + 1];
                for (auto it = first[VN2int[beta]].begin(); it != first[VN2int[beta]].end(); it++)
                {
                    b.push_back(*it);
                }
            }
            newTerm.subsequence = b;
            //只有闭包生成的新项不在集合I中才加入
            int newTermFlag = 1;
            for (int k = 0; k < statusSet[statusNum].size(); k++)
            {
                if (newTerm == statusSet[statusNum][k])
                    newTermFlag = 0;
            }
            if (newTermFlag == 1)
            {
                termQueue.push(newTerm);
                statusSet[statusNum].push_back(newTerm);
            }
        }
        termQueue.pop();
    }
    //合并后继符
    /*vector<term> newSet; //合并后继符的项集
    map<term, int> mergedTermMap;
    term curTerm = statusSet[statusNum][0];
    term cleanTerm = statusSet[statusNum][0];
    newSet.push_back(curTerm);
    cleanTerm.subsequence.clear(); //map中不保留后继符
    mergedTermMap[cleanTerm]++;

    int size = statusSet[statusNum].size();
    for (int i = 1; i < size; i++)
    {
        curTerm = statusSet[statusNum][i];
        cleanTerm = statusSet[statusNum][i];
        cleanTerm.subsequence.clear();
        if (mergedTermMap[cleanTerm] != 0)
        {
            for(int i=0;i<curTerm.subsequence.size();i++)
            {

            }
        }
        else
        {
            newSet.push_back(curTerm);
            mergedTermMap[cleanTerm]++;
        }
        
    }*/
}

//GOTO函数无须变动，不管是什么分析，都是从集合I读入一个符号经过闭包运算得到集合J。goto函数内部调用了closure()。
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
            for (int j = 0; j < it->subsequence.size(); j++)
            {
                if (j == 0)
                    cout << "," << it->subsequence[j];
                else
                    cout << "_" << it->subsequence[j];
            }
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
        for (auto it = VN2int.begin(); it != VN2int.end(); it++)
        { //goto表跳过增广文法的左部
            if (it->first == getVn(grammar[0].substr(0, 2)))
                continue;
            cout << gotoTable[i][it->second] << "\t";
        }
        cout << endl;
    }
}

//相比LR0和SLR，LR1分析表只对规约项进行改动(项种类：移进项(不变)、接受项(?)、GOTO项(不变))
//删了十几行，加了1行就ok了
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
        { //一个项集的移进项和goto项都处理完毕之后，开始处理规约项
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
                        //LR(1)分析中，只有规约项的后继符才进行规约
                        for (int _i = 0; _i < tmpTerm.subsequence.size(); _i++)
                            actionTable[curStatus][VT2int[tmpTerm.subsequence[_i]]] = 1000 + genNum; //同样为避免编号冲突，规约项全体加1000
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
        //新状态集的活前缀后面的符号(包括vn和vt)入队列,若产生的状态集是已有的状态集的就不用了
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
    initGrammar();   //初始化文法
    VT2int["$"] = 0; //文法中没有$符号，人为增加该终结符
    readVnAndVt();   //读取文法中所有的VN和VT
    converge();      //构造first和follow集
    constructStatusSet();
    return 0;
}
