#pragma once
#include<iostream>
#include<vector>
#include<iomanip>
#include <sstream>
#include<string>
#define Min 1e-5

class Matrix
{
public:
	
	Matrix(int row, int col);//
	Matrix(std::vector<std::vector<double>> data);
	Matrix();
	Matrix(double num);
	Matrix operator+(Matrix M);//
	Matrix operator-(Matrix M);//
	Matrix operator=(Matrix M);//
	bool operator==(Matrix M);//
	bool operator!=(Matrix M);//
	Matrix operator*(Matrix M)  ;//
	
	
	
	bool isValid() { return errorFlag; };
	int getRow() { return this->row; };//
	int getCol() { return this->col; };//
	bool getErrorFlag() { return this->errorFlag; };
	void initMatrix(std::vector<std::vector<double>>);//
	void iniErrorFlag() { errorFlag = false; };
	Matrix scalarMulti(int num);
	Matrix scalarMulti(double num);
	

	//double* double_ptr;//
	std::vector<std::vector<double>> value;
	friend std::ostream& operator<< (std::ostream& out, Matrix& M)//
	{
		
		if (M.getErrorFlag())
		{
			out << "Compute Error" << std::endl;
			return out;//错误信息输出
			
		}

		int maxLength = 0;
		std::string tmpdata;
		//std::ostringstream sout(tmpdata);
		bool neg = false;
		bool up = false;
		for (int i = 0; i < M.getRow(); i++)

		{
			for (int j = 0; j < M.getCol(); j++)
			{
				tmpdata = std::to_string(M.value[i][j]);
				int size = tmpdata.size();
				if (size - 7 >= maxLength)
				{

					if (neg)
						continue;

					if (tmpdata.size() >= 7 + 6)
					{
						maxLength = 11;//科学计数法修正
						up = true;
						if (tmpdata.size() >= 7 + 6 + 1 && tmpdata[0] == '-')
						{
							neg = true;
							maxLength += 1;//负数修正


						}

					}
					else
					{
						maxLength = tmpdata.size() - 7;
					}
				}
				tmpdata.clear();
			}
		}
		if (up)
			maxLength = 11;
		if (neg)
			maxLength = 12;


		//std::cout << maxLength << std::endl;
		tmpdata.clear();

		for (int i = 0; i < M.getRow(); i++)
		{
			out << "   ";
			for (int j = 0; j < M.getCol(); j++)
			{

				out << std::to_string(M.value[i][j]);
				tmpdata = std::to_string(M.value[i][j]);
				int dl = tmpdata.size()-7;
				//out << "*" << dl << "*";
				if (tmpdata.size() > 7+6)
				{
					dl=maxLength-1;//科学计数法修正
				}
				if (tmpdata[0] == '-')
				{
					dl++;//负数修正
				}
				for (int k = 0; k < maxLength - dl; k++)
				{
					out << " ";
				}
				out << "  ";
				
				//out << tmpdata;
				tmpdata.clear();
				

			}
			
			std::cout << std::endl << std::endl;
		}

		return out;
	}
	friend std::istream& operator>> (std::istream& in, Matrix& M)
	{
		in >> M.row >> M.col;
		for (int i = 0; i < M.value.size(); i++)
		{
			for (int j = 0; j < M.value[i].size(); j++)
			{
				in >> M.value[i][j];
			}
		}

		return in;

	}

private:
	int row;
	int col;
	
	bool errorFlag=false;

};



