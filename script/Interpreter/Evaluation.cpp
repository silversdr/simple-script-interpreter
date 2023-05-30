#include "Evaluation.h"

Evaluation::Evaluation()
{
	functionEval = new Function;
}

std::vector<token> Evaluation::DToken2token(std::vector<D_token> tokenstream)
//将Dtoken流转换成token流参与表达式计算
{
	//std::cout << "tokenstream.size" << " " << tokenstream.size()<<std::endl;
	std::vector<token> tmp;
	for (int i=0;i<tokenstream.size();i++)
	{
		
		D_token it = tokenstream[i];
		token tmpToken;
		if (it.abstractType == abstractType::Var_operand)
		{
			
	
			tmpToken.type = tmpToken.VAR;
			switch (it.type)
			{
				
			case D_EXPERSSION_TOKEN_INT://整型常量
				tmpToken.var.type = D_EXPERSSION_TOKEN_INT;
				tmpToken.var.int_val = it.INT_val;
				break;
			case D_EXPRESSION_TOKEN_DOUBLE://实型常量
				tmpToken.var.type = D_EXPRESSION_TOKEN_DOUBLE;
				tmpToken.var.double_val = it.DOUBLE_val;
				break;
			case D_EXPRESSION_TOKEN_STRING://字符串
				tmpToken.var.type = D_EXPRESSION_TOKEN_STRING;
				tmpToken.var.string_val = it.STR_val;
				break;
			case D_EXPRESSION_TOKEN_NAME://变量
				if (memory->count(it.STR_val)==0)//变量还未被赋值
				{
					std::string errorMsg = "错误 表达式中出现未赋值的变量";
					throw(errorMsg);
				}
				tmpToken.var = (*memory)[it.STR_val];
				//从内存中取出值
				//变量赋值之后需要重新对值绑定负号，
				//否则再求值过程中可能出现符号数量不匹配的异常
				break;
			case D_EXPRESSION_TOKEN_FUNCCALL://函数调用标记，调用函数求值器进行求值
				tmpToken.type = tmpToken.VAR;
				try
				{
					tmpToken.var = functionEval->execFunction(it);
					//返回值tmpToken
				}
				catch (std::string error)
				{
					std::string errorMsg = "函数求值错误\n" + error;
					throw(errorMsg);
				}
				if (tmpToken.var.type == D_EXPRESSION_TOKEN_NULLVARIANT)
				{
					//返回空值，不能出现在表达式中，报错
					std::string errorMsg = "表达式中出现空值，请检查函数调用是否正确";
					throw(errorMsg);
				}
				

				break;
			case D_EXPRESSION_TOKEN_MATRIX_VISIT://矩阵访问
				tmpToken.type = tmpToken.VAR;
				try 
				{
					Evaluation tmpeval;
					tmpeval.setMemory(memory);//设置求值器和函数求值器内存
					tmpeval.functionEval->inPutFuncCallVec(this->functionEval->getFunVec());//设置函数求值标记
					tmpeval.functionEval->loadFile(this->functionEval->getFiles());//设置函数文件
					tmpeval.setMatrixVisitVec(this->matrix_visit_vec);//为临时求值器传递矩阵访问标记
					matrix_visit_token m;
					m = (*matrix_visit_vec)[it.matrix_visit_id];//获取矩阵访问信息
					

					tmpToken.var = matrixVisit(&m, &tmpeval, memory);//进行矩阵访问和矩阵内存绑定,获取结果值


				}
				catch (std::string e)
				{
					error("矩阵值访问错误\n" + e);
				}

				break;
			case D_EXPRESSION_TOKEN_PROERTY://属性访问
				tmpToken.type = tmpToken.VAR;
				
				tmpToken.var = propertyEval(it);//计算属性值
				break;
			default:
				break;
			}
			tmp.push_back(tmpToken);
			continue;
		}
		else if (it.abstractType == abstractType::Var_operator)
		{
			//std::cout << "***" << std::endl;
			tmpToken.type = tmpToken.OP;
			for (auto i : operators)
			{
				if (it.type == i.type)
				{
					tmpToken.op.type = it.type;
					tmpToken.op.ass = i.ass;
					tmpToken.op.icp = i.icp;
					tmpToken.op.isp = i.isp;
					tmpToken.op.numbers = i.numbers;
					
				}
			}
			tmp.push_back(tmpToken);
			continue;


		}
	}
	//std::cout << "size: " << tmp.size()<<std::endl;
	tmp = boundNegative(tmp);
	return tmp;
}

