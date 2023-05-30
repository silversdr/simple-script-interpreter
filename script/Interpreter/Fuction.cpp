#include "Fuction.h"
void Function::loadFile(std::string filePath)
{
	std::vector<std::string> fileContent;
    fileContent = loadFile_cons(filePath);

    filesContent[filePath] = fileContent;
    scanFile(filePath);
  
}

std::vector<std::string> Function::getFiles()
{
    return this->files;
}

void Function::loadFile(std::vector<std::string > files)
{
    for (auto it : files)
    {
        loadFile(it);
    }
}

void Function::unloadFile(std::string  filePath)
{
    //删除作用域和传引用作用域，删除文件内容和token内容
    if (filesContent.count(filePath) == 0)
    {
        error("尝试卸载不存在的文件");
        return;
    }
    else
    {
        std::vector<std::string> unloadFileScopes = filesScope[filePath];
       // std::vector<std::string> newScopes;
      
        for (auto scope : unloadFileScopes)
        {
            scopes.erase(scope);
        }
        filesDtoken.erase(filePath);
        filesContent.erase(filePath);
        filesScope.erase(filePath);
        
    }
}

void Function::unloadFile(std::vector<std::string> files)
{
    for (auto file : files)
    {
        unloadFile(file);
    }
}

void Function::setParentMemory(std::map<std::string, VARIANT>* parentMemory)
{
    this->parentMemory = parentMemory;
}

void Function::inPutFuncCallVec(std::vector<func_call_token> func_vec)
{
    this->func_call_vec = func_vec;
}

void Function::scanFile(std::string filePath)
{
    if (filesContent.count(filePath) == 0)
    {
        std::string errorMsg = "错误 尝试扫描未加载的文件";
        throw(errorMsg);
    }
    std::vector<std::vector<D_token>> tokenScript;
    std::vector<D_token> tokens;
    DLexer lexer;
    for (int i = 0; i < filesContent[filePath].size(); i++)
    {
        lexer.input(filesContent[filePath][i]);
        tokens = lexer.getTokens();
        tokenScript.push_back(tokens);
    }
    filesDtoken[filePath] = tokenScript;

    regScope(filePath);
    
    
}

std::vector<func_call_token> Function::getFunVec()
{
    return this->func_call_vec;
}

fuction_info Function::regFuction(std::string filePath, std::string scope, int *begin)
{
    fuction_info fuc;
    fuc.begin = *begin;
    int* pc;
    pc = begin;
    bool inFunc = false;
    
    std::vector<std::vector<D_token>> script;
    script = filesDtoken[filePath];
    //fuction name a b ...
    if (script[*begin].size() < 2)
    {
        error("函数声明参数过少");
    }
    else
    {
        if (script[*begin][1].type == D_EXPRESSION_TOKEN_NAME)
        {
            fuc.fucName = script[*begin][1].STR_val;
        }
        else
        {
            error("非法函数名");
        }
        //读取参数个数
        fuc.argc = -1;
        for (int i = 1; i < script[*begin].size(); i++)
        {
            if (script[*begin][i].type == D_EXPRESSION_TOKEN_NAME)
            {
                fuc.argc++;
            }
            else
            {
                std::cout << script[*begin].size()<< std::endl;
                error(scope + "-> " +fuc.fucName + " 注册错误  非法形参列表");
            }//所有的形式参数都是name
            inFunc = true;

        }
        (*pc)++;//跳过函数声明语句
        for (; *pc < script.size();)
        {
            if (script[*pc].size() > 0)
            {
                if (script[*pc][0].type == D_EXPRESSION_TOKEN_ENDFUNC)
                {
                    if (script[*pc].size() != 3)
                    {
                        error("函数返回参数错误");//不允许返回表达式
                    }
                    else
                    {
                        if (script[*pc][1].type == D_EXPRESSION_TOKEN_RETURN)
                        {
                            if (script[*pc][2].type == D_EXPRESSION_TOKEN_NAME || script[*pc][2].type == D_EXPRESSION_TOKEN_NULL)
                            {
                                //成功返回
                                fuc.end = *pc;
                                (*pc)++;
                                
                                fuc.filePath = filePath;
                                return fuc;
                            }
                            error("非法的返回值");
                        }
                    }
                }
                if(inFunc)
                if (script[*pc][0].type == D_EXPRESSION_TOKEN_FUNCTION ||
                    script[*pc][0].type == D_EXPRESSION_TOKEN_ENDSCOPE)
                {
                    error( scope +"."+ fuc.fucName + "错误 找不到ENDFUNC");
                }
            }
            (*pc)++;
        }
        error("错误 找不到ENDFUNC");
    }
    
}

