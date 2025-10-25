// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Portal.h>
#include <PortalVR.h>

struct PortalTools
{

	static bool ConnectPortalPair(APortalVR* PortalA, APortalVR* PortalB, bool IsXR);
	static bool DisconnectPortalPair(APortalVR* Portal, bool IsXR);

	struct Teleport
	{		
		static bool TeleportWorldLocation(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut);
		static bool TeleportWorldRotation(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut);
		static bool TeleportActor(AActor* ActorToTeleport, const APortalVR* PortalIn);

		static bool TeleportWorldLocationMirrored(const APortalVR* portalIn, const APortalVR* portalOut, const FVector& worldLocationIn, FVector& worldLocationOut);
		static bool TeleportWorldRotationMirrored(const APortalVR* portalIn, const APortalVR* portalOut, const FQuat& worldLocationIn, FQuat& worldLocationOut);
		
	};
	//static AActor* CreateVisualCopy(AActor* SourceActor, APortal* portalIn, APortal* portalOut);
};