std::vector<token> Evaluation::boundNegative(std::vector<token> tokenStream)
{
	//负号出现的情况
	/*
	连续两个操作符出现（对于第一个操作符，右括号除外，因为括号内部可能是表达式）
	且第二个操作符后方出现 常量 类型
	则第二个操作符可以结合为负号
	否则按减号看待
	*/
	std::vector<token> newTokenStream;
	int i = 0;
	int size = tokenStream.size();
	if (size > 2)
	{
		for (i = 0; i < size - 2; i++)
		{
			token tk = tokenStream[i];
			if (tokenStream[i].type == tk.OP &&
				(tokenStream[i].op.type != D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS))
			{
				//std::cout << "1" << std::endl;
				if (tokenStream[i + 1].op.type == D_EXPRESSION_TOKEN_MINUS)
				{
					if (tokenStream[i + 2].type == tk.VAR)
					{

						token tk2 = tokenStream[i + 2];
						token tk1, op;
						tk1.type = tk1.VAR;
						tk1.var.type = D_EXPERSSION_TOKEN_INT;
						tk1.var.int_val = -1;
						op.type = op.OP;
						op.op.type = D_EXPERSSION_TOKEN_MULTI;
						for (auto i : operators)
						{
							if (op.op.type == i.type)
							{

								op.op.ass = i.ass;
								op.op.icp = i.icp;
								op.op.isp = i.isp;
								op.op.numbers = i.numbers;

							}
						}//利用符号表匹配符号
						std::vector<token> exp;
						exp.push_back(tk1);
						exp.push_back(op);
						exp.push_back(tk2);
						Evaluation tmpEval;
						tmpEval.setMemory(this->memory);
						tmpEval.nifix2postfix(exp);
						tmpEval.evals(tmpEval.postfixExpression);
						//tmpEval.nifix2postfix =  tmpEval.nifix2postfix(exp);
						//构造后缀表达式用求值器进行计算
						token result;
						result.type = result.VAR;
						result.var = tmpEval.result;
						
						newTokenStream.push_back(tokenStream[i]);
						newTokenStream.push_back(result);

						i += 2;
						continue;
					}
				}

			}
			if(i == 0 && tokenStream[i].op.type == D_EXPRESSION_TOKEN_MINUS)
			{
				//std::cout << "in" << std::endl;
				if (tokenStream[i + 1].type == tk.VAR)
				{
					token tk2 = tokenStream[i + 1];
					token tk1, op;
					tk1.type = tk1.VAR;
					tk1.var.type = D_EXPERSSION_TOKEN_INT;
					tk1.var.int_val = -1;
					op.type = op.OP;
					op.op.type = D_EXPERSSION_TOKEN_MULTI;
					for (auto i : operators)
					{
						if (op.op.type == i.type)
						{

							op.op.ass = i.ass;
							op.op.icp = i.icp;
							op.op.isp = i.isp;
							op.op.numbers = i.numbers;

						}
					}//利用符号表匹配符号
					std::vector<token> exp;
					exp.push_back(tk1);
					exp.push_back(tk2);
					exp.push_back(op);
					//构造后缀表达式用求值器进行计算
					token result;
					result.type = result.VAR;
					result.var = evals(exp);

					
					newTokenStream.push_back(result);//存储结果

					i += 1;
					//std::cout << "in" << std::endl;
					continue;
				}
			}
			newTokenStream.push_back(tk);


		}
		for (; i < tokenStream.size(); i++)
		{
			token tk = tokenStream[i];
			newTokenStream.push_back(tk);
		}
		//std::cout << "test" << std::endl;
		//test_printToken(newTokenStream);
		return newTokenStream;
	}
	else if (size == 2)
	{
		//std::cout << "in" << std::endl;
		if (tokenStream[0].type == token::OP && tokenStream[0].op.type == D_EXPRESSION_TOKEN_MINUS)
		{
			//std::cout << "in" << std::endl;
			if (tokenStream[1].type == token::VAR)//如果是 - var的形式
			{
				//std::cout << "in" << std::endl;
				token tk = tokenStream[1];
				token tk1, op;
				tk1.type = tk1.VAR;
				tk1.var.type = D_EXPERSSION_TOKEN_INT;
				tk1.var.int_val = -1;
				op.type = op.OP;
				op.op.type = D_EXPERSSION_TOKEN_MULTI;
				for (auto i : operators)
				{
					if (op.op.type == i.type)
					{
						
						op.op.ass = i.ass;
						op.op.icp = i.icp;
						op.op.isp = i.isp;
						op.op.numbers = i.numbers;

					}
				}//利用符号表匹配符号
				std::vector<token> exp;
				exp.push_back(tk1);
				exp.push_back(tk);
				exp.push_back(op);
				//构造后缀表达式用求值器进行计算
				token result;
				result.type = result.VAR;
				result.var = evals(exp);

				newTokenStream.push_back(result);
				return newTokenStream;

			}
			
		}
		return tokenStream;

	}
	else
	{
		return tokenStream;
	}
	return newTokenStream;
}