void Function::regScope(std::string filePath)//注册作用域
{
   
    if (filesDtoken.count(filePath)==0)
    {
        std::string errorMsg = "错误 尝试注册未加载的文件";
        throw(errorMsg);
    }
    std::vector<std::vector<D_token>> script;
    script = filesDtoken[filePath];
    std::string scopeName = "";
    int pc = 0;
    bool inScope = false;
  
    for (; pc < script.size(); )
    {
        if (!inScope)//在作用域外
        {
           
            if (script[pc].size() > 0)
            {
     
  
                if (script[pc][0].type == D_EXPRESSION_TOKEN_SCOPE)
                {

                    if (script[pc].size() == 2)
                    {
                        if(script[pc][1].type==D_EXPRESSION_TOKEN_NAME)
                        scopeName = script[pc][1].STR_val;
                        else
                        {
                            error("非法的作用域" + script[pc][1].STR_val + "请检查作用域名");
                        }

                        inScope = true;
                        continue;
                    }
                    else if (script[pc].size() == 3)
                    {
                        if (script[pc][1].type == D_EXPRESSION_TOKEN_NAME && script[pc][2].type == D_EXPERSSION_TOKEN_MULTI)
                        {
                            scopeName = script[pc][1].STR_val;
                            inScope = true;
                            referenceSnedScope.push_back(scopeName);//传引用作用域添加
                            continue;//?
                        }
                        else
                        {
                           
                            error("非法的作用域" + script[pc][1].STR_val + "请检查作用域名");
                        }
                        
                       // test_print(scopeName);
                    }
                    else if (script[pc].size() > 3)
                    {
                        error("错误 作用域声明时 参数过多");
                    }
                    else
                    {
                        error("错误 找不到作用域名");
                    }
                }
            }
        }
        if (inScope)
        {
          
            std::vector<fuction_info> scopeFuncVec;
            for (; pc < script.size(); )
            {
                if (script[pc].size() > 0)
                {

                    if (script[pc][0].type == D_EXPRESSION_TOKEN_FUNCTION)
                    {

                        fuction_info info = regFuction(filePath, scopeName, &pc);
                        for (auto it : scopeFuncVec)
                        {
                            if (it.fucName == info.fucName)
                            {
                                error("作用域 " + scopeName + " 函数 " + info.fucName + "重复定义");
                            }
                        }
                        scopeFuncVec.push_back(info);//可能抛出异常

                        continue;
                    }
                   
                    
                    if (script[pc][0].type == D_EXPRESSION_TOKEN_ENDSCOPE)
                    {
                        if (script[pc].size() == 1)
                        {
                         
                            for (int i = 0; i < scopeFuncVec.size(); i++)
                            {
                                if (scopes.count(scopeName) != 0)
                                {
                                    error("作用域 " + scopeName + "重复定义");
                                }
                                scopes[scopeName] = scopeFuncVec;//注册函数
                                filesScope[filePath].push_back(scopeName);//将文件所属的作用域添加到文件名下
                            
                            
                            }
                         
                                inScope = false;
                                break;//跳出作用域内循环

                        }
                        else
                        {
                            error("错误的作用域末尾");
                        }
                    }
                
                
                }
               
                pc++;
           
                int x = 0;
            }//作用域内循环
        }
        pc++;
    }

}

VARIANT Function::execFunction(D_token funcSign)
{
    
    if (funcSign.func_call_id == -1)//不存在函数调用
    {
        //test_print("s");
        error("错误的函数调用解析");
    }
    fuction_info function;
    std::string scopeName;
    func_call_token token = func_call_vec[funcSign.func_call_id];//保存了函数调用语句的token流
    scopeName = token.function_call[0].STR_val;
    function.fucName = token.function_call[2].STR_val;
    bool exist=false;
    for (auto it : scopes[scopeName])
    {
        //在注册的作用域内查找函数
        if (it.fucName == function.fucName)
        {
            exist = true;
            function = it;//从已注册的函数表中获取函数详细信息
        }
        for (auto scope : referenceSnedScope)
        {
            //test_print(scope);
            if (scope == scopeName)
            {
               // test_print("s");
                function.reference = true;//为传递引用函数
            }
        }
    }

    if (!exist)
    {
        error("尝试调用不存在的函数，请检查函数文件是否注册");
    }


    std::vector<std::vector<D_token>> functionBody;

    for (int i = function.begin; i <= function.end; i++)
    {
        functionBody.push_back(filesDtoken[function.filePath][i]);
    }

    //进入函数的时候用vector接收形参列表中的变量，将其替换为VARIANT 的vector后传入函数中,函数调用形式为scope->funcName(exp1,wxp2,...)
    //若函数注册为地址传递函数，则在进入函数前额外记录形参列表的名字压入向量，在函数返回前返回一个等长的vector为形参列表经过函数后的值，并在返回后
    //在内存中依次赋值
    std::vector<std::vector<D_token>> argList;
    

    int size = token.function_call.size();
    //scope -> funcname ( exp1 , exp2 , ...    )
    std::vector<D_token> line;
    int commaCounter =0;
        for (int i = 4; i < size - 1; i++)
        {
            if (token.function_call[i].type != D_EXPRESSION_TOKEN_COMMA)//参数表以逗号进行分隔
            {
                line.push_back(token.function_call[i]);//
            }
            else
            {
                commaCounter++;
                argList.push_back(line);
                line.clear();
            
            }
        }
        if(!line.empty())//防止参数数量为0时传入空vector导致形参实参数量不匹配
        argList.push_back(line);//输入形参列表，不对形参进行检查


        
        if (commaCounter != function.argc - 1)
        {
            error("函数形参与实参数量不匹配1");
        }
        

        for (auto it : argList)
        {
            if (it.empty())
            {
                //出现形式参数为空
                error("错误 形式参数表存在空值");
            }
        }
 
    
   
        VARIANT returnValue;
        returnValue = calcuFunction(argList, function, functionBody, parentMemory);
        return returnValue;
}

