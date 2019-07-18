#include "Mark.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "NeuronNet/Private/NeuralNet/Net.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Controller.h"


AMark::AMark()
{
	PrimaryActorTick.bCanEverTick = true;

	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	LeftBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftBoxTrigger"));
	LeftBoxTrigger->SetupAttachment(RootComponent);
	LeftBoxTrigger->SetRelativeScale3D(FVector(0.25f, 2.1f, 0.25f));
	LeftBoxTrigger->SetRelativeLocation(FVector(0.f, -103.2f, 0.f));
	LeftBoxTrigger->SetCollisionProfileName(TEXT("Trigger"));

	FrontBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FrontBoxTrigger"));
	FrontBoxTrigger->SetupAttachment(RootComponent);
	FrontBoxTrigger->SetRelativeScale3D(FVector(2.1f, 0.25f, 0.25f));
	FrontBoxTrigger->SetRelativeLocation(FVector(103.2f, 0.f, 0.f));
	FrontBoxTrigger->SetCollisionProfileName(TEXT("Trigger"));

	RightBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RightBoxTrigger"));
	RightBoxTrigger->SetupAttachment(RootComponent);
	RightBoxTrigger->SetRelativeScale3D(FVector(0.25f, 2.1f, 0.25f));
	RightBoxTrigger->SetRelativeLocation(FVector(0.f, 103.2f, 0.f));
	RightBoxTrigger->SetCollisionProfileName(TEXT("Trigger"));

	//------------------------------------Arrows------------------------------------\\

	LeftArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftArrow"));
	LeftArrow->SetupAttachment(LeftBoxTrigger);
	LeftArrow->SetRelativeLocationAndRotation(FVector(0.f, 32.f, 0.f), FRotator(0.f, -90.f, 0.f));
	LeftArrow->SetRelativeScale3D(FVector(6.7f, 0.8f, 6.f));
	LeftArrow->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftArrow->SetArrowColor(FColor::Green);
	LeftArrow->bHiddenInGame = false;

	FrontArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontArrow"));
	FrontArrow->SetupAttachment(FrontBoxTrigger);
	FrontArrow->SetRelativeLocationAndRotation(FVector(-32.f, 0.f, 0.f), FRotator(0.f, 0.f, 90.f));
	FrontArrow->SetRelativeScale3D(FVector(0.8f, 6.4f, 6.f));
	FrontArrow->SetCollisionResponseToAllChannels(ECR_Ignore);
	FrontArrow->SetArrowColor(FColor::Green);
	FrontArrow->bHiddenInGame = false;

	RightArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightArrow"));
	RightArrow->SetupAttachment(RightBoxTrigger);
	RightArrow->SetRelativeLocationAndRotation(FVector(0.f, -32.f, 0.f), FRotator(0.f, 90.f, 0.f));
	RightArrow->SetRelativeScale3D(FVector(6.7f, 0.8f, 6.f));
	RightArrow->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightArrow->SetArrowColor(FColor::Green);
	RightArrow->bHiddenInGame = false;


	//------------------------------------Neural Net------------------------------------\\
	
	NNet = CreateDefaultSubobject<UNet>(TEXT("NeuralNet"));

}


void AMark::BeginPlay()
{
	Super::BeginPlay();

	Direction = StartDirection = GetActorForwardVector();

	LeftBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMark::OnLeftTriggetOn);
	LeftBoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AMark::OnLeftTriggetOff);

	FrontBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMark::OnFrontTriggetOn);
	FrontBoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AMark::OnFrontTriggetOff);

	RightBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMark::OnRightTriggetOn);
	RightBoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AMark::OnRightTriggetOff);


	//------------------------------------CAPSULE------------------------------------\\	
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMark::MarkHit);

	StartLocation = GetActorLocation();

	StartRotation = GetActorRotation();

	NeedMove = true;

	//------------------------------------Neural Net------------------------------------\\

	TArray<int> Top = { 3,6, 6, 2 };

	check(NNet->CreateNet(Top,false));

	//NNet->DebugFunction();
	
	///Мнгновенная тренировка
	/*
	for (auto i = 0; i < 10000; ++i)
	{
		TriggerStats = { FMath::RandBool(), FMath::RandBool(),FMath::RandBool() };
		NNet->StartNet(TriggerStats);

		NNet->Backpropagation(ExpectedResult(TriggerStats));
	}
	*/

	GetWorld()->GetTimerManager().SetTimer(NetWorkTimerHandle, this, &AMark::WorkNet, 0.3f, true);

}

