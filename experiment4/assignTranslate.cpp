#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <string.h>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include <fstream>
using namespace std;

/********************************************************
 *                                                      *
 *                第一部分：词法分析                       *  
 *                                                      *
 *******************************************************/
// 关键字表置初始值
string Cppkeyword[100] = {"#", "标识符(变量名)", "整数", "实数", "字符常量", "+", "-", "*", "/", "<",
                          "<=", "==", "!=", ">", ">=", "&", "&&", "||", "=", "(",
                          ")", "[", "]", "{", "}", ":", ";", ",", "@", "!",
                          "void", "int", "float", "char", "if", "else", "while", "do", "for", "include",
                          "iostream", "using", "namespace", "std", "main", "return", "null"};
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
    for (int i = 30; i <= 46; i++)
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
        else
            item.syn = 29;
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
    case '@':
        item.syn = 28;
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

/****************************************************************
 *                                                              *
 *        第二部分、语法分析前的一些准备工作,主要包括:                 *      
 *      (1) 扫描文法，获取文法中出现的所有的Vn和Vt                    *  
 *      (2) 将文法中的产生式拆分为左部、右部,其中右部采用vector进行保存  *
 *      (3) 构造所有Vn和Vt的first集和follow集                      * 
 *                                                              *
 ****************************************************************/
vector<string> grammar;          //存储文法
map<string, int> VN2int, VT2int; //VN、VT映射为下标索引
int symbolNum = 0;
map<string, bool> nullable; //各终结符或非终结符是否可空
set<string> first[50];      //存储各Vn和Vt的first集，没错，Vt也构造first集，就是其自身
set<string> follow[50];     //存储各Vn和Vt的follow集，Vt的follow都是空，240~263行取消注释可查看Vt的first和follow集

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
    for (int k = 0; k <= 46; k++)
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

bool allNullable(vector<string> Y, int left, int right)
{ //判断 Y[left]...Y[right]是否全可空
    if (left >= Y.size() || left > right || right < 0)
        return true;
    for (int i = left; i <= right; i++)
    {
        if (nullable[Y[i]] == false)
            return false;
    }
    return true;
}

void getFirstFollowSet()
{
    /*计算FIRST、FOLLOW、nullable的算法*/
    for (auto it = VT2int.begin(); it != VT2int.end(); it++)
    { //对每一个终结符Z，first[Z]={Z}
        string Vt = it->first;
        int Vt_index = it->second;
        first[Vt_index].insert(Vt);
    }
    for (int grammarIndex = 0; grammarIndex < grammar.size(); grammarIndex++)
    {
        //对于每个产生式：X->Y1Y2...Yk
        string X;
        vector<string> Y;
        int delimiterIndex = grammar[grammarIndex].find("->");
        X = grammar[grammarIndex].substr(0, delimiterIndex);                                                                       //以"->"为界，分隔产生式
        string rightGrama = grammar[grammarIndex].substr(delimiterIndex + 2, grammar[grammarIndex].length() - delimiterIndex - 2); //提取左部产生式
        Y = splitGrammarIntoYi(rightGrama);

        int k = Y.size();
        nullable["null"] = true;
        //如果所有Yi都是可空的，则nullable[X]=true
        if (allNullable(Y, 0, k - 1))
        {
            nullable[X] = true;
        }

        for (int i = 0; i < k; i++)
        {
            //如果Y0...Y(i-1)都是可空的(言外之意Yi不空),则first[X] = first[X]∪first[Yi] (1)
            if (nullable[Y[i]] == false && allNullable(Y, 0, i - 1))
            {
                if (i <= k - 1)
                {
                    set<string> setX = first[VN2int[X]];
                    //判断Yi是终结符还是非终结符
                    set<string> setY = VT2int.count(Y[i]) != 0 ? first[VT2int[Y[i]]] : first[VN2int[Y[i]]];
                    set_union(setX.begin(), setX.end(), setY.begin(), setY.end(), inserter(setX, setX.begin())); //(1)
                    first[VN2int[X]] = setX;
                }
            }
            //如果Y(i+1)...Yk都是可空的(言外之意Y0..Y(i-1)都不空)，则follow[Yi] = follow[Yi]∪follow[X] (2)
            if (allNullable(Y, i + 1, k - 1))
            {
                set<string> setX = follow[VN2int[X]];
                //判断Yi是终结符还是非终结符
                set<string> setY = VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]];
                set_union(setX.begin(), setX.end(), setY.begin(), setY.end(), inserter(setY, setY.begin()));
                VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]] = setY;
            }

            for (int j = i + 1; j < k; j++)
            {
                //如果Y(i+1)...Y(j-1)都是可空的(言外之意Yj不空),则follow[Yi] = follow[Yi]∪first[Yj] (3)
                if (nullable[Y[j]] == false && allNullable(Y, i + 1, j - 1))
                {
                    if (j <= k - 1)
                    {
                        set<string> setYi = VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]];
                        set<string> setYj = VT2int.count(Y[j]) ? first[VT2int[Y[j]]] : first[VN2int[Y[j]]];
                        set_union(setYi.begin(), setYi.end(), setYj.begin(), setYj.end(), inserter(setYi, setYi.begin()));
                        VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]] = setYi;
                    }
                }
            }
        }
    }
}

