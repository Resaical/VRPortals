// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PortalComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALSVR_API UPortalComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPortalComponent();



protected:

	// Called when the component is added/initialized
	virtual void OnRegister() override; 

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UStaticMesh> DefaultPortalMesh;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UStaticMeshComponent> PortalMesh;

	
	TUniquePtr<FRenderTarget> PortalRenderTarget;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UMaterialInstance> PortalMaterial;

	// Linking objects
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AActor> OtherPortal;
};
