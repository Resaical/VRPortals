// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Engine/TextureRenderTarget2D.h"
#include <Camera/CameraComponent.h>
#include "GameFramework/GameUserSettings.h"
#include <PortalSubsystem.h>
#include <PortalFunctions.h>

APortal::APortal()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));

    // Mesh
    {
        PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
        PortalMesh->SetupAttachment(RootComponent);
        PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        PortalHollowCubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Hollow Cube Mesh"));
        PortalHollowCubeMesh->SetupAttachment(RootComponent);
        PortalHollowCubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    //SceneCapture2D
    {
        SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D"));
        SceneCaptureComponent2D->SetupAttachment(RootComponent);
        SceneCaptureComponent2D->HideComponent(PortalMesh);
        SceneCaptureComponent2D->HideComponent(PortalHollowCubeMesh);
        SceneCaptureComponent2D->SetTickGroup(TG_PrePhysics);
    }

    //Render Target
    {
        FString RenderTargetName = FString::Printf(TEXT("%s_PortalRenderTargetLeft"), *GetName());
        PortalRenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(*RenderTargetName);
        PortalRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // 8-bit per channel
        PortalRenderTarget->InitAutoFormat(1024, 1024);
        PortalRenderTarget->UpdateResourceImmediate(true);
    }

    //BoxTrigger
    {
        PortalBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Portal BoxTrigger"));
        PortalBoxTrigger->SetupAttachment(RootComponent);

        FScriptDelegate OnBeginDelegate;
        OnBeginDelegate.BindUFunction(this, TEXT("OnPortalOverlapBegin"));
        PortalBoxTrigger->OnComponentBeginOverlap.Add(OnBeginDelegate);

        FScriptDelegate OnEndDelegate;
        OnEndDelegate.BindUFunction(this, TEXT("OnPortalOverlapEnd"));
        PortalBoxTrigger->OnComponentEndOverlap.Add(OnEndDelegate);

        PortalBoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PortalBoxTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
        PortalBoxTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
        PortalBoxTrigger->SetGenerateOverlapEvents(true);
    }
}

void APortal::BeginPlay()
{
    Super::BeginPlay();

    FVector2D viewportSize = FVector2D(0, 0);
    if (GEngine && GEngine->GameViewport)
    {
        viewportSize = GEngine->GameUserSettings->GetScreenResolution();
    }
    PortalRenderTarget->ResizeTarget(viewportSize.X, viewportSize.Y);

    UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(PortalMesh->GetMaterial(0), this);
    PortalMesh->SetMaterial(0, DynMat);
    DynMat->SetTextureParameterValue(RenderTargetParameterName, PortalRenderTarget);

    DynMat = UMaterialInstanceDynamic::Create(PortalHollowCubeMesh->GetMaterial(0), this);
    PortalHollowCubeMesh->SetMaterial(0, DynMat);
    DynMat->SetTextureParameterValue(RenderTargetParameterName, PortalRenderTarget);
    PortalHollowCubeMesh->SetHiddenInGame(true, true);

    GetWorld()->GetSubsystem<UPortalSubsystem>()->ActivePortals.Add(this);
}

void APortal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (OtherPortal)
    {
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

        FVector PlayerCameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
        FQuat PlayerCameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion();

        FVector otherPortalCameraNewLocation;
        PortalTools::FlatScreen::TeleportWorldLocationMirrored(this, OtherPortal, PlayerCameraLocation, otherPortalCameraNewLocation);

        FQuat otherPortalCameraNewRotation;
        PortalTools::FlatScreen::TeleportWorldRotationMirrored(this, OtherPortal, PlayerCameraRotation, otherPortalCameraNewRotation);

        OtherPortal->SceneCaptureComponent2D->SetWorldLocationAndRotation(otherPortalCameraNewLocation, otherPortalCameraNewRotation);
    }
}

void APortal::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (OtherPortal != OldOtherPortalInEditor)
    {
        if (OtherPortal) PortalTools::FlatScreen::ConnectPortalPair(this, OtherPortal);
        else PortalTools::FlatScreen::DisconnectPortalPair(this);
    }
}

void APortal::OnPortalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!ActorsToCheckTeleport.Contains(OtherActor))
    {
        ActorsToCheckTeleport.Add(OtherActor);
    }
    PortalHollowCubeMesh->SetHiddenInGame(false, true);
    PortalMesh->SetHiddenInGame(true, true);
    GEngine->GameViewport->EngineShowFlags.DisableOcclusionQueries = true;
}

void APortal::OnPortalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ActorsToCheckTeleport.Remove(OtherActor);
    PortalHollowCubeMesh->SetHiddenInGame(true, true);
    PortalMesh->SetHiddenInGame(false, true);
    GEngine->GameViewport->EngineShowFlags.DisableOcclusionQueries = false;
}




