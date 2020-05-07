#include <iostream>
#include "Canva.h"
#include <fstream>
#include "ArticalGraphic.h"

using namespace std;
int main() {
	MyTimer timer;
	
	initgraph(1100, 650, 1);
	RECT FunctionRect = { 150,10,950,610 };
	//initgraph(500, 500, 1);
	//RECT FunctionRect = { 0,0,500,500 };
	
	Sleep(10);
	BeginBatchDraw();

	Canva canva(FunctionRect);
	
	canva.backcolor = RGB(77, 111, 107);
	timer.Start();

	MyTimer timer2;
	timer2.Start();
	MyTimer timer3;
	timer3.Start();

	ArticalGraphic plane(0, nullptr, [](int, void**, ArticalGraphic* p_this){
		//p_this->View() = ViewZone(100,100,20,130);
		p_this->Camera().MoveTo({ 0,50,-100 });
		p_this->AddGraphc(Graphic_Polygon({ 0,0,0, RED }, { -50,0,100, GREEN }, { 50,0,100, BLUE }));
		p_this->AddGraphc(Graphic_Polygon({ 0,50,100, GREEN }, { 0,-50,100, RED }, { 0,0,0, CYAN }));
	});

//	ArticalGraphic plane(0, nullptr, [](int, void**, ArticalGraphic* p_this){
//		//p_this->View() = ViewZone(100,100,20,130);
//		p_this->Camera().MoveTo({ 0,20,10 });
//		p_this->Camera().LookTo({ 0,0,0 });
//		const COLORREF color1 = BLUE;
//		const COLORREF color2 = RED;
//#define SOLVE_R(c1,c2,x) (DWORD(int((c2 & 0xff) * (x)) + int((c1 & 0xff) * (1 - (x)))))
//#define SOLVE_G(c1,c2,x) (DWORD(int(((c2 & 0xff00) >> 8) * (x))+ int(((c1 & 0xff00) >> 8) * (1 - (x)))) << 8)
//#define SOLVE_B(c1,c2,x) (DWORD(int(((c2 & 0xff0000) >>16)* (x)) + int(((c1 & 0xff0000) >> 16) * (1 - (x)))) << 16)
//#define SOLVE_COLOR(color1,color2,x) DWORD(SOLVE_R(color1, color2, x) | SOLVE_G(color1, color2, x) | SOLVE_B(color1, color2, x))
//		float(*p)(float x, float y) = [](float x, float y) { return 10 * sin(sqrt(x * x + y * y)) / sqrt(x * x + y * y); };
//		const float d = 0.1;
//		const float range = 10;
//		for (float x = -range; x < range; x += d) {
//			for (float y = -range; y < range; y += d) {
//#define POINTXY(x,y) CPoint3F(x ,y,p(x, y), SOLVE_COLOR(color1,color2,(p(x,y) + 3) / 13))
//				CPoint3F data[] ={ POINTXY(x, y), POINTXY(x + d, y), POINTXY(x + d, y + d), POINTXY(x, y + d) };
//				p_this->AddGraphc(Graphic_Polygon(data, 4));
//				//p_this->AddGraphc(Graphic_Polygon(data[0],data[1],data[3]));
//				//p_this->AddGraphc(Graphic_Polygon(data[1],data[2],data[3]));
//			}
//		}
//	});
	//plane.Init();
	for (int i = 0; /*i < 50*/; i++) {

		MyTimer timer;
		timer.Start();

		canva.Clean();
		timer.MarkTime("Clean");
		//cout << "=====================" << endl;
		canva.Draw(plane, 100, 50);
		//canva.Draw(plane, -100, 0);
		//canva.Draw(plane, 100, 0);
		timer.MarkTime("Triangle");
		
		canva.Banding();
		timer.MarkTime("Banding");

		canva.Paint();
		timer.MarkTime("Paint");

		canva.Flush();
		timer.MarkTime("Flush");
		//cout << timer.EachTimes() << endl;
		//system("pause");
		timer3.DoInFPS(120, [&](float call_times) {
			timer3.MarkFPS();
			static float lastCallTimes = 0;
			plane.Camera().MoveByRotateInPoint({ 0,0,0 }, { 1,1,1 }, (call_times - lastCallTimes) / 20.0);
			plane.Camera().LookTo({ 0,0,50 });
			//plane.Camera().MoveByRotateInPoint({ 0,0,0 }, { 1,1,1 }, 1 / 20.0);
			//plane.Camera().LookTo({ 0,0,0 });
			lastCallTimes = call_times;
			},0);
		timer2.DoInFPS(2, [&](float call_times) {
			cout << "\rFPS:" << timer2.GetFPS() << "\t" << "FPS:" << timer3.GetFPS() << "\t" << timer2.GetTime() / 1000.0 << "s             ";
			});
		//timer2.ControlFPS_Rough(120);
		timer2.MarkFPS();
		//system("pause");
	}
	struct TempStruct1 {
		size_t size;
		CPoint3F arr[3];
	};
	struct TempStruct2 {
		size_t size;
		CPoint3F arr[3];
	public:
		TempStruct2& operator=(const TempStruct2& t) { 
			memcpy(this, &t, sizeof(TempStruct2)); 
			return *this; }
	};
	TempStruct1 tmpTri11, tmpTri12 = { 1,CPoint3F(),CPoint3F(),CPoint3F() };
	TempStruct2 tmpTri21, tmpTri22 = { 1,CPoint3F(),CPoint3F(),CPoint3F() };
	Graphic_Polygon tri1, tri2= { CPoint3F(1,2,3,4),CPoint3F(4,3,2,1),CPoint3F(5,6,7,8) };
	const int n = 1e+5;
	timer.Start();
	tmpTri11.size = 0;
	for (size_t i = 0; i < n; i++) {
		tmpTri11.size = i;
		tmpTri12.arr[0] = CPoint3F(rand(), rand(), rand(), rand());
		tmpTri11 = tmpTri12;
	}
	timer.MarkTime("TempStruct1");
	tmpTri11.size = 0;
	for (size_t i = 0; i < n; i++) {
		tmpTri11.size = i;
		tmpTri22.arr[0] = CPoint3F(rand(), rand(), rand(), rand());
		tmpTri21 = tmpTri22;
	}
	timer.MarkTime("TempStruct2");
	tmpTri11.size = 0;
	for (size_t i = 0; i < n; i++) {
		tmpTri11.size = i;
		tri2[0] = CPoint3F(rand(), rand(), rand(), rand());
		tri1 = tri2;
	//tri1 = Graphic_Line(CPoint3F(9, 8, 7, 6), CPoint3F(6, 7, 8, 9));
	}

	timer.MarkTime("TempClass");
	cout << timer.EachTimes() << endl;
	cout << tmpTri11.arr << tmpTri12.arr << tmpTri21.arr << tmpTri22.arr << endl << tri1 << " " << tri2 << endl;
	system("pause");

}