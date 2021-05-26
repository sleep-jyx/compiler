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

vector<string> grammar;          //存储文法
map<string, int> VN2int, VT2int; //VN、VT映射为下标索引
int symbolNum = 0;
set<string> firstVT[50], lastVT[50];
int PriorityTable[50][50]; //算符优先关系表,行列都是VT
int f[50], g[50];          //优先函数表

void initGrammar()
{ /*
    grammar.push_back("E->TE'");
    grammar.push_back("E'->+TE'");
    grammar.push_back("E'->null");
    grammar.push_back("T->FT'");
    grammar.push_back("T'->*FT'");
    grammar.push_back("T'->null");
    grammar.push_back("F->(E)");
    grammar.push_back("F->i");
    */
    //如果要把'#'也作为算符进入优先表，这一行必须要加，而且必须是作为grammar[0]，
    //因为在getFirstVTAndLastVT()是跳过了grammar[0],防止'#'参与firstVT和lastVT的构建
    //一句话就是：对于'#',只需要它成为终结符，不需要参与firstVT和lastVT的构建
    grammar.push_back("E->#E#");
    grammar.push_back("E->E+T");
    grammar.push_back("E->E-T");
    grammar.push_back("E->T");
    grammar.push_back("T->T*F");
    grammar.push_back("T->T/F");
    grammar.push_back("T->F");
    grammar.push_back("F->(E)");
    //grammar.push_back("F->i");

    /*
    grammar.push_back("S'->#S#");
    grammar.push_back("S->D(E)");
    grammar.push_back("E->R;P");
    grammar.push_back("E->P");
    grammar.push_back("P->S");
    grammar.push_back("P->i");
    grammar.push_back("D->i");
    */
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
void getFirstVTAndLastVT()
{
    set<string> oldFirstVT[50];
    set<string> oldLastVT[50];
    int isConverge = 1;
    do
    { //非终结符的firstVT、lastVT不再变化则收敛
        isConverge = 1;
        //grammar[0]="E->#E#"，只需要'#'参与成为终结符(在readVnAndVt()部分已经完成)，不要再参与firstVT和lastVT的计算了
        for (int grammarIndex = 1; grammarIndex < grammar.size(); grammarIndex++)
        { //对于每个产生式：X->Y1Y2...Yk
            string X;
            vector<string> Y;
            int delimiterIndex = grammar[grammarIndex].find("->");
            X = grammar[grammarIndex].substr(0, delimiterIndex);                                                                       //以"->"为界，分隔产生式
            string rightGrama = grammar[grammarIndex].substr(delimiterIndex + 2, grammar[grammarIndex].length() - delimiterIndex - 2); //提取左部产生式
            Y = splitGrammarIntoYi(rightGrama);

            // 求firstVT
            set<string> P = firstVT[VN2int[X]];
            if (VT2int.count(Y[0]) != 0)
            { //if (P->a...)then a->FIRSTVT(P)
                set<string> seta{Y[0]};
                set_union(seta.begin(), seta.end(), P.begin(), P.end(), inserter(P, P.begin()));
            }
            if (VT2int.count(Y[0]) == 0 && Y.size() > 1 && VT2int.count(Y[1]) != 0)
            { //if (P->Qa...)then a->FIRSTVT(P)
                set<string> seta{Y[1]};
                set_union(seta.begin(), seta.end(), P.begin(), P.end(), inserter(P, P.begin()));
            }
            if (VT2int.count(Y[0]) == 0)
            { //if (P->Q...)then	 FIRSTVT(Q)->FIRSTVT(P)
                set<string> Q = firstVT[VN2int[Y[0]]];
                set_union(Q.begin(), Q.end(), P.begin(), P.end(), inserter(P, P.begin()));
            }
            firstVT[VN2int[X]] = P; //别忘记更新

            //求lastVT
            P = lastVT[VN2int[X]];
            if (VT2int.count(Y[Y.size() - 1]) != 0)
            { //if (P->...a)then a->LASTVT(P)
                set<string> seta{Y[Y.size() - 1]};
                set_union(seta.begin(), seta.end(), P.begin(), P.end(), inserter(P, P.begin()));
            }
            if (VT2int.count(Y[Y.size() - 1]) == 0 && Y.size() > 1 && VT2int.count(Y[Y.size() - 2]) != 0)
            { //if (P->...aQ)then a->LASTVT(P)
                set<string> seta{Y[Y.size() - 2]};
                set_union(seta.begin(), seta.end(), P.begin(), P.end(), inserter(P, P.begin()));
            }
            if (VT2int.count(Y[Y.size() - 1]) == 0)
            { //if (P->...Q)then	 LASTVT(Q)->LASTVT(P)
                set<string> Q = lastVT[VN2int[Y[Y.size() - 1]]];
                set_union(Q.begin(), Q.end(), P.begin(), P.end(), inserter(P, P.begin()));
            }
            lastVT[VN2int[X]] = P; //别忘记更新
        }
        // 检查是否收敛
        for (auto it = VN2int.begin(); it != VN2int.end(); it++)
        {
            int vnindex = it->second;
            if (oldFirstVT[vnindex].size() != firstVT[vnindex].size() || oldLastVT[vnindex].size() != lastVT[vnindex].size())
                isConverge = 0;
            //保存旧状态，以便之后和新状态比较是否变化判断收敛与否
            oldFirstVT[vnindex] = firstVT[vnindex];
            oldLastVT[vnindex] = lastVT[vnindex];
        }
    } while (isConverge != 1);
    // 输出firstVN和lastVN
    cout << "\tfirstVT\t\tlastVT\t" << endl;
    for (auto it = VN2int.begin(); it != VN2int.end(); it++)
    {
        cout << it->first << "\t";
        for (auto it2 = firstVT[it->second].begin(); it2 != firstVT[it->second].end(); it2++)
            cout << *it2;
        cout << "\t\t";
        for (auto it2 = lastVT[it->second].begin(); it2 != lastVT[it->second].end(); it2++)
            cout << *it2;
        cout << endl;
    }
}

//构造算符优先关系表
void constructPriorityTable()
{
    //grammar.push_back("E->#E#"); //这是为了让'#'也参与到优先表中，但同时保证'#'不出现在各非终结符的firstVT和lastVT中
    //VT2int["#"] = ++symbolNum;   //把'#'读到终结符中去
    for (int i = 0; i < grammar.size(); i++)
    {
        string X;
        vector<string> Y;
        int delimiterIndex = grammar[i].find("->");
        X = grammar[i].substr(0, delimiterIndex);
        string rightGrama = grammar[i].substr(delimiterIndex + 2, grammar[i].length() - delimiterIndex - 2); //提取左部产生式
        Y = splitGrammarIntoYi(rightGrama);
        if (Y.size() == 1)
            continue;
        for (int j = 0; j < Y.size() - 1; j++)
        { //若出现 E→...ab...的情况：a = b
            if (VT2int.count(Y[j]) != 0 && VT2int.count(Y[j + 1]) != 0)
                PriorityTable[VT2int[Y[j]]][VT2int[Y[j + 1]]] = 10;

            //若出现 E→...aA.. 且 b∈FIRSTVT【A】的情况：a < b
            if (VT2int.count(Y[j]) != 0 && VN2int.count(Y[j + 1]) != 0)
            {
                for (auto it = firstVT[VN2int[Y[j + 1]]].rbegin(); it != firstVT[VN2int[Y[j + 1]]].rend(); it++)
                {
                    int col = VT2int[*it];
                    PriorityTable[VT2int[Y[j]]][col] = -1;
                }
            }
            //若出现 E→...Ab...且 a∈LASTVT【A】的情况：a > b
            if (VN2int.count(Y[j]) != 0 && VT2int.count(Y[j + 1]) != 0)
            {
                for (auto it = lastVT[VN2int[Y[j]]].rbegin(); it != lastVT[VN2int[Y[j]]].rend(); it++)
                {
                    int row = VT2int[*it];
                    PriorityTable[row][VT2int[Y[j + 1]]] = 1;
                }
            }
            //若出现  E→....aAb... 的情况：a = b
            if (VT2int.count(Y[j]) != 0 && VN2int.count(Y[j + 1]) != 0 && j + 2 <= Y.size() - 1 && VT2int.count(Y[j + 2]) != 0)
                PriorityTable[VT2int[Y[j]]][VT2int[Y[j + 2]]] = 10;
        }
    }
    //grammar.pop_back();
    //输出算符优先分析表
    cout << "———————————————————算符优先分析表———————————————————————" << endl;
    for (auto Vt_it = VT2int.rbegin(); Vt_it != VT2int.rend(); Vt_it++)
    {
        int col = Vt_it->second; //终结符作为列索引
        cout << "\t" << Vt_it->first;
    }
    cout << endl;

    for (auto Vt_it = VT2int.rbegin(); Vt_it != VT2int.rend(); Vt_it++)
    {
        int row = Vt_it->second; //终结符作为行索引
        cout << Vt_it->first << " ";
        for (auto Vt_it2 = VT2int.rbegin(); Vt_it2 != VT2int.rend(); Vt_it2++)
        {
            int col = Vt_it2->second; //终结符作为列索引
            if (PriorityTable[row][col] == 10)
                cout << "\t=";
            else if (PriorityTable[row][col] == -1)
                cout << "\t<";
            else if (PriorityTable[row][col] == 1)
                cout << "\t>";
            else
                cout << "\t" << PriorityTable[row][col];
        }
        cout << endl;
    }
}

//构造优先函数表
void constructPriorityFuncTable()
{
    int oldf[50];
    int oldg[50];
    int isConverge = 1;
    fill(f, f + 50, 1);
    fill(g, g + 50, 1);
    do
    {
        isConverge = 1;
        for (auto Vt_it = VT2int.begin(); Vt_it != VT2int.end(); Vt_it++)
        {
            int a = Vt_it->second;
            for (auto Vt_it2 = VT2int.begin(); Vt_it2 != VT2int.end(); Vt_it2++)
            {
                int b = Vt_it2->second;
                //if a > b but f(a)<=g(b) then f(a)=g(b)+1;
                if (PriorityTable[a][b] == 1 && f[a] <= g[b])
                    f[a] = g[b] + 1;
                //if a < b but f(a)>=g(b) then g(b)=f(a)+1;
                if (PriorityTable[a][b] == -1 && f[a] >= g[b])
                    g[b] = f[a] + 1;
                //if a = b but f(a)!=g(b) then g(b)=f(a)=max(f(a),g(b));
                if (PriorityTable[a][b] == 10 && f[a] != g[b])
                {
                    int t = max(f[a], g[b]);
                    f[a] = g[b] = t;
                }
            }
        }

        // 检查是否收敛
        for (auto Vt_it = VT2int.begin(); Vt_it != VT2int.end(); Vt_it++)
        {
            int index = Vt_it->second;
            if (oldf[index] != f[index] || oldg[index] != g[index])
                isConverge = 0;
            //保存旧状态，以便之后和新状态比较是否变化判断收敛与否
            copy(f, f + 50, oldf);
            copy(g, g + 50, oldg);
        }
    } while (isConverge != 1);

    //输出优先函数
    cout << "———————————————————优先函数———————————————————————" << endl;
    for (auto Vt_it = VT2int.rbegin(); Vt_it != VT2int.rend(); Vt_it++)
        cout << Vt_it->first << " ";
    cout << endl;
    for (auto Vt_it = VT2int.rbegin(); Vt_it != VT2int.rend(); Vt_it++)
        cout << f[Vt_it->second] << " ";
    cout << endl;
    for (auto Vt_it = VT2int.rbegin(); Vt_it != VT2int.rend(); Vt_it++)
        cout << g[Vt_it->second] << " ";
    cout << endl;
}

double cal(double a, double b, string c)
{
    if (c == "+")
        return a + b;
    else if (c == "-")
        return a - b;
    else if (c == "*")
        return a * b;
    else if (c == "/")
        return a / b;
}

void opPriorityAnalyze(int choice) //choice = 1：使用算符优先关系表；choice=2:使用优先函数
{
    stack<double> opnd; //操作数栈
    stack<string> optr; //运算符栈
    optr.push("#");
    for (int i = 0; i < lexicalTableLen; i++)
    {
        if (lexicalTable[i].syn == 2 || lexicalTable[i].syn == 3)
        { //2整数，3实数
            double num = stod(lexicalTable[i].token);
            opnd.push(num);
        }
        else
        { //除数字以外都算是操作符。运算符栈不空时，比较栈顶运算符优先级
            string curOP = lexicalTable[i].token;
            string lastOP = optr.top();
            if (curOP == ")" || curOP == "#")
            {
                while (curOP == ")" && optr.top() != "(" || curOP == "#" && optr.top() != "#")
                {
                    double a, b;
                    string c;
                    b = opnd.top();
                    opnd.pop();
                    a = opnd.top();
                    opnd.pop();
                    c = optr.top();
                    optr.pop();
                    opnd.push(cal(a, b, c));
                }
                optr.pop(); //弹出'('
            }
            else if (choice == 1 ? PriorityTable[VT2int[curOP]][VT2int[lastOP]] == 1 || PriorityTable[VT2int[lastOP]][VT2int[curOP]] == -1 : f[VT2int[curOP]] > g[VT2int[lastOP]] || f[VT2int[lastOP]] < g[VT2int[curOP]])
            { //加||换位的原因：由于有几个运算符号之间没有直接关系，以防(行，列)无对应关系，找(列，行)的对应关系
                //注：优先关系不是四则运算，比如 + < ( 的同时 ( < + ，
                //读头算符优先级高，则加入算符栈
                optr.push(curOP);
            }
            else if (choice == 1 ? PriorityTable[VT2int[curOP]][VT2int[lastOP]] == -1 : f[VT2int[curOP]] < g[VT2int[lastOP]])
            { //读头算符优先级低，取出两个运算数和一个运算符进行计算
                double a, b;
                string c;
                b = opnd.top();
                opnd.pop();
                a = opnd.top();
                opnd.pop();
                c = optr.top();
                optr.pop();
                opnd.push(cal(a, b, c));
                optr.push(curOP);
            }
        }
        //每次动作之后查看下推栈的情况
        cout << "————————";
        for (int j = 0; j < lexicalTableLen; j++)
        {
            if (j == i)
                cout << "\"";
            cout << lexicalTable[j].token;
            if (j == i)
                cout << "\"";
        }
        cout << "————————" << endl;
        stack<double> tmp;
        cout << "\t操作数下推栈:";
        while (!opnd.empty())
        {
            double tmpTop = opnd.top();
            tmp.push(tmpTop);
            opnd.pop();
        }
        while (!tmp.empty())
        {
            cout << tmp.top() << " ";
            opnd.push(tmp.top());
            tmp.pop();
        }
        cout << endl;
        stack<string> tmp2;
        cout << "\t运算符下推栈:";
        while (!optr.empty())
        {
            string tmpTop = optr.top();
            //cout << tmpTop;
            tmp2.push(tmpTop);
            optr.pop();
        }
        while (!tmp2.empty())
        {
            cout << tmp2.top() << " ";
            optr.push(tmp2.top());
            tmp2.pop();
        }
        cout << endl;
    }
    if (optr.empty())
    {
        cout << "————————*识别成功:";
        for (int j = 0; j < lexicalTableLen - 1; j++)
            cout << lexicalTable[j].token;
        cout << " = " << opnd.top() << endl;
    }
    else
        cout << "————————*识别失败*————————" << endl;
}
int main()
{
    initGrammar();                //初始化文法
    readVnAndVt();                //读取文法中所有的VN和VT
    getFirstVTAndLastVT();        //获取非终结符的firstVt和lastVT
    constructPriorityTable();     //由文法构造算符优先关系表表
    constructPriorityFuncTable(); //由算符优先关系表构造优先函数
    string code;
    ifstream myfile("ex3Data.txt");
    cout << "—————————————词法分析———————————————————" << endl;
    while (getline(myfile, code)) //按行读取文件，可读取空格
        scanner(code);            // 词法分析结束，分析结果存储在lexicalTable中

    cout << "————————算符优先分析过程————————" << endl;
    opPriorityAnalyze(1); //1使用算符优先关系表分析，2使用优先函数分析
    return 0;
}