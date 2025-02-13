// These functions are from the PlayerCharacter script in the project
// I have isolated them so that everything shown is my own work

void APlayerCharacter::TerrainBrushRaise() // Raises vertices as a brush
{
	APlayerController* const PlayerController = WeakPlayer->GetController<APlayerController>();
	check(PlayerController);

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (!IsValid(Cast<AMeshGenerator>(HitResult.GetActor())))
	{
		return;
	}

	HexMesh = Cast<AMeshGenerator>(HitResult.GetActor());

	// Finding what vertices are within brush range
	FVector HitPosition = HitResult.ImpactPoint;
	FVector WorldPosition = HexMesh->GetActorLocation();

	// Radius of the inner circle of brush
	BrushRadiusClose = BrushRadiusComplete * 0.5;

	for (int i = 1; i < HexMesh->Vertices.Num(); i++)
	{
		double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();

		// If the vertex is within the larger radius and isn't on the edge
		if (DistanceFromVertex <= BrushRadiusComplete && !HexMesh->EdgeVertexIndexes.Contains(i))
		{
			double SpecificBrushPower;

			// If vertex is within the inner circle, assign a common power
			if (DistanceFromVertex <= BrushRadiusClose)
			{
				SpecificBrushPower = BrushPower * (BrushRadiusComplete / BrushRadiusClose);
			}

			// If vertex is outside the inner circle, assign a unique power based on the distance from the hit location
			else
			{
				SpecificBrushPower = BrushPower * (BrushRadiusComplete / DistanceFromVertex);
			}
			
			// Raising the vertex by its unique amount
			HexMesh->Vertices[i].Z += SpecificBrushPower;
		}
	}

	// Update the mesh
	HexMesh->ProcHexMeshUpdate();
}

void APlayerCharacter::TerrainBrushLower() // Lowers vertices as a brush
{
	APlayerController* const PlayerController = WeakPlayer->GetController<APlayerController>();
	check(PlayerController);

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (!IsValid(Cast<AMeshGenerator>(HitResult.GetActor())))
	{
		return;
	}

	HexMesh = Cast<AMeshGenerator>(HitResult.GetActor());

	// Finding what vertices are within brush range
	FVector HitPosition = HitResult.ImpactPoint;
	FVector WorldPosition = HexMesh->GetActorLocation();

	// Radius of the inner circle of brush
	BrushRadiusClose = BrushRadiusComplete * 0.5;

	for (int i = 1; i < HexMesh->Vertices.Num(); i++)
	{
		double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();

		// If the vertex is within the larger radius and isn't on the edge
		if (DistanceFromVertex <= BrushRadiusComplete && !HexMesh->EdgeVertexIndexes.Contains(i))
		{
			double SpecificBrushPower;

			// If vertex is within the inner circle, assign a common power
			if (DistanceFromVertex <= BrushRadiusClose)
			{
				SpecificBrushPower = BrushPower * (BrushRadiusComplete / BrushRadiusClose);
			}

			// If vertex is outside the inner circle, assign a unique power based on the distance from the hit location
			else
			{
				SpecificBrushPower = BrushPower * (BrushRadiusComplete / DistanceFromVertex);
			}
			
			// Lowering the vertex by its unique amount
			HexMesh->Vertices[i].Z -= SpecificBrushPower;
		}
	}

	// Update the mesh
	HexMesh->ProcHexMeshUpdate();
}

void APlayerCharacter::TerrainVertexRaise() // Raises a specific vertex
{
	APlayerController* const PlayerController = WeakPlayer->GetController<APlayerController>();
	check(PlayerController);

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (!IsValid(Cast<AMeshGenerator>(HitResult.GetActor())))
	{
		return;
	}

	HexMesh = Cast<AMeshGenerator>(HitResult.GetActor());

	// Find the closest vertex to where the user is clicking
	FVector HitPosition = HitResult.ImpactPoint;
	FVector WorldPosition = HexMesh->GetActorLocation();
	int ClosestIndex = 0;

	double LowestDistanceFromVertex = (HitPosition - (HexMesh->Vertices[0] + WorldPosition)).Length();

	for (int i = 1; i < HexMesh->Vertices.Num(); i++)
	{
		double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();

		// If the vertex is within the larger radius and isn't on the edge
		if (DistanceFromVertex <= LowestDistanceFromVertex && !HexMesh->EdgeVertexIndexes.Contains(i))
		{
			LowestDistanceFromVertex = DistanceFromVertex;
			ClosestIndex = i;
		}
	}

	// Raise the found vertex up and update the mesh
	HexMesh->Vertices[ClosestIndex].Z += BrushPower;
	HexMesh->ProcHexMeshUpdate();
}