void converge()
{ //迭代计算first和follow集，直到收敛
    set<string> oldFirst[50];
    set<string> oldFollow[50];
    int isConverge = 1;
    string _vn = getVn(grammar[0].substr(0, 2));
    //这是一个可以手动修改的地方，很多教材的终止符不一样，这里统一一下，都用#作为终止符
    follow[VN2int[_vn]].insert("#");
    int times = 1; //经过多少轮才收敛
    do
    { //非终结符的first、follow不再变化则收敛
        isConverge = 1;
        getFirstFollowSet();
        //VN的状态
        for (auto it = VN2int.begin(); it != VN2int.end(); it++)
        {
            int vnindex = it->second;
            if (oldFirst[vnindex].size() != first[vnindex].size() || oldFollow[vnindex].size() != follow[vnindex].size())
                isConverge = 0;
            //保存旧状态，以便之后和新状态比较是否变化判断收敛与否
            oldFirst[vnindex] = first[vnindex];
            oldFollow[vnindex] = follow[vnindex];
        }
    } while (isConverge != 1);
    //输出最终结果
    cout << endl;
    for (auto it = VN2int.begin(); it != VN2int.end(); it++)
    {
        int vnindex = it->second;
        if (oldFirst[vnindex].size() != first[vnindex].size() || oldFollow[vnindex].size() != follow[vnindex].size())
        {
            isConverge = 0;
        }
        //输出状态
        cout << it->first << "的first集：\t";
        for (auto first_it = first[vnindex].begin(); first_it != first[vnindex].end(); first_it++)
        {
            cout << *first_it << " ";
        }
        cout << "\t" << it->first << "的follow集：\t";
        for (auto follow_it = follow[vnindex].begin(); follow_it != follow[vnindex].end(); follow_it++)
        {
            cout << *follow_it << " ";
        }
        cout << endl;
    }
}

/****************************************************************
 *                                                              *
 *                  第三部分：构造文法的LR1分析表                   *
 *          关键函数：(1) closure():计算项集的LR1闭包               *
 *                  (2) GOTO():由项集I读入Vn或Vt转向项集J          *
 *                                                              *
 ****************************************************************/
struct term
{                 //LR1项集内部的项
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
    //该文法使用SLR仍有冲突(哈工大mooc例子)，使用LR1分析无冲突
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

    /*
    //习题册P184习题,成功构造(另外发现：如果程序没错的话，就是题目给的答案有点问题)
    grammar.push_back("Z->S");
    grammar.push_back("S->L=R");
    grammar.push_back("S->R");
    grammar.push_back("R->L");
    grammar.push_back("L->*R");
    grammar.push_back("L->I");
    */
    //习题册P189习题
    /*
    grammar.push_back("S'->S");
    grammar.push_back("S->Aa");
    grammar.push_back("S->dAb");
    grammar.push_back("S->Bb");
    grammar.push_back("S->dBa");
    grammar.push_back("A->c");
    grammar.push_back("B->c");
    */
}

//该函数作用:项集I读入Vn或Vt可能会生成新的项集J，但也有可能指向已有项集，该函数就是判断是否指向已有项集
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
    if (firstTerm.dotPos == -1) //如果没有活前缀"·"
        firstTerm.dotPos = 0;   //就添加活前缀
    firstTerm.leftPart = X;
    firstTerm.rightPart = Y;
    firstTerm.subsequence.push_back("#");
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
            //只有闭包生成的新项B->gamma,不在集合I中才加入；在集合I中的只要相应的增加后继符
            int newTermFlag = -1; //先假设不在集合I中
            for (int k = 0; k < statusSet[statusNum].size(); k++)
            {
                if (newTerm.leftPart == statusSet[statusNum][k].leftPart && newTerm.rightPart == statusSet[statusNum][k].rightPart && newTerm.dotPos == statusSet[statusNum][k].dotPos)
                    newTermFlag = k;
            }
            if (newTermFlag == -1)
            { //不在集合I中就加入
                termQueue.push(newTerm);
                statusSet[statusNum].push_back(newTerm);
            }
            else
            { //如果新项B->gamma在集合I中，就新增其后继符(如果有新后继符的话)
                map<string, int> subsequenceMap;
                for (int m = 0; m < statusSet[statusNum][newTermFlag].subsequence.size(); m++)
                {
                    subsequenceMap[statusSet[statusNum][newTermFlag].subsequence[m]]++;
                }
                for (int m = 0; m < newTerm.subsequence.size(); m++)
                {
                    if (subsequenceMap[newTerm.subsequence[m]] == 0)
                        statusSet[statusNum][newTermFlag].subsequence.push_back(newTerm.subsequence[m]);
                }
            }
        }
        termQueue.pop();
    }
}

