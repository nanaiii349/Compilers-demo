# <center>语法分析器(LL(1))实践报告</center>

## <center>2018211307班 罗天佑 2018211349</center>

## 一．系统功能设计

本语法分析器采取自顶向下的分析方式，对用户指定的文法进行分析，可以自动实现消除直接左递归、消除间接左递归、提取左公因子、自动获取非终结符的FIRST集、FOLLOW集、构建LL(1)预测分析表，构造LL(1)预测分析程序。并且提供了一定的错误检测机制。

## 二．模块划分

1. 读取文法模块,此模块提供文件读入与手工输入两种方式，丰富了用户使用选择。

```c++
void readGrammar(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar, string defaultPath);

```

2. 规范文法模块，对读入的文法消除直接左递归、消除间接左递归、提取左公因子使其符合 LL(1)文法的要求。

```c++
void normativeLaw(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);

```

3. 计算 FIRST 集模块，自动生成非终结符的 FIRST

```c++
void getFirst(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);

```

4. 计算 FOLLOW 集模块，自动生成非终结符的 FOLLOW

```c++
void getFollow(unordered_map<char, nonTerminator> &NonTerminator, unordered_map<char, vector<string>> &Grammar);

```

5. 根据已知的 FIRST 集与 FOLLOW 集，自动构建 LL(1)预测分析表

```c++
void makeTable(unordered_map<char, nonTerminator> NonTerminator, unordered_map<char, vector<string>> Grammar, vector<vector<string>> &analysisTable);

```

6. 根据 LL(1)预测分析表判断用户输入的符号串是否符合该文法

```c++
bool analyzer(vector<vector<string>> analysisTable, string input);

```

## 三．功能函数

1. 在文法中找到终结符

```c++
void findTerminator(unordered_map<char, vector<string>> &Grammar);

```

2. 根据读入的符号与分析栈顶的符号在分析表中查询相应的候选式

```c++
string findCandidate(char row, char line, vector<vector<string>> analysisTable);
```

3. 将候选式填入响应的分析表中,如果冲突返回-1

```c++
int fillTable(char row, char line, string Candidate, vector<vector<string>> &analysisTable);

```

4. 判断当前符号的类别，1 为非终结符、7 为终结符、5 为空串

```c++
int isTerminator(char target);

```

5. 找到该字符在数组中的位置

```c++
int findIndex(char target, vector<char> arr);

```

6. 为新创建的非终结符创建一个不是非终结符也不是终结符的符号作为名字

```c++
char creatNewname(char name);

```

7.判断分析栈栈顶符号与输入串当前字符是否匹配

```c++
int symbolMatches(char a, char b);

```

8. 判断该字符是否在该数组中出现

```c++
bool visedChar(char target, vector<char> arr);

```

9. 根据字符串的长度比较大小（长度降序排序）

```c++
bool cmp(string a, string b);

```

10.在控制台中输出绿色的横线以区分每个部分

```c++
void splitLine();

```

## 四．实验环境

1. Windows 10 家庭中文版 版本 1909（OS 内部版本
   18363.1139）
2. IDE：VS code
3. 编译器：gcc version 5.1.0 (tdm64-1)

## 五．数据结构

1. 为每个非终结符提供一个结构体保存该非终结符是否存有空串并记录其 first、follow

```c++
typedef struct NodeI
{
    char name;
    int isEmpty;
    vector<char> first;
    vector<char> follow;
} nonTerminator;

```

2. 记录文法中出现的非终结符

```c++
vector<char> nonterminator;

```

3. 记录文法中出现的终结符

```c++
vector<char> terminator; //此处’&’代表 num（整数），’^’代表 letter（字母）

```

4. 使用 unordered_map 保存非终结符的信息，使对非终结符的查询的时间效率达到 O(1)级别。

```c=+
unordered_map<char, nonTerminator> NonTerminator

```

5. 使用 unordered_map 保存每个产生式的候选式，使对产生式的查询的时间效率达到 O(1)级别。

```c++
unordered_map<char, vector<string>> Grammar

```

6. 使用 unordered_map 保存每个产生式中候选式的左公因子的信息，便于提取左公因子。使对其的查询的时间效率达到 O(1)级别。

```c++
unordered_map<string, vector<int>> &LMF

```

7. 使用 vector 容器保存 LL(1)预测分析表

```c++
vector<vector<string>> analysisTable

```

## 六．创新点

1. 本语法分析器强调用户的交互感受，因此程序提供手动输入与文件输入两种形式，其中文件输入形式中还提供了默认文件地址，使用户可以快速对文法进行分析，并快速进入预测分析程序
2. 本程序可实现自动消除直接左递归、消除间接左递归、提取左公因子，让用户无忧无虑的输入法。该程序就是文法的老大哥，会亲切的规范它。
3. 本程序提供一定的错误处理。对于不属于 LL(1)文法的文法，在填写预测分析表时如果发现冲突，程序会显示冲突

