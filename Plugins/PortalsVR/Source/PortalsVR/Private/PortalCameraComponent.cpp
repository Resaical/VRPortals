// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalCameraComponent.h"

// Sets default values for this component's properties
UPortalCameraComponent::UPortalCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//SceneCapture2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D"));

	//// Set up the scene capture component properties BEFORE attachment
	//SceneCapture2D->SetMobility(EComponentMobility::Movable);
	//SceneCapture2D->ProjectionType = ECameraProjectionMode::Perspective;
	//SceneCapture2D->FOVAngle = 90.0f;
	//SceneCapture2D->bCaptureEveryFrame = true;
	//SceneCapture2D->bCaptureOnMovement = true;
	//SceneCapture2D->CaptureSource = SCS_FinalColorLDR;

	//SceneCapture2D->SetupAttachment(this);
}


// Called when the game starts
void UPortalCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

// Called every frame
void UPortalCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPortalCameraComponent::SetPortalRenderTarget(UTextureRenderTarget2D* renderTarget)
{
	SceneCapture2D->TextureTarget = renderTarget;
}