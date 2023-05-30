#ifndef FUCTION_H
#define FUCTION_H
#include<iostream>
#include<fstream>
#include"Constant.h"
#include"Interpreter.h"
#include"DLexer.h"


/*
负责函数相关操作的类
包括函数的形参列表初始化，形参列表匹配，在作用域内查找函数，错误处理和返回值
函数以文本文件形式存在
通过interpreter进行解析

*/

/*

作用域注册语法
SCOPE scopeName
...
ENDSCOPE scopeName
作用域不可嵌套
函数定义语法
FUNCTION fucName (arg1,arg2,...)
...
ENDFUNC RETURN name
//末尾必须return



函数调用有两种形式
一种是单行调用，另一种是在求值表达式中作为操作数调用
因此需要在解释器中添加单行调用的方法和在求值器中添加获取返回值的接口
*/
struct fuction_info
{
	//std::string scopeName;
	std::string filePath;
	std::string fucName;
	int argc;
	int begin;//在文件中的行数
	int end;
	bool reference = false;//是否为传引用函数
};

struct scope_info
{
	std::map<std::string, fuction_info> functions;//函数表(fucName->fucInfo)
	std::string filePath;//文件路径
	std::string scopeName;
};


class Function
{
public:
	/**************************函数求值器初始化接口****************************************/
	void loadFile(std::string filePath);//从文件中读取作用域和函数
	void loadFile(std::vector<std::string > files );
	void unloadFile(std::string  flePath);//卸载函数文件
	void unloadFile(std::vector<std::string> files);
	void inPutFuncCallVec(std::vector<func_call_token>);//从解释器中读取函数调用标记
	void setParentMemory(std::map<std::string, VARIANT>* parentMemory);//设置内存方便函数访问
	void setMatrixVisitVec(std::vector<matrix_visit_token> *vec);//函数求值器会自身调用求值器，故需要设置矩阵访问标记
	/**************************************************************************************/
	VARIANT execFunction(D_token);//用函数标记执行函数
	std::vector<func_call_token> getFunVec();
	std::vector<std::string> getFiles();
	

private:
	void scanFile(std::string filePath);//扫描文件，注册函数和作用域;
	/*函数调用相关*/
	void regScope(std::string filePath);//注册作用域
	fuction_info regFuction(std::string filePath, std::string scope, int *begin);//向作用域里注册函数
	//传入实际参数表，函数体，父内存指针，返回可能存在的返回值
	VARIANT calcuFunction(std::vector<std::vector<D_token>> argList, fuction_info function,std::vector < std::vector<D_token>> functionBody,std::map<std::string, VARIANT>* parentMemory );
	
	/**************/

	
	VARIANT returnValue;
	std::map< std::string ,std::vector<fuction_info>> scopes;//作用域集合(作用域名->（函数名->函数信息）)
	std::vector<std::string> referenceSnedScope;
	std::map<std::string, std::vector<std::string>> filesContent;
	std::map<std::string, std::vector<std::vector<D_token>>  >filesDtoken;
	std::map<std::string, std::vector<std::string>> filesScope; //文件路径 ->作用域集合

	std::vector<std::string> files;//扫描的文件地址集合
	std::vector<func_call_token> func_call_vec;//函数调用标记
	std::map<std::string, VARIANT> * parentMemory;//指向调用函数的作用域的内存
	std::vector<matrix_visit_token> *matrix_visit_vec;//矩阵访问的标记，矩阵访问使用求值器进行求值

};




#define _P(s) std::cout << s << std::endl;

#endif

