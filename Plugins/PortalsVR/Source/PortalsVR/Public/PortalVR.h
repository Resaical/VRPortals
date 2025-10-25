// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SceneCaptureComponent2D.h>
#include <Components/BoxComponent.h>
#include "PortalVR.generated.h"

#define PORTAL_ACTOR_STEREOSCOPIC_IN_CHARGE 0

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
	{
		Super::EndPlay(EndPlayReason);
	};
protected:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Teleport
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
	UTextureRenderTarget2D* PortalRenderTargetLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* PortalRenderTargetRight;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RenderTargetParameterNameLeft = TEXT("RenderTargetPortalLeft");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RenderTargetParameterNameRight = TEXT("RenderTargetPortalRight");

	// Linking objects
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	APortalVR* OtherPortal = nullptr;

	UPROPERTY()
	APortalVR* OldOtherPortalInEditor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* SceneCaptureComponent2DLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* SceneCaptureComponent2DRight;

	bool leftImageRendered = false;
	bool rightImageRendered = false;

	//Teleport
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PortalBoxTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ActorsToCheckTeleport;

};