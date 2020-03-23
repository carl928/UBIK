// 2020 Sticky Snout Studio (Jonas Mølgaard)

#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_UBIKSolver.generated.h"

/**
 *
 */
USTRUCT(BlueprintInternalUseOnly)
struct UBIKRUNTIME_API FAnimNode_UBIKSolver : public FAnimNode_SkeletalControlBase
{
    GENERATED_USTRUCT_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))

	//** Head bone to modify **/
	UPROPERTY(EditAnywhere, Category = HeadBone)
	FBoneReference HeadBoneToModify;

	/** Left Clavicle bone to modify **/
	UPROPERTY(EditAnywhere, Category = ClavicleBone)
	FBoneReference LeftClavicleBoneToModify;

	/** Right Clavicle Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = ClavicleBone)
	FBoneReference RightClavicleBoneToModify;

	/** Left Upper Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = UpperArm)
	FBoneReference LeftUpperArmBoneToModify;

	/** Right Upper Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = UpperArm)
	FBoneReference RightUpperArmBoneToModify;

	/** Left Lower Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = LowerArm)
	FBoneReference LeftLowerArmBoneToModify;

	/** Right Lower Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = LowerArm)
	FBoneReference RightLowerArmBoneToModify;

	/** Left Hand bone to modify **/
	UPROPERTY(EditAnywhere, Category = Hand)
	FBoneReference LeftHandBoneToModify;

	/** Right Hand bone to modify **/
	UPROPERTY(EditAnywhere, Category = Hand)
	FBoneReference RightHandBoneToModify;

	//** Pelvis1 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine)
	FBoneReference Spine01_BoneToModify;

	//** Pelvis2 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine)
	FBoneReference Spine02_BoneToModify;

	//** Pelvis3 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine)
	FBoneReference Spine03_BoneToModify;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
		FTransform HeadEffector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
		FTransform LeftEffector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
		FTransform RightEffector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float UpperArmsDistance = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float DistinctShoulderRotationMultiplier = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float DistinctShoulderRotationLimit = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float ClavicleOffset = -32.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float ElbowBaseOffsetAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float ElbowYDistanceStart = .2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float ElbowYWeight = 130.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float ElbowHandsRotSpeed = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float HeadHandsAngleLimit = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
		float HeadHandAngleOkSpan = 80.f;

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	// End of FAnimNode_SkeletalControlBase interface

};

