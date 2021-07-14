#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <iomanip>
#include <unordered_map>
using namespace std;

int main()
{
    string str="E->E+T";
    string a;
    a.assign(str.begin()+4,str.end());
    cout<<a.size()<<endl;
    return 0;
}