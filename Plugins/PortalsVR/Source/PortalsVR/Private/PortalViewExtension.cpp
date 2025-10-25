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

        for (TWeakObjectPtr<APortalVR> Portal : Subsystem->ActivePortalsXR)
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
}


void FPortalViewExtension::PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView)
{
    FSceneInterface* Scene = InView.Family->Scene;
    if (!Scene) return;
    UWorld* World = Scene->GetWorld();
    if (!World || !World->IsGameWorld()) return;

    UPortalSubsystem* Subsystem = World->GetSubsystem<UPortalSubsystem>();
    if (!Subsystem) return;

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
    auto CamLocation = PlayerPawn->ActorToWorld().TransformPosition(CamLocationLocal);
    auto CamRotation = PlayerPawn->ActorToWorld().TransformRotation(CamRotationLocal);

    FQuat offsetRotator = FQuat::Identity;
    FVector offsetLocation = FVector(0, 0, 0);

    GEngine->XRSystem->GetRelativeEyePose(HMDDeviceId,EyeIndex, offsetRotator, offsetLocation);

    FVector EyeLocation = CamRotation.RotateVector(offsetLocation) + CamLocation;

    FVector eyeSyncronizedLocation;
    PortalTools::XR::TeleportWorldLocationMirroredXR(Portal->OtherPortal, Portal, EyeLocation, eyeSyncronizedLocation);
    FQuat eyeSyncronizedRotation;
    PortalTools::XR::TeleportWorldRotationMirroredXR(Portal->OtherPortal, Portal, CamRotation, eyeSyncronizedRotation);


    auto proj = GEngine->XRSystem->GetStereoRenderingDevice()->GetStereoProjectionMatrix(EyeIndex);

    InView.ViewLocation = eyeSyncronizedLocation;
    InView.ViewRotation = eyeSyncronizedRotation.Rotator();
    InView.UpdateProjectionMatrix(proj);
    InView.UpdateViewMatrix();

    if (EyeIndex == 0) Portal->leftImageRendered = true;
    else Portal->rightImageRendered = true;    
}

