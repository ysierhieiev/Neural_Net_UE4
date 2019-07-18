// Fill out your copyright notice in the Description page of Project Settings.

#include "Net.h"
#include "Paths.h"
#include "Engine/Engine.h"
#include "PlatformFilemanager.h"
#include "PlatformFile.h"
#include "FileHelper.h"


// Sets default values for this component's properties
UNet::UNet()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UNet::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNet::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString UNet::GetPathToData()
{
	return FPaths::ProjectSavedDir() + TEXT("NetData/Data.txt");
}


bool UNet::CreateNet(TArray<int> &Topology, bool bBias)
{
	useBias = bBias;

	//check(LoadData(Topology));

	RandomizeWeights(Topology, -1.f, 1.f);

	/// Создание
	int ArrayIndex = 0;

	for (int CurrentLayer = 0; CurrentLayer < Topology.Num(); ++CurrentLayer)
	{
		Layers.Add(Layer());
		
		for (int CurrentNeuron = 0; CurrentNeuron < Topology[CurrentLayer]; ++CurrentNeuron)
		{
			if (CurrentLayer == 0)
			{
				Layers.Last().Add(Neuron(Neuron_Type::Input,CurrentNeuron,Topology[1], WeightsArray[ArrayIndex]));
				++ArrayIndex;
			}
			else if (CurrentLayer == (Topology.Num() - 1))
			{
				Layers.Last().Add(Neuron(Neuron_Type::Output, CurrentNeuron, 0, NeuronWeights()));
			}
			else
			{
				Layers.Last().Add(Neuron(Neuron_Type::Hidden, CurrentNeuron, Topology[CurrentLayer+1], WeightsArray[ArrayIndex]));
				++ArrayIndex;
			}
		}

		if (bBias && (CurrentLayer != Topology.Num()-1))
		{
			Layers.Last().Add(Neuron(Neuron_Type::Bias, Topology[CurrentLayer], Topology[CurrentLayer + 1], WeightsArray[ArrayIndex]));
			++ArrayIndex;
		}
	}
	return true;
}


bool UNet::LoadData(TArray<int> Topology)
{
	FString FileContent = "";
	FString FilePath = GetPathToData();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*FilePath))
	{
		FFileHelper::LoadFileToString(FileContent, *FilePath);
		
		auto TMPTopology = Topology;

		
		for (auto i = 0; i < Topology.Num() - 1; ++i)
		{
			if (useBias)
			{
				++TMPTopology[i];
			}
			NumberOfNeuronsWithWeights += TMPTopology[i];
		}
		

		for (auto i = 0; i < NumberOfNeuronsWithWeights; ++i)
		{
			WeightsArray.AddDefaulted();
		}

		int X = 0,//Номер нейрона
			Z = 1;//Номер слоя

		for (auto i = 0; i < NumberOfNeuronsWithWeights; ++i)
		{

			for (auto j = 0; j < Topology[Z]; ++j)
			{
				
				if (FileContent.Find(" ") == -1)
				{
					WeightsArray[i].Add(FCString::Atod(*FileContent));
					break;
				}
				
				auto CountDeleted = (FileContent.Find(" ")+1);
				WeightsArray[i].Add(FCString::Atod(*FileContent.Mid(0, CountDeleted)));
				FileContent.RemoveAt(0, CountDeleted);
			}

			if (X == TMPTopology[Z - 1]-1)
			{
				X = 0;
				++Z;
				continue;
			}
			++X;
		}
	}
	else
	{
		UE_LOG(LogStreaming, Warning, TEXT("Failed to read file '%s' error."), *FilePath);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "File not Found");
		return false;
	}
	return true;
}

void UNet::RandomizeWeights(TArray<int> Topology,float Min, float Max)
{
	auto TMPTopology = Topology;

	for (auto i = 0; i < Topology.Num() - 1; ++i)
	{
		if (useBias)
		{
			++TMPTopology[i];
		}
		NumberOfNeuronsWithWeights += TMPTopology[i];
	}

	for (auto i = 0; i < NumberOfNeuronsWithWeights; ++i)
	{
		WeightsArray.AddDefaulted();
	}

	int X = 0,//Номер нейрона
		Z = 1;//Номер слоя

	for (auto i = 0; i < NumberOfNeuronsWithWeights; ++i)
	{

		for (auto j = 0; j < Topology[Z]; ++j)
		{		
			WeightsArray[i].Add(FMath::RandRange(Min,Max));
		}
		if (X == TMPTopology[Z - 1] - 1)
		{
			X = 0;
			++Z;
			continue;
		}
		++X;
	}
}


