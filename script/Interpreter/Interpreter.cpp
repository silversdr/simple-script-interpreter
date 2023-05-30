#include "Interpreter.h"

Interpreter::Interpreter()
{
	evals = new Evaluation;
	this->evals->setMemory(&this->memory);//为求值器设置内存
	
	
}

void Interpreter::loadFunctionFile(std::string filePath)
{
	if (evals == NULL)
	{
		std::string errorMsg = "求值器为空";
		throw(errorMsg);
	}
	if (evals->functionEval == NULL)
	{
		std::string errorMsg = "函数求值器为空";
		throw(errorMsg);
	}
	evals->functionEval->loadFile(filePath);
}
void Interpreter::loadFunctionFile(std::vector<std::string> filesPath)
{
	for (auto line : filesPath)
	{
		this->loadFunctionFile(line);
	}
}

void Interpreter::unloadFunctionFile(std::string filePath)
{
	if (evals == NULL)
	{
		std::string errorMsg = "求值器为空";
		throw(errorMsg);
	}
	if (evals->functionEval == NULL)
	{
		std::string errorMsg = "函数求值器为空";
		throw(errorMsg);
	}
	evals->functionEval->unloadFile(filePath);
}

void Interpreter::unloadFunctionFile(std::vector<std::string> filesPath)
{
	for (auto line : filesPath)
	{
		this->unloadFunctionFile(line);
	}
}


void Interpreter::inputScript(std::vector<std::vector<D_token>> script)
{
	for(int i=0;i<script.size();i++)
	script[i] = boundProperty(script[i]);//进行属性绑定
	
	
	for (int i = 0; i < script.size(); i++)//进行矩阵绑定
	{
		for (int j = 0; j < script[i].size(); j++)
		{
			if (script[i][j].type == D_EXPRESSION_TOKEN_NAME)
				script[i] = boundMatrixVisit(script[i], j);
		}
	}
	
	for (int i = 0; i < script.size(); i++)//进行函数绑定
	{
		for (int j = 0; j < script[i].size(); j++)
		{
			if (script[i][j].type == D_EXPRESSION_TOKEN_NAME)
				script[i] =boundFuctionCall(script[i], j);
		}
	}
	
	
	this->script = script;

	evals->functionEval->inPutFuncCallVec(func_call_vec);//对函数求值器f设置func_call_vec
	evals->setMatrixVisitVec(&martrix_visit_vec);//对求值器设置矩阵访问标记

	//test_printDtoken(script[2]);
	this->execScript(this->script);//执行脚本
	
}
//输入d_token流

void Interpreter::execCode(std::vector<D_token > line)
{
	if (line.size() == 0)//空语句
	{
		execNULL();
	}
	else
	{
		switch (line[0].type)
		{
		case D_EXPRESSION_TOKEN_NAME:
		case D_EXPRESSION_TOKEN_MATRIX_VISIT:
			execASIGN(line);
			break;
		case D_EXPRESSION_TOKEN_PRINT:
			execPRINT(line);
			break;
		case D_EXPRESSION_TOKEN_FOR:
			execFOR(line);
			break;
		case D_EXPRESSION_TOKEN_ENDFOR:
			execENDFOR(line);
			break;
		case D_EXPERSSION_TOKEN_IF:
			execIF(line);
			break;
		case D_EXPRESSION_TOKEN_ELSE:
			execELSE(line);
			break;
		case D_EXPRESSION_TOKEN_ENDIF:
			execENDIF(line);
			break;
		case D_EXPRESSION_TOKEN_WHILE:
			execWHILE(line);
			break;
		case D_EXPERSSION_TOKEN_ENDWHILE:
			execENDWHILE(line);
			break;
		case D_EXPRESSION_TOKEN_FUNCCALL://单独的函数调用语句
			execFUNCTION(line);
			break;
		case D_EXPRESSION_TOKEN_RETURN:
			execRETURN(line);
			break;
		case D_EXPRESSION_TOKEN_ENDFUNC:
			execENDFUNC(line);
			break;
		default:
			std::string sentence;
			for (int i = 0; i< line.size(); i++)
			{
				sentence += " " + line[i].STR_val;
			}
			error("无效的语句 "+sentence+" Interpreter::execCode");
			break;

		}


		
		
	}

}

