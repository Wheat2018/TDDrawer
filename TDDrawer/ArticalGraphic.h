#pragma once
#include <iostream>
#include "CoorTransition.h"
#include "ViewZone.h"
#include "Canva.h"
#include <MyTimer.h>
#include <ppl.h>

class ArticalGraphic: public Brush {
public:

	void CoorTransform()const {
		tranGraphics = std::move(camera.TranGraphics(graphics));
		transformed = true;
	}
	void ViewProject()const {
		finalGraphics = std::move(view.InciseProject(tranGraphics));
		projected = true;
	}

	void FastTran_Proj()const {
		finalGraphics = graphics;
		auto position = camera.position;
		auto X = camera.X;
		auto Y = camera.Y;
		auto Z = camera.Z;
		auto near_vec = view.near_vec;
		auto nearDepth = view.nearDepth;
		auto canvaWidth = view.canvaWidth;
		auto canvaHeight = view.canvaHeight;
		concurrency::parallel_for_each(&finalGraphics.data_vec[0], &finalGraphics.data_vec[finalGraphics.data_vec.size], [&](CPoint3F& p) {
			CoorTransition::CoorTran(p, position, X, Y, Z);
			ViewZone::ViewZoneProject(near_vec, nearDepth, canvaWidth, canvaHeight, p);
			});
		//concurrency::array_view<CPoint3D> point_view(finalGraphics.data_vec.size, finalGraphics.data_vec.arr.arr);
		//concurrency::parallel_for_each(point_view.extent, [=](concurrency::index<1> idx) restrict(amp) {
		//	CPoint3D tmp = point_view[idx];
		//	CoorTransition::CoorTran(tmp, position, X, Y, Z);
		//	ViewZone::ViewZoneProject(near_vec, nearDepth, canvaWidth, canvaHeight, tmp);
		//	point_view[idx] = tmp;
		//	});
		//point_view.synchronize();
		projected = true;
		transformed = true;
	}

	CoorTransition camera;
	ViewZone view;
	
	MyGraphicFastVector graphics;

	mutable MyGraphicFastVector tranGraphics;
	mutable bool transformed = true;

	mutable MyGraphicFastVector finalGraphics;
	mutable bool projected = true;

public:

	ArticalGraphic() :view(100, 100, 0.1, 1000) { ; }

	ArticalGraphic(int argc, void** argv, 
		void(*func)(int argc, void** argv, ArticalGraphic* p_this)):ArticalGraphic() {
		func(argc, argv, this);
	}

	CoorTransition& Camera() { transformed = false; projected = false; return camera; }
	const CoorTransition& Camera_()const { return camera; }

	ViewZone& View() { projected = false; return view; }
	const ViewZone& View_()const { return view; }

	void AddGraphc(const Graphic_Polygon& g) {
		graphics.push_back(g);
	}
	void AddGraphc(const MyGraphicVector& v) {
		graphics.join_back(v);
	}

	void Draw(CanvaInterface& canva)const {
		//MyTimer timer;
		//timer.Start();
		//CoorTransform();
		//timer.MarkTime("CoorTransform");
		//ViewProject();
		//timer.MarkTime("ViewProject");
		//canva.Draw(finalGraphics);
		////FastTran_Proj();
		////timer.MarkTime("FastTran_Proj");
		//timer.MarkTime("Draw");
		//cout << timer.EachTimes() << endl;
		if (!transformed) CoorTransform();
		if (!projected) ViewProject();
		canva.Draw(finalGraphics);
		//if (!transformed && !projected) FastTran_Proj();
	}

};

