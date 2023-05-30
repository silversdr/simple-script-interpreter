#ifndef DLEXER_H
#define DLEXER_H

#include<string>
#include<vector>
#include<iostream>
#include<sstream>
#include<map>
#include<fstream>
#include<math.h>
#include"Constant.h"

class DLexer
{
public:
	DLexer(std::string data);
	DLexer();
	void input(std::string data);
	void inputFile(std::string filePath);
	std::vector<std::string> getFileContent(std::string filePath);
	std::vector<D_token> getTokens(void);
	std::vector<std::vector<D_token>> getTokenStream(std::string filePath);

	

private:
	bool isLetter(char ch);
	bool isNumber(char ch);
	void boundFloat(void);
	void boundNegative(void);

	std::vector<D_token> tokenStream;
	std::map<std::string, std::vector<std::vector<D_token>>> filesContent;
	std::map<std::string, std::vector<std::string>> filesInfo;
	std::vector<std::string> reserved_keywords = {

	"IF",
	"ENDIF",
	"ELSE",
	
	
	"WHILE",
	"ENDWHILE",
	
	"FOR",
	"TO",
	"STEP",
	"ENDFOR",

	"ASIGN",//赋值语句

	"PRINT",
	"LOAD",
	"SAVE",

	"ENDL",

	"FUNCTION",
	"ENDFUNC",
	"RETURN",

	"SCOPE",
	"ENDSCOPE",
	
	"NULL",


	};

};//注意和枚举量的顺序要相同



#endif // !DLEXER_H