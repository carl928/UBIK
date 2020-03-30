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

	MeshComponent = Output.AnimInstanceProxy->GetSkelMeshComponent();

	ConvertTransforms();
	
	SetShoulder();
	SetLeftUpperArm();
	SetRightUpperArm();

	ResetUpperArmsLocation();
	SolveArms();
	
	// SetBaseCharTransform();

	if (bDrawDebug)
	{
		// DebugDraw();
	}
}

bool FAnimNode_UBIKSolver::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return true;
}

void FAnimNode_UBIKSolver::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);

	DeltaTime += Context.GetDeltaTime();
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

	LeftHandTransformW = UUBIK::AddLocalOffset(LeftHandTransformW, FVector(8.f, 0.f, 0.f));
	RightHandTransformW = UUBIK::AddLocalOffset(RightHandTransformW, FVector(8.f, 0.f, 0.f));

	HeadTransformC = HeadTransformW * ComponentSpace;
	LeftHandTransformC = LeftHandTransformW * ComponentSpace;
	RightHandTransformC = RightHandTransformW * ComponentSpace;
}

/** SideEffects: Updates the following MemberVariables:
* ShoulderTransformW, HeadTransformS, LeftTransformS, RightTransformS
**/
void FAnimNode_UBIKSolver::SetShoulder()
{
	FRotator RotationFromHead = GetShoulderRotationFromHead();
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RotationFromHead: %s"), *RotationFromHead.ToString());

	FRotator RotationFromHands = GetShoulderRotationFromHands();
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RotationFromHands: %s"), *RotationFromHands.ToString());

	FRotator Rotation = FMath::Lerp(RotationFromHead, RotationFromHands, 0.7f);
	FVector Translation = GetShoulderLocation();
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("GetShoulderLocation: %s"), *Translation.ToString());

	ShoulderTransformC = FTransform(Rotation, Translation, FVector::OneVector);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("ShoulderTransformC: %s"), *ShoulderTransformC.ToString());

	ShoulderTransformW = ShoulderTransformC * ComponentSpaceW;
	HeadTransformS = HeadTransformW * ShoulderTransform;
	LeftHandTransformS = LeftHandTransformW * ShoulderTransform;
	RightHandTransformS = RightHandTransformW * ShoulderTransform;
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

	// TODO: Repurposing the variables below. Refactor this later
	FVector Hand = (LeftHandTransformW * HeadTransformW.Inverse()).GetTranslation();
	FVector HandHeadDelta = LeftHandTransformW.GetTranslation() - TopHeadTranslation;
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LEFTHAND: LeftTransformW: %s LastAngle: %f Hand: %s HandHeadDelta: %s"), *LeftTransformW.ToString(), LeftHeadHandAngle, *Hand.ToString(), *HandHeadDelta.ToString());

	LeftHeadHandAngle = GetHeadHandAngle(LeftHeadHandAngle, Hand, HandHeadDelta);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftHeadHandAngle: %f"), LeftHeadHandAngle);

	Hand = (RightHandTransformW * HeadTransformW.Inverse()).GetTranslation();
	HandHeadDelta = RightHandTransformW.GetTranslation() - TopHeadTranslation;
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RIGHTHAND: RightTransformW: %s LastAngle: %f Hand: %s HandHeadDelta: %s"), *RightTransformW.ToString(), RightHeadHandAngle, *Hand.ToString(), *HandHeadDelta.ToString());
	RightHeadHandAngle = GetHeadHandAngle(RightHeadHandAngle, Hand, HandHeadDelta);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RightHeadHandAngle: %f"), RightHeadHandAngle);

	FRotator YawRotator = FRotator(0.f, ((LeftHeadHandAngle + RightHeadHandAngle) / 2.f), 0.f);
	FTransform TempTransform = FTransform(YawRotator, FVector::ZeroVector, FVector::OneVector);

	// Back in WorldSpace
	TempTransform = TempTransform * HeadTransformW;

	TempTransform = TempTransform * ComponentSpace;

	return TempTransform.Rotator();
}

float FAnimNode_UBIKSolver::GetHeadHandAngle(float LastAngle, FVector Hand, FVector HandHeadDelta)
{
	float HeadHandAlpha = UKismetMathLibrary::MapRangeClamped(FVector(HandHeadDelta.X, HandHeadDelta.Y, 0.f).Size(), 20.f, 50.f, 0.f, 1.f);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("HeadHandAlpha: %f"), HeadHandAlpha);

	FVector Hand2D = FVector(Hand.X, Hand.Y, 0.f);
	FVector Hand2DNormalized = Hand2D.GetSafeNormal();
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Hand2DNormalized.Y, Hand2DNormalized.X));
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("Angle: %f"), Angle);

	bool Selector1 = FVector::DotProduct(Hand2DNormalized, FVector::ForwardVector) > Settings.HeadHandAngleLimitDot;
	bool Selector2 = FMath::Sign(LastAngle) == FMath::Sign(Angle) || 
		Angle < Settings.OkSpanAngle && Angle > -Settings.OkSpanAngle;
	bool Selector = Selector1 && Selector2;

	float SelectedFloat = (Selector) ? (Angle) : (Settings.HeadHandAngleLimit * FMath::Sign(LastAngle));
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("SelectedFloat: %f"), SelectedFloat);

	return FMath::Lerp(0.f, SelectedFloat, HeadHandAlpha);
}