void Interpreter::execFUNCTION(std::vector<D_token > line)
{

	if (line.size() != 1)
	{
		std::string errorMsg = "错误的函数调用语句 token过多";
		throw(errorMsg);
	}
	try {
		evals->functionEval->execFunction(line[0]);//处理单句函数调用的情况 scope->funcname(arg)

	}
	catch (std::string er)
	{
		throw("错误的函数解析 请检查函数语法是否正确\n" + er);
	}
}

void Interpreter::execRETURN(std::vector<D_token > line)
{
	int size = this->script.size() - 1;
	if (this->script[size][0].type != D_EXPRESSION_TOKEN_ENDFUNC)
	{
		error("错误的返回语句，返回语句需要在函数中使用");
	}
	
	if (line.size() < 2)
	{
		error("需要返回值");
	}
	else if (line.size() > 2)
	{
		error("错误 不允许返回表达式");
	}
	else if (line[1].type == D_EXPRESSION_TOKEN_NULL)
	{
		returnValue.type = D_EXPRESSION_TOKEN_NULLVARIANT;
		return;
	}//处理空返回值
	else if (memory.count(line[1].STR_val) == 0)
	{
		error("尝试返回未使用的变量(可能是函数中的变量)" + line[1].STR_val);
	}
	this->returnValue = memory[line[1].STR_val];
	pc = this->script.size();//直接结束
	return;
}//处理函数中途返回的情况

void Interpreter::execENDFUNC(std::vector<D_token > line)
{
	//对第三个参数开始的表达式进行求值
	std::vector<D_token > exp;
	if (line.size() != 3 )
	{
		error("错误的返回");
		return;
	}
	if (line[2].type==D_EXPRESSION_TOKEN_NULL)
	{
		returnValue.type = D_EXPRESSION_TOKEN_NULLVARIANT;
		return;
	}//处理函数末尾的空返回值
	if (memory.count(line[2].STR_val) == 0)
	{
		error("尝试返回未使用的变量(可能是函数中的变量)" + line[2].STR_val);
	}
	this->returnValue = memory[line[2].STR_val];//返回值
	
}//处理函数尾返回的情况

void Interpreter::execNULL()
{
	return;
}

void Interpreter::execASIGN(std::vector<D_token > line)//赋值语句
{
	if (line.size() < 3)
	{
		std::string errorMsg = "错误的赋值 或 表达式计算";
		throw(errorMsg);
	}
	if (line[0].type == D_EXPRESSION_TOKEN_NAME)
	{
		if (line[1].type == D_EXPRESSION_TOKEN_EQUALFOR && line[2].type == D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS)//矩阵赋值
		{
			std::vector<D_token > matrixIniLine;
			for (int i = 2; i < line.size(); i++)
			{
				matrixIniLine.push_back(line[i]);
			}
			Matrix m(matrixInitial(matrixIniLine));
			VARIANT matrix;
			matrix.type = D_EXPRESSION_TOKEN_MATRIX;
			matrix.matrix_val = m;
			memory[line[0].STR_val] = matrix;



		}
		else //表达式计算赋值
		{
			if (!(line[1].type == D_EXPRESSION_TOKEN_EQUALFOR))
			{
				std::string errorMsg = "错误的赋值 或 表达式计算";
				throw(errorMsg);
			}

			std::vector<D_token > exp;
			for (int i = 2; i < line.size(); i++)
			{
				exp.push_back(line[i]);
			}//exp中是可能合法的中缀表达式
			evals->inputDataStream(exp);//使用求值器进行求值，可能会抛出异常

			memory[line[0].STR_val] = evals->result;//将值存储到内存中
		}
	}
	else if (line[0].type == D_EXPRESSION_TOKEN_MATRIX_VISIT)//矩阵元素赋值
	{
		
		matrix_visit_token token = martrix_visit_vec[line[0].matrix_visit_id];
		evals->matrixVisit(&token, this->evals, &memory);//获取矩阵的内存地址指针

		std::vector<D_token > exp;
		for (int i = 2; i < line.size(); i++)
		{
			exp.push_back(line[i]);
		}//exp中是可能合法的中缀表达式


		evals->inputDataStream(exp);//使用求值器进行求值，可能会抛出异常
		if (evals->result.type != D_EXPERSSION_TOKEN_INT && evals->result.type != D_EXPRESSION_TOKEN_DOUBLE)
		{
			error("矩阵赋值错误 尝试给矩阵 " + token.matrixName + " 赋值非整型和非实型的值");
		}


		if (evals->result.type == D_EXPERSSION_TOKEN_INT)
			//token.matrix->double_ptr[token.col + token.row * token.matrix->getCol()] = (double)evals->result.int_val;
			token.matrix->value[token.row][token.col] = evals->result.int_val;
		else
			//token.matrix->double_ptr[token.col + token.row * token.matrix->getCol()] = (double)evals->result.double_val;
			token.matrix->value[token.row][token.col] = evals->result.double_val;

	}
}