void Evaluation::printVARIANT(VARIANT var)
{
	
	
	
	switch (var.type)
	{
	case D_EXPERSSION_TOKEN_INT:
		std::cout << var.int_val;
	
		break;
	case D_EXPRESSION_TOKEN_DOUBLE:
		std::cout << var.double_val;
	
		break;
	case D_EXPRESSION_TOKEN_STRING:
		std::cout << var.string_val;
	
		break;
	case D_EXPRESSION_TOKEN_MATRIX:
		std::cout << std::endl << var.matrix_val << std::endl;
	
		break;
		
	default:
		std::string errorMsg = "print未知类型的值";
		throw(errorMsg);


	}
}

void Evaluation::inputDataStream(std::vector<D_token> dataStream)
{
	try {
		this->calcuTokenStream = DToken2token(dataStream);
		//test_print("in");
		this->postfixExpression = nifix2postfix(calcuTokenStream);

		this->result = evals(postfixExpression);
	}
	catch (std::string e)
	{
		error("求值器错误\n" + e);
	}
}

bool Evaluation::isNotBracket(OPERATOR op)
{
	if (op.type == D_EXPRESSION_TOKEN_LEFT_PARENTHESIS || op.type == D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS)
	{
		return false;
	}
	return true;
}

std::vector<token> Evaluation::nifix2postfix(std::vector<token> nifixExpression)
{
	//test_printToken(nifixExpression, "nifix");

	std::stack<OPERATOR> op;
	std::vector<token> postfixExpression;
	token min;
	min.type = min.OP;
	min.op = { 2, 0, 0, right2left, oper_min };
	op.push(min.op);//初始化栈底，优先级最低
	
	for (int i = 0; i < nifixExpression.size(); i++)
	{
		//test_printToken(postfixExpression, "FOR STATE");
		//test_printOpStack(op);
		token tmpToken = nifixExpression[i];
		if (tmpToken.type == tmpToken.VAR)
		{
			postfixExpression.push_back(tmpToken);
			continue;
		}
		else if(tmpToken.type == tmpToken.OP)
		{
			OPERATOR oper = tmpToken.op;
			
			if (isNotBracket(oper))
			{
				
				if (oper.icp > op.top().isp)
				{
					op.push(oper);
					continue;
				}
				else if (oper.icp == op.top().isp)
				{
					op.push(oper);
					continue;
				}
				else if (oper.icp < op.top().isp)
				{

					while (oper.icp < op.top().isp)//当栈顶符号的优先级较当前符号更大时，连续弹出栈顶符号直到第一个比它小的符号出现
					{
						OPERATOR tmp = op.top();
						token tmpToken;
						tmpToken.type = tmpToken.OP;
						tmpToken.op = op.top();
						op.pop();
						postfixExpression.push_back(tmpToken);
					}
					op.push(oper);
					continue;

				}
			}
			if (!isNotBracket(oper))
			{
				//std::cout << "in" << std::endl;
				if (oper.type == D_EXPRESSION_TOKEN_LEFT_PARENTHESIS)
				{
					
					op.push(oper);
					continue;
				}
				if (oper.type == D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS)
				{
					
					do
					{
						if (op.size() == 1)//如果此时符号栈为空
						{
							

							std::string errorMsg = "表达式错误：未找到匹配的左括号";
							throw(errorMsg);
						}
						OPERATOR tmp = op.top();
						token tmpToken;
						tmpToken.type = tmpToken.OP;
						tmpToken.op = op.top();
						op.pop();
						postfixExpression.push_back(tmpToken);

						//不断弹出元素直到弹出左括号，找不到左括号则报错
					} while (op.top().type != D_EXPRESSION_TOKEN_LEFT_PARENTHESIS && op.size() > 1);
					//判断终止条件
					if (op.top().type == D_EXPRESSION_TOKEN_LEFT_PARENTHESIS)
					{
						op.pop();
						continue;
					}
					else
					{
						std::string errorMsg = "表达式错误：未找到匹配的左括号";
						throw(errorMsg);
					}
				}
			}
		}
		
		
	}//结束总循环
	//test_printOpStack(op);
	//test_printToken(postfixExpression, "AFTER FOR STATE");
	while (op.top().type != oper_min)
	{
		OPERATOR tmp = op.top();
		token tmpToken;
		tmpToken.type = tmpToken.OP;
		tmpToken.op = op.top();
		op.pop();
		postfixExpression.push_back(tmpToken);
	}//循环处理结束后连续弹出符号直到碰到栈顶
	
	//test_printToken(postfixExpression,"END Evaluation::nifix2postfix");
	this->postfixExpression = postfixExpression;
	return postfixExpression;
}

