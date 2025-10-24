#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PortalViewExtension.h"
#include "Portal.h"
#include "PortalVR.h"
#include "PortalSubsystem.generated.h"

#define PORTAL_SUBSYSTEM_STEREOSCOPIC_IN_CHARGE 0

UCLASS()
class UPortalSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override
    {
        Super::Initialize(Collection);
    }

    virtual void Deinitialize() override
    {
        ViewExtension.Reset();

        Super::Deinitialize();
    }

    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UPortalSubsystem, STATGROUP_Tickables);
    }

    virtual void OnWorldBeginPlay(UWorld& InWorld) override;



    virtual void Tick(float DeltaTime) override;


    TSharedPtr<FPortalViewExtension, ESPMode::ThreadSafe> ViewExtension;
    TArray<TWeakObjectPtr<APortalVR>> ActivePortals;

    bool leftRendered = false;
    bool rightRendered = false;

    FTransform lateUpdatedLeftEyeWorldTransform = FTransform::Identity;
    FTransform lateUpdatedRightEyeWorldTransform = FTransform::Identity;

    bool FirstFSceneViewExtensionPass = true;
};