void APlayerCharacter::TerrainVertexLower() // Lowers a specific vertex
{
	APlayerController* const PlayerController = WeakPlayer->GetController<APlayerController>();
	check(PlayerController);

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (!IsValid(Cast<AMeshGenerator>(HitResult.GetActor())))
	{
		return;
	}

	HexMesh = Cast<AMeshGenerator>(HitResult.GetActor());
	
	// Find the closest vertex to where the user is clicking
	FVector HitPosition = HitResult.ImpactPoint;
	FVector WorldPosition = HexMesh->GetActorLocation();
	int ClosestIndex = 0;
	
	double LowestDistanceFromVertex = (HitPosition - (HexMesh->Vertices[0] + WorldPosition)).Length();

	for (int i = 1; i < HexMesh->Vertices.Num(); i++)
	{
		double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();
		
		// If the vertex is within the larger radius and isn't on the edge
		if (DistanceFromVertex <= LowestDistanceFromVertex && !HexMesh->EdgeVertexIndexes.Contains(i))
		{
			LowestDistanceFromVertex = DistanceFromVertex;
			ClosestIndex = i;
		}
	}
	
	// Lower the found vertex down and update the mesh
	HexMesh->Vertices[ClosestIndex].Z -= BrushPower;
	HexMesh->ProcHexMeshUpdate();
}

void UToolsManagerComponent::TerrainNoise() // Randomly raises or lowers groups of vertices
{
	APlayerController* const PlayerController = WeakPlayer->GetController<APlayerController>();
	check(PlayerController);

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (!IsValid(Cast<AMeshGenerator>(HitResult.GetActor())))
	{
		return;
	}

	HexMesh = Cast<AMeshGenerator>(HitResult.GetActor());

	FVector HitPosition = HitResult.ImpactPoint;
	FVector WorldPosition = HexMesh->GetActorLocation();

	for (int i = 0; i < HexMesh->Vertices.Num(); i++)
	{

		// If the vertex is within the larger radius and isn't on the edge
		if (!HexMesh->EdgeVertexIndexes.Contains(i))
		{
			double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();

			if (DistanceFromVertex <= BrushRadiusComplete)
			{
				// Add to the vertices height a random value between the negative and positive brush power
				HexMesh->Vertices[i].Z += FMath::RandRange(-BrushPower, BrushPower);
			}
		}
	}

	// Update the mesh
	HexMesh->ProcHexMeshUpdate();
}

void UToolsManagerComponent::TerrainFlatten(int ToWhereIndex) // Applies the same Z height to a group of vertices
{
	APlayerController* const PlayerController = WeakPlayer->GetController<APlayerController>();
	check(PlayerController);

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (!IsValid(Cast<AMeshGenerator>(HitResult.GetActor())))
	{
		return;
	}

	HexMesh = Cast<AMeshGenerator>(HitResult.GetActor());

	FVector HitPosition = HitResult.ImpactPoint;
	FVector WorldPosition = HexMesh->GetActorLocation();

	float NearbyVertsZ = 0;
	TArray<int> NearbyVertIndex;
	float LowestVertZ = std::numeric_limits<float>::max();
	float HighestVertZ = std::numeric_limits<float>::min();

	for (int i = 0; i < HexMesh->Vertices.Num(); i++)
	{

		// If the vertex is within the larger radius and isn't on the edge
		if (!HexMesh->EdgeVertexIndexes.Contains(i))
		{
			double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();

			if (DistanceFromVertex <= BrushRadiusComplete)
			{

				// Collecting all vertices to be altered in a list
				NearbyVertsZ += HexMesh->Vertices[i].Z;
				NearbyVertIndex.Add(i);

				// Recording the highest and lowest heights of relevant vertices
				LowestVertZ = HexMesh->Vertices[i].Z < LowestVertZ ? HexMesh->Vertices[i].Z : LowestVertZ;
				HighestVertZ = HexMesh->Vertices[i].Z > HighestVertZ ? HexMesh->Vertices[i].Z : HighestVertZ;
			}
		}	
	}

	float meanZ = NearbyVertsZ / NearbyVertIndex.Num();

	for (int i = 0; i < NearbyVertIndex.Num(); i++)
	{
		// The specific applied value is determined by the flatten mode
		// 0). Flatten to the lowest vertex height
		// 1). Flatten to the highest vertex height
		// 2). Flatten to the average vertex height
		switch (ToWhereIndex)
		{
		case 0:
			HexMesh->Vertices[NearbyVertIndex[i]].Z = LowestVertZ;
			break;
		case 1:
			HexMesh->Vertices[NearbyVertIndex[i]].Z = HighestVertZ;
			break;
		case 2:
			HexMesh->Vertices[NearbyVertIndex[i]].Z = meanZ;
			break;
		default:
			break;
		}
	}

	// Update the mesh
	HexMesh->ProcHexMeshUpdate();
}