#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include "./parse.cpp"
using namespace std;

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
{
    set<string> oldFirst[50];
    set<string> oldFollow[50];
    int isConverge = 1;
    string _vn = getVn(grammar[0].substr(0, 2));
    follow[VN2int[_vn]].insert("$");
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