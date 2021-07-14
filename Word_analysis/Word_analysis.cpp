#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <windows.h>
using namespace std;

struct Table
{
    string sign;
    string nsign;
    int property;
    int i;
    int j;
};
vector<string> keys = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do"
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
};

struct Table table[10000];
int tableNum = 0;
int buffNum = 0;
int keyNum = 0;
int identifierNum = 0;
int numberNum = 0;
int boundNum = 0;
int operatorNum = 0;
int stringNum = 0;
int errorNum = 0;
int solvedNum = 0;
string path = "E:\\Git\\VScode\\EXERCISE\\Compiler\\sample.c";
char buff[10000][1000];
HANDLE hOut;

void analysisBuff();
int isComment(int buffSize, int i, int *pos, int flag);
void readProgram();
int isLetter(char c);
int isDigit(char c);
void print(int prop, int i, int j);
string propMapping(int prop);
void statistics();
void errorHandling(string reason, int i, int j);
void errorFixed();
void rewrite();

int main()
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    readProgram();
    analysisBuff();
    statistics();
    if (errorNum - solvedNum > 0)
        rewrite();
    return 0;
}

void analysisBuff()
{
    int flag = 0;
    int isString = 0;
    int isOct = 0;
    int state = 0;

    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    printf("Analysising...\n");
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    for (int i = 0; i < buffNum; i++)
    {
        if (flag == 1)
            flag = 0;
        state = 0;
        if (buff[i][0] == '#') //对于#开头的语句忽略处理
            continue;
        for (int j = 0; j < int(strlen(buff[i])); j++)
        {
            if (isString == 0)
                flag = isComment(strlen(buff[i]), i, &j, flag);
            if (flag == 0)
            {
                char c = buff[i][j];
                char c_adv = buff[i][j + 1];
                switch (state)
                {
                case 0:
                    if (isLetter(c))
                    {
                        table[tableNum].sign += c;
                        if (isLetter(c_adv) || isDigit(c_adv))
                            state = 1;
                        else
                        {
                            state = 0;
                            print(1, i, j);
                        }
                    }
                    else if (isDigit(c))
                    {
                        table[tableNum].sign += c;
                        if (c == '0')
                        {
                            if (c_adv == 'X' || c_adv == 'x') //处理十六进制数
                            {
                                state = 3;
                                break;
                            }
                            else if (isDigit(c_adv)) //处理八进制
                            {
                                if (c_adv == '8' || c_adv == '9')
                                    isOct = 1;
                                state = 7;
                                break;
                            }
                        }
                        if (isDigit(c_adv)) //处理十进制
                            state = 2;
                        else if (c_adv == '.')
                        {
                            state = 5;
                        }
                        else if (c_adv == 'E' || c_adv == 'e')
                        {
                            state = 8;
                        }
                        else //处理一位数
                        {
                            state = 0;
                            print(3, i, j);
                        }
                    }
                    else if (c == '=' || c == '(' || c == ')' || c == ';' || c == '[' || c == ']' || c == '{' || c == '}' || c == ',' || c == ':' || c == '\'' || c == '\\')
                    {
                        table[tableNum].sign += c;
                        state = 0;
                        if (c == '=' && c_adv == '=')
                        {
                            table[tableNum].sign += c_adv;
                            j++;
                            print(5, i, j);
                        }
                        else
                            print(4, i, j);
                    }
                    else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '^' || c == '!' || c == '%' || c == '|' || c == '>' || c == '<' || c == '?' || c == '.')
                    {
                        if (c == '.' && isDigit(c_adv))
                        {
                            errorHandling("There are no numbers before the point of the count.", i, j);
                            table[tableNum].sign += '0';
                            table[tableNum].sign += c;
                            errorFixed();
                            state = 5;
                        }
                        else if ((c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '^' || c == '!' || c == '%' || c == '|') && c_adv == '=')
                        {
                            table[tableNum].sign += c;
                            table[tableNum].sign += c_adv;
                            j++;
                            state = 0;
                            print(5, i, j);
                        }
                        else if (c == '<')
                        {
                            table[tableNum].sign += c;
                            if (c_adv == '>' || c_adv == '=' || c_adv == '<') //识别 "<>" "<=" "<<"
                            {
                                table[tableNum].sign += c_adv;
                                j++;
                            }
                            if (buff[i][j + 1] == '=' && c_adv == '<') //识别"<<="
                            {
                                table[tableNum].sign += buff[i][j + 1];
                                j++;
                            }
                            state = 0;
                            print(5, i, j); //作为运算符输出
                        }
                        else if (c == '>')
                        {
                            table[tableNum].sign += c;
                            if (c_adv == '=' || c_adv == '>') //识别 ">>" ">="
                            {
                                table[tableNum].sign += c_adv;
                                j++;
                            }
                            if (buff[i][j + 1] == '=' && c_adv == '>') //识别 ">>="
                            {
                                table[tableNum].sign += buff[i][j + 1];
                                j++;
                            }
                            state = 0;
                            print(5, i, j);
                        }
                        else if ((c == '&' && c_adv == '&') || (c == '|' && c == '|')) //识别 "&&" "||"
                        {
                            table[tableNum].sign += c;
                            table[tableNum].sign += c_adv;
                            j++;
                            state = 0;
                            print(5, i, j);
                        }
                        else
                        {
                            table[tableNum].sign += c;
                            state = 0;
                            print(5, i, j);
                        }
                    }
                    else if (c == '"')
                    {
                        table[tableNum].sign += c;
                        isString = 1;
                        state = 4; //处理“”中字符串
                        print(1, i, j);
                    }
                    else if (c == ' ' || c == '\0' || c == '\r' || c == '\n')
                        break;
                    else
                    {
                        table[tableNum].sign += c;
                        errorHandling("An unknown character appears.", i, j); //出现未知字符
                        state = 13;
                    }
                    break;

                case 1: //处理字符串
                    table[tableNum].sign += c;
                    if (!isLetter(c_adv) && !isDigit(c_adv))
                    {
                        state = 0;
                        print(1, i, j);
                    }
                    break;

                case 2: //处理数串
                    table[tableNum].sign += c;
                    if (isDigit(c_adv))
                        break;
                    else if (c_adv == '.')
                    {
                        state = 5;
                    }
                    else if (c_adv == 'E' || c_adv == 'e')
                    {
                        state = 8;
                    }
                    else
                    {
                        state = 0;
                        print(3, i, j);
                    }
                    break;

                case 3: //处理十六进制数符
                    table[tableNum].sign += c;
                    if ((c_adv >= '0' && c_adv <= '9') || (c_adv >= 'a' && c_adv <= 'f') || (c_adv >= 'A' && c_adv <= 'Z'))
                        break;
                    else if ((c_adv >= 'g' && c_adv <= 'z') || (c_adv >= 'G' && c_adv <= 'Z'))
                    {
                        errorHandling("unable to find numeric literal operator 'operator.", i, j); //符号格式错误
                        state = 13;
                    }
                    else if (c_adv == '.')
                    {
                        errorHandling("hexadecimal floating constants require an exponent.", i, j); //符号格式错误
                        state = 13;
                    }
                    else
                    {
                        state = 0;
                        print(3, i, j);
                    }
                    break;

                case 7: //处理八进制
                    table[tableNum].sign += c;
                    if (c_adv >= '0' && c_adv <= '7')
                        break;
                    else if (c_adv == '8' || c_adv == '9')
                        isOct = 1;
                    else if (c_adv == '.')
                    {
                        state = 5;
                        table[tableNum].sign.erase(0, 1);
                        isOct = 0;
                    }
                    else if (c_adv == 'e' || c_adv == 'E')
                    {
                        state = 8;
                        table[tableNum].sign.erase(0, 1);
                        isOct = 0;
                    }
                    else
                    {
                        state = 0;
                        if (isOct == 1)
                        {
                            errorHandling("invalid digit in octal constant.", i, j); //符号格式错误
                            table[tableNum].sign.erase(0, 1);
                            errorFixed();
                            print(3, i, j);
                        }
                        else
                        {
                            print(3, i, j);
                        }
                    }
                    break;

                case 4: //处理“”中的字符串
                    table[tableNum].sign += c;
                    if (c == '\\')
                    {
                        table[tableNum].sign += c_adv;
                        j++;
                    }
                    if (buff[i][j + 1] == '"')
                    {
                        state = 0;
                        isString = 0;
                        print(6, i, j);
                        tableNum++;

                        table[tableNum].sign += c_adv; //把结尾的"也处理
                        print(4, i, j);
                        j++;
                    }
                    break;

                case 5: //处理小数部分
                    table[tableNum].sign += c;
                    if (c == '.' && !isDigit(c_adv))
                    {
                        errorHandling("The number is missing after the number of points.", i, j);
                        table[tableNum].sign += '0';
                        errorFixed();
                    }
                    if (isDigit(c_adv))
                        break;
                    else if (c_adv == 'E' || c_adv == 'e')
                    {
                        state = 8;
                    }
                    else if (c_adv == '.')
                    {
                        errorHandling("too many decimal points in number.", i, j); //符号格式错误
                        state = 13;
                    }
                    else
                    {
                        state = 0;
                        print(3, i, j);
                    }
                    break;

                case 8: //处理指数部分
                    table[tableNum].sign += c;
                    if (c_adv == '+' || c_adv == '-')
                    {
                        state = 9;
                    }
                    else if (isDigit(c_adv))
                    {
                        state = 6;
                    }
                    else
                    {
                        state = 0;
                        errorHandling("exponent has no digits.", i, j);
                        table[tableNum].sign += '0';
                        errorFixed();
                        print(3, i, j);
                    }
                    break;

                case 9: //处理加号
                    if (isDigit(c_adv))
                    {
                        table[tableNum].sign += c;
                        state = 6;
                    }
                    else
                    {
                        state = 0;
                        errorHandling("exponent has no digits.", i, j);
                        table[tableNum].sign += '0';
                        errorFixed();
                        print(3, i, j);
                        j--;
                    }
                    break;

                case 6: //处理指数值
                    table[tableNum].sign += c;
                    if (!isDigit(c_adv))
                    {
                        state = 0;
                        print(3, i, j);
                    }
                    break;

                case 13: //处理错误
                    table[tableNum].sign += c;
                    if (c_adv == ' ' || c_adv == '"' || c_adv == '=' || c_adv == '(' || c_adv == ')' || c_adv == ';' || c_adv == '[' || c_adv == ']' || c_adv == '{' || c_adv == '}' || c_adv == ',' || c_adv == '+' || c_adv == '-' || c_adv == '*' || c_adv == '/' || c_adv == '&' || c_adv == '^' || c_adv == '!' || c_adv == '%' || c_adv == '?' || c_adv == '|' || c_adv == '.' || c_adv == '>' || c_adv == '<' || c_adv == '\0' || c_adv == '\n' || c_adv == '\r')
                    {
                        state = 0;
                        print(-1, i, j);
                    }
                    break;

                default:
                    break;
                }
            }
        }
        //printf("\n");
    }
}

