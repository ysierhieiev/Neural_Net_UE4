#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Mark.generated.h"

class UBoxComponent;

UCLASS()
class NEURONNET_API AMark : public ACharacter
{
	GENERATED_BODY()

public:
	
	AMark();

protected:
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TArray<bool> ExpectedResult(TArray<bool> Status);

	FVector StartLocation;

	FRotator StartRotation;

	FVector StartDirection;

	FVector Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool NeedMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* LeftBoxTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* FrontBoxTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* RightBoxTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* LeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* FrontArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* RightArrow;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Information")
	TArray<bool> TriggerStats = { false,false,false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Information")
	TArray<int> TriggerCounts = { 0,0,0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNet* NNet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	int Iteration = 0;

	FTimerHandle NetWorkTimerHandle;

	UFUNCTION()
	void OnLeftTriggetOn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnLeftTriggetOff(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnFrontTriggetOn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnFrontTriggetOff(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRightTriggetOn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnRightTriggetOff(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void MarkHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Turn(bool LeftTurn, bool RightTurn);

	UFUNCTION()
	void WorkNet();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



};
