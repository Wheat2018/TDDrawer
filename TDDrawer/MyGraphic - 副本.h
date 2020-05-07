/*
	作    者: Wheat
	最后修改: 2019.03.03
*/
#pragma once
#include <iostream>
#include "MyMat.hpp"
#include "Vector3F.h"

class Canva;
class Pointer_MyGraphic;
class Graphic_Triangle;
/*============================================================================
	确保只使用MyGraphic的某一种派生类时，可以将Pointer_MyGraphic的容器替
	换成对应派生类的容器，因为MyGraphic的派生类和Pointer_MyGraphic的接口
	是一样的。
	替换后由于多态指针变更为实际类型，构造与拷贝花费时间减少（约为0.7倍）。
=============================================================================*/
typedef MyVector<Pointer_MyGraphic> MyGraphicVector;
//typedef MyVector<Graphic_Triangle> MyGraphicVector;

class MyGraphic{
private:

protected:
	const size_t size;								//为保证与Pointer_MyGraphic对外接口一致，将size隐藏

	void CheckIndex(size_t& index)const;

	MyGraphic& Assign(const MyGraphic& g);
	MyGraphic& Assign(const Pointer_MyGraphic& g);


public:
	MyVector<CPoint3D> Incise(const MyMat<float>& parameter)const;

	const size_t Size()const { return size; }

	//派生类构造函数必须调用基类的某一构造函数，否则无法编译。
	MyGraphic(size_t size) :size(size) { ; }

	MyGraphic(const CPoint3D* points, size_t size) :MyGraphic(size) { memcpy(GetDataPointer(), points, sizeof(CPoint3D) * size); }

	MyGraphicVector Incise3D(const MyMat<float>& parameter)const;

	MyGraphicVector Incise2D(const MyMat<float>& parameter)const;

	//接口特性
	virtual MyGraphicVector Incise3D(Vector3F n, float D)const;
	/*	
		名    称:	Incise
		简    述:	在透视投影中，有时需要对面片进行可视空间内的切割工作，此
					操作可以保证不参与显示的空间点不会在绘图操作中出现。
		传入参数:	n 切割依据平面的法向量。
					D 切割依据平面固定数值。由法向量n的三个变量作为A、B、C，
					再根据所给D，由方程Ax+By+Cz+D=0即可确定切割依据平面
		传出参数:	本对象实例切去切割依据平面负半空间部分，返回若干个与本派
					生类实例同类型的实例
		备    注:	
		示    例:	vec = object.Incise3D({1,0,0},0);
		作    者:	Wheat
		最后修改:	2019.02.27
	*/
	
	virtual MyGraphicVector Incise2D(Vector3F n)const;
	/*	
		名    称:	Incise
		简    述:	在透视投影中，有时需要对面片进行可视空间内的切割工作，此
					操作可以保证不参与显示的空间点不会在绘图操作中出现。与
					Incise3D不同的是，Incise2D只对投影完成后的二位面片进行切
					割。
		传入参数:	n 切割依据直线的法向量。
					D 切割依据直线固定数值。由法向量n的三个变量作为A、B、C，
					由方程Ax+By+Cz+D=0即可确定切割依据平面。
		传出参数:	本对象实例切去切割依据平面负半空间部分，返回若干个与本派
					生类实例同类型的实例。
		备    注:	
		示    例:	vec = object.Incise2D({1,0,0});
		作    者:	Wheat
		最后修改:	2019.02.27
	*/

	virtual void Draw(Canva& canva)const = 0;
	/*	
		名    称:	Draw
		简    述:	在画布中进行本派生类对象实例的绘图操作
		传入参数:	canva 目标画布。
		传出参数:	无
		备    注:	本派生类对象实例中，所有CPoint点的Point3D成分，都必须投影
					转换成PPoint成分，才可进行该操作。
		示    例:	object.Draw(canva);
		作    者:	Wheat
		最后修改:	2019.02.27
	*/

	virtual MyGraphic* Copy()const = 0;
	/*
		名    称:	Copy
		简    述:	申请内存，填充本派生类对象实例的拷贝。
		传入参数:	无
		传出参数:	指向拷贝于动态内存的实例的多态指针。
		备    注:	内存的释放和管理权移交主调函数，该函数只负责申请与初始化。
		示    例:	p = object.Copy();
		作    者:	Wheat
		最后修改:	2019.02.27
	*/

	virtual MyGraphic& operator =(const MyGraphic& g) = 0 { memcpy(GetDataPointer(), g.GetDataPointer(), sizeof(CPoint3D) * size); return *this; }
	/*
		名    称:	operator =
		简    述:	通用赋值运算符。这是一个纯虚函数，但拥有默认调用函数体
					其子类在遇到同类赋值时，仅调用父类赋值函数默认函数体，
					而遇到旁类赋值时，向下调用子类重写的赋值函数。
		传入参数:	g 源派生类实例。
		传出参数:	本实例的引用。
		备    注:	源派生类实例与本派生类实例类型大小不匹配时，最大限度地
					赋值，不报错，不抛出异常。
		示    例:	object = g;
		作    者:	Wheat
		最后修改:	2019.02.27
	*/

	virtual CPoint3D* GetDataPointer()const { return (CPoint3D*)(this + 1); }
	/*
		名    称:	GetDataPointer(const)
		简    述:	获取派生类对象的点/数组指针。
		传入参数:	无
		传出参数:	派生类对象的点/数组指针。
		备    注:	为提高拷贝速度，默认获取数据指针方法为this指针偏移基类
					长度，适用于所有以栈类型数组作为数据数组的派生类，且数
					据数组为首个成员变量。若非这种情况（如使用动态内存存放
					数据），请重载该函数。
		示    例:	object.GetDataPointer();
		作    者:	Wheat
		最后修改:	2019.02.27
	*/

