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
	
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftTransformW is: %s"), *LeftTransformW.ToString());
}

bool FAnimNode_UBIKSolver::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return true;
}

void FAnimNode_UBIKSolver::ConvertTransforms()
{
	// Technically ShoulderTransformC should be set 1st
	ShoulderTransformW = ShoulderTransformC * ComponentSpaceW;
	FTransform ShoulderTransform = ShoulderTransformW.Inverse();

	LeftTransformW = UUBIK::AddLocalOffset(LeftTransformW, FVector(8.f, 0.f, 0.f));
	RightTransformW = UUBIK::AddLocalOffset(LeftTransformW, FVector(8.f, 0.f, 0.f));

	HeadTransformC = HeadTransformW * ComponentSpace;
	LeftTransformC = LeftTransformW * ComponentSpace;
	RightTransformC = RightTransformW * ComponentSpace;

}

/* Only valid if i base this off FAnimNode_Base */
//void FAnimNode_UBIKSolver::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
//{
//}


