// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_UBIKSolver.generated.h"

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIK_Settings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	float UpperArmsDistance = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	float DistinctShoulderRotationMultiplier = 60.f;
};

/**
 *
 */
USTRUCT(BlueprintInternalUseOnly)
struct UBIKRUNTIME_API FAnimNode_UBIKSolver : public FAnimNode_SkeletalControlBase
{
    GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	FUBIK_Settings Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float UpperArmsDistance = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float DistinctShoulderRotationMultiplier = 60.f;

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	// End of FAnimNode_SkeletalControlBase interface

};

