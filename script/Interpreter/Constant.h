#ifndef CONSTANT_H
#define CONSTANT_H
#include <vector>
#include <string>
#include <stack>
#include<fstream>
#include "Matrix.h"
class Evaluation;

/////////////////////////////////////////////////////////DLEXER_H
struct D_token
{
	int type = 0;			//类型
	int abstractType = 0;	//运算符或者操作数，用于表达式求值
	int errorType = 0;		//错误类型
	int INT_val;			//值
	double DOUBLE_val;
	std::string STR_val;

	int func_call_id = -1;//默认不为函数调用标记，函数调用信息记录在interpreter的vec中
	int matrix_visit_id = -1;//默认不为矩阵访问标记
	int reserved_propertyType = -1;//默认不是某个成员的属性 NO_PROPERTY
	int bound_propertyType = -1;//绑定属性默认不存在
	void clear();


};
//作为变量的保留属性使用  使用语法 
static std::vector<std::string> reserved_property =
{
	"row",// matrix.row 返回矩阵行值
	"col",// matrix.col 返回矩阵列值
};//值应该与下方的枚举量的顺序一致

enum propertyType
{
	NO_PROPERTY = -1,
	MATRIX_ROW,
	MATRIX_COL,
};


static std::vector<std::string> bound_property;//绑定的属性和只有一个参数的函数绑定




enum type
{
	D_EXPRESSION_TOKEN_UNKNOWN = 0,

	/*保留关键字*/
	D_EXPERSSION_TOKEN_IF,
	D_EXPRESSION_TOKEN_ENDIF,
	D_EXPRESSION_TOKEN_ELSE,
	D_EXPRESSION_TOKEN_WHILE,
	D_EXPERSSION_TOKEN_ENDWHILE,
	D_EXPRESSION_TOKEN_FOR,
	D_EXPRESSION_TOKEN_TO,
	D_EXPRESSION_TOKEN_STEP,
	D_EXPRESSION_TOKEN_ENDFOR,
	D_EXPRESSION_TOKEN_ASIGN,
	D_EXPRESSION_TOKEN_PRINT,
	D_EXPRESSION_TOKEN_LOAD,
	D_EXPRESSION_TOKEN_SAVE,
	D_EXPRESSION_TOKEN_ENDL,

	D_EXPRESSION_TOKEN_FUNCTION,
	D_EXPRESSION_TOKEN_ENDFUNC,
	D_EXPRESSION_TOKEN_RETURN,

	D_EXPRESSION_TOKEN_SCOPE,//作用域
	D_EXPRESSION_TOKEN_ENDSCOPE,//结束作用域

	D_EXPRESSION_TOKEN_NULL,//空返回值



	/*保留关键字结束*/

	
	D_EXPRESSION_TOKEN_NAME,	//标识符
	D_EXPRESSION_TOKEN_FUNCCALL,//函数调用标记
	D_EXPRESSION_TOKEN_MATRIX_VISIT,//矩阵访问标记 作为实型变量使用


	/*常量类型*/
	D_EXPERSSION_TOKEN_INT,			//整型
	D_EXPRESSION_TOKEN_STRING,		//字符串
	D_EXPRESSION_TOKEN_DOUBLE,		//实型
	D_EXPRESSION_TOKEN_MATRIX,		//矩阵类型 可用NAME[exp1][exp2]对矩阵中的某个值进行访问
	D_EXPRESSION_TOKEN_PROERTY,     //属性标记，可以作为右值参与计算
	D_EXPRESSION_TOKEN_NULLVARIANT,	//空值 仅作为标记，不能参与运算


	/*运算符*/
	D_EXPRESSION_TOKEN_PLUS,//+
	D_EXPRESSION_TOKEN_MINUS,//-
	D_EXPERSSION_TOKEN_MULTI,///*
	D_EXPRESSION_TOKEN_DIV,// /
	D_EXPRESSION_TOKEN_POW,/// **
	D_EXPRESSION_TOKEN_EQUALFOR,// =
	D_EXPRESSION_TOKEN_MOD,//%
	D_EXPRESSION_TOKEN_EQUAL,// ==

