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
    int dotPos; //活前缀在右部的位置

    bool operator==(const term &b) const
    {
        if (leftPart == b.leftPart && rightPart == b.rightPart)
            return true;
        return false;
    }
};
vector<term> statusSet[50];
int globalStatusNum = 1;
int actionTable[50][50]; //action表，行表示状态，列表示终结符
int gotoTable[50][50];   //goto表，行表示状态，列表示非终结符

void initGrammar()
{
    /*
    grammar.push_back("S'->S");
    grammar.push_back("S->BB");
    grammar.push_back("B->aB");
    grammar.push_back("B->b");
    */
    /*
    grammar.push_back("S'->E");
    grammar.push_back("E->aA");
    grammar.push_back("E->bB");
    grammar.push_back("A->cA");
    grammar.push_back("A->d");
    grammar.push_back("B->cB");
    grammar.push_back("B->d");
    */
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
{                  //合并相同的状态集
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

int actionVT(int statusNum, term moveTerm, string vt, int dotPos)
{ //由状态集statusNum中的某个移进项目moveTerm读入一个终结符vt,活前缀位置
    term tmpTerm;
    tmpTerm.leftPart = moveTerm.leftPart;
    //活前缀后移一位
    for (int i = 0; i < moveTerm.rightPart.size(); i++)
    {
        string str = moveTerm.rightPart[i];
        if (i == dotPos)
            continue;
        tmpTerm.rightPart.push_back(str);
    }
    tmpTerm.rightPart.insert(tmpTerm.rightPart.begin() + dotPos + 1, "·");
    //活前缀后移一位，位置+1
    dotPos = dotPos + 1;
    //如果后移一位之后发现成为了规约项目
    if (dotPos == moveTerm.rightPart.size() - 1)
    {
        tmpTerm.termType = 4;
        statusSet[globalStatusNum++].push_back(tmpTerm);
    }
    else if (VT2int.count(moveTerm.rightPart[dotPos + 1]) != 0)
    { //移动后的活前缀不在最后，且紧随着一个终结符(移进项目)
        tmpTerm.termType = 2;
        statusSet[globalStatusNum++].push_back(tmpTerm);
    }
    else if (VN2int.count(moveTerm.rightPart[dotPos + 1]) != 0)
    { //活前缀不在最后，且紧随着一个非终结符(待约项目)，将上一个状态集中的该非终结符产生式加入
        //先加入 B->a·B
        tmpTerm.termType = 3;
        statusSet[globalStatusNum].push_back(tmpTerm);
        //再遍历文法，把左部为B的产生式都加入状态集
        for (int k = 0; k < grammar.size(); k++)
        {
            string X = getVn(grammar[k].substr(0, 2));
            if (X == moveTerm.rightPart[dotPos + 1])
            {
                vector<string> Y;
                split(grammar[k], X, Y);
                if (VT2int.count(Y[0]) != 0) //S->·bBB，移进项目
                    tmpTerm.termType = 2;
                else if (VN2int.count(Y[0]) != 0) //S->b·BB,待约项目
                    tmpTerm.termType = 3;
                if (find(Y.begin(), Y.end(), "·") == Y.end()) //没有活前缀"·"
                {
                    Y.insert(Y.begin(), "·");
                }
                tmpTerm.leftPart = X;
                tmpTerm.rightPart = Y;
                statusSet[globalStatusNum].push_back(tmpTerm);
            }
        }
        globalStatusNum++;
    }
    int flag = mergeSet();
    if (flag != -1) //可合并
    {
        statusSet[globalStatusNum - 1].clear();
        globalStatusNum--;
        return flag;
    }
    else
        return globalStatusNum - 1;
}

//新的gotoVN函数可以一次处理一个状态集中的多个项(保证这多个项的活前缀后的符号相同)，所以在构造状态集的过程中不用再一个个项目的BFS遍历了，应该用活前缀后的符号(取不重复集)进行BFS遍历
int gotoVN(int statusNum, term reduceTerm, string vn, int dotPos)
{ //由状态集statusNum中的某个规约项目reduceTerm读入一个非终结符vn,活前缀位置

    for (int i = 0; i < statusSet[statusNum].size(); i++)
    {
        vector<string> right = statusSet[statusNum][i].rightPart;
        for (int j = 0; j < right.size(); j++)
        {
            if (right[j] == "·")
            {
                dotPos = j;
                break;
            }
        }
        if (dotPos < right.size() - 1 && vn == right[dotPos + 1])
        {
            reduceTerm = statusSet[statusNum][i];
            term tmpTerm;
            tmpTerm.leftPart = reduceTerm.leftPart;
            //活前缀后移一位reduceTerm.rightPart[dotPos + 1]
            for (int i = 0; i < reduceTerm.rightPart.size(); i++)
            {
                string str = reduceTerm.rightPart[i];
                if (i == dotPos)
                    reduceTerm.rightPart[dotPos + 1] continue;
                tmpTerm.rightPart.push_back(str);
            }
            tmpTerm.rightPart.insert(tmpTerm.rightPart.begin() + dotPos + 1, "·");
            //活前缀后移一位，位置+1
            dotPos = dotPos + 1;
            //如果后移一位之后发现成为了规约项目,则加入新项目集
            if (dotPos == reduceTerm.rightPart.size() - 1)
            {
                tmpTerm.termType = 4;
                statusSet[globalStatusNum].push_back(tmpTerm);
            }
            else if (VT2int.count(reduceTerm.rightPart[dotPos + 1]) != 0)
            { //活前缀不在最后，且紧随着一个终结符(移进项目)，加入新项目
                tmpTerm.termType = 2;
                string vt = reduceTerm.rightPart[dotPos + 1];
                cout << "I" << statusNum << "--" << vt << "-->"
                     << "I" << globalStatusNum << endl;
                statusSet[globalStatusNum].push_back(tmpTerm);
            }
            else if (VN2int.count(reduceTerm.rightPart[dotPos + 1]) != 0)
            { //活前缀不在最后，且紧随着一个非终结符(待约项目)，将上一个状态集中的该非终结符产生式加入
                //先加入 S->B·B
                tmpTerm.termType = 3;
                string vn = reduceTerm.rightPart[dotPos + 1];
                cout << "I" << statusNum << "--" << vn << "-->"
                     << "I" << globalStatusNum << endl;
                statusSet[globalStatusNum].push_back(tmpTerm);
                //再遍历文法，把左部为B的产生式都加入状态集
                for (int k = 0; k < grammar.size(); k++)
                {
                    string X = getVn(grammar[k].substr(0, 2));
                    if (X == reduceTerm.rightPart[dotPos + 1])
                    {
                        vector<string> Y;
                        split(grammar[k], X, Y);
                        if (VT2int.count(Y[0]) != 0) //S->·bBB，移进项目
                            tmpTerm.termType = 2;
                        else if (VN2int.count(Y[0]) != 0) //S->b·BB,待约项目
                            tmpTerm.termType = 3;
                        if (find(Y.begin(), Y.end(), "·") == Y.end()) //没有活前缀"·"就加入
                        {
                            Y.insert(Y.begin(), "·");
                        }
                        tmpTerm.leftPart = X;
                        tmpTerm.rightPart = Y;
                        statusSet[globalStatusNum].push_back(tmpTerm);
                    }
                }
            }
        }
    }
    int flag = mergeSet();
    if (flag != -1) //可合并
    {
        statusSet[globalStatusNum - 1].clear();
        globalStatusNum--;
        return flag;
    }
    else
        return globalStatusNum - 1;
}

void constructStatusSet()
{
    int statusNum = 0; //状态集编号
    term tmpTerm;
    string X;
    vector<string> Y;
    //构造I0
    split(grammar[0], X, Y);
    if (find(Y.begin(), Y.end(), "·") == Y.end()) //没有活前缀"·"
        Y.insert(Y.begin(), "·");
    tmpTerm.dotPos = 0;
    tmpTerm.leftPart = X;
    tmpTerm.rightPart = Y;

    if (VN2int.count(Y[1]) != 0) //S->·E,待约项目
    {                            //先把S->·E加入状态集0，再继续找E的产生式
        tmpTerm.termType = 3;
        statusSet[0].push_back(tmpTerm);
        //再遍历文法，把左部为E的产生式都加入状态集
        queue<string> vn_q;
        vn_q.push(Y[1]);
        while (!vn_q.empty())
        {
            string tmpX = vn_q.front();
            int existSameVnFlag = 0;
            for (int k = 0; k < grammar.size(); k++)
            {
                string X2 = getVn(grammar[k].substr(0, 2));

                if (X2 == tmpX)
                {
                    existSameVnFlag = 1;
                    vector<string> Y2;
                    split(grammar[k], X2, Y2);
                    if (VT2int.count(Y2[0]) != 0) //S->·bBB，移进项目
                        tmpTerm.termType = 2;
                    else if (VN2int.count(Y2[0]) != 0) //S->b·BB,待约项目
                    {
                        tmpTerm.termType = 3;
                        if (Y2[0] != tmpX)
                            vn_q.push(Y2[0]);
                    }
                    if (find(Y2.begin(), Y2.end(), "·") == Y2.end()) //没有活前缀"·"就加入
                    {
                        Y2.insert(Y2.begin(), "·");
                    }
                    tmpTerm.dotPos = 0;
                    tmpTerm.leftPart = X2;
                    tmpTerm.rightPart = Y2;
                    statusSet[0].push_back(tmpTerm);
                }
            }
            if (existSameVnFlag == 1)
                vn_q.pop();
        }
    }

    //构造其他状态集,BFS,为了在队列中也能够区分不同状态集的项目，引入分隔项
    queue<term> q;
    queue<string> symbolToRead;
    map<string, int> symbolMap;
    string stringSep = "sep";
    term sep;
    sep.termType = 666; //挪用下这个变量，虽然用法不恰当
    int curStatus = 0;  //队列中当前项的状态
    for (int i = 0; i < statusSet[0].size(); i++)
    {
        string symbolStr = statusSet[0][i].rightPart[statusSet[0][i].dotPos + 1];
        if (symbolMap[symbolStr] != 0)
            symbolToRead.push(symbolStr);
        else
            symbolMap[symbolStr]++;
        q.push(statusSet[0][i]);
    }
    //symbolMap.clear();
    q.push(sep); //加入分隔项
    symbolToRead.push(stringSep);
    while (!symbolToRead.empty())
    {
        /*
        int dotPos = -1;
        tmpTerm = q.front();
        if (tmpTerm.termType == 666)
        {
            curStatus++;
            q.pop();
            continue;
        }
        //找到当前项目右部活前缀的位置
        for (int j = 0; j < tmpTerm.rightPart.size(); j++)
        {
            if (tmpTerm.rightPart[j] == "·")
            {
                dotPos = j;
                break;
            }
        }*/
        for (int ii = 0; ii < symbolMap[symbolToRead.front()]; ii++)
        { //有1或n个项目读入同一个符号
            for (int jj = 0; jj < statusSet[curStatus].size(); jj++)
            { //找到这1或n个项目
                if (statusSet[curStatus][jj].rightPart[statusSet[curStatus][jj].dotPos + 1] == symbolToRead.front())
                {
                    if (statusSet[curStatus][jj].dotPos == statusSet[curStatus][jj].rightPart.size() - 1)
                    { //如果是规约项目（活前缀在最后）
                        cout << "I" << curStatus << "中的";
                        for (auto it = statusSet[curStatus].begin(); it != statusSet[curStatus].end(); it++)
                        {
                            cout << it->leftPart << "->";
                            for (int j = 0; j < it->rightPart.size(); j++)
                                cout << it->rightPart[j];
                        }
                        cout << "是规约项" << endl;
                        //判断该规约项是用哪个产生式规约的
                        string reduceTerm = tmpTerm.leftPart + "->";
                        for (int ii = 0; ii < tmpTerm.rightPart.size() - 1; ii++)
                            reduceTerm = reduceTerm + tmpTerm.rightPart[ii];
                        int genNum = -1;
                        for (int ii = 0; ii < grammar.size(); ii++)
                            if (reduceTerm == grammar[ii])
                                genNum = ii;
                        //接受状态
                        if (genNum == 0)
                            actionTable[curStatus][VT2int["$"]] = 10000;
                        else
                        { //LR(0)分析中只要某状态集中存在规约项，则action表中该行所有终结符用同一产生式规约
                            for (auto it = VT2int.begin(); it != VT2int.end(); it++)
                                actionTable[curStatus][it->second] = 1000 + genNum; //同样为避免编号冲突，规约项全体加1000
                        }
                        q.pop();
                        continue;
                    }
                }
            }
        }
        int sNum = 0;
        if (dotPos == tmpTerm.rightPart.size() - 1)
        { //如果是规约项目（活前缀在最后）
            cout << "I" << curStatus << "中的";
            for (auto it = statusSet[curStatus].begin(); it != statusSet[curStatus].end(); it++)
            {
                cout << it->leftPart << "->";
                for (int j = 0; j < it->rightPart.size(); j++)
                    cout << it->rightPart[j];
            }
            cout << "是规约项" << endl;
            //判断该规约项是用哪个产生式规约的
            string reduceTerm = tmpTerm.leftPart + "->";
            for (int ii = 0; ii < tmpTerm.rightPart.size() - 1; ii++)
                reduceTerm = reduceTerm + tmpTerm.rightPart[ii];
            int genNum = -1;
            for (int ii = 0; ii < grammar.size(); ii++)
                if (reduceTerm == grammar[ii])
                    genNum = ii;
            //接受状态
            if (genNum == 0)
                actionTable[curStatus][VT2int["$"]] = 10000;
            else
            { //LR(0)分析中只要某状态集中存在规约项，则action表中该行所有终结符用同一产生式规约
                for (auto it = VT2int.begin(); it != VT2int.end(); it++)
                    actionTable[curStatus][it->second] = 1000 + genNum; //同样为避免编号冲突，规约项全体加1000
            }
            q.pop();
            continue;
        }
        else if (VT2int.count(tmpTerm.rightPart[dotPos + 1]) != 0)
        { //如果是移进项目（活前缀后面是终结符）
            string vt = tmpTerm.rightPart[dotPos + 1];
            sNum = actionVT(curStatus, tmpTerm, vt, dotPos);
            //cout << "I" << curStatus << "--" << vt << "-->"
            //     << "I" << sNum << endl;
            actionTable[curStatus][VT2int[vt]] = 100 + sNum; //为避免编号冲突，令移进项全体加100
        }
        reduceTerm.rightPart[dotPos + 1] else if (VN2int.count(tmpTerm.rightPart[dotPos + 1]) != 0)
        { //如果是待约项目（活前缀后面是非终结符）
            string vn = tmpTerm.rightPart[dotPos + 1];
            sNum = gotoVN(curStatus, tmpTerm, vn, dotPos);
            //cout << "I" << curStatus << "--" << vn << "-->"
            //     << "I" << sNum << endl;
            gotoTable[curStatus][VN2int[vn]] = sNum;
        }
        //新状态集入队列,指向已有的状态集的就不要入队列了
        if (sNum == globalStatusNum - 1)
        {
            for (int ii = 0; ii < statusSet[sNum].size(); ii++)
                q.push(statusSet[sNum][ii]);
            q.push(sep); //引入分隔项
        }
        q.pop();
    }

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
                cout << it->rightPart[j];
            }
            cout << "     \t│" << endl;
        }
        cout << "└───────────────┘" << endl;
    }
    //输出分析表
    cout << " \t";
    for (auto it = VT2int.begin(); it != VT2int.end(); it++)
        cout << it->first << "\t";
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
                cout << actionTable[i][it->second] << "\t";
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

int main()
{
    /*
    LR(0)分析
    1、构造初始化文法
    2、识别所有终结符和非终结符?
    3、构造初始状态集(BFS)
    4、由初始状态集构造其他状态集(BFS)
    5、构造状态分析表(action和goto)
    */
    initGrammar();   //初始化文法
    VT2int["$"] = 0; //文法中没有$符号，人为增加该终结符
    readVnAndVt();   //读取文法中所有的VN和VT
    constructStatusSet();

    return 0;
}

/*   
printf("┌──┬────────┐\n");
printf("├──┼────────┤\n");
printf("└──┴────────┘\n");
	─ │ ┌ ┐└ ┘├ ┤┬ ┴ ┼
*/