int isComment(int buffSize, int i, int *pos, int flag) //0:无注释  1：//注释   2：/*注释
{
    int j = *pos;
    if (j + 1 < buffSize && buff[i][j] == '/' && buff[i][j + 1] == '/') // "//"注释
    {
        printf("Comment start in : %d : %d\n", i, j);
        return 1;
    }
    if (j + 1 < buffSize && buff[i][j] == '/' && buff[i][j + 1] == '*') // "/*"注释开始
    {
        printf("Comment start in : %d : %d\n", i, j);
        return 2;
    }
    if (j + 1 < buffSize && buff[i][j] == '*' && buff[i][j + 1] == '/') // "*/"注释结束
    {
        printf("Comment end in : %d : %d\n", i, j);
        *pos += 2;
        return 0;
    }
    return flag;
}

void readProgram()
{
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_BLUE);
    cout << "Reading..." << endl;
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    ifstream infile(path, ios::in | ios::binary);
    if (!infile)
    {
        cout << "error" << endl;
        return;
    }
    infile.getline(buff[buffNum], 1000);
    do
    {
        buffNum++;
    } while (infile.getline(buff[buffNum], 1000));
    infile.close();
}

int isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || c == '_' || (c >= 'A' && c <= 'Z');
}

int isDigit(char c)
{
    return c >= '0' && c <= '9';
}

