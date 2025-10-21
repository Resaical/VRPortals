// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PortalVRSceneCaptureComponent2D.generated.h"

/**
 * 
 */
UCLASS()
class PORTALSVR_API UPortalVRSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()
	
public:
	virtual void UpdateSceneCaptureContents(FSceneInterface* Scene) override;
};
