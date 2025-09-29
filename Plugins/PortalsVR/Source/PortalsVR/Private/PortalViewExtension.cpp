#include "PortalViewExtension.h"
#include "Logging/LogMacros.h"
#include "PortalSubsystem.h"
#include <Portal.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/TextureRenderTarget2D.h>
#include "Runtime/Renderer/Private/SceneRendering.h"
#include <Camera/CameraComponent.h>

FPortalViewExtension::FPortalViewExtension(const FAutoRegister& AutoRegister)
    : FSceneViewExtensionBase(AutoRegister)
{
}

float FOV = 30.0f * PI / 180.0f;   // radians
float Aspect = 16.0f / 9.0f;       // typical render target aspect ratio
float Near = 0.1f;
float Far = 1000.0f;

float YScale = 1.0f / FMath::Tan(FOV / 2.0f);   // ≈ 3.732
float XScale = YScale / Aspect;                 // ≈ 2.101
float Q = Far / (Far - Near);                   // ≈ 1.0001

FMatrix WrongProj =
FMatrix(
    FPlane(XScale, 0.0f, 0.0f, 0.0f),
    FPlane(0.0f, YScale, 0.0f, 0.0f),
    FPlane(0.0f, 0.0f, Q, 1.0f),
    FPlane(0.0f, 0.0f, -Q * Near, 0.0f)
);

void FPortalViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{   
    FSceneInterface* Scene = InView.Family->Scene;
    if (!Scene) return;
    UWorld* World = Scene->GetWorld();
    if (!World) return;

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


                FMatrix proj = InView.ViewMatrices.GetProjectionMatrix();
                FVector EyeLocation = InView.ViewMatrices.GetViewOrigin();
                FTransform EyeTransform(InView.ViewRotation, EyeLocation);
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
            }            
        }
    }
}