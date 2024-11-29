// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include <map>
#include "ProceduralMeshComponent.h"
using namespace std;

// Sets default values
AMeshGenerator::AMeshGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMeshGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	if (HexComplexity >= 1 && HexComplexity <= 8 && HexSide >= 10 && HexSide <= 100000)
	{
		ProcHexMeshCreate();
	}
}

// Called every frame
void AMeshGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeshGenerator::ProcHexMeshCreate()
{
	// Finding the number of vertices based on the Hex's set complexity and side length
	Vertices.Empty();

	int n = FMath::Pow(static_cast<double>(2), HexComplexity - 1);
	int VerticesArraySize = 1 + 6 * ( (n + 1) * ( (n + 1) - 1 ) / 2 ); // Calculating the number of vertices in the mesh: Verts = 1+6*(n*(n-1)/2)

	Vertices.SetNum(VerticesArraySize);

	// Find the number of vertices that go through the middle of the hexagon and along each side
	int MaxNumberOfVerts = FMath::Pow( static_cast<double>(2), HexComplexity ) + 1;
	int MinNumberOfVerts = FMath::Pow( static_cast<double>(2), HexComplexity - 1) + 1;

	// Finding the number of tri's based on the Hex's set complexity and side length
	Triangles.Empty();
	
	HexArea = ( ( 3 * FMath::Sqrt( static_cast<double>(3) ) ) / 2 ) * FMath::Square(HexSide); // Calculating the 2d area of the hex: area = (3*Sqrt(3)/2)*Sq(HexSide)
	
	TriangleSide = HexSide / FMath::Pow( static_cast<double>(2), HexComplexity - 1);
	int TrianglesWithinHex = HexArea / ( ( FMath::Sqrt( static_cast<double>(3) ) / 4) * FMath::Square(TriangleSide) ) ; // Calculating the number of triangles that fit within the hex
	Triangles.SetNum(TrianglesWithinHex * 3);

	// Filling the Vertices array and placing each correctly
	double HexApothem = HexSide * FMath::Sqrt( static_cast<double>(3) ) * 0.5;
	double TriApothem = HexApothem / FMath::Pow( static_cast<double>(2), HexComplexity - 1);

	int CurrentVertIndex = 0;
	int VertLayers = MaxNumberOfVerts - MinNumberOfVerts;

	FVector RowBaseVector = FVector(0, 0, 0);
	
	// Laying out the bottom half of the hexagon's vertices
	for (int i = 0; i < VertLayers; i++)
	{
		RowBaseVector = i * FVector(-TriangleSide / 2, TriApothem, 0);
		Vertices[CurrentVertIndex++] = RowBaseVector;
		
		for (int j = 0; j < MinNumberOfVerts-1+i; j++)
		{
			Vertices[CurrentVertIndex++] = FVector(Vertices[CurrentVertIndex-1].X + TriangleSide, RowBaseVector.Y, 0);
		}
	}

	// Laying out the line of vertices through the hexagon's middle
	RowBaseVector = FVector(-(HexSide / 2), HexApothem, 0);
	Vertices[CurrentVertIndex++] = RowBaseVector;
	
	for (int i = 0; i < MaxNumberOfVerts - 1; i++)
	{
		Vertices[CurrentVertIndex++] = FVector(Vertices[CurrentVertIndex-1].X + TriangleSide, RowBaseVector.Y, 0);
	}
	
	// Laying out the top half of the hexagon's vertices
	for (int i = 0; i < VertLayers; i++)
	{
		RowBaseVector = FVector(-(TriangleSide / 2) * (VertLayers - i - 1), (TriApothem * (i+1) ) + HexApothem, 0);
		Vertices[CurrentVertIndex++] = RowBaseVector;
		
		for (int j = 0; j < MaxNumberOfVerts-(i+2); j++)
		{
			Vertices[CurrentVertIndex++] = FVector(Vertices[CurrentVertIndex-1].X + TriangleSide, RowBaseVector.Y, 0);
		}
	}
	
	// Filling the Triangles array as to connect each vertex

	// Creating tries across the bottom edge
	int CurrentTriIndex = 0;
	
	Triangles[CurrentTriIndex++] = 0;
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex-1] + MinNumberOfVerts + 1;
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex-2] + 1;
	
	for (int i = 1; i < FMath::Pow(static_cast<double>(2), HexComplexity - 1); i++)
	{
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex-1];
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex-1] + (MinNumberOfVerts + 1);
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex-2] + 1;
	}
	
	// Creating tries in the bottom half
	for (int j = 1; j < FMath::Pow(static_cast<double>(2), HexComplexity - 1); j++)
	{
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (MinNumberOfVerts + j);

		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3];
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3] + (MinNumberOfVerts + j);
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 4];

		for (int i = 0; i < FMath::Pow(static_cast<double>(2), HexComplexity - 1) + j - 1; i++) {
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1];
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (MinNumberOfVerts + j);

			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3];
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3] + (MinNumberOfVerts + j);
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 4];
		}
	}

	// Creating tries in the middle
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (FMath::Pow(static_cast<double>(2), HexComplexity) + 1);

	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3];
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3] + (FMath::Pow(static_cast<double>(2), HexComplexity));
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 4];

	for (int i = 0; i < FMath::Pow(static_cast<double>(2), HexComplexity) - 1; i++)
	{
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1];
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (FMath::Pow(static_cast<double>(2), HexComplexity) + 1);

		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3];
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3] + (FMath::Pow(static_cast<double>(2), HexComplexity));
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 4];
	}

	// Creating tries in the top half
	for (int j = FMath::Pow(static_cast<double>(2), HexComplexity - 1) - 1; j > 0; j--)
	{
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (MinNumberOfVerts + j + 1);

		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3];
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3] + (MinNumberOfVerts + j - 1);
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 4];

		for (int i = 0; i < FMath::Pow(static_cast<double>(2), HexComplexity - 1) + j - 1; i++)
		{
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1];
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (MinNumberOfVerts + j + 1);

			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3];
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 3] + (MinNumberOfVerts + j - 1);
			Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 4];
		}
	}

	// Creating tris across the top edge
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
	Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (MinNumberOfVerts + 1);

	for (int i = 1; i < FMath::Pow(static_cast<double>(2), HexComplexity - 1); i++)
	{
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 2];
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] + 1;
		Triangles[CurrentTriIndex++] = Triangles[CurrentTriIndex - 1] - (MinNumberOfVerts + 1);
	}

	// Creating the final mesh
	Mesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);




	
	// EVERYTHING ABOVE THIS IS REAL
	// Everything below is mad, mad, madness

	//double VerticesArraySize = 1 + 6 * ( (HexComplexity+1) * ( (HexComplexity+1) - 1 ) / 2 ); // Calculating the number of vertices in the mesh: Verts = 1+6*(n*(n-1)/2)

		//for (int j = 1; j < VertLayers; j++)
	//{
	//	for (int i = 0; i < FMath::Pow(static_cast<double>(2), HexComplexity - 1) + j; i++)
	//	{
	//		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1]; //3
	//		CurrentTriIndex++;
	//		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; //4
	//		CurrentTriIndex++;
	//		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - MinNumberOfVerts - j; //0, "1" will be i
	//		CurrentTriIndex++;

	//		tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//		tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//		tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//		tri = tri1 + "    " + tri2 + "    " + tri3;
	//		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));

	//		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; //3 
	//		CurrentTriIndex++;
	//		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + MinNumberOfVerts + j; //8, "1" will be i
	//		CurrentTriIndex++;
	//		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; //4 
	//		CurrentTriIndex++;

	//		tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//		tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//		tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//		tri = tri1 + "    " + tri2 + "    " + tri3;
	//		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));
	//	}
	//}


		//for (int i = 0; i < MaxNumberOfVerts-1; i++)
	//{
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1];
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1;
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - MaxNumberOfVerts;
	//	CurrentTriIndex++;

	//	tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//	tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//	tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//	tri = tri1 + "    " + tri2 + "    " + tri3;
	//	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));

	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3];
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + MaxNumberOfVerts-1;
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4];
	//	CurrentTriIndex++;

	//	tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//	tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//	tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//	tri = tri1 + "    " + tri2 + "    " + tri3;
	//	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));
	//}

	//Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] ; // = 12
	//CurrentTriIndex++;
	//Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; // = 13
	//CurrentTriIndex++;
	//Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - MinNumberOfVerts - (MinNumberOfVerts - 1); // = 8, "0" = i
	//CurrentTriIndex++;

	//tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//tri = tri1 + "    " + tri2 + "    " + tri3;
	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));
	//	
	//Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; // = 12
	//CurrentTriIndex++;
	//Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + MinNumberOfVerts + (MinNumberOfVerts - 1); // = 16, "0" = i
	//CurrentTriIndex++;
	//Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; // = 13
	//CurrentTriIndex++;

	//tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//tri = tri1 + "    " + tri2 + "    " + tri3;
	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));
	//
	//for (int j = 1; j < MaxNumberOfVerts - 1 - 1; j++)
	//{
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1]; // = 12
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; // = 13
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - MaxNumberOfVerts ; // = 8, "0" = i
	//	CurrentTriIndex++;

	//	tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//	tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//	tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//	tri = tri1 + "    " + tri2 + "    " + tri3;
	//	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));
	//	
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; // = 12
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + MaxNumberOfVerts - (VertLayers - j); // = 16, "0" = i
	//	CurrentTriIndex++;
	//	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; // = 13
	//	CurrentTriIndex++;

	//	tri1 = FString::SanitizeFloat(Triangles[CurrentTriIndex-3]);
	//	tri2 = FString::SanitizeFloat(Triangles[CurrentTriIndex-2]);
	//	tri3 = FString::SanitizeFloat(Triangles[CurrentTriIndex-1]);
	//	tri = tri1 + "    " + tri2 + "    " + tri3;
	//	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%s"), *tri));
	//}

	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("TOP")));
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 2; // = 
	// GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("%i"), Triangles[CurrentTriIndex]));
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; // = 13
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - MaxNumberOfVerts + 0; // = 8, "0" = i
	// CurrentTriIndex++;
	// 	
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; // = 12
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + MaxNumberOfVerts - (VertLayers - 0); // = 16, "0" = i
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; // = 13
	// CurrentTriIndex++;

	//for (int i = 1; i < VertLayers; i++)
	//{
	//	// for (int j = 0; j < VertLayers; j++)
	//	// {
	//	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1]; // = 13
	//	// 	CurrentTriIndex++;
	//	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; // = 14
	//	// 	CurrentTriIndex++;
	//	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - (MaxNumberOfVerts + 0); // = 9, "0" = i
	//	// 	CurrentTriIndex++;
	//	//
	//	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; // = 13
	//	// 	CurrentTriIndex++;
	//	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + (MaxNumberOfVerts - 2 - 0); // = 17, "0" = i
	//	// 	CurrentTriIndex++;
	//	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; // = 14
	//	// 	CurrentTriIndex++;
	//	// }
	//}

	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1]; // = 14
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; // = 15
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - (MaxNumberOfVerts + 0); // = 10, "0" = i
	// CurrentTriIndex++;
	// 	
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; // = 14
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + (MaxNumberOfVerts - 2 - 0); // = 18, "0" = i
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; // = 15
	// CurrentTriIndex++;

	// for (int j = 1; j < VertLayers; j++)
	// {
	// 	for (int i = 0; i < MinNumberOfVerts + j; i++)
	// 	{
	// 		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1]; //3
	// 		CurrentTriIndex++;
	// 		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; //4
	// 		CurrentTriIndex++;
	// 		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - MinNumberOfVerts - j; //0, "1" will be i
	// 		CurrentTriIndex++;
	//
	// 		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; //3 
	// 		CurrentTriIndex++;
	// 		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + MinNumberOfVerts + j; //8, "1" will be i
	// 		CurrentTriIndex++;
	// 		Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; //4 
	// 		CurrentTriIndex++;
	// 	}
	// }
	

	// // Triangles[CurrentTriIndex] = 11
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; // = 12
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; // = 13
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - (MinNumberOfVerts + 2 + 0); // = 8, "0" = i
	// CurrentTriIndex++;
	//
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3]; // = 12
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + (MinNumberOfVerts + 0); // = 16, "0" = i
	// CurrentTriIndex++;
	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4]; // = 13
	// CurrentTriIndex++;

	// for (int i = 0; i < MinNumberOfVerts; i++)
	// {
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1];
	// 	// CurrentTriIndex++;
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1;
	// 	// CurrentTriIndex++;
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - MinNumberOfVerts - (MinNumberOfVerts - 1) ;
	// 	// CurrentTriIndex++;
	//
	//
	//
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3];
	// 	// CurrentTriIndex++;
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + MaxNumberOfVerts + j;
	// 	// CurrentTriIndex++;
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4];
	// 	// CurrentTriIndex++;
	// }

	// for (int i = 0; i < MaxNumberOfVerts-1; i++)
	// {
	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - 1; //27
	// 	CurrentTriIndex++;
	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1; //28
	// 	CurrentTriIndex++;
	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] - 2; // 19
	// 	CurrentTriIndex++;
	//
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3];
	// 	// CurrentTriIndex++;
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-3] + (MaxNumberOfVerts - 1);
	// 	// CurrentTriIndex++;
	// 	// Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-4];
	// 	// CurrentTriIndex++;
	// }
	//
	
	// for (int i = 1; i < MinNumberOfVerts-1; i++) // Laying out bottom half of hexagon
	// {
	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-1] + 1;
	// 	int levelVectIndex1 = CurrentTriIndex;
	// 	CurrentTriIndex++;
	//
	// 	Triangles[CurrentTriIndex] = Triangles[levelVectIndex1] + MinNumberOfVerts + 1 + 1;
	//
	// 	CurrentTriIndex++;
	//
	// 	Triangles[CurrentTriIndex] = Triangles[CurrentTriIndex-2] + 1;
	// 	int levelVectIndex2 = CurrentTriIndex;
	//
	// 	CurrentTriIndex++;
	//
	// 	Triangles[CurrentTriIndex] = Triangles[levelVectIndex1];
	//
	// 	CurrentTriIndex++;
	//
	// 	Triangles[CurrentTriIndex] = Triangles[levelVectIndex2];
	//
	// 	CurrentTriIndex++;
	// 	
	// 	Triangles[CurrentTriIndex] = Triangles[levelVectIndex2] - MinNumberOfVerts - 1;
	//
	// 	CurrentTriIndex++;
	// }

}

void AMeshGenerator::ProcHexMeshUpdate()
{
	Mesh->UpdateMeshSection(0, Vertices, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());
}

void AMeshGenerator::StaticHexMeshCreate()
{
	// Next job
}
