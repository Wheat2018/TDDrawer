#pragma once
#include "Vector3.hpp"
#include "MyGraphic.h"
#include <amp.h>
#include <ppl.h>

typedef Point3F PPoint;		//projected point ͶӰ�㣬����ͶӰ�㣬(x,y)��ʾ��Ļ��ӡλ�ã�zֵ��ʾ����
#define PI 3.1415626535

class ViewZone {
private:

public:
	//��һ
	Vector3F near_vec;			//��Ұ��������α߽�����
	float viewDepth;			//��Ұ��Զ������Ȳ�

	//���
	float angleX;				//X������Ұ�Ƕ�
	float angleY;				//Y������Ұ�Ƕ�
	float nearDepth;			//���������
	float farDepth;				//Զ�������


	//��Լ������
	Vector3F far_vec;			//��ҰԶ������α߽�����
	MyMat<float> inciseParameter3D;
	MyMat<float> inciseParameter2D;

	//��������
	float canvaWidth = 500;					//ͶӰ������
	float canvaHeight = 500;				//ͶӰ������

	ViewZone(const Vector3F& near, float viewDepth);
	ViewZone(float angleX, float angleY, float nearDepth, float farDepth);
	
	//�������и�
	inline Graphic_Polygon ZoneIncise3D(const Graphic_Polygon& v)const { return v.Incise(inciseParameter3D); }
	MyGraphicVector ZoneIncise3D(const MyGraphicVector& v)const;
	MyGraphicFastVector ZoneIncise3D(const MyGraphicFastVector& v)const { return v.Incise(inciseParameter3D); }
	inline Graphic_Polygon ZoneIncise2D(const Graphic_Polygon& v)const { return v.Incise(inciseParameter2D); }
	MyGraphicVector ZoneIncise2D(const MyGraphicVector& v)const;
	MyGraphicFastVector ZoneIncise2D(const MyGraphicFastVector& v)const { return v.Incise(inciseParameter2D); }

	//ͶӰ�任���������
	inline static void ViewZoneProject(
		const Vector3F& near_vec,
		const float& nearDepth,
		const float& canvaWidth,
		const float& canvaHeight,
		Point3F& p) restrict(amp,cpu)
	{
		float pro = nearDepth / p.z;
		p.x = (p.x * pro / near_vec.x / 2 + 0.5f) * canvaWidth;
		p.y = (-p.y * pro / near_vec.y / 2 + 0.5f) * canvaHeight;
	}

	inline PPoint Project(const Point3F& p)const {
		PPoint result = p;
		ViewZoneProject(near_vec, nearDepth, canvaWidth, canvaHeight, result);
		return result;
	}
	inline void Project_(Point3F& p)const {
		ViewZoneProject(near_vec, nearDepth, canvaWidth, canvaHeight, p);
	}
	inline Graphic_Polygon Project(const Graphic_Polygon& g)const {
		Graphic_Polygon result(g);
		for (int i = result.Size() - 1; i >= 0; i--) Project_(result[i]);
		return std::move(result);
	}

	inline void Project_(Graphic_Polygon& g)const {
		for (int i = g.Size() - 1; i >= 0; i--) Project_(g[i]);
	}
	inline MyGraphicVector Project(const MyGraphicVector& v)const {
		//for (size_t i = 0; i < v.size; i++) result.push_back(Project(v[i]));
		//MyGraphicVector result(v);
		//for (size_t i = 0; i < result.size; i++) Project_(result[i]);
		MyGraphicVector result(v.size);
		result.Fill();

#ifdef PPL_Parallel

		concurrency::parallel_for(0, v.size, [&](int idx) {
			result[idx] = std::move(Project(v[idx]));
			});

#else

#ifdef Pragma_Parallel
#pragma omp parallel for
#endif // Pragma_Parallel

		for (int idx = 0; idx < v.size; idx++)
			result[idx] = std::move(Project(v[idx]));

#endif // PPL_Parallel


		return std::move(result);
	}
	inline MyGraphicFastVector Project(const MyGraphicFastVector& v)const {
		MyGraphicFastVector result(v);

#ifdef PPL_Parallel
		concurrency::parallel_for_each(&result.data_vec[0], &result.data_vec[0] + result.data_vec.size, [&](CPoint3F& p) {
			Project_(p);
			});
#else
		auto p = &result.data_vec[0];
#ifdef Pragma_Parallel
#pragma omp parallel for
#endif // Pragma_Parallel

		for (int i = 0; i < result.data_vec.size; i++)
			Project_(p[i]);

#endif // PPL_Parallel

		/*===============AMP=================*/
		//MyGraphicFastVector result(v);
		//auto near_vec = this->near_vec;
		//auto nearDepth = this->nearDepth;
		//auto canvaWidth = this->canvaWidth;
		//auto canvaHeight = this->canvaHeight;
		//concurrency::array_view<CPoint3D> point_view(result.data_vec.size, result.data_vec.arr.arr);
		//concurrency::parallel_for_each(point_view.extent, [=](concurrency::index<1> idx) restrict(amp) {
		//	CPoint3D tmp = point_view[idx];
		//	ViewZoneProject(near_vec, nearDepth, canvaWidth, canvaHeight, tmp);
		//	point_view[idx] = tmp;
		//	});
		//point_view.synchronize();
		return std::move(result);
	}
	//�и�ͶӰ
	inline Graphic_Polygon InciseProject(const Graphic_Polygon& g)const {
		return std::move(ZoneIncise2D(Project(ZoneIncise3D(g))));
	}
	inline MyGraphicVector InciseProject(const MyGraphicVector& v)const {
		return std::move(/*ZoneIncise2D*/(Project(ZoneIncise3D(v))));
	}
	inline MyGraphicFastVector InciseProject(const MyGraphicFastVector& v)const {
		return std::move(/*ZoneIncise2D*/(Project(/*ZoneIncise3D*/(v))));
	}

	bool CheckPoint(const Point3F& p);

};

