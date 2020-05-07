#include "CoorTransition1.h"

MessageQueue<CoorTranThreadMessage> requestQueue_CoorTransition;
MessageQueue<CoorTranThreadMessage> replyQueue_CoorTransition;

LP_START_FUN(CoorTransition_thread_fun) {
	while (true) {
		CoorTranThreadMessage mes;
		requestQueue_CoorTransition.wait_and_pop(mes);

		if (!mes.coorTran) return;
		mes.coorTran->TranGraphics_core(mes.startIndex, mes.endIndex);

		replyQueue_CoorTransition.push(mes);
	}
}
//
//MyTimer timer;
//
//initgraph(1100, 650, 1);
//
//Sleep(10);
//RECT FunctionRect = { 150,10,950,610 };
//BeginBatchDraw();
//
//Canva canva(FunctionRect);
//
////MyVector<Graphic_Triangle> a;
////a.push_back(Pointer_MyGraphic());
//
//canva.backcolor = RGB(77, 111, 107);
//const int xxx = 1000;
//timer.Start();
//MyGraphicVector test(xxx);
//for (int j = 0; j < xxx; j++) {
//	test.push_back(Graphic_Triangle({ 1,2,3, RED }, { 4,5,6, GREEN }, { 7,8,9, BLUE }));
//}
//timer.MarkTime("MyGraphicVector");
//cout << timer.EachTimes() << endl;
////cout << "===================test2=================" << endl;
////MyGraphicVector test2(test);
////cout << "===================test3=================" << endl;
////MyGraphicVector test3(0);
////test3 = test;
//
//MyVector<CoorTransition*> v;
//for (int i = 1; i <= 10; i++) {
//	v[i - 1] = new CoorTransition(i);
//}
//CoorTransition coorTran2(1);
//
//timer.Start();
//
//
//for (int i = 1; i <= 10; i++) {
//	//cout << "==========第" << i << "环节" << "==============" << endl;
//	v[i - 1]->TranGraphics(&test);
//	//cout << v[i - 1]->nowGraphics << endl;
//
//	timer.MarkTime("Test1");
//}
//
////cout << "==========参考环节==============" << endl;
//coorTran2.TranGraphics(&test, false);
////cout << coorTran2.nowGraphics << endl;
//timer.MarkTime("Test2");
//cout << timer.EachTimes() << endl;