## 七．流程图

![avatar](./8.png)

## 八．测试用例以及运行结果

### 文法：(存在间接左递归、直接左递归)
1. C->Af|g
2. A->Ba|b
3. B->Cd|e

### 运行结果：(填表时出现冲突，显示冲突)

![avatar](./1.jpg)

### 文法：（存在左公因子）

1. S->aab|aa|aabd|aad|bb|bbcd

### 运行结果：
![avatar](./2.jpg)
![avatar](./3.jpg)

### 文法

1. E->E+T|E-T|T
2. T->T*F|T/F|F
3. F->(E)|&

### 运行结果：

![avatar](./4.jpg)


```
----------------------------------------------------------------------
Please enter your symbol string:(1+(2/3))
ANALYSISING...
----------------------------------------------------------------------
No:1  | analysisStack:$E| input:( 1 + ( 2 / 3 ) ) $ | output:E->TG
----------------------------------------------------------------------
No:2  | analysisStack:$GT| input:( 1 + ( 2 / 3 ) ) $ | output:T->FU
----------------------------------------------------------------------
No:3  | analysisStack:$GUF| input:( 1 + ( 2 / 3 ) ) $ | output:F->(E)
----------------------------------------------------------------------
No:4  | analysisStack:$GU)E(| input:( 1 + ( 2 / 3 ) ) $ | output:((pop)
----------------------------------------------------------------------
No:5  | analysisStack:$GU)E| input:1 + ( 2 / 3 ) ) $ | output:E->TG
----------------------------------------------------------------------
No:6  | analysisStack:$GU)GT| input:1 + ( 2 / 3 ) ) $ | output:T->FU
----------------------------------------------------------------------
No:7  | analysisStack:$GU)GUF| input:1 + ( 2 / 3 ) ) $ | output:F->&
----------------------------------------------------------------------
No:8  | analysisStack:$GU)GU&| input:1 + ( 2 / 3 ) ) $ | output:&(pop)
----------------------------------------------------------------------
No:9  | analysisStack:$GU)GU| input:+ ( 2 / 3 ) ) $ | output:U->@
----------------------------------------------------------------------
No:10  | analysisStack:$GU)G| input:+ ( 2 / 3 ) ) $ | output:G->+TG
----------------------------------------------------------------------
No:11  | analysisStack:$GU)GT+| input:+ ( 2 / 3 ) ) $ | output:+(pop)
----------------------------------------------------------------------
No:12  | analysisStack:$GU)GT| input:( 2 / 3 ) ) $ | output:T->FU
----------------------------------------------------------------------
No:13  | analysisStack:$GU)GUF| input:( 2 / 3 ) ) $ | output:F->(E)
----------------------------------------------------------------------
No:14  | analysisStack:$GU)GU)E(| input:( 2 / 3 ) ) $ | output:((pop)
----------------------------------------------------------------------
No:15  | analysisStack:$GU)GU)E| input:2 / 3 ) ) $ | output:E->TG
----------------------------------------------------------------------
No:16  | analysisStack:$GU)GU)GT| input:2 / 3 ) ) $ | output:T->FU
----------------------------------------------------------------------
No:17  | analysisStack:$GU)GU)GUF| input:2 / 3 ) ) $ | output:F->&
----------------------------------------------------------------------
No:18  | analysisStack:$GU)GU)GU&| input:2 / 3 ) ) $ | output:&(pop)
----------------------------------------------------------------------
No:19  | analysisStack:$GU)GU)GU| input:/ 3 ) ) $ | output:U->/FU
----------------------------------------------------------------------
No:20  | analysisStack:$GU)GU)GUF/| input:/ 3 ) ) $ | output:/(pop)
----------------------------------------------------------------------
No:21  | analysisStack:$GU)GU)GUF| input:3 ) ) $ | output:F->&
----------------------------------------------------------------------
No:22  | analysisStack:$GU)GU)GU&| input:3 ) ) $ | output:&(pop)
----------------------------------------------------------------------
No:23  | analysisStack:$GU)GU)GU| input:) ) $ | output:U->@
----------------------------------------------------------------------
No:24  | analysisStack:$GU)GU)G| input:) ) $ | output:G->@
----------------------------------------------------------------------
No:25  | analysisStack:$GU)GU)| input:) ) $ | output:)(pop)
----------------------------------------------------------------------
No:26  | analysisStack:$GU)GU| input:) $ | output:U->@
----------------------------------------------------------------------
No:27  | analysisStack:$GU)G| input:) $ | output:G->@
----------------------------------------------------------------------
No:28  | analysisStack:$GU)| input:) $ | output:)(pop)
----------------------------------------------------------------------
No:29  | analysisStack:$GU| input:$ | output:U->@
----------------------------------------------------------------------
No:30  | analysisStack:$G| input:$ | output:G->@
----------------------------------------------------------------------
No:31  | analysisStack:$| input:$ | output:ACCEPT
----------------------------------------------------------------------
**The symbol string you entered is right.
Congratulations.**

```