void print(int prop, int i, int j)
{
    printf("%4d \t", tableNum);
    table[tableNum].i = i;
    table[tableNum].j = j;
    table[tableNum].property = prop;
    if (prop == 1)
    {
        vector<string>::iterator result = find(keys.begin(), keys.end(), table[tableNum].sign);
        if (result != keys.end())
            table[tableNum].property = 2;
    }
    string p = propMapping(table[tableNum].property);
    if (prop == -1) //处理错误符号
    {
        SetConsoleTextAttribute(hOut, FOREGROUND_RED);
        cout << "<wrong sign:" << table[tableNum].sign << "    property:" << p << ">" << endl;
    }
    else
    {
        cout << "<sign:" << table[tableNum].sign << "    property:" << p << ">" << endl;
    }
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    tableNum++;
}

string propMapping(int prop)
{
    if (prop == 1)
    {
        identifierNum++;
        return "Identifier";
    }
    if (prop == 2)
    {
        keyNum++;
        return "Keyword";
    }
    if (prop == 3)
    {
        numberNum++;
        return "Number";
    }
    if (prop == 4)
    {
        boundNum++;
        return "Bound";
    }
    if (prop == 5)
    {
        operatorNum++;
        return "Operator";
    }
    if (prop == 6)
    {
        stringNum++;
        return "String";
    }
    return "Unknown.";
}

