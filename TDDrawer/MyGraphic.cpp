/*
	��    ��: Wheat
	����޸�: 2019.03.03
*/
#include "MyGraphic.h"
#include "Canva.h"
#include <ppl.h>

using namespace std;

//#define DEBUG


#define SOLVE_R(c1,c2,x) (DWORD(int((c2 & 0xff) * (x)) + int((c1 & 0xff) * (1 - (x)))))
#define SOLVE_G(c1,c2,x) (DWORD(int(((c2 & 0xff00) >> 8) * (x))+ int(((c1 & 0xff00) >> 8) * (1 - (x)))) << 8)
#define SOLVE_B(c1,c2,x) (DWORD(int(((c2 & 0xff0000) >>16)* (x)) + int(((c1 & 0xff0000) >> 16) * (1 - (x)))) << 16)
#define SOLVE_COLOR(color1,color2,x) DWORD(SOLVE_R(color1, color2, x) | SOLVE_G(color1, color2, x) | SOLVE_B(color1, color2, x))

inline CPoint3F Plane_Line(const float& A, const float& B, const float& C, const float& D, const CPoint3F& p1, const CPoint3F& p2) {
	CPoint3F interP;
	MyMat<float> mat1(3);
	MyMat<float> mat2(3, 1);
	float a = p1.x, b = p1.y, c = p1.z;
	float da = a - p2.x, db = b - p2.y, dc = c - p2.z;
	if (max(abs(da), max(abs(db), abs(dc))) == abs(da)) {
		mat1 = {
			A	, B		, C		,
			db	, -da	, 0		,
			-dc	, 0		, da
		};
		mat2 = {
				-D			,
			a * db - b * da ,
			c * da - a * dc
		};
	}
	else if (max(abs(da), max(abs(db), abs(dc))) == abs(db)) {
		mat1 = {
			A	, B		, C		,
			db	, -da	, 0		,
			0	, dc	, -db
		};
		mat2 = {
				-D			,
			a * db - b * da ,
			b * dc - c * db
		};
	}
	else {
		mat1 = {
			A	, B		, C		,
			-dc	, 0		, da	,
			0	, dc	, -db
		};
		mat2 = {
				-D			,
			c * da - a * dc ,
			b * dc - c * db
		};
	}
	MyMat<float> result = std::move(mat1.Inverse() * mat2);
#ifdef DEBUG
	if (result.Empty()) cout << "Plane_Line Warning: ϵ�����󲻿���!" << endl;
	if (Euclidean_distance3D(p2, p1) == 0) cout << "Plane_Line Warning: ���ô�����Ŀ����غ�!" << endl;
#endif // DEBUG
	interP.x = result.arr[0], interP.y = result.arr[1], interP.z = result.arr[2];
	interP.color = SOLVE_COLOR(p1.color, p2.color, Euclidean_distance3D(interP, p1) / Euclidean_distance3D(p2, p1));
	//cout << " *** " << p1 << " " << p2 << " " << interP << " ||| " << float(interP.x - p1.x) / (p2.x - p1.x) << endl;
	return interP;
}
inline CPoint3F Line_Line(const float& A, const float& B, const float& C, const CPoint3F& p1, const CPoint3F& p2) {
	CPoint3F interP;
	MyMat<float> mat1(2);
	MyMat<float> mat2(2, 1);
	float a = p1.x, b = p1.y;
	float da = a - p2.x, db = b - p2.y;
	mat1 = {
		A	, B		,
		db	, -da	,
	};
	mat2 = {
		-C				,
		a*db - b * da	,
	};
	MyMat<float> result = std::move(mat1.Inverse() * mat2);
#ifdef DEBUG
	if (result.Empty()) cout << "Line_Line Warning: ϵ�����󲻿���!" << endl;
	if (Euclidean_distance2D(p2, p1) == 0) cout << "Line_Line Warning: ���ô�����Ŀ����غ�!" << endl;
#endif // DEBUG
	interP.x = result.arr[0], interP.y = result.arr[1];
	float distanceRate = Euclidean_distance2D(interP, p1) / Euclidean_distance2D(p2, p1);
	interP.z = p1.z + (p2.z - p1.z) * distanceRate;
	interP.color = SOLVE_COLOR(p1.color, p2.color, distanceRate);
	return interP;
}


