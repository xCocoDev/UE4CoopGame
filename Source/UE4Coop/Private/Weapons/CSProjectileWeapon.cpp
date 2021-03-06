// Fill out your copyright notice in the Description page of Project Settings.


#include "CSProjectileWeapon.h"

void ACSProjectileWeapon::Fire()
{
    AActor* MyOwner = GetOwner();

    if (MyOwner == nullptr)
        return;

    FVector EyeLocation;
    FRotator EyeRotation;

    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
}