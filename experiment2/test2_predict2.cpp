//增加可空集判断，注释代码删除
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include "../Util/parse.cpp"
using namespace std;

map<string, bool> nullable;      //各终结符或非终结符是否可空
vector<string> grammar;          //存储文法
set<string> first[50];           //存储各Vn和Vt的first集，没错，Vt也构造first集，就是其自身
set<string> follow[50];          //存储各Vn和Vt的follow集，Vt的follow都是空，240~263行取消注释可查看Vt的first和follow集
string VN[50], VT[50];           //非终结符，终结符
map<string, int> VN2int, VT2int; //VN、VT映射为下标索引
int symbolNum = 0;
int PreAnalyseTable[50][50]; //预测分析表

void initGrammar()
{
    grammar.push_back("E->TE'");
    grammar.push_back("E'->+TE'");
    grammar.push_back("E'->null");
    grammar.push_back("T->FT'");
    grammar.push_back("T'->*FT'");
    grammar.push_back("T'->null");
    grammar.push_back("F->(E)");
    grammar.push_back("F->i");
}

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
    for (int k = 1; k <= 44; k++)
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
                {
                    VN2int[Vn] = ++symbolNum;
                    VN[symbolNum] = Vn; //可通过索引下标输出对应非终结符名字
                }
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
                { //该终结符第一次出现,将该终结符映射为下标索引
                    VT2int[Vt] = ++symbolNum;
                    VT[symbolNum] = Vt; //可通过索引下标输出对应终结符名字
                }
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
{

    set<string> oldFirst[50];
    set<string> oldFollow[50];
    int isConverge = 1;
    follow[VN2int["E"]].insert("#");
    int times = 1; //经过多少轮才收敛
    do
    { //非终结符的first、follow不再变化则收敛
        cout << "——————————————————————————————————————————————————————" << endl;
        cout << "第" << times++ << "次遍历" << endl;
        isConverge = 1;
        getFirstFollowSet();
        //VN的状态
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
            //保存旧状态，以便之后和新状态比较是否变化判断收敛与否
            oldFirst[vnindex] = first[vnindex];
            oldFollow[vnindex] = follow[vnindex];
        }
        /*
        //查看终结符VT的first和follow集
        for (auto it = VT2int.begin(); it != VT2int.end(); it++)
        {
            int vnindex = it->second;
            cout << it->first << "的first集：\t";
            for (auto first_it = first[vnindex].begin(); first_it != first[vnindex].end(); first_it++)
            {
                cout << *first_it << " ";
            }
            cout << "\t" << it->first << "的follow集：";
            for (auto follow_it = follow[vnindex].begin(); follow_it != follow[vnindex].end(); follow_it++)
            {
                cout << *follow_it << " ";
            }
            cout << endl;
        }
        cout << endl;
        //查看nullable集
        for (auto it = nullable.begin(); it != nullable.end(); it++)
        {
            cout << "符号:\t" << it->first << " \t可空:" << it->second << endl;
        }
    */
    } while (isConverge != 1);
}
/*
void constructSelect()
{ //TODO:构造Select
    for (int i = 0; i < 8; i++)
    { //X->ABC，如果A是VN且VN不含空，selecl[X]=first[A];如果A是VT,select[X] = first[A]={A};如果ABC都可以取空，select[X] = follow[X]
        int delimiterIndex = grammar[i].find("->");
        string X = grammar[i].substr(0, delimiterIndex);                                                     //以"->"为界，分隔产生式
        string rightGrama = grammar[i].substr(delimiterIndex + 2, grammar[i].length() - delimiterIndex - 2); //提取右部产生式
        string A;
        if (rightGrama[0] >= 'A' && rightGrama[0] <= 'Z')
        { //A是VN
            A = getVn(rightGrama.substr(0, 2));
            set<string> setA = first[VN2int[A]];
            set_union(setA.begin(), setA.end(), select[VN2int[X]].begin(), select[VN2int[X]].end(), inserter(select[VN2int[X]], select[VN2int[X]].begin()));
        }
        else
        { //A是VT
            A = getVt(rightGrama.substr(0, rightGrama.length()));
            set_union(first[VT2int[A]].begin(), first[VT2int[A]].end(), select[VN2int[X]].begin(), select[VN2int[X]].end(), inserter(select[VN2int[X]], select[VN2int[X]].begin()));
        }

        if (nullable[A] == true)
        {
        }
    }
}
*/
void constructPAT()
{ //construct Predict Analyse Table,构造预测分析表
    fill(PreAnalyseTable[0], PreAnalyseTable[0] + 50 * 50, -1);
    for (int i = 0; i < grammar.size(); i++)
    {
        int delimiterIndex = grammar[i].find("->");
        string X = grammar[i].substr(0, delimiterIndex);                                                     //提取每个产生式的左部非终结符
        string rightGrama = grammar[i].substr(delimiterIndex + 2, grammar[i].length() - delimiterIndex - 2); //提取右部产生式

        int row = VN2int[X];
        int col;
        if (rightGrama[0] >= 'A' && rightGrama[0] <= 'Z')
        {
            string Vn = getVn(rightGrama.substr(0, 2));
            if (nullable[Vn] != true)
            {
                for (auto it = first[VN2int[Vn]].begin(); it != first[VN2int[Vn]].end(); it++)
                {
                    col = VT2int[*it];
                    PreAnalyseTable[row][col] = i;
                }
            }
            else
            {
                for (auto it = follow[VN2int[Vn]].begin(); it != follow[VN2int[Vn]].end(); it++)
                {
                    col = VT2int[*it];
                    PreAnalyseTable[row][col] = i;
                }
            }
        }
        else
        {
            string Vt = getVt(rightGrama.substr(0, rightGrama.length()));
            if (nullable[Vt] != true)
            {
                col = VT2int[Vt];
                PreAnalyseTable[row][col] = i;
            }
            else
            {
                for (auto it = follow[VN2int[X]].begin(); it != follow[VN2int[X]].end(); it++)
                {
                    col = VT2int[*it];
                    PreAnalyseTable[row][col] = i;
                }
            }
        }
    }
    //输出预测分析表
    cout << "———————————————————预测分析表———————————————————————" << endl;
    for (auto Vt_it = VT2int.begin(); Vt_it != VT2int.end(); Vt_it++)
    {
        int col = Vt_it->second; //终结符作为列索引
        cout << "\t" << Vt_it->first;
    }
    cout << endl;
    for (auto Vn_it = VN2int.begin(); Vn_it != VN2int.end(); Vn_it++)
    {
        int row = Vn_it->second; //非终结符作为行索引
        cout << Vn_it->first << " ";
        for (auto Vt_it = VT2int.begin(); Vt_it != VT2int.end(); Vt_it++)
        {
            int col = Vt_it->second; //终结符作为列索引
            if (PreAnalyseTable[row][col] == -1)
                cout << "\t-";
            else
                cout << "\t" << PreAnalyseTable[row][col];
        }
        cout << endl;
    }
}

