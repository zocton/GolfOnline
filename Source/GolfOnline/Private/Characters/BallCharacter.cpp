// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BallCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Projectiles/Projectile.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Managers/GolfOnlineGameInstance.h"
#include "Managers/WidgetManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Talents/TalentMenu.h"
#include "GameFramework/HUD.h"
#include "NiagaraComponent.h"

// Sets default values
ABallCharacter::ABallCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// We don't need the inherited skeletal mesh component or its collision
	GetMesh()->DestroyComponent();
	GetCapsuleComponent()->DestroyComponent();


	// Create mesh component for the ball
	Ball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball0"));
	Ball->SetStaticMesh(BallMesh);
	Ball->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Ball->SetSimulatePhysics(true);
	Ball->SetAngularDamping(0.1f);
	Ball->SetLinearDamping(0.1f);
	Ball->BodyInstance.MassScale = 3.5f;
	Ball->BodyInstance.MaxAngularVelocity = 800.0f;
	Ball->SetNotifyRigidBodyCollision(true);
	RootComponent = Ball;

	GetCharacterMovement()->UpdatedComponent = Ball;
	
	// Create a camera boom attached to the root (ball)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(Ball, FName("Center"));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true); // Rotation of the ball should not affect rotation of boom
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 1200.f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 3.f;

	// Create a camera and attach to boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	GetCapsuleComponent()->InitCapsuleSize(50.0f, 50.0f);
	//GetCapsuleComponent()->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	GetCapsuleComponent()->SetupAttachment(Ball);

	// Set up forces
	RollTorque = 50000000.0f;
	JumpImpulse = 350000.0f;
	bCanJump = true; // Start being able to jump

	// Only rotate camera
	/*bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;*/

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Rotation rate
	//GetCharacterMovement()->JumpZVelocity = 600.0f;
	//GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	//Initialize projectile class
	ProjectileClass = AProjectile::StaticClass();

	//Initialize fire rate
	FireRate = 0.25f;
	bIsFiringWeapon = false;
}

// Called when the game starts or when spawned
void ABallCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABallCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetCharacterMovement()->UpdateBasedMovement(DeltaTime);
}

// Called to bind functionality to input
void ABallCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Handle firing projectiles
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABallCharacter::StartFire);

	// Handle movement inputs
	PlayerInputComponent->BindAction("Stop", IE_Pressed, this, &ABallCharacter::StopMovement);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABallCharacter::Jump);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ABallCharacter::Boost);
	PlayerInputComponent->BindAction("OpenTalents", IE_Pressed, this, &ABallCharacter::OpenTalents);
	PlayerInputComponent->BindAxis("MoveRightAxis", this, &ABallCharacter::MoveRightAxis);
	PlayerInputComponent->BindAxis("MoveForwardAxis", this, &ABallCharacter::MoveForwardAxis);
}

void ABallCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ABallCharacter, CurrentHealth);
}

void ABallCharacter::OnHealthUpdate()
{
    //Client-specific functionality
    if (IsLocallyControlled())
    {
        FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

        if (CurrentHealth <= 0)
        {
            FString deathMessage = FString::Printf(TEXT("You have been killed."));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
        }
    }

    //Server-specific functionality
    if (GetLocalRole() == ROLE_Authority)
    {
        FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
    }

    //Functions that occur on all machines. 
    /*
        Any special functionality that should occur as a result of damage or death should be placed here.
    */
}

void ABallCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ABallCharacter::SetCurrentHealth(float HealthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(HealthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ABallCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void ABallCharacter::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &ABallCharacter::StopFire, FireRate, false);
		HandleFire();
	}
}

void ABallCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void ABallCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	ABallCharacter* spawnedProjectile = GetWorld()->SpawnActor<ABallCharacter>(spawnLocation, spawnRotation, spawnParameters);
}

void ABallCharacter::Boost()
{
	const FVector Impulse = Ball->GetComponentVelocity().GetSafeNormal() * BoostImpulse;
	Ball->AddImpulse(Impulse);
}

void ABallCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	bCanJump = true;
}

void ABallCharacter::StopMovement()
{
	Ball->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	Ball->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
}

void ABallCharacter::MoveRightAxis(float Val)
{
	const FVector Torque = FVector(-1.f * Val * RollTorque, 0.f, 0.f);
	Ball->AddTorqueInRadians(Torque);
}

void ABallCharacter::MoveForwardAxis(float Val)
{
	const FVector Torque = FVector(0.f, Val * RollTorque, 0.f);
	Ball->AddTorqueInRadians(Torque);
}

void ABallCharacter::Jump()
{
	if (!bCanJump) return;

	const FVector Impulse = FVector::UpVector * JumpImpulse;
	Ball->AddImpulse(Impulse);

	bCanJump = false;
}

void ABallCharacter::OpenTalents()
{
	GetGameInstance()->GetSubsystem<UWidgetManager>()->OpenInGameMenu(EMenuType::Talents);
}