inline CPoint3F InterPoint(const float* parameter, const int dimension, const CPoint3F& p1, const CPoint3F& p2) {
	if (dimension == 4) return Plane_Line(parameter[0], parameter[1], parameter[2], parameter[3], p1, p2);
	else return Line_Line(parameter[0], parameter[1], parameter[2], p1, p2);
}
MyVector<CPoint3F> InciseCore(const MyMat<float>& parameter, const MyArray<CPoint3F>& arr) {
#define CHECK_POINT3D_BY_PLANE(p,A,B,C,D) ((p).x * (A) + (p).y * (B) + (p).z * (C) + (D))
#define CHECK_POINT2D_BY_LINE(p,A,B,C) ((p).x * (A) + (p).y * (B) + (C))
#define CHECK_POINT(demension,p,para) ((demension) == 4 ? CHECK_POINT3D_BY_PLANE(p, para[0], para[1], para[2], para[3]) :\
										 CHECK_POINT2D_BY_LINE(p, para[0], para[1], para[2]))
	if (arr.len == 0) return MyVector<CPoint3F>(0);
	if (arr.len == 1) {
		MyVector<CPoint3F> result;
		for (size_t i = 0; i < parameter.rows; i++) {
			auto para = parameter[i];
			if (CHECK_POINT(parameter.cols, arr[0], para) < 0) return std::move(result);
		}
		result.push_back(arr[0]);
		return std::move(result);
	}

	MyVector<CPoint3F> pointVec1(arr), pointVec2(arr.len), *rawPoints = &pointVec1, *newPoints = &pointVec2;

	for (size_t i = 0; i < parameter.rows; i++) {
		//���ϵ��ǣ�����0Ϊ���ϵ㣬����0Ϊ�ٽ�㣬С��0Ϊ�����ϵ�
		MyArray<float> conformFlags(rawPoints->size);

		//���ϵ������鸳ֵ
		auto para = parameter[i];
		for (size_t k = 0; k < rawPoints->size; k++) {
			conformFlags[k] = CHECK_POINT(parameter.cols, (*rawPoints)[k], para);
		}

		//���ݷ��ϵ����������·��ϵ�
		newPoints->Clear();
		for (size_t k = 0; k < rawPoints->size; k++) {
			if (conformFlags[k] > 0) {														//��ǰΪ���ϵ�
				newPoints->push_back((*rawPoints)[k]);
				if (conformFlags.CirIndex(k + 1) <= 0) {									//����һ���㲢�Ƿ��ϵ�
					int nextConform = k + 2;
					for (; conformFlags.CirIndex(nextConform) <= 0; nextConform++);			//һֱ��ǰ��ѭ����������һ�����ϵ�λ��

					if (conformFlags.CirIndex(k + 1) == 0)									//1 0 # 1���
						newPoints->push_back((*rawPoints).CirIndex(k + 1));
					else																	//1 -1 # 1���
						newPoints->push_back(InterPoint(para, parameter.cols,
						(*rawPoints)[k], (*rawPoints).CirIndex(k + 1)));

					if (arr.len == 2) break;													//1 #������Ѵ��������е㣬�˳�
					if (nextConform - k <= 2 && conformFlags.CirIndex(k + 1) == 0) {		//1 0 1���
						k = nextConform - 1;											//��һѭ��
						continue;
					}

					if (conformFlags.CirIndex(nextConform - 1) == 0)						//1 # 0 1���
						newPoints->push_back((*rawPoints).CirIndex(nextConform - 1));
					else																	//1 # -1 1���
						newPoints->push_back(InterPoint(para, parameter.cols,
						(*rawPoints).CirIndex(nextConform - 1), (*rawPoints).CirIndex(nextConform)));
					k = nextConform - 1;
					continue;
				}
			}
		}
		swap(rawPoints, newPoints);
	}
	return std::move(*rawPoints);
}

std::ostream & operator<<(std::ostream & o, const Graphic_Polygon & g) {
	for (size_t i = 0; i < g.Size(); i++) o << "P" << i << g[i] << " ";
	return o;
}

class VectorCanva : public CanvaInterface, public MyVector<CPoint3F> {
	//����λ����й���
public:

