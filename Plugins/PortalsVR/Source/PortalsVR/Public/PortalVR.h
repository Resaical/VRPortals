// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalVR.generated.h"

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

	void ConnectToPortal(APortalVR* otherPortal);

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
	APortalVR* OtherPortal;
};