### 文法：(存在间接左递归、直接左递归)
1. A->(A)A|@|B
2. B->b|bAc

### 运行结果：

![avatar](./5.jpg)
![avatar](./6.jpg)

### 文法：
1. E->A|B
2. A->&|^
3. B->(L)
4. L->E,L|E

### 运行结果：

![avatar](./7.jpg)

ANALYSISING...
----------------------------------------------------------------------
No:1  | analysisStack:$E| input:( a , ( b , ( 2 ) ) , ( c ) ) $ | output:E->B
----------------------------------------------------------------------
No:2  | analysisStack:$B| input:( a , ( b , ( 2 ) ) , ( c ) ) $ | output:B->(L)
----------------------------------------------------------------------
No:3  | analysisStack:$)L(| input:( a , ( b , ( 2 ) ) , ( c ) ) $ | output:((pop)
----------------------------------------------------------------------
No:4  | analysisStack:$)L| input:a , ( b , ( 2 ) ) , ( c ) ) $ | output:L->EM
----------------------------------------------------------------------
No:5  | analysisStack:$)ME| input:a , ( b , ( 2 ) ) , ( c ) ) $ | output:E->A
----------------------------------------------------------------------
No:6  | analysisStack:$)MA| input:a , ( b , ( 2 ) ) , ( c ) ) $ | output:A->^
----------------------------------------------------------------------
No:7  | analysisStack:$)M^| input:a , ( b , ( 2 ) ) , ( c ) ) $ | output:^(pop)
----------------------------------------------------------------------
No:8  | analysisStack:$)M| input:, ( b , ( 2 ) ) , ( c ) ) $ | output:M->,L
----------------------------------------------------------------------
No:9  | analysisStack:$)L,| input:, ( b , ( 2 ) ) , ( c ) ) $ | output:,(pop)
----------------------------------------------------------------------
No:10  | analysisStack:$)L| input:( b , ( 2 ) ) , ( c ) ) $ | output:L->EM
----------------------------------------------------------------------
No:11  | analysisStack:$)ME| input:( b , ( 2 ) ) , ( c ) ) $ | output:E->B
----------------------------------------------------------------------
No:12  | analysisStack:$)MB| input:( b , ( 2 ) ) , ( c ) ) $ | output:B->(L)
----------------------------------------------------------------------
No:13  | analysisStack:$)M)L(| input:( b , ( 2 ) ) , ( c ) ) $ | output:((pop)
----------------------------------------------------------------------
No:14  | analysisStack:$)M)L| input:b , ( 2 ) ) , ( c ) ) $ | output:L->EM
----------------------------------------------------------------------
No:15  | analysisStack:$)M)ME| input:b , ( 2 ) ) , ( c ) ) $ | output:E->A
----------------------------------------------------------------------
No:16  | analysisStack:$)M)MA| input:b , ( 2 ) ) , ( c ) ) $ | output:A->^
----------------------------------------------------------------------
No:17  | analysisStack:$)M)M^| input:b , ( 2 ) ) , ( c ) ) $ | output:^(pop)
----------------------------------------------------------------------
No:18  | analysisStack:$)M)M| input:, ( 2 ) ) , ( c ) ) $ | output:M->,L
----------------------------------------------------------------------
No:19  | analysisStack:$)M)L,| input:, ( 2 ) ) , ( c ) ) $ | output:,(pop)
----------------------------------------------------------------------
No:20  | analysisStack:$)M)L| input:( 2 ) ) , ( c ) ) $ | output:L->EM
----------------------------------------------------------------------
No:21  | analysisStack:$)M)ME| input:( 2 ) ) , ( c ) ) $ | output:E->B
----------------------------------------------------------------------
No:22  | analysisStack:$)M)MB| input:( 2 ) ) , ( c ) ) $ | output:B->(L)
----------------------------------------------------------------------
No:23  | analysisStack:$)M)M)L(| input:( 2 ) ) , ( c ) ) $ | output:((pop)
----------------------------------------------------------------------
No:24  | analysisStack:$)M)M)L| input:2 ) ) , ( c ) ) $ | output:L->EM
----------------------------------------------------------------------
No:25  | analysisStack:$)M)M)ME| input:2 ) ) , ( c ) ) $ | output:E->A
----------------------------------------------------------------------
No:26  | analysisStack:$)M)M)MA| input:2 ) ) , ( c ) ) $ | output:A->&
----------------------------------------------------------------------
No:27  | analysisStack:$)M)M)M&| input:2 ) ) , ( c ) ) $ | output:&(pop)
----------------------------------------------------------------------
No:28  | analysisStack:$)M)M)M| input:) ) , ( c ) ) $ | output:M->@
----------------------------------------------------------------------
No:29  | analysisStack:$)M)M)| input:) ) , ( c ) ) $ | output:)(pop)
----------------------------------------------------------------------
No:30  | analysisStack:$)M)M| input:) , ( c ) ) $ | output:M->@
----------------------------------------------------------------------
No:31  | analysisStack:$)M)| input:) , ( c ) ) $ | output:)(pop)
----------------------------------------------------------------------
No:32  | analysisStack:$)M| input:, ( c ) ) $ | output:M->,L
----------------------------------------------------------------------
No:33  | analysisStack:$)L,| input:, ( c ) ) $ | output:,(pop)
----------------------------------------------------------------------
No:34  | analysisStack:$)L| input:( c ) ) $ | output:L->EM
----------------------------------------------------------------------
No:35  | analysisStack:$)ME| input:( c ) ) $ | output:E->B
----------------------------------------------------------------------
No:36  | analysisStack:$)MB| input:( c ) ) $ | output:B->(L)
----------------------------------------------------------------------
No:37  | analysisStack:$)M)L(| input:( c ) ) $ | output:((pop)
----------------------------------------------------------------------
No:38  | analysisStack:$)M)L| input:c ) ) $ | output:L->EM
----------------------------------------------------------------------
No:39  | analysisStack:$)M)ME| input:c ) ) $ | output:E->A
----------------------------------------------------------------------
No:40  | analysisStack:$)M)MA| input:c ) ) $ | output:A->^
----------------------------------------------------------------------
No:41  | analysisStack:$)M)M^| input:c ) ) $ | output:^(pop)
----------------------------------------------------------------------
No:42  | analysisStack:$)M)M| input:) ) $ | output:M->@
----------------------------------------------------------------------
No:43  | analysisStack:$)M)| input:) ) $ | output:)(pop)
----------------------------------------------------------------------
No:44  | analysisStack:$)M| input:) $ | output:M->@
----------------------------------------------------------------------
No:45  | analysisStack:$)| input:) $ | output:)(pop)
----------------------------------------------------------------------
No:46  | analysisStack:$| input:$ | output:ACCEPT
----------------------------------------------------------------------