	inline void Pixel(int x, int y, float depth, COLORREF color = 0) {
		push_back(CPoint3F(x, y, depth, color));
	}
	inline void Draw(const Brush& g) { ; }
	inline void Draw(const MyGraphicVector& v) { ; }
	inline size_t Rows() { return 0; }
	inline size_t Cols() { return 0; }
	inline void Clean() { ; }
};
inline int Round(float x) { return x >= 0 ? (x + 0.9) : (x - 0.9); }
inline void DrawLine(CanvaInterface & canva, const CPoint3F& p1, const CPoint3F& p2) {
	float delta_depth, delta_x, delta_y, depth, x, y;
	int dx, dy, steps;
	dx = Round(p2.x - p1.x);
	dy = Round(p2.y - p1.y);
	steps = max(abs(dx), abs(dy));

	if (steps > 10000) steps = 10000;
	delta_depth = (p2.z - p1.z) / steps;
	delta_x = (float)dx / steps;
	delta_y = (float)dy / steps;
	depth = p1.z;
	x = p1.x;
	y = p1.y;
	canva.Pixel(x, y, p1.z, p1.color);
	for (int k = 1; k <= steps; k++) {
		depth += delta_depth;
		x += delta_x;
		y += delta_y;
		canva.Pixel(x, y, depth, SOLVE_COLOR(p1.color, p2.color, (float)k / steps));
	}
}
inline void DrawPolygon(CanvaInterface & canva, const MyArray<CPoint3F>& arr) {
	switch (arr.Len()) {
	case 0:
#ifdef DEBUG
		cout << "Draw Error: �հ׵�!" << endl;
#endif // DEBUG
		return;
	case 1:
		canva.Pixel(arr[0].x, arr[0].y, arr[0].z, arr[0].color);
		return;
	case 2:
		DrawLine(canva, arr[0], arr[1]);
		return;
	default:

		int minX = arr[0].x, maxX = minX, minY = arr[0].y, maxY = minY;
		for (int i = arr.Len() - 1; i >= 0; i--) {
			minX = min(arr[i].x, minX), maxX = max(arr[i].x, maxX);
			minY = min(arr[i].y, minY), maxY = max(arr[i].y, maxY);
		}
		if (minX == maxX && minY == maxY) return;
		else if (minX == maxX || minY == maxY) {
			for (int i = 0; i < arr.Len(); i++) DrawLine(canva, arr[i], arr.CirIndex(i));
			return;
		}

		VectorCanva points;
		for (int i = arr.Len() - 1; i >= 0; i--)  DrawLine(points, arr[i], arr.CirIndex(i + 1));

		if ((maxX - minX) <= (maxY - minY)) {
#define major x
#define minor y
			points.ShellSort([](const CPoint3F& p1, const CPoint3F& p2) { return int(p1.major - p2.major); });
			for (size_t i = 0; i < points.size;) {
				CPoint3F least = points[i];
				CPoint3F largest = least;
				size_t j = i + 1;
				for (; j < points.size && points[i].major == points[j].major; j++) {
					if (points[j].minor < least.minor) least = points[j];
					if (points[j].minor > largest.minor) largest = points[j];
				}
				DrawLine(canva, least, largest);
				i = j;
			}
		}
		else {
#define major y
#define minor x
			points.ShellSort([](const CPoint3F& p1, const CPoint3F& p2) { return int(p1.major - p2.major); });
			for (size_t i = 0; i < points.size;) {
				CPoint3F least = points[i];
				CPoint3F largest = least;
				size_t j = i + 1;
				for (; j < points.size && points[i].major == points[j].major; j++) {
					if (points[j].minor < least.minor) least = points[j];
					if (points[j].minor > largest.minor) largest = points[j];
				}
				DrawLine(canva, least, largest);
				i = j;
			}
		}
		return;
	}
}
void Graphic_Polygon::Draw(CanvaInterface & canva) const {
	DrawPolygon(canva, arr);
}

Graphic_Polygon Graphic_Polygon::Incise(const MyMat<float>& parameter) const {
	if (!Size()) return Graphic_Polygon();
	auto points = InciseCore(parameter,arr);
	if (points.size) return Graphic_Polygon(&points[0], points.size);
	return Graphic_Polygon();
}

void Graphic_Polygon_Virtual::Draw(CanvaInterface & canva, CPoint3F * arr) const {
	if (!arr) return;
	MyArray<CPoint3F> tmp;
	tmp.Load(arr + index, size);
	DrawPolygon(canva, tmp);
}

Graphic_Polygon Graphic_Polygon_Virtual::Incise(const MyMat<float>& parameter, CPoint3F * arr) const {
	if (!size || !arr) return Graphic_Polygon();
	MyArray<CPoint3F> tmp;
	tmp.Load(arr + index, size);

	auto points = InciseCore(parameter, tmp);
	if (points.size) return Graphic_Polygon(&points[0], points.size);
	return Graphic_Polygon();
}

