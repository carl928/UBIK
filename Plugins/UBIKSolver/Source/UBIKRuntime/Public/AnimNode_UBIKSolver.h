// 2020 Sticky Snout Studio (Jonas M�lgaard)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
//#include "Animation/AnimNodeBase.h"
#include "UBIK.h"
#include "AnimNode_UBIKSolver.generated.h"

/**
 * UBIK Solver
 */
USTRUCT(BlueprintInternalUseOnly)
struct UBIKRUNTIME_API FAnimNode_UBIKSolver : public FAnimNode_SkeletalControlBase
//struct UBIKRUNTIME_API FAnimNode_UBIKSolver : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

public:
	/** Feed in the HMD transform in WorldSpace. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FTransform HeadTransformW;
	
	/** Feed in the Left MotionController in WorldSpace. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FTransform LeftHandTransformW;
	
	/** Feed in the Right MotionController in WorldSpace. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FTransform RightHandTransformW;

	/** These settings will be returned by calling the GetUBIKSettings function. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (PinShownByDefault))
	FUBIKSettings Settings;

	/** Set this to get debug draws of certain internal transforms. Used only for debugging purposes. **/
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

	// Input link (Only in use if base is FAnimNode_Base, otherwise it's already defined inside the FAnimNode_SkeletalControlBase)
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	//FComponentSpacePoseLink ComponentPose;

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	//virtual void EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output);
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones);
	// End of FAnimNode_SkeletalControlBase interface

private:
	FTransform ComponentSpaceW;
	FTransform ShoulderTransformW;

	FTransform HeadTransformC;
	FTransform LeftHandTransformC;
	FTransform RightHandTransformC;
	FTransform ShoulderTransformC;
	FTransform LeftClavicleC;	// TODO: May be able to turn into a FRotator instead
	FTransform RightClavicleC;	// TODO: May be able to turn into a FRotator instead

	/** WorldSpace inverted **/
	FTransform ShoulderTransform;
	FTransform ComponentSpace;

	FTransform HeadTransformS;
	FTransform LeftHandTransformS;
	FTransform RightHandTransformS;
	FTransform LeftUpperArmTransformS;
	FTransform RightUpperArmTransformS;

	float LeftHeadHandAngle;
	float RightHeadHandAngle;

	void ConvertTransforms();
	void SetShoulder();

	FVector GetShoulderLocation();
	FRotator GetShoulderRotationFromHead();
	FRotator GetShoulderRotationFromHands();
	float GetHeadHandAngle(float LastAngle, FVector Hand, FVector HandHeadDelta);
	void SetLeftUpperArm();
	void SetRightUpperArm();
	FTransform RotateUpperArm(bool IsLeftArm, FVector HandTranslation);
};

