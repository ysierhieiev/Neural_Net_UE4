
#pragma once

#include "CoreMinimal.h"	
#include "Components/ActorComponent.h"
#include "NeuronNet/Public/NeuralNet/Neuron.h"
#include "Net.generated.h"

typedef TArray<double> NeuronWeights;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNet : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UNet();

protected:

	virtual void BeginPlay() override;

	TArray<NeuronWeights> WeightsArray;

	TArray<Layer> Layers;

	double Error;

	double RecentAverageError;

	double RecentAverageSmoothingFactor = 100;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FString GetPathToData();

	bool CreateNet();

	bool CreateNet(TArray<int> &Topology, bool bBias = false);

	bool LoadData(TArray<int> Topology);

	void RandomizeWeights(TArray<int> Topology, float Min = -1.f, float Max = 1.f);

	bool SaveData();

	// начало операции

	void StartNet(TArray<bool> InputVals);

	void Backpropagation(TArray<bool> TargetVals);



	TArray<double> GetResult();

	bool ToBool(double Val);


	void DebugFunction();



private:
	
	bool useBias = false;

	int NumberOfNeuronsWithWeights = 0;

};
