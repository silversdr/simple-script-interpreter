#include"Constant.h"

void test_printDtoken(std::vector<D_token> tokenStream, std::string id)
{
	std::cout << "--------------" << std::endl;
	std::cout << "size: " << tokenStream.size() <<" id:"<<id << std::endl;
	for (int i = 0; i < tokenStream.size(); i++)
		std::cout << tokenStream[i].STR_val << "  " << tokenStream[i].abstractType << "  "
		<< tokenStream[i].type << "  " << tokenStream[i].errorType << std::endl;
	std::cout << "--------------" << std::endl;
	//std::cout << std::endl;

}
void error(std::string errorMsg)
{
	throw(errorMsg);
}

void test_printToken(std::vector<token> tokens, std::string id)
{
	std::cout << "test_PrintToken id:" << id << std::endl;
	std::cout << "tokenSize: " << tokens.size() << std::endl;
	for (int i = 0; i < tokens.size(); i++)
	{
		token it = tokens[i];

		std::cout << "  " << it.type << "  ";


		switch (it.type)
		{
		case it.OP:

			std::cout << "operator : ";
			std::cout << it.op.type << " " << it.op.numbers << " " << it.op.ass << " " << it.op.icp << " " << it.op.isp;
			std::cout << std::endl;
			break;
		case it.VAR:
			std::cout << "Variant : ";
			std::cout << it.var.type << " " << "var: ";
			switch (it.var.type)
			{
			case D_EXPERSSION_TOKEN_INT:
				std::cout << it.var.int_val << std::endl;
				break;
			case D_EXPRESSION_TOKEN_DOUBLE:
				std::cout << it.var.double_val << std::endl;
				break;
			case D_EXPRESSION_TOKEN_STRING:
				std::cout << it.var.string_val << std::endl;
				break;



			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void test_printOpStack(std::stack<OPERATOR> stack, std::string id)
{
	std::cout << "test_PrintStack id: " << id << std::endl;
	while (stack.size() > 1)
	{
		std::cout << stack.top().type << std::endl;
		stack.pop();
	}
}

void test_printVARIANT(VARIANT var, std::string id)
{
	std::cout << "test_printVARIANT id:" << id << std::endl;
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
	default:
		std::string errorMsg = "print未知类型的值";
		throw(errorMsg);


	}
}

void matrix_visit_token::separateExp()
{
	D_token token;
	
	int i = 1;
	if (this->matrix_visit.empty())
	{
		return;
	}
	
	if (matrix_visit[i].type == D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS)
	{
		i++;
	}
	for (; i < matrix_visit.size(); i++)
	{
		if (matrix_visit[i].type == D_EXPERSSION_TOKEN_RIGHT_SQUARE_BRACKETS)
		{
			i++;
			break;
		}
		if(matrix_visit[i].type!=D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS)
		this->row_exp.push_back(matrix_visit[i]);
	}
	if (matrix_visit[i].type == D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS)
	{
		for (; i < matrix_visit.size(); i++)
		{
			if (matrix_visit[i].type == D_EXPERSSION_TOKEN_RIGHT_SQUARE_BRACKETS)
			{
				break;
			}
			if (matrix_visit[i].type != D_EXPRESSION_TOKEN_LEFT_SQUARE_BRACKETS)
				this->col_exp.push_back(matrix_visit[i]);
		}
	}

	
}

void matrix_visit_token::setEval(Evaluation* e)
{
	this->eval = e;
}

void test_print(std::string s)
{
	std::cout << s << std::endl;
}

std::vector<std::string> loadFile_cons(std::string filePath)
{
	std::vector<std::string> fileContent;
	std::ifstream in(filePath);
	if (!in)
	{
		std::cout << __DATE__ << " " << __TIME__
			<< __FILE__ << " " << __LINE__
			<< ": dict read error" << std::endl;
		error("无法打开文件 " + filePath);
	}
	
	std::string line;
	while (getline(in, line))
	{
		fileContent.push_back(line);
	}
	in.close();
	return fileContent;
}