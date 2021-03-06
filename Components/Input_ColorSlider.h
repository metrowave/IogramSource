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


#pragma once

#include "IoComponentBase.h"
#include <Urho3D/UI/UIElement.h>
#include "ColorSlider.h"

class URHO3D_API Input_ColorSlider : public IoComponentBase {
	URHO3D_OBJECT(Input_ColorSlider, IoComponentBase)
public:
	Input_ColorSlider(Urho3D::Context* context);


	ColorSlider* mySlider;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
	Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);


	static Urho3D::String iconTexture;
	virtual void HandleCustomInterface(Urho3D::UIElement* customElement);

	void HandleSliderChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	//void HandleSliderChanged_X(StringHash eventType, VariantMap& eventData);
	//void HandleSliderChanged_Y(StringHash eventType, VariantMap& eventData);
	//void HandleSliderChanged_Z(StringHash eventType, VariantMap& eventData);
	//void HandleLineEditMin(StringHash eventType, VariantMap& eventData);
	//void HandleLineEditMax(StringHash eventType, VariantMap& eventData);

	void PushChange(Urho3D::Color curColor, bool solve=true);
};