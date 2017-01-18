#include "Curve_PolylineBlend.h"

#include <assert.h>

#include "Polyline.h"
#include "Geomlib_PolylineBlend.h"

using namespace Urho3D;

Urho3D::String Curve_PolylineBlend::iconTexture = "Textures/Icons/Curve_PolylineBlend.png";

Curve_PolylineBlend::Curve_PolylineBlend(Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("LoftPolylines");
	SetFullName("Loft Polylines");
	SetDescription("Lofts a collection of polylines into a mesh");
	SetGroup(IoComponentGroup::CURVE);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Polyline");
	inputSlots_[0]->SetVariableName("P1");
	inputSlots_[0]->SetDescription("First polyline");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Polyline");
	inputSlots_[1]->SetVariableName("P2");
	inputSlots_[1]->SetDescription("Second polyline");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Lofted mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_PolylineBlend::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 0
	Variant polyIn1 = inSolveInstance[0];
	if (!Polyline_Verify(polyIn1)) {
		URHO3D_LOGWARNING("P1 must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}
	// Verify input slot 1
	Variant polyIn2 = inSolveInstance[1];
	if (!Polyline_Verify(polyIn2)) {
		URHO3D_LOGWARNING("P2 must be a valid polyline.");
		outSolveInstance[0] = Variant();
		return;
	}

	///////////////////
	// COMPONENT'S WORK

	Variant mesh;
	bool success = Geomlib::PolylineBlend(polyIn1, polyIn2, mesh);
	if (!success) {
		URHO3D_LOGWARNING("PolylineBlend operation failed.");
		outSolveInstance[0] = Variant();
		return;
	}

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = mesh;
}