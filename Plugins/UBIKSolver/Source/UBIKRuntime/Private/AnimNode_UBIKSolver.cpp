// 2020 Sticky Snout Studio (Jonas Mølgaard)

#include "AnimNode_UBIKSolver.h"

void FAnimNode_UBIKSolver::GatherDebugData(FNodeDebugData& DebugData)
{
}

void FAnimNode_UBIKSolver::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
}

bool UUBIKCalibrate::Calibrate()
{
	return true;
}
