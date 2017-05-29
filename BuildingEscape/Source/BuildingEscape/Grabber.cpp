// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape.h"
#include "Grabber.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	ConfigurePhysicsHandleComponent();
	ConfigureInputComponent();

	// ...
	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty!"));
}

void UGrabber::ConfigurePhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("UPhysicsHandleComponent not found for %s Component within %s"), *GetName(), *GetOwner()->GetName());
	}
}

void UGrabber::ConfigureInputComponent()
{
	Input = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!Input) {
		UE_LOG(LogTemp, Error, TEXT("UInputComponent not found for %s Component within %s"), *GetName(), *GetOwner()->GetName());
	} else {
		//Bind Input
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::GrabOn);
		Input->BindAction("Grab", IE_Released, this, &UGrabber::GrabOff);
	}
}

void UGrabber::GrabOn()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key pressed!"));
	FHitResult HitResult = GetPhysicsBodyByLineTrace();
	if (HitResult.GetActor()) 
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitResult.GetComponent(),
			NAME_None,
			HitResult.GetComponent()->GetOwner()->GetActorLocation(),
			HitResult.GetComponent()->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::GrabOff()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key released!"));
	PhysicsHandle->ReleaseComponent();
}

FHitResult UGrabber::GetPhysicsBodyByLineTrace()
{
	LineTraceRoute LTRoute = GetLineTraceRoute();

	FHitResult HitResult;
	FCollisionQueryParams CollissionQueryParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		LTRoute.start,
		LTRoute.end,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		CollissionQueryParams
	);

	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Hit Result: %s"), *ActorHit->GetName());
	}
	return HitResult;
}

LineTraceRoute UGrabber::GetLineTraceRoute()
{
	/// Get player view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	LineTraceRoute LineTraceRouteStruct;
	LineTraceRouteStruct.start = PlayerViewPointLocation;
	LineTraceRouteStruct.end = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * ViewRange;;

	return LineTraceRouteStruct;
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle->GrabbedComponent)
	{
		LineTraceRoute LTRoute = GetLineTraceRoute();
		PhysicsHandle->SetTargetLocation(LTRoute.end);
	}
}
