// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Portal.h>
#include <PortalVR.h>

struct PortalTools
{
	struct XR
	{
		static bool ConnectPortalPairXR(APortalVR* PortalA, APortalVR* PortalB);
		static bool DisconnectPortalPairXR(APortalVR* Portal);

		static bool TeleportWorldLocationXR(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut);
		static bool TeleportWorldRotationXR(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut);
		static bool TeleportActorTroughPortalMirroredXR(AActor* ActorToTeleport, const APortalVR* PortalIn);

		static bool TeleportWorldLocationMirroredXR(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut);
		static bool TeleportWorldRotationMirroredXR(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut);
	};

	struct FlatScreen
	{
		static bool ConnectPortalPair(APortal* PortalA, APortal* PortalB);
		static bool DisconnectPortalPair(APortal* Portal);

		static bool TeleportWorldLocation(const APortal* portalIn, const APortal* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut);
		static bool TeleportWorldRotation(const APortal* portalIn, const APortal* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut);			
		static bool TeleportActorTroughPortalMirrored(AActor* ActorToTeleport, const APortal* PortalIn);

		static bool TeleportWorldLocationMirrored(const APortal* portalIn, const APortal* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut);
		static bool TeleportWorldRotationMirrored(const APortal* portalIn, const APortal* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut);
	};
};