/*
	��    ��: Wheat
	����޸�: 2019.03.03
*/
#pragma once
#include <iostream>
#include "MyMat.hpp"
#include "Vector3F.h"

class Canva;
class Pointer_MyGraphic;
class Graphic_Triangle;
/*============================================================================
	ȷ��ֻʹ��MyGraphic��ĳһ��������ʱ�����Խ�Pointer_MyGraphic��������
	���ɶ�Ӧ���������������ΪMyGraphic���������Pointer_MyGraphic�Ľӿ�
	��һ���ġ�
	�滻�����ڶ�ָ̬����Ϊʵ�����ͣ������뿽������ʱ����٣�ԼΪ0.7������
=============================================================================*/
typedef MyVector<Pointer_MyGraphic> MyGraphicVector;
//typedef MyVector<Graphic_Triangle> MyGraphicVector;

class MyGraphic{
private:

protected:
	const size_t size;								//Ϊ��֤��Pointer_MyGraphic����ӿ�һ�£���size����

	void CheckIndex(size_t& index)const;

	MyGraphic& Assign(const MyGraphic& g);
	MyGraphic& Assign(const Pointer_MyGraphic& g);


public:
	MyVector<CPoint3D> Incise(const MyMat<float>& parameter)const;

	const size_t Size()const { return size; }

	//�����๹�캯��������û����ĳһ���캯���������޷����롣
	MyGraphic(size_t size) :size(size) { ; }

	MyGraphic(const CPoint3D* points, size_t size) :MyGraphic(size) { memcpy(GetDataPointer(), points, sizeof(CPoint3D) * size); }

	MyGraphicVector Incise3D(const MyMat<float>& parameter)const;

	MyGraphicVector Incise2D(const MyMat<float>& parameter)const;

	//�ӿ�����
	virtual MyGraphicVector Incise3D(Vector3F n, float D)const;
	/*	
		��    ��:	Incise
		��    ��:	��͸��ͶӰ�У���ʱ��Ҫ����Ƭ���п��ӿռ��ڵ��и������
					�������Ա�֤��������ʾ�Ŀռ�㲻���ڻ�ͼ�����г��֡�
		�������:	n �и�����ƽ��ķ�������
					D �и�����ƽ��̶���ֵ���ɷ�����n������������ΪA��B��C��
					�ٸ�������D���ɷ���Ax+By+Cz+D=0����ȷ���и�����ƽ��
		��������:	������ʵ����ȥ�и�����ƽ�渺��ռ䲿�֣��������ɸ��뱾��
					����ʵ��ͬ���͵�ʵ��
		��    ע:	
		ʾ    ��:	vec = object.Incise3D({1,0,0},0);
		��    ��:	Wheat
		����޸�:	2019.02.27
	*/
	
	virtual MyGraphicVector Incise2D(Vector3F n)const;
	/*	
		��    ��:	Incise
		��    ��:	��͸��ͶӰ�У���ʱ��Ҫ����Ƭ���п��ӿռ��ڵ��и������
					�������Ա�֤��������ʾ�Ŀռ�㲻���ڻ�ͼ�����г��֡���
					Incise3D��ͬ���ǣ�Incise2Dֻ��ͶӰ��ɺ�Ķ�λ��Ƭ������
					�
		�������:	n �и�����ֱ�ߵķ�������
					D �и�����ֱ�߹̶���ֵ���ɷ�����n������������ΪA��B��C��
					�ɷ���Ax+By+Cz+D=0����ȷ���и�����ƽ�档
		��������:	������ʵ����ȥ�и�����ƽ�渺��ռ䲿�֣��������ɸ��뱾��
					����ʵ��ͬ���͵�ʵ����
		��    ע:	
		ʾ    ��:	vec = object.Incise2D({1,0,0});
		��    ��:	Wheat
		����޸�:	2019.02.27
	*/

	virtual void Draw(Canva& canva)const = 0;
	/*	
		��    ��:	Draw
		��    ��:	�ڻ����н��б����������ʵ���Ļ�ͼ����
		�������:	canva Ŀ�껭����
		��������:	��
		��    ע:	�����������ʵ���У�����CPoint���Point3D�ɷ֣�������ͶӰ
					ת����PPoint�ɷ֣��ſɽ��иò�����
		ʾ    ��:	object.Draw(canva);
		��    ��:	Wheat
		����޸�:	2019.02.27
	*/

	virtual MyGraphic* Copy()const = 0;
	/*
		��    ��:	Copy
		��    ��:	�����ڴ棬��䱾���������ʵ���Ŀ�����
		�������:	��
		��������:	ָ�򿽱��ڶ�̬�ڴ��ʵ���Ķ�ָ̬�롣
		��    ע:	�ڴ���ͷź͹���Ȩ�ƽ������������ú���ֻ�����������ʼ����
		ʾ    ��:	p = object.Copy();
		��    ��:	Wheat
		����޸�:	2019.02.27
	*/

