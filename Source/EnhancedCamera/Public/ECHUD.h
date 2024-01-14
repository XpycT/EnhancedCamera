// Copyright 2024 XpycT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ECHUD.generated.h"

/**
 * 
 */
UCLASS()
class ENHANCEDCAMERA_API AECHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AECHUD();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box")
	FLinearColor SelectionBoxColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box")
	float SelectionBoxThickness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box")
	bool bUseFill = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box", meta = (EditCondition = "bUseFill"))
	FLinearColor SelectionBoxFillColor;

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void BeginSelection(const FVector2D& StartPoint);

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void UpdateSelection(const FVector2D& EndPoint);

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void EndSelection();

	UFUNCTION(BlueprintNativeEvent, Category = "Selection Box")
	void DrawSelectionBox(const FVector2D& StartPoint, const FVector2D& EndPoint);

	UFUNCTION(BlueprintNativeEvent, Category = "Selection Box")
	void PerformSelection();

protected:
	virtual void DrawHUD() override;

private:
	bool bIsDrawingSelectionBox;
	bool bIsDrawingSelectionFill;
	bool bIsPerformingSelection;
	FVector2D SelectionStart;
	FVector2D SelectionEnd;
};