void Interpreter::execPRINT(std::vector<D_token > line)//尽量不控制格式//矩阵除外
{
	
	std::vector< std::vector<D_token >> exps;
	std::vector<D_token > exp;
	int i = 1;
		for (i = 1; i < line.size(); i++)
		{
			
				if (line[i].type == D_EXPRESSION_TOKEN_COMMA)
				{
					if (!exp.empty())
					{
						exps.push_back(exp);
						exp.clear();
						continue;
					}

				}//是逗号就结束
				exp.push_back(line[i]);
			
		}
		if (!exp.empty())
		{
			exps.push_back(exp);
			exp.clear();
			
		}
	
	//以逗号进行分隔
	for (auto exp : exps)
	{

		if (exp.size() == 1 && exp[0].type == D_EXPRESSION_TOKEN_ENDL)
		{
			
			std::cout << std::endl;
			continue;

		}
		evals->inputDataStream(exp);//使用求值器进行求值，可能会抛出异常
		evals->printVARIANT(evals->result);
	}//依次输出
	
	
	
	//或者定向到其它的输出流中*/

}

void Interpreter::execScript(std::vector<std::vector<D_token >> scripts)
{

	
	match(scripts);
	for (this->pc = 0; this->pc < scripts.size(); )
	{
		try
		{
			execCode(scripts[pc]);
			if (this->pcIncreaseSign)
			{
				this->pc++;
			}
			else
			{
				this->pcIncreaseSign = true;//复位
			}
		}
		catch (std::string errorMsg)
		{
			Error e;
			e.errorMsg = errorMsg;
			e.pc = pc;
			throw(e);
		}
	}
}

void Interpreter::match(std::vector<std::vector<D_token >> scripts)//用于匹配成对出现的标识符
{
	std::stack<int> _for;
	std::stack<int> _while;
	D_token sign;
	std::stack<if_stack_token> _if_stack;
	if_stack_token data;
	if_match if_data;
	
	for (int i = 0; i < scripts.size(); i++)
	{
		if (scripts[i].size()>=1)
		{
			sign = scripts[i][0];
			switch (sign.type)
			{
			case D_EXPRESSION_TOKEN_FOR:
				_for.push(i);
				break;
			case D_EXPRESSION_TOKEN_ENDFOR:
				if (_for.size() == 0)
				{
					std::string errorMsg = "ENDFOR 没有匹配的FOR";
					throw(errorMsg);
				}
				else
				{
					for_map[_for.top()] = i;
					_for.pop();
				}
				break;
			case D_EXPERSSION_TOKEN_IF:
				
				data.pc = i;
				data.type = data.IF;
				_if_stack.push(data);
				break;
			case D_EXPRESSION_TOKEN_ELSE:
				if (_if_stack.empty()||_if_stack.top().type != _if_stack.top().IF)//栈为空
				{
					std::string errorMsg = "错误 ELSE 找不到匹配的 IF";
					throw(errorMsg);
				}
				
				data.pc = i;
				data.type = data.ELSE;
				_if_stack.push(data);
				break;
			case D_EXPRESSION_TOKEN_ENDIF:
				if (_if_stack.size() == 0)
				{
					std::string errorMsg = "错误 ENDIF 找不到匹配的IF";
					throw(errorMsg);
				}
				
				if_data.endif_state = i;
				switch (_if_stack.top().type)
				{
				case 1 ://ELSE
					if_data.else_state = _if_stack.top().pc;
					_if_stack.pop();
					if_data.if_state = _if_stack.top().pc;
					_if_stack.pop();
					this->if_vec.push_back(if_data);
					this->if_map[i] = if_data;

					break;
				case 0: //IF
					if_data.if_state = _if_stack.top().pc;
					_if_stack.pop();
					if_data.else_state = -1;
					this->if_vec.push_back(if_data);
					this->if_map[i] = if_data;
					break;

				default:
					break;

				}//判断是否有else
				break;
			case D_EXPRESSION_TOKEN_WHILE:
				_while.push(i);

				break;
			case D_EXPERSSION_TOKEN_ENDWHILE:
				if (_while.size() == 0)
				{
					std::string errorMsg = "ENDWHILE 没有匹配的WHILE";
					throw(errorMsg);
				}
				else
				{
					while_map[_while.top()] = i;
					_while.pop();
				}
				break;
			default:
				break;
			}
		}
	}
	if (!_if_stack.empty())
	{
		error("错误 IF 找不到对应的 ENDIF");
	}
	if (!_for.empty())
	{
		error("错误 FOR 找不到对应的 ENDFOR");
	}
	if (!_while.empty())
	{
		error("错误 WHILE 找不到对应的 ENDWHILE");
	}
}

