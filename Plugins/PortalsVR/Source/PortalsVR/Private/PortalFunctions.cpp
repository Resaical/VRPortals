// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalFunctions.h"

namespace
{
    bool UnlinkPortalReferencesXR(APortalVR* Portal)
    {
        if (!Portal) return false;

        Portal->SceneCaptureComponent2DLeft->TextureTarget = nullptr;
        Portal->SceneCaptureComponent2DRight->TextureTarget = nullptr;

        Portal->OtherPortal = nullptr;
        Portal->OldOtherPortalInEditor = nullptr;

        return true;
    }
    bool LinkPortalReferencesXR(APortalVR* PortalA, APortalVR* PortalB)
    {
        if (!PortalA || !PortalB) return false;

        PortalA->OtherPortal = PortalB;
        PortalA->OldOtherPortalInEditor = PortalB;

        PortalB->OtherPortal = PortalA;
        PortalB->OldOtherPortalInEditor = PortalA;

        PortalA->SceneCaptureComponent2DLeft->TextureTarget = PortalB->PortalRenderTargetLeft;
        PortalA->SceneCaptureComponent2DRight->TextureTarget = PortalB->PortalRenderTargetRight;

        PortalB->SceneCaptureComponent2DLeft->TextureTarget = PortalA->PortalRenderTargetLeft;
        PortalB->SceneCaptureComponent2DRight->TextureTarget = PortalA->PortalRenderTargetRight;

        return true;
    }

    bool UnlinkPortalReferences(APortal* Portal)
    {
        if (!Portal) return false;

        Portal->SceneCaptureComponent2D->TextureTarget = nullptr;

        Portal->OtherPortal = nullptr;
        Portal->OldOtherPortalInEditor = nullptr;

        return true;
    }
    bool LinkPortalReferences(APortal* PortalA, APortal* PortalB)
    {
        if (!PortalA || !PortalB) return false;

        PortalA->OtherPortal = PortalB;
        PortalA->OldOtherPortalInEditor = PortalB;

        PortalB->OtherPortal = PortalA;
        PortalB->OldOtherPortalInEditor = PortalA;

        PortalA->SceneCaptureComponent2D->TextureTarget = PortalB->PortalRenderTarget;
        PortalB->SceneCaptureComponent2D->TextureTarget = PortalA->PortalRenderTarget;

        return true;
    }
}

bool PortalTools::XR::TeleportWorldLocationXR(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut)
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
                  
bool PortalTools::XR::TeleportWorldRotationXR(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldRotationIn, FQuat& worldRotationOut)
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
                 
bool PortalTools::XR::TeleportActorTroughPortalMirroredXR(AActor* Actor, const APortalVR* PortalIn)
{
    if (!PortalIn || !Actor || !PortalIn->OtherPortal) return false;
    FVector ActorWorldLocation = Actor->GetActorLocation();
    FVector TeleportedLocation;
    PortalTools::XR::TeleportWorldLocationMirroredXR(PortalIn, PortalIn->OtherPortal, ActorWorldLocation, TeleportedLocation);

    FRotator ActorWorldRotation = Actor->GetActorRotation();
    FQuat TeleportedRotator;
    PortalTools::XR::TeleportWorldRotationMirroredXR(PortalIn, PortalIn->OtherPortal, ActorWorldRotation.Quaternion(), TeleportedRotator);

    Actor->SetActorLocation(TeleportedLocation);
    Actor->SetActorRotation(TeleportedRotator.Rotator());
    return true;
}
               
bool PortalTools::XR::TeleportWorldLocationMirroredXR(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut)
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
               
bool PortalTools::XR::TeleportWorldRotationMirroredXR(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut)
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

bool PortalTools::XR::ConnectPortalPairXR(APortalVR* PortalA, APortalVR* PortalB)
{
    if (!PortalA || !PortalB) return false;

    DisconnectPortalPairXR(PortalA);
    DisconnectPortalPairXR(PortalB);

    LinkPortalReferencesXR(PortalA, PortalB);
    return true;
}

bool PortalTools::XR::DisconnectPortalPairXR(APortalVR* Portal)
{
    if (!Portal) return false;

    if (Portal->OldOtherPortalInEditor) UnlinkPortalReferencesXR(Portal->OldOtherPortalInEditor);
    UnlinkPortalReferencesXR(Portal);

    return true;
}


bool PortalTools::FlatScreen::TeleportWorldLocation(const APortal* portalIn, const APortal* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut)
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

bool PortalTools::FlatScreen::TeleportWorldRotation(const APortal* portalIn, const APortal* portalOut, const FQuat& worldRotationIn, FQuat& worldRotationOut)
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

bool PortalTools::FlatScreen::TeleportActorTroughPortalMirrored(AActor* Actor, const APortal* PortalIn)
{
    if (!PortalIn || !Actor || !PortalIn->OtherPortal) return false;
    FVector ActorWorldLocation = Actor->GetActorLocation();
    FVector TeleportedLocation;
    PortalTools::FlatScreen::TeleportWorldLocationMirrored(PortalIn, PortalIn->OtherPortal, ActorWorldLocation, TeleportedLocation);

    FRotator ActorWorldRotation = Actor->GetActorRotation();
    FQuat TeleportedRotator;
    PortalTools::FlatScreen::TeleportWorldRotationMirrored(PortalIn, PortalIn->OtherPortal, ActorWorldRotation.Quaternion(), TeleportedRotator);

    Actor->SetActorLocation(TeleportedLocation);
    Actor->SetActorRotation(TeleportedRotator.Rotator());
    return true;
}

bool PortalTools::FlatScreen::TeleportWorldLocationMirrored(const APortal* portalIn, const APortal* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut)
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

bool PortalTools::FlatScreen::TeleportWorldRotationMirrored(const APortal* portalIn, const APortal* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut)
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

bool PortalTools::FlatScreen::ConnectPortalPair(APortal* PortalA, APortal* PortalB)
{
    if (!PortalA || !PortalB) return false;

    DisconnectPortalPair(PortalA);
    DisconnectPortalPair(PortalB);

    LinkPortalReferences(PortalA, PortalB);
    return true;
}

bool PortalTools::FlatScreen::DisconnectPortalPair(APortal* Portal)
{
    if (!Portal) return false;

    if (Portal->OldOtherPortalInEditor) UnlinkPortalReferences(Portal->OldOtherPortalInEditor);
    UnlinkPortalReferences(Portal);

    return true;
}

