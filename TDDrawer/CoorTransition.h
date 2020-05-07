#pragma once
#include "MyGraphic.h"
#include <ppl.h>
#include <amp.h>


class CoorTransition {
private:

public:

	Vector3F I = { 1,0,0 }, J = { 0,1,0 }, K = { 0,0,1 };		//摄像机方向信息，其中Z轴表景深，即视野方向
	Vector3F X, Y, Z;
	Vector3F position = { 0,0,0 };								//摄像机位置信息

	CoorTransition() { CalculateXYZ(); }
	
	void RotateHorizontal(float angle) { I.Rotate(J, angle).Unitize(); K.Rotate(J, angle).Unitize(); CalculateXYZ(); }
	void RotateVertical(float angle) { J.Rotate(I, angle).Unitize(); K.Rotate(I, angle).Unitize(); CalculateXYZ(); }
	void RotateByView(float angle) { I.Rotate(K, angle).Unitize(); J.Rotate(K, angle).Unitize(); CalculateXYZ(); }

	void CalculateXYZ() { X = Vector3F(I.x, J.x, K.x).Unitize(), Y = Vector3F(I.y, J.y, K.y).Unitize(), Z = Vector3F(I.z, J.z, K.z).Unitize(); }
	
	void MoveByRotateInPoint(Point3F pos_rot_by, Vector3F n_rot_by, float angle);
	void MoveTo(Vector3F position) { this->position = position; }
	void LookTo(Vector3F target);

	void LookTo_Fix_I_in_Plane(Vector3F target, Vector3F plane);
	void LookTo_Fix_J_in_Plane(Vector3F target, Vector3F plane);

	inline static void CoorTran(Point3F& rawCoor,
		const Vector3F& position,
		const Vector3F& X,
		const Vector3F& Y,
		const Vector3F& Z) restrict(amp, cpu)
	{
		Point3F p = rawCoor - position;
		rawCoor = p.x * X + p.y * Y + p.z * Z;
	}
	inline CPoint3F Tran(const CPoint3F& rawCoor)const {
		//调用该函数之前确保先调用过CalculateXYZ;
		CPoint3F nowCoor(rawCoor);
		CoorTran(nowCoor, position, X, Y, Z);
		return nowCoor;
	}
	inline void Tran_( CPoint3F& rawCoor)const {
		//调用该函数之前确保先调用过CalculateXYZ;
		CoorTran(rawCoor, position, X, Y, Z);
	}
	static inline void InvTran(Point3F& nowCoor,
		const Point3F& position,
		const Point3F& I,
		const Point3F& J,
		const Point3F& K) restrict(amp, cpu)
	{
		nowCoor = nowCoor.x * I + nowCoor.y * J + nowCoor.z * K + position;
	}
	inline CPoint3F InvTran(const CPoint3F& nowCoor)const {
		//调用该函数之前确保先调用过CalculateXYZ;
		CPoint3F rawCoor(nowCoor);
		InvTran(rawCoor, position, I, J, K);
		return rawCoor;
	}
	inline void InvTran(CPoint3F& nowCoor)const {
		//调用该函数之前确保先调用过CalculateXYZ;
		InvTran(nowCoor, position, I, J, K);
	}

	inline Graphic_Polygon TranGraphics(const Graphic_Polygon& g)const {
		Graphic_Polygon result(g.Size());
		for (int i = g.Size() - 1; i >= 0; i--) result[i] = Tran(g[i]);
		return std::move(result);
	}
	inline MyGraphicVector TranGraphics(const MyGraphicVector& rawGraphics)const {
		MyGraphicVector nowGraphics(rawGraphics.size);
		nowGraphics.Fill();

#ifdef PPL_Parallel

		concurrency::parallel_for(0, rawGraphics.size, [&](int idx) {
			nowGraphics[idx]=std::move(TranGraphics(rawGraphics[idx]));
			});

#else

#ifdef Pragma_Parallel
#pragma omp parallel for
#endif // Pragma_Parallel

		for (int idx = 0; idx < rawGraphics.size; idx++)
			nowGraphics[idx] = std::move(TranGraphics(rawGraphics[idx]));

#endif // PPL_Parallel

		return std::move(nowGraphics);
	}
	inline MyGraphicFastVector TranGraphics(const MyGraphicFastVector& rawGraphics)const {
		MyGraphicFastVector result(rawGraphics);

#ifdef PPL_Parallel
		concurrency::parallel_for_each(&result.data_vec[0], &result.data_vec[0] + result.data_vec.size, [&](CPoint3F& p) {
			Tran_(p);
			});
#else
		auto p = &result.data_vec[0];
#ifdef Pragma_Parallel
#pragma omp parallel for
#endif // Pragma_Parallel

		for (int i = 0; i < result.data_vec.size; i++)
			Tran_(p[i]);

#endif // PPL_Parallel

		/*===============AMP=================*/
		//MyGraphicFastVector result(rawGraphics);
		//auto position = this->position;
		//auto X = this->X;
		//auto Y = this->Y;
		//auto Z = this->Z;
		//concurrency::array_view<CPoint3F> point_view(result.data_vec.size, result.data_vec.arr.arr);
		//concurrency::parallel_for_each(point_view.extent, [=](concurrency::index<1> idx) restrict(amp) {
		//	CPoint3F tmp = point_view[idx];
		//	CoorTran(tmp, position, X, Y, Z);
		//	point_view[idx] = tmp;
		//	});
		//point_view.synchronize();
		return std::move(result);
	}

};