VARIANT Function::calcuFunction(std::vector<std::vector<D_token>> argList, fuction_info function, std::vector < std::vector<D_token>> functionBody,
    std::map<std::string, VARIANT>* parentMemory)
{
    int pc = 0;//函数体程序计数器
    int endfunc = functionBody.size() - 1;
    std::map<std::string, VARIANT> memory;//管理的内存
    std::vector<D_token> actualParameterNameList;//实际参数名列表，用于传引用函数的值的返回
    //对函数进行求值
    //参数   实参列表 函数信息 函数体 父内存空间
    //1.将实参列表求值并赋给子内存空间中的形式参数值
    //2.构造解释器对函数体进行解析
    //3.处理返回值
    if (argList.size() != function.argc)
    {
        //std::cout << argList.size() << function.argc << function.fucName;
        //参数数量不匹配;
        error("函数形参与实参数量不匹配");
    }
    if(function.reference)//是传引用函数，检查实参列表是否都为可操作变量
    {
        for (auto line : argList)
        {
            if (line.size() != 1)
            {
                error("错误 传引用函数不允许出现表达式类型参数");
            }
            if (line[0].type != D_EXPRESSION_TOKEN_NAME)
            {
                error("错误 传引用函数不支持常量类型参数");
            }
            actualParameterNameList.push_back(line[0]);//存储实际参数名
        }
    }
    //处理函数头，取出形式参数名
    std::vector<D_token> formalParameterList;
    for (int i = 0; i < function.argc; i++)
    {
        formalParameterList.push_back(functionBody[0][i + 2]);
    }//取出形式参数名
    //使用InterPreter时需要把函数末尾传入进行解析
    Interpreter funcInterpreter;//创建函数的变量作用域和解析器
    Evaluation tmpevals;//创建临时求值器
    for (int i = 0; i < this->files.size(); i++)
    {
        funcInterpreter.loadFunctionFile(files[i]);//为函数解析器导入函数文件
        tmpevals.functionEval->loadFile(files[i]);//为临时求值器的函数求值器部分导入函数文件
    }//完成函数的导入
    D_token equal;
    equal.abstractType = abstractType::Var_operator;
    equal.type = D_EXPRESSION_TOKEN_EQUALFOR;
    

    tmpevals.functionEval->inPutFuncCallVec(this->func_call_vec);//为函数求值器设置扫描得到的函数标记
    tmpevals.setMemory(parentMemory);//为求值器设置内存
    tmpevals.setMatrixVisitVec(this->matrix_visit_vec);//为临时求值器设置矩阵访问标志

    
    std::vector<VARIANT> parameterListValue;//
    try {
        for (int i = 0; i < function.argc; i++)
        {
       
            tmpevals.inputDataStream(argList[i]);
            parameterListValue.push_back(tmpevals.result);
        }
    }
    catch (std::string errors)
    {
        std::string e = "错误 实参传递给形参时出错\n" + errors;
        error(e);
    }

    for (int i = 0; i < function.argc; i++)
    {
        funcInterpreter.memory[formalParameterList[i].STR_val] = parameterListValue[i];
    }//传递参数（将子内存中的形式参数赋值）
    
    std::vector<std::vector<D_token>> f;
    for (int i = 1; i < functionBody.size(); i++)
    {
        f.push_back(functionBody[i]);
    }

    funcInterpreter.inputScript(f);

    if (function.reference)
    {
       
        for (int i = 0; i < actualParameterNameList.size(); i++)
        {
            (*parentMemory)[actualParameterNameList[i].STR_val] = funcInterpreter.memory[formalParameterList[i].STR_val] ;//将形式参数值反覆盖实际参数值
        }
    }
    
    VARIANT returnVal = funcInterpreter.returnValue;
    return returnVal;
}

void Function::setMatrixVisitVec(std::vector<matrix_visit_token> *vec)
{
    this->matrix_visit_vec = vec;
}
