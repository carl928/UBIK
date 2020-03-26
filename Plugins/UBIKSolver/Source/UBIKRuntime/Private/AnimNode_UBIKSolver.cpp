// 2020 Sticky Snout Studio (Jonas Mølgaard)

#include "AnimNode_UBIKSolver.h"

void FAnimNode_UBIKSolver::GatherDebugData(FNodeDebugData& DebugData)
{
}

/* Only valid if i base this off FAnimNode_SkeletalControlBase */
void FAnimNode_UBIKSolver::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	ConvertTransforms(LeftWorldTransform);
	UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftWorldTransform is: "), *LeftWorldTransform.ToString());
}

void FAnimNode_UBIKSolver::ConvertTransforms(FTransform& LeftTransform)
{
	LeftTransform = UUBIK::AddLocalOffset(LeftWorldTransform, FVector(8.f, 0.f, 0.f));
}

/* Only valid if i base this off FAnimNode_Base */
//void FAnimNode_UBIKSolver::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
//{
//}


