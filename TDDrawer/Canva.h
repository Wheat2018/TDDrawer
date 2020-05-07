#pragma once
#include "MyMat.hpp"
#include <graphics.h>
#include "MyGraphic.h"
#include <ppl.h>
#include <amp.h>
#include <amp_math.h>
using namespace std;
#define PI 3.1415926535f
__interface CanvaInterface;

class Canva:public CanvaInterface {
private:
	MyMat<DWORD> img;

	int xMisreg = 0, yMisreg = 0;
public:
	MyMat<float> depthMat;

	RECT area;
	RECT screen;
	size_t rows = 0, cols = 0;

	COLORREF backcolor = 0;



	void Clean();

	inline size_t Rows() { return rows; }
	inline size_t Cols() { return cols; }

	inline void Pixel_core(int x, int y, COLORREF color = 0) { img[y][x] = BGR(color); }
	inline void Pixel(int x, int y, COLORREF color = 0) {
		x += xMisreg, y += yMisreg;
		if (y >= 0 && y < rows && x >= 0 && x < cols)
			Pixel_core(x, y, color);
	}
	inline void Pixel(int x, int y, float depth, COLORREF color = 0) {
		x += xMisreg, y += yMisreg;
		if (y >= 0 && y < rows && x >= 0 && x < cols /*&& depthMat[y][x] >= 0*/ && depthMat[y][x] > depth) {
			Pixel_core(x, y, color);
			depthMat[y][x] = depth;
		}
	}

	inline void Draw(const Brush& g) { g.Draw(*this); }
	inline void Draw(const Brush& g, int x, int y) {
		int tmpX = xMisreg, tmpY = yMisreg;
		xMisreg = x, yMisreg = y;
		Draw(g);
		xMisreg = tmpX, yMisreg = tmpY;
	}
	inline void Draw(const MyGraphicVector& v) { 
		//std::function<Canva()> f = [this]() { return area; };
		//concurrency::combinable<Canva> canva(f);
		//concurrency::parallel_for_each(&v[0], &v[v.size], [&](const Graphic_Polygon& g) {
		//	g.Draw(canva.local());
		//	});
		//canva.combine_each([&](Canva& local) {
		//	Mix(local);
		//	});
		for (int i = 0; i < v.size; i++) v[i].Draw(*this);
	}
	inline void Draw(const MyGraphicVector& v, int x , int y) {
		int tmpX = xMisreg, tmpY = yMisreg;
		xMisreg = x, yMisreg = y;
		Draw(v);
		xMisreg = tmpX, yMisreg = tmpY;
	}

	void Mix(const Canva& canva) {
		for (int i = depthMat.cols * depthMat.rows - 1; i >= 0 ; i--) {
			if (depthMat.arr[i] > canva.depthMat.arr[i]) {
				depthMat.arr[i] = canva.depthMat.arr[i];
				img.arr[i] = canva.img.arr[i];
			}
		}
	}

	void Line(int xa, int ya, int xb, int yb, COLORREF color);
	void Triangle(POINT p1, POINT p2, POINT p3, COLORREF color, bool fill = true);

	//渐变


	void Line_c(int xa, int ya, int xb, int yb, COLORREF color_a, COLORREF color_b);

	void Line_cd(int xa, int ya, int xb, int yb, float depthA, float depthB, COLORREF color_a, COLORREF color_b);
	void Triangle_c(POINT p1, POINT p2, POINT p3, COLORREF color1, COLORREF color2, COLORREF color3, bool fill = true);
	void Triangle_cd(CPoint3F p1, CPoint3F p2, CPoint3F p3);


#define RATE_R(c,x) (DWORD(int((c & 0xff) * (x))))
#define RATE_G(c,x) (DWORD(int(((c & 0xff00) >> 8) * (x))) << 8)
#define RATE_B(c,x) (DWORD(int(((c & 0xff0000) >>16)* (x))) << 16)
#define RATE_COLOR(color,x) DWORD(RATE_R(color, x) | RATE_G(color, x) | RATE_B(color, x))