void statistics()
{
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_BLUE);
    printf("Done\n");
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN);
    for (int i = 0; i < 70; i++)
        printf("*");
    printf("\n");
    printf("There are %d lines in this program.After word analysis,we get %d symbol \n", buffNum, tableNum);
    printf("Including %d Key, %d Identifier, %d Number, %d Bound, %d Operator, %d String.\n", keyNum, identifierNum, numberNum, boundNum, operatorNum, stringNum);
    for (int i = 0; i < 70; i++)
        printf("*");
    printf("\n");
    printf("There are %d errors we find,%d of them have been fixed.\n", errorNum, solvedNum);
    for (int i = 0; i < 70; i++)
        printf("*");
    printf("\n");
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
}

void errorHandling(string reason, int i, int j)
{
    SetConsoleTextAttribute(hOut, FOREGROUND_RED);
    for (int i = 0; i < 70; i++)
        printf("-");
    cout << endl
         << path << ":" << i << ":" << j << endl
         << "errorId:" << errorNum << "\terrorReason:" << reason << endl
         << buff[i] << endl;
    for (int k = 0; k < j; k++)
        cout << " ";
    cout << "?" << endl;
    for (int i = 0; i < 70; i++)
        printf("-");
    printf("\n");
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    errorNum++;
}

void errorFixed()
{
    SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
    printf("The error was fixed!\n");
    SetConsoleTextAttribute(hOut,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN |
                                FOREGROUND_BLUE);
    solvedNum++;
}

void rewrite()
{
    cout << "You can rewrite symbols that have errors." << endl
         << "You can input '@' to pass" << endl;
    for (int i = 0; i <= tableNum; i++)
    {
        if (table[i].property == -1)
        {
            string re;
            SetConsoleTextAttribute(hOut,
                                    FOREGROUND_RED);
            cout << "wrong sign:" << table[i].sign << endl;
            SetConsoleTextAttribute(hOut,
                                    FOREGROUND_RED |
                                        FOREGROUND_GREEN |
                                        FOREGROUND_BLUE);
            cout << "Please input the sign you really want:";
            //cin >> table[i].nsign;
            cin >> re;
            if (re[0] != '@')
            {
                table[i].nsign = re;
                SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
                cout << "new sign:" << table[i].nsign << endl;
            }
            else
            {
                SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
                cout << "You choose not to rewrite the sign." << endl;
            }
        }
    }
    SetConsoleTextAttribute(hOut, FOREGROUND_BLUE);
    cout << "rewrite done!" << endl;
}