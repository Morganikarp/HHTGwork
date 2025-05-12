// This function is from the ToolsManagerComponent script in the project
// I have isolated it so that everything shown is my own work

FString UToolsManagerComponent::Set2dp(double value) // small function used for simplifying data before writing it to a file
{
	value *= 100.0f;
	value = round(value) / 100.0f;
	return FString::SanitizeFloat(value);
}

void UToolsManagerComponent::SaveTile(TArray<AActor*> ObjectArray, TArray<AActor*> DropInArray, TArray<AActor*> SplineArray) // Takes in all relevant date to the tile and write it to a .csv
{
	// Setting up dialogue window for saving
	IDesktopPlatform* Platform = FDesktopPlatformModule::Get();
	FString DefaultPath = FPlatformProcess::UserDir();;
	TArray<FString> SavedTilePath;

	// Saving empty file at chosen directory
	if (!Platform->SaveFileDialog(nullptr, "Save Tile", DefaultPath, "MyTile", "Tile Files|*.csv", EFileDialogFlags::None, SavedTilePath))
	{
		return;
	}

	// Current .csv save file layout
	// (0) Hex Complexity
	// (1) Altered mesh vertex index ; Altered mesh vertex Z-location , ...
	// (2) int if brush (1 if true) ; Index of array with mesh in DA_BrushObjects ; int if flush with terrain (1 if true) Object 0 location x / y / z ; Object 0 rotation x / y / z ; Object 0 scale x / y / z , ...
	// (3) Name from Spline Data in DA_SplineObjects ; Index of final major Spline Point ; Major Index Step ; Spline index 0 location x / y / z ; Spline index 1 location x / y / z ; ... ; Spline index (final) location x / y / z , ...

	// Formatting defaults
	FString FormatSeperator = ","; // Seperates "cells"
	FString DataSeparator = ";"; // Seperates data within the same "cell"
	FString VectorSeperator = "/"; // Seperates values that together form a vector

	TArray<FString> Data;
	Data.SetNum(4);

	// (0) Hex Complexity
	Data[0] = FString::FromInt(HexMeshRef->HexComplexity);

	// (1) Altered mesh vertex index ; Altered mesh vertex Z-location , ...

	for (int i = 0; i < HexMeshRef->Vertices.Num(); i++)
	{
		if (FString::SanitizeFloat(HexMeshRef->Vertices[i].Z) != "0.0")
		{

			Data[1] += FString::FromInt(i) + DataSeparator + Set2dp(HexMeshRef->Vertices[i].Z) + FormatSeperator;
		}
	}

	// (2) int if brush (1 if true) ; Index of array with mesh in DA_BrushObjects ; int if flush with terrain (1 if true) Object 0 location x / y / z ; Object 0 rotation x / y / z ; Object 0 scale x / y / z , ...

	for (int i = 0; i < ObjectArray.Num(); i++)
	{
		AObjectBrush* BrushObject = Cast<AObjectBrush>(ObjectArray[i]);

		Data[2] += FString::SanitizeFloat(1.0f) + DataSeparator; // int if brush (1 if true)

		Data[2] += FString::SanitizeFloat(BrushObject->MeshIndex) + DataSeparator; // Index of array with mesh in DA_SplineObjects

		if (BrushObject->AssetFlush) { Data[2] += FString::SanitizeFloat(1.0f) + DataSeparator; }
		else { Data[2] += FString::SanitizeFloat(0.0f) + DataSeparator; }

		Data[2] += Set2dp(BrushObject->ObjectTrans.GetLocation().X) + VectorSeperator + Set2dp(BrushObject->ObjectTrans.GetLocation().Y) + VectorSeperator + Set2dp(BrushObject->ObjectTrans.GetLocation().Z) + DataSeparator; // Object location
		Data[2] += Set2dp(BrushObject->ObjectTrans.GetRotation().X) + VectorSeperator + Set2dp(BrushObject->ObjectTrans.GetRotation().Y) + VectorSeperator + Set2dp(BrushObject->ObjectTrans.GetRotation().Z) + DataSeparator;  // Object rotation
		Data[2] += Set2dp(BrushObject->ObjectTrans.GetScale3D().X) + VectorSeperator + Set2dp(BrushObject->ObjectTrans.GetScale3D().Y) + VectorSeperator + Set2dp(BrushObject->ObjectTrans.GetScale3D().Z) + FormatSeperator;  // Object scale
	}

	for (int i = 0; i < DropInArray.Num(); i++)
	{
		ASpawnHeroBuilding* DropInObject = Cast<ASpawnHeroBuilding>(DropInArray[i]);

		Data[2] += FString::SanitizeFloat(0.0f) + DataSeparator; // int if brush (1 if true)

		Data[2] += FString::SanitizeFloat(DropInObject->MeshIndex) + DataSeparator; // Index of array with mesh in DA_SplineObjects

		if (DropInObject->AssetFlush)
		{
			Data[2] += FString::SanitizeFloat(1.0f) + DataSeparator;
		}

		else
		{
			Data[2] += FString::SanitizeFloat(0.0f) + DataSeparator;
		}

		Data[2] += Set2dp(DropInObject->ObjectTrans.GetLocation().X) + VectorSeperator + Set2dp(DropInObject->ObjectTrans.GetLocation().Y) + VectorSeperator + Set2dp(DropInObject->ObjectTrans.GetLocation().Z) + DataSeparator; // Object location

		Data[2] += Set2dp(DropInObject->ObjectTrans.GetRotation().X) + VectorSeperator + Set2dp(DropInObject->ObjectTrans.GetRotation().Y) + VectorSeperator + Set2dp(DropInObject->ObjectTrans.GetRotation().Z) + DataSeparator;  // Object rotation

		Data[2] += Set2dp(DropInObject->ObjectTrans.GetScale3D().X) + VectorSeperator + Set2dp(DropInObject->ObjectTrans.GetScale3D().Y) + VectorSeperator + Set2dp(DropInObject->ObjectTrans.GetScale3D().Z) + FormatSeperator;  // Object scale
	}

	// (3) Name from Spline Data in DA_SplineObjects ; Index of final major Spline Point ; Major Index Step ; Spline index 0 location x / y / z ; Spline index 1 location x / y / z ; ... ; Spline index (final) location x / y / z ,

	for (int i = 0; i < SplineArray.Num(); i++)
	{
		ASplinePath* Spline = Cast<ASplinePath>(SplineArray[i]);

		Data[3] += Spline->MeshName + DataSeparator; // Name from Spline Data in DA_SplineObjects

		Data[3] += FString::SanitizeFloat(Spline->LastMajorIndex) + DataSeparator; // Index of final major Spline Point

		Data[3] += FString::SanitizeFloat(Spline->MajorVertStep) + DataSeparator; // Major Index Step

		for (int j = 0; j <= Spline->LastMajorIndex; j++) // Spline point vector locations
		{

			Data[3] += Set2dp(Spline->SplineVertexPositions[j].X) + VectorSeperator + Set2dp(Spline->SplineVertexPositions[j].Y) + VectorSeperator + Set2dp(Spline->SplineVertexPositions[j].Z);

			if (j == Spline->LastMajorIndex) {
				Data[3] += FormatSeperator;
			}
			else {
				Data[3] += DataSeparator;
			}

		}

	}

	// Writing to save file
	if (!FFileHelper::SaveStringArrayToFile(Data, *SavedTilePath[0]))
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, FString::Printf(TEXT("Save Failed")));
	}
}

bool UToolsManagerComponent::ImportTile() // Takes in a .csv from a dialogue window and reads it
{
	IDesktopPlatform* Platform = FDesktopPlatformModule::Get();
	FString DefaultPath = FPlatformProcess::UserDir();;
	TArray<FString> FileExplorerSelection;

	// Return false if dialogue is cancelled
	if (!Platform->OpenFileDialog(nullptr, "Import Tile", DefaultPath, "", "Tile Files|*.csv", EFileDialogFlags::None, FileExplorerSelection))
	{
		return false;
	}

	// Writes the save data to the ImportTileData array for use in blueprints
	if (FFileHelper::LoadFileToStringArray(ImportTileData, *FileExplorerSelection[0]))
	{
		return true;
	}

	return false;
}
