// These functions are from the PlayerCharacter script in the project
// I have isolated them so that everything shown is my own work

void APlayerCharacter::TopographyBrushRaise() // Triggered when the appropriate input is held down
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
	HexMesh->Vertices[ClosestIndex].Z += 10;
	HexMesh->ProcHexMeshUpdate();
}

void APlayerCharacter::TopographyBrushLower() // Triggered when the appropriate input is held down
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
	HexMesh->Vertices[ClosestIndex].Z -= 10;
	HexMesh->ProcHexMeshUpdate();
}
