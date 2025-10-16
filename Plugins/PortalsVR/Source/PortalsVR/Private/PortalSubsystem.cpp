#include "PortalSubsystem.h"
#include "PortalVR.h"
#include <PortalSubsystem.h>
#include <Engine/TextureRenderTarget2D.h>
#include "GameFramework/GameUserSettings.h"
#include <Camera/CameraComponent.h>
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "IXRCamera.h"



void UPortalSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    #if PORTAL_SUBSYSTEM_STEREOSCOPIC_IN_CHARGE   

    if (IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get())
    {
        if (TSharedPtr<IXRCamera, ESPMode::ThreadSafe> XRCamera = XRSystem->GetXRCamera())
        {
            // Register SceneCapture for late update just like the main camera
            XRCamera->SetupLateUpdate(lateUpdatedLeftEyeWorldTransform);
        }
    }

    #endif
}

void UPortalSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    #if PORTAL_SUBSYSTEM_STEREOSCOPIC_IN_CHARGE

    if (SceneCapture && SceneCapture->SceneProxy)
    {
        ENQUEUE_RENDER_COMMAND(GetLatestEyesTransform)(
            [Proxy = SceneCapture->SceneProxy](FRHICommandListImmediate& RHICmdList)
            {
                if (!Proxy) return;

                // Get the XR camera from the XR system
                if (GEngine->XRSystem.IsValid())
                {
                    TSharedPtr<IXRCamera, ESPMode::ThreadSafe> XRCamera = GEngine->XRSystem->GetXRCamera();
                    if (XRCamera.IsValid())
                    {
                        FMatrix ViewMatrix;
                        FMatrix ProjectionMatrix;

                        // 0 = left eye, 1 = right eye
                        XRCamera->GetViewMatrices_RenderThread(0, ViewMatrix, ProjectionMatrix);

                        // Convert the view matrix to world transform
                        // The view matrix transforms world -> eye, so invert it to get eye -> world
                        FTransform EyeWorldTransform = FTransform(ViewMatrix.InverseFast());

                        FVector EyeLocation = EyeWorldTransform.GetLocation();
                        FQuat EyeRotation = EyeWorldTransform.GetRotation();

                        // Now you have the absolute world-space eye location and rotation
                        // Example: offset the SceneCapture proxy using this
                        FSceneCaptureProxy2D* CaptureProxy = static_cast<FSceneCaptureProxy2D*>(Proxy);
                        CaptureProxy->ProxyTransform.SetLocation(EyeLocation + FVector(10, 0, 0));
                        CaptureProxy->ProxyTransform.SetRotation(EyeRotation);
                    }
                }
            });

    #endif 

    for (auto P : ActivePortals)
    {
        for (auto Actor : P->ActorsToCheckTeleport)
        {
            auto PlayerController = GetWorld()->GetFirstPlayerController();
            auto PlayerPawn = PlayerController->GetPawn();

            bool IsVRPlayer = false;
            if (Actor == PlayerPawn) IsVRPlayer = true;

            FVector LocationCheckTeleport = IsVRPlayer ? PlayerController->PlayerCameraManager->GetCameraLocation() : Actor->GetActorLocation();
            FVector PortalForward = P->GetActorForwardVector();
            FVector PortalToActor = (LocationCheckTeleport - P->GetActorLocation()).GetSafeNormal();

            auto dot = FVector::DotProduct(PortalForward, PortalToActor);

            UE_LOG(LogTemp, Warning, TEXT("Portal Forward = %s"), *PortalForward.ToString());
            UE_LOG(LogTemp, Warning, TEXT("Actor To Portal = %s"), *PortalToActor.ToString());
            UE_LOG(LogTemp, Warning, TEXT("Dot = %f"), dot);

            if (dot >= 0) P->Teleport(Actor);
        }

    }


    leftRendered = false;
    rightRendered = false;
}

