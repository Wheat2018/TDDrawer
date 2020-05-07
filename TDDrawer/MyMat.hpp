#pragma once

#include <iostream>
#include <iomanip>
#include <initializer_list>
#include "MyVector.hpp"

//ע��������ֵ����Ϊģ�壬���򿽱������
template <class T>
class MyMat {
private:
	bool destruct = true;
	void Resize(size_t rows , size_t cols ){
		if (rows * cols != this->rows * this->cols) {
			if (arr && destruct) delete[] arr;
			if (rows * cols) {
				arr = new T[rows * cols];
				destruct = true;
			}
			else arr = nullptr;
		}
		this->rows = rows;
		this->cols = cols;
	}

public:
	T* arr = nullptr;
	size_t rows = 0, cols = 0;
	int printWeight = 2;

	MyMat(size_t n = 0) : MyMat(n, n) { ; }
	MyMat(size_t rows, size_t cols): rows(rows), cols(cols) {
		if (rows == 0 || cols == 0) return;
		arr = new T[rows * cols];
	}
	MyMat(const MyMat<T>& mat) { *this = mat; }
	MyMat(MyMat<T>&& mat) { *this = (MyMat<T>&&) mat; }

	MyMat<T>& operator =(const MyMat<T>& mat) {
		if (this != &mat) {
			Resize(mat.rows, mat.cols);
			memcpy(arr, mat.arr, sizeof(T) * rows * cols);
			printWeight = mat.printWeight;
		}
		return *this;
	}

	MyMat<T>& operator =(MyMat<T>&& mat) {
		if (this != &mat) {
			if (arr && destruct) delete[] arr;
			destruct = mat.destruct;
			rows = mat.rows;
			cols = mat.cols;
			printWeight = mat.printWeight;
			arr = mat.arr;
			mat.arr = nullptr;
		}
		return *this;
	}

	MyMat<T>& operator = (std::initializer_list<T> il) {
		size_t ilSize = il.size();
		for (size_t i = 0; i < rows * cols; i++) {
			if (i < ilSize) arr[i] = *(il.begin() + i);
			else arr[i] = 0;
		}
		return *this;
	}

	~MyMat() {	if (arr && destruct) delete[] arr;	}

	bool Empty() { return arr == nullptr; }

	void Load(T* arr, size_t rows, size_t cols) { 
		this->rows = rows; 
		this->cols = cols; 
		this->arr = arr; 
		destruct = false;
	}
	void Clean() { memset(arr, 0, sizeof(T) * rows * cols); }
	void Set(T e) {
		T* p = arr;
		for (int i = rows * cols; i > 0; i--) *(p++) = e;
	}
	void Eye(){
		if (cols != rows) {
			std::cout << "MyMat::Inverse Error: �Ƿ�������!" << std::endl;
			return;
		}
		Clean();
		for (int i = 0; i < rows; i++) (*this)[i][i] = 1;
	}

	T* operator[](size_t index) { return arr + index * cols; }
	const T* operator[](size_t index)const { return arr + index * cols; }
	T& operator()(size_t index) { return arr[index]; }
	const T& operator()(size_t index)const { return arr[index]; }
#define OPERATOR_CORE(opt) T* p = arr; for (int i = rows * cols; i > 0; i--) *(p++) opt num; return *this;
#define OPERATOR_CORE2(opt)	T *p1 = arr, *p2 = mat.arr; for (int i = rows * cols; i > 0; i--) *(p1++) opt *(p2++); return *this;

	MyMat<T>& operator*=(T num) { OPERATOR_CORE(*=); }
	MyMat<T>& operator/=(T num) { OPERATOR_CORE(/=); }
	MyMat<T>& operator+=(T num) { OPERATOR_CORE(+=); }
	MyMat<T>& operator-=(T num) { OPERATOR_CORE(-=); }
	MyMat<T>& operator+=(const MyMat<T>& mat) { OPERATOR_CORE2(+=); }
	MyMat<T>& operator-=(const MyMat<T>& mat) { OPERATOR_CORE2(-=); }
	MyMat<T> operator*(T num) { return std::move(MyMat<T>(*this) *= num); }
	MyMat<T> operator/(T num) { return std::move(MyMat<T>(*this) /= num); }
	MyMat<T> operator+(T num) { return std::move(MyMat<T>(*this) += num); }
	MyMat<T> operator-(T num) { return std::move(MyMat<T>(*this) -= num); }