//GOTO函数无须变动，不管是LR0,SLR,LR1分析，都是从集合I读入一个符号经过闭包运算得到集合J。goto函数内部调用了closure()。
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
        cout << "┌───────────────────────┐" << endl;
        cout << "│      I" << i << "\t\t│" << endl;
        cout << "├───────────────────────┤" << endl;
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

        cout << "└───────────────────────┘" << endl;
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
                        actionTable[curStatus][VT2int["#"]] = 10000;
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

/***********************************************************************
 *                                                                     *
 * 第四部分：使用第三部分生成的LR1分析表对赋值表达式进行语法分析，               *
 *         边语法分析边进行语法制导翻译，生成四元式序列和汇编代码               *
 *                                                                     *
 *   (1) GEN():输出四元式；并将目的操作数与符号表中的相应符号进行绑定或注册      *
 *   (2) translate():对输入串边进行语法分析边翻译为四元式序列(规约时翻译)      *
 *   (3) translateToAssembly():根据语义子程序的汇编形式将四元式翻译为汇编代码  *
 *                                                                     *
 ***********************************************************************/

struct Symbol
{
    string varName;       //变量名
    string valueStr{"0"}; //变量的值，字符串形式,初始化为0
    int PLACE{-1};        //该变量在符号表中的位置,初始化为-1
};
struct FourYuanFormula
{                  //四元式结构体
    string op;     //操作符
    int arg1Index; //源操作数1的符号表地址
    int arg2Index; //源操作数2的符号表地址
    Symbol result; //目的操作数
};
vector<FourYuanFormula> formula; //四元式序列
vector<Symbol> symbolTable;      //符号表
map<string, int> ENTRY;          //用于查变量的符号表入口地址
int tempVarNum = 0;              //临时变量个数
Symbol newtemp()
{ //生成新的临时变量
    tempVarNum++;
    return Symbol{"T" + to_string(tempVarNum)};
}

void GEN(string op, int arg1, int arg2, Symbol &result)
{ //运算符、参数1在符号表的编号、参数2在符号表的编号，结果符号
    //产生一个四元式，并填入四元式序列表
    cout << "(" << op << ",";
    arg1 != -1 ? cout << symbolTable[arg1].varName : cout << "_";
    cout << ",";
    arg2 != -1 ? cout << symbolTable[arg2].varName : cout << "_";
    cout << "," << result.varName << ")" << endl;
    formula.push_back(FourYuanFormula{op, arg1, arg2, result}); //插入到四元式序列中
    if (op == "@")
    { //将临时变量result注册进入符号表
        result.varName;
        result.PLACE = symbolTable.size();
        result.valueStr = "-" + symbolTable[arg1].valueStr;
        symbolTable.push_back(result);
        ENTRY[result.varName] = result.PLACE;
    }
    if (op == "+")
    { //将临时变量result注册进入符号表
        result.PLACE = symbolTable.size();
        result.valueStr = to_string(stoi(symbolTable[arg1].valueStr) + stoi(symbolTable[arg2].valueStr));
        symbolTable.push_back(result);
        ENTRY[result.varName] = result.PLACE;
    }
    if (op == "-")
    { //将临时变量result注册进入符号表
        result.PLACE = symbolTable.size();
        result.valueStr = to_string(stoi(symbolTable[arg1].valueStr) - stoi(symbolTable[arg2].valueStr));
        symbolTable.push_back(result);
        ENTRY[result.varName] = result.PLACE;
    }
    if (op == "*")
    { //将临时变量result注册进入符号表
        result.PLACE = symbolTable.size();
        result.valueStr = to_string(stoi(symbolTable[arg1].valueStr) * stoi(symbolTable[arg2].valueStr));
        symbolTable.push_back(result);
        ENTRY[result.varName] = result.PLACE;
    }
    if (op == "/")
    { //将临时变量result注册进入符号表
        result.PLACE = symbolTable.size();
        result.valueStr = to_string(stoi(symbolTable[arg1].valueStr) / stoi(symbolTable[arg2].valueStr));
        symbolTable.push_back(result);
        ENTRY[result.varName] = result.PLACE;
    }
    if (op == "=") //这个result不是临时变量了，故不用注册进入符号表，只进行绑定
        result.valueStr = symbolTable[arg1].valueStr;
}