void Interpreter::execFOR(std::vector<D_token > line)
{
	
	if (for_stack.empty() || for_stack.top().record.for_state != pc)//第一次进循环时
	{
		std::vector<D_token > var_state, target, step;
		bool To_sign = false;
		bool Step_sign = false;

		for (int i = 1; i < line.size(); i++)
		{
			D_token word = line[i];
			if (!To_sign)
			{
				if (word.type == D_EXPRESSION_TOKEN_TO)
				{
					To_sign = true;
					continue;
				}
				var_state.push_back(word);//在for后和to之前的为变量赋值表达式
				continue;
			}
			if (!Step_sign)
			{
				if (word.type == D_EXPRESSION_TOKEN_STEP)
				{
					Step_sign = true;
					continue;
				}
				target.push_back(word);//在to后和step前的是目标值表达式
				continue;
			}
			step.push_back(word);

		}
		if (step.empty())
		{
			D_token step_default;
			step_default.INT_val = 0;
			step_default.type = D_EXPERSSION_TOKEN_INT;
			step_default.abstractType = abstractType::Var_operand;
			step.push_back(step_default);

		}//step的默认值为0，即不对其进行操作而交由用户在循环体内改变值
		for_stack_token info;
		
		info.record.for_state = pc;
		info.record.endfor_state = for_map[pc];//记录该循环的起止
		
		evals->inputDataStream(step);

		if (evals->result.type == D_EXPERSSION_TOKEN_INT)
		{
			info.step = evals->result.int_val;
		}
		else if (evals->result.type == D_EXPRESSION_TOKEN_DOUBLE)
		{
			info.step = evals->result.double_val;
		}
		info.target = target;

		if (var_state[0].type == D_EXPRESSION_TOKEN_NAME)
		{
			info.var = var_state[0].STR_val;
		}

		execASIGN(var_state);//第一次进入循环时对变量进行初始化赋值;
		for_stack.push(info);

		evals->inputDataStream(for_stack.top().target);
		if (!evals->getLogicResult())//判断循环条件是否满足
		{
			pc = for_stack.top().record.endfor_state;//直接执行endfor的下一条语句
			for_stack.pop();//将当前已经结束的for循环从栈里弹出
			return;
		}
		else
		{
			return;
		}//由endfor来改变对应的值


	}//对循环的信息进行记录，并且压入栈中
	else
	{
		
		return;
	}//处理已经在栈中的循环
	//已经在栈中的循环必然是从endfor跳过来的，已经在endfor中处理变量值和判断，因此在这里无需处理


	

}

