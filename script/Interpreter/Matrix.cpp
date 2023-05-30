#include "Matrix.h"
Matrix::Matrix(int row, int col)
{
	if (row <= 0 || col <= 0)
	{
		this->row = 1; this->col = 1;
	}
	this->row = row;
	this->col = col;
	
	std::vector<double> row_value;
	for (int i = 0; i < col; i++)
	{
		row_value.push_back(0);
	}
	for (int i = 0; i < row; i++)
	{
		value.push_back(row_value);
	}//初始化


}

Matrix::Matrix()
{
	this->row = 1; this->col = 1;
	

	std::vector<double> row_value;
	for (int i = 0; i < col; i++)
	{
		row_value.push_back(0);
	}
	for (int i = 0; i < row; i++)
	{
		value.push_back(row_value);
	}//初始化

}

Matrix::Matrix(double num)
{
	this->row = 1; this->col = 1;
	
	std::vector<double> row_value;
	for (int i = 0; i < col; i++)
	{
		row_value.push_back(0);
	}
	for (int i = 0; i < row; i++)
	{
		value.push_back(row_value);
	}//初始化
	value[0][0] = num;
}

Matrix::Matrix(std::vector<std::vector<double>> data)
{
	this->row = data.size();
	int maxCol = 0;
	for (int i = 0; i < data.size(); i++)
	{
		if (maxCol <= data[i].size())
			maxCol = data[i].size();
	}
	this->col = maxCol;//找到向量中最大的那个作为列值
	Matrix tmp(this->getRow(), this->getCol());
	*this = tmp;

	value = data;
	int maxCount = 0;
	for (auto line : value)
	{
		if (line.size() > maxCount)
		{
			maxCount = line.size();
		}
	}
	for (int i=0;i<value.size();i++)
	{
		if (value[i].size() < maxCount)
		{
			for (int j = value[i].size(); j < maxCount; j++)
			{
				value[i].push_back(0);
			}
		}
	}

}


Matrix Matrix::operator+(Matrix M)
{
	if (M.getErrorFlag() == true)
	{
		this->errorFlag = true;
	}
	Matrix obj (this->getRow(),this->getCol());
	
	if (M.getCol() == obj.getCol() && M.getRow() == obj.getRow())
	{
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				obj.value[i][j] = M.value[i][j] + this->value[i][j];
				return obj;
			}
		}
	}



	obj.errorFlag = true;//型号不匹配时
	std::string errorMsg = "矩阵运算 型号不匹配";
	throw(errorMsg);
	return obj;

}

Matrix Matrix::operator-(Matrix M)
{
	if (M.getErrorFlag() == true)
		this->errorFlag = true;//连续运算时的错误传递
	Matrix obj(this->getRow(), this->getCol());
	

	if (M.getCol() == obj.getCol() && M.getRow() == obj.getRow())
	{
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				obj.value[i][j] = M.value[i][j] - this->value[i][j];
				return obj;
			}
		}
	}
	std::string errorMsg = "矩阵运算 型号不匹配";
	throw(errorMsg);
	obj.errorFlag = true;//型号不匹配时
	return obj;

}

bool Matrix::operator==(Matrix M)
{
	if (M.getErrorFlag() == true)
		this->errorFlag = true;//连续运算时的错误传递
	

	if ((M.getCol() == this->getCol() && M.getRow() == this->getRow()))
	{

		for (int i = 0; i<row; i++)
			for(int j=0;j<col;j++)
			if (M.value[i][j]!=this->value[i][j])
			{
				return false;//元素不相等
			}
		return true;
	}



	return false;//型号不相等
}

bool Matrix::operator!=(Matrix M)
{
	if (M.getErrorFlag() == true)
		this->errorFlag = true;//连续运算时的错误传递
	

	if (M == *this)
	{
		return false;
	}
	return true;//型号不相等
}

Matrix Matrix::operator=(Matrix M)
{
	if (M.getErrorFlag() == true)
	{
		this->errorFlag = true;//连续运算时的错误传递
		return *this;
	}
	
	this->row = M.getRow();
	this->col = M.getCol();//重新初始化
	this->value = M.value;
	
	return *this;//返回左值

}

Matrix Matrix::operator*( Matrix M)  //this*M
{

	

	if (M.getErrorFlag() == true)
	{
		this->errorFlag = true;//连续运算时的错误传递
		return *this;
	}
	
	if (!(this->col == M.row))
	{
		std::string errorMsg = "矩阵乘法 型号不匹配";
		throw(errorMsg);
		this->errorFlag = true;//乘法型号不匹配
		return *this;
	}
	Matrix result(this->row, M.col);

	//col + row*COL

	for (int i = 0; i < result.getRow(); i++)

		for (int j = 0; j < result.getCol(); j++)//i j是结果矩阵的i j
		{
			double tmp = 0;
			for (int m = 0; m < this->getCol(); m++)//每一行依次相乘再相加
			{
				tmp += (this->value[i][m]) * (M.value[m][j]);
			}
			result.value[i][j] = tmp;

		}
	return result;


	
}

void Matrix::initMatrix(std::vector<std::vector<double>> data)
{
	this->row = data.size();
	int maxCol = 0;
	for (int i = 0; i < data.size(); i++)
	{
		if (maxCol <= data[i].size())
			maxCol = data[i].size();
	}
	this->col = maxCol;//找到向量中最大的那个作为列值
	

	
	
	
	int count = 0;
	int diff = 0;
	
	Matrix tmp(this->getRow(), this->getCol()) ;
	*this=tmp ;



	value = data;
	int maxCount = 0;
	for (auto line : value)
	{
		if (line.size() > maxCount)
		{
			maxCount = line.size();
		}
	}
	for (auto it : value)
	{
		if (it.size() < maxCount)
		{
			for (int i = it.size(); i < maxCount; i++)
			{
				it.push_back(0);
			}
		}
	}

}

Matrix Matrix::scalarMulti(int num)
{
	Matrix result;
	result = *this;
	for (int i = 0; i < value.size(); i++)
	{
		for (int j = 0; j < value[i].size(); j++)
		{
			value[i][j] *= num;
		}
	}
	return result;
}

Matrix Matrix::scalarMulti(double num)
{
	Matrix result;
	result = *this;
	for (int i = 0; i < value.size(); i++)
	{
		for (int j = 0; j < value[i].size(); j++)
		{
			value[i][j] *= num;
		}
	}
	return result;
}

	
