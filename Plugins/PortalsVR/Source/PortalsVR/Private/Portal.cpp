// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Engine/TextureRenderTarget2D.h"
#include <Camera/CameraComponent.h>
#include "GameFramework/GameUserSettings.h"
#include <PortalSubsystem.h>

// Sets default values
APortal::APortal()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));

    // Mesh
    {
        //static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
        //if (PlaneMesh.Succeeded()) DefaultPortalMesh = PlaneMesh.Object;

        PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
        PortalMesh->SetupAttachment(RootComponent);
        PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        //PortalMesh->SetStaticMesh(DefaultPortalMesh);
        //PortalMesh->SetMobility(EComponentMobility::Movable);
        //PortalMesh->bEditableWhenInherited = true;

        //PortalMesh->SetRelativeLocation({ 0, 0, 100 });
        //FQuat defaultRot = FRotator(0, 90, 90).Quaternion();
        //PortalMesh->SetRelativeRotation(defaultRot);
        //PortalMesh->SetRelativeScale3D({ 1, 2, 1 });
    }    

    //SceneCapture2D
    {
        SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture 2D"));
        SceneCaptureComponent2D->SetupAttachment(RootComponent);
        SceneCaptureComponent2D->bUseCustomProjectionMatrix = true;

    }

    //Render Target
    {
        PortalRenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("Portal render target"));
        PortalRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // 8-bit per channel
        PortalRenderTarget->InitAutoFormat(1024, 1024);
        PortalRenderTarget->UpdateResourceImmediate(true);
    }

    //Material
    {

    }
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
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

    //GetWorld()->GetSubsystem<UPortalSubsystem>()->ActivePortals.Add(this);
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    //Matrix multiplication to syncronize portal camera
    {
        if (OtherPortal)
        {
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();            
            APawn* PlayerPawn = PlayerController->GetPawn();                
            UCameraComponent* CameraComponent = PlayerPawn->FindComponentByClass<UCameraComponent>();
            FTransform CameraTransform = CameraComponent->GetComponentTransform();

            auto playerCameraWorld = CameraTransform.ToMatrixWithScale();
            auto portalWorldToLocal = GetTransform().ToMatrixWithScale().Inverse();
            FMatrix RotMatrix = FRotationMatrix(FRotator(0,180.0f,0));
            auto otherPortalWorld = OtherPortal->GetTransform().ToMatrixWithScale();

            auto portalTransformMatrix = playerCameraWorld * portalWorldToLocal * RotMatrix * otherPortalWorld;

            auto otherPortalCameraNewLocation = portalTransformMatrix.GetOrigin();
            auto otherPortalCameraNewRotation = portalTransformMatrix.Rotator();

            UE_LOG(LogTemp, Warning, TEXT("Other portal camera pos = %s"), *otherPortalCameraNewLocation.ToString());

            OtherPortal->SceneCaptureComponent2D->SetWorldLocationAndRotation(otherPortalCameraNewLocation, otherPortalCameraNewRotation);
        }
        
    }    
}

void APortal::ConnectToPortal(APortal* otherPortal)
{
    OtherPortal = otherPortal;
    otherPortal->SceneCaptureComponent2D->TextureTarget = PortalRenderTarget;
    OtherPortal->SceneCaptureComponent2D->HiddenActors.Add(this);
}

void APortal::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (OtherPortal)
    {
        ConnectToPortal(OtherPortal);
    }
}