void FAnimNode_UBIKSolver::SetLeftUpperArm()
{
	LeftUpperArmTransformS = RotateUpperArm(true, LeftHandTransformS.GetTranslation());
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftUpperArmTransformS %s"), *LeftUpperArmTransformS.ToString());

	FVector X = (LeftUpperArmTransformS * ShoulderTransformW).GetTranslation() - ShoulderTransformW.GetTranslation();
	FVector Z = FVector::UpVector;

	LeftClavicleC = FTransform(UKismetMathLibrary::MakeRotFromXZ(X, Z), FVector::ZeroVector, FVector::OneVector) * ComponentSpace;
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftClavicleC %s"), *LeftClavicleC.ToString());
}

void FAnimNode_UBIKSolver::SetRightUpperArm()
{
	RightUpperArmTransformS = RotateUpperArm(false, RightHandTransformS.GetTranslation());
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RightUpperArmTransformS %s"), *RightUpperArmTransformS.ToString());

	FVector X = (RightUpperArmTransformS * ShoulderTransformW).GetTranslation() - ShoulderTransformW.GetTranslation();
	FVector Z = FVector::UpVector;

	RightClavicleC = FTransform(UKismetMathLibrary::MakeRotFromXZ(X, Z), FVector::ZeroVector, FVector::OneVector) * ComponentSpace;
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RightClavicleC %s"), *RightClavicleC.ToString());
}

FTransform FAnimNode_UBIKSolver::RotateUpperArm(bool IsLeftArm, FVector HandTranslation)
{
	float Sign = (IsLeftArm) ? (1.f) : (-1.f);

	FVector InitialUpperArmTranslation = FVector::RightVector * Settings.UpperArmsDistance / (2.f * Sign);
	FVector HandUpperArmDirection = HandTranslation - InitialUpperArmTranslation;
	
	float ForwardDistanceRatio = FVector::DotProduct(HandUpperArmDirection, FVector::ForwardVector) / Settings.ArmLength;
	float UpwardsDistanceRatio = FVector::DotProduct(HandUpperArmDirection, FVector::UpVector) / Settings.ArmLength;
	
	float Yaw;
	if (ForwardDistanceRatio > 0)
	{
		float TempYaw = (ForwardDistanceRatio - 0.5f) * Settings.DistinctShoulderRotationMultiplier;
		Yaw = FMath::Clamp(TempYaw, 0.f, Settings.DistinctShoulderRotationLimit) + Settings.ClavicleOffset;
	}
	else
	{
		float TempYaw = (ForwardDistanceRatio - 0.08f) * Settings.DistinctShoulderRotationMultiplier;
		Yaw = FMath::Clamp(TempYaw, -Settings.DistinctShoulderRotationLimit, 0.f) + Settings.ClavicleOffset;
	}

	float TempRoll = (UpwardsDistanceRatio - 0.2f)* Settings.DistinctShoulderRotationMultiplier;
	float Roll = FMath::Clamp(TempRoll, 0.f, Settings.DistinctShoulderRotationLimit);

	float NotSign = (IsLeftArm) ? (-1.f) : (1.f);
	FRotator Rotation = FRotator(0.f, Yaw * NotSign, Roll * NotSign);

	return FTransform(Rotation, InitialUpperArmTranslation, FVector::OneVector).Inverse();
}

/** The purpose of this function, is to make sure the UpperArm_l and UpperArm_r locations 
* are used in the coming calculations to ensure they reach right angle.
* They do that by setting LeftUpperArmTransformS and RightUpperArmTransformS.
**/
void FAnimNode_UBIKSolver::ResetUpperArmsLocation()
{
	if (MeshComponent && MeshComponent->SkeletalMesh)
	{
		FTransform LeftUpperArm = MeshComponent->GetSocketTransform(LeftUpperArmBoneToModify.BoneName, RTS_World);
		LeftUpperArmTransformS = LeftUpperArm * ShoulderTransform;
		//UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftUpperArmTransformS %s"), *LeftUpperArmTransformS.ToString());

		FTransform RightUpperArm = MeshComponent->GetSocketTransform(RightUpperArmBoneToModify.BoneName, RTS_World);
		RightUpperArmTransformS = RightUpperArm * ShoulderTransform;
		//UE_LOG(UBIKRuntimeLog, Display, TEXT("RightUpperArmTransformS %s"), *RightUpperArmTransformS.ToString());
	}
}

