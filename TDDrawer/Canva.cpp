#include "Canva.h"
#include <iostream>

using namespace std;

#define DEPTH_INF (float)(3.39e+38)
void Canva::Clean() { img.Set(BGR(backcolor)); depthMat.Set(DEPTH_INF); }


void Canva::Line(int xa, int ya, int xb, int yb, COLORREF color) {
	float delta_x, delta_y, x, y;
	int dx, dy, steps;
	dx = xb - xa;
	dy = yb - ya;
	steps = max(abs(dx), abs(dy));

	if (steps > 10000) steps = 10000;
	delta_x = (float)dx / steps;
	delta_y = (float)dy / steps;
	x = xa;
	y = ya;
	Pixel(x, y, color);
	for (int k = 1; k <= steps; k++) {
		x += delta_x;
		y += delta_y;
		Pixel(x, y, color);
	}
}

void Canva::Triangle(POINT p1, POINT p2, POINT p3, COLORREF color, bool fill) {
	if (!fill) {
		Line(p1.x, p1.y, p2.x, p2.y, color);
		Line(p1.x, p1.y, p3.x, p3.y, color);
		Line(p2.x, p2.y, p3.x, p3.y, color);
		return;
	}

	if (p1.y > p2.y) swap(p1, p2);
	if (p2.y > p3.y) {
		if (p1.y > p3.y) swap(p1, p3);
		swap(p2, p3);
	}

	int minY = p1.y;
	int height = p3.y - p1.y;

	p1.y -= minY, p2.y -= minY, p3.y -= minY;

	int *leftArr = new int[height + 1];
	int *rightArr = new int[height + 1];

#define TRIANGLE_CORE(p1,p2,sideArr) {			\
float delta_x, x;								\
int dx, dy;										\
dx = p2.x - p1.x;								\
dy = p2.y - p1.y;								\
												\
delta_x = (float)dx / dy;						\
x = p1.x;										\
sideArr[p1.y] = x;								\
for (int k = p1.y + 1; k <= p2.y; k++) {		\
	x += delta_x;								\
	sideArr[k] = x;								\
}}

	TRIANGLE_CORE(p1, p2, leftArr);
	TRIANGLE_CORE(p2, p3, leftArr);
	TRIANGLE_CORE(p1, p3, rightArr);

	for (int i = 0; i <= height; i++) {
		int y = i + minY;
		if (y < 0 || y >= rows) continue;

		int leftSide = min(leftArr[i], rightArr[i]);
		int rightSide = max(leftArr[i], rightArr[i]);
		if (leftSide < 0) leftSide = 0;
		if (rightSide >= cols) rightSide = cols - 1;

		for (int j = leftSide; j <= rightSide; j++) {
			Pixel_core(j, y, color);
		}
	}

	delete[] leftArr;
	delete[] rightArr;
}

void Canva::Line_c(int xa, int ya, int xb, int yb, COLORREF color_a, COLORREF color_b) {
#define SOLVE_R(c1,c2,x) (DWORD(int((c2 & 0xff) * (x)) + int((c1 & 0xff) * (1 - (x)))))
#define SOLVE_G(c1,c2,x) (DWORD(int(((c2 & 0xff00) >> 8) * (x))+ int(((c1 & 0xff00) >> 8) * (1 - (x)))) << 8)
#define SOLVE_B(c1,c2,x) (DWORD(int(((c2 & 0xff0000) >>16)* (x)) + int(((c1 & 0xff0000) >> 16) * (1 - (x)))) << 16)
#define SOLVE_COLOR(color1,color2,x) DWORD(SOLVE_R(color1, color2, x) | SOLVE_G(color1, color2, x) | SOLVE_B(color1, color2, x))
	float delta_x, delta_y, x, y;
	int dx, dy, steps;
	dx = xb - xa;
	dy = yb - ya;
	steps = max(abs(dx), abs(dy));

	if (steps > 10000) steps = 10000;
	delta_x = (float)dx / steps;
	delta_y = (float)dy / steps;
	x = xa;
	y = ya;
	Pixel(x, y, color_a);
	for (int k = 1; k <= steps; k++) {
		x += delta_x;
		y += delta_y;
		Pixel(x, y, SOLVE_COLOR(color_a, color_b, (float)k / steps));
	}
}
void Canva::Line_cd(int xa, int ya, int xb, int yb, float depthA, float depthB, COLORREF color_a, COLORREF color_b) {
#define SOLVE_R(c1,c2,x) (DWORD(int((c2 & 0xff) * (x)) + int((c1 & 0xff) * (1 - (x)))))
#define SOLVE_G(c1,c2,x) (DWORD(int(((c2 & 0xff00) >> 8) * (x))+ int(((c1 & 0xff00) >> 8) * (1 - (x)))) << 8)
#define SOLVE_B(c1,c2,x) (DWORD(int(((c2 & 0xff0000) >>16)* (x)) + int(((c1 & 0xff0000) >> 16) * (1 - (x)))) << 16)
#define SOLVE_COLOR(color1,color2,x) DWORD(SOLVE_R(color1, color2, x) | SOLVE_G(color1, color2, x) | SOLVE_B(color1, color2, x))
	float delta_x, delta_y, x, y;
	int dx, dy, steps;
	dx = xb - xa;
	dy = yb - ya;
	steps = max(abs(dx), abs(dy));

	if (steps > 10000) steps = 10000;
	delta_x = (float)dx / steps;
	delta_y = (float)dy / steps;
	x = xa;
	y = ya;
	Pixel(x, y, color_a);
	for (int k = 1; k <= steps; k++) {
		x += delta_x;
		y += delta_y;
		Pixel(x, y, SOLVE_COLOR(color_a, color_b, (float)k / steps));
	}
}

