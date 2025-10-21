

#include "PortalVR.h"
#include <PortalSubsystem.h>
#include <Engine/TextureRenderTarget2D.h>
#include "GameFramework/GameUserSettings.h"
#include <Camera/CameraComponent.h>
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "IXRCamera.h"

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


        SceneCaptureComponent2DRight = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D Right"));
        SceneCaptureComponent2DRight->SetupAttachment(RootComponent);
        SceneCaptureComponent2DRight->bUseCustomProjectionMatrix = true;
        SceneCaptureComponent2DRight->bCaptureEveryFrame = true;
        SceneCaptureComponent2DRight->SetTickGroup(TG_PostPhysics);
        SceneCaptureComponent2DRight->HideComponent(PortalMesh);
        SceneCaptureComponent2DRight->HideComponent(PortalHollowCubeMesh);

    }

    //Render Target
    {
        PortalRenderTargetLeft = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("Portal render target Left"));
        PortalRenderTargetLeft->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // 8-bit per channel
        PortalRenderTargetLeft->InitAutoFormat(1024, 1024);
        PortalRenderTargetLeft->UpdateResourceImmediate(true);

        PortalRenderTargetRight = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("Portal render target Right"));
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

    if (GEngine && GEngine->XRSystem)
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

    GetWorld()->GetSubsystem<UPortalSubsystem>()->ActivePortals.Add(this);
    count = 0;   
}


// Called every frame
void APortalVR::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    leftImageRendered = false;
    rightImageRendered = false;

    if (count == 5)
    {
        if (GEngine && GEngine->XRSystem.IsValid())
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

#if PORTAL_ACTOR_STEREOSCOPIC_IN_CHARGE 

    for (int eyeIndex = eSSE_LEFT_EYE; eyeIndex <= eSSE_RIGHT_EYE; eyeIndex++)
    {
        auto Target = (eyeIndex == eSSE_LEFT_EYE) ? PortalRenderTargetLeft : PortalRenderTargetRight;
        auto sceneCapture = (eyeIndex == eSSE_LEFT_EYE) ? OtherPortal->SceneCaptureComponent2DLeft : OtherPortal->SceneCaptureComponent2DRight;

        auto proj = GEngine->XRSystem->GetStereoRenderingDevice()->GetStereoProjectionMatrix(eyeIndex);


        auto PlayerController = GetWorld()->GetFirstPlayerController();
        auto PlayerPawn = PlayerController->GetPawn();

        int32 HMDDeviceId = IXRTrackingSystem::HMDDeviceId;

        FVector CamLocationLocal = FVector::Zero();
        FQuat CamRotationLocal = FQuat::Identity;

        GEngine->XRSystem->GetCurrentPose(HMDDeviceId, CamRotationLocal, CamLocationLocal);
        auto CamLocation = PlayerPawn->ActorToWorld().TransformPosition(CamLocationLocal);
        auto CamRotation = PlayerPawn->ActorToWorld().TransformRotation(CamRotationLocal);

        FQuat offsetRotator = FQuat::Identity;
        FVector offsetLocation = FVector(0,0,0);

        GEngine->XRSystem->GetRelativeEyePose(HMDDeviceId, eyeIndex, offsetRotator, offsetLocation);  

        FVector EyeLocation = CamRotation.RotateVector(offsetLocation) + CamLocation;
        FTransform EyeTransform(CamRotation, EyeLocation);
        FMatrix EyeWorld = EyeTransform.ToMatrixWithScale();

        if (eyeIndex == eSSE_LEFT_EYE) 
        {
            UE_LOG(LogTemp, Log, TEXT("Cam Location HMD : %s\n"), *CamLocation.ToString());
        }

        auto portalWorldToLocal = GetTransform().ToMatrixWithScale().Inverse();
        FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
        auto otherPortalWorld = OtherPortal->GetTransform().ToMatrixWithScale();

        auto portalTransformMatrix = EyeWorld * portalWorldToLocal * RotMatrix * otherPortalWorld;

        auto otherPortalCameraNewLocation = portalTransformMatrix.GetOrigin();
        auto otherPortalCameraNewRotation = portalTransformMatrix.Rotator();

        sceneCapture->CustomProjectionMatrix = proj;
        sceneCapture->SetWorldLocationAndRotation(otherPortalCameraNewLocation, otherPortalCameraNewRotation);

    #if PORTAL_REVERT_LATE_XRHMD_UPDATE

        if(eyeIndex == eSSE_LEFT_EYE) GetWorld()->GetSubsystem<UPortalSubsystem>()->lateUpdatedLeftEyeWorldTransform = EyeTransform;
        else GetWorld()->GetSubsystem<UPortalSubsystem>()->lateUpdatedRightEyeWorldTransform = EyeTransform;

    #endif
    }

#endif
}

void APortalVR::OnXRLateUpdate()
{

}

void APortalVR::ConnectToPortal(APortalVR* otherPortal)
{
    OtherPortal = otherPortal;
    otherPortal->SceneCaptureComponent2DLeft->TextureTarget = PortalRenderTargetLeft;
    otherPortal->SceneCaptureComponent2DRight->TextureTarget = PortalRenderTargetRight;

    //OtherPortal->SceneCaptureComponent2DLeft->HiddenActors.Add(this);
    //OtherPortal->SceneCaptureComponent2DRight->HiddenActors.Add(this);

}

void APortalVR::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (OtherPortal)
    {
        ConnectToPortal(OtherPortal);
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

void APortalVR::Teleport(AActor* Actor)
{
    if (!OtherPortal || !Actor) return;

    auto ActorWorldMatrix = Actor->ActorToWorld().ToMatrixWithScale();
    auto portalWorldToLocalMatrix = GetTransform().ToMatrixWithScale().Inverse();
    FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
    auto otherPortalWorldMatrix = OtherPortal->GetTransform().ToMatrixWithScale();

    auto portalTransformMatrix = ActorWorldMatrix * portalWorldToLocalMatrix * RotMatrix * otherPortalWorldMatrix;

    auto otherPortalActorNewLocation = portalTransformMatrix.GetOrigin();
    auto otherPortalActorNewRotation = portalTransformMatrix.Rotator();

    Actor->SetActorLocation(otherPortalActorNewLocation);
    Actor->SetActorRotation(otherPortalActorNewRotation);
}




