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
	FHitResult HitResult = LineTrace();
	if (HitResult.GetActor()) 
	{
		PhysicsHandle->GrabComponentAtLocation(
			HitResult.GetComponent(),
			NAME_None,
			HitResult.GetComponent()->GetOwner()->GetActorLocation()//, //HitResult.GetActor()->GetActorLocation()
			//HitResult.GetComponent()->GetOwner()->GetActorRotation()
		);
		/*PhysicsHandle->GrabComponent(
			HitResult.GetComponent(),
			NAME_None,
			HitResult.GetComponent()->GetOwner()->GetActorLocation(), //HitResult.GetActor()->GetActorLocation()
			true
		);*/
	}
}

void UGrabber::GrabOff()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key released!"));
	PhysicsHandle->ReleaseComponent();
}

FHitResult UGrabber::LineTrace()
{
	/// Get player view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	//UE_LOG(LogTemp, Warning, TEXT("Location: %s, Position: %s"),
	//	*PlayerViewPointLocation.ToString(), *PlayerViewPointRotator.ToString());

	/// Debug line end
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * ViewRange;

	/// Add debug line
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f
	);

	FHitResult HitResult;
	FCollisionQueryParams CollissionQueryParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		CollissionQueryParams
	);

	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Hit Result: %s"), *ActorHit->GetName());
	}
	return HitResult;
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Get player view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	//UE_LOG(LogTemp, Warning, TEXT("Location: %s, Position: %s"),
	//	*PlayerViewPointLocation.ToString(), *PlayerViewPointRotator.ToString());

	/// Debug line end
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * ViewRange;

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}