int main()
{
    initGrammar();  //初始化文法
    readVnAndVt();  //读取文法中所有的VN和VT
    converge();     //构造VN的first和follow集直到收敛
    constructPAT(); //构造预测分析表

    string code;
    ifstream myfile("ex2Data.txt");
    cout << "—————————————词法分析———————————————————" << endl;
    while (getline(myfile, code)) //按行读取文件，可读取空格
        scanner(code);            // 词法分析结束，分析结果存储在lexicalTable中
    cout << "—————————————语法分析———————————————————" << endl;
    stack<string> op;
    op.push("#");
    op.push("E");
    int pointer = 0; //输入串指针
    while (!op.empty())
    {
        string X = op.top(); //下推栈PDA的栈顶符号
        if (X == lexicalTable[pointer].token || VT2int.count(X) != 0 && lexicalTable[pointer].syn == 1)
        { //若栈顶符号和读头符号匹配：栈顶弹出，读头指针后移一位
            cout << "匹配:" << X << "==" << lexicalTable[pointer].token;
            op.pop();
            pointer++;
        }
        else if (VT2int.count(X) != 0)
        { //若栈顶符号是终结符，却不和读头符号匹配，出错
            //注：本程序认定“null”也属于终结符，如果栈顶的终结符是“null”，则弹出
            if (X == "null")
            {
                op.pop();
                cout << "弹出NULL";
            }
            else
            {
                cout << "error：栈顶和读头符号不匹配" << endl;
                break;
            }
        }
        else if (VT2int.count(lexicalTable[pointer].token) != 0 && PreAnalyseTable[VN2int[X]][VT2int[lexicalTable[pointer].token]] == -1)
        { //预测分析表不存在的推导
            cout << "error:预测分析表不存在该推导" << endl;
            break;
        }
        else
        {
            int row = VN2int[X];
            int col;
            if (VT2int.count(lexicalTable[pointer].token) != 0)
            { //输入串读头符号的非终结符是存在于文法中的
                col = VT2int[lexicalTable[pointer].token];
            }
            else if (lexicalTable[pointer].syn == 1)
            { //由于文法中的i广泛的代表变量名，所以如果读头符号是非"i"的其他变量(不在文法终结符表中的终结符)，一律认为就是i
                col = VT2int["i"];
            }

            string nextGrammar = grammar[PreAnalyseTable[row][col]];
            int delimiterIndex = nextGrammar.find("->");
            string rightGrama = nextGrammar.substr(delimiterIndex + 2, nextGrammar.length() - delimiterIndex - 2); //提取右部产生式
            vector<string> Y = splitGrammarIntoYi(rightGrama);
            cout << "推导:" << X << "->";
            for (int i = 0; i < Y.size(); i++)
            {
                cout << Y[i];
            }
            op.pop(); //弹出栈顶符号，将Yk,Yk-1,....,Y0压栈
            for (int i = Y.size() - 1; i >= 0; i--)
            {
                op.push(Y[i]);
            }
        }
        //每次动作之后查看下推栈的情况
        stack<string> tmp;
        cout << "\t\t下推栈:";
        while (!op.empty())
        {
            string tmpTop = op.top();
            cout << tmpTop;
            tmp.push(tmpTop);
            op.pop();
        }
        cout << endl;
        while (!tmp.empty())
        {
            op.push(tmp.top());
            tmp.pop();
        }
    }

    return 0;
}