VARIANT Evaluation::evals(std::vector<token> postfixExpression)
{
	std::stack<VARIANT> varStack;
	for (int i = 0; i < postfixExpression.size(); i++)
	{
		token tmpToken = postfixExpression[i];
		if (tmpToken.type == tmpToken.VAR)
		{
			varStack.push(tmpToken.var);
		}
		else if(tmpToken.type==tmpToken.OP)
		{
			VARIANT var1, var2;
			
			if(tmpToken.op.numbers == 2)
			{
				if (varStack.size() < 2)
				{
					std::string errorMsg = "表达式异常 符号数不匹配";

					throw(errorMsg);
				}
				var2 = varStack.top();
				varStack.pop();
				var1 = varStack.top();
				varStack.pop();
				VARIANT result = calculate(var1, tmpToken.op, var2);
				varStack.push(result);

			}
			else if (tmpToken.op.numbers == 1)
			{
				if (varStack.size() < 1)
				{
					std::string errorMsg = "表达式异常 符号数不匹配";

					throw(errorMsg);
				}
				var1 = varStack.top();
				varStack.pop();
				VARIANT result = calculate(var1,tmpToken.op);
				varStack.push(result);
			}
		}
	}//总循环结束
	if (varStack.size() > 1)
	{
		std::string errorMsg = "表达式异常 过多的操作数";
		throw(errorMsg);
		
	}
	else if(varStack.size()==0)
	{
		std::string errorMsg = "表达式异常 操作数栈为空";
		throw(errorMsg);
		
	}
	else
	{	
		
		//test_printVARIANT(varStack.top(), "Evaluation::eval: ");
		//std::cout << std::endl;
		this->result = varStack.top();
		return varStack.top();
	}
}