	CPoint3D& operator [](size_t index) { return GetDataPointer()[index]; }
	const CPoint3D& operator [](size_t index)const { return GetDataPointer()[index]; }
	/*
		名    称:	operator [](const)
		简    述:	派生类对象的点/数组（常值）索引器。
		传入参数:	index 索引。
		传出参数:	本派生类实例在该索引下的点的（常值）引用。
		备    注:	索引越界时，runtime错误。
		示    例:	cpoint = object[0];
		作    者:	Wheat
		最后修改:	2019.02.27
	*/

	virtual Vector3F GetN()const { return Vector3F(); }
	friend std::ostream& operator << (std::ostream& o, const MyGraphic& g);

};

class Pointer_MyGraphic {
private:
	MyGraphic* gp = nullptr;

public:
	Pointer_MyGraphic() { ; }
	~Pointer_MyGraphic() { if(gp) delete gp; }
	Pointer_MyGraphic(const MyGraphic& g) { gp = g.Copy(); }										//MyGraphic参数构造：深拷贝
	Pointer_MyGraphic(const Pointer_MyGraphic& pointer) { *this = pointer; }						//拷贝构造：深拷贝
	Pointer_MyGraphic(Pointer_MyGraphic&& pointer) { *this = (Pointer_MyGraphic&&)pointer; }		//移动构造：浅拷贝
	Pointer_MyGraphic& operator=(const Pointer_MyGraphic& pointer);
	Pointer_MyGraphic& operator=(Pointer_MyGraphic&& pointer);
	
	operator MyGraphic&() { return  *gp; }
	operator const MyGraphic&()const { return  *gp; }

	//对象特性
	size_t Size() { return gp->Size(); }
	MyGraphic* Copy()const;
	CPoint3D* GetDataPointer()const;
	CPoint3D& operator[](int index)const;
	void Draw(Canva& canva)const;
	MyGraphicVector Incise3D(Vector3F n, float D)const;
	MyGraphicVector Incise2D(Vector3F n)const;
	friend std::ostream& operator << (std::ostream& o, const Pointer_MyGraphic& g) { if (g.gp) return o << *(g.gp); return o; }

	friend class MyGraphic;
};

class Graphic_Point :public MyGraphic {
protected:
	CPoint3D p;

public:
	//附加构造函数，可删除
	Graphic_Point(const CPoint3D& p) :Graphic_Point() { this->p = p; }

	//接口特性
	Graphic_Point() :MyGraphic(1) { ; }
	Graphic_Point(const CPoint3D* points) :MyGraphic(points, 1) { ; }
	virtual MyGraphic* Copy()const { return new Graphic_Point(*this); }
	virtual MyGraphic& operator =(const MyGraphic& g) { return Assign(g); }
	virtual void Draw(Canva& canva)const;
	
	//可选接口
	virtual MyGraphicVector Incise3D(Vector3F n, float D)const;
	virtual MyGraphicVector Incise2D(Vector3F n)const;
};

class Graphic_Line :public MyGraphic {
protected:
	CPoint3D arr[2];

public:
	//附加构造函数，可删除
	Graphic_Line(const CPoint3D& p1, const CPoint3D& p2) :Graphic_Line() { arr[0] = p1; arr[1] = p2; }

	//接口特性
	Graphic_Line() :MyGraphic(2) { ; }
	Graphic_Line(const CPoint3D* points) :MyGraphic(points, 2) { ; }
	virtual MyGraphic* Copy()const { return new Graphic_Line(*this); }
	virtual MyGraphic& operator =(const MyGraphic& g) { return Assign(g); }
	virtual void Draw(Canva& canva)const;

	//可选接口
	virtual MyGraphicVector Incise3D(Vector3F n, float D)const;
	virtual MyGraphicVector Incise2D(Vector3F n)const;
};

class Graphic_Triangle :public MyGraphic {
protected:
	CPoint3D arr[3];

public:
	//附加构造函数，可删除
	Graphic_Triangle(const CPoint3D& p1, const CPoint3D& p2, const CPoint3D& p3) :Graphic_Triangle(){ arr[0] = p1; arr[1] = p2; arr[2] = p3; }

	//拓展特性
	Vector3F GetN()const { return (Vector3F(arr[1] - arr[0]) ^ Vector3F(arr[2] - arr[1])).Unitize(); }
	MyGraphicVector Banding(COLORREF color = 0)const;

	//接口特性
	Graphic_Triangle() :MyGraphic(3) { ; }
	Graphic_Triangle(const CPoint3D* points) :MyGraphic(points, 3) { ; }
	virtual MyGraphic* Copy()const { return new Graphic_Triangle(*this); }
	virtual MyGraphic& operator =(const MyGraphic& g) { return Assign(g); }
	virtual void Draw(Canva& canva)const;

	//可选接口
	virtual MyGraphicVector Incise3D(Vector3F n, float D)const;
	virtual MyGraphicVector Incise2D(Vector3F n)const;
};

class Line_Triangle : public Graphic_Triangle {
public:
	Line_Triangle() { ; }
	Line_Triangle(const CPoint3D& p1, const CPoint3D& p2, const CPoint3D& p3) :Graphic_Triangle(p1, p2, p3) { ; }
	Line_Triangle(const Graphic_Triangle& tri) :Graphic_Triangle(tri) { ; }
	MyGraphic* Copy()const { return new Line_Triangle(*this); }
	MyGraphicVector Incise3D(Vector3F n, float D)const {
		MyGraphicVector result;
		result.push_back(*this);
		return std::move(result);
	}
	MyGraphicVector Incise2D(Vector3F n)const {
		MyGraphicVector result;
		result.push_back(*this);
		return std::move(result);
	}
	void Draw(Canva& canva)const;

};

