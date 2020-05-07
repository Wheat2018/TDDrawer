/*
	作    者: Wheat
	最后修改: 2019.03.03
*/
#pragma once
#include <iostream>
#include "MyMat.hpp"
#include <Vector3.hpp>
#include <ppl.h>

#define PPL_Parallel
//#define Pragma_Parallel

#define Euclidean_distance3D(p1,p2) (sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z)))
#define Euclidean_distance2D(p1,p2) (sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)))

class Graphic_Polygon;
typedef MyVector<Graphic_Polygon> MyGraphicVector;
__interface CanvaInterface;
__interface FastCanvaInterface;
__interface Brush;


__interface CanvaInterface {
	inline void Pixel(int x, int y, float depth, COLORREF color = 0);
	inline void Draw(const Brush& g);
	inline void Draw(const MyGraphicVector& v) { ; }
	inline size_t Rows();
	inline size_t Cols();
	inline void Clean();
};
__interface FastCanvaInterface :public CanvaInterface {
	inline void DrawHorizonLine(int x, int y1, float depth1, COLORREF color1, int y2, float depth2, COLORREF color2);
};
__interface Brush {
	void Draw(CanvaInterface& canva)const;
};


class Graphic_Polygon :public Brush {
protected:

public:
	MyArray<CPoint3F> arr;
	//附加构造函数，可删除
	Graphic_Polygon() { ; }
	Graphic_Polygon(size_t size) :arr(size) { ; }
	Graphic_Polygon(const Graphic_Polygon& g) :arr(g.arr) { ; }
	Graphic_Polygon(Graphic_Polygon&& g) :arr(std::move(g.arr)) { ; }
	Graphic_Polygon(const CPoint3F* points, size_t size) :Graphic_Polygon(size) {
		memcpy(&arr[0], points, sizeof(CPoint3F) * size);
	}
	Graphic_Polygon(CPoint3F p) :Graphic_Polygon(1) { arr[0] = p; }
	Graphic_Polygon(CPoint3F p1, CPoint3F p2) :Graphic_Polygon(2) { arr[0] = p1, arr[1] = p2; }
	Graphic_Polygon(CPoint3F p1, CPoint3F p2, CPoint3F p3) :Graphic_Polygon(3) { arr[0] = p1, arr[1] = p2, arr[2] = p3; }

	Graphic_Polygon(const MyArray<CPoint3F>& arr) :arr(arr) { ; }
	Graphic_Polygon(MyArray<CPoint3F>&& arr) :arr(std::move(arr)) { ; }

	Graphic_Polygon& operator =(const Graphic_Polygon& g) { arr = g.arr; return *this; }
	Graphic_Polygon& operator =(Graphic_Polygon&& g) { arr = std::move(g.arr); return *this; }
	CPoint3F& operator [](size_t index) { return arr[index]; }
	const CPoint3F& operator [](size_t index)const { return arr[index]; }
	friend std::ostream& operator << (std::ostream& o, const Graphic_Polygon& g);

	size_t Size()const { return arr.Len(); }
	//拓展特性
	Vector3F GetN()const { return (Vector3F&)(Vector3F(arr[1] - arr[0]) ^ Vector3F(arr[2] - arr[1])).Unitize(); }

	void Draw(CanvaInterface& canva)const;

	Graphic_Polygon Incise(const MyMat<float>& parameter)const;


};

class Graphic_Polygon_Virtual {
protected:

public:
	size_t size;
	size_t index;
	Graphic_Polygon_Virtual(size_t size = 0, size_t index = 0) :index(index), size(size) { ; }


	void Draw(CanvaInterface& canva, CPoint3F* arr)const;

	Graphic_Polygon Incise(const MyMat<float>& parameter, CPoint3F* arr)const;
};

class MyGraphicFastVector : public Brush {
public:
	MyVector<CPoint3F> data_vec;
	MyVector<Graphic_Polygon_Virtual> pol_vec;

	MyGraphicFastVector() { ; }
	MyGraphicFastVector(size_t data_vec_length, size_t pol_vec_length) :
		data_vec(data_vec_length),pol_vec(pol_vec_length) { ; }
	MyGraphicFastVector(const MyGraphicFastVector& v) :
		data_vec(v.data_vec), pol_vec(v.pol_vec) { ; }
	MyGraphicFastVector(MyGraphicFastVector&& v):
		data_vec(std::move(v.data_vec)),pol_vec(std::move(v.pol_vec)) { ; }

	MyGraphicFastVector& operator = (const MyGraphicFastVector& v) { 
		data_vec.Copy(v.data_vec);
		pol_vec.Copy(v.pol_vec);
		return *this;
	}
	MyGraphicFastVector& operator = (MyGraphicFastVector&& v) { 
		data_vec = std::move(v.data_vec);
		pol_vec = std::move(v.pol_vec);
		return *this;
	}

	void Clear() { data_vec.Clear(); pol_vec.Clear(); }

	void push_back(const Graphic_Polygon& e) {
		pol_vec.push_back(Graphic_Polygon_Virtual(e.Size(), data_vec.size));
		for (size_t i = 0; i < e.Size(); i++) data_vec.push_back(e[i]);
	}

	void join_back(const MyGraphicVector& v) {
		for (size_t i = 0; i < v.size; i++) push_back(v[i]);
	}

	void Draw(CanvaInterface& canva)const {
		CPoint3F *arr = data_vec.arr.arr;
		for (size_t i = 0; i < pol_vec.size; i++) 
			pol_vec[i].Draw(canva, arr);
	}

	MyGraphicFastVector Incise(const MyMat<float>& parameter)const {
		MyGraphicFastVector result(data_vec.size, pol_vec.size);
		CPoint3F *arr = data_vec.arr.arr;
		for (size_t i = 0; i < pol_vec.size; i++) 
			result.push_back(pol_vec[i].Incise(parameter,arr));
		//concurrency::critical_section cs;
		//concurrency::parallel_for_each(&pol_vec[0], &pol_vec[0] + pol_vec.size, [&](const Graphic_Polygon_Virtual& p) {
		//	Graphic_Polygon tmp = std::move(p.Incise(parameter, arr));
		//	cs.lock();
		//	result.push_back(tmp);
		//	cs.unlock();
		//	});
		return std::move(result);
	}
};
