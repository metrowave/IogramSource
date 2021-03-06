//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Geomlib_PolylinePointFromParameter.h"

#include "Polyline.h"

// Inputs:
//   polyline: storing type VAR_VARIANTMAP, with map storing type VAR_VARIANTVECTOR under key "vertices", with those variants storing type VAR_VECTOR3;
//             >= 2 distinct points in the polyline
//   t:        0 <= t <= 1 (t will be set to an appropriate endpoint if it's outside the valid interval
// Outputs:
//   point: 3D point on the line corresponding to parameter t; start of polyline is t=0, end of polyline is t=1
// Returns true if computation was successfully carried out.
bool Geomlib::PolylinePointFromParameter(
	const Urho3D::Variant& polyline,
	float t,
	Urho3D::Vector3& point
)
{
	using Urho3D::VariantVector;
	using Urho3D::Vector3;

	//////////////////////////////////
	// Extract and validate input data
	//////////////////////////////////

	if (!Polyline_Verify(polyline)) {
		return false;
	}
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	const VariantVector vertexList = Polyline_ComputeSequentialVertexList(polyline);
	if (vertexList.Size() == 0) {
		return false;
	}
	else if (vertexList.Size() == 1) {
		// degenerate 1-point polyline
		point = vertexList[0].GetVector3();
		return true;
	}

	////////////////////////////////////////////////////////
	// Vertex count is >= 2, proper computation begins here.
	////////////////////////////////////////////////////////

	float totalLength = 0.0f;
	// loop over each edge [i, i + 1]
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 seg = vertexList[i + 1].GetVector3() - vertexList[i].GetVector3();
		totalLength += seg.Length();
	}

	if (Urho3D::Equals(totalLength, 0.0f)) {
		// All points in polyline are identical, or, more likely,
		// the Variants do not store Vector3's and so return the origin for GetVector3().
		point = vertexList[0].GetVector3();
		return true;
	}

	float targetDistance = t * totalLength;

	// loop over each edge [i, i + 1]
	float startLength = 0.0f;
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 start = vertexList[i].GetVector3();
		Vector3 end = vertexList[i + 1].GetVector3();
		Vector3 seg = end - start;
		float segLength = seg.Length();
		float endLength = startLength + segLength;

		// If point lies on this segment....
		if (endLength >= targetDistance) {
			if (Urho3D::Equals(segLength, 0.0f)) {
				point = start; // or end, since in this case start == end
				return true;
			}
			float s = (targetDistance - startLength) / segLength;
			point = start + s * seg;
			return true;
		}

		// Point lies on a later segment. Update and carry on.
		startLength = endLength;
	}

	// If computation logic is correct, we should never arrive here.
	return false;
}