	virtual MyGraphic& operator =(const MyGraphic& g) = 0 { memcpy(GetDataPointer(), g.GetDataPointer(), sizeof(CPoint3D) * size); return *this; }
	/*
		��    ��:	operator =
		��    ��:	ͨ�ø�ֵ�����������һ�����麯������ӵ��Ĭ�ϵ��ú�����
					������������ͬ�ำֵʱ�������ø��ำֵ����Ĭ�Ϻ����壬
					���������ำֵʱ�����µ���������д�ĸ�ֵ������
		�������:	g Դ������ʵ����
		��������:	��ʵ�������á�
		��    ע:	Դ������ʵ���뱾������ʵ�����ʹ�С��ƥ��ʱ������޶ȵ�
					��ֵ�����������׳��쳣��
		ʾ    ��:	object = g;
		��    ��:	Wheat
		����޸�:	2019.02.27
	*/

	virtual CPoint3D* GetDataPointer()const { return (CPoint3D*)(this + 1); }
	/*
		��    ��:	GetDataPointer(const)
		��    ��:	��ȡ���������ĵ�/����ָ�롣
		�������:	��
		��������:	���������ĵ�/����ָ�롣
		��    ע:	Ϊ��߿����ٶȣ�Ĭ�ϻ�ȡ����ָ�뷽��Ϊthisָ��ƫ�ƻ���
					���ȣ�������������ջ����������Ϊ��������������࣬����
					������Ϊ�׸���Ա���������������������ʹ�ö�̬�ڴ���
					���ݣ��������ظú�����
		ʾ    ��:	object.GetDataPointer();
		��    ��:	Wheat
		����޸�:	2019.02.27
	*/

	CPoint3D& operator [](size_t index) { return GetDataPointer()[index]; }
	const CPoint3D& operator [](size_t index)const { return GetDataPointer()[index]; }
	/*
		��    ��:	operator [](const)
		��    ��:	���������ĵ�/���飨��ֵ����������
		�������:	index ������
		��������:	��������ʵ���ڸ������µĵ�ģ���ֵ�����á�
		��    ע:	����Խ��ʱ��runtime����
		ʾ    ��:	cpoint = object[0];
		��    ��:	Wheat
		����޸�:	2019.02.27
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
	Pointer_MyGraphic(const MyGraphic& g) { gp = g.Copy(); }										//MyGraphic�������죺���
	Pointer_MyGraphic(const Pointer_MyGraphic& pointer) { *this = pointer; }						//�������죺���
	Pointer_MyGraphic(Pointer_MyGraphic&& pointer) { *this = (Pointer_MyGraphic&&)pointer; }		//�ƶ����죺ǳ����
	Pointer_MyGraphic& operator=(const Pointer_MyGraphic& pointer);
	Pointer_MyGraphic& operator=(Pointer_MyGraphic&& pointer);
	
	operator MyGraphic&() { return  *gp; }
	operator const MyGraphic&()const { return  *gp; }

	//��������
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
	//���ӹ��캯������ɾ��
	Graphic_Point(const CPoint3D& p) :Graphic_Point() { this->p = p; }

	//�ӿ�����
	Graphic_Point() :MyGraphic(1) { ; }
	Graphic_Point(const CPoint3D* points) :MyGraphic(points, 1) { ; }
	virtual MyGraphic* Copy()const { return new Graphic_Point(*this); }
	virtual MyGraphic& operator =(const MyGraphic& g) { return Assign(g); }
	virtual void Draw(Canva& canva)const;
	
	//��ѡ�ӿ�
	virtual MyGraphicVector Incise3D(Vector3F n, float D)const;
	virtual MyGraphicVector Incise2D(Vector3F n)const;
};

class Graphic_Line :public MyGraphic {
protected:
	CPoint3D arr[2];

public:
	//���ӹ��캯������ɾ��
	Graphic_Line(const CPoint3D& p1, const CPoint3D& p2) :Graphic_Line() { arr[0] = p1; arr[1] = p2; }

	//�ӿ�����
	Graphic_Line() :MyGraphic(2) { ; }
	Graphic_Line(const CPoint3D* points) :MyGraphic(points, 2) { ; }
	virtual MyGraphic* Copy()const { return new Graphic_Line(*this); }
	virtual MyGraphic& operator =(const MyGraphic& g) { return Assign(g); }
	virtual void Draw(Canva& canva)const;

	//��ѡ�ӿ�
	virtual MyGraphicVector Incise3D(Vector3F n, float D)const;
	virtual MyGraphicVector Incise2D(Vector3F n)const;
};

class Graphic_Triangle :public MyGraphic {
protected:
	CPoint3D arr[3];

public:
	//���ӹ��캯������ɾ��
	Graphic_Triangle(const CPoint3D& p1, const CPoint3D& p2, const CPoint3D& p3) :Graphic_Triangle(){ arr[0] = p1; arr[1] = p2; arr[2] = p3; }

	//��չ����
	Vector3F GetN()const { return (Vector3F(arr[1] - arr[0]) ^ Vector3F(arr[2] - arr[1])).Unitize(); }
	MyGraphicVector Banding(COLORREF color = 0)const;

	//�ӿ�����
	Graphic_Triangle() :MyGraphic(3) { ; }
	Graphic_Triangle(const CPoint3D* points) :MyGraphic(points, 3) { ; }
	virtual MyGraphic* Copy()const { return new Graphic_Triangle(*this); }
	virtual MyGraphic& operator =(const MyGraphic& g) { return Assign(g); }
	virtual void Draw(Canva& canva)const;

	//��ѡ�ӿ�
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