	D_EXPRESSION_TOKEN_NE,//!=
	D_EXPRESSION_TOKEN_GT,//>
	D_EXPRESSION_TOKEN_GE,//>=
	D_EXPRESSION_TOKEN_LT,//<
	D_EXPRESSION_TOKEN_LE,//<=

	D_EXPERSSION_TOKEN_POINT_PLUS,/// .+
	D_EXPRESSION_TOKEN_POINT_MINUS,/// .-
	D_EXPRESSION_TOKEN_OR,/// |
	D_EXPERSSION_TOKEN_AND,/// &
	D_EXPRESSION_TOKEN_NOT,// !

	D_EXPRESSION_TOKEN_ARROW,//->

	/*界符*/
	D_EXPRESSION_TOKEN_LEFT_PARENTHESIS,// (
	D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS,// )
	D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS,// [
	D_EXPERSSION_TOKEN_RIGHT_SQUARE_BRACKETS,// ]
	D_EXPRESSION_TOKEN_COMMA,// ,
	D_EXPERSSION_TOKEN_SEMICOLON,// ;
	D_EXPRESSION_TOKEN_DOT,//.


	D_TOKEN_MIN//栈底



};

enum abstractType
{
	Var_default = 0,
	Var_operand,
	Var_operator,


};

enum errorType
{
	D_ERRORTYPE_TOKEN_DEFAULT,
	D_ERRORTYPE_STRING_MISSING,//字符串缺失
	D_ERRORTYPE_ILLEAGEL_NUMBER,//非法数字
	D_ERRORTYPE_OP_MISSING,//操作数缺失
};

/////////////////////////////////////EVALUATION_H
struct VARIANT//值，表达式计算时使用variant传递值，代表一个值的常量
{

	enum var_error
	{
		error = -1
	};

	//统一使用dtoken内的类型名称
	int type = 0;
	int int_val;
	double double_val;
	std::string string_val;
	Matrix matrix_val;//矩阵值;

	bool null = false;//处理函数返回空值的情况


};
struct OPERATOR
{
	int numbers;        // 操作数  
	int icp;            // 优先级  
	int isp;            // 优先级  
	int ass;  // 结合性  
	int type; // 操作符
};
 enum  operator_type {
	/* 算数运算 */
	oper_lparen = D_EXPRESSION_TOKEN_LEFT_PARENTHESIS,    // 左括号  
	oper_rparen = D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS,        // 右括号  
	oper_plus = D_EXPRESSION_TOKEN_PLUS,          // 加  
	oper_minus = D_EXPRESSION_TOKEN_MINUS,         // 减  
	oper_multiply = D_EXPERSSION_TOKEN_MULTI,      // 乘  
	oper_divide = D_EXPRESSION_TOKEN_DIV,        // 除  
	oper_mod = D_EXPRESSION_TOKEN_MOD,           // 取余

	/* 关系运算 */
	oper_lt = D_EXPRESSION_TOKEN_LT,            // 小于  
	oper_gt = D_EXPRESSION_TOKEN_GT,            // 大于  
	oper_eq = D_EXPRESSION_TOKEN_EQUAL,            // 等于  
	oper_ne = D_EXPRESSION_TOKEN_NE,            // 不等于  
	oper_le = D_EXPRESSION_TOKEN_LE,            // 不大于  
	oper_ge = D_EXPRESSION_TOKEN_GE,            // 不小于  
	/* 逻辑运算 */
	oper_and = D_EXPERSSION_TOKEN_AND,           // 且  
	oper_or = D_EXPRESSION_TOKEN_OR,            // 或  
	oper_not = D_EXPRESSION_TOKEN_NOT,           // 非  

	oper_min = D_TOKEN_MIN            // 栈底  
};

//结合性枚举类
enum  associativity {
	left2right,
	right2left
};