	void Banding() {

		//for (size_t i = 1; i < rows - 1; i++) {
		//	for (size_t j = 1; j < cols - 1; j++) {

		//		
		//		float xtg1 = depthMat[i][j] - depthMat[i - 1][j];
		//		float xtg2 = depthMat[i + 1][j] - depthMat[i][j];
		//		float ytg1 = depthMat[i][j] - depthMat[i][j - 1];
		//		float ytg2 = depthMat[i][j + 1] - depthMat[i][j];
		//		float x_dAngle = abs(atan((xtg2 - xtg1) / (1 + xtg1 * xtg2)));
		//		float y_dAngle = abs(atan((ytg2 - ytg1) / (1 + ytg1 * ytg2)));
		//		float max_grad_change = 1 -  max(x_dAngle, y_dAngle) * 2 / PI;
		//		img[i][j] = RATE_COLOR(img[i][j], max_grad_change);
		//	}
		//}
		concurrency::array_view<DWORD, 2> img_view(rows, cols, img.arr);
		concurrency::array_view<float, 2> depth_view(rows, cols, depthMat.arr);

		concurrency::parallel_for_each(img_view.extent, [=](concurrency::index<2> idx) restrict(amp) {
			//if (idx[0]<1 || idx[0] > depth_view.extent[0] - 1 || idx[1]<1 || idx[1] > depth_view.extent[1] - 1) return;
			float xtg1 = depth_view(idx) - depth_view(idx[0] - 1, idx[1]);
			float xtg2 = depth_view(idx[0] + 1, idx[1]) - depth_view(idx);
			float ytg1 = depth_view(idx) - depth_view(idx[0], idx[1] - 1);
			float ytg2 = depth_view(idx[0], idx[1] + 1) - depth_view(idx);
			float x_dAngle;
			if (xtg1 * xtg2 > 0 && (
				(concurrency::fast_math::fabs(xtg1) >= concurrency::fast_math::fabs(xtg2) && xtg1 <= 0) ||
				(concurrency::fast_math::fabs(xtg1) < concurrency::fast_math::fabs(xtg2) && xtg2 >= 0)))
				x_dAngle = 0;
			else
				x_dAngle = concurrency::fast_math::fabs(concurrency::fast_math::atan((xtg2 - xtg1) / (1 + xtg1 * xtg2)));
			
			float y_dAngle;
			if (ytg1 * ytg2 > 0 && (
				(concurrency::fast_math::fabs(ytg1) >= concurrency::fast_math::fabs(ytg2) && ytg1 <= 0) ||
				(concurrency::fast_math::fabs(ytg1) < concurrency::fast_math::fabs(ytg2) && ytg2 >= 0)))
				y_dAngle = 0;
			else
				y_dAngle = concurrency::fast_math::fabs(concurrency::fast_math::atan((ytg2 - ytg1) / (1 + ytg1 * ytg2)));
			//int max_grad_change = 1.5 - max(x_dAngle, y_dAngle) * 2 / PI;
			float max_grad_change = 1.0 - max(x_dAngle, y_dAngle) * 2 / PI;
			if (max_grad_change >= 1) return;
			img_view(idx) = RATE_COLOR(img_view(idx), max_grad_change);
			});
		//img_view.synchronize();

	}

	/*====================以下函数依赖于图形库===================*/
	DWORD* const pMem = GetImageBuffer();
	void GetScreenRECT();
	void Paint();
	void Flush();
	/*===========================================================*/

	Canva(RECT area);
	//Canva& operator=(const Canva & canva) { 
	//	//xMisreg = canva.xMisreg;
	//	//yMisreg = canva.yMisreg;
	//	//area = canva.area;
	//	//backcolor = canva.backcolor;
	//	//cols = canva.cols;
	//	//depthMat = canva.depthMat;
	//	//img = canva.img;
	//	//canva.pMem;
	//	//rows = canva.rows;
	//	//screen = canva.screen;
	//	Mix(canva);
	//	return *this;
	//}
};

