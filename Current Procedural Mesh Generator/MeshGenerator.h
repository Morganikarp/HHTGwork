// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshGenerator.generated.h"

class UProceduralMeshComponent;

UCLASS()
class HEXTONHILLS_API AMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION() void ProcHexMeshCreate();
	UFUNCTION() void ProcHexMeshUpdate();

	UFUNCTION() void StaticHexMeshCreate();
	
	UProceduralMeshComponent* Mesh;

	UPROPERTY() int HexScale;
	UPROPERTY(EditAnywhere) int HexComplexity = 2;
	
	UPROPERTY(EditAnywhere) int HexSide = 100;
	UPROPERTY() double TriangleSide;
	UPROPERTY() double HexArea;

	UPROPERTY(Meta = (MakeEditWidget = true)) TArray<FVector> Vertices;
	UPROPERTY() TArray<int> Triangles;
	
};
