#pragma once

#include "CoreMinimal.h"
#include <PortalViewExtension.h>
#include <Portal.h>
#include <PortalVR.h>
#include "PortalSubsystem.generated.h"

UCLASS()

class UPortalSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override
    {
        Super::Initialize(Collection);
        ViewExtension = FSceneViewExtensions::NewExtension<FPortalViewExtension>();
    }

    virtual void Deinitialize() override
    {
        ViewExtension.Reset(); // unregister extension
        Super::Deinitialize();
    }    

    TSharedPtr<FPortalViewExtension, ESPMode::ThreadSafe> ViewExtension;
    TArray<TWeakObjectPtr<APortalVR>> ActivePortals;
};