// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalFunctions.h"

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

//AActor* PortalTools::CreateVisualCopy(AActor* SourceActor, APortal* portalIn, APortal* portalOut)
//{
//    if (!SourceActor)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("CreateVisualCopy: Invalid parameters"));
//        return nullptr;
//    }
//
//    UWorld* World = SourceActor->GetWorld();
//    if (!World) return nullptr;
//
//    FVector copyLocation;
//    PortalTools::Teleport::TeleportWorldLocationMirrored(portalIn, portalOut, SourceActor->GetActorLocation(), copyLocation);
//    ATwinPortalCopy* VisualActor = World->SpawnActor<ATwinPortalCopy>(copyLocation, SourceActor->GetActorRotation());
//
//    USceneComponent* Root = VisualActor->GetRootComponent();
//    if (!Root)
//    {
//        Root = NewObject<USceneComponent>(VisualActor, TEXT("RootComponent"));
//        VisualActor->SetRootComponent(Root);
//        Root->RegisterComponent();
//    }
//
//    const TArray<UActorComponent*> Components = SourceActor->GetComponents().Array();
//    for (UActorComponent* Comp : Components)
//    {
//        //SWITCH with different visual structures
//
//        if (UStaticMeshComponent* SrcMesh = Cast<UStaticMeshComponent>(Comp))
//        {
//            UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(VisualActor);
//            NewMesh->SetStaticMesh(SrcMesh->GetStaticMesh());
//
//            for (int32 i = 0; i < SrcMesh->GetNumMaterials(); ++i)
//            {
//                NewMesh->SetMaterial(i, SrcMesh->GetMaterial(i));
//            }
//            NewMesh->SetupAttachment(Root);
//
//            const FTransform RelativeTransform = SrcMesh->GetComponentTransform().GetRelativeTransform(SourceActor->GetActorTransform());
//            NewMesh->SetRelativeTransform(RelativeTransform);
//
//            NewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//            NewMesh->SetSimulatePhysics(false);
//            NewMesh->SetGenerateOverlapEvents(false);
//
//            NewMesh->RegisterComponent();
//        }
//    }
//    VisualActor->SetActorLocation(copyLocation);
//    VisualActor->SetActorRotation(SourceActor->GetActorRotation());
//    VisualActor->SetActorScale3D(SourceActor->GetActorScale3D());
//    return VisualActor;
//}
