//预测分析法初稿，拉跨
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include "../Util/parse.cpp"
using namespace std;

class Rule
{
public:
    string rule[50];  //文法规则
    word *sentence;   //经过词法分析后的句子
    int front = 0;    //读头下标
    word currentWord; //当前读头读到的单词]

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
/*
void read() //读终结符、非终结符、产生式
{
    char c;
    int i = 0;
    int n = 0;
    cout << "输入产生式集合（空字用‘@’表示）,输入一条后换行，以‘end’结束:" << endl;
    string ss;
    string dd;
    int j = 0;
    int y = 0;
    while (cin >> ss && ss != "end")
    {
        dd.clear();
        dd += ss[0];
        proce[j] += dd;
        for (i = 3; i < ss.length(); i++)
        {
            if (ss[i] != '|')
            {
                dd.clear();
                dd += ss[i];
                proce[j] += dd;
            }
            else
            {
                dd.clear();
                dd += ss[0];
                dd += ss[++i];
                proce[++j] += dd;
            }
        }
        j++;
    }

    getnum['#'] = 0; //#代表结束标志
    //	getzf[0]='#';没有定义数组大小的时候这样输入是错误的
    getzf.push_back('#');
    //终结符压栈
    for (int i = 0; i < proce.size(); i++)
    {
        for (int k = 0; k < proce[i].length(); k++)
        {
            if (proce[i][k] != '-' && proce[i][k] != '|')
            {
                if (proce[i][k] < 64 || proce[i][k] > 90)
                {
                    for (y = 0; y < getzf.size(); y++)
                    {
                        if (proce[i][k] == getzf[y])
                            break;
                    }
                    if (y == getzf.size() && k != 2)
                    { //这里让k!=2是不让第三位置的>进去
                        getnum[proce[i][k]] = ++n;
                        getzf.push_back(proce[i][k]);
                    }
                }
            }
        }
    }
    getnum['@'] = ++n;
    numv = n; //终结符的数量等于当前n的值
    getzf.push_back('@');
    //非终结符压栈
    for (int i = 0; i < proce.size(); i++)
    {
        for (int k = 0; k < proce[i].length(); k++)
        {
            if (proce[i][k] != '-' && proce[i][k] != '|' && proce[i][k] != '>')
            {
                if (proce[i][k] > 64 && proce[i][k] < 91)
                {
                    for (y = 0; y < getzf.size(); y++)
                    {
                        if (proce[i][k] == getzf[y])
                            break;
                    }
                    if (y == getzf.size())
                    {
                        getnum[proce[i][k]] = ++n;
                        num = n; //终结符加非终结符的数量等于当前i的值
                        getzf.push_back(proce[i][k]);
                    }
                }
            }
        }
    }
}
*/

map<string, bool> nullable;
string grammar[50];
set<string> first[50];
set<string> follow[50];
//set<string> nullable[50];
string VN[50], VT[50];           //非终结符，终结符
map<string, int> VN2int, VT2int; //VN、VT映射为下标索引
int symbolNum = 0;

void scanGrama(string grammar)
{
    //扫描一个产生式，识别所有的非终结符和终结符
    for (int j = 0; j < grammar.length(); j++)
    {
        if (grammar[j] >= 'A' && grammar[j] <= 'Z')
        { //非终结符一般大写
            if (grammar[j + 1] == '\'')
            { //带'的非终结符,如 E',T'
                string Vn = grammar.substr(j, 2);
                if (VN2int[Vn] == 0)
                {
                    VN2int[Vn] = ++symbolNum;
                    VN[symbolNum] = Vn; //可通过索引下标输出对应非终结符名字
                }
                j = j + 2 - 1;
            }
            else
            { //不带'的正常非终结符，如E，T
                string Vn = grammar.substr(j, 1);
                if (VN2int[Vn] == 0)
                {
                    VN2int[Vn] = ++symbolNum;
                    VN[symbolNum] = Vn; //可通过索引下标输出对应非终结符名字
                }
                j = j + 1 - 1;
            }
        }
        else if (grammar.substr(j, 2) == "->")
        {
            j = j + 2 - 1;
        }
        else
        { //扫描产生式右部的可能的终结符(关键词表)
            Cppkeyword[1] = "i";
            for (int k = 1; k <= 44; k++)
            {
                string Vt = grammar.substr(j, Cppkeyword[k].length());
                if (Vt == Cppkeyword[k])
                {
                    if (VT2int[Vt] == 0)
                    { //该终结符第一次出现,将该终结符映射为下标索引
                        VT2int[Vt] = ++symbolNum;
                        VT[symbolNum] = Vt; //可通过索引下标输出对应终结符名字
                    }
                    j = j + Cppkeyword[k].length() - 1;
                }
            }
        }
    }
}
string getVn(string grama)
{
    if (grama[1] == '\'')
    { //带'的非终结符,如 E',T'
        return grama.substr(0, 2);
    }
    else
    { //不带'的正常非终结符，如E，T
        return grama.substr(0, 1);
    }
}

string getVt(string grammar)
{
    Cppkeyword[1] = "i";
    for (int k = 1; k <= 44; k++)
    {
        string Vt = grammar.substr(0, Cppkeyword[k].length());
        if (Vt == Cppkeyword[k])
        {
            return Vt;
        }
    }
}
void readVnAndVt()
{

    /*
    for (int i = 0; i < 7; i++)
    {
        int delimiterIndex = grama[i].find("->"); //以"->"为界，分隔产生式

        string leftGrama = grama[i].substr(0, delimiterIndex);                                           //提取左部非终结符
        string rightGrama = grama[i].substr(delimiterIndex + 2, grama[i].length() - delimiterIndex - 2); //提取左部非终结符
        //cout << leftGrama << " " << rightGrama << endl;
        VN[i] = leftGrama;
        if (grama[i].find("null") != string::npos)
        { //若非终结符产生式可空，置nullable[Vn]=true
            nullable[VN[i]] == true;
        }
        //产生式右部第一个字符是否是终结符(关键词)
        int isKeyword = 0;
        for (int j = 5; j <= 43; j++)
        {
            if (rightGrama.substr(0, Cppkeyword[i].length()) == Cppkeyword[i])
            {
                //item.syn = i;
                isKeyword = 1;
            }
        }*/
    grammar[0] = "E->TE'";
    grammar[1] = "E'->+TE'";
    grammar[2] = "E'->null";
    grammar[3] = "T->FT'";
    grammar[4] = "T'->*FT'";
    grammar[5] = "T'->null";
    grammar[6] = "F->(E)";
    grammar[7] = "F->i";
    //扫描一个产生式，识别所有的非终结符和终结符
    for (int i = 0; i < 8; i++)
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
                /*
                if (grama[i][j + 1] == '\'')
                { //带'的非终结符,如 E',T'
                    string Vn = grama[i].substr(j, 2);
                    if (VN2int[Vn] == 0)
                    {
                        VN2int[Vn] = ++VNindex;
                        VN[VNindex] = Vn; //可通过索引下标输出对应非终结符名字
                    }
                    j = j + 2 - 1;
                }
                else
                { //不带'的正常非终结符，如E，T
                    string Vn = grama[i].substr(j, 1);
                    if (VN2int[Vn] == 0)
                    {
                        VN2int[Vn] = ++VNindex;
                        VN[VNindex] = Vn; //可通过索引下标输出对应非终结符名字
                    }
                    j = j + 1 - 1;
                }
                */
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

                /*
                Cppkeyword[1] = "i";
                for (int k = 1; k <= 44; k++)
                {
                    string Vt = grama[i].substr(j, Cppkeyword[k].length());
                    if (Vt == Cppkeyword[k])
                    {
                        if (VT2int[Vt] == 0)
                        { //该终结符第一次出现,将该终结符映射为下标索引
                            VT2int[Vt] = ++VTindex;
                            VT[VTindex] = Vt; //可通过索引下标输出对应终结符名字
                        }
                        j = j + Cppkeyword[k].length() - 1;
                    }
                }
                */
            }
        }
    }

    cout << "非终结符VN" << endl;
    for (auto it = VN2int.begin(); it != VN2int.end(); it++)
    {
        cout << it->first << " " << it->second << endl;
    }
    cout << "终结符VT" << endl;
    for (auto it = VT2int.begin(); it != VT2int.end(); it++)
    {
        cout << it->first << " " << it->second << endl;
    }
}

