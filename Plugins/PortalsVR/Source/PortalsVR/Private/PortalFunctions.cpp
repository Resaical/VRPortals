// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalFunctions.h"

namespace
{
    bool DisconnectPortal(APortalVR* Portal, bool IsXR)
    {
        if (!Portal) return false;

        if (IsXR)
        {
            Portal->SceneCaptureComponent2DLeft->TextureTarget = nullptr;
            Portal->SceneCaptureComponent2DRight->TextureTarget = nullptr;
        }
        else
        {
            //To fill for flat screen games.
        }
        Portal->OtherPortal = nullptr;
        Portal->OldOtherPortal = nullptr;

        return true;
    }
}

bool PortalTools::Teleport::TeleportWorldLocation(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut)
{
    if (portalIn == nullptr || portalOut == nullptr)
    {
        worldLocationOut = FVector::Zero();
        return false;
    }

    FMatrix portalWorldToLocal = portalIn->GetTransform().ToMatrixWithScale().Inverse();
    auto otherPortalWorld = portalOut->GetTransform().ToMatrixWithScale();

    auto portalMirroredTransformMatrix = portalWorldToLocal * otherPortalWorld;

    worldLocationOut = portalMirroredTransformMatrix.TransformPosition(worldLocationIn);

    return true;
}

bool PortalTools::Teleport::TeleportWorldRotation(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldRotationIn, FQuat& worldRotationOut)
{
    if (portalIn == nullptr || portalOut == nullptr)
    {
        worldRotationOut = FQuat::Identity;
        return false;
    }

    FMatrix portalWorldToLocal = portalIn->GetTransform().ToMatrixWithScale().Inverse();
    auto otherPortalWorld = portalOut->GetTransform().ToMatrixWithScale();

    auto portalMirroredTransformMatrix = portalWorldToLocal * otherPortalWorld;

    FQuat RotationDelta = FQuat(portalMirroredTransformMatrix.ToQuat());
    worldRotationOut = RotationDelta * worldRotationIn;

    return true;
}

bool PortalTools::Teleport::TeleportActor(AActor* Actor, const APortalVR* PortalIn)
{
    if (!PortalIn || !Actor || !PortalIn->OtherPortal) return false;
    FVector ActorWorldLocation = Actor->GetActorLocation();
    FVector TeleportedLocation;
    PortalTools::Teleport::TeleportWorldLocationMirrored(PortalIn, PortalIn->OtherPortal, ActorWorldLocation, TeleportedLocation);

    FRotator ActorWorldRotation = Actor->GetActorRotation();
    FQuat TeleportedRotator;
    PortalTools::Teleport::TeleportWorldRotationMirrored(PortalIn, PortalIn->OtherPortal, ActorWorldRotation.Quaternion(), TeleportedRotator);

    Actor->SetActorLocation(TeleportedLocation);
    Actor->SetActorRotation(TeleportedRotator.Rotator());
    return true;
}

bool PortalTools::Teleport::TeleportWorldLocationMirrored(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut)
{
    if (portalIn == nullptr || portalOut == nullptr)
    {
        worldLocationOut = FVector::Zero();
        return false;
    }

    FMatrix portalWorldToLocal = portalIn->GetTransform().ToMatrixWithScale().Inverse();
    FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
    auto otherPortalWorld = portalOut->GetTransform().ToMatrixWithScale();

    auto portalMirroredTransformMatrix = portalWorldToLocal * RotMatrix * otherPortalWorld;

    worldLocationOut = portalMirroredTransformMatrix.TransformPosition(worldLocationIn);

    return true;
}

bool PortalTools::Teleport::TeleportWorldRotationMirrored(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut)
{
    if (portalIn == nullptr || portalOut == nullptr)
    {
        worldLocationOut = FQuat::Identity;
        return false;
    }

    FMatrix portalWorldToLocal = portalIn->GetTransform().ToMatrixWithScale().Inverse();
    FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
    auto otherPortalWorld = portalOut->GetTransform().ToMatrixWithScale();

    auto portalMirroredTransformMatrix = portalWorldToLocal * RotMatrix * otherPortalWorld;

    FQuat RotationDelta = FQuat(portalMirroredTransformMatrix.ToQuat());
    worldLocationOut = RotationDelta * worldLocationIn;

    return true;

}

bool PortalTools::ConnectPortalPair(APortalVR* PortalA, APortalVR* PortalB, bool IsXR)
{
    if (!PortalA || !PortalB) return false;

    DisconnectPortalPair(PortalA, IsXR);
    DisconnectPortalPair(PortalB, IsXR);

    PortalA->OtherPortal = PortalB;
    PortalA->OldOtherPortal = PortalB;

    PortalB->OtherPortal = PortalA;
    PortalB->OldOtherPortal = PortalA;

    if (IsXR)
    {
        PortalA->SceneCaptureComponent2DLeft->TextureTarget = PortalB->PortalRenderTargetLeft;
        PortalA->SceneCaptureComponent2DRight->TextureTarget = PortalB->PortalRenderTargetRight;
        PortalB->SceneCaptureComponent2DLeft->TextureTarget = PortalA->PortalRenderTargetLeft;
        PortalB->SceneCaptureComponent2DRight->TextureTarget = PortalA->PortalRenderTargetRight;
    }
    else
    {
        //To fill for flat screen games.
    }
    
    return true;
}

bool PortalTools::DisconnectPortalPair(APortalVR* Portal, bool IsXR)
{
    if (!Portal) return false;

    if (Portal->OldOtherPortal) DisconnectPortal(Portal->OldOtherPortal, IsXR);
    DisconnectPortal(Portal, IsXR);

    return true;
}