	MyMat<T> operator*(const MyMat<T>& mat) {			//����˷�
		MyMat<T> result(rows, mat.cols);
		if (cols == mat.rows) {
			for (int i = 0; i < result.rows; i++) {
				for (int j = 0; j < result.cols; j++) {
					T tmp = 0;
					for (int n = 0; n < cols; n++) tmp += (*this)[i][n] * mat[n][j];
					result[i][j] = tmp;
				}
			}
		}
		return result; 
	}

	MyArray<MyMat<T>> GetRowVectors()const {	//��ȡ������������ֵ��������������޸ģ������Swap������ȡ�ǳ�ֵ����
		/*ԭ��	MyArray�е�MyMat<T>�������⴦��Load�������󣬰���
				ָ����е�ָ�롣��������������ǳ������
					������һ��������������罫��������ֱֵ�Ӹ�ֵ����һ
				�������������еĸ�������MyMat<T>��ͬʱ����������򽫸�
				������������ֵ������ȥ����������������ֵ�����á�
					�����֮����������ֵ�����������ã���������ֵ�����
				����������ֵ��
					���ʹ��std::move��������������ֵ���ƶ�������ȡ������
				�����ظ�ʹ�ú�������ֵ��
		*/
		MyArray<MyMat<T>> result(rows);
		for (int i = 0; i < rows; i++) result[i].Load((*(MyMat<T>*)this)[i], 1, cols);
		return std::move(result);
	}

	MyMat<T> Inverse() {
#define SWAP(a,b) { auto SWAPTMP = a; a = b; b = SWAPTMP; }

		if (cols != rows) {
			std::cout << "MyMat::Inverse Error: �Ƿ�������!" << std::endl;
			return MyMat<T>(0);
		}

		MyMat<T>& rowMat = *this;
		//������������
		MyMat<T> result(rows);	
		result.Eye();

		//��ȡԭ�����������������������ã��Է�������б任
		MyArray<MyMat<T>> rowVec(0), feaRowVec(0);
		rowVec = std::move(rowMat.GetRowVectors());
		feaRowVec = std::move(result.GetRowVectors());

		//�����б任
		for (int i = 0; i < rows; i++) {
			if (rowMat[i][i] == 0) {							//�Խ�Ԫ��Ϊ0
				T tmp = 0;
				for (int rest = i + 1; rest < rows; rest++) {	//������Ѱ���в�Ϊ0��������
					tmp = rowMat[rest][i];
					if (tmp) {
						//rowVec[i] += rowVec[rest] / tmp;
						//feaRowVec[i] += feaRowVec[rest] / tmp;
						rowVec[rest] /= tmp;
						feaRowVec[rest] /= tmp;
						SWAP(rowVec[i], rowVec[rest]);
						SWAP(feaRowVec[i], feaRowVec[rest]);
						break;
					}
				}
				if(!tmp) return MyMat<T>(0);					//û���ҵ�ԭ������в�Ϊ0��������������󲻴���
			}
			else if (rowMat[i][i] != 1) {						//ԭ����Խ�Ԫ�ع�һ��
				T tmp = rowMat[i][i];
				rowVec[i] /= tmp;
				feaRowVec[i] /= tmp;
			}
			for (int n = 0; n < rows; n++) {					//��֤ԭ������н���һ����ֵ1
				if (n != i) {
					T tmp = rowMat[n][i];
					if (tmp) {
						rowVec[n] -= rowVec[i] * tmp;
						feaRowVec[n] -= feaRowVec[i] * tmp;
					}
				}
			}
		}
		std::swap(*this, result);
		return *this;
	}

	void Swap(const MyMat<T>& v) {
		MyMat<T> temp(0);
		memcpy(&temp, (MyMat<T>*)&v, sizeof(MyMat<T>));
		memcpy((MyMat<T>*)&v, this, sizeof(MyMat<T>));
		memcpy(this, &temp, sizeof(MyMat<T>));
		temp.arr = nullptr;
	}

	friend std::ostream& operator <<(std::ostream& o,  const MyMat<T>& mat) {
		for (size_t i = 0; i < mat.rows; i++) {
			for (size_t j = 0; j < mat.cols; j++) 
				o << std::setw(mat.printWeight + 4) << std::setprecision(mat.printWeight) 
				<< mat[i][j];
			o << std::endl;
		}
		return o;
	}

};

