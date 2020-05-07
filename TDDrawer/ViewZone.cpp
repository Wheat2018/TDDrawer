#include "ViewZone.h"
#include "CoorTransition.h"
#include "MyMat.hpp"

#define INCISE_PARAMETER {										\
	0		,		0		,		1		, -near_vec.z	,	\
	0		,		0		,		-1		, far_vec.z		,	\
-near_vec.z	,		0		, near_vec.x	,		0		,	\
near_vec.z	,		0		, near_vec.x	,		0		,	\
	0		, -near_vec.z	, near_vec.y	,		0		,	\
	0		, near_vec.z	, near_vec.y	,		0			\
}
#define INC_PAR_NUM 6
ViewZone::ViewZone(const Vector3F &near_vec, float viewDepth) :inciseParameter3D(INC_PAR_NUM, 4), inciseParameter2D(4, 3) {
	//组一
	this->near_vec = near_vec;
	this->viewDepth = viewDepth;

	//组二
	angleX = 2 * atan(near_vec.x / near_vec.z) * 180 / PI;
	angleY = 2 * atan(near_vec.y / near_vec.z) * 180 / PI;
	nearDepth = near_vec.z;
	farDepth = nearDepth + viewDepth;

	//被约束变量
	far_vec = (farDepth / nearDepth) * near_vec;
	inciseParameter3D = INCISE_PARAMETER;
	inciseParameter2D = {
		0		,		1		,		0			,
		0		,		-1		,		canvaHeight	,
		1		,		0		,		0			,
		-1		,		0		,		canvaWidth
	};
}

ViewZone::ViewZone(float angleX, float angleY, float nearDepth, float farDepth) :inciseParameter3D(INC_PAR_NUM, 4), inciseParameter2D(4, 3) {
	//组二
	this->angleX = angleX;
	this->angleY = angleY;
	this->nearDepth = nearDepth;
	this->farDepth = farDepth;

	//组一
	near_vec = Vector3F(nearDepth * tan(angleX / 2 * PI / 180), nearDepth * tan(angleY / 2 * PI / 180), nearDepth);
	viewDepth = farDepth - nearDepth;

	//被约束变量
	far_vec = (farDepth / nearDepth) * near_vec;
	inciseParameter3D = INCISE_PARAMETER;
	inciseParameter2D = {
		0		,		1		,		0			,
		0		,		-1		,		canvaHeight	,
		1		,		0		,		0			,
		-1		,		0		,		canvaWidth
	};
}

MyGraphicVector ViewZone::ZoneIncise3D(const MyGraphicVector & v) const {
	MyGraphicVector result(v.size);
	//for (size_t i = 0; i < v.size; i++) {
	//	auto tmp = ZoneIncise3D(v[i]);
	//	if (tmp.Size() > 0) result.push_back(std::move(tmp));
	//}
	result.Fill();
	concurrency::parallel_for(0, v.size, [&](int idx) {
		result[idx] = std::move(ZoneIncise3D(v[idx]));
		});
	//concurrency::combinable<MyGraphicVector> com;
	//concurrency::parallel_for(0, v.size, [&](int idx) {
	//	auto tmp = ZoneIncise3D(v[idx]);
	//	if (tmp.Size() > 0) com.local().push_back(std::move(tmp));
	//	});
	//com.combine_each([&](MyGraphicVector& local) {
	//	//std::cout << local.size << std::endl;
	//	result.join_back(local);
	//	});

	return std::move(result);
}

MyGraphicVector ViewZone::ZoneIncise2D(const MyGraphicVector & v) const {
	MyGraphicVector result(v.size);
	//for (size_t i = 0; i < v.size; i++) {
	//	auto tmp = ZoneIncise2D(v[i]);
	//	if (tmp.Size() > 0) result.push_back(std::move(tmp));
	//}
	result.Fill();
	concurrency::parallel_for(0, v.size, [&](int idx) {
		result[idx] = std::move(ZoneIncise2D(v[idx]));
		});	
	//concurrency::combinable<MyGraphicVector> com;
	//concurrency::parallel_for(0, v.size, [&](int idx) {
	//	auto tmp = ZoneIncise2D(v[idx]);
	//	if (tmp.Size() > 0) com.local().push_back(std::move(tmp));
	//	});
	//com.combine_each([&](MyGraphicVector& local) {
	//	//std::cout << local.size << std::endl;
	//	result.join_back(local);
	//	});
	return std::move(result);
}


bool ViewZone::CheckPoint(const Point3F & p) {
	if (p.z < nearDepth || p.z > farDepth) return false;
	Vector3F view = (p.z / nearDepth) * near_vec;
	return abs(p.x) <= view.x && abs(p.y) <= view.y;
}