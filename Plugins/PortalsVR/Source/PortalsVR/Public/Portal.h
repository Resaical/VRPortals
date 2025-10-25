// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <PortalCamera.h>
#include <PortalCameraComponent.h>
#include <Components/BoxComponent.h>

class UMaterialInstance;
class UStaticMeshComponent;
class UTextureRenderTarget2D;

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

	UFUNCTION()
	void OnPortalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPortalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PortalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PortalHollowCubeMesh;


	//RenderTarget
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* PortalRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RenderTargetParameterName = TEXT("RenderTargetPortal");

	// Linking objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APortal* OtherPortal = nullptr;

	UPROPERTY()
	APortal* OldOtherPortalInEditor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* SceneCaptureComponent2D;


	//Teleport
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PortalBoxTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsToCheckTeleport;
};