void Canva::Triangle_c(POINT p1, POINT p2, POINT p3, COLORREF color1, COLORREF color2, COLORREF color3, bool fill) {
	if (!fill) {
		Line_c(p1.x, p1.y, p2.x, p2.y, color1, color2);
		Line_c(p1.x, p1.y, p3.x, p3.y, color1, color3);
		Line_c(p2.x, p2.y, p3.x, p3.y, color2, color3);
		return;
	}

	if (p1.y > p2.y) { swap(p1, p2); swap(color1, color2); }
	if (p2.y > p3.y) {
		if (p1.y > p3.y) { swap(p1, p3); swap(color1, color3); }
		swap(p2, p3);
		swap(color2, color3);
	}

	int minY = p1.y;
	int height = p3.y - p1.y;

	p1.y -= minY, p2.y -= minY, p3.y -= minY;

	int *leftArr = new int[height + 1];
	DWORD *leftColorArr = new DWORD[height + 1];
	int *rightArr = new int[height + 1];
	DWORD *rightColorArr = new DWORD[height + 1];

#define TRIANGLE_C_CORE(p1,p2,c1,c2,sideArr,sideColorArr) {		\
float delta_x, x;												\
int dx, dy;														\
dx = p2.x - p1.x;												\
dy = p2.y - p1.y;												\
																\
delta_x = (float)dx / dy;										\
x = p1.x;														\
sideArr[p1.y] = x;												\
sideColorArr[p1.y] = c1;										\
for (int k = p1.y + 1; k <= p2.y; k++) {						\
	x += delta_x;												\
	sideArr[k] = x;												\
	sideColorArr[k] = SOLVE_COLOR(c1, c2, (float)(k-p1.y) / dy);\
}}

	TRIANGLE_C_CORE(p1, p2, color1, color2, leftArr, leftColorArr);
	TRIANGLE_C_CORE(p2, p3, color2, color3, leftArr, leftColorArr);
	TRIANGLE_C_CORE(p1, p3, color1, color3, rightArr, rightColorArr);

	int y;
	int leftSide, rightSide, length;
	COLORREF leftColor, rightColor;
	for (int i = 0; i <= height; i++) {
		y = i + minY;
		if (y < 0 || y >= rows) continue;

		leftSide = leftArr[i];
		rightSide = rightArr[i];
		leftColor = leftColorArr[i];
		rightColor = rightColorArr[i];
		if (leftSide > rightSide) { swap(leftSide, rightSide); swap(leftColor, rightColor); }
		length = rightSide - leftSide;
		for (int j = leftSide; j <= rightSide; j++) {
			Pixel(j, y, SOLVE_COLOR(leftColor, rightColor, float(j - leftSide) / length));
		}

		//Line_c(leftArr[i], y, rightArr[i], y, leftColorArr[i], rightColorArr[i]);
	}

	delete[] leftArr;
	delete[] rightArr;
}

struct Graphic_Triangle_Temp {
	int x;
	float depth;
	COLORREF color;
};

