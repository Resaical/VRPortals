// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SceneCaptureComponent2D.h>
#include "PortalVR.generated.h"

#define PORTAL_ACTOR_STEREOSCOPIC_IN_CHARGE 1

UCLASS()


class PORTALSVR_API APortalVR : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APortalVR();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnXRLateUpdate();	

	void ConnectToPortal(APortalVR* otherPortal);

	//Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PortalMesh;

	//RenderTarget
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* PortalRenderTargetLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* PortalRenderTargetRight;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RenderTargetParameterNameLeft = TEXT("RenderTargetPortalLeft");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RenderTargetParameterNameRight = TEXT("RenderTargetPortalRight");

	// Linking objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APortalVR* OtherPortal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* SceneCaptureComponent2DLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* SceneCaptureComponent2DRight;

	bool leftImageRendered = false;
	bool rightImageRendered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* eyeLeftDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)

};