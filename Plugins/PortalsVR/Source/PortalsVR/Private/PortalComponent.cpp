// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalComponent.h"

// Sets default values for this component's properties
UPortalComponent::UPortalComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    //// Default plane
    //static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
    //if (PlaneMesh.Succeeded()) DefaultPortalMesh = PlaneMesh.Object; 

    //if (!PortalMesh.Get()) PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
    //PortalMesh->SetStaticMesh(DefaultPortalMesh.Get());
    //PortalMesh->SetupAttachment(this);

}


void UPortalComponent::OnRegister()
{
    Super::OnRegister();    

    //auto ownerRoot = GetOwner()->GetRootComponent();
    //PortalMesh->AttachToComponent(ownerRoot, FAttachmentTransformRules::KeepRelativeTransform);

    //PortalMesh->SetRelativeLocation({ 0, 0, 100 });
    //FQuat defaultRot = FRotator(90,0,90).Quaternion();
    //PortalMesh->SetRelativeRotation(defaultRot);
    //PortalMesh->SetRelativeScale3D({ 1, 2, 1 });
}

// Called when the game starts
void UPortalComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPortalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