bool Geomlib::PolylineTransformFromParameter(const Urho3D::Variant & polyline, float t, Urho3D::Matrix3x4 & transform)
{
	using Urho3D::VariantVector;
	using Urho3D::Vector3;

	// the components making up the transform
	Urho3D::Vector3 position;
	Urho3D::Vector3 tangent;
	Urho3D::Vector3 normal;
	Urho3D::Vector3 x_axis;


	//////////////////////////////////
	// Extract and validate input data
	//////////////////////////////////

	if (!Polyline_Verify(polyline)) {
		return false;
	}
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	const VariantVector vertexList = Polyline_ComputeSequentialVertexList(polyline);
	if (vertexList.Size() == 0) {
		return false;
	}
	else if (vertexList.Size() == 1) {
		// degenerate 1-point polyline, no transform defined
		return false;
	}


	bool isClosed = Polyline_IsClosed(polyline);

	float totalLength = 0.0f;
	// loop over each edge [i, i + 1]
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 seg = vertexList[i + 1].GetVector3() - vertexList[i].GetVector3();
		totalLength += seg.Length();
	}

	if (Urho3D::Equals(totalLength, 0.0f)) {
		// All points in polyline are identical, or, more likely,
		// the Variants do not store Vector3's and so return false.
		return false;
	}

	float targetDistance = t * totalLength;

	// loop over each edge [i, i + 1]
	float startLength = 0.0f;
	for (unsigned i = 0; i < vertexList.Size() - 1; ++i) {
		Vector3 start = vertexList[i].GetVector3();
		Vector3 end = vertexList[i + 1].GetVector3();
		Vector3 seg = end - start;
		float segLength = seg.Length();
		float endLength = startLength + segLength;

		// If point lies on this segment....
		if (endLength >= targetDistance) {
			//if (Urho3D::Equals(segLength, 0.0f)) {
			//	point = start; // or end, since in this case start == end
			//	return true;
			//}
			// Here is the actual transform computation
			float s = (targetDistance - startLength) / segLength;
			position = start + s * seg;

			// if it is just a line, use the "Look at" idea
			if (vertexList.Size() == 2) {
				Vector3 targetPt = vertexList[1].GetVector3();
				Vector3 startPt = vertexList[0].GetVector3();
				Vector3 up = Vector3::UP;
				Urho3D::Quaternion rot;
				rot.FromLookRotation(targetPt - startPt, up);
				Urho3D::Matrix3x4 xform(position, rot, Vector3::ONE);
				transform = xform;
			}
			else {
				GetVertexNormal(polyline, normal, i);
			}

			tangent = seg.Normalized();
			x_axis = normal.CrossProduct(tangent);

			
			Urho3D::Quaternion rot;
			rot.FromLookRotation(tangent, normal);
			//Urho3D::Quaternion quat(tangent, normal, x_axis);
			//Urho3D::Matrix3 rotation = quat.RotationMatrix();

			//Urho3D::Matrix3x4 xform(position, rot, Vector3::ONE);
			//transform = xform;
			transform.SetRotation(rot.RotationMatrix());
			transform.SetTranslation(position);
			//transform.SetScale(1.0f);

			return true;
		}

		// Point lies on a later segment. Update and carry on.
		startLength = endLength;
	}

	// If computation logic is correct, we should never arrive here.
	return false;

}

bool Geomlib::GetVertexNormal(const Urho3D::Variant & polyline, Urho3D::Vector3& normal, int vert_id)
{
	using Urho3D::Vector3;

	bool isClosed = Polyline_IsClosed(polyline);
	const Urho3D::VariantVector vertexList = Polyline_ComputeSequentialVertexList(polyline);

	if (vert_id == 0) {
		// depends whether closed or not
		if (isClosed) {
			// note we want the second last entry of vertexList
			Vector3 leftHandDir = (vertexList[vertexList.Size() - 2].GetVector3() - vertexList[0].GetVector3()).Normalized();
			Vector3 rightHandDir = (vertexList[1].GetVector3() - vertexList[0].GetVector3()).Normalized();
			normal = leftHandDir.CrossProduct(rightHandDir);

			return true;
		}
		else {
			// open polyline
			// use the first two segments to get the plane, make the normal 90deg to tangent
			Vector3 leftHandDir = (vertexList[vert_id].GetVector3() - vertexList[vert_id+1].GetVector3()).Normalized();
			Vector3 rightHandDir = (vertexList[vert_id + 2].GetVector3() - vertexList[vert_id+1].GetVector3()).Normalized();
			normal = leftHandDir.CrossProduct(rightHandDir);
			
			return true;
		}
	}
	else if (vert_id < vertexList.Size() - 1) {
		// interior vertices, the meat
		Vector3 leftHandDir = (vertexList[vert_id - 1].GetVector3() - vertexList[vert_id].GetVector3()).Normalized();
		Vector3 rightHandDir = (vertexList[vert_id + 1].GetVector3() - vertexList[vert_id].GetVector3()).Normalized();
		float angle = leftHandDir.Angle(rightHandDir);
		normal = rightHandDir.CrossProduct(leftHandDir);
		
		return true;

	}

	// shouldn't get here
	return false;
}

