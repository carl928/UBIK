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
	FRotator RotationFromHand = GetShoulderRotationFromHead();
	FRotator RotationFromHands = GetShoulderRotationFromHands();
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
	FRotator HeadRotator = HeadTransformC.Rotator();
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
	return FRotator(0.f, HeadTransformC.Rotator().Yaw, 0.f);
}

FRotator FAnimNode_UBIKSolver::GetShoulderRotationFromHands()
{
	FVector Offset = FVector(0.f, 0.f, 15.f);
	FVector TopHeadTranslation = HeadTransformW.GetTranslation() + HeadTransformW.Rotator().RotateVector(Offset);

	FVector Head = (LeftTransformW * HeadTransformW.Inverse()).GetTranslation();
	FVector HandHeadDelta = LeftTransformW.GetTranslation() - TopHeadTranslation;
	
	LeftHeadHandAngle = GetHeadHandAngle(LeftHeadHandAngle, Head, HandHeadDelta);
	UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftHeadHandAngle: %f"), LeftHeadHandAngle);

	return FRotator();
}

float FAnimNode_UBIKSolver::GetHeadHandAngle(float LastAngle, FVector Hand, FVector HandHeadDelta)
{
	float Alpha = UKismetMathLibrary::MapRangeClamped(FVector(HandHeadDelta.X, HandHeadDelta.Y, 0.f).Size(), 20.f, 50.f, 0.f, 1.f);
	FVector Hand2D = FVector(Hand.X, Hand.Y, 0.f);
	FVector Hand2DNormalized = Hand2D.GetSafeNormal();
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Hand2DNormalized.Y, Hand2DNormalized.X));

	bool Selector1 = FVector::DotProduct(Hand2DNormalized, FVector::ForwardVector) > Settings.HeadHandAngleLimitDot;
	bool Selector2 = FMath::Sign(LastAngle) == FMath::Sign(Angle) || Angle < Settings.OkSpanAngle && Angle > -Settings.OkSpanAngle;
	bool Selector = Selector1 && Selector2;

	float SelectedFloat = (Selector) ? (Angle) : (Settings.HeadHandAngleLimit * FMath::Sign(LastAngle));

	return FMath::Lerp(0.f, SelectedFloat, Alpha);
}

/* Only valid if i base this off FAnimNode_Base */
//void FAnimNode_UBIKSolver::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
//{
//}


