// This function is from the PlayerCharacter script in the project
// I have isolated it so that everything shown is my own work

void APlayerCharacter::SaveTile()
{
    if (!HexMesh.IsValid())
    {
        GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("No mesh ref, click on mesh with terrain tool")));
        return;
    }
	
    FString FormatSeperator = ","; // Seperates "cells"
    FString DataSeperator = ";"; // Seperates data within the same "cell"

    // Current .csv save file layout
    //(0) Hex Complexity
    //(1) Altered mesh vertex index ; Altered mesh vertex Z-location , ...

    FString Path = FPaths::ProjectDir() + "TileOutput\\" + SavePath;
    TArray<FString> Data;
    Data.SetNum(2);

    // (0) Hex Complexity
    Data[0] = FString::FromInt(HexMesh->HexComplexity);

    //(1) Altered mesh vertex index ; Altered mesh vertex Z-location , ...	
    for (int i = 0; i < HexMesh->Vertices.Num(); i++)
    {
        if (FString::SanitizeFloat(HexMesh->Vertices[i].Z) != "0.0")
        {
            Data[1] += FString::FromInt(i) + DataSeperator + FString::SanitizeFloat(HexMesh->Vertices[i].Z) + FormatSeperator;

            if (i == HexMesh->Vertices.Num())
            {
                Data[1] += FString::SanitizeFloat(HexMesh->Vertices[i].Z);
            }
        }
    }
	

    // If writing to the file works...
    if (FFileHelper::SaveStringArrayToFile(Data, *Path))
    {
        GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("Save Succeeded")));
    }
	
    // If writing to the file fails...
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow,  FString::Printf(TEXT("Save Failed")));
    }
}