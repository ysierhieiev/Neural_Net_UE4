#include "NeuronNet/Public/NeuralNet/Neuron.h"


Neuron::Neuron()
{
}

Neuron::Neuron(Neuron_Type TNeuron, int NeuronIndex, int NumOutPuts, TArray<double> CWeights)
{
	NIndex = NeuronIndex;
	NType = TNeuron;

	switch (TNeuron)
	{
	case 0: NOutputVal = 1.f;
		
	case 1:

	case 2:
		for (auto i = 0; i < NumOutPuts; ++i)
		{
			OutputWeights.Add(Connection(CWeights[i]));
		}
		break;
	case 3: break;
	default: break;
	}
}



Neuron::~Neuron()
{

}

TArray<double> Neuron::GetOutputWeightsArray()
{
	TArray<double> WeightArray;
	for (int i = 0; i < OutputWeights.Num(); ++i)
	{
		WeightArray.Add(OutputWeights[i].Weight);
	}
	return WeightArray;
}

void Neuron::MoveForward(Layer &PrevLayer)
{
	auto sum = Summirise(PrevLayer);


	NOutputVal = Neuron::ActivationFunctionSigmoid(sum);

}

double Neuron::Summirise(Layer &PrevLayer)
{
	double sum = 0.0;

	for (int n = 0; n < PrevLayer.Num(); ++n)
	{
		sum += PrevLayer[n].GetOutputVal() *
			PrevLayer[n].OutputWeights[NIndex].Weight;
	}
	return sum;
}

double Neuron::SumDOW(Layer &NextLayer)
{
	double sum = 0.0;

	for (int n = 0; n < NextLayer.Num(); ++n)
	{
		if (NextLayer[n].GetType() == Neuron_Type::Bias) { continue; }

		sum += OutputWeights[n].Weight * NextLayer[n].NGradient;
	}

	return sum;
}

double Neuron::ActivationFunctionSigmoid(float sum)
{
	return 1 / (1 + FMath::Exp(-1 * (sum)));
}

void Neuron::CalculateHiddenGradients(Layer &NextLayer)
{
	double  DerivativeOutputWeights = SumDOW(NextLayer);
	NGradient = DerivativeOutputWeights * Neuron::ActivationFunctionDerivativeSigmoid(NOutputVal);
}

void Neuron::CalculateOutputGradients(double TargetVals)
{

	double Delta = TargetVals - NOutputVal;
	NGradient = Delta * Neuron::ActivationFunctionDerivativeSigmoid(NOutputVal);

}

void Neuron::UpdateInputWeights(Layer & PrevLayer)
{
	for (int n = 0; n < PrevLayer.Num(); ++n)
	{
		Neuron &PrevNeuron = PrevLayer[n];
		float OldDeltaWeight = PrevNeuron.OutputWeights[NIndex].DeltaWeight;

		float NewDeltaWeight = eta * PrevNeuron.GetOutputVal() * NGradient + alpha * OldDeltaWeight;
		PrevNeuron.OutputWeights[NIndex].DeltaWeight = NewDeltaWeight;
		PrevNeuron.OutputWeights[NIndex].Weight += NewDeltaWeight;
	}
}

double Neuron::ActivationFunctionDerivativeSigmoid(double x)
{
	return (1 - x) * x;
}

double Neuron::ActivationFunctionDerivativeTangh(double x)
{
	return 1 - x*x;
}