void FAnimNode_UBIKSolver::SolveArms()
{
	/** LEFT ARM **/
	FVector HandLoc = LeftHandTransformS.GetTranslation();
	FRotator HandRot = LeftHandTransformS.GetRotation().Rotator();
	FVector UpperArmLoc = LeftUpperArmTransformS.GetTranslation();

	FTransform UpperArmBase = FTransform::Identity;
	FTransform LowerArmBase = FTransform::Identity;
	
	SetElbowBasePosition(UpperArmLoc, HandLoc, true, UpperArmBase, LowerArmBase);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LEFT UpperArmBase: %s - LowerArmBase: %s "), *UpperArmBase.ToString(), *LowerArmBase.ToString());

	float BaseAngle = RotateElbowByHandPosition(HandLoc);

	FTransform UpperArm = FTransform::Identity;
	FTransform LowerArm = FTransform::Identity;

	RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, UpperArm, LowerArm);

	float SafeAngle = UUBIK::SafeguardAngle(RotateElbowByHandRotation(LowerArm, HandRot), LeftElbowHandAngle, 120.f);

	LeftElbowHandAngle = FMath::FInterpTo(LeftElbowHandAngle, SafeAngle, DeltaTime, Settings.ElbowHandsRotSpeed);

	RotateElbow(LeftElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, LeftUpperArmTransformS, LeftLowerArmTransformS);
	LeftUpperArmTransformW = LeftUpperArmTransformS * ShoulderTransformW;
	LeftLowerArmTransformW = LeftLowerArmTransformS * ShoulderTransformW;

	/** RIGHT ARM **/
	HandLoc = RightHandTransformS.GetTranslation();
	HandRot = RightHandTransformS.GetRotation().Rotator();
	UpperArmLoc = RightUpperArmTransformS.GetTranslation();

	UpperArmBase = FTransform::Identity;
	LowerArmBase = FTransform::Identity;

	SetElbowBasePosition(UpperArmLoc, HandLoc, false, UpperArmBase, LowerArmBase);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RIGHT UpperArmBase: %s - LowerArmBase: %s "), *UpperArmBase.ToString(), *LowerArmBase.ToString());

	BaseAngle = RotateElbowByHandPosition(HandLoc);

	UpperArm = FTransform::Identity;
	LowerArm = FTransform::Identity;

	RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, UpperArm, LowerArm);

	SafeAngle = UUBIK::SafeguardAngle(RotateElbowByHandRotation(LowerArm, HandRot), RightElbowHandAngle, 120.f);

	RightElbowHandAngle = FMath::FInterpTo(RightElbowHandAngle, SafeAngle, DeltaTime, Settings.ElbowHandsRotSpeed);

	RotateElbow(RightElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, RightUpperArmTransformS, RightLowerArmTransformS);
	RightUpperArmTransformW = RightUpperArmTransformS * ShoulderTransformW;
	RightLowerArmTransformW = RightLowerArmTransformS * ShoulderTransformW;
}

void FAnimNode_UBIKSolver::SetElbowBasePosition(FVector UpperArm, FVector Hand, bool IsLeftArm, FTransform& UpperArmTransform, FTransform& LowerArmTransform)
{
	float UpperArmToHandLen = (UpperArm - Hand).Size();
	
	float Sign = (IsLeftArm) ? (-1.f) : (1.f);

	float Beta = UUBIK::CosineRule(Settings.UpperArmLength, UpperArmToHandLen, Settings.LowerArmLength) * Sign;
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("Beta: %f"), Beta);

	float Omega = UUBIK::CosineRule(Settings.LowerArmLength, Settings.UpperArmLength, UpperArmToHandLen);
	Omega = (IsLeftArm) ? (180.f - Omega) : (180.f + Omega);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("Omega: %f"), Omega);

	FQuat A = FQuat::FindBetweenNormals(FVector::ForwardVector, (Hand - UpperArm).GetSafeNormal());
	FQuat B = FQuat(FVector::UpVector, FMath::DegreesToRadians(Beta));

	UpperArmTransform = FTransform((A * B), UpperArm, FVector::OneVector);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("UpperArmTransform: %s"), *UpperArmTransform.ToString());

	FTransform TempLowerArm = FTransform(FRotator(0.f, Omega, 0.f), FVector::ForwardVector * Settings.UpperArmLength, FVector::OneVector);
	LowerArmTransform = TempLowerArm * UpperArmTransform;
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LowerArmTransform: %s"), *LowerArmTransform.ToString());
}

float FAnimNode_UBIKSolver::RotateElbowByHandPosition(FVector Hand)
{
	// TODO
	return 0.0f;
}

float FAnimNode_UBIKSolver::RotateElbowByHandRotation(FTransform LowerArm, FRotator Hand)
{
	// TODO
	return 0.0f;
}

void FAnimNode_UBIKSolver::RotateElbow(float Angle, FTransform UpperArm, FTransform LowerArm, FVector HandLoc, bool IsLeftArm, FTransform& NewUpperArm, FTransform& NewLowerArm)
{
	// TODO
}