void Canva::Triangle_cd(CPoint3F cp1, CPoint3F cp2, CPoint3F cp3) {
	//if (!fill) {
	//	Line_c(p1.x, p1.y, p2.x, p2.y, color1, color2);
	//	Line_c(p1.x, p1.y, p3.x, p3.y, color1, color3);
	//	Line_c(p2.x, p2.y, p3.x, p3.y, color2, color3);
	//	return;
	//}
	if (cp1.y > cp2.y) swap(cp1, cp2);
	if (cp2.y > cp3.y) {
		if (cp1.y > cp3.y)  swap(cp1, cp3);
		swap(cp2, cp3);
	}

	int minY = cp1.y;
	int height = cp3.y - cp1.y;

	cp1.y -= minY, cp2.y -= minY, cp3.y -= minY;

	Graphic_Triangle_Temp* leftArr = new Graphic_Triangle_Temp[height + 1];
	Graphic_Triangle_Temp* rightArr = new Graphic_Triangle_Temp[height + 1];
	//int *leftArr = new int[height + 1];
	//COLORREF *leftColorArr = new COLORREF[height + 1];
	//int *rightArr = new int[height + 1];
	//COLORREF *rightColorArr = new COLORREF[height + 1];

#define TRIANGLE_DRAW_CORE(cp1,cp2,sideArr) {								\
float delta_depth, delta_x, depth, x;										\
int dx, dy;																	\
delta_depth = cp2.z - cp1.z;												\
dx = cp2.x - cp1.x;															\
dy = cp2.y - cp1.y;															\
																			\
delta_x = (float)dx / dy;													\
x = cp1.x;																	\
depth = cp1.z;																\
sideArr[int(cp1.y)].x = x;													\
sideArr[int(cp1.y)].depth = depth;											\
sideArr[int(cp1.y)].color = cp1.color;										\
for (int k = cp1.y + 1; k <= cp2.y; k++) {									\
x += delta_x;																\
sideArr[k].x = x;															\
depth += delta_depth;														\
sideArr[k].depth = depth;													\
sideArr[k].color = SOLVE_COLOR(cp1.color, cp2.color, (float)(k-cp1.y) / dy);\
}}

	TRIANGLE_DRAW_CORE(cp1, cp2, leftArr);
	TRIANGLE_DRAW_CORE(cp2, cp3, leftArr);
	TRIANGLE_DRAW_CORE(cp1, cp3, rightArr);

	int y;
	int length;
	Graphic_Triangle_Temp leftSide, rightSide;
	for (int i = max(0, -minY); i <= height; i++) {
		y = i + minY;
		if (y >= rows) break;

		leftSide = leftArr[i];
		rightSide = rightArr[i];
		if (leftSide.x > rightSide.x) { swap(leftSide, rightSide); }
		length = rightSide.x - leftSide.x;
		for (int j = leftSide.x; j <= rightSide.x; j++) {
			Pixel(j, y,
				leftSide.depth + (rightSide.depth - leftSide.depth) * float(j - leftSide.x) / length,
				SOLVE_COLOR(leftSide.color, rightSide.color, float(j - leftSide.x) / length));
		}
		//Line_c(leftArr[i].x, y, rightArr[i].x, y, leftArr[i].color, rightArr[i].color);
	}

	delete[] leftArr;
	delete[] rightArr;
}

void Canva::GetScreenRECT() { GetClipBox(GetImageHDC(), &screen); }

void Canva::Paint() {
	int left = max(area.left, screen.left) - area.left;
	int right = min(area.right, screen.right) - area.left;
	int top = max(area.top, screen.top) - area.top;
	int bottom = min(area.bottom, screen.bottom) - area.top;

	int width = right - left;
	int height = bottom - top;

	if (width <= 0 || height <= 0) return;

	DWORD *p = pMem + (area.top + top) * screen.right;
	for (int i = top; i < bottom; i++) {
		memcpy(p + (area.left + left), img[i] + left, sizeof(DWORD) * width);
		p += screen.right;
	}
}

void Canva::Flush() { FlushBatchDraw(); }

Canva::Canva(RECT area) : area(area), rows(area.bottom - area.top), cols(area.right - area.left),
img(area.bottom - area.top, area.right - area.left),
depthMat(area.bottom - area.top, area.right - area.left) {
	GetScreenRECT();
	Clean();
}