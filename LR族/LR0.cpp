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
    bool operator==(const term &b) const
    {
        if (leftPart == b.leftPart && rightPart == b.rightPart)
            return true;
        return false;
    }
};
vector<term> statusSet[50];
int globalStatusNum = 1;

void initGrammar()
{
    grammar.push_back("S'->S");
    grammar.push_back("S->BB");
    grammar.push_back("B->aB");
    grammar.push_back("B->b");
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

int gotoVN(int statusNum, term reduceTerm, string vn, int dotPos)
{ //由状态集statusNum中的某个规约项目reduceTerm读入一个非终结符vn,活前缀位置
    term tmpTerm;
    tmpTerm.leftPart = reduceTerm.leftPart;
    //活前缀后移一位
    for (int i = 0; i < reduceTerm.rightPart.size(); i++)
    {
        string str = reduceTerm.rightPart[i];
        if (i == dotPos)
            continue;
        tmpTerm.rightPart.push_back(str);
    }
    tmpTerm.rightPart.insert(tmpTerm.rightPart.begin() + dotPos + 1, "·");
    //如果后移一位之后发现成为了规约项目(就是说本身活前缀就在倒数第二位)
    if (dotPos == reduceTerm.rightPart.size() - 2)
    {
        tmpTerm.termType = 4;
        statusSet[globalStatusNum++].push_back(tmpTerm);
    }
    else if (VT2int.count(reduceTerm.rightPart[dotPos + 1]) != 0)
    { //活前缀不在最后，且紧随着一个终结符(移进项目)
        tmpTerm.termType = 2;
        statusSet[globalStatusNum++].push_back(tmpTerm);
    }
    else if (VN2int.count(reduceTerm.rightPart[dotPos + 1]) != 0)
    { //活前缀不在最后，且紧随着一个非终结符(待约项目)，将上一个状态集中的该非终结符产生式加入
        //先加入 S->B·B
        tmpTerm.termType = 3;
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
void constructStatusSet()
{
    int statusNum = 0; //状态集编号
    term tmpTerm;
    string X;
    vector<string> Y;
    //构造I0
    for (int i = 0; i < grammar.size(); i++)
    {
        split(grammar[i], X, Y);
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
        statusSet[0].push_back(tmpTerm);
    }
    //构造其他状态集,BFS,为了在队列中也能够区分不同状态集的项目，引入分隔项
    queue<term> q;
    term sep;
    sep.termType = 666; //挪用下这个变量，虽然用法不恰当
    int curStatus = 0;  //队列中当前项是什么状态
    for (int i = 0; i < statusSet[0].size(); i++)
        q.push(statusSet[0][i]);
    q.push(sep); //加入分隔项
    while (!q.empty())
    {
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
            q.pop();
            continue;
        }
        else if (VT2int.count(tmpTerm.rightPart[dotPos + 1]) != 0)
        { //如果是移进项目（活前缀后面是终结符）
            sNum = actionVT(curStatus, tmpTerm, tmpTerm.rightPart[dotPos + 1], dotPos);
            cout << "I" << curStatus << "--" << tmpTerm.rightPart[dotPos + 1] << "-->"
                 << "I" << sNum << endl;
            //cout << sNum << endl;
        }
        else if (VN2int.count(tmpTerm.rightPart[dotPos + 1]) != 0)
        { //如果是待约项目（活前缀后面是非终结符）
            sNum = gotoVN(curStatus, tmpTerm, tmpTerm.rightPart[dotPos + 1], dotPos);
            cout << "I" << curStatus << "--" << tmpTerm.rightPart[dotPos + 1] << "-->"
                 << "I" << sNum << endl;
            //cout << sNum << endl;
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

    //输出
    for (int i = 0; i < globalStatusNum; i++)
    {
        cout << "┌───────────────┐" << endl;
        cout << "│      I" << i << "       │" << endl;
        cout << "├───────────────┤" << endl;
        for (auto it = statusSet[i].begin(); it != statusSet[i].end(); it++)
        {
            cout << "│";
            cout << it->leftPart << "->";
            for (int j = 0; j < it->rightPart.size(); j++)
            {
                cout << it->rightPart[j];
            }
            cout << "\t\t│" << endl;
        }
        cout << "└───────────────┘" << endl;
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
    5、构造状态分析表(action和goto)
    */
    initGrammar(); //初始化文法
    readVnAndVt(); //读取文法中所有的VN和VT
    constructStatusSet();

    return 0;
}

/*   
printf("┌──┬────────┐\n");
printf("├──┼────────┤\n");
printf("└──┴────────┘\n");
	─ │ ┌ ┐└ ┘├ ┤┬ ┴ ┼
*/