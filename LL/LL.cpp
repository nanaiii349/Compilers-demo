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
    int isEmpty;
    vector<char> first;
    vector<char> follow;
} nonTerminator;
vector<char> nonterminator;
vector<char> terminator; //此处n代表num（整数），l代表letter（字母）
HANDLE hOut;

void readGrammar(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar, string defaultPath); //
void readBuff(char buff[], int cnt, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void normativeLaw(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void eliminatesDirectRecursion(char name, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void eliminateIndirectLeftRecursion(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void extractLeftMaleFactor(char name, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar, unordered_map<string, vector<int>> &LMF, vector<string> leftmalefactor);
void findLeftMaleFactor(vector<string> Generational, unordered_map<string, vector<int>> &LMF);
void getFirst(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void getFollow(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);
void makeTable(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, vector<vector<string>> &analysisTable);
bool analyzer(vector<vector<string>> analysisTable, string input);
void submissionVerify(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, vector<vector<string>> analysisTable);
void findTerminator(unordered_map<char, vector<string>> &Grammar);
string findCandidate(char row, char line, vector<vector<string>> analysisTable);
int fillTable(char row, char line, string Candidate, vector<vector<string>> &analysisTable);
int isTerminator(char target);
int findIndex(char target, vector<char> arr);
char creatNewname(char name);
int symbolMatches(char a, char b);
bool visedChar(char target, vector<char> arr);
bool visedInt(int target, vector<int> arr);
bool cmp(string a, string b);
void splitLine();

int main()
{
    string defaultPath = "E:\\Git\\VScode\\EXERCISE\\Compiler\\Grammar.txt";
    unordered_map<char, nonTerminator> NonTerminator;
    unordered_map<char, vector<string>> Grammar;
    vector<vector<string>> analysisTable;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    readGrammar(NonTerminator, Grammar, defaultPath);
    normativeLaw(NonTerminator, Grammar);
    getFirst(NonTerminator, Grammar);
    getFollow(NonTerminator, Grammar);
    makeTable(NonTerminator, Grammar, analysisTable);
    while (true)
    {
        submissionVerify(NonTerminator, Grammar, analysisTable);
        cout << "Whether to continue(please enter \"y\" or \"n\")";
        char Response;
        cin >> Response;
        if (Response == 'n')
            break;
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
            cout << "No" << cnt << "  Generational:" << buff << endl;
            readBuff(buff, cnt, NonTerminator, Grammar);
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
            readBuff(buff, cnt, NonTerminator, Grammar);
            memset(buff, '\0', 1000);
            cnt++;
        }
    }
}

void readBuff(char buff[], int cnt, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar)
{
    vector<string> Generational;
    char name = buff[0];
    if (!visedChar(name, nonterminator))
        nonterminator.push_back(name);
    nonTerminator tmp;
    tmp.name = name;
    tmp.isEmpty = 0;
    if (cnt == 0)
        tmp.follow.push_back('$');
    for (int i = 3; i < int(strlen(buff)) && buff[i] != '\r'; i++)
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

void normativeLaw(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar)
{
    int isDirectRecursion = 0;
    int isLeftMaleFactor = 0;
    int flag = 0;
    findTerminator(Grammar);
    splitLine();
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << "Normativing..." << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    // eliminateIndirectLeftRecursion(NonTerminator, Grammar);
    for (auto iter = Grammar.begin(); iter != Grammar.end(); ++iter)
    {
        vector<string> Generational = iter->second;
        char name = iter->first;
        for (int i = 0; i < int(Generational.size()); i++)
        {
            if (name == Generational[i][0])
            {
                eliminatesDirectRecursion(name, NonTerminator, Grammar);
                isDirectRecursion = 1;
                break;
            }
        }
    }
    if (isDirectRecursion == 1)
    {
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "Eliminates direct left recursion:" << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
        for (auto iter = Grammar.begin(); iter != Grammar.end(); ++iter)
        {
            cout << iter->first << "->";
            vector<string> Generational = iter->second;
            cout << Generational[0];
            for (int i = 1; i < int(Generational.size()); i++)
            {
                cout << "|" << Generational[i];
            }
            cout << endl;
        }
    }
    else
    {
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "There is no direct left recursion in this Grammar." << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
    }
    while (flag == 0)
    {
        for (auto iter = Grammar.begin(); iter != Grammar.end(); ++iter)
        {
            flag = -1;
            vector<string> Generational = iter->second;
            vector<string> leftmalefactor;
            char name = iter->first;
            unordered_map<string, vector<int>> LMF;
            findLeftMaleFactor(Generational, LMF);
            for (auto item = LMF.begin(); item != LMF.end(); ++item)
            {
                if (item->second.size() > 1)
                {
                    flag = 0;
                    isLeftMaleFactor = 1;
                    leftmalefactor.push_back(item->first);
                }
            }
            if (flag == 0)
            {
                sort(leftmalefactor.begin(), leftmalefactor.end(), cmp);
                extractLeftMaleFactor(name, NonTerminator, Grammar, LMF, leftmalefactor);
            }
        }
    }
    if (isLeftMaleFactor == 1)
    {
        splitLine();
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "Extract left male factor:" << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
        for (auto iter = Grammar.begin(); iter != Grammar.end(); ++iter)
        {
            cout << iter->first << "->";
            vector<string> Generational = iter->second;
            cout << Generational[0];
            for (int i = 1; i < int(Generational.size()); i++)
            {
                cout << "|" << Generational[i];
            }
            cout << endl;
        }
    }
    else
    {
        splitLine();
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "There is no left male factor in this Grammar." << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
    }
}

void eliminatesDirectRecursion(char name, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar)
{
    auto iter = Grammar.find(name);
    vector<string> Generational = iter->second;
    vector<string> betaSet;
    vector<string> alphaSet;
    for (int i = 0; i < int(Generational.size()); i++) //获得betaSet、alphaSet
    {
        if (name == Generational[i][0])
        {
            string alpha;
            // for (int j = 1; j < int(Generational[i].size()); j++)
            //     alpha.push_back(Generational[i][j]);
            alpha.assign(Generational[i].begin() + 1, Generational[i].end());
            alphaSet.push_back(alpha);
        }
        else
        {
            string beta = Generational[i];
            betaSet.push_back(beta);
        }
    }

    char newNonTerminatorName = creatNewname(name);
    nonTerminator newNonTerminator;
    vector<string> newGrammar;
    newNonTerminator.name = newNonTerminatorName;
    newNonTerminator.isEmpty = 1;
    NonTerminator.find(name)->second.isEmpty = 0;
    iter->second.clear();
    for (int i = 0; i < int(betaSet.size()); i++)
    {
        string tmp;
        if (betaSet[i][0] != '@')
            tmp = betaSet[i];
        tmp.push_back(newNonTerminatorName);
        iter->second.push_back(tmp);
    }
    for (int i = 0; i < int(alphaSet.size()); i++)
    {
        string tmp;
        tmp = alphaSet[i];
        tmp.push_back(newNonTerminatorName);
        newGrammar.push_back(tmp);
    }
    newGrammar.push_back("@");
    Grammar.emplace(newNonTerminatorName, newGrammar);
    NonTerminator.emplace(newNonTerminatorName, newNonTerminator);
}

void eliminateIndirectLeftRecursion(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar)
{
    vector<char> order;
    int isILR = 0;
    for (auto iter = Grammar.begin(); iter != Grammar.end(); ++iter)
    {
        char name = iter->first;
        if (!visedChar(name, order))
            order.push_back(name);
        for (int i = 0; i < int(iter->second.size()); i++)
        {
            name = iter->second[i][0];
            if (isTerminator(name) == 1 && !visedChar(name, order))
            {
                order.push_back(name);
            }
        }
    }
    int change = 0;
    while (change == 0)
    {
        change = -1;
        for (auto iter = Grammar.begin(); iter != Grammar.end(); ++iter)
        {
            char leftName = iter->first;
            int leftIndex = findIndex(leftName, order);
            vector<string> Generational = iter->second;
            iter->second.clear();
            for (int i = 0; i < int(Generational.size()); i++)
            {
                char rightName = Generational[i][0];
                if (isTerminator(rightName) == 1 && findIndex(rightName, order) < leftIndex)
                {
                    change = 0;
                    isILR = 1;
                    vector<string> alphaSet = Grammar.find(rightName)->second;
                    string beta;
                    beta.assign(Generational[i].begin() + 1, Generational[i].end());
                    for (int i = 0; i < int(alphaSet.size()); i++)
                    {
                        string tmp;
                        if (alphaSet[i][0] == '@' && beta.size() > 0)
                            tmp = beta;
                        else
                            tmp = alphaSet[i] + beta;
                        iter->second.push_back(tmp);
                    }
                }
                else
                    iter->second.push_back(Generational[i]);
            }
        }
    }
    splitLine();
    if (isILR == 1)
    {
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "Eliminates direct left recursion:" << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
        for (auto iter = Grammar.begin(); iter != Grammar.end(); ++iter)
        {
            cout << iter->first << "->";
            vector<string> Generational = iter->second;
            cout << Generational[0];
            for (int i = 1; i < int(Generational.size()); i++)
            {
                cout << "|" << Generational[i];
            }
            cout << endl;
        }
    }
    else
    {
        SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
        cout << "There is no direct left recursion in this Grammar." << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
    }
}

void extractLeftMaleFactor(char name, unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar, unordered_map<string, vector<int>> &LMF, vector<string> leftmalefactor)
{
    auto iter = Grammar.find(name);
    vector<string> Generational = iter->second;
    vector<int> used(Generational.size(), 0);
    vector<string> updateGrammar;
    iter->second.clear();
    for (int i = 0; i < int(leftmalefactor.size()); i++)
    {
        vector<int> emplace = LMF.find(leftmalefactor[i])->second;
        string alpha = leftmalefactor[i];
        if (emplace.size() > 1)
        {
            int l = alpha.size();
            int flag = 0;
            char newNonTerminatorName = creatNewname(name);
            nonTerminator newNonTerminator;
            newNonTerminator.name = newNonTerminatorName;
            vector<string> newGrammar;
            for (int j = 0; j < int(emplace.size()); j++)
            {
                if (used[emplace[j]] != -1)
                {
                    flag = 1;
                    used[emplace[j]] = -1;
                    string Candidate = Generational[emplace[j]];
                    string beta;
                    if (l == int(Candidate.size()))
                    {
                        newNonTerminator.isEmpty = 1;
                        beta.push_back('@');
                    }
                    else
                        beta.assign(Candidate.begin() + l, Candidate.end());
                    newGrammar.push_back(beta);
                }
            }
            if (flag == 1)
            {
                NonTerminator.emplace(newNonTerminatorName, newNonTerminator);
                Grammar.emplace(newNonTerminatorName, newGrammar);
                alpha.push_back(newNonTerminatorName);
                iter->second.push_back(alpha);
            }
        }
    }
    for (int i = 0; i < int(Generational.size()); i++)
    {
        if (used[i] != -1)
            iter->second.push_back(Generational[i]);
    }
}

void findLeftMaleFactor(vector<string> Generational, unordered_map<string, vector<int>> &LMF)
{
    int change = 0;
    int j = 0;
    vector<string> leftmalefactor(int(Generational.size()));
    while (change == 0)
    {
        change = -1;
        for (int i = 0; i < int(Generational.size()); i++)
        {
            string Candidate = Generational[i];
            if (j < int(Candidate.size()))
            {
                leftmalefactor[i].push_back(Generational[i][j]);
                auto item = LMF.find(leftmalefactor[i]);
                if (item == LMF.end())
                {
                    vector<int> same;
                    same.push_back(i);
                    LMF.emplace(leftmalefactor[i], same);
                }
                else
                {
                    item->second.push_back(i);
                }
                change = 0;
            }
        }
        j++;
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
                if (!visedChar(fristChar, iter->second.first))
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
                            if (!visedChar(next->second.first[k], iter->second.first))
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
                        if (!visedChar(curChar, iter->second.first))
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
    cout << endl;
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

void getFollow(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar)
{
    int change = 0;
    while (change != -1)
    {
        change = -1;
        for (auto item = Grammar.begin(); item != Grammar.end(); ++item) //逐非终结符遍历
        {
            auto headIter = NonTerminator.find(item->first);
            vector<char> n;
            for (int i = 0; i < int(headIter->second.follow.size()); i++)
            {
                n.push_back(headIter->second.follow[i]);
            }
            for (int i = 0; i < int(item->second.size()); i++) //逐候选式遍历
            {
                int l = item->second[i].size();
                for (int j = 0; j < l; j++) //候选式逐字符遍历
                {
                    char curChar = item->second[i][j];
                    auto iter = NonTerminator.find(curChar);
                    if (isTerminator(curChar) == 1) //在候选式中找到非终结符
                    {
                        int k = j;
                        int flag = 0;
                        char nextChar = item->second[i][++k];
                        while (k < l - 1 && flag == 0)
                        {
                            flag = -1;
                            if (isTerminator(nextChar) == 1)
                            {
                                auto Niter = NonTerminator.find(nextChar);
                                for (int i = 0; i < int(Niter->second.first.size()); i++)
                                {
                                    if (!visedChar(Niter->second.first[i], iter->second.follow) && Niter->second.first[i] != '@')
                                    {
                                        iter->second.follow.push_back(Niter->second.first[i]);
                                        change = 0;
                                    }
                                }
                                if (Niter->second.isEmpty == 1)
                                {
                                    nextChar = item->second[i][k++];
                                    flag = 0;
                                }
                            }
                            else if (isTerminator(nextChar) >= 2)
                            {
                                if (!visedChar(nextChar, iter->second.follow) && nextChar != '@')
                                {
                                    iter->second.follow.push_back(nextChar);
                                    change = 0;
                                }
                            }
                        }
                        if (k == l - 1)
                        {
                            if (isTerminator(nextChar) == 1)
                            {
                                auto Niter = NonTerminator.find(nextChar);
                                for (int i = 0; i < int(Niter->second.first.size()); i++)
                                {
                                    if (Niter->second.first[i] != '@' && !visedChar(Niter->second.first[i], iter->second.follow))
                                    {
                                        iter->second.follow.push_back(Niter->second.first[i]);
                                        change = 0;
                                    }
                                }
                                if (Niter->second.isEmpty == 1)
                                {
                                    for (int i = 0; i < int(n.size()); i++)
                                    {
                                        if (n[i] != '@' && !visedChar(n[i], iter->second.follow))
                                        {
                                            iter->second.follow.push_back(n[i]);
                                            change = 0;
                                        }
                                    }
                                }
                            }
                            else if (isTerminator(nextChar) > 1)
                            {
                                if (!visedChar(nextChar, iter->second.follow) && nextChar != '@')
                                {
                                    iter->second.follow.push_back(nextChar);
                                    change = 0;
                                }
                            }
                        }
                        else if (k == l)
                        {
                            for (int i = 0; i < int(n.size()); i++)
                            {
                                if (n[i] != '@' && !visedChar(n[i], iter->second.follow))
                                {
                                    iter->second.follow.push_back(n[i]);
                                    change = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    splitLine();
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << endl;
    cout << "FOLLOW:" << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    for (auto iter = NonTerminator.begin(); iter != NonTerminator.end(); ++iter)
    {
        cout << iter->first << ": ";
        for (int i = 0; i < int(iter->second.follow.size()); i++)
        {
            cout << iter->second.follow[i] << "  ";
        }
        cout << endl;
    }
}

void makeTable(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, vector<vector<string>> &analysisTable)
{
    // vector<vector<string>> analysisTable;
    int Conflict = 0;
    for (int i = 0; i < int(nonterminator.size()); i++)
    {
        vector<string> tmp;
        for (int j = 0; j < int(terminator.size()); j++)
        {
            tmp.push_back("err");
        }
        analysisTable.push_back(tmp);
    }
    for (auto Generational = Grammar.begin(); Generational != Grammar.end(); ++Generational)
    {
        int l = Generational->second.size();
        // int row = findIndex(Generational->first, nonterminator);
        vector<string> tmp = Generational->second;
        // cout << "Generational of " << Generational->first << "(" << row << "):: ";
        for (int i = 0; i < l; ++i) //逐候选式
        {
            string Candidate = tmp[i];
            // cout << "Candidate:" << Candidate << "|";
            int flag = 0;
            int s = Candidate.size();
            int j = 0;
            while (flag == 0 && j < s - 1)
            {
                flag = -1;
                if (isTerminator(Candidate[j]) == 1)
                {
                    auto item = NonTerminator.find(Candidate[j]);
                    vector<char> first = item->second.first;
                    for (int k = 0; k < int(first.size()); k++)
                    {
                        if (isTerminator(first[k]) != 5)
                            Conflict = fillTable(Generational->first, first[k], Candidate, analysisTable);
                    }
                    if (item->second.isEmpty == 1)
                    {
                        j++;
                        flag = 0;
                    }
                }
                else if (isTerminator(Candidate[j]) < 1)
                {
                    cout << "Error Symbol:" << Candidate[j];
                }
                else if (isTerminator(Candidate[j]) != 5)
                {
                    Conflict = fillTable(Generational->first, Candidate[j], Candidate, analysisTable);
                }
                else if (isTerminator(Candidate[j]) == 5)
                {
                    j++;
                    flag = 0;
                }
            }
            if (j == s - 1) //单独判断最后一位符号
            {
                if (isTerminator(Candidate[j]) == 1)
                {
                    auto item = NonTerminator.find(Candidate[j]);
                    vector<char> first = item->second.first;
                    for (int k = 0; k < int(first.size()); k++)
                    {
                        if (isTerminator(first[k]) != 5)
                            Conflict = fillTable(Generational->first, first[k], Candidate, analysisTable);
                    }
                    if (item->second.isEmpty == 1)
                    {
                        auto iter = NonTerminator.find(Generational->first);
                        for (int k = 0; k < int(iter->second.follow.size()); k++)
                        {
                            Conflict = fillTable(Generational->first, iter->second.follow[k], Candidate, analysisTable);
                        }
                    }
                }
                else if (isTerminator(Candidate[j]) < 1)
                {
                    cout << "Error Symbol:" << Candidate[j];
                }
                else if (isTerminator(Candidate[j]) != 5)
                {
                    Conflict = fillTable(Generational->first, Candidate[j], Candidate, analysisTable);
                }
                else if (isTerminator(Candidate[j]) == 5)
                {
                    auto iter = NonTerminator.find(Generational->first);
                    for (int k = 0; k < int(iter->second.follow.size()); k++)
                    {
                        Conflict = fillTable(Generational->first, iter->second.follow[k], Candidate, analysisTable);
                    }
                }
            }
        }
    }
    if (Conflict == -1)
    {
        SetConsoleTextAttribute(hOut, FOREGROUND_RED);
        cout << "There were conflicts when filling out the form" << endl;
        SetConsoleTextAttribute(hOut,
                                FOREGROUND_RED |
                                    FOREGROUND_GREEN |
                                    FOREGROUND_BLUE);
        return;
    }
    splitLine();
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << endl;
    cout << "analysisTable:" << endl
         << "   ";
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    for (int j = 0; j < int(terminator.size()); j++)
        cout << setw(6) << left << terminator[j];
    cout << endl;
    for (int i = 0; i < int(nonterminator.size()); i++)
    {
        cout << nonterminator[i] << ": ";
        for (int j = 0; j < int(terminator.size()); j++)
        {
            if (analysisTable[i][j] == "err")
            {
                SetConsoleTextAttribute(hOut, FOREGROUND_RED);
                cout << setw(6) << left << analysisTable[i][j];
                SetConsoleTextAttribute(hOut,
                                        FOREGROUND_RED |
                                            FOREGROUND_GREEN |
                                            FOREGROUND_BLUE);
            }
            else
                cout << setw(6) << left << analysisTable[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

bool analyzer(vector<vector<string>> analysisTable, string input)
{
    vector<char> analysisStack;
    analysisStack.push_back('$');
    analysisStack.push_back(nonterminator[0]);
    int cnt = 0;
    int pos = 0;
    int working = 0;
    while (working == 0)
    {
        cnt++;
        splitLine();
        cout << "No:" << cnt << "  | analysisStack:";
        for (int i = 0; i < int(analysisStack.size()); i++)
            cout << analysisStack[i];
        cout << "| input:";
        for (int i = pos; i < int(input.size()); i++)
            cout << input[i] << " ";
        cout << "| output:";
        char curState = analysisStack.back();
        if (isTerminator(curState) == 1)
        {
            string Candidate = findCandidate(curState, input[pos], analysisTable);
            if (Candidate != "err")
            {
                cout << curState << "->" << Candidate;
                analysisStack.pop_back();
                for (int i = int(Candidate.size()) - 1; i >= 0; i--)
                {
                    if (isTerminator(Candidate[i]) != 5)
                        analysisStack.push_back(Candidate[i]);
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
        }
        else if (isTerminator(curState) > 1)
        {
            if (symbolMatches(curState, input[pos]) == 1)
            {
                SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
                cout << "ACCEPT";
                SetConsoleTextAttribute(hOut,
                                        FOREGROUND_RED |
                                            FOREGROUND_GREEN |
                                            FOREGROUND_BLUE);
                working = 1;
            }
            else if (symbolMatches(curState, input[pos]) == 2)
            {
                analysisStack.pop_back();
                cout << curState << "(pop)";
                pos++;
            }
            else if (symbolMatches(curState, input[pos]) == -1)
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
        cout << endl;
    }
    if (working == 1)
        return true;
    return false;
}

void submissionVerify(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, vector<vector<string>> analysisTable)
{
    splitLine();
    string input;
    cout << "Please enter your symbol string:";
    cin >> input;
    bool haveNum = visedChar('&', terminator);
    bool haveLetter = visedChar('^', terminator);
    for (int i = 0; i < int(input.size()); i++)
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
    cout << "ANALYSISING..." << endl;
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

int isTerminator(char target) //一个简单的词法分析器
{
    if (visedChar(target, nonterminator))
        return 1;
    else if (visedChar(target, terminator))
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
                    if (!visedChar(Generational->second[i][j], terminator))
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

string findCandidate(char row, char line, vector<vector<string>> analysisTable)
{
    if (isTerminator(line) == 4)
        line = '&';
    if (isTerminator(line) == 3)
        line = '^';
    int Nrow = findIndex(row, nonterminator);
    int Nline = findIndex(line, terminator);
    return analysisTable[Nrow][Nline];
}

int fillTable(char row, char line, string Candidate, vector<vector<string>> &analysisTable)
{
    int flag = 0;
    int Nrow = findIndex(row, nonterminator);
    int Nline = findIndex(line, terminator);
    if (analysisTable[Nrow][Nline] == "err")
        analysisTable[Nrow][Nline] = Candidate;
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

int symbolMatches(char a, char b)
{
    if (a == '$' && b == '$')
        return 1;
    else if (a == b)
        return 2;
    else if (a != b)
    {
        if (a == '&' && isTerminator(b) == 4)
            return 2;
        if (a == '^' && isTerminator(b) == 3)
            return 2;
    }
    return -1;
}

bool visedChar(char target, vector<char> arr)
{
    vector<char>::iterator result = find(arr.begin(), arr.end(), target);
    return result != arr.end();
}

bool visedInt(int target, vector<int> arr)
{
    vector<int>::iterator result = find(arr.begin(), arr.end(), target);
    return result != arr.end();
}

bool cmp(string a, string b)
{
    return a.size() > b.size();
}

char creatNewname(char name)
{
    char newNonTerminatorName = name;                                                                     //构造新的非终结符
    while (visedChar(newNonTerminatorName, nonterminator) || visedChar(newNonTerminatorName, terminator)) //获得新非终结符名字
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