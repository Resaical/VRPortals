#include "PortalViewExtension.h"
#include "Logging/LogMacros.h"
#include "PortalSubsystem.h"
#include <Portal.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/TextureRenderTarget2D.h>
#include "Runtime/Renderer/Private/SceneRendering.h"
#include <Camera/CameraComponent.h>
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "IXRCamera.h"
#include "SceneView.h"                           
#include "SceneCaptureRendering.h" 
#include <PortalFunctions.h>

FPortalViewExtension::FPortalViewExtension(const FAutoRegister& AutoRegister)
    : FSceneViewExtensionBase(AutoRegister)
{
}

int i = 0;

// 0 is LEFT, 1 is RIGHT
std::unordered_map<int, std::pair<APortalVR*, int>> PortalsSceneCapturesMap;

void FPortalViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{      
    FSceneInterface* Scene = InView.Family->Scene;
    if (!Scene) return;
    UWorld* World = Scene->GetWorld();
    if (!World) return;

    UPortalSubsystem* Subsystem = World->GetSubsystem<UPortalSubsystem>();
    if (!Subsystem) return;
     
    if (Subsystem->FirstFSceneViewExtensionPass)
    {
        Subsystem->FirstFSceneViewExtensionPass = false;
        PortalsSceneCapturesMap.clear();

        for (TWeakObjectPtr<APortalVR> Portal : Subsystem->ActivePortals)
        {
            if (Portal.Get())
            {
                if (!Portal->OtherPortal) continue;

                // 0 is LEFT, 1 is RIGHT
                int key = Portal->SceneCaptureComponent2DLeft->GetViewState(0)->GetViewKey();
                PortalsSceneCapturesMap.insert({ key, {Portal.Get(), 0}});
                key = Portal->SceneCaptureComponent2DRight->GetViewState(0)->GetViewKey();
                PortalsSceneCapturesMap.insert({ key, {Portal.Get(), 1} });
            }
        }
    }

    i = 0;

#if PORTAL_VIEW_EXTENSION_STEREOSCOPIC_IN_CHARGE_GAME_THREAT

    if ((InView.StereoViewIndex != eSSE_LEFT_EYE) && (InView.StereoViewIndex != eSSE_RIGHT_EYE)) return;    

    bool leftEye = InView.StereoViewIndex == eSSE_LEFT_EYE;    

    if (UPortalSubsystem* Subsystem = World->GetSubsystem<UPortalSubsystem>())
    {
        if (Subsystem->leftRendered && leftEye) return;
        if (Subsystem->rightRendered && !leftEye)return;

        for (TWeakObjectPtr<APortalVR> Portal : Subsystem->ActivePortals)
        {
            if (Portal.Get())
            {
                auto Target = (leftEye) ? Portal->PortalRenderTargetLeft : Portal->PortalRenderTargetRight;
                auto sceneCapture = (leftEye) ? Portal->OtherPortal->SceneCaptureComponent2DLeft : Portal->OtherPortal->SceneCaptureComponent2DRight;



                //New method
                auto PlayerController = World->GetFirstPlayerController();
                auto PlayerPawn = PlayerController->GetPawn();

                int32 HMDDeviceId = IXRTrackingSystem::HMDDeviceId;

                FVector realHMDLocation;
                FQuat realHMDRotation;

                GEngine->XRSystem->GetCurrentPose(HMDDeviceId, realHMDRotation, realHMDLocation);
                auto WorldCamLocation = PlayerPawn->ActorToWorld().TransformPosition(realHMDLocation);
                auto WorldCamRotation = PlayerPawn->ActorToWorld().TransformRotation(realHMDRotation);

                FQuat offsetRotator = FQuat::Identity;
                FVector offsetLocation = FVector(0, 0, 0);

                GEngine->XRSystem->GetRelativeEyePose(HMDDeviceId, InView.StereoViewIndex, offsetRotator, offsetLocation);

                FVector EyeLocation = WorldCamRotation.RotateVector(offsetLocation) + WorldCamLocation;
                FTransform EyeTransform(WorldCamRotation, EyeLocation);

                //

                FMatrix proj = InView.ViewMatrices.GetProjectionMatrix();
                FVector EyeLocationt = InView.ViewMatrices.GetViewOrigin();
                //FTransform EyeTransform(InView.ViewRotation, EyeLocation);
                FMatrix EyeWorld = EyeTransform.ToMatrixWithScale();

                auto portalWorldToLocal = Portal->GetTransform().ToMatrixWithScale().Inverse();
                FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
                auto otherPortalWorld = Portal->OtherPortal->GetTransform().ToMatrixWithScale();

                auto portalTransformMatrix = EyeWorld * portalWorldToLocal * RotMatrix * otherPortalWorld;

                auto otherPortalCameraNewLocation = portalTransformMatrix.GetOrigin();
                auto otherPortalCameraNewRotation = portalTransformMatrix.Rotator();

                sceneCapture->SetWorldLocationAndRotation(otherPortalCameraNewLocation, otherPortalCameraNewRotation);
                sceneCapture->TextureTarget = Target;
                sceneCapture->CustomProjectionMatrix = proj;

                if (leftEye) Subsystem->leftRendered = true;
                else Subsystem->rightRendered = true;

                UE_LOG(LogTemp, Log, TEXT("Pose HMD : %s\n"), *EyeLocation.ToString());

                UE_LOG(LogTemp, Log, TEXT("View Get Origin : %s\n"), *EyeLocationt.ToString());
            }
        }
    }

#endif
}