void translate()
{
    cout << "—————————————赋值表达式为四元式序列———————————————————" << endl;
    //需要三个栈：状态栈、符号栈、语义栈
    stack<int> status;      //状态栈
    stack<Symbol> op;       //符号栈
    stack<string> semantic; //语义栈
    int pointer = 0;        //输入串指针
    status.push(0);         //状态栈初始化
    op.push(Symbol{"#"});   //符号栈初始化,{name、value、place}
    while (!op.empty())
    {
        int curStatus = status.top();                      //当前状态
        string symbolToRead = lexicalTable[pointer].token; //读头符号
        //读头不论是变量还是数字都当做终结符i处理(因为分析表中只有i这个终结符可以代表这些)
        if (lexicalTable[pointer].syn == 1 || lexicalTable[pointer].syn == 2) //1是变量，2是整数
        {
            symbolToRead = "i";
            semantic.push(lexicalTable[pointer].token); //语义栈
            //将语义加入符号表，并添加入口地址映射
            Symbol tempSym; //讲道理，如果是变量应该只有变量名没有值(至少在未初始化和未赋值前)，而整数应该只有值而没有变量名。为兼顾两者，有如下处理
            tempSym.varName = lexicalTable[pointer].token;
            tempSym.valueStr = lexicalTable[pointer].token;
            tempSym.PLACE = symbolTable.size();
            symbolTable.push_back(tempSym);
            ENTRY[tempSym.varName] = tempSym.PLACE;
        }
        //goto原状态下标，移进项+100，规约项+1000，接受态10000
        if (actionTable[curStatus][VT2int[symbolToRead]] >= 100 && actionTable[curStatus][VT2int[symbolToRead]] < 1000)
        { //由上一个状态读入一个终结符转入新状态
            status.push(actionTable[curStatus][VT2int[symbolToRead]] - 100);
            op.push(Symbol{symbolToRead});
            pointer++;
        }
        else if (actionTable[curStatus][VT2int[symbolToRead]] >= 1000 && actionTable[curStatus][VT2int[symbolToRead]] < 10000)
        { //规约项，由上一个状态读入一个终结符
            int reduceGrammaIndex = actionTable[curStatus][VT2int[symbolToRead]] - 1000;
            term reduceTerm;
            split(grammar[reduceGrammaIndex], reduceTerm.leftPart, reduceTerm.rightPart);

            if (reduceGrammaIndex == 1)
            { //A->i=E
                Symbol E, i;
                for (int popTime = 0; popTime < 3; popTime++)
                {
                    if (popTime == 0)
                        E = op.top();
                    if (popTime == 2)
                        i = op.top();
                    op.pop();
                    status.pop();
                }
                op.push(Symbol{reduceTerm.leftPart});
                curStatus = status.top();
                status.push(gotoTable[curStatus][VN2int[op.top().varName]]);

                GEN("=", E.PLACE, -1, i);
            }
            else if (reduceGrammaIndex == 2)
            { //E->@E
                Symbol E1 = op.top();
                int popNum = reduceTerm.rightPart.size();
                while (popNum)
                {
                    status.pop();
                    op.pop();
                    popNum--;
                }
                op.push(Symbol{reduceTerm.leftPart});
                curStatus = status.top();
                status.push(gotoTable[curStatus][VN2int[op.top().varName]]);

                Symbol T = newtemp();
                GEN("@", E1.PLACE, -1, T);
                semantic.pop(); //更新语义栈
                semantic.push(T.varName);
                op.top().PLACE = T.PLACE;
                op.top().valueStr = T.valueStr;
            }
            else if (reduceGrammaIndex >= 3 && reduceGrammaIndex <= 6)
            { //E->E 加减乘除 E ，规约
                Symbol E1, E2;
                for (int popTime = 0; popTime < 3; popTime++)
                { //E->E + E 要出栈三次，进栈一次
                    if (popTime == 0)
                        E2 = op.top();
                    if (popTime == 2)
                        E1 = op.top();
                    op.pop();
                    status.pop();
                }
                op.push(Symbol{reduceTerm.leftPart}); //把"E"push进符号栈，但还没和语义栈关联，下面进行
                curStatus = status.top();
                status.push(gotoTable[curStatus][VN2int[op.top().varName]]);

                Symbol T = newtemp();
                string opStr[4] = {"+", "-", "*", "/"};
                GEN(opStr[reduceGrammaIndex - 3], E1.PLACE, E2.PLACE, T);
                semantic.pop(); //更新语义栈
                semantic.pop();
                semantic.push(T.varName);
                op.top().PLACE = T.PLACE;
                op.top().valueStr = T.valueStr;
            }
            else if (reduceGrammaIndex == 7)
            { //E->(E)
                Symbol E1;
                for (int popTime = 0; popTime < 3; popTime++)
                {
                    if (popTime == 1)
                        E1 = op.top();
                    op.pop();
                    status.pop();
                }
                op.push(Symbol{reduceTerm.leftPart}); //把"E"push进符号栈，但还没和语义栈关联，下面进行
                curStatus = status.top();
                status.push(gotoTable[curStatus][VN2int[op.top().varName]]);

                op.top().PLACE = ENTRY[semantic.top()]; //E.PLACE = ENTRY(i)
                semantic.pop();
            }
            else if (reduceGrammaIndex == 8)
            { //E->i
                int popNum = reduceTerm.rightPart.size();
                while (popNum)
                {
                    status.pop();
                    op.pop();
                    popNum--;
                }
                op.push(Symbol{reduceTerm.leftPart}); //把"E"push进符号栈，但还没和语义栈关联，下面进行
                curStatus = status.top();
                status.push(gotoTable[curStatus][VN2int[op.top().varName]]);
                //E->i规约,将E与语义栈栈顶值关联之后
                op.top().PLACE = ENTRY[semantic.top()]; //E.PLACE = ENTRY(i)
            }
        }
        else if (actionTable[curStatus][VT2int[symbolToRead]] == 10000)
        { //接受状态
            cout << "该表达式输入串能够被成功分析，acc！" << endl;
            status.pop();
            op.pop();
            status.pop();
            op.pop();
        }
    }
}

