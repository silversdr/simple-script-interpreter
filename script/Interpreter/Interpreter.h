#ifndef INTERPRETER_H
#define INTERPRETER_H
class Evaluation;
#include<map>
#include<string>
#include<vector>

#include "Constant.h"
#include "Evaluation.h"
#include "Error.h"
//#include "Fuction.h"


/*
该类对输入的多行语句进行分隔处理
并且负责管理内存
*/

/*
语法规则
每行为一条语句
所有的变量都是全局作用域
变量可以直接赋值使用，不需要声明类型
a = 1 
a = 3.5
a = " 1sd54s6"
a = [ 1 2; 5 6; 2]
变量声明不能连续赋值
a = b = 3 (x)
变量可以随时被覆盖
a = 3
a = 3.5

PRINT 打印变量

FOR exp1  TO  exp2  【STEP exp3】
exps
ENDFOR
//exp1为初始化语句 格式 [name] = [exp] exp为可求值的整型
//exp2为可求值的整型
//exp3为循环变量[name]的改变条件,可选
//exps为循环体
//ENDFOR为结束循环的标志

IF exp1
exps1

[ELSE]
exps3
ENDIF

//exp1是可求值的逻辑表达式，并且值的结果是整型和实型中的一种
//exps1,exps3为表达式组
//所有的IF会和其最近的ENDIF配对

WHILE exp1
exps
ENDWHILE




*/



/*
程序按行执行语句
将每条语句判断其类型后分别交给相应的函数进行执行
全局维护计数器pc表示当前执行的语句的行数
函数从内存中读取pc的值并取出相应的指令token
*/


class Interpreter//输入D_token流后会自动执行,需要输入函数存在的文件
{
	/*
	一个Interpreter和与其绑定的求值器共同管理一片内存空间
	*/
public:
	/**************************解释器对外接口*************************************************/
	void inputScript(std::vector<std::vector<D_token>> script);//输入解析成d_token流的脚本文件
	//对求值器内的函数求值器进行作用域输入
	void loadFunctionFile(std::string filePath);
	void loadFunctionFile(std::vector<std::string> filesPath);
	void unloadFunctionFile(std::string filePath);
	void unloadFunctionFile(std::vector<std::string> filesPath);
	/*****************************************************************************************/
	Interpreter();


private:
	void execScript(std::vector<std::vector<D_token >> scripts);//运行脚本
	void match(std::vector<std::vector<D_token >> scripts);//匹配脚本中需要匹配的结构，如for和endfor,利用栈实现
	void execCode(std::vector<D_token > line);//判断语句的类型
	void execFOR(std::vector<D_token > line);
 	void execENDFOR(std::vector<D_token > line);
	void execWHILE(std::vector<D_token > line);
    void execENDWHILE(std::vector<D_token > line);
	void execIF(std::vector<D_token > line);
    void execELSE(std::vector<D_token > line);
    void execENDIF(std::vector<D_token > line);
	void execASIGN(std::vector<D_token > line);//赋值语句
	void execPRINT(std::vector<D_token > line);

	/*********************************函数相关处理******************************************/
	void execFUNCTION(std::vector<D_token > line);
	void execRETURN(std::vector<D_token > line);//函数返回语句
	void execENDFUNC(std::vector<D_token > line);
	std::vector<D_token> boundFuctionCall(std::vector<D_token > ,int i);//将函数调用的token流替换为函数调用标记，并存储在vec中，
	//求值时从函数调用标记中取出ID在vec中取出token流后使用求值器求值

	/***************************************************************************************/

	std::vector<D_token> boundProperty(std::vector<D_token >);//将变量和属性进行绑定属性 a.b  NAME.Property作为右值使用


	std::vector<D_token> boundMatrixVisit(std::vector<D_token >, int i);//将矩阵值的访问替换为矩阵访问标记，并存储在vec中，再使用的时候顺序求值
	//注意要区分两种情况，一种是作为值来用，一种是作为目标地址进行赋值
	
	void execNULL();


	
public:
	
	std::vector<std::vector<D_token>> script;//保存的脚本
	std::map<std::string, VARIANT> memory;//管理的内存
	Evaluation *evals;//求值器,在初始化时需要使用setMemory方法来关联求值器和内存,包括函数求值都使用求值器进行
	VARIANT returnValue;//解析函数体求值后的返回值



private:
	std::map<int ,int> for_map;//用于记录脚本匹配的for和endfor;
	std::stack<for_stack_token> for_stack;//用于记录for循环执行信息的栈
	std::map<int, int> while_map;
	std::stack<while_stack_token> while_stack;

	std::vector<if_match> if_vec;
	std::map<int,if_match> if_map;
	//bool else_sign;//判断程序在if处是否选择执行else,并在else处进行复位


	std::vector<func_call_token> func_call_vec;//用于替换token流中的函数调用，然后依次求值
	std::vector<matrix_visit_token> martrix_visit_vec;//用于替换token流中的矩阵访问，然后依次求值
	
	int pc;//程序计数器
	bool pcIncreaseSign = true;//pc自增判断，当执行跳转后需要停止自增,当执行语句后需要停止一次pc的自增时，将该标志设置为false即可，该标记会自动复位

	
};


std::vector<std::vector<double>> matrixInitial(std::vector<D_token > line);//与矩阵类的初始化的构造函数的接口，可能会抛出异常



#endif




/*
全局维护计数器
判断
存储for的变量，表达式和步长
判断是否符合
	符合 找到最近的endfor

	不符合  计数器自增




A=10
IF A < 100
	FOR B = 1 TO B<=A STEP 1
		FOR C = 1 TO  C <= A - B STEP 1
			PRINT " "
		ENDFOR
		FOR D = 1 TO  D<=2*B-1 STEP 1
			PRINT "*"
		ENDFOR
		PRINT ENDL
	ENDFOR
ELSE
	PRINT "NO\n"
ENDIF



*/