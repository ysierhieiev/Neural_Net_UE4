// Fill out your copyright notice in the Description page of Project Settings
#pragma once

#include "CoreMinimal.h"

class Neuron;

typedef TArray<Neuron> Layer;

enum Neuron_Type { Bias = 0, Input, Hidden, Output };

struct Connection
{
	float Weight;
	float DeltaWeight;

	Connection(float _Weight) { Weight = _Weight; }
};


class NEURONNET_API Neuron
{
public:
	Neuron();
	Neuron(Neuron_Type TNeuron, int NeuronIndex, int NumOutPuts, TArray<double> CWeights);
	~Neuron();

	TArray<double> GetOutputWeightsArray();

	double GetOutputVal() { return NOutputVal; }

	void SetOutputVal(double NOVal) { NOutputVal = NOVal; }

	void MoveForward(Layer &PrevLayer);

	double Summirise(Layer &PrevLayer);

	Neuron_Type GetType() { return NType; };

	void CalculateHiddenGradients(Layer &NextLayer);

	void CalculateOutputGradients(double TargetVals);

	void UpdateInputWeights(Layer &PrevLayer);


	
private:
	Neuron_Type NType;

	TArray<Connection> OutputWeights;

	int NIndex;

	double NOutputVal;

	double NGradient;

	double eta = 0.75; //[0.0 ... 1.0]
	double alpha = 0.8; //[0.0 ... n] 

	double SumDOW(Layer &NextLayer);


	double ActivationFunctionDerivativeSigmoid(double x);

	double ActivationFunctionSigmoid(float sum);

	double ActivationFunctionDerivativeTangh(double x);

};
