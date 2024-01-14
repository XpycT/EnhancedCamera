// Copyright 2024 XpycT. All Rights Reserved.


#include "ECHUD.h"

#include "ECSelector.h"
#include "Engine/Canvas.h"

AECHUD::AECHUD()
{
	SelectionBoxColor = FLinearColor::Green;
	SelectionBoxThickness = 1.0f;
	bIsDrawingSelectionBox = false;
	bIsDrawingSelectionFill = false;
	bIsPerformingSelection = false;
}

void AECHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bIsDrawingSelectionBox)
	{
		DrawSelectionBox(SelectionStart, SelectionEnd);
	}

	if (bIsPerformingSelection)
	{
		PerformSelection();
	}
}

void AECHUD::BeginSelection(const FVector2D& StartPoint)
{
	SelectionStart = StartPoint;
	bIsDrawingSelectionBox = true;
	
	if(bUseFill)
	{
		bIsDrawingSelectionFill = true;
	}
}

void AECHUD::UpdateSelection(const FVector2D& EndPoint)
{
	SelectionEnd = EndPoint;
}

void AECHUD::EndSelection()
{
	bIsDrawingSelectionBox = false;
	bIsPerformingSelection = true;
	
	if(bUseFill)
	{
		bIsDrawingSelectionFill = false;
	}
}

// Default implementation of DrawSelectionBox. Draws a rectangle on the HUD.
void AECHUD::DrawSelectionBox_Implementation(const FVector2D& StartPoint, const FVector2D& EndPoint)
{
	if (Canvas)
	{
		// Calculate corners of the selection rectangle.
		const auto TopRight = FVector2D(SelectionEnd.X, SelectionStart.Y);
		const auto BottomLeft = FVector2D(SelectionStart.X, SelectionEnd.Y);

		// Draw lines to form the selection rectangle.
		Canvas->K2_DrawLine(SelectionStart, TopRight, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(TopRight, SelectionEnd, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(SelectionEnd, BottomLeft, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(BottomLeft, SelectionStart, SelectionBoxThickness, SelectionBoxColor);

		if(bUseFill)
		{
			const auto FillWidth = SelectionEnd.X - SelectionStart.X;
			const auto FillHeight = SelectionEnd.Y - SelectionStart.Y;
			DrawRect(SelectionBoxFillColor, SelectionStart.X, SelectionStart.Y, FillWidth, FillHeight);
		}
	}
}

// Default implementation of PerformSelection. Selects actors within the selection box.
void AECHUD::PerformSelection_Implementation()
{
	// Array to store actors that are within the selection rectangle.
	TArray<AActor*> SelectedActors;
	GetActorsInSelectionRectangle<AActor>(SelectionStart, SelectionEnd, SelectedActors, false, false);

	// Find the UECSelector component and pass the selected actors to it.
	if (const auto PC = GetOwningPlayerController())
	{
		if (const auto SelectorComponent = PC->FindComponentByClass<UECSelector>())
		{
			SelectorComponent->HandleSelectedActors(SelectedActors);
		}
	}

	bIsPerformingSelection = false;
}