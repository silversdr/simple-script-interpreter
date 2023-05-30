# simple-script-interpreter
Qt5 C++实现的简单脚本语言的解释器

## 项目简介
提供一个语法简单的矩阵计算相关脚本语言，该脚本语言支持包括按条件判断IF语句，按条件循环WHILE和FOR，中缀表达式求值和对变量的赋值，其内置数据类型包括整型int，实型double，字符串类string，和矩阵类型matrix。除字符串类型只支持字符串加法外，矩阵类型支持常见的矩阵运算，其余数据类型支持标准四则运算，一部分条件运算和逻辑表达式的计算。该脚本语言提供了一个输出语句PRINT用于输出变量的值。该脚本语言可以通过中括号运算符访问矩阵类型变量的元素值，同时支持使用点运算符访问矩阵变量的行值和列值。该脚本语言支持用户自定义函数，并以函数文件的方式载入环境，在不需要使用时也支持从环境中卸载。函数包括传值函数和传引用函数，分别依赖于函数存在的作用域的类型。系统的可视化界面提供包括新建脚本，运行脚本，新建函数文件，卸载和载入函数文件，展示运行结果，显示错误异常，保存脚本等功能。

## 脚本语法
> ‘ ‘中的符号表示直接输入的符号， [] 中的符号表示可选项
### 常量
可以在脚本中直接使用常量
- 支持类型
  - 整型如 12 -12 0
  - 实数如 12.3 0.2 -0.35
  - 字符串如 ""  "str"
### 定义变量
```
NAME = VAL
```
- NAME:字母开头，后接数字和大小写字母的任意长度的字符串
- VAL:代表一个非空值（NULL）的值，可以是常量，矩阵初始化语句和任意可以计算得到值的表达式
> 对于矩阵，使用 `matrixName = [a00 a01 a02;a10;a11;]`的形式进行初始化，该语句定义了一个3行3列的矩阵。只能使用常数值对矩阵进行初始化。

### 矩阵值
```
matrixName ‘[‘ exp1 ‘]’ ‘[‘ exp2 ‘]’
```
该语句会返回矩阵的元素值的引用，`exp1 ` 和 `exp2`必须是可以求值的表达式。

对矩阵的赋值操作由下面语句完成
```
matrixName ‘[‘ exp1 ‘]’ ‘[‘ exp2 ‘]’ = exp3
```

```
mat.row
mat.col
```
使用属性值获取行列数


### 选择结构
```
IF exp1
...
[ELSE]
...
ENDIF
```
将IF后的exp1当成合法表达式对其求值，使用其逻辑值判断执行的代码块
ELSE是可选项

### 循环结构
```
FOR exp1 TO exp2 [ STEP int_value ]
...
ENDFOR
```
- Exp1是循环变量的初始化语句 NAME = VARIANT
- Exp2是循环结束判断的表达式,当对该表达式的求值结果的逻辑值为假时跳出循环
- 可选的STEP 是步长，用于对exp1中的循环变量进行自增，当不使用该选项时步长的默认值设置为0

```WHILE exp
...
ENDWHILE
```
- Exp是循环结束判断的表达式,当对该表达式的求值结果的逻辑值为假时跳出循环

### 内置函数
PRINT
```
PRINT exp1,exp2,...,[ENDL]
```
- 将可求值的表达式或换行标记用逗号隔开
- 程序会依次打印表达式的值
- 使用可选的ENDL表示换行

### 自定义函数
解释器支持加载用户自定义函数，需要预先将函数和其作用域定义在单独的文件中。

#### 作用域
每个函数都有它的作用域（分组），定义函数必须先定义其作用域SCOPE
例如：
```
SCOPE name [*]
...
ENDSCOPE
```
定义了一个作用域名为 scopename的作用域,作用域名后面的 * 是可选的标记
- 带有 * 标记的作用域内的所有函数都会成为传引用函数，即在函数返回时将函数管理的内存空间的形参列表的值反赋值给调用函数的内存空间。

#### 函数定义
```
FUNCTION funcName [ formalParameter1 formalParameter2 ...]
...
RETURN (NULL|NAME)
...
ENDFUNC RETURN (NULL|NAME)
```
- 使用FUNCTION关键字定义函数，并使用ENDFUNC RETURN 关键字结束函数
- 函数的返回值支持 NAME 或 NULL
- NAME返回在内存中存储的值，NULL返回不可计算的空值
- 如果没有成功返回会抛出异常
- 可以在函数中途使用RETURN 关键字进行返回
- 通过 `scopename->function(para,...)`使用函数
#### 定义示例
```
SCOPE matrix
//普通作用域
FUNCTION sum matrix1 matrix2
val = matrix1 + matrix2
ENDFUNC RETURN val

ENDSCOPE

SCOPE refer *
//传引用作用域
FUNCTION swap a b
c=a
a=b
b=c
ENDFUNC RETURN NULL
ENDSCOPE
```
以上代码定义了普通作用域 matrix下的sum函数 和 传引用作用域refer下的 swap函数。

## TODO
- GUI存在问题，需要先载入函数文件再载入脚本文件才能使用函数文件中的函数
