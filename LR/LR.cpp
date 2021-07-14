#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <iomanip>
#include <unordered_map>
using namespace std;

typedef struct NodeI
{
    int kind;    //1：待约项目、2：移进项目、3.规约项目、4：接受项目
    int metaPos; //记录原符号的位置
    string Candidate;
    vector<char> forwardSearch;
} validItemBody;
typedef struct NodeII
{
    unordered_map<char, int> readyTogo;
    vector<validItemBody> VIB;
} validItemSet;
typedef struct NodeIII
{
    int isEmpty;
    vector<char> first;
} nonTerminator;
vector<char> nonterminator;
vector<char> terminator; //此处'&'代表num（整数），'^'代表letter（字母）
HANDLE hOut;
int Conflict = 0;
int CandidPos = 3;

void readGrammar(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar, string defaultPath); //
void readBuff(char buff[], int cnt, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void getFirst(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void constructValidItemFamily(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, vector<validItemSet> &validItemFamily);
validItemSet constructValidItemSet(validItemSet &VIS, char name);
void getClosure(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, validItemSet &VIS);
void makeTable(vector<validItemSet> VIF, vector<vector<string>> &analysisTable);
bool analyzer(vector<vector<string>> analysisTable, string input);
void submissionVerify(vector<vector<string>> analysisTable);

void printAnalysisTable(vector<vector<string>> analysisTable);
void printVIF(vector<validItemSet> validItemFamily);
vector<char> findFirst(string str, unordered_map<char, nonTerminator> NonTerminator);
validItemBody CreateVIB(string str, char name, int isMath, int metaPos);
void findTerminator(unordered_map<char, vector<string>> &Grammar);
string findCandidate(int Nrow, char line, vector<vector<string>> analysisTable);
int fillTable(int row, char line, string Candidate, vector<vector<string>> &analysisTable);
int isTerminator(char target);
int findVIBIndex(string str, vector<validItemBody> VIB);
int findVISIndex(validItemSet targetVIS, vector<validItemSet> VIF);
int findIndex(char target, vector<char> arr);
char creatNewname(char name);
bool vised(char target, vector<char> arr);
void splitLine();

int main()
{
    string defaultPath = "E:\\Git\\VScode\\EXERCISE\\Compiler\\LR\\Grammar.txt";
    unordered_map<char, vector<string>> Grammar;
    unordered_map<char, nonTerminator> NonTerminator;
    vector<vector<string>> analysisTable;
    vector<validItemSet> validItemFamily;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    readGrammar(NonTerminator, Grammar, defaultPath);
    getFirst(NonTerminator, Grammar);
    constructValidItemFamily(NonTerminator, Grammar, validItemFamily);
    makeTable(validItemFamily, analysisTable);
    if (Conflict == 0)
    {
        while (true)
        {
            submissionVerify(analysisTable);
            cout << "Whether to continue(please enter \"y\" or \"n\")";
            char Response;
            cin >> Response;
            if (Response == 'n')
                break;
        }
    }
    splitLine();
    cout << endl
         << "THANK YOU |*_*|" << endl;
    return 0;
}

void readGrammar(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar, string defaultPath)
{
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << "Reading..." << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    cout << "Please enter it the way you like(1:File input  2:Enter it manually) " << endl;

    int choose;
    char firstNonterminstor;
    cin >> choose;
    if (choose == 1)
    {
        cout << "Please enter absolute address of your file(enter '#' to used default address)" << endl;
        string path;
        cin >> path;
        if (path[0] == '#')
        {
            path = defaultPath;
            cout << "defaultPath:" << defaultPath << endl;
        }
        ifstream infile(path, ios::in | ios::binary);
        if (!infile)
        {
            SetConsoleTextAttribute(hOut, FOREGROUND_RED);
            cout << "file error" << endl;
            SetConsoleTextAttribute(hOut,
                                    FOREGROUND_RED |
                                        FOREGROUND_GREEN |
                                        FOREGROUND_BLUE);
            return;
        }
        splitLine();
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "Grammar:" << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
        char buff[1000];
        int cnt = 0;
        infile.getline(buff, 1000);
        do //逐产生式分析
        {
            cout << "No" << cnt << "  Generational: " << buff << endl;
            readBuff(buff, cnt, NonTerminator, Grammar);
            if (cnt == 0)
                firstNonterminstor = buff[0];
            memset(buff, '\0', 1000);
            cnt++;
        } while (infile.getline(buff, 1000));
        // cout << endl;
        infile.close();
    }
    else if (choose == 2)
    {
        char buff[1000];
        int cnt = 0;
        splitLine();
        SetConsoleTextAttribute(hOut, FOREGROUND_RED);
        cout << "Note:you can use '&' to representative integer and use '^' to representative letter." << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
        while (true)
        {
            cout << "Please enter No" << cnt << " Generational(enter '#' to end):";
            cin >> buff;
            if (buff[0] == '#')
                break;
            if (cnt == 0)
                firstNonterminstor = buff[0];
            readBuff(buff, cnt, NonTerminator, Grammar);
            memset(buff, '\0', 1000);
            cnt++;
        }
    }
    if (Grammar.find(firstNonterminstor)->second.size() > 1)
    {
        char NewNonterminatorName = creatNewname('A');
        nonTerminator NewNonterminator;
        NewNonterminator.isEmpty = 0;
        if (!vised(NewNonterminatorName, nonterminator))
            nonterminator.push_back(NewNonterminatorName);
        string tmp;
        vector<string> vtmp;
        tmp.push_back(firstNonterminstor);
        vtmp.push_back(tmp);
        cout << "Broad Grammar: " << NewNonterminatorName << "->" << firstNonterminstor << endl;
        NonTerminator.emplace(NewNonterminatorName, NewNonterminator);
        Grammar.emplace(NewNonterminatorName, vtmp);
    }
    else
    {
        nonterminator.erase(nonterminator.begin());
        nonterminator.push_back(firstNonterminstor);
    }

    findTerminator(Grammar);
}

void readBuff(char buff[], int cnt, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar)
{
    vector<string> Generational;
    char name = buff[0];
    if (!vised(name, nonterminator))
        nonterminator.push_back(name);
    nonTerminator tmp;
    tmp.isEmpty = 0;
    for (int i = CandidPos; i < int(strlen(buff)) && buff[i] != '\r'; i++)
    {
        string Candidate;
        while (buff[i] != '|' && i < int(strlen(buff)) && buff[i] != '\r')
        {
            if (isTerminator(buff[i]) > 0)
            {
                if (isTerminator(buff[i]) == 5)
                    tmp.isEmpty = 1;
                Candidate += buff[i];
            }
            i++;
        }
        Generational.push_back(Candidate);
    }
    auto item = Grammar.find(name);
    if (item == Grammar.end())
    {
        NonTerminator.emplace(name, tmp);
        Grammar.emplace(name, Generational);
    }
    else
    {
        for (int i = 0; i < int(Generational.size()); i++)
            item->second.push_back(Generational[i]);
    }
}

void getFirst(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar)
{
    int change = 0;
    for (auto iter = NonTerminator.begin(); iter != NonTerminator.end(); ++iter) //第一遍便利只考虑非终结符情况
    {
        char name = iter->first;
        auto item = Grammar.find(name);
        // cout << name << ":    fristChar_Terminator: ";
        for (int i = 0; i < int(item->second.size()); i++)
        {
            char fristChar = item->second[i][0];
            if (isTerminator(fristChar) > 1)
            {
                // cout << fristChar << " ";
                if (!vised(fristChar, iter->second.first))
                {
                    iter->second.first.push_back(fristChar);
                    // iter->second.isAdd = 1;
                }
            }
        }
        // cout << endl;
    }
    while (change != -1) //直到不再添加新的frist为止
    {
        change = -1;
        for (auto iter = NonTerminator.begin(); iter != NonTerminator.end(); ++iter) //逐非终结符遍历
        {
            char name = iter->first;
            // cout << name << ":    fristChar_NonTerminator: ";
            auto item = Grammar.find(name);
            for (int i = 0; i < int(item->second.size()); i++) //逐候选式遍历
            {
                int j = 0;
                char curChar = item->second[i][j];
                int flag = 0;
                while (flag == 0 && j < int(item->second[i].size())) //如果非终结符为空串则找到下一个字符
                {
                    flag = 1;
                    if (isTerminator(curChar) == 1)
                    {
                        auto next = NonTerminator.find(curChar); //找到非终结符
                        //next->second：要加入的非终结符的属性
                        for (int k = 0; k < int(next->second.first.size()); k++)
                        {
                            // char newFrist = next->second.first[i];
                            if (!vised(next->second.first[k], iter->second.first))
                            {
                                if (isTerminator(next->second.first[k]) == 5 && (j + 1) != int(item->second[i].size()))
                                {
                                    cout << "There is no end to the candidate so '@' is not added to first set" << endl;
                                }
                                else
                                {
                                    iter->second.first.push_back(next->second.first[k]);
                                    change = 1;
                                }
                            }
                        }
                        if (next->second.isEmpty == 1)
                        {
                            curChar = item->second[i][++j];
                            flag = 0;
                        }
                    }
                    else if (isTerminator(curChar) > 1)
                    {
                        if (!vised(curChar, iter->second.first))
                        {
                            iter->second.first.push_back(curChar);
                            change = 1;
                        }
                    }
                }
            }
            // cout << endl;
        }
    }
    splitLine();
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << "FIRST:" << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    for (auto iter = NonTerminator.begin(); iter != NonTerminator.end(); ++iter)
    {
        cout << iter->first << ": ";
        for (int i = 0; i < int(iter->second.first.size()); i++)
        {
            cout << iter->second.first[i] << "  ";
        }
        cout << endl;
    }
}

void submissionVerify(vector<vector<string>> analysisTable)
{
    splitLine();
    string input;
    cout << "Please enter your symbol string:";
    cin >> input;
    bool haveNum = vised('&', terminator);
    bool haveLetter = vised('^', terminator);
    for (int i = 0; i < int(input.size()); i++) //对输入符号作预处理，检查是否出现非法符号
    {
        if (isTerminator(input[i]) != 7)
        {
            if ((isTerminator(input[i]) == 4 && haveNum) || (isTerminator(input[i]) == 3 && haveLetter))
                continue;
            SetConsoleTextAttribute(hOut, FOREGROUND_RED);
            cout << "An undefined terminator appears." << endl;
            SetConsoleTextAttribute(hOut,
                                    FOREGROUND_RED |
                                        FOREGROUND_GREEN |
                                        FOREGROUND_BLUE);
            return;
        }
    }
    input.push_back('$');
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << endl
         << endl
         << "ANALYSISING..." << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    if (analyzer(analysisTable, input))
    {
        splitLine();
        cout << "The symbol string you entered is right." << endl
             << "Congratulations." << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
    }
    else
    {
        splitLine();
        SetConsoleTextAttribute(hOut, FOREGROUND_RED);
        cout << "There are some problems with the string of symbols you entered." << endl
             << "Please try again" << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
    }
}

bool analyzer(vector<vector<string>> analysisTable, string input)
{
    vector<int> stateStack;
    vector<char> symbolStack;
    stateStack.push_back(0);
    symbolStack.push_back('-');
    int cnt = 0;
    int pos = 0;
    int working = 0;
    while (working == 0)
    {
        cnt++;
        splitLine();
        cout << "No:" << cnt << endl
             << "stateStack:";
        for (int i = 0; i < int(stateStack.size()); i++)
            cout << stateStack[i];
        cout << endl
             << "symbolStack:";
        for (int i = 0; i < int(symbolStack.size()); i++)
            cout << symbolStack[i];
        cout << endl
             << "input:";
        for (int i = pos; i < int(input.size()); i++)
            cout << input[i] << " ";
        cout << endl
             << "output:";
        string Candidate = findCandidate(stateStack.back(), input[pos], analysisTable);
        if (Candidate.back() == '!')
        {
            Candidate.pop_back();
            int next = stoi(Candidate, 0, 10);
            stateStack.push_back(next);
            symbolStack.push_back(input[pos]);
            pos++;
            cout << "Shift " << next;
        }
        else if (Candidate == "ACC")
        {
            SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
            cout << "ACCEPT";
            SetConsoleTextAttribute(hOut,
                                    FOREGROUND_RED |
                                        FOREGROUND_GREEN |
                                        FOREGROUND_BLUE);
            working = 1;
        }
        else if (Candidate != "err")
        {
            string bottom;
            bottom.assign(Candidate.begin() + CandidPos, Candidate.end());
            cout << "Reduce " << Candidate;
            if (bottom.front() != '@')
            {
                while (!bottom.empty() && working == 0)
                {
                    if ((bottom.back() == symbolStack.back()) || (bottom.back() == '&' && isTerminator(symbolStack.back()) == 4) || (bottom.back() == '^' && isTerminator(symbolStack.back()) == 3))
                    {
                        bottom.pop_back();
                        symbolStack.pop_back();
                        stateStack.pop_back();
                    }
                    else
                    {
                        SetConsoleTextAttribute(hOut, FOREGROUND_RED);
                        cout << "Dismatch";
                        SetConsoleTextAttribute(hOut,
                                                FOREGROUND_RED |
                                                    FOREGROUND_GREEN |
                                                    FOREGROUND_BLUE);
                        working = -1;
                    }
                }
            }
            string tmp = findCandidate(stateStack.back(), Candidate[0], analysisTable);
            if (tmp.back() == '!')
            {
                tmp.pop_back();
                int next = stoi(tmp, 0, 10);
                stateStack.push_back(next);
                symbolStack.push_back(Candidate.front());
            }
            else
            {
                SetConsoleTextAttribute(hOut, FOREGROUND_RED);
                cout << "Can not find Candidate";
                SetConsoleTextAttribute(hOut,
                                        FOREGROUND_RED |
                                            FOREGROUND_GREEN |
                                            FOREGROUND_BLUE);

                working = -1;
            }
        }
        else
        {
            SetConsoleTextAttribute(hOut, FOREGROUND_RED);
            cout << "Can not find Candidate";
            SetConsoleTextAttribute(hOut,
                                    FOREGROUND_RED |
                                        FOREGROUND_GREEN |
                                        FOREGROUND_BLUE);

            working = -1;
        }
        cout << endl;
    }
    return working == 1;
}

void constructValidItemFamily(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, vector<validItemSet> &validItemFamily)
{

    //构造第一个有效项目集
    validItemSet VIS;
    string str;
    str.push_back(nonterminator.back());
    str += "->";
    str += Grammar.find(nonterminator.back())->second[0];
    validItemBody newVIB = CreateVIB(str, nonterminator.back(), 1, CandidPos);
    newVIB.forwardSearch.push_back('$');
    VIS.VIB.push_back(newVIB);
    getClosure(NonTerminator, Grammar, VIS);

    vector<validItemSet> validItemStack;
    validItemFamily.push_back(VIS);
    validItemStack.push_back(VIS);

    while (!validItemStack.empty())
    {
        VIS = validItemStack.back();
        validItemStack.pop_back();
        if (VIS.readyTogo.size() > 0)
        {
            vector<int> trip;
            for (auto iter = VIS.readyTogo.begin(); iter != VIS.readyTogo.end(); ++iter)
            {
                char Go = iter->first;
                // cout << "Go:" << Go << endl;
                validItemSet exportVIS = constructValidItemSet(VIS, Go);
                getClosure(NonTerminator, Grammar, exportVIS);
                // printVIS(exportVIS, cnt);
                int VISIndex = findVISIndex(exportVIS, validItemFamily);
                if (VISIndex == -1)
                {
                    trip.push_back(validItemFamily.size());
                    validItemFamily.push_back(exportVIS);
                    validItemStack.push_back(exportVIS);
                }
                else
                    trip.push_back(VISIndex);
            }
            int i = 0;
            int VISIndex = findVISIndex(VIS, validItemFamily);
            for (auto iter = validItemFamily[VISIndex].readyTogo.begin(); iter != validItemFamily[VISIndex].readyTogo.end(); ++iter)
            {
                iter->second = trip[i++];
            }
        }
    }
    printVIF(validItemFamily);
}

void printVIF(vector<validItemSet> validItemFamily)
{
    int cnt = 0;
    splitLine();
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << "validItemFamily:" << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    for (int i = 0; i < int(validItemFamily.size()); i++)
    {
        if (i != 0)
            splitLine();
        cout << "NO " << cnt << endl;
        validItemSet VIS = validItemFamily[i];
        for (int k = 0; k < int(VIS.VIB.size()); k++)
        {
            string str;
            cout << "Candidate:";
            for (int j = 0; j < int(VIS.VIB[k].Candidate.size()); j++)
            {
                if (j == VIS.VIB[k].metaPos)
                    str.push_back('.');
                str.push_back(VIS.VIB[k].Candidate[j]);
            }
            if (VIS.VIB[k].metaPos == int(VIS.VIB[k].Candidate.size()))
                str.push_back('.');
            cout << setw(10) << left << str;
            cout << "forward:  ";
            for (int j = 0; j < int(VIS.VIB[k].forwardSearch.size()); j++)
                cout << VIS.VIB[k].forwardSearch[j] << " | ";
            cout << endl;
        }
        if (VIS.readyTogo.size() > 0)
        {
            cout << "readyTogo:";
            for (auto iter = VIS.readyTogo.begin(); iter != VIS.readyTogo.end(); ++iter)
                cout << "\'" << iter->first << "\' -> " << iter->second << " | ";
            cout << endl;
        }
        cnt++;
    }
}

validItemSet constructValidItemSet(validItemSet &VIS, char name)
{
    validItemSet newVIS;
    for (int i = 0; i < int(VIS.VIB.size()); i++)
    {
        validItemBody VIB = VIS.VIB[i];
        if (VIB.kind <= 2)
        {
            if (VIB.Candidate[VIB.metaPos] == name)
            {
                validItemBody newVIB = CreateVIB(VIB.Candidate, VIB.Candidate[0], 1, VIB.metaPos + 1);
                newVIB.forwardSearch = VIB.forwardSearch;
                newVIS.VIB.push_back(newVIB);
            }
        }
    }
    return newVIS;
}

void getClosure(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, validItemSet &VIS)
{
    int change = 0;
    while (change == 0)
    {
        change = -1;
        vector<validItemBody> VIBtmp = VIS.VIB;
        for (int i = 0; i < int(VIBtmp.size()); i++)
        {
            validItemBody VIB = VIBtmp[i];
            if (VIB.kind <= 2)
            {
                char Starting = VIB.Candidate[VIB.metaPos];
                auto newTrip = VIS.readyTogo.find(Starting);
                if (newTrip == VIS.readyTogo.end())
                    VIS.readyTogo.emplace(Starting, -1);
                if (VIB.kind == 1) //计算闭包只对待约项目有效
                {
                    string str;
                    str.assign(VIB.Candidate.begin() + VIB.metaPos + 1, VIB.Candidate.end());
                    vector<char> ctmp = findFirst(str, NonTerminator);
                    vector<string> Generational = Grammar.find(Starting)->second;
                    for (int i = 0; i < int(Generational.size()); i++)
                    {
                        string stmp;
                        stmp.push_back(Starting);
                        stmp += "->";
                        stmp += Generational[i];
                        int Index = findVIBIndex(stmp, VIS.VIB);
                        if (Index == -1)
                        {
                            string str;
                            str.push_back(Starting);
                            str += "->";
                            str += Generational[i];
                            validItemBody newVIB = CreateVIB(str, Starting, 0, CandidPos);
                            newVIB.forwardSearch = ctmp;
                            if (ctmp.back() == '@')
                            {
                                newVIB.forwardSearch.pop_back();
                                for (int j = 0; j < int(VIB.forwardSearch.size()); j++)
                                {
                                    if (!vised(VIB.forwardSearch[j], newVIB.forwardSearch))
                                        newVIB.forwardSearch.push_back(VIB.forwardSearch[j]);
                                }
                            }
                            VIS.VIB.push_back(newVIB);
                            change = 0;
                        }
                        else
                        {
                            for (int k = 0; k < int(ctmp.size()); k++)
                            {
                                if (!vised(ctmp[k], VIS.VIB[Index].forwardSearch) && ctmp[k] != '@')
                                {
                                    change = 0;
                                    VIS.VIB[Index].forwardSearch.push_back(ctmp[k]);
                                }
                            }
                            if (ctmp.back() == '@')
                            {
                                for (int j = 0; j < int(VIB.forwardSearch.size()); j++)
                                {
                                    if (!vised(VIB.forwardSearch[j], VIS.VIB[Index].forwardSearch))
                                    {
                                        VIS.VIB[Index].forwardSearch.push_back(VIB.forwardSearch[j]);
                                        change = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void makeTable(vector<validItemSet> VIF, vector<vector<string>> &analysisTable)
{
    //初始化分析表
    for (int i = 0; i < int(VIF.size()); i++)
    {
        vector<string> tmp;
        for (int j = 0; j < int(terminator.size()); j++)
        {
            tmp.push_back("err");
        }
        for (int j = 0; j < int(nonterminator.size()); j++)
        {
            tmp.push_back("err");
        }
        analysisTable.push_back(tmp);
    }
    for (int i = 0; i < int(VIF.size()); i++)
    {
        validItemSet VIS = VIF[i];
        for (auto item = VIS.readyTogo.begin(); item != VIS.readyTogo.end(); ++item)
        {
            char cur = item->first;
            string str = to_string(item->second);
            str.push_back('!');
            Conflict = fillTable(i, cur, str, analysisTable);
        }
        for (auto item = VIS.VIB.begin(); item != VIS.VIB.end(); ++item)
        {
            if (item->kind == 3)
            {
                for (int j = 0; j < int(item->forwardSearch.size()); j++)
                {
                    Conflict = fillTable(i, item->forwardSearch[j], item->Candidate, analysisTable);
                }
            }
            if (item->kind == 4)
            {
                Conflict = fillTable(i, '$', "ACC", analysisTable);
            }
        }
    }
    if (Conflict == 0)
        printAnalysisTable(analysisTable);
}

void printAnalysisTable(vector<vector<string>> analysisTable)
{
    splitLine();
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << endl;
    cout << "analysisTable:" << endl
         << "     ";
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    for (int j = 0; j < int(terminator.size()); j++)
        cout << setw(10) << left << terminator[j];
    for (int j = 0; j < int(nonterminator.size()); j++)
        cout << setw(10) << left << nonterminator[j];
    cout << endl;
    for (int i = 0; i < int(analysisTable.size()); i++)
    {
        cout << setw(3) << left << i << ": ";
        for (int j = 0; j < int(analysisTable[i].size()); j++)
        {
            if (analysisTable[i][j] == "err")
            {
                SetConsoleTextAttribute(hOut, FOREGROUND_RED);
                cout << setw(10) << left << analysisTable[i][j];
                SetConsoleTextAttribute(hOut,
                                        FOREGROUND_RED |
                                            FOREGROUND_GREEN |
                                            FOREGROUND_BLUE);
            }
            else
                cout << setw(10) << left << analysisTable[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

validItemBody CreateVIB(string Candidate, char name, int isMath, int metaPos)
{
    validItemBody newVIB;
    newVIB.metaPos = metaPos;
    newVIB.Candidate = Candidate;

    if (metaPos == int(Candidate.size()))
        if (name == nonterminator.back())
            newVIB.kind = 4;
        else
            newVIB.kind = 3;
    else if (isTerminator(Candidate[metaPos]) == 5)
        newVIB.kind = 3;
    else if (isTerminator(Candidate[metaPos]) == 1)
        newVIB.kind = 1;
    else if (isTerminator(Candidate[metaPos]) == 7)
        newVIB.kind = 2;
    return newVIB;
}

vector<char> findFirst(string str, unordered_map<char, nonTerminator> NonTerminator)
{
    vector<char> ans;
    if (str.size() == 0)
        ans.push_back('@');
    else
        for (int i = 0; i < int(str.size()); i++)
        {
            char curChar = str[i];
            if (isTerminator(curChar) == 1)
            {
                nonTerminator tmp = NonTerminator.find(curChar)->second;
                for (int j = 0; j < int(tmp.first.size()); j++)
                {
                    if (tmp.first[j] != '@')
                        ans.push_back(tmp.first[j]);
                }
                if (tmp.isEmpty != 1)
                {
                    break;
                }
                else if (i == int(str.size()) - 1)
                {
                    ans.push_back('@');
                }
            }
            else if (isTerminator(curChar) == 7)
            {
                ans.push_back(curChar);
                break;
            }
            else
            {
                cout << "ERROR WHEN findFirst" << endl
                     << endl;
            }
        }
    return ans;
}

int findVIBIndex(string str, vector<validItemBody> VIB)
{
    int flag = -1;
    for (int i = 0; i < int(VIB.size()) && flag == -1; i++)
    {
        if (str == VIB[i].Candidate && VIB[i].metaPos == 3)
        {
            flag = i;
        }
    }
    return flag;
}

int findVISIndex(validItemSet targetVIS, vector<validItemSet> VIF)
{
    for (int i = 0; i < int(VIF.size()); i++)
    {
        int isMath = 0;
        validItemSet VIS = VIF[i];
        if (targetVIS.VIB.size() == VIS.VIB.size())
        {
            // sort(targetVIS.VIB.begin(), targetVIS.VIB.end(), cmpVIB);
            // sort(VIS.VIB.begin(), VIS.VIB.end(), cmpVIB);
            for (int j = 0; j < int(VIS.VIB.size()) && isMath == 0; j++)
            {
                if (targetVIS.VIB[j].metaPos == VIS.VIB[j].metaPos)
                {
                    int k = 0;
                    while (k < int(targetVIS.VIB[j].Candidate.size()) && isMath == 0)
                    {
                        if (VIS.VIB[j].Candidate[k] != targetVIS.VIB[j].Candidate[k])
                            isMath = -1;
                        k++;
                    }
                    if (isMath == 0)
                    {
                        if (targetVIS.VIB[j].forwardSearch.size() == VIS.VIB[j].forwardSearch.size())
                        {
                            for (int k = 0; k < int(targetVIS.VIB[j].forwardSearch.size()) && isMath == 0; k++)
                            {
                                if (!vised(targetVIS.VIB[j].forwardSearch[k], VIS.VIB[j].forwardSearch))
                                    isMath = -1;
                            }
                        }
                        else
                            isMath = -1;
                    }
                }
                else
                    isMath = -1;
            }
        }
        else
            isMath = -1;
        if (isMath == 0)
            return i;
    }
    return -1;
}

int isTerminator(char target) //一个简单的词法分析器
{
    if (vised(target, nonterminator))
        return 1;
    else if (vised(target, terminator))
        return 7;
    else if (target == '+' || target == '-' || target == '/' || target == '*' || target == '(' || target == ')' || target == ',') //界符
        return 2;
    else if ((target >= 'a' && target <= 'z') || (target >= 'A' && target <= 'Z') || target == '^') //字母
        return 3;
    else if ((target >= '0' && target <= '9') || target == '&') //数字
        return 4;
    else if (target == '@') //'@'表示空串
        return 5;
    else if (target == '$')
        return 6;
    else if (target == '|') //识别'|'区别候选式
        return -1;
    return -2; //其他情况
}

void findTerminator(unordered_map<char, vector<string>> &Grammar)
{
    terminator.push_back('$');
    for (auto Generational = Grammar.begin(); Generational != Grammar.end(); ++Generational) //逐产生式
    {
        int l = Generational->second.size();
        for (int i = 0; i < l; ++i) //逐候选式
        {
            for (int j = 0; j < int(Generational->second[i].size()); ++j)
            {
                if (isTerminator(Generational->second[i][j]) == 1)
                {
                    j++;
                    j--;
                }
                else if (isTerminator(Generational->second[i][j]) < 1)
                {
                    cout << "others:" << Generational->second[i][j] << " ";
                }
                else if (isTerminator(Generational->second[i][j]) != 5)
                {
                    if (!vised(Generational->second[i][j], terminator))
                    {
                        terminator.push_back(Generational->second[i][j]);
                    }
                }
            }
        }
    }
}

int findIndex(char target, vector<char> arr)
{
    vector<char>::iterator iter = find(arr.begin(), arr.end(), target);
    return iter - arr.begin(); //res即是target在vector数组中的下标
}

string findCandidate(int Nrow, char line, vector<vector<string>> analysisTable)
{
    if (isTerminator(line) == 4)
        line = '&';
    if (isTerminator(line) == 3)
        line = '^';
    int Nline;
    if (vised(line, terminator))
        Nline = findIndex(line, terminator);
    else
        Nline = findIndex(line, nonterminator) + terminator.size();
    return analysisTable[Nrow][Nline];
}

int fillTable(int row, char line, string Candidate, vector<vector<string>> &analysisTable)
{
    int flag = 0;
    int Nline;
    if (vised(line, terminator))
        Nline = findIndex(line, terminator);
    else
        Nline = findIndex(line, nonterminator) + terminator.size();
    if (analysisTable[row][Nline] == "err")
        analysisTable[row][Nline] = Candidate;
    else
    {
        flag = -1;
        SetConsoleTextAttribute(hOut, FOREGROUND_RED);
        cout << "nonterminator:" << row << " terminator:" << line << " Multiple defined table items appear!!" << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
    }
    return flag;
}

bool vised(char target, vector<char> arr)
{
    vector<char>::iterator result = find(arr.begin(), arr.end(), target);
    return result != arr.end();
}

bool cmpVIB(validItemBody a, validItemBody b)
{
    if (a.Candidate.size() == b.Candidate.size())
    {
        int i = 0;
        while (a.Candidate[i] == b.Candidate[i])
            i++;
        return (a.Candidate[i] - b.Candidate[i]) > 0;
    }
    return a.Candidate.size() < b.Candidate.size();
}

char creatNewname(char name)
{
    char newNonTerminatorName = name;                                                             //构造新的非终结符
    while (vised(newNonTerminatorName, nonterminator) || vised(newNonTerminatorName, terminator)) //获得新非终结符名字
    {
        newNonTerminatorName++;
    }
    nonterminator.push_back(newNonTerminatorName);
    return newNonTerminatorName;
}

void splitLine()
{
    SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
    for (int i = 0; i < 70; i++)
        printf("-");
    cout << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
}