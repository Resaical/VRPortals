// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalCamera.h"

// Sets default values
APortalCamera::APortalCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));

	SceneCapture2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D"));
	SceneCapture2D->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APortalCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APortalCamera::SetPortalRenderTarget(UTextureRenderTarget2D* renderTarget)
{
	SceneCapture2D->TextureTarget = renderTarget;
}