void FPortalViewExtension::PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView)
{
    FSceneInterface* Scene = InView.Family->Scene;
    if (!Scene) return;
    UWorld* World = Scene->GetWorld();
    if (!World || !World->IsGameWorld()) return;

    UPortalSubsystem* Subsystem = World->GetSubsystem<UPortalSubsystem>();
    if (!Subsystem) return;

#if PORTAL_REVERT_LATE_XRHMD_UPDATE

    if ((InView.StereoViewIndex != eSSE_LEFT_EYE) && (InView.StereoViewIndex != eSSE_RIGHT_EYE)) return;

    auto eyeTransform = (InView.StereoViewIndex == eSSE_LEFT_EYE) ? Subsystem->lateUpdatedRightEyeWorldTransform : Subsystem->lateUpdatedRightEyeWorldTransform;

    InView.ViewLocation = eyeTransform.GetLocation();
    InView.ViewRotation = eyeTransform.Rotator();
    InView.UpdateViewMatrix();

#endif

#if PORTAL_VIEW_EXTENSION_STEREOSCOPIC_IN_CHARGE_RENDER_THREAT 

    if (InView.StereoViewIndex == eSSE_LEFT_EYE || InView.StereoViewIndex == eSSE_RIGHT_EYE) return;
             
    int key = InView.GetViewKey();

    auto it = PortalsSceneCapturesMap.find(key);
    if (it == PortalsSceneCapturesMap.end()) return;

    int EyeIndex = it->second.second;
    APortalVR* Portal = it->second.first;

    if (!Portal->OtherPortal) return;
        
    if (EyeIndex == 0 && Portal->leftImageRendered) return;
    if (EyeIndex == 1 && Portal->rightImageRendered) return;

    auto PlayerController = Scene->GetWorld()->GetFirstPlayerController();
    auto PlayerPawn = PlayerController->GetPawn();

    int32 HMDDeviceId = IXRTrackingSystem::HMDDeviceId;

    FVector CamLocationLocal = FVector::Zero();
    FQuat CamRotationLocal = FQuat::Identity;

    GEngine->XRSystem->GetCurrentPose(HMDDeviceId, CamRotationLocal, CamLocationLocal);

    GEngine->XRSystem->GetCurrentPose(HMDDeviceId, CamRotationLocal, CamLocationLocal);
    auto CamLocation = PlayerPawn->ActorToWorld().TransformPosition(CamLocationLocal);
    auto CamRotation = PlayerPawn->ActorToWorld().TransformRotation(CamRotationLocal);

    FQuat offsetRotator = FQuat::Identity;
    FVector offsetLocation = FVector(0, 0, 0);

    GEngine->XRSystem->GetRelativeEyePose(HMDDeviceId,EyeIndex, offsetRotator, offsetLocation);

    FVector EyeLocation = CamRotation.RotateVector(offsetLocation) + CamLocation;
    FTransform EyeTransform(CamRotation, EyeLocation);
    FMatrix EyeWorld = EyeTransform.ToMatrixWithScale();

    FVector eyeSyncronizedLocation;
    PortalTools::Teleport::TeleportWorldLocationMirrored(Portal->OtherPortal, Portal, EyeLocation, eyeSyncronizedLocation);
    FQuat eyeSyncronizedRotation;
    PortalTools::Teleport::TeleportWorldRotationMirrored(Portal->OtherPortal, Portal, CamRotation, eyeSyncronizedRotation);

    auto portalWorldToLocal = Portal->OtherPortal->GetTransform().ToMatrixWithScale().Inverse();
    FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
    auto otherPortalWorld = Portal->GetTransform().ToMatrixWithScale();

    auto portalTransformMatrix = EyeWorld * portalWorldToLocal * RotMatrix * otherPortalWorld;

    auto otherPortalCameraNewLocation = portalTransformMatrix.GetOrigin();
    auto otherPortalCameraNewRotation = portalTransformMatrix.Rotator();

    auto proj = GEngine->XRSystem->GetStereoRenderingDevice()->GetStereoProjectionMatrix(EyeIndex);


    InView.ViewLocation = eyeSyncronizedLocation;
    InView.ViewRotation = eyeSyncronizedRotation.Rotator();
    InView.UpdateProjectionMatrix(proj);
    InView.UpdateViewMatrix();

    if (EyeIndex == 0) Portal->leftImageRendered = true;
    else Portal->rightImageRendered = true;
    
#endif

}

void FPortalViewExtension::PreRenderBasePass_RenderThread(FRDGBuilder& GraphBuilder, bool bDepthBufferIsPopulated)
{

    
}
