#include "CoorTransition.h"

void CoorTransition::MoveByRotateInPoint(Point3F pos_rot_by, Vector3F n_rot_by, float angle) {
	n_rot_by.Unitize();
	Vector3F posVec = pos_rot_by - position;
	posVec.Rotate(n_rot_by, angle);
	position = pos_rot_by - posVec;
}

void CoorTransition::LookTo(Vector3F target) {
	Vector3F targetK = (target - position).Unitize();
	float angle = acos((targetK * K > 1) ? 1 : targetK * K);
	if (angle == 0) return;
	Vector3F n = (K ^ targetK).Unitize();
	I.Rotate(n, angle).Unitize(); J.Rotate(n, angle).Unitize(); K.Rotate(n, angle).Unitize();
	CalculateXYZ();
}

void CoorTransition::LookTo_Fix_I_in_Plane(Vector3F target, Vector3F plane) {
	K = (target - position).Unitize();
	Vector3F targetI = (plane ^ K).Unitize();
	if (targetI * I > 0) I = targetI;
	else I = -targetI;
	J = K ^ I;
	CalculateXYZ();
}

void CoorTransition::LookTo_Fix_J_in_Plane(Vector3F target, Vector3F plane) {
	K = (target - position).Unitize();
	Vector3F targetJ = (plane ^ K).Unitize();
	if (targetJ * J > 0) J = targetJ;
	else J = -targetJ;
	I = J ^ K;
	CalculateXYZ();
}