

#include "PortalVR.h"
#include <PortalSubsystem.h>
#include <Engine/TextureRenderTarget2D.h>
#include "GameFramework/GameUserSettings.h"
#include <Camera/CameraComponent.h>
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "IXRCamera.h"
#include <PortalFunctions.h>

/// Sets default values
APortalVR::APortalVR()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));

    //Mesh
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
        SceneCaptureComponent2DLeft = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D Left"));
        SceneCaptureComponent2DLeft->SetupAttachment(RootComponent);
        SceneCaptureComponent2DLeft->bUseCustomProjectionMatrix = true;
        SceneCaptureComponent2DLeft->bCaptureEveryFrame = true;
        SceneCaptureComponent2DLeft->SetTickGroup(TG_PostPhysics);
        SceneCaptureComponent2DLeft->HideComponent(PortalMesh);
        SceneCaptureComponent2DLeft->HideComponent(PortalHollowCubeMesh);
        SceneCaptureComponent2DLeft->SetFlags(RF_Transactional);

        SceneCaptureComponent2DRight = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D Right"));
        SceneCaptureComponent2DRight->SetupAttachment(RootComponent);
        SceneCaptureComponent2DRight->bUseCustomProjectionMatrix = true;
        SceneCaptureComponent2DRight->bCaptureEveryFrame = true;
        SceneCaptureComponent2DRight->SetTickGroup(TG_PostPhysics);
        SceneCaptureComponent2DRight->HideComponent(PortalMesh);
        SceneCaptureComponent2DRight->HideComponent(PortalHollowCubeMesh);
        SceneCaptureComponent2DRight->SetFlags(RF_Transactional);
    }

    //Render Target
    {
        FString LeftRTName = FString::Printf(TEXT("%s_PortalRenderTargetLeft"), *GetName());
        PortalRenderTargetLeft = CreateDefaultSubobject<UTextureRenderTarget2D>(*LeftRTName);
        PortalRenderTargetLeft->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // 8-bit per channel
        PortalRenderTargetLeft->InitAutoFormat(1024, 1024);
        PortalRenderTargetLeft->UpdateResourceImmediate(true);        

        LeftRTName = FString::Printf(TEXT("%s_PortalRenderTargetRight"), *GetName());
        PortalRenderTargetRight = CreateDefaultSubobject<UTextureRenderTarget2D>(*LeftRTName);
        PortalRenderTargetRight->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // 8-bit per channel
        PortalRenderTargetRight->InitAutoFormat(1024, 1024);
        PortalRenderTargetRight->UpdateResourceImmediate(true);
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
int count = 0;

// Called when the game starts or when spawned
void APortalVR::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine && GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected())
    {
        const auto RenderTargetSize = GEngine->XRSystem->GetHMDDevice()->GetIdealRenderTargetSize();
        PortalRenderTargetLeft->ResizeTarget(RenderTargetSize.X, RenderTargetSize.Y);
        PortalRenderTargetRight->ResizeTarget(RenderTargetSize.X, RenderTargetSize.Y);
    }

    UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(PortalMesh->GetMaterial(0), this);
    PortalMesh->SetMaterial(0, DynMat);
    DynMat->SetTextureParameterValue(RenderTargetParameterNameLeft, PortalRenderTargetLeft);
    DynMat->SetTextureParameterValue(RenderTargetParameterNameRight, PortalRenderTargetRight);

    DynMat = UMaterialInstanceDynamic::Create(PortalHollowCubeMesh->GetMaterial(0), this);
    PortalHollowCubeMesh->SetMaterial(0, DynMat);
    DynMat->SetTextureParameterValue(RenderTargetParameterNameLeft, PortalRenderTargetLeft);
    DynMat->SetTextureParameterValue(RenderTargetParameterNameRight, PortalRenderTargetRight);
    PortalHollowCubeMesh->SetHiddenInGame(true, true);

    GetWorld()->GetSubsystem<UPortalSubsystem>()->ActivePortalsXR.Add(this);
    count = 0;   

    //if (OtherPortal)PortalTools::ConnectPortalPair(this, OtherPortal, true);
    //else PortalTools::DisconnectPortalPair(this, true);
}


// Called every frame
void APortalVR::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    leftImageRendered = false;
    rightImageRendered = false;

    //DrawDebugLine( GetWorld(),this->GetActorLocation(), OtherPortal->GetActorLocation(), FColor::Blue, true, 0.1f, 0, 5.f);

    if (count == 5)
    {
        if (GEngine && GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected())
        {
            IStereoRendering* StereoDevice = GEngine->XRSystem->GetStereoRenderingDevice().Get();
            if (StereoDevice)
            {
                SceneCaptureComponent2DLeft->CustomProjectionMatrix = StereoDevice->GetStereoProjectionMatrix(eSSE_LEFT_EYE);
                SceneCaptureComponent2DRight->CustomProjectionMatrix = StereoDevice->GetStereoProjectionMatrix(eSSE_RIGHT_EYE);
            }
        }
        count++;
    }
    else if( count < 5)
    {
        count++;
    }
}

void APortalVR::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);    

    if (OtherPortal != OldOtherPortalInEditor)
    {
        if (OtherPortal) PortalTools::XR::ConnectPortalPairXR(this, OtherPortal);
        else PortalTools::XR::DisconnectPortalPairXR(this);
    }
}

void APortalVR::OnPortalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!ActorsToCheckTeleport.Contains(OtherActor))
    {
        ActorsToCheckTeleport.Add(OtherActor);
    }
    PortalHollowCubeMesh->SetHiddenInGame(false, true);
    PortalMesh->SetHiddenInGame(true, true);
    GEngine->GameViewport->EngineShowFlags.DisableOcclusionQueries = true;
}

void APortalVR::OnPortalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ActorsToCheckTeleport.Remove(OtherActor);
    PortalHollowCubeMesh->SetHiddenInGame(true, true);
    PortalMesh->SetHiddenInGame(false, true);
    GEngine->GameViewport->EngineShowFlags.DisableOcclusionQueries = false;

}