void Interpreter::execENDFOR(std::vector<D_token > line)
{
	//std::cout << "execendfor" << std::endl;
	if (line.size() != 1)
	{
		std::string errorMsg = "错误的 ENDFOR语句";
		throw(errorMsg);
	}
	
	switch (memory[for_stack.top().var].type)
	{
	case  D_EXPERSSION_TOKEN_INT:
		memory[for_stack.top().var].type = D_EXPRESSION_TOKEN_DOUBLE;
		memory[for_stack.top().var].double_val = memory[for_stack.top().var].int_val + for_stack.top().step;

		break;
	case D_EXPRESSION_TOKEN_DOUBLE:
		memory[for_stack.top().var].double_val += for_stack.top().step;

		break;

	default:
		std::string errorMsg = "FOR 循环中 错误的循环变量类型";
		throw(errorMsg);
		break;

	}//对循环变量自增其步长
	

	evals->inputDataStream(for_stack.top().target);
	//std::cout <<eval.result.int_val << eval.getLogicResult() << std::endl;
	if (!evals->getLogicResult())//不满足条件表达式时出循环
	{	

		for_stack.pop();//出循环
		return;
	}
	else
	{
		//std::cout << "重新循环" << std::endl;
		pc = for_stack.top().record.for_state;//重新循环
		return;
	}
	
}

void Interpreter::execIF(std::vector<D_token > line)
{
	std::vector<D_token > exp;
	for (int i = 1; i < line.size(); i++)
	{
		exp.push_back(line[i]);
	}
	evals->inputDataStream(exp);
	for (auto it : if_vec)
	{
		if (evals->getLogicResult())
		{
			if_map[pc].else_sign = false;
			it.else_sign = false;
			//不执行else
			return;
		}
		else
		{
			if_map[pc].else_sign = true;//执行else
			it.else_sign = true;


			if (it.if_state == pc)
			{
				if (it.else_state == -1)//不存在对应的else
				{
			
					pc = it.endif_state;
					return;
				}

				if_map[pc].else_sign = true;
				it.else_sign = true;
				pcIncreaseSign = false;//不进行自增
				pc = it.else_state;//按条件跳转到else
				return;
			}

		}
	}
}

void Interpreter::execELSE(std::vector<D_token > line)
{

	if (line.size() != 1)
	{
		std::string errorMsg = "错误的ELSE语句";
		throw(errorMsg);
	}
	for (auto it : if_vec)
	{
		if (it.else_state == pc);
		{
			if (if_map[it.if_state].else_sign)
			{
				//执行else
				return;
			}
			else
			{
				pc = it.endif_state;
			}
		}
	}

	return;
}

void Interpreter::execENDIF(std::vector<D_token > line)
{
	if (line.size() != 1)
	{
		std::string errorMsg = "错误的ENDIF语句";
		throw(errorMsg);
	}
	return;
}

void Interpreter::execWHILE(std::vector<D_token > line)
{
	if (while_stack.empty() || while_stack.top().record.while_state != pc)//第一次进入循环，记录while循环
	{
		while_stack_token data;
		for (int i = 1; i < line.size(); i++)
		{
			data.target.push_back(line[i]);//保存目标值表达式
		}
		data.record.while_state = pc;
		data.record.endwhile_state = while_map[pc];
		while_stack.push(data);//数据进栈
		evals->inputDataStream(data.target);
		if (evals->getLogicResult())
		{
			return;//符合循环条件
		}
		else
		{
			pc = data.record.endwhile_state;//执行endwhile的下一条语句
			return;
		}
	}
	else
	{
		evals->inputDataStream(while_stack.top().target);
		if (evals->getLogicResult())
		{
			return;
		}
		else
		{
			pc = while_stack.top().record.endwhile_state;
			return;
		}
			
	}
}
void Interpreter::execENDWHILE(std::vector<D_token > line)
{
	if (line.size() != 1)
	{
		std::string errorMsg = "错误的ENDWHILE语句";
		throw(errorMsg);
	}

	pc = while_stack.top().record.while_state;
	pcIncreaseSign = false;
	return;
}

