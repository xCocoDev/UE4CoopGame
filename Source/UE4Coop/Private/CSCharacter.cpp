// Fill out your copyright notice in the Description page of Project Settings.

#include "CSCharacter.h"
#include "CSWeapon.h"
#include "Components/CSHealthComponent.h"
#include "Abilities/CSAttributeSet.h"
#include "../UE4Coop.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACSCharacter::ACSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->SetupAttachment(RootComponent);

    GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

    GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp);

    HealthComp = CreateDefaultSubobject<UCSHealthComponent>(TEXT("HealthComp"));

    // Our ability system component
    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    AttributeSet = CreateDefaultSubobject<UCSAttributeSet>(TEXT("AttributeSet"));

    ZoomedFOV = 65.5f;
    ZoomInterpSpeed = 20.0f;

    WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ACSCharacter::BeginPlay()
{
	Super::BeginPlay();

    DefaultFOV = CameraComp->FieldOfView;

    HealthComp->OnHealthChanged.AddDynamic(this, &ACSCharacter::OnHealthChanged);

    if (Role == ROLE_Authority)
    {
        bDied = false;

        // Spawn a default weapon
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        CurrentWeapon = GetWorld()->SpawnActor<ACSWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

        if (CurrentWeapon)
        {
            CurrentWeapon->SetOwner(this);
            CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
        }
    }
}

// Called every frame
void ACSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
    float NewFOV    = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

    CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ACSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveRight", this, &ACSCharacter::MoveRight);
    PlayerInputComponent->BindAxis("MoveForward", this, &ACSCharacter::MoveForward);

    PlayerInputComponent->BindAxis("LookUp", this, &ACSCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("Turn", this, &ACSCharacter::AddControllerYawInput);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACSCharacter::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACSCharacter::EndCrouch);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACSCharacter::MoveJump);

    PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ACSCharacter::BeginZoom);
    PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ACSCharacter::EndZoom);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACSCharacter::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACSCharacter::StopFire);

    AbilitySystem->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "EAbilityInput"));
}

void ACSCharacter::AcquireAbility(TSubclassOf<UGameplayAbility> AbiltyToAcquire)
{
    if (AbilitySystem)
    {
        if (HasAuthority() && AbiltyToAcquire)
            AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbiltyToAcquire, 1, 0));

        AbilitySystem->InitAbilityActorInfo(this, this);
    }
}

void ACSCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector() * Value);
}

void ACSCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector() * Value);
}

void ACSCharacter::MoveJump()
{
    Jump();
}

void ACSCharacter::BeginCrouch()
{
    Crouch();
}

void ACSCharacter::EndCrouch()
{
    UnCrouch();
}

void ACSCharacter::BeginZoom()
{
    bWantsToZoom = true;
}

void ACSCharacter::EndZoom()
{
    bWantsToZoom = false;
}

void ACSCharacter::StartFire()
{
    if (CurrentWeapon)
        CurrentWeapon->StartFire();
}

void ACSCharacter::StopFire()
{
    if (CurrentWeapon)
        CurrentWeapon->StopFire();
}

FVector ACSCharacter::GetPawnViewLocation() const
{
    if (CameraComp)
        return CameraComp->GetComponentLocation();

    return Super::GetPawnViewLocation();
}

UAbilitySystemComponent* ACSCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

void ACSCharacter::OnHealthChanged(UCSHealthComponent* HealthComponent, float Health, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Health <= 0.0f && !bDied)
    {
        bDied = true;

        GetMovementComponent()->StopMovementImmediately();
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        DetachFromControllerPendingDestroy();

        SetLifeSpan(10.0f);
    }
}

void ACSCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (AbilitySystem)
        AbilitySystem->RefreshAbilityActorInfo();
}

void ACSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACSCharacter, CurrentWeapon);
    DOREPLIFETIME(ACSCharacter, bDied);
}