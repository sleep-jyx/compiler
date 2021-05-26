#include "../Util/parse.cpp"

int main()
{

    string code;
    ifstream myfile("ex1Data.txt");
    if (!myfile.is_open())
    {
        cout << "未成功打开文件" << endl;
    }

    while (getline(myfile, code)) //按行读取文件，可读取空格
    {
        //cout << code << endl;
        scanner(code);
    }

    return 0;
}

/*
    string aa = "hello";
    if (aa.find('s') != string::npos)
        cout << aa.find('11');
    else
        cout << "没找到" << endl;
*/

/*
从指定位数搜索字符，返回值是所在位置下标,从字符串首开始
    string aa = "\"hello\"<<endl";
    for (int i = 0; i < aa.length(); i++)
        cout << aa[i] << endl;
    cout << aa.find_first_of('"', 0) << endl;
*/