// 2020 Sticky Snout Studio (Jonas Mølgaard)

#include "AnimNode_UBIKSolver.h"
#include "UBIKRuntime.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

void FAnimNode_UBIKSolver::GatherDebugData(FNodeDebugData& DebugData)
{
}

/* Only valid if i base this off FAnimNode_SkeletalControlBase */
void FAnimNode_UBIKSolver::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	ComponentSpaceW = Output.AnimInstanceProxy->GetComponentTransform();
	ComponentSpace = ComponentSpaceW.Inverse();

	ConvertTransforms();
	SetShoulder();
	
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftTransformW is: %s"), *LeftTransformW.ToString());
}

bool FAnimNode_UBIKSolver::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return true;
}

/** SideEffects: Updates the following MemberVariables:
* ShoulderTransformW, ShoulderTransform, LeftTransformW, RightTransformW, 
* HeadTransformC, LeftTransformC, RightTransformC
**/
void FAnimNode_UBIKSolver::ConvertTransforms()
{
	// Technically ShoulderTransformC should be set 1st
	ShoulderTransformW = ShoulderTransformC * ComponentSpaceW;
	ShoulderTransform = ShoulderTransformW.Inverse();

	LeftTransformW = UUBIK::AddLocalOffset(LeftTransformW, FVector(8.f, 0.f, 0.f));
	RightTransformW = UUBIK::AddLocalOffset(LeftTransformW, FVector(8.f, 0.f, 0.f));

	HeadTransformC = HeadTransformW * ComponentSpace;
	LeftTransformC = LeftTransformW * ComponentSpace;
	RightTransformC = RightTransformW * ComponentSpace;
}

/** SideEffects: Updates the following MemberVariables:
* ShoulderTransformW, HeadTransformS, LeftTransformS, RightTransformS
**/
void FAnimNode_UBIKSolver::SetShoulder()
{
	FRotator RotationFromHand; // TODO
	FRotator RotationFromHands; // TODO
	FRotator Rotation = FMath::Lerp(RotationFromHand, RotationFromHands, 0.7f);
	FVector Translation = GetShoulderLocation();
	// UE_LOG(UBIKRuntimeLog, Display, TEXT("GetShoulderLocation: %s"), *Translation.ToString());

	ShoulderTransformC = FTransform(Rotation, Translation, FVector::OneVector);

	ShoulderTransformW = ShoulderTransformC * ComponentSpaceW;
	HeadTransformS = HeadTransformW * ShoulderTransform;
	LeftTransformS = LeftTransformW * ShoulderTransform;
	RightTransformS = RightTransformW * ShoulderTransform;
}

FVector FAnimNode_UBIKSolver::GetShoulderLocation()
{
	FVector Offset = FVector(-9.f, 0.f, -7.f);
	FRotator HeadRotator = HeadTransformC.GetRotation().Rotator();
	FRotator HeadYaw = FRotator(0.f, HeadRotator.Yaw, HeadRotator.Roll);
	Offset = HeadYaw.RotateVector(Offset);
	FRotator Delta = FRotator(HeadRotator.Pitch, 0.f, 0.f);
	FVector ShoulderTranslation = HeadTransformC.GetTranslation() + Offset;
	FTransform TempShoulder = FTransform(FRotator::ZeroRotator, ShoulderTranslation, FVector::OneVector);
	FTransform TempRotatedShoulder = UUBIK::RotatePointAroundPivot(TempShoulder, HeadTransformC, Delta);
	return TempRotatedShoulder.GetTranslation() + FVector(0.f, 0.f, -17.f);
}

FRotator FAnimNode_UBIKSolver::GetShoulderRotationFromHead()
{
	return FRotator();
}

FRotator FAnimNode_UBIKSolver::GetShoulderRotationFromHands()
{
	return FRotator();
}

/* Only valid if i base this off FAnimNode_Base */
//void FAnimNode_UBIKSolver::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
//{
//}


