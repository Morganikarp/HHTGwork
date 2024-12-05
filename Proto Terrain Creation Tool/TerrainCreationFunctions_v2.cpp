// These functions are from the PlayerCharacter script in the project
// I have isolated them so that everything shown is my own work

void APlayerCharacter::TerrainBrushRaise() // Raises vertices as a brush
{

	// Ensure that the user is clicking on the hex
	APlayerController* const PlayerController = GetController<APlayerController>();
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

	for (int i = 1; i < HexMesh->Vertices.Num(); i++)
	{
		double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();

		if (DistanceFromVertex <= BrushRadius)
		{
			// Finding how much the vertex should be raised based on how close it is to the mouse
			double SpecificBrushPower = BrushPower * (BrushRadius / DistanceFromVertex);
			
			// Raising the vertex by its unique amount
			HexMesh->Vertices[i].Z += SpecificBrushPower;
		}
	}

	// Update the mesh
	HexMesh->ProcHexMeshUpdate();
}

void APlayerCharacter::TerrainBrushLower() // Lowers vertices as a brush
{

	// Ensure that the user is clicking on the hex
	APlayerController* const PlayerController = GetController<APlayerController>();
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

	for (int i = 1; i < HexMesh->Vertices.Num(); i++)
	{
		double DistanceFromVertex = (HitPosition - (HexMesh->Vertices[i] + WorldPosition)).Length();

		if (DistanceFromVertex <= BrushRadius)
		{
			// Finding how much the vertex should be lowered based on how close it is to the mouse
			double SpecificBrushPower = BrushPower * (BrushRadius / DistanceFromVertex);

			// Lowering the vertex by its unique amount
			HexMesh->Vertices[i].Z -= SpecificBrushPower;
		}
	}

	// Update the mesh
	HexMesh->ProcHexMeshUpdate();
}

void APlayerCharacter::TerrainVertexRaise() // Raises a specific vertex
{

	// Ensure that the user is clicking on the hex
	APlayerController* const PlayerController = GetController<APlayerController>();

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

		if (DistanceFromVertex <= LowestDistanceFromVertex)
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

	// Ensure that the user is clicking on the hex
	APlayerController* const PlayerController = GetController<APlayerController>();

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
		
		if (DistanceFromVertex <= LowestDistanceFromVertex)
		{
			LowestDistanceFromVertex = DistanceFromVertex;
			ClosestIndex = i;
		}
	}
	
	// Lower the found vertex down and update the mesh
	HexMesh->Vertices[ClosestIndex].Z -= BrushPower;
	HexMesh->ProcHexMeshUpdate();
}