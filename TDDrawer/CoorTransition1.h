#pragma once
//#include "DeriveVector.hpp"
#include "MyGraphic.h"
#include "MessageQueue.hpp"

class CoorTransition;

struct CoorTranThreadMessage {
	CoorTransition* coorTran;
	int startIndex;
	int endIndex;
};

extern MessageQueue<CoorTranThreadMessage> requestQueue_CoorTransition;
extern MessageQueue<CoorTranThreadMessage> replyQueue_CoorTransition;

LP_START_FUN(CoorTransition_thread_fun);

class CoorTransition {
private:

public:

	Vector3F I = { 0,1,0 }, J = { 1,0,0 }, K = { 0,0,1 };		//�����������Ϣ������Z��������Ұ����
	Vector3F X, Y, Z;
	Vector3F position = { 0,0,0 };								//�����λ����Ϣ
	MyGraphicVector* rawGraphics = nullptr;

	MyGraphicVector nowGraphics;

	MyVector<ThreadHandle> threadHandles;
	int threadCount;
	CoorTransition(int threadCount = 4 ):threadHandles(threadCount),threadCount(threadCount) {
		CalculateXYZ();

		threadHandles.size = threadCount;
		for (int i = 0; i < threadCount; i++) {
			MyCreateThread(&threadHandles[i], CoorTransition_thread_fun);
		}
	}
	~CoorTransition() {
		for (int i = 0; i < threadCount; i++) {
			requestQueue_CoorTransition.push({ nullptr,0,0 });
		}

	}

	void CalculateXYZ() { X = { I.x,J.x,K.x }, Y = { I.y,J.y,K.y }, Z = { I.z,J.z,K.z }; }

	inline CPoint3F Tran(const CPoint3F& rawCoor) {
		//���øú���֮ǰȷ���ȵ��ù�CalculateXYZ;
		Vector3F p = rawCoor - position;
		return { p.x * X + p.y * Y + p.z * Z,rawCoor.color };
	}

	void LoadRawGraphics(MyGraphicVector* rawGraphics) {
		if (rawGraphics) {
			this->rawGraphics = rawGraphics;
			nowGraphics = *rawGraphics;
		}
	}

	MyGraphicVector& TranGraphics(MyGraphicVector* rawGraphics, bool multithread = true) {
		LoadRawGraphics(rawGraphics);
		if (!this->rawGraphics) {
			std::cout << "CoorTransition(error):δ����ԭʼ����ͼ����" << std::endl;
			return nowGraphics;
		}

		if (multithread) {
			requestQueue_CoorTransition.Clear();
			replyQueue_CoorTransition.Clear();
			float delta = (float)rawGraphics->size / threadCount;

			for (int i = 0; i < threadCount; i++) {
				requestQueue_CoorTransition.push({ this,int(i * delta),int((i + 1) * delta) });
			}

			int count = 0;
			while (count < threadCount) {
				CoorTranThreadMessage temp;
				replyQueue_CoorTransition.wait_and_pop(temp);
				count++;
			}
		}
		else
			TranGraphics_core();
		return nowGraphics;
	}

	inline MyGraphicVector& TranGraphics_core() {

	
		for (size_t i = 0; i < rawGraphics->size; i++) {
			for (size_t j = 0; j < (*rawGraphics)[i].Size(); j++) {
				nowGraphics[i][j] = Tran((*rawGraphics)[i][j]);
			}
		}

		return nowGraphics;
	}
	inline void TranGraphics_core(int startIndex, int endIndex) {

		for (int i = startIndex; i < endIndex; i++) {
			for (size_t j = 0; j < (*rawGraphics)[i].Size(); j++) {
				nowGraphics[i][j] = Tran((*rawGraphics)[i][j]);
			}
		}

	}
};

