#pragma once

#include <iostream>
#include <iomanip>
#include <initializer_list>
#include "MyVector.hpp"

template <class T>
class MyMat {
private:

public:
	MyArray<T> arr;
	size_t rows = 0, cols = 0;
	int printWeight = 2;

	MyMat(size_t n = 0) : MyMat(n, n) { ; }
	MyMat(size_t rows, size_t cols): rows(rows), cols(cols) {
		if (rows == 0 || cols == 0) return;
		arr.Resize(rows * cols);
	}
	MyMat(const MyMat<T>& mat) { *this = mat; }
	MyMat(MyMat<T>&& mat) { *this = (MyMat<T>&&) mat; }

	MyMat<T>& operator =(const MyMat<T>& mat) {
		if (this != &mat) {
			rows = mat.rows;
			cols = mat.cols;
			printWeight = mat.printWeight;
			arr = mat.arr;
		}
		return *this;
	}

	MyMat<T>& operator =(MyMat<T>&& mat) {
		if (this != &mat) {
			rows = mat.rows;
			cols = mat.cols;
			printWeight = mat.printWeight;
			arr = std::move(mat.arr);
		}
		return *this;
	}

	bool Empty() { return arr == nullptr; }

	void Load(T* arr, size_t rows, size_t cols) { 
		this->rows = rows; 
		this->cols = cols; 
		this->arr.Load(arr, rows*cols);
	}
	void Clean() { Set(0); }
	void Set(T e) {
		arr.Set(e);
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

	MyMat<T>& operator=(std::initializer_list<T> il) {
		for (size_t i = 0; i < rows * cols; i++) {
			if (i < il.size()) arr[i] = *(il.begin() + i);
			else arr[i] = 0;
		}
		return *this;
	}
#define OPERATOR_CORE(opt) T* p = arr + 0; for (int i = rows * cols; i > 0; i--) *(p++) opt num; return *this;
#define OPERATOR_CORE2(opt)	T *p1 = arr + 0; const T *p2 = mat.arr + 0; for (int i = rows * cols; i > 0; i--) *(p1++) opt *(p2++); return *this;

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

	MyArray<MyMat<T>> GetRowVectors()const {	//��ȡ������������ֵ��������������޸ģ����std::moveʹ��
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
		for (int i = 0; i < rows; i++) result[i].Load((T*)(*this)[i], 1, cols);

		return std::move(result);
	}

	MyMat<T> Inverse()const {
		if (cols != rows) {
			std::cout << "MyMat::Inverse Error: �Ƿ�������!" << std::endl;
			return MyMat<T>(0);
		}

		MyMat<T>& rowMat = (MyMat<T>&)(*this);
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
						rowVec[i] += rowVec[rest] / tmp;
						feaRowVec[i] += feaRowVec[rest] / tmp;
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
					rowVec[n] -= rowVec[i] * tmp;
					feaRowVec[n] -= feaRowVec[i] * tmp;
				}
			}
		}
		return result;
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
				<< ((MyMat<T>&)mat)[i][j];
			o << std::endl;
		}
		return o;
	}

};