VARIANT Evaluation::calculate(VARIANT var1, OPERATOR op, VARIANT var2)
{

	VARIANT result;
	switch (op.type)
	{
		/*
		
		 + - * / 
		
		*/
	case D_EXPRESSION_TOKEN_PLUS:
		if (var1.type == D_EXPRESSION_TOKEN_STRING && var2.type == D_EXPRESSION_TOKEN_STRING)//字符串加法
		{
			result.type = D_EXPRESSION_TOKEN_STRING;
			result.string_val = var1.string_val + var2.string_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型加法
		{
			result.type = D_EXPERSSION_TOKEN_INT;
			result.int_val = var1.int_val + var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型加法
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			result.double_val = var1.double_val + var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合加法，全部转换成实型计算
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			result.double_val = var1.double_val + var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			result.double_val = var1.int_val + var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_MATRIX && var2.type == D_EXPRESSION_TOKEN_MATRIX)//矩阵加法
		{
			result.type = D_EXPRESSION_TOKEN_MATRIX;
			result.matrix_val = var1.matrix_val + var2.matrix_val;
			return result;
		}
		else
		{
			//未定义的加法运算，报错
			
			std::string errorMsg = "错误 未定义的加法运算";
			throw(errorMsg);
		}
		break;

	case D_EXPRESSION_TOKEN_MINUS:

		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型减法
		{
		result.type = D_EXPERSSION_TOKEN_INT;
		result.int_val = var1.int_val - var2.int_val;
		return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型减法
		{
		result.type = D_EXPRESSION_TOKEN_DOUBLE;
		result.double_val = var1.double_val - var2.double_val;
		return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合减法，全部转换成实型计算
		{
		result.type = D_EXPRESSION_TOKEN_DOUBLE;
		result.double_val = var1.double_val - var2.int_val;
		return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{
		result.type = D_EXPRESSION_TOKEN_DOUBLE;
		result.double_val = var1.int_val - var2.double_val;
		return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_MATRIX && var2.type == D_EXPRESSION_TOKEN_MATRIX)//矩阵减法
		{
			result.type = D_EXPRESSION_TOKEN_MATRIX;
			result.matrix_val = var1.matrix_val - var2.matrix_val;
			return result;
		}
		else
		{
		//未定义的减法运算，报错
			std::string errorMsg = "错误 未定义的减法运算";
			throw(errorMsg);
		}
		break;


	case D_EXPERSSION_TOKEN_MULTI:
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型乘法
		{
			result.type = D_EXPERSSION_TOKEN_INT;
			result.int_val = var1.int_val * var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型乘法
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			result.double_val = var1.double_val * var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合乘法，全部转换成实型计算
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			result.double_val = var1.double_val * var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			result.double_val = var1.int_val * var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_MATRIX && var2.type == D_EXPRESSION_TOKEN_MATRIX)//矩阵乘法
		{
			result.type = D_EXPRESSION_TOKEN_MATRIX;
			result.matrix_val = var1.matrix_val * var2.matrix_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_MATRIX &&
			((var2.type == D_EXPRESSION_TOKEN_DOUBLE) || (var2.type == D_EXPERSSION_TOKEN_INT)))//矩阵的数乘
		{
			result.type = D_EXPRESSION_TOKEN_MATRIX;
			
			if (var2.type == D_EXPRESSION_TOKEN_DOUBLE)//与实型数数乘
			{
				result.matrix_val = var1.matrix_val.scalarMulti(var2.double_val);
			}
			else//与整数数乘
			{
				result.matrix_val = var1.matrix_val.scalarMulti(var2.int_val);
			}
			return result;
		}
		else if (var2.type == D_EXPRESSION_TOKEN_MATRIX &&
			((var1.type == D_EXPRESSION_TOKEN_DOUBLE) || (var1.type == D_EXPERSSION_TOKEN_INT)))//矩阵的数乘
		{
			result.type = D_EXPRESSION_TOKEN_MATRIX;

			if (var1.type == D_EXPRESSION_TOKEN_DOUBLE)//与实型数数乘
			{
				result.matrix_val = var2.matrix_val.scalarMulti(var1.double_val);
			}
			else//与整数数乘
			{			
				result.matrix_val = var2.matrix_val.scalarMulti(var1.int_val);
			}
			return result;
		}
		else
		{
			//未定义的乘法运算，报错
			std::string errorMsg = "错误 未定义的乘法运算";
			throw(errorMsg);
		}
		break;

	case D_EXPRESSION_TOKEN_DIV:
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型除法
		{
			result.type = D_EXPERSSION_TOKEN_INT;
			if (var2.int_val != 0)
			{
				result.int_val = var1.int_val / var2.int_val;
			}
			else
			{
				std::string errorMsg = "错误 除数不能为0";
				throw(errorMsg);
			}
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型除法
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			if (var2.double_val != 0)
			{
				result.double_val = var1.double_val / var2.double_val;
			}
			else
			{
				std::string errorMsg = "错误 除数不能为0";
				throw(errorMsg);
			}
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合除法，全部转换成实型计算
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			if (var2.int_val != 0)
			{
				result.double_val = var1.double_val / var2.int_val;
			}
			else
			{
				std::string errorMsg = "错误 除数不能为0";
				throw(errorMsg);
			}
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{
			result.type = D_EXPRESSION_TOKEN_DOUBLE;
			if (var2.double_val != 0)
			{
				result.double_val = var1.int_val / var2.double_val;
			}
			else
			{
				std::string errorMsg = "错误 除数不能为0";
				throw(errorMsg);
			}
			return result;
		}
		else
		{
			//未定义的除法运算，报错
			std::string errorMsg = "错误 未定义的除法运算";
			throw(errorMsg);
		}

		break;


	case D_EXPRESSION_TOKEN_MOD:
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val % var2.int_val;
			return result;
		}
		else
		{
			//未定义的除法运算，报错
			std::string errorMsg = "错误 未定义的取余运算";
			throw(errorMsg);
		}

	
	case D_EXPRESSION_TOKEN_EQUAL:
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{
			
			result.int_val = var1.int_val == var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{
			
			result.int_val = var1.double_val == var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_MATRIX && var2.type == D_EXPRESSION_TOKEN_MATRIX)//矩阵
		{
			result.int_val = var1.matrix_val == var2.matrix_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_STRING && var2.type == D_EXPRESSION_TOKEN_STRING)//字符串
		{
			result.int_val = var1.string_val == var2.string_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{
	
			result.int_val = var1.double_val == var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{
		
			result.int_val = var1.int_val == var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 相等 判断";
			throw (errorMsg);
		}
		break;//等于

	case D_EXPRESSION_TOKEN_NE:
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val != var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{

			result.int_val = var1.double_val != var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_MATRIX && var2.type == D_EXPRESSION_TOKEN_MATRIX)//矩阵
		{
			result.int_val = var1.matrix_val != var2.matrix_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_STRING && var2.type == D_EXPRESSION_TOKEN_STRING)//字符串
		{
			result.int_val = var1.string_val != var2.string_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{

			result.int_val = var1.double_val != var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{

			result.int_val = var1.int_val != var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 不等 判断";
			throw (errorMsg);
		}
		break;//不等于

	case D_EXPRESSION_TOKEN_GT:
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val > var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{

			result.int_val = var1.double_val > var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{

			result.int_val = var1.double_val > var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{

			result.int_val = var1.int_val > var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 大于 判断";
			throw (errorMsg);
		}
		break;//大于
	
	case D_EXPRESSION_TOKEN_GE:
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val >= var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{

			result.int_val = var1.double_val >= var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{

			result.int_val = var1.double_val >= var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{

			result.int_val = var1.int_val >= var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 大于等于 判断";
			throw (errorMsg);
		}
		break;//大于等于


	case D_EXPRESSION_TOKEN_LT:
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val < var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{

			result.int_val = var1.double_val < var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{

			result.int_val = var1.double_val < var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{

			result.int_val = var1.int_val < var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 小于 判断";
			throw (errorMsg);
		}
		break;//小于

	case D_EXPRESSION_TOKEN_LE:
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val <= var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{

			result.int_val = var1.double_val <= var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{

			result.int_val = var1.double_val <= var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{

			result.int_val = var1.int_val <= var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 小于等于 判断";
			throw (errorMsg);
		}
		break;//小于

	case D_EXPERSSION_TOKEN_AND://与
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val && var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{

			result.int_val = var1.double_val && var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{

			result.int_val = var1.double_val && var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{

			result.int_val = var1.int_val && var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 与 运算";
			throw (errorMsg);
		}
		break;//与

	case D_EXPRESSION_TOKEN_OR://或
		result.type = D_EXPERSSION_TOKEN_INT;
		if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPERSSION_TOKEN_INT)//整型
		{

			result.int_val = var1.int_val || var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPRESSION_TOKEN_DOUBLE)//实型
		{

			result.int_val = var1.double_val || var2.double_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE && var2.type == D_EXPERSSION_TOKEN_INT)//整型和实型混合
		{

			result.int_val = var1.double_val || var2.int_val;
			return result;
		}
		else if (var1.type == D_EXPERSSION_TOKEN_INT && var2.type == D_EXPRESSION_TOKEN_DOUBLE)
		{

			result.int_val = var1.int_val || var2.double_val;
			return result;
		}
		else
		{
			std::string errorMsg = "未定义的 或 运算";
			throw (errorMsg);
		}
		break;//或



	default:
		std::string errorMsg = "错误 未定义的求值操作";
		throw(errorMsg);
		break;
	}
}

VARIANT Evaluation::calculate(VARIANT var1, OPERATOR op)
{

	/* 
	 
	 ！ 
	
	*/
	VARIANT result;
	switch (op.type)
	{
	case D_EXPRESSION_TOKEN_NOT:
		if (var1.type == D_EXPERSSION_TOKEN_INT)
		{
			result.type = D_EXPERSSION_TOKEN_INT;
			result.int_val = !(int)var1.int_val;
			return result;
		}
		else if (var1.type == D_EXPRESSION_TOKEN_DOUBLE)
		{
			result.type = D_EXPERSSION_TOKEN_INT;
			result.int_val = !var1.int_val;
			return result;
		}
		else
		{
			std::string errorMsg = "错误 未定义的取反运算";
			throw(errorMsg);
		}
		break;

	default:
		std::string errorMsg = "错误 未定义的求值操作";
		throw(errorMsg);
		
		break;
	}
}

void Evaluation::setMemory(std::map<std::string, VARIANT>* memory)
{
	this->memory = memory;
	this->functionEval->setParentMemory(memory);
}


bool Evaluation::getLogicResult()
{
	switch (this->result.type)
	{
	case D_EXPERSSION_TOKEN_INT:
		if (result.int_val)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	case D_EXPRESSION_TOKEN_DOUBLE:
		if(result.double_val)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	default:
		std::string errorMsg = "错误 表达式无法作为逻辑表达式求值";
		throw(errorMsg);
		break;

	}
}


void Evaluation::inPutFuncCallVec(std::vector<func_call_token> vec)
{
	this->functionEval->inPutFuncCallVec(vec);
}

void Evaluation::setMatrixVisitVec(std::vector<matrix_visit_token> *vec)
{
	this->matrix_visit_vec = vec;
	this->functionEval->setMatrixVisitVec(vec);
}

VARIANT Evaluation::matrixVisit(matrix_visit_token* matrixVisitInfo, Evaluation* evals, std::map<std::string, VARIANT>* memory)
//参数 矩阵信息结构体指针 指向初始化完成的矩阵信息结构体
//     求值器指针 指向初始化完成的求值器
//     内存空间指针 指向本作用域的内存空间
{
	
	if (memory->count(matrixVisitInfo->matrixName) == 0)
	{
		error("矩阵访问错误 尝试访问未定义的变量");

	}
	if ((*memory)[matrixVisitInfo->matrixName].type != D_EXPRESSION_TOKEN_MATRIX)
	{
		error("矩阵访问错误 尝试以矩阵访问形式访问非矩阵变量");
	}
	matrixVisitInfo->matrix = &(*memory)[matrixVisitInfo->matrixName].matrix_val;//设置内存指针
	//尝试求值
	try
	{
		evals->inputDataStream(matrixVisitInfo->row_exp);
		if (evals->result.type != D_EXPERSSION_TOKEN_INT && evals->result.type!=D_EXPRESSION_TOKEN_DOUBLE)
		{
			error("错误 矩阵索引值只能为整型或实型");
		}
		if (evals->result.type == D_EXPERSSION_TOKEN_INT)
		{
			matrixVisitInfo->row = evals->result.int_val;
		}
		else
		{
			matrixVisitInfo->row = (int)evals->result.double_val;
		}

		evals->inputDataStream(matrixVisitInfo->col_exp);
		if (evals->result.type != D_EXPERSSION_TOKEN_INT && evals->result.type != D_EXPRESSION_TOKEN_DOUBLE)
		{
			error("错误 矩阵索引值只能为整型或实型");
		}
		if (evals->result.type == D_EXPERSSION_TOKEN_INT)
		{
			matrixVisitInfo->col = evals->result.int_val;
		}
		else
		{
			matrixVisitInfo->col = (int)evals->result.double_val;
		}
		//如果表达式计算得到实型的值，将它强制类型转换成整型使用

		if (matrixVisitInfo->matrix->getCol() <= matrixVisitInfo->col)
		{
			error(matrixVisitInfo->matrixName+"  访问错误 矩阵 列 索引 上溢出");
		}
		if (matrixVisitInfo->col < 0)
		{
			error(matrixVisitInfo->matrixName + "  访问错误 矩阵 列 索引 下溢出");
		}
		if (matrixVisitInfo->matrix->getRow() <= matrixVisitInfo->row)
		{
			error(matrixVisitInfo->matrixName + "  访问错误 矩阵 行 索引 上溢出");
		}
		if (matrixVisitInfo->row < 0)
		{
			error(matrixVisitInfo->matrixName + "  访问错误 矩阵 行 索引 下溢出");
		}
		VARIANT result;
		result.type = D_EXPRESSION_TOKEN_DOUBLE;
		//result.double_val = matrixVisitInfo->matrix->double_ptr[matrixVisitInfo->col + 
		//	matrixVisitInfo->row * matrixVisitInfo->matrix->getCol()];//row 行 col 列
		result.double_val = matrixVisitInfo->matrix->value[matrixVisitInfo->row][matrixVisitInfo->col];
		return result;
	}
	catch (std::string s)
	{
		error(s);
	}
	catch (...)
	{
		error("Evaluation::matrixVisit 未知错误类型");
	}

}

VARIANT Evaluation::propertyEval(D_token propertySign)
{
	//std::cout << "in";
	if (memory->count(propertySign.STR_val) == 0)
	{
		error("错误 尝试使用未赋值变量的属性");
	}
	VARIANT result;
	
	if (propertySign.reserved_propertyType != -1)
	{
		switch (propertySign.reserved_propertyType)
		{
		case MATRIX_ROW://矩阵操作
			result.type = D_EXPERSSION_TOKEN_INT;
			if ((*memory)[propertySign.STR_val].type!=D_EXPRESSION_TOKEN_MATRIX)//尝试对非矩阵值访问行列
			{
				error("Row 属性使用错误 尝试访问非矩阵值的行");
			}
			result.int_val = (*memory)[propertySign.STR_val].matrix_val.getRow();
	
			return result;
			break;
		case MATRIX_COL:
			result.type = D_EXPERSSION_TOKEN_INT;
			if ((*memory)[propertySign.STR_val].type != D_EXPRESSION_TOKEN_MATRIX)//尝试对非矩阵值访问行列
			{
				error("Col 属性使用错误 尝试访问非矩阵值的列");
			}
			result.int_val = (*memory)[propertySign.STR_val].matrix_val.getCol();
			return result;
			break;

		default:
			error(propertySign.STR_val + "保留属性值未实现");
			break;
		}

	}
	else//处理用户绑定属性值
	{
		
	}
	error("找不到属性");
}