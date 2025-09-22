

#include "PortalVR.h"
#include <PortalSubsystem.h>
#include <Engine/TextureRenderTarget2D.h>
#include "GameFramework/GameUserSettings.h"
#include <Camera/CameraComponent.h>



/// Sets default values
APortalVR::APortalVR()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));

    PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
    PortalMesh->SetupAttachment(RootComponent);
    PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PortalRenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("Portal render target"));
    PortalRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // 8-bit per channel
    PortalRenderTarget->InitAutoFormat(1024, 1024);
    PortalRenderTarget->UpdateResourceImmediate(true);
}

// Called when the game starts or when spawned
void APortalVR::BeginPlay()
{
    Super::BeginPlay();

    FVector2D viewportSize = FVector2D(0, 0);
    if (GEngine && GEngine->GameViewport)
    {
        viewportSize = GEngine->GameUserSettings->GetScreenResolution();
    }
    PortalRenderTarget->ResizeTarget(viewportSize.X, viewportSize.Y);

    UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(PortalMesh->GetMaterial(0), this);
    PortalMesh->SetMaterial(0, DynMat);
    DynMat->SetTextureParameterValue(RenderTargetParameterName, PortalRenderTarget);

    GetWorld()->GetSubsystem<UPortalSubsystem>()->ActivePortals.Add(this);
}

// Called every frame
void APortalVR::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //Matrix multiplication to syncronize portal camera
    {
        if (OtherPortal)
        {
            //APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            //APawn* PlayerPawn = PlayerController->GetPawn();
            //UCameraComponent* CameraComponent = PlayerPawn->FindComponentByClass<UCameraComponent>();
            //FTransform CameraTransform = CameraComponent->GetComponentTransform();

            //auto playerCameraWorld = CameraTransform.ToMatrixWithScale();
            //auto portalWorldToLocal = GetTransform().ToMatrixWithScale().Inverse();
            //FMatrix RotMatrix = FRotationMatrix(FRotator(0, 180.0f, 0));
            //auto otherPortalWorld = OtherPortal->GetTransform().ToMatrixWithScale();

            //auto portalTransformMatrix = playerCameraWorld * portalWorldToLocal * RotMatrix * otherPortalWorld;

            //auto otherPortalCameraNewLocation = portalTransformMatrix.GetOrigin();
            //auto otherPortalCameraNewRotation = portalTransformMatrix.Rotator();

            //UE_LOG(LogTemp, Warning, TEXT("Other portal camera pos = %s"), *otherPortalCameraNewLocation.ToString());

        }

    }
}

void APortalVR::ConnectToPortal(APortalVR* otherPortal)
{
    OtherPortal = otherPortal;
}

void APortalVR::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (OtherPortal)
    {
        ConnectToPortal(OtherPortal);
    }
}

