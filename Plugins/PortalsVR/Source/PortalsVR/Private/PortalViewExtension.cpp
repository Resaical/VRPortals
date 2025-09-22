#include "PortalViewExtension.h"
#include "Logging/LogMacros.h"
#include "PortalSubsystem.h"
#include <Portal.h>
#include <Kismet/GameplayStatics.h>


FPortalViewExtension::FPortalViewExtension(const FAutoRegister& AutoRegister)
    : FSceneViewExtensionBase(AutoRegister)
{
}

void FPortalViewExtension::PreRenderView_RenderThread( FRDGBuilder& GraphBuilder, FSceneView& InView)
{
    const EStereoscopicPass EyePass = InView.StereoPass;

    FSceneInterface* Scene = InView.Family->Scene;
    if (!Scene) return;
    UWorld* World = Scene->GetWorld();
    if (!World) return;

    if (UPortalSubsystem* Subsystem = World->GetSubsystem<UPortalSubsystem>())
    {
        for (TWeakObjectPtr<APortalVR> Portal : Subsystem->ActivePortals)
        {
            if (APortalVR* P = Portal.Get())
            {                
                FMatrix InvView = InView.ViewMatrices.GetViewMatrix().Inverse();
                FTransform CameraTransform(InvView);
                FMatrix proj = InView.ViewMatrices.GetProjectionMatrix();

                //auto sceneCapture = P->OtherPortal->SceneCaptureComponent2D;
                //sceneCapture->SetWorldTransform(CameraTransform);
                //sceneCapture->CustomProjectionMatrix = proj;
            }
        }
    }

    //FTransform ThroughPortalTransform = ComputePortalView(EyePos, EyePass);

    //if (USceneCaptureComponent2D* sceneCapture = PortalActor->SceneCapture)
    //{
    //    sceneCapture->SetWorldLocationAndRotation(
    //        ThroughPortalTransform.GetLocation(),
    //        ThroughPortalTransform.GetRotation().Rotator() );

    //    sceneCapture->bUseCustomProjectionMatrix = true;
    //    sceneCapture->CustomProjectionMatrix = InView.ViewMatrices.GetProjectionMatrix();

    //    sceneCapture->TextureTarget = (EyePass == eSSP_LEFT)
    //        ? PortalActor->LeftEyeRT
    //        : PortalActor->RightEyeRT;

    //    Capture->UpdateContent(); // Forces render into RT
    //}
}