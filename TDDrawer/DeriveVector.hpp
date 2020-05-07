#pragma once
#include "MyVector.hpp"
#include <iostream>
#include "Vector3.hpp"
#include "Canva.h"

class GraphicVec:public MyVector<Point3F>{
private:
	//屏蔽基类的可变长特性
	void push_back(const Point3F& element) { ; }
	void Delete(int index) { ; }
	void Clear() { ; }
	void Resize(size_t maxLen) { ; }
	void Sort() { ; }

public:
	size_t size;

	GraphicVec(size_t maxLen = 10) :MyVector<Point3F>(maxLen) { ; }
	GraphicVec(const MyVector<Point3F>& v) :MyVector<Point3F>(v) {; }
	//GraphicVec& operator = (const MyVector<Point3F>& v) { this->MyVector<Point3F>::operator=(v); }

	virtual void Draw(Canva& canva) { cout<<"GraphicVec:调用了无用函数！"<<endl; }

	virtual Point3F& operator [](size_t index) { return *(new Point3F()); }
};

class Vec_Point :public GraphicVec {
private:

public:

	Vec_Point():GraphicVec(1) { this->size = 1; }
	Vec_Point(const Point3F& p):Vec_Point() { (*this)[0] = p; }
	Vec_Point(const Vec_Point& vec_p) :GraphicVec(vec_p) { ; }
	Vec_Point& operator = (const Vec_Point& vec_p) { this->MyVector<Point3F>::operator=(vec_p); return *this; }

	Vec_Point& operator = (const Point3F& p) { (*this)[0] = p; return *this; }
	//friend std::ostream& operator << (std::ostream& o, const Vec_Point& vec_p) { return o << vec_p[0]; }

	void Draw(Canva& canva) { 
		;
	}

};

class Vec_Triangle :public GraphicVec {
private:

public:

	Vec_Triangle():GraphicVec(3) { this->size = 3; }
	Vec_Triangle(const Point3F& p1, const Point3F& p2, const Point3F& p3) :Vec_Triangle() { (*this)[0] = p1; (*this)[1] = p2; (*this)[2] = p3; }
	Vec_Triangle(const Vec_Triangle& vec_tria) :GraphicVec(vec_tria) { ; }

	Vec_Triangle& operator = (const Vec_Triangle& vec_tria) { this->MyVector<Point3F>::operator=(vec_tria); return *this; }

	//friend std::ostream& operator << (std::ostream& o, const Vec_Triangle& vec_p) { return o << "[" << vec_p[0] << vec_p[1] << vec_p[2] << "]"; }
	void Draw(Canva& canva) {
//		void Canva::Triangle_c(POINT p1, POINT p2, POINT p3, COLORREF color1, COLORREF color2, COLORREF color3, bool fill) {
//			if (!fill) {
//				Line_c(p1.x, p1.y, p2.x, p2.y, color1, color2);
//				Line_c(p1.x, p1.y, p3.x, p3.y, color1, color3);
//				Line_c(p2.x, p2.y, p3.x, p3.y, color2, color3);
//				return;
//			}
//
//			if (p1.y > p2.y) { swap(p1, p2); swap(color1, color2); }
//			if (p2.y > p3.y) {
//				if (p1.y > p3.y) { swap(p1, p3); swap(color1, color3); }
//				swap(p2, p3);
//				swap(color2, color3);
//			}
//
//			int minY = p1.y;
//			int height = p3.y - p1.y;
//
//			p1.y -= minY, p2.y -= minY, p3.y -= minY;
//
//			int *leftArr = new int[height + 1];
//			DWORD *leftColorArr = new DWORD[height + 1];
//			int *rightArr = new int[height + 1];
//			DWORD *rightColorArr = new DWORD[height + 1];
//
//#define TRIANGLE_C_CORE(p1,p2,c1,c2,sideArr,sideColorArr) {		\
//float delta_x, x;												\
//int dx, dy;														\
//dx = p2.x - p1.x;												\
//dy = p2.y - p1.y;												\
//																\
//delta_x = (float)dx / dy;										\
//x = p1.x;														\
//sideArr[p1.y] = x;												\
//sideColorArr[p1.y] = c1;										\
//for (int k = p1.y + 1; k <= p2.y; k++) {						\
//	x += delta_x;												\
//	sideArr[k] = x;												\
//	sideColorArr[k] = SOLVE_COLOR(c1, c2, (float)(k-p1.y) / dy);\
//}}
//
//			TRIANGLE_C_CORE(p1, p2, color1, color2, leftArr, leftColorArr);
//			TRIANGLE_C_CORE(p2, p3, color2, color3, leftArr, leftColorArr);
//			TRIANGLE_C_CORE(p1, p3, color1, color3, rightArr, rightColorArr);
//
//			int y;
//			int leftSide, rightSide, length;
//			COLORREF leftColor, rightColor;
//			for (int i = 0; i <= height; i++) {
//				y = i + minY;
//				if (y < 0 || y >= rows) continue;
//				
//				leftSide = leftArr[i];
//				rightSide = rightArr[i];
//				leftColor = leftColorArr[i];
//				rightColor = rightColorArr[i];
//				if (leftSide > rightSide) { swap(leftSide, rightSide); swap(leftColor, rightColor); }
//				length = rightSide - leftSide;
//				for (int j = leftSide; j <= rightSide; j++) {
//					Pixel(j, y, SOLVE_COLOR(leftColor, rightColor, float(j - leftSide) / length));
//				}
//
//				//Line_c(leftArr[i], y, rightArr[i], y, leftColorArr[i], rightColorArr[i]);
//			}
//
//			delete[] leftArr;
//			delete[] rightArr;
//		};
	}
};

typedef MyVector<GraphicVec> Graphic3DVector;