## 九．遇到的问题及解决办法

&emsp;在本次语法分析程序的设计与实现过程中，我遇到最大的问题在于如何使用尽量少的数据结构来描述文法中的非终结符、终结符、产生式、候选式，以及分别记录他们的关键属性来简化我的分析流程。所以我设计的三个关键数据结构为：`unordered_map<char, nonTerminator> NonTerminator`、`unordered_map<char, vector<string>> Grammar`、`vector<vector<string>> analysisTable`

使用 unordered_map 保存非终结符的信息和每个产生式的候选式，使对非终结符的查询的时间效率达到 O(1)级别。不仅提升了查询效率，而且将非终结符于产生式独立出来的数据结构使得我在计算FIRST集、FOLLOW集中逻辑清晰明了。而预测分析表作为LL(1)文法分析的关键，使用vector容器，可以高效使用它的许多优秀的特性，也给我的预测分析带来了很大的便利。

其次，在寻找FIRST集与FOLLOW集时，由于计算过程中一个非终结符的FIRST集与FOLLOW集往往受另一个非终结符FIRST集与FOLLOW集的影响。因此，如何找到计算FIRST集与FOLLOW集的结束条件变得困难起来。我的解决方案是设置一个flag值记录发生改变的情况，在每次遍历非终结符的时候如果添加新的FIRST或FOLLOW则标记发生改变。如果遍历一次非终结符过程中没有FIRST或FOLLOW改变，则认为不会再出现新的FIRST或FOLLOW，结束计算。虽然会多遍历一遍非终结符集合来分析是否终止，但是换来了逻辑上的简单直观。并且对于实际中文法出现的非终结符数量规模不会特别大，我认为这样的时间牺牲是值得的。


## 十．实践总结与心得

本次实验强化了我对与语法分析以及语法分析器的理解，也让我切身的体验了设计编译原理过程中的困难与解决困难获得的收获。语法分析器作为编译原理的核心分析部分，实际的上机设计与实现对于我更加深入理解 LR(1)文法，自底向上的预测分析过程有极大的帮助。在设计的过程中，我也懂得了提前规划与设计好数据结构对于后续程序功能实现的巨大作用。这也锻炼了我处理大型工程时的经验。感谢老师提供这样的上机实验项目，让我们让张老师的同行的道路又迈进了一步。
