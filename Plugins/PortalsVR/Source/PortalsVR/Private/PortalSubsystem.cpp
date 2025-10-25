#include "PortalSubsystem.h"
#include "PortalVR.h"
#include <Engine/TextureRenderTarget2D.h>
#include "GameFramework/GameUserSettings.h"
#include <Camera/CameraComponent.h>
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "IXRCamera.h"
#include <PortalFunctions.h>


void UPortalSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    if (GEngine && GEngine->XRSystem && GEngine->XRSystem->GetHMDDevice()->IsHMDConnected())
    {
        IsXRGame = true;
        ViewExtension = FSceneViewExtensions::NewExtension<FPortalViewExtension>();
    }
}

void UPortalSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
   
    FirstFSceneViewExtensionPass = true;

    if (IsXRGame)
    {
        for (auto P : ActivePortalsXR)
        {
            if (!P->OtherPortal) continue;

            TArray<AActor*> SafeCopy = P->ActorsToCheckTeleport;
            if (SafeCopy.IsEmpty())continue;

            for (auto Actor : SafeCopy)
            {
                auto PlayerController = GetWorld()->GetFirstPlayerController();
                auto PlayerPawn = PlayerController->GetPawn();

                bool IsPlayer = false;
                if (Actor == PlayerPawn) IsPlayer = true;

                FVector LocationCheckTeleport = IsPlayer ? PlayerController->PlayerCameraManager->GetCameraLocation() : Actor->GetActorLocation();
                FVector PortalForward = P->GetActorForwardVector();
                FVector PortalToActor = (LocationCheckTeleport - P->GetActorLocation()).GetSafeNormal();

                auto dot = FVector::DotProduct(PortalForward, PortalToActor);
                if (dot >= 0) PortalTools::XR::TeleportActorTroughPortalMirroredXR(Actor, P.Get());
            }
        }
    }
    else
    {
        for (auto P : ActivePortals)
        {
            if (!P->OtherPortal) continue;

            TArray<AActor*> SafeCopy = P->ActorsToCheckTeleport;
            if (SafeCopy.IsEmpty())continue;

            for (auto Actor : SafeCopy)
            {
                auto PlayerController = GetWorld()->GetFirstPlayerController();
                auto PlayerPawn = PlayerController->GetPawn();

                bool IsPlayer = false;
                if (Actor == PlayerPawn) IsPlayer = true;

                FVector LocationCheckTeleport = IsPlayer ? PlayerController->PlayerCameraManager->GetCameraLocation() : Actor->GetActorLocation();
                FVector PortalForward = P->GetActorForwardVector();
                FVector PortalToActor = (LocationCheckTeleport - P->GetActorLocation()).GetSafeNormal();

                auto dot = FVector::DotProduct(PortalForward, PortalToActor);
                if (dot >= 0) PortalTools::FlatScreen::TeleportActorTroughPortalMirrored(Actor, P.Get());
            }
        }

    }
}

