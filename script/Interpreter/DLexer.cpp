#include "DLexer.h"
DLexer::DLexer()
{

}
DLexer::DLexer(std::string data)
{
	input(data);
}

std::vector<std::vector<D_token>>  DLexer::getTokenStream(std::string filePath)
{
	if(filesContent.count(filePath)!=0)
	return this->filesContent[filePath];
	else
	{
		error("尝试打开未输入的 文件");
	}
}

std::vector<std::string>  DLexer::getFileContent(std::string filePath)
{
	if (filesContent.count(filePath) == 0)
	{
		error("尝试获取未输入的文件");
	}
	return filesInfo[filePath];
}


void DLexer::inputFile(std::string filePath)
{
	std::vector<std::string> fileContent;
	std::ifstream in(filePath);
	if (!in)
	{
		std::cout << __DATE__ << " " << __TIME__
			<< __FILE__ << " " << __LINE__
			<< ": dict read error" << std::endl;
		exit(-1);
	}
	
	std::string line;
	while (getline(in, line))
	{
		fileContent.push_back(line);
	}
	filesInfo[filePath] = fileContent;
	for (auto line : fileContent)
	{
		this->input(line);
		this->filesContent[filePath].push_back(tokenStream);
	}


	in.close();
}


void DLexer::input(std::string data)
{
	tokenStream.clear();
	int n = 0;
	char it;
	D_token token;
	bool NAMESIGN = false;
	for (n = 0; n < data.length();)
	{
		it = data[n++];

		while (it == ' ')
		{
			it = data[n];
			n++;
		}
		if (isLetter(it))
		{
			/*标识符与关键字的识别*/
			while (isLetter(it) || isNumber(it))
			{
				token.STR_val.push_back(it);
				it = data[n++];
			}
			n--;
			for (int count = 0; count < reserved_keywords.size(); count++)
			{
				if (token.STR_val == reserved_keywords[count])
				{
					//保留关键字
					token.type = count + 1;
					break;
				}
			}
			if (!token.type)
			{
				token.type = D_EXPRESSION_TOKEN_NAME;
				token.abstractType = abstractType::Var_operand;
			}
			tokenStream.push_back(token);
			token.clear();

			continue;

		}
		if (it == '"')
		{
			/*识别字符串*/
			if (data.size() >= 2)
			{
				/*字符串*/
				it = data[n];
				token.STR_val.push_back(it);
				while (it != '"' && n <= data.length())
				{
					it = data[++n];
					if(it != '"')
					token.STR_val.push_back(it);
					

				}
				if (it == '"')
				{
					token.type = D_EXPRESSION_TOKEN_STRING;
					token.abstractType = abstractType::Var_operand;

				}
				else if (n > data.length())
				{
					token.errorType = D_ERRORTYPE_STRING_MISSING;
				}
				n++;
				
				tokenStream.push_back(token);
				token.clear();
			}
			else
			{
				token.errorType = D_ERRORTYPE_STRING_MISSING;
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		if (isNumber(it))
		{
			/*正整数*/
			int sum = 0;
			while (isNumber(it))
			{
				token.STR_val.push_back(it);
				sum = sum * 10 + it - '0';
				it = data[n++];
			}
			n--;

			token.type = D_EXPERSSION_TOKEN_INT;
			token.abstractType = abstractType::Var_operand;
			token.INT_val = sum;
			tokenStream.push_back(token);
			token.clear();


			continue;
		}




		/*运算符和界符*/

		if (it == '+')
		{
			token.type = D_EXPRESSION_TOKEN_PLUS;
			token.abstractType = abstractType::Var_operator;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '-' && n < data.size())
		{
			if (data[n] == '>')
			{
				token.type = D_EXPRESSION_TOKEN_ARROW;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				it = data[n++];
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}
			else
			{
				token.type = D_EXPRESSION_TOKEN_MINUS;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		else if (it == '-')
		{
			token.type = D_EXPRESSION_TOKEN_MINUS;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '%')
		{
			token.type = D_EXPRESSION_TOKEN_MOD;
			token.abstractType = abstractType::Var_operator;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '*' && n < data.size())
		{
			if (data[n] == '*')
			{
				token.type = D_EXPRESSION_TOKEN_POW;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				it = data[n++];
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}
			else
			{
				token.type = D_EXPERSSION_TOKEN_MULTI;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		else if (it == '*')
		{
			token.type = D_EXPERSSION_TOKEN_MULTI;
			token.abstractType = abstractType::Var_operator;
			token.errorType = D_ERRORTYPE_OP_MISSING;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '/' && n < data.size())
		{
			if (data[n] == '/')
			{
				while (data[n++] != '\n' && n < data.size());
			}
			else
			{
				token.type = D_EXPRESSION_TOKEN_DIV;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		else if (it == '/')
		{
			token.type = D_EXPRESSION_TOKEN_DIV;
			token.abstractType = abstractType::Var_operator;
			token.errorType = D_ERRORTYPE_OP_MISSING;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}

		if (it == '.')
		{
			if (n < data.size())
			{
				it = data[n++];
				if (it == '+')
				{
					token.type = D_EXPERSSION_TOKEN_POINT_PLUS;
					token.abstractType = abstractType::Var_operator;
					token.STR_val = ".+";
					tokenStream.push_back(token);
					token.clear();
					continue;
				}
				if (it == '-')
				{
					token.type = D_EXPRESSION_TOKEN_POINT_MINUS;
					token.abstractType = abstractType::Var_operator;
					token.STR_val = ".-";
					tokenStream.push_back(token);
					token.clear();
					continue;
				}
				n--;
			}
			token.type = D_EXPRESSION_TOKEN_DOT;
			token.STR_val = ".";
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '=' && n < data.size())
		{
			if (data[n] == '=')
			{
				token.type = D_EXPRESSION_TOKEN_EQUAL;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				it = data[n++];
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}
			else
			{
				token.type = D_EXPRESSION_TOKEN_EQUALFOR;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		else if (it == '=')
		{
			token.type = D_EXPRESSION_TOKEN_EQUALFOR;
			token.errorType = D_ERRORTYPE_OP_MISSING;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}

		if (it == '>' && n < data.size())
		{
			if (data[n] == '=')
			{
				token.type = D_EXPRESSION_TOKEN_GE;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				it = data[n++];
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}
			else
			{
				token.type = D_EXPRESSION_TOKEN_GT;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		else if (it == '>')
		{
			token.type = D_EXPRESSION_TOKEN_GT;
			token.abstractType = abstractType::Var_operator;
			token.errorType = D_ERRORTYPE_OP_MISSING;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}

		if (it == '<' && n < data.size())
		{
			if (data[n] == '=')
			{
				token.type = D_EXPRESSION_TOKEN_LE;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				it = data[n++];
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}
			else
			{
				token.type = D_EXPRESSION_TOKEN_LT;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		else if (it == '<')
		{
			token.type = D_EXPRESSION_TOKEN_LT;
			token.abstractType = abstractType::Var_operator;
			token.errorType = D_ERRORTYPE_OP_MISSING;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}




		if (it == '|')
		{
			token.type = D_EXPRESSION_TOKEN_OR;
			token.abstractType = abstractType::Var_operator;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '&')
		{
			token.type = D_EXPERSSION_TOKEN_AND;
			token.abstractType = abstractType::Var_operator;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '!' && n < data.size())
		{
			if (data[n] == '=')
			{
				token.type = D_EXPRESSION_TOKEN_NE;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				it = data[n++];
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}
			else
			{
				token.type = D_EXPRESSION_TOKEN_NOT;
				token.abstractType = abstractType::Var_operator;
				token.STR_val.push_back(it);
				tokenStream.push_back(token);
				token.clear();
			}

			continue;
		}
		else if (it == '!')
		{
			token.type = D_EXPRESSION_TOKEN_NOT;
			token.abstractType = abstractType::Var_operator;
			token.errorType = D_ERRORTYPE_OP_MISSING;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '(')
		{
			token.type = D_EXPRESSION_TOKEN_LEFT_PARENTHESIS;
			token.abstractType = abstractType::Var_operator;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == ')')
		{
			token.type = D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS;
			token.abstractType = abstractType::Var_operator;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == '[')
		{
			token.type = D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == ']')
		{
			token.type = D_EXPERSSION_TOKEN_RIGHT_SQUARE_BRACKETS;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == ',')
		{
			token.type = D_EXPRESSION_TOKEN_COMMA;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}
		if (it == ';')
		{
			token.type = D_EXPERSSION_TOKEN_SEMICOLON;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
			token.clear();

			continue;
		}


		
		if (it != ' '&&it!='\n'&&it!='\r'&&it!='\0')//忽略空字符
		{
		
			token.type = D_EXPRESSION_TOKEN_UNKNOWN;
			token.STR_val.push_back(it);
			tokenStream.push_back(token);
		}
		token.clear();



	}
	
	boundFloat();
	boundNegative();

	
}

bool DLexer::isLetter(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;

	return false;
}
bool DLexer::isNumber(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;

	return false;
}

void D_token::clear()
{
	this->DOUBLE_val = 0;
	this->INT_val = 0;
	this->STR_val = "";
	this->type = D_EXPRESSION_TOKEN_UNKNOWN;
}

void DLexer::boundFloat(void)
{
	std::vector<D_token> newTokenStream;
	int i = 0;
	int size = this->tokenStream.size();
	for (i = 0; i < size-2; i++)
	{
		D_token token = tokenStream[i];
		if (tokenStream[i].type == D_EXPERSSION_TOKEN_INT)
		{
			if (tokenStream[i + 1].type == D_EXPRESSION_TOKEN_DOT)
			{
				if (tokenStream[i + 2].type == D_EXPERSSION_TOKEN_INT)
				{
					token.abstractType = abstractType::Var_operand;
					token.type = D_EXPRESSION_TOKEN_DOUBLE;
					token.STR_val = tokenStream[i].STR_val + tokenStream[i + 1].STR_val + tokenStream[i + 2].STR_val;
					token.DOUBLE_val = (double)tokenStream[i].INT_val + 
									   (double)tokenStream[i + 2].INT_val / (pow(10, tokenStream[i + 2].STR_val.size()));
					newTokenStream.push_back(token);
					token.clear();
					i += 2;
					continue;
				}
			}

		}
		newTokenStream.push_back(token);
		token.clear();

	}
	for (; i < this->tokenStream.size(); i++)
	{
		D_token token = tokenStream[i];
		newTokenStream.push_back(token);
	}
	tokenStream = newTokenStream;
}

void DLexer::boundNegative(void)
{
	//负号出现的情况
	/*
	连续两个操作符出现（对于第一个操作符，右括号除外，因为括号内部可能是表达式）
	且第二个操作符后方出现 常量 类型
	则第二个操作符可以结合为负号
	否则按减号看待
	*/
	std::vector<D_token> newTokenStream;
	int i = 0;
	int size = this->tokenStream.size();
	


		for (i = 0; i < size - 2; i++)
		{
			D_token token = tokenStream[i];
			if (tokenStream[i].abstractType == abstractType::Var_operator &&
				(tokenStream[i].type != D_EXPERSSION_TOKEN_RIGHT_PARENTHESIS))
			{
				//std::cout << "1" << std::endl;
				if (tokenStream[i + 1].type == D_EXPRESSION_TOKEN_MINUS)
				{
					if (tokenStream[i + 2].abstractType == abstractType::Var_operand &&
						tokenStream[i + 2].type != D_EXPRESSION_TOKEN_NAME)
					{
						token.abstractType = abstractType::Var_operand;
						token.type = tokenStream[i + 2].type;
						token.STR_val = tokenStream[i + 1].STR_val + tokenStream[i + 2].STR_val;
						switch (token.type)
						{
						case D_EXPERSSION_TOKEN_INT:
							token.INT_val = tokenStream[i + 2].INT_val;
							token.INT_val *= -1;

							break;
						case D_EXPRESSION_TOKEN_DOUBLE:
							token.DOUBLE_val = tokenStream[i + 2].DOUBLE_val;
							token.DOUBLE_val *= -1;
							break;
						default:
							break;

						}
						newTokenStream.push_back(tokenStream[i]);
						newTokenStream.push_back(token);
						token.clear();
						i += 2;
						continue;
					}
				}

			}
			newTokenStream.push_back(token);
			token.clear();

		}
		for (; i < this->tokenStream.size(); i++)
		{
			D_token token = tokenStream[i];
			newTokenStream.push_back(token);
		}

	

	tokenStream = newTokenStream;
}


std::vector<D_token> DLexer::getTokens(void)
{
	return tokenStream;
}