//将四元式序列翻译为目标程序代码(汇编)
void translateToAssembly()
{
    cout << "—————————————四元式序列翻译为汇编指令序列———————————————————" << endl;
    for (int i = 0; i < formula.size(); i++)
    {
        string op = formula[i].op;
        int arg1 = formula[i].arg1Index;
        int arg2 = formula[i].arg2Index;
        Symbol result = formula[i].result;

        if (op == "=")
        {
            cout << "MOV R0," << symbolTable[arg1].varName << endl;
            cout << "MOV " << result.varName << ",R0" << endl;
        }
        else if (op == "@")
        {
            cout << "MOV R0," << symbolTable[arg1].varName << endl;
            cout << "NEG R0" << endl;
            cout << "MOV " << result.varName << ",R0" << endl;
        }
        else if (op == "+")
        {
            cout << "MOV R0," << symbolTable[arg1].varName << endl;
            cout << "ADD R0," << symbolTable[arg2].varName << endl;
            cout << "MOV " << result.varName << " R0" << endl;
        }
        else if (op == "-")
        {
            cout << "MOV R0," << symbolTable[arg1].varName << endl;
            cout << "SUB R0," << symbolTable[arg2].varName << endl;
            cout << "MOV " << result.varName << " R0" << endl;
        }
        else if (op == "*")
        {
            cout << "MOV AL," << symbolTable[arg1].varName << endl;
            cout << "MOV BL," << symbolTable[arg2].varName << endl;
            cout << "MUL BL" << endl;
            cout << "MOV " << result.varName << ",AX" << endl;
        }
        else if (op == "/")
        {
            cout << "MOV AX," << symbolTable[arg1].varName << endl;
            cout << "MOV BH," << symbolTable[arg2].varName << endl;
            cout << "DIV BH" << endl; //除法运算后，AH余数，AL商
            cout << "MOV " << result.varName << ",AL" << endl;
        }
    }
}

int main()
{
    initGrammar(); //初始化文法
    //VT2int["$"] = 0; //文法中没有$符号，人为增加该终结符
    readVnAndVt();        //读取文法中所有的VN和VT
    converge();           //构造first和follow集
    constructStatusSet(); //构造LR(1)分析表
    string code;
    ifstream myfile("ex4Data.txt");
    cout << "—————————————词法分析———————————————————" << endl;
    while (getline(myfile, code)) //按行读取文件，可读取空格
        scanner(code);            //词法分析结束，分析结果存储在lexicalTable中
    translate();                  //赋值表达式源程序翻译为四元式序列
    translateToAssembly();        //将四元式序列翻译为汇编代码
    return 0;
}
