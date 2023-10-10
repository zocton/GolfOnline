// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BallCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMesh;
class UTalentMenu;
class UNiagaraComponent;

UCLASS()
class GOLFONLINE_API ABallCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABallCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ball")
	UStaticMesh* BallMesh;

	UPROPERTY(EditAnywhere, Category="Ball")
	float JumpImpulse;

	UPROPERTY(EditAnywhere, Category = "Ball")
	float RollTorque;

	UPROPERTY(EditAnywhere, Category = "Ball")
	float BoostImpulse;

	bool bCanJump;

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Getter for Max Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** Getter for Current Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** Setter for Current Health. Clamps the value between 0 and MaxHealth and calls OnHealthUpdate. Should only be called on the server.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float HealthValue);

	/** Event for taking damage. Overridden from APawn.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* NiagaraComponent;

	/** The Mesh for the ball character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Mesh")
	UStaticMeshComponent* Ball;

	/** The spring arm for camera connection to the player's ball mesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Camera Components")
	USpringArmComponent* SpringArm;

	/** The camera. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Camera Components")
	UCameraComponent* Camera;

	/** The player's maximum health. This is the highest that their health can be, and the value that their health starts at when spawned.*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	/** The player's current health. When reduced to 0, they are considered dead.*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
		float CurrentHealth;

	/** RepNotify for changes made to current health.*/
	UFUNCTION()
		void OnRep_CurrentHealth();

	/** Response to health being updated. Called on the server immediately after modification, and on clients in response to a RepNotify*/
	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;

	/** Delay between shots in seconds. Used to control fire rate for our test projectile, but also to prevent an overflow of server functions from binding SpawnProjectile directly to input.*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	/** If true, we are in the process of firing projectiles. */
	bool bIsFiringWeapon;

	/** Function for beginning weapon fire.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();

	/** Function for ending weapon fire. Once this is called, the player can use StartFire again.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable)
		void HandleFire();

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MoveSpeed = 100.0f;

	UFUNCTION()
		void StopMovement();

	UFUNCTION()
		void MoveRightAxis(float Val);
	UFUNCTION()
		void MoveForwardAxis(float Val);

	void Jump();

	UFUNCTION()
	void OpenTalents();

	UFUNCTION()
	void Boost();

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle FiringTimer;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UTalentMenu> BP_TalentMenu;
	UTalentMenu* CurrentTalentWindow;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
