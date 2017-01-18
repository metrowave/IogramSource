#pragma once

#include "IoComponentBase.h"

class URHO3D_API Curve_PolylineDivide : public IoComponentBase {
	URHO3D_OBJECT(Curve_PolylineDivide, IoComponentBase)
public:
	Curve_PolylineDivide(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
	);

	static Urho3D::String iconTexture;
};