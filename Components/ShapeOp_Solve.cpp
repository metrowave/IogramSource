#include "ShapeOp_Solve.h"

#include <assert.h>

#include <iostream>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ShapeOp_API.h"
#include "ShapeOp_IogramWrapper.h"

#include "TriMesh.h"

using namespace Urho3D;

String ShapeOp_Solve::iconTexture = "Textures/Icons/DefaultIcon.png";

ShapeOp_Solve::ShapeOp_Solve(Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("ShapeOpSolve");
	SetFullName("ShapeOp Solve");
	SetDescription("...");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Operators");

	inputSlots_[0]->SetName("Mesh input");
	inputSlots_[0]->SetVariableName("M");
	inputSlots_[0]->SetDescription("Mesh input");
	inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	inputSlots_[1]->SetName("Constraint List");
	inputSlots_[1]->SetVariableName("CL");
	inputSlots_[1]->SetDescription("Constraint List");
	inputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[1]->SetDataAccess(DataAccess::LIST);

	inputSlots_[2]->SetName("Force List");
	inputSlots_[2]->SetVariableName("FL");
	inputSlots_[2]->SetDescription("Force List");
	inputSlots_[2]->SetVariantType(VariantType::VAR_VARIANTMAP);
	inputSlots_[2]->SetDataAccess(DataAccess::LIST);

	outputSlots_[0]->SetName("Mesh output");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Mesh output");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void ShapeOp_Solve::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Variant mesh_in = inSolveInstance[0];
	if (!TriMesh_Verify(mesh_in)) {
		URHO3D_LOGWARNING("ShapeOp_Solve --- M must be a TriMesh");
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	VariantVector vertex_list = TriMesh_GetVertexList(mesh_in);
	VariantVector face_list = TriMesh_GetFaceList(mesh_in);

	VariantVector constraint_list = inSolveInstance[1].GetVariantVector();
	for (unsigned i = 0; i < constraint_list.Size(); ++i) {
		if (!ShapeOpConstraint_Verify(constraint_list[i])) {
			URHO3D_LOGWARNING("ShapeOp_Solve --- constraint_list failed verification at index " + i);
			SetAllOutputsNull(outSolveInstance);
			return;
		}
	}

	VariantVector force_list = inSolveInstance[2].GetVariantVector();
	for (unsigned i = 0; i < force_list.Size(); ++i) {
		if (!ShapeOpVertexForce_Verify(force_list[i])) {
			URHO3D_LOGWARNING("ShapeOp_Solve --- force_list failed verification at index " + i);
			return;
		}
	}

	URHO3D_LOGINFO("ShapeOp_Solve all constraints verified");

	ShapeOpSolver* op = shapeop_create();

	std::vector<double> pts_in = ShapeOp_TriMeshToPoints(mesh_in);
	int nb_points = (int)(pts_in.size() / 3);
	shapeop_setPoints(op, pts_in.data(), nb_points);

	// all ids are captured here so their data won't go out of scope after added to the solver
	std::vector<std::vector<int> > all_ids;
	for (unsigned i = 0; i < constraint_list.Size(); ++i) {
		std::vector<int> ids = ShapeOpConstraint_ids(constraint_list[i]);
		all_ids.push_back(ids);
	}

	for (unsigned i = 0; i < constraint_list.Size(); ++i) {
		Variant constraint = constraint_list[i];

		shapeop_addConstraint(
			op,
			ShapeOpConstraint_constraintType(constraint).CString(),
			all_ids[i].data(),
			ShapeOpConstraint_nb_ids(constraint),
			ShapeOpConstraint_weight(constraint)
		);
	}

	URHO3D_LOGINFO("ShapeOp_Solve --- Constraints added");

	// all forces are captured here so their data won't go out of scope after added to the solver
	std::vector<std::vector<double> > all_forces;
	for (unsigned i = 0; i < force_list.Size(); ++i) {
		std::vector<double> force = ShapeOpVertexForce_force(force_list[i]);
		all_forces.push_back(force);
	}

	for (unsigned i = 0; i < force_list.Size(); ++i) {
		Variant vertex_force = force_list[i];

		shapeop_addVertexForce(
			op,
			all_forces[i].data(),
			ShapeOpVertexForce_id(vertex_force)
		);
	}

	URHO3D_LOGINFO("ShapeOp_Solve --- Vertex forces added");

	shapeop_init(op);

	shapeop_solve(op, 100);

	std::vector<double> pts_out(pts_in.size());
	shapeop_getPoints(op, pts_out.data(), nb_points);
	for (int i = 0; i < pts_out.size(); ++i) {
		std::cout << pts_out[i] << " ";
	}

	shapeop_delete(op);

	VariantVector new_vertex_list = ShapeOp_PointsToVertexList(pts_out);

	Variant mesh_out = TriMesh_Make(new_vertex_list, face_list);

	outSolveInstance[0] = mesh_out;
}