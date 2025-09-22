// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <PortalCamera.h>
#include <PortalCameraComponent.h>

class UMaterialInstance;
class UStaticMeshComponent;
class UTextureRenderTarget2D;
class APortalCamera;

#include "Portal.generated.h"

UCLASS()
class PORTALSVR_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
		
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ConnectToPortal(APortal* otherPortal);

	//Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PortalMesh;

	//RenderTarget
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* PortalRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RenderTargetParameterName = TEXT("RenderTargetPortal");

	// Linking objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APortal* OtherPortal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* SceneCaptureComponent2D;

};
