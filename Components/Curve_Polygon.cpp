#include "Curve_Polygon.h"

#include <assert.h>


#include "StockGeometries.h"

#include "Geomlib_TransformVertexList.h"

#include "Polyline.h"

using namespace Urho3D;

String Curve_Polygon::iconTexture = "Textures/Icons/Curve_Polygon.png";

Curve_Polygon::Curve_Polygon(Context* context) :
	IoComponentBase(context, 2, 1)
{
	SetName("Polygon");
	SetFullName("Construct Polygon");
	SetDescription("Construct a polygon with n sides");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Number of sides");
	inputSlots_[0]->SetVariableName("N");
	inputSlots_[0]->SetDescription("Number of sides for the polygon");
	inputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Variant(5));
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Transformation");
	inputSlots_[1]->SetVariableName("T");
	inputSlots_[1]->SetDescription("Transformation to apply to polygon");
	inputSlots_[1]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[1]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[1]->DefaultSet();

	outputSlots_[0]->SetName("Polygon");
	outputSlots_[0]->SetVariableName("P");
	outputSlots_[0]->SetDescription("Constructed polygon");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Curve_Polygon::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	// EXTRACT & VERIFY
	if (!IsAllInputValid(inSolveInstance)) {
		SetAllOutputsNull(outSolveInstance);
	}
	int n = inSolveInstance[0].GetInt();
	Urho3D::Matrix3x4 tr = inSolveInstance[1].GetMatrix3x4();
	if (n <= 2) {
		URHO3D_LOGWARNING("N must be >= 3");
		SetAllOutputsNull(outSolveInstance);
		return;
	}

	// COMPONENT'S WORK

	Variant base_polygon = MakeRegularPolygon(n);
	VariantVector base_vertex_list = Polyline_ComputeSequentialVertexList(base_polygon);
	VariantVector vertex_list = Geomlib::TransformVertexList(tr, base_vertex_list);
	Variant polygon = Polyline_Make(vertex_list);

	outSolveInstance[0] = polygon;
}