std::vector<std::vector<double>> matrixInitial(std::vector<D_token > line)
{
	std::string errorMsg;
	if (line.size() < 3)
	{
		errorMsg = "错误 矩阵初始化中token过少";//最少的至少为[var]为1*1的矩阵
		throw(errorMsg);
	}
	if (line[0].type != D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS
		|| line[line.size() - 1].type != D_EXPERSSION_TOKEN_RIGHT_SQUARE_BRACKETS)//首末位不是中括号
	{
		errorMsg = "错误的矩阵初始化 检查是否缺少末尾反中括弧";//最少的至少为[var]为1*1的矩阵
		throw(errorMsg);
	}
	std::vector<std::vector<double>> iniData;
	std::vector<double> matrixLine;
	for (int i = 1; i < line.size() - 1; i++)//从初始化列表中抽取数据
	{
		switch (line[i].type)//扫描token流中的合法值，遇到非法值时发送异常消息
		{
		case D_EXPERSSION_TOKEN_INT:
			matrixLine.push_back((double)line[i].INT_val);
			break;
		case D_EXPRESSION_TOKEN_DOUBLE:
			matrixLine.push_back(line[i].DOUBLE_val);
			break;
		case D_EXPERSSION_TOKEN_SEMICOLON:
			iniData.push_back(matrixLine);
			matrixLine.clear();
			break;
		default:
			errorMsg = "错误 意外的矩阵初始化符号，请检查矩阵初始化语句";//最少的至少为[var]为1*1的矩阵
			throw(errorMsg);
			break;

		}
		
	}//endfor
	iniData.push_back(matrixLine);//循环结束后最后一次的line没有添加到iniData中
	matrixLine.clear();
	return iniData;
}

std::vector<D_token> Interpreter::boundFuctionCall(std::vector<D_token > tokenStream,int i)//函数调用形式 NAME -> NAME ( ... )
{
	std::vector<D_token> result;
	int begin = i;
	int end;
	int j = 0;
	for (j = 0; j < i; j++)
	{
		result.push_back(tokenStream[j]);
	}
	if (tokenStream.size() >= i + 4)
	{	
		if (tokenStream[i + 1].type == D_EXPRESSION_TOKEN_ARROW && tokenStream[i + 2].type == D_EXPRESSION_TOKEN_NAME)
		{

			if (tokenStream[i + 3].type == D_EXPRESSION_TOKEN_LEFT_PARENTHESIS)
			{
				i += 3;
				while (i < tokenStream.size())
				{
					if (tokenStream[i].type == D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS)
					{
						break;
					}
					if (tokenStream[i].type == D_EXPRESSION_TOKEN_NAME)
					{
						tokenStream = boundFuctionCall(tokenStream, i);//函数的嵌套调用
					}
					i++;

				}
				if (tokenStream[i].type == D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS)
				{
					end = i;
					D_token func;
					func.type = D_EXPRESSION_TOKEN_FUNCCALL;
					func.abstractType = abstractType::Var_operand;
					func.func_call_id = this->func_call_vec.size();//顺序添加

					//存在函数调用，不对参数列表进行判断
					result.push_back(func);
					for (int z = end + 1; z < tokenStream.size(); z++)
					{
						result.push_back(tokenStream[z]);
					}


					std::vector<D_token > fuc_call;
					for (int i = begin; i <= end; i++)
					{
						fuc_call.push_back(tokenStream[i]);

					}
					func_call_token token;
					token.ID = func_call_vec.size();
					token.function_call = fuc_call;
					func_call_vec.push_back(token);
					//将函数调用替换并添加到vec中


					return result;
				}


			}
		}
		//不存在函数调用
		for (j = i; j < tokenStream.size(); j++)
		{
			result.push_back(tokenStream[j]);
		}
		return result;
	}
	else//不存在函数调用
	{
		for (j = i; j < tokenStream.size(); j++)
		{
			result.push_back(tokenStream[j]);
		}
	}
	return result;
}

