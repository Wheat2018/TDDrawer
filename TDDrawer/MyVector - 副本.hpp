#pragma once
#include <iostream>

template <class T>
struct MyVector {
public:
	int size = 0;
	size_t maxLen = 0;
	T *arr = nullptr;

	//bool deepCopy = true;			//在模板对象复制不需要进行深拷贝时关掉该开关，以便提速

	//MyVector() :maxLen(0), arr = nullptr{; }
	MyVector(size_t maxLen = 10) :maxLen(maxLen) { if (maxLen > 0) arr = new T[maxLen]; }
	MyVector(const MyVector<T>& v) { (*this) = v; }
	MyVector(MyVector<T>&& v) { (*this) = (MyVector<T>&&)v; }
	~MyVector() { if (arr) delete[] arr; }

	MyVector<T>& operator = (const MyVector<T>& v) {
		std::cout << "调用了赋值!" << std::endl; 
		if (this != &v) {
			Resize(v.maxLen);
			CopyElem(arr, v.arr, v.size);   //memcpy(arr, v.arr, sizeof(T)*v.size);
			size = v.size;
		}
		return *this;
	}
	MyVector<T>& operator = (MyVector<T>&& v) {
		std::cout << "调用了移动赋值!" << std::endl; 
		if (this != &v) {
			size = v.size;
			maxLen = v.maxLen;
			arr = v.arr;
			v.arr = nullptr;
		}
		return *this;
	}

	T& operator [] (int index) { return arr[index]; }
	friend std::ostream& operator << (std::ostream& o,const  MyVector& v) {
		o << '[';
		for (int i = 0; i < v.size; i++) {
			o << v.arr[i] << " ";
		}
		o << "\b]";
		return o;
	}

	void push_back(const T& element) {
		if (size >= maxLen) Dilatation();
		arr[size++] = element;
	}

	void push_back(T&& element) {
		if (size >= maxLen) Dilatation();
		arr[size++] = std::move(element);
	}

	void push_back(const MyVector<T>& v) {
		if (size + v.size > maxLen) Dilatation();
		CopyElem(arr + size, v.arr, v.size);
		size += v.size;
	}
	void Delete(int index) {
		if (index >= size) return;
		size--;
		if (index == size) return;

		CopyElem(arr + index, arr + index + 1, (size - index));			//memcpy(arr + index, arr + index + 1, sizeof(T)*(size - index));
	}
	void Clear() { size = 0; }
	void Resize(size_t maxLen) {
		Clear();

		if (maxLen == this->maxLen) return;

		if (arr) delete[] arr;
		this->maxLen = maxLen;
		arr = new T[maxLen];
	}
	void Swap(const MyVector<T>& v) {
		MyVector<T> temp(0);
		memcpy(&temp, (MyVector<T>*)&v, sizeof(MyVector<T>));
		memcpy((MyVector<T>*)&v, this, sizeof(MyVector<T>));
		memcpy(this, &temp, sizeof(MyVector<T>));
		temp.arr = nullptr;
	}
	void Sort() { qsort(arr, size, sizeof(T), Compare); }



private:

	static int Compare(const void* elem1, const void* elem2) { return *(T*)elem1 > *(T*)elem2; }

	inline void CopyElem(T* dst, T*src, size_t size) {
		for (int i = 0; i < size; i++) *(dst++) = *(src++);
		//if (deepCopy) {
		//	for (int i = 0; i < size; i++) *(dst++) = *(src++);
		//}
		//else {
		//	memcpy(dst, src, sizeof(T) * size);
		//}
	}

	void Dilatation(size_t maxLen = 0) {					//注意：容器无填充 或 arr无指向 时调用会出错
		//std::cout << "MyVector: Dilatation!" << std::endl;
		if (maxLen > this->maxLen)return;
		if (maxLen) this->maxLen = maxLen;
		else {
			if (this->maxLen) this->maxLen *= 2;
			else this->maxLen = 1;
		}
		T* temp = arr;
		arr = new T[this->maxLen];
		if (temp) {
			CopyElem(arr, temp, size);		//memcpy(arr, temp, sizeof(T)*size);
			delete[] temp;
		}
	}
};
