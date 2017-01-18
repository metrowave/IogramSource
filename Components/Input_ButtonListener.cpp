#include "Input_ButtonListener.h"

#include <assert.h>

#include <Urho3D/Core/Variant.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>

#include "IoGraph.h"

using namespace Urho3D;

String Input_ButtonListener::iconTexture = "Textures/Icons/Input_ToggleListener.png";


Input_ButtonListener::Input_ButtonListener(Urho3D::Context* context) : IoComponentBase(context, 2, 1)
{
	SetName("ButtonListener");
	SetFullName("Button Listener");
	SetDescription("Listens to button in scene");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("INPUT");

	inputSlots_[0]->SetName("Button Pointers");
	inputSlots_[0]->SetVariableName("BP");
	inputSlots_[0]->SetDescription("Pointers to screen buttons");
	inputSlots_[0]->SetVariantType(VariantType::VAR_PTR);

	inputSlots_[1]->SetName("Mute");
	inputSlots_[1]->SetVariableName("M");
	inputSlots_[1]->SetDescription("Mute this listener");
	inputSlots_[1]->SetVariantType(VariantType::VAR_BOOL);
	inputSlots_[1]->SetDefaultValue(false);
	inputSlots_[1]->DefaultSet();


	outputSlots_[0]->SetName("Values");
	outputSlots_[0]->SetVariableName("V");
	outputSlots_[0]->SetDescription("Values Out");
	outputSlots_[0]->SetVariantType(VariantType::VAR_BOOL); // this would change to VAR_FLOAT if access becomes LIST
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Input_ButtonListener::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	if (inSolveInstance[0].GetPtr() == NULL)
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}
	
	Button* b = (Button*)inSolveInstance[0].GetVoidPtr();
	String typeName = b->GetTypeName();
	bool mute = inSolveInstance[1].GetBool();
	if (b && typeName == Button::GetTypeNameStatic())
	{
		if (!mute)
			SubscribeToEvent(b, E_PRESSED, URHO3D_HANDLER(Input_ButtonListener, HandleButtonPress));			
		else
			UnsubscribeFromEvent(E_PRESSED);			

		outSolveInstance[0] = b->IsSelected();
	}
}

void Input_ButtonListener::HandleButtonPress(StringHash eventType, VariantMap& eventData)
{
	solvedFlag_ = 0;
	GetSubsystem<IoGraph>()->QuickTopoSolveGraph();
}