std::vector<D_token> Interpreter::boundMatrixVisit(std::vector<D_token > tokenStream, int i)//NAME [ exp1 ] [ exp2 ]
{
	
	std::vector<D_token> result;
	int begin = i;
	int end;
	int j = 0;
	matrix_visit_token matrix_visit_info;
	for (j = 0; j < i; j++)
	{
		result.push_back(tokenStream[j]);
	}
	if (tokenStream.size() >= i + 6)//可能存在矩阵的访问 最少token  NAME [ VAR ] [ VAR ]
	{
		
		if (tokenStream[i + 1].type == D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS) // [
		{
	
;			i += 1;
			while (i < tokenStream.size())
			{
				if (tokenStream[i].type == D_EXPERSSION_TOKEN_RIGHT_SQUARE_BRACKETS)
				{
					break;
				}
				if (tokenStream[i].type == D_EXPRESSION_TOKEN_NAME)
				{
					
					tokenStream = boundMatrixVisit(tokenStream, i);//矩阵值访问的嵌套调用
		
				}
				i++;
				
			}
			
			if (i < tokenStream.size()-1)
			{
				i += 1;
				while (i < tokenStream.size())
				{
					if (tokenStream[i].type == D_EXPERSSION_TOKEN_RIGHT_SQUARE_BRACKETS)
					{
						break;
					}
					if (tokenStream[i].type == D_EXPRESSION_TOKEN_NAME)
					{
						tokenStream = boundMatrixVisit(tokenStream, i);//矩阵值访问的嵌套调用
					}
					i++;

				}
				if (i < tokenStream.size())//成功匹配 NAME [ exp1 ] [ exp2 ]
				{
				
					end = i;
					D_token matrix_visit;
					matrix_visit.abstractType = abstractType::Var_operand;//值
					matrix_visit.matrix_visit_id = martrix_visit_vec.size();
					matrix_visit.type = D_EXPRESSION_TOKEN_MATRIX_VISIT;
					result.push_back(matrix_visit);
					for (int z = end + 1; z < tokenStream.size(); z++)
					{
						result.push_back(tokenStream[z]);
					}

					matrix_visit_token token;
					for (int z = begin; z <= end; z++)
					{
						token.matrix_visit.push_back(tokenStream[z]);
					}
					token.matrixName = tokenStream[begin].STR_val;
					token.ID = martrix_visit_vec.size();
					//token.matrix = &(memory[token.matrixName].matrix_val);//需要在计算前进行设置并且判断当前的变量是否为矩阵
					//使用token进行计算
					//未完成标记  调用token的函数对表达式进行拆分和解析
					token.separateExp();
					token.setEval(this->evals);
					martrix_visit_vec.push_back(token);
				
					return result;

				}



			}
			else//  name [ exp1 (NULL)     or     name [ exp1 ] (NULL)  
			{
				
				error("错误的矩阵值访问");
			}
		}


	}
	//不存在矩阵访问
	
		
for (j = begin; j < tokenStream.size(); j++)
{
	result.push_back(tokenStream[j]);
}
	
return result;

}

std::vector<D_token> Interpreter::boundProperty(std::vector<D_token > tokenStream)//NAME.NAME
{
	int size = tokenStream.size();
	D_token propertyToken;
	propertyToken.type = D_EXPRESSION_TOKEN_PROERTY;
	propertyToken.abstractType = abstractType::Var_operand;
	std::vector<D_token> result;
	int i = 0;
	for (i = 0; i < size - 2; i++)
	{
		bool sign = false;
		D_token  tk1, tk2, tk3;
		tk1 = tokenStream[i];
		tk2 = tokenStream[i + 1];
		tk3 = tokenStream[i + 2];
		if (tk1.type == D_EXPRESSION_TOKEN_NAME && tk2.type == D_EXPRESSION_TOKEN_DOT && tk3.type == D_EXPRESSION_TOKEN_NAME)
		{
			//是属性访问的标记
			for (int i1 = 0; i1 < reserved_property.size(); i1++)
			{
				if (tk3.STR_val == reserved_property[i1])
				{
					//是保留属性 优先匹配保留属性
					propertyToken.reserved_propertyType = i1;
					
					propertyToken.STR_val = tk1.STR_val;
					result.push_back(propertyToken);
					i += 3;
					sign = true;
					break;
				}
			}
			if (!sign)
				for (int j = 0; j < bound_property.size(); j++)//处理绑定属性
				{

				}

			if (!sign)//两种都匹配不成功
			{
				error("错误 无法匹配保留属性 请检查属性是否绑定");
			}


		}
		if (i < size)
		{
			result.push_back(tokenStream[i]);
		}
	}
	if (i < size)//返回末尾的token流
	{
		for (; i < size; i++)
		{
			result.push_back(tokenStream[i]);
		}
	}
	return result;
}
