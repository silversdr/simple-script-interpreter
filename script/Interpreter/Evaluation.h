#ifndef EVALUATION_H
#define EVALUATION_H
#include <stack>
#include <vector>
#include <string>
#include <map>
#include<sstream>
#include "Constant.h"
#include "Fuction.h"
/*
将传入的DToken流解析为中缀表达式并尝试对表达式求值
通过将中缀表达式转换成后缀表达式进行求值

类型对外接口为 void inputDataStream(std::vector<D_token>)
输入待求值的中缀表达式D_Token流，类会尝试对其求值，可以通过访问类成员
获取转换得到的token流和后缀表达式以及计算结果

可能会抛出std::string类型的异常
*/



class Function;


class Evaluation
{
	//输入dtoken流，转换成token流
	//先将token流转换成后缀表达式再求值
public:
	Evaluation();
	std::vector<token> postfixExpression;
	std::vector<token> calcuTokenStream;
	
	VARIANT result;

private:

	std::vector<token> DToken2token(std::vector<D_token>);//将Lexer分离的dtoken流转换成易于求值操作的token流
	std::vector<token> nifix2postfix(std::vector<token>);//中缀表达式转换后缀表达式，便于下一步的求值
	//需要处理可能抛出的string类型的异常
	VARIANT evals(std::vector<token> postfixExpression);//对后缀表达式进行求值
	//需要处理可能抛出的string类型的异常
	bool isNotBracket(OPERATOR);//判断op是不是括号，为了单独处理括号的情况
	std::vector<token> boundNegative(std::vector<token>);//对变量进行负号的绑定
	VARIANT calculate(VARIANT var1, OPERATOR op, VARIANT var2);//对双目运算符进行求值，目前支持对四则运算的求值
	VARIANT calculate(VARIANT var1, OPERATOR op);//对单目运算符求值
	//新增的运算符号在caiculat中实现
	
	


	std::map<std::string, VARIANT>     * memory;//可以获取的内存区域
	std::vector<matrix_visit_token> *matrix_visit_vec;//矩阵访问的标记，矩阵访问使用求值器进行求值
	


public:
	void printVARIANT(VARIANT var);//打印变量的值

	/******************求值器初始化接口*************************/
	void setMemory(std::map<std::string, VARIANT> *memory);//设置自身内存和函数求值器的父内存
	void setMatrixVisitVec(std::vector<matrix_visit_token> *);//为自身和函数求值器设置矩阵访问标记
	void inputDataStream(std::vector<D_token>);//对外部的接口，输入待求值的中缀表达式，尽量不要在自身调用此函数，会覆盖结果的值
	void inPutFuncCallVec(std::vector<func_call_token>);//为函数求值器设置函数访问标记
	/************************矩阵求值相关**********************************/
	VARIANT matrixVisit(matrix_visit_token* matrixVisitInfo, Evaluation* eval, std::map<std::string, VARIANT>*);//访问可能合法的矩阵值，处理错误
	//绑定矩阵内存空间，返回结果
	/**************************属性求值相关********************************/
	VARIANT propertyEval(D_token propertySign);

	/**********************************************************/
	bool getLogicResult();//结果的逻辑值，结果无法作为逻辑值则报错
	
	Function* functionEval;//函数求值器
};

#endif // !EVALUATION_H