bool allNullable(vector<string> Y, int left, int right)
{ //判断 Y[left]...Y[right]是否全可空
    int allNullableFlag = 1;
    for (int i = left; i <= right; i++)
    {
        if (nullable[Y[i]] == false)
        {
            allNullableFlag = 0;
        }
    }
    if (allNullableFlag == 1)
        return true;
    else
        return false;
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
    for (int grammarIndex = 0; grammarIndex < 8; grammarIndex++)
    {
        //对于每个产生式：X->Y1Y2...Yk
        string X;
        vector<string> Y;
        int delimiterIndex = grammar[grammarIndex].find("->");
        X = grammar[grammarIndex].substr(0, delimiterIndex);                                                                       //以"->"为界，分隔产生式
        string rightGrama = grammar[grammarIndex].substr(delimiterIndex + 2, grammar[grammarIndex].length() - delimiterIndex - 2); //提取左部产生式
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

        int k = Y.size();
        nullable["null"] = true;
        //如果所有Yi都是可空的，则nullable[X]=true
        int allNullableFlag = 1;
        for (int i = 0; i < k; i++)
        {
            if (nullable[Y[i]] == false)
            {
                allNullableFlag = 0;
                //如果Y0...Y(i-1)都是可空的(言外之意Yi不空),则first[X] = first[X]∪first[Yi] (1)
                if (i <= k - 1)
                {
                    set<string> setX = first[VN2int[X]];
                    //判断Yi是终结符还是非终结符
                    set<string> setY = VT2int.count(Y[i]) != 0 ? first[VT2int[Y[i]]] : first[VN2int[Y[i]]];
                    set_union(setX.begin(), setX.end(), setY.begin(), setY.end(), inserter(setX, setX.begin())); //(1)
                    first[VN2int[X]] = setX;
                }
            }
            //如果Y(i+1)...Yk都是可空的(言外之意Y0..Y(i-1)都不空)，则follow[Yi] = follow[Yi]∪follow[X] (3)
            int rightNullableFlag = 1; //Y(i+1)...Yk全部可空标记
            for (int j = i + 1; j < k; j++)
            {
                if (nullable[Y[j]] == false)
                {
                    rightNullableFlag = 0;
                    //如果Y(i+1)...Y(j-1)都是可空的(言外之意Yj不空),则follow[Yi] = follow[Yi]∪first[Yj] (2)
                    if (j <= k - 1)
                    {
                        set<string> setYi = VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]];
                        set<string> setYj = VT2int.count(Y[j]) ? first[VT2int[Y[j]]] : first[VN2int[Y[j]]];
                        set_union(setYi.begin(), setYi.end(), setYj.begin(), setYj.end(), inserter(setYi, setYi.begin())); //(2)
                        /* if (VT2int[Y[i]] != 0)
                            { //Y[i]是终结符
                                follow[VT2int[Y[i]]] = setYi;
                            }
                            else
                            { //Y[i]是非终结符
                                follow[VN2int[Y[i]]] = setYi;
                            }*/

                        VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]] = setYi;
                    }
                }
            }
            if (rightNullableFlag == 1)
            {
                set<string> setX = follow[VN2int[X]];
                //判断Yi是终结符还是非终结符
                set<string> setY = VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]];
                set_union(setX.begin(), setX.end(), setY.begin(), setY.end(), inserter(setY, setY.begin()));
                VT2int.count(Y[i]) ? follow[VT2int[Y[i]]] : follow[VN2int[Y[i]]] = setY;
            }
        }
        if (allNullableFlag == 1)
        {
            nullable[X] = true;
        }
    }
}

int main()
{
    string text = "E->F|null";
    readVnAndVt();
    for (auto it = nullable.begin(); it != nullable.end(); it++)
    {
        cout << "符号:" << it->first << " 可空:" << it->second << endl;
    }
    for (int times = 1; times < 5; times++)
    {
        cout << "第" << times << "次遍历" << endl;
        getFirstFollowSet();
        //查看非终结符的first集
        for (auto it = VN2int.begin(); it != VN2int.end(); it++)
        {
            int vnindex = it->second;
            cout << it->first << "的first集：";
            for (auto first_it = first[vnindex].begin(); first_it != first[vnindex].end(); first_it++)
            {
                cout << *first_it << " ";
            }
            cout << endl;
        }
    }
    return 0;
}