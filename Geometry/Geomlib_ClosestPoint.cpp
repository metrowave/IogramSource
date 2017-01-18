#include "Geomlib_ClosestPoint.h"

#include <Urho3D/Math/Vector3.h>

#include "Geomlib_RayTriangleIntersection.h"

using Urho3D::Vector3;

// Returns point on line segment AB closest to query point Q
// Intended to be crash proof.
Urho3D::Vector3 Geomlib::SegmentClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& Q
)
{
	Vector3 S = B - A; // points from A to B along the segment
	Vector3 V = Q - A;

	if (S.DotProduct(S) == 0) {
		// A, B are the same point; segment is a point
		return A;
	}

	float u = V.DotProduct(S) / S.DotProduct(S);

	if (u < 0) {
		return A;
	}
	else if (u > 1) {
		return B;
	}

	return A + u * S;
}

// Returns point on perimeter of triangle ABC closest to query point Q
// Intended to be crash proof.
Urho3D::Vector3 Geomlib::TrianglePerimeterClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& Q
)
{
	Vector3 pAB = SegmentClosestPoint(A, B, Q);
	Vector3 pBC = SegmentClosestPoint(B, C, Q);
	Vector3 pCA = SegmentClosestPoint(C, A, Q);

	Vector3 ABtoQ = Q - pAB;
	Vector3 BCtoQ = Q - pBC;
	Vector3 CAtoQ = Q - pCA;

	float dAB = ABtoQ.DotProduct(ABtoQ);
	float dBC = BCtoQ.DotProduct(BCtoQ);
	float dCA = CAtoQ.DotProduct(CAtoQ);

	if (dAB <= dBC && dAB <= dCA) {
		return pAB;
	}
	else if (dBC <= dCA) {
		return pBC;
	}
	else {
		return pCA;
	}
}

Urho3D::Vector3 Geomlib::TriangleClosestPoint(
	const Urho3D::Vector3& A,
	const Urho3D::Vector3& B,
	const Urho3D::Vector3& C,
	const Urho3D::Vector3& Q
)
{
	Vector3 E1 = B - A;
	Vector3 E2 = C - A;
	Vector3 N1 = E1.CrossProduct(E2);

	float s1 = 0.0f;
	bool check1 = RayTriangleIntersection(A, B, C, Q, N1, s1);
	if (check1) {
		return Q + s1 * N1;
	}

	float s2 = 0.0f;
	Vector3 N2 = -1 * N1;
	bool check2 = RayTriangleIntersection(A, B, C, Q, N2, s2);
	if (check2) {
		return Q + s2 * N2;
	}

	// ray misses triangle
	// project Q to plane of triangle ABC

	Vector3 U = N1.Normalized();
	float c = U.DotProduct(Q - A);
	Vector3 projQ = Q - c * U;

	return TrianglePerimeterClosestPoint(A, B, C, projQ);
}