bool UNet::SaveData()
{
	WeightsArray.Empty();
	for (int LayerNum = 0; LayerNum < Layers.Num()-1; ++LayerNum)
	{
		for (int n = 0; n < Layers[LayerNum].Num(); ++n)
		{
			WeightsArray.Add(Layers[LayerNum][n].GetOutputWeightsArray());			
		}
	}

	FString FileContent = "";
	FString FilePath = GetPathToData();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*FilePath))
	{
		for (int i = 0; i < WeightsArray.Num(); ++i)
		{
			for (int j = 0; j < WeightsArray[i].Num(); ++j)
			{
				FileContent.Append(FString::SanitizeFloat(WeightsArray[i][j]) + " ");
			}
		}
		FileContent.RemoveAt(FileContent.Len(), 1);
		FFileHelper::SaveStringToFile(FileContent, *FilePath);
	}
	else
	{
		UE_LOG(LogStreaming, Warning, TEXT("Failed to write file '%s' error."), *FilePath);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "File not Found");
		return false;
	}
	return true;
}

void UNet::StartNet(TArray<bool> InputVals)
{
	if (useBias)
	{
		check(InputVals.Num() == (Layers[0].Num() - 1));
	}
	else
	{
		check(InputVals.Num() == (Layers[0].Num()));
	}

	for (int i = 0; i < InputVals.Num(); ++i) {
		Layers[0][i].SetOutputVal((double)InputVals[i]);
	}
	
	for (int CurrentLayer = 1; CurrentLayer < Layers.Num(); ++CurrentLayer)
	{
		Layer &PrevLayer = Layers[CurrentLayer - 1];

		for (int n = 0; n < Layers[CurrentLayer].Num(); ++n)
		{
			if (Layers[CurrentLayer][n].GetType() == Neuron_Type::Bias)
			{continue;}

			Layers[CurrentLayer][n].MoveForward(PrevLayer);
		}
	}
}

void UNet::Backpropagation(TArray<bool> TargetVals)
{
	//Root MSE
	Layer &OutputLayer = Layers.Last();
	Error = 0.0;

	for (int n = 0; n < OutputLayer.Num(); ++n)
	{
		double Delta = TargetVals[n] - OutputLayer[n].GetOutputVal();
		Error += Delta * Delta;
	}
	Error /= OutputLayer.Num();
	Error = sqrt(Error);

	// Как сильно ошибается сеть
	RecentAverageError = (RecentAverageError * RecentAverageSmoothingFactor + Error)
		/ (RecentAverageSmoothingFactor + 1.0);

	///Вычисляем градиент

	//Градиент Output Layer
	for (int n = 0; n < OutputLayer.Num() ; ++n)
	{
		OutputLayer[n].CalculateOutputGradients(TargetVals[n]);
	}

	//Градиент Hidden Layer

	for (int LayerNum = Layers.Num() - 2; LayerNum > 0; --LayerNum)
	{
		Layer &HiddenLayer = Layers[LayerNum];
		Layer &NextLayer = Layers[LayerNum + 1];

		for (int n = 0; n < HiddenLayer.Num(); ++n)
		{
			HiddenLayer[n].CalculateHiddenGradients(NextLayer);
		}
	}

	/// Обновление весов

	for (int LayerNum = Layers.Num() - 1; LayerNum > 0; --LayerNum)
	{
		Layer &CurrentLayer= Layers[LayerNum];
		Layer &PrevLayer = Layers[LayerNum - 1];

		for (int n = 0; n < CurrentLayer.Num(); ++n)
		{
			if (CurrentLayer[n].GetType() == Neuron_Type::Bias) { continue; }

			CurrentLayer[n].UpdateInputWeights(PrevLayer);
		}

	}
	
}

TArray<double> UNet::GetResult()
{
	TArray<double> Result;
	for (int n = 0; n < Layers.Last().Num(); ++n)
	{
		Result.Add(Layers.Last()[n].GetOutputVal());
	}
	return Result;
}

bool UNet::ToBool(double Val)
{
	return Val >= 0.5;

}

void UNet::DebugFunction()
{

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue,"RAE = " + FString::SanitizeFloat(RecentAverageError));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, "NumOut = " + FString::FromInt (Layers.Last().Num()));
	
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue,"2 = " + FString::SanitizeFloat(Layers.Last()[1].GetOutputVal()));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue,"1 = " + FString::SanitizeFloat(Layers.Last()[0].GetOutputVal()));
	   	

	GEngine->AddOnScreenDebugMessage(-1, 0.f,FColor::Cyan, FString::FromInt(Layers[0].Num()) + " " + FString::FromInt(Layers[1].Num()) + " " + FString::FromInt(Layers[2].Num()));

}
