

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
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));

    PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
    PortalMesh->SetupAttachment(RootComponent);
    PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    eyeLeftDebug = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eye left debug"));
    eyeLeftDebug->SetupAttachment(RootComponent);
    eyeLeftDebug->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    eyeRightDebug = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eye right debug"));
    eyeRightDebug->SetupAttachment(RootComponent);
    eyeRightDebug->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    //SceneCapture2D
    {
        SceneCaptureComponent2DLeft = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D Left"));
        SceneCaptureComponent2DLeft->SetupAttachment(RootComponent);
        SceneCaptureComponent2DLeft->bUseCustomProjectionMatrix = true;
        SceneCaptureComponent2DLeft->bCaptureEveryFrame = false;
        SceneCaptureComponent2DLeft->SetTickGroup(TG_PostPhysics);

        SceneCaptureComponent2DRight = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D Right"));
        SceneCaptureComponent2DRight->SetupAttachment(RootComponent);
        SceneCaptureComponent2DRight->bUseCustomProjectionMatrix = true;
        SceneCaptureComponent2DRight->bCaptureEveryFrame = false;
        SceneCaptureComponent2DRight->SetTickGroup(TG_PostPhysics);

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
}

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

    GetWorld()->GetSubsystem<UPortalSubsystem>()->ActivePortals.Add(this);
}

// Called every frame
void APortalVR::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (int eyeIndex = eSSE_LEFT_EYE; eyeIndex <= eSSE_RIGHT_EYE; eyeIndex++)
    {
        auto Target = (eyeIndex == eSSE_LEFT_EYE) ? PortalRenderTargetLeft : PortalRenderTargetRight;
        auto sceneCapture = (eyeIndex == eSSE_LEFT_EYE) ? OtherPortal->SceneCaptureComponent2DLeft : OtherPortal->SceneCaptureComponent2DRight;

        auto proj = GEngine->XRSystem->GetStereoRenderingDevice()->GetStereoProjectionMatrix(eyeIndex);


        auto PlayerController = GetWorld()->GetFirstPlayerController();
        auto CamManager = PlayerController->PlayerCameraManager;
        const auto CameraTransform = CamManager->GetTransform();

        int32 HMDDeviceId = IXRTrackingSystem::HMDDeviceId;

        //FVector CamLocation = FVector::Zero();
        //FQuat CamRotation = FQuat::Identity;
        FVector CamLocation = CameraTransform.GetLocation();
        FQuat CamRotation = CameraTransform.GetRotation();


        GEngine->XRSystem->GetCurrentPose(HMDDeviceId, CamRotation, CamLocation);

        FQuat offsetRotator = FQuat::Identity;
        FVector offsetLocation = FVector(0,0,0);

        GEngine->XRSystem->GetRelativeEyePose(HMDDeviceId, eyeIndex, offsetRotator, offsetLocation);  

        FVector EyeLocation = CamRotation.RotateVector(offsetLocation) + CamLocation;
        FTransform EyeTransform(CamRotation, EyeLocation);
        FMatrix EyeWorld = EyeTransform.ToMatrixWithScale();

        //DEBUG
        auto targetDebugEye = (eyeIndex == eSSE_LEFT_EYE) ? eyeLeftDebug : eyeRightDebug;
        targetDebugEye->SetWorldLocation(EyeLocation + FVector(15, 0, 0));


        if (eyeIndex == eSSE_LEFT_EYE) 
        {
            UE_LOG(LogTemp, Log, TEXT("Cam Location HMD : %s\n"), *CamLocation.ToString());
            UE_LOG(LogTemp, Log, TEXT("PlayerManagerCameraPos : %s \n"), *CameraTransform.GetLocation().ToString());
        }
        else
        {

        }

        //

        auto portalWorldToLocal = GetTransform().ToMatrixWithScale().Inverse();
        FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
        auto otherPortalWorld = OtherPortal->GetTransform().ToMatrixWithScale();

        auto portalTransformMatrix = EyeWorld * portalWorldToLocal * RotMatrix * otherPortalWorld;

        auto otherPortalCameraNewLocation = portalTransformMatrix.GetOrigin();
        auto otherPortalCameraNewRotation = portalTransformMatrix.Rotator();

        sceneCapture->CustomProjectionMatrix = proj;
        sceneCapture->SetWorldLocationAndRotation(otherPortalCameraNewLocation, otherPortalCameraNewRotation);
    }
}

void APortalVR::OnXRLateUpdate()
{

}

void APortalVR::ConnectToPortal(APortalVR* otherPortal)
{
    OtherPortal = otherPortal;
    otherPortal->SceneCaptureComponent2DLeft->TextureTarget = PortalRenderTargetLeft;
    otherPortal->SceneCaptureComponent2DRight->TextureTarget = PortalRenderTargetRight;

    OtherPortal->SceneCaptureComponent2DLeft->HiddenActors.Add(this);
    OtherPortal->SceneCaptureComponent2DRight->HiddenActors.Add(this);

}

void APortalVR::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (OtherPortal)
    {
        ConnectToPortal(OtherPortal);
    }
}


