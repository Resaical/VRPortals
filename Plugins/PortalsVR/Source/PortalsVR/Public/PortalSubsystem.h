#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PortalViewExtension.h"
#include "Portal.h"
#include "PortalVR.h"
#include "PortalSubsystem.generated.h"

UCLASS()
class UPortalSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override
    {
        Super::Initialize(Collection);

        //ViewExtension = FSceneViewExtensions::NewExtension<FPortalViewExtension>();
    }

    virtual void Deinitialize() override
    {
        //ViewExtension.Reset();

        Super::Deinitialize();
    }

    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UPortalSubsystem, STATGROUP_Tickables);
    }

    virtual void Tick(float DeltaTime) override
    {
        Super::Tick(DeltaTime);

        leftRendered = false;
        rightRendered = false;
    }

    TSharedPtr<FPortalViewExtension, ESPMode::ThreadSafe> ViewExtension;
    TArray<TWeakObjectPtr<APortalVR>> ActivePortals;

    bool leftRendered = false;
    bool rightRendered = false;
};