static const OPERATOR operators[] = {
	/* 算数运算 */
//  {操作数，入栈前优先级，入栈后优先级，结合性，符号代码}
	{2, 17, 1, left2right, oper_lparen},     // 左括号  
	{2, 17, 17, left2right, oper_rparen},    // 右括号  
	{2, 12, 12, left2right, oper_plus},      // 加  
	{2, 12, 12, left2right, oper_minus},     // 减  
	{2, 13, 13, left2right, oper_multiply},  // 乘  
	{2, 13, 13, left2right, oper_divide},    // 除  
	{2, 13, 13, left2right, oper_mod},       // 取余
	/* 关系运算 */
	{2, 10, 10, left2right, oper_lt},        // 小于  
	{2, 10, 10, left2right, oper_gt},        // 大于  
	{2, 9, 9, left2right, oper_eq},          // 等于  
	{2, 9, 9, left2right, oper_ne},          // 不等于  
	{2, 10, 10, left2right, oper_le},        // 不大于  
	{2, 10, 10, left2right, oper_ge},        // 不小于  
	/* 逻辑运算 */
	{2, 5, 5, left2right, oper_and},         // 且  
	{2, 4, 4, left2right, oper_or},          // 或  
	{1, 15, 15, right2left, oper_not},       // 非  
	/* 最小优先级 */
	{2, 0, 0, right2left, oper_min}          // 栈底  
};
/*
扩充运算符号类型时
先将词法扩充，并扩充词法分析规则
再将符号表扩充，并在calculate函数中定义恰当的运算规则

*/

struct token
{
	enum tokenType
	{
		VAR,
		OP
	};
	int type;
	VARIANT var;
	OPERATOR op;

};
//END EVALUATION_H
//INTERPRETER_H



struct if_match//记录每个if可能对应的else和对应的endif所在的行号即pc值
{
	int if_state;
	bool else_sign = false;//每次进入if时初始化这个if的else_sign，并在else中判断后复位
	int else_state;
	int endif_state;
};
struct if_stack_token
{
	int pc;
	int type;
	enum
	{
		IF,
		ELSE,
		ENDIF,
	};
};

struct while_match//记录while的配对情况
{
	int while_state;
	int endwhile_state;
};

struct while_stack_token//当target满足时持续执行循环
{
	while_match record;
	std::vector<D_token> target;//终止目标值表达式
};

struct for_match
{
	int for_state;//起始行号
	int endfor_state;//终止行号
};
struct for_stack_token//记录for语句的配对情况
{
	for_match record;
	std::string var;//用于判断的变量名
	std::vector<D_token> target;//终止目标值表达式
	double step;//步长
};

struct func_call_token
{
	std::vector<D_token> function_call;//函数调用的token流
	int ID;//用于在vector中的索引
//	bool reference = false;//是否是传引用函数
};

struct matrix_visit_token
{
	std::vector<D_token> matrix_visit;
	//行列的值表达式
	std::vector<D_token> row_exp;
	std::vector<D_token> col_exp;
	//求值之后的整型值
	int row;
	int col;
	
	std::string matrixName;//矩阵的名字
	Matrix  *matrix;//指向内存中的值

	
	int ID;//用于在vector中的索引


	//未完成标记

	void separateExp();
	void setEval(Evaluation*);//设置求值器求值，使用当前作用域的求值器

	Evaluation* eval;
	//将结果保存
};


void test_printToken(std::vector<token>, std::string id = "");//输出token流的内容
void test_printOpStack(std::stack<OPERATOR>, std::string id = "");//输出符号栈的内容
void test_printVARIANT(VARIANT var, std::string id = "");//输出值，未定义的类型会给出提示
void test_printDtoken(std::vector<D_token> tokenStream, std::string id = "");
void test_print(std::string s);
void error(std::string errorMsg);
std::vector<std::string> loadFile_cons(std::string filePath);



#endif // !CONSTANT_H