void AMark::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearTimer(NetWorkTimerHandle);
	NNet->SaveData();
}

void AMark::OnLeftTriggetOn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	++TriggerCounts[0];
	LeftArrow->SetArrowColor(FColor::Red);
	TriggerStats[0] = true;
	
}

void AMark::OnLeftTriggetOff(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//--TriggerCounts[0];
	if(--TriggerCounts[0] == 0)
	{
		LeftArrow->SetArrowColor(FColor::Green);
		TriggerStats[0] = false;
	}
}

void AMark::OnFrontTriggetOn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	++TriggerCounts[1];
	FrontArrow->SetArrowColor(FColor::Red);
	TriggerStats[1] = true;
}

void AMark::OnFrontTriggetOff(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (--TriggerCounts[1] == 0)
	{
		FrontArrow->SetArrowColor(FColor::Green);
		TriggerStats[1] = false;
	}
}

void AMark::OnRightTriggetOn(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	++TriggerCounts[2];
	RightArrow->SetArrowColor(FColor::Red);
	TriggerStats[2] = true;
}

void AMark::OnRightTriggetOff(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (--TriggerCounts[2] == 0)
	{
		RightArrow->SetArrowColor(FColor::Green);
		TriggerStats[2] = false;
	}
}

void AMark::MarkHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	NeedMove = false;

	NNet->Backpropagation(ExpectedResult(TriggerStats));
	
	++Iteration;
	SetActorLocationAndRotation(StartLocation, StartRotation);
	Direction = StartDirection;


	NeedMove = true;
}

void AMark::Turn(bool LeftTurn, bool RightTurn)
{
	if (LeftTurn && RightTurn)
	{		
		Direction = -1.f * GetActorForwardVector();
			
	}
	else if (LeftTurn)
	{
		Direction = FVector(-1) * GetActorRightVector();
	}
	else if(RightTurn)
	{
		Direction = GetActorRightVector();
	}
}

void AMark::WorkNet()
{
	//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Magenta, "Work Net, time: " + FString::SanitizeFloat(UGameplayStatics::GetRealTimeSeconds(GetWorld())));

	NNet->StartNet(TriggerStats);
	
	NNet->Backpropagation(ExpectedResult(TriggerStats));

	
	TArray<double> TMPR = NNet->GetResult();
	   	
	Turn(NNet->ToBool(TMPR[0]), NNet->ToBool(TMPR[1]));
}

TArray<bool> AMark::ExpectedResult(TArray<bool> Status)
{
	TArray<bool> ExpResult;

	if (Status[0] == false && Status[1] == false && Status[2] == false)
	{
		ExpResult = { false, false };
	}
	else if (Status[0] == false && Status[1] == false && Status[2] == true)
	{
		ExpResult = { false, false };
	}
	else if (Status[0] == false && Status[1] == true && Status[2] == false)
	{
		if (FMath::RandBool())
		{
			ExpResult = { false, true };
		}
		else
		{
			ExpResult = { true, false };
		}
	}
	else if (Status[0] == false && Status[1] == true && Status[2] == true) // 4
	{
		ExpResult = { true, false };
	}
	if (Status[0] == true && Status[1] == false && Status[2] == false)
	{
		ExpResult = { false, false };
	}
	else if (Status[0] == true && Status[1] == false && Status[2] == true)
	{
		ExpResult = { false, false };
	}
	else if (Status[0] == true && Status[1] == true && Status[2] == false)
	{
		ExpResult = { false, true };
	}
	else if (Status[0] == true && Status[1] == true && Status[2] == true)
	{
		ExpResult = { true, true };
	}
	return ExpResult;
}




void AMark::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	NNet->DebugFunction();
	if(NeedMove)
	AddMovementInput(Direction, 0.4f);

}


void AMark::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

