// 2020 Sticky Snout Studio (Jonas Mølgaard)

#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "UBIK.h"
#include "AnimNode_UBIKSolver.generated.h"

/**
 * UBIK Solver
 */
USTRUCT(BlueprintInternalUseOnly)
struct UBIKRUNTIME_API FAnimNode_UBIKSolver : public FAnimNode_SkeletalControlBase
{
    GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault, ToolTip = "Feed in the HMD transform in WorldSpace."))
	FTransform HeadEffector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault, ToolTip = "Feed in the Left MotionController in WorldSpace."))
	FTransform LeftEffector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault, ToolTip = "Feed in the Right MotionController in WorldSpace."))
	FTransform RightEffector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault, ToolTip = ""))
	FUBIKSettings Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	bool bDrawDebug = false;

	//** Head bone to modify **/
	UPROPERTY(EditAnywhere, Category = HeadBone)
	FBoneReference HeadBoneToModify = FBoneReference("head");

	/** Left Clavicle bone to modify **/
	UPROPERTY(EditAnywhere, Category = ClavicleBone)
	FBoneReference LeftClavicleBoneToModify = FBoneReference("clavicle_l");

	/** Right Clavicle Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = ClavicleBone)
	FBoneReference RightClavicleBoneToModify = FBoneReference("clavicle_r");

	/** Left Upper Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = UpperArm)
	FBoneReference LeftUpperArmBoneToModify = FBoneReference("upperarm_l");

	/** Right Upper Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = UpperArm)
	FBoneReference RightUpperArmBoneToModify = FBoneReference("upperarm_r");

	/** Left Lower Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = LowerArm)
	FBoneReference LeftLowerArmBoneToModify = FBoneReference("lowerarm_l");;

	/** Right Lower Arm bone to modify **/
	UPROPERTY(EditAnywhere, Category = LowerArm)
	FBoneReference RightLowerArmBoneToModify = FBoneReference("lowerarm_r");

	/** Left Hand bone to modify **/
	UPROPERTY(EditAnywhere, Category = Hand)
	FBoneReference LeftHandBoneToModify = FBoneReference("hand_l");

	/** Right Hand bone to modify **/
	UPROPERTY(EditAnywhere, Category = Hand)
	FBoneReference RightHandBoneToModify = FBoneReference("hand_r");

	//** Spine01 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine_and_Pelvis)
	FBoneReference Spine01_BoneToModify = FBoneReference("spine_01");

	//** Spine02 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine_and_Pelvis)
	FBoneReference Spine02_BoneToModify = FBoneReference("spine_02");

	//** Spine03 bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine_and_Pelvis)
	FBoneReference Spine03_BoneToModify = FBoneReference("spine_03");

	//** Pelvis bone to modify **/
	UPROPERTY(EditAnywhere, Category = Spine_and_Pelvis)
	FBoneReference PelvisBoneToModify = FBoneReference("pelvis");

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	// End of FAnimNode_SkeletalControlBase interface

};

