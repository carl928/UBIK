// 2020 Sticky Snout Studio (Jonas Molgaard)

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
	check(OutBoneTransforms.Num() == 0);

	//UE_LOG(UBIKRuntimeLog, Display, TEXT("DeltaTime: %f"), DeltaTime);
	ComponentSpaceW = Output.AnimInstanceProxy->GetComponentTransform();
	ComponentSpace = ComponentSpaceW.Inverse();

	MeshComponent = Output.AnimInstanceProxy->GetSkelMeshComponent();
	World = MeshComponent->GetWorld();

	ConvertTransforms();
	
	SetShoulder();
	SetLeftUpperArm();
	SetRightUpperArm();

	ResetUpperArmsLocation();
	SolveArms();
	
	BaseCharTransformC = GetBaseCharTransform();

	if (bDrawDebug)
	{
		DrawDebug();
	}
	
	// Stuff from the AnimBP
	Head = HeadTransformC.Rotator();
	Spine03 = FMath::RInterpTo(Spine03, BaseCharTransformC.Rotator(), DeltaTime, Settings.BodyInterSpeed);
	Spine02 = FMath::RInterpTo(Spine02, Spine03, DeltaTime, Settings.BodyInterSpeed);
	Spine01 = FMath::RInterpTo(Spine01, Spine02, DeltaTime, Settings.BodyInterSpeed);
	Pelvis = FTransform(Spine01, BaseCharTransformC.GetTranslation(), FVector::OneVector);

	Clavicle_l = LeftClavicleC.Rotator();
	UpperArm_l = LeftUpperArmTransformC.Rotator();
	LowerArm_l = LeftLowerArmTransformC.Rotator();
	Hand_l = FRotator(LeftHandTransformC.Rotator().Quaternion() * FRotator(0.f, -25.f, 180.f).Quaternion());

	Clavicle_r = FRotator(RightClavicleC.Rotator().Quaternion() * FRotator(180.f, 0.f, 0.f).Quaternion());
	UpperArm_r = FRotator(RightUpperArmTransformC.Rotator().Quaternion() * FRotator(0.f, 180.f, 180.f).Quaternion());
	//UE_LOG(LogUBIKRuntime, Display, TEXT("UpperArm_r: %s"), *UpperArm_r.ToString());
	LowerArm_r = FRotator(RightLowerArmTransformC.Rotator().Quaternion() * FRotator(0.f, 180.f, 180.f).Quaternion());
	Hand_r = FRotator(LeftHandTransformC.Rotator().Quaternion() * FRotator(180.f, 25.f, 180.f).Quaternion());
	//UE_LOG(LogUBIKRuntime, Display, TEXT("Clavicle_r: %s"), *Clavicle_r.ToString());

	//FCompactPoseBoneIndex CompactPoseBoneToModify = BoneToModify.GetCompactPoseIndex(BoneContainer);
	//FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
	//FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
	// UE_LOG(LogUBIKRuntime, Display, TEXT("BoneContainer.GetNumBones : %i"), BoneContainer.GetNumBones()); // Returns 68

	/// HEAD BONE

	FBoneReference BoneToModify = HeadBoneToModify;

	// THE FOLLWOING CODE CAN BE EXTRACTED TO AN OWN FUNCTION ModifyBoneAndAddToOutput

	FCompactPoseBoneIndex CompactPoseBoneToModify = BoneToModify.GetCompactPoseIndex(BoneContainer);
	//UE_LOG(LogUBIKRuntime, Display, TEXT("CompactPoseBoneToModify : %i"), CompactPoseBoneToModify.GetInt()); // Returns 48

	FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	// Convert to Bone Space.
	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, BCS_ComponentSpace);

	// Transform the head by applying the rotation - we are completely replacing the existing one
	NewBoneTM.SetRotation(Head.Quaternion());
	
	// Transform the head by applying the translation (if any) - we are completely replacing the existing one 
	//NewBoneTM.SetTranslation(...)
	
	// Convert back to Component Space.
	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, BCS_ComponentSpace);

	// Add transform to the output
	OutBoneTransforms.Add(FBoneTransform(BoneToModify.GetCompactPoseIndex(BoneContainer), NewBoneTM));


	/// GO ON WITH THE REST OF THE TRANSFORMS - IS THE SAMNE


	/*
	int32 HeadBoneIndex = BoneContainer.GetPoseBoneIndexForBoneName(HeadBoneToModify.BoneName);

	if (HeadBoneIndex != INDEX_NONE)
	{
		OutBoneTransforms.Add(FBoneTransform(CompactPoseBoneToModify, HeadTransformC));
	}
	UE_LOG(LogUBIKRuntime, Display, TEXT("CompactPoseBoneToModify: %i"), CompactPoseBoneToModify.GetInt());
*/
}

bool FAnimNode_UBIKSolver::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	//UE_LOG(LogUBIKRuntime, Display, TEXT("RequiredBones: i%"), RequiredBones.GetNumBones());
	return true;
}

void FAnimNode_UBIKSolver::UpdateInternal(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateInternal(Context);

	DeltaTime = Context.GetDeltaTime();
}

void FAnimNode_UBIKSolver::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	HeadBoneToModify.Initialize(RequiredBones);
	LeftClavicleBoneToModify.Initialize(RequiredBones);
	RightClavicleBoneToModify.Initialize(RequiredBones);
	LeftUpperArmBoneToModify.Initialize(RequiredBones);
	RightUpperArmBoneToModify.Initialize(RequiredBones);
	LeftLowerArmBoneToModify.Initialize(RequiredBones);
	RightLowerArmBoneToModify.Initialize(RequiredBones);
	LeftHandBoneToModify.Initialize(RequiredBones);
	RightHandBoneToModify.Initialize(RequiredBones);
	Spine01_BoneToModify.Initialize(RequiredBones);
	Spine02_BoneToModify.Initialize(RequiredBones);
	Spine03_BoneToModify.Initialize(RequiredBones);
	PelvisBoneToModify.Initialize(RequiredBones);
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
	FTransform UpperArm = FTransform::Identity;
	FTransform LowerArm = FTransform::Identity;

	SetElbowBasePosition(UpperArmLoc, HandLoc, true, UpperArmBase, LowerArmBase);

	float BaseAngle = RotateElbowByHandPosition(HandLoc, true);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("BaseAngle: %f"), BaseAngle);

	RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, UpperArm, LowerArm);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LowerArm: %s"), *LowerArm.ToString());

	float Angle = RotateElbowByHandRotation(LowerArm, HandRot);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("Angle: %f"), Angle);

	float SafeAngle = UUBIK::SafeguardAngle(LeftElbowHandAngle, Angle, 120.f);

	LeftElbowHandAngle = FMath::FInterpTo(LeftElbowHandAngle, SafeAngle, DeltaTime, Settings.ElbowHandsRotSpeed);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("LeftElbowHandAngle: %f"), LeftElbowHandAngle);

	RotateElbow(LeftElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, LeftUpperArmTransformS, LeftLowerArmTransformS);
	LeftUpperArmTransformW = LeftUpperArmTransformS * ShoulderTransformW;
	LeftLowerArmTransformW = LeftLowerArmTransformS * ShoulderTransformW;

	float Roll = FMath::Max((LeftHandTransformW.GetTranslation() - LeftUpperArmTransformW.GetTranslation()).Z, 0.f);
	FTransform UpperArmS = LeftUpperArmTransformW * ComponentSpace;
	LeftUpperArmTransformC = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(0.f, 0.f, Roll), UpperArmS.Rotator()));
	LeftLowerArmTransformC = LeftLowerArmTransformW * ComponentSpace;

	/** RIGHT ARM **/
	HandLoc = RightHandTransformS.GetTranslation();
	HandRot = RightHandTransformS.GetRotation().Rotator();
	UpperArmLoc = RightUpperArmTransformS.GetTranslation();

	SetElbowBasePosition(UpperArmLoc, HandLoc, false, UpperArmBase, LowerArmBase);

	BaseAngle = RotateElbowByHandPosition(HandLoc, false);

	UpperArm = FTransform::Identity;
	LowerArm = FTransform::Identity;

	RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, UpperArm, LowerArm);

	Angle = RotateElbowByHandRotation(LowerArm, HandRot);

	SafeAngle = UUBIK::SafeguardAngle(RightElbowHandAngle, Angle, 120.f);

	RightElbowHandAngle = FMath::FInterpTo(RightElbowHandAngle, SafeAngle, DeltaTime, Settings.ElbowHandsRotSpeed);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("RightElbowHandAngle: %f"), RightElbowHandAngle);

	RotateElbow(RightElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, RightUpperArmTransformS, RightLowerArmTransformS);
	RightUpperArmTransformW = RightUpperArmTransformS * ShoulderTransformW;
	RightLowerArmTransformW = RightLowerArmTransformS * ShoulderTransformW;

	Roll = FMath::Max((RightHandTransformW.GetTranslation() - RightUpperArmTransformW.GetTranslation()).Z, 0.f);
	UpperArmS = RightUpperArmTransformW * ComponentSpace;
	RightUpperArmTransformC = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(0.f, 0.f, Roll), UpperArmS.Rotator()));
	RightLowerArmTransformC = RightLowerArmTransformW * ComponentSpace;
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

float FAnimNode_UBIKSolver::RotateElbowByHandPosition(FVector Hand, bool IsLeftArm)
{
	FVector TempHand = Hand / Settings.ArmLength;

	float Sign = (IsLeftArm) ? (1.f) : (-1.f);

	float Angle = Settings.ElbowBaseOffsetAngle +
		(Settings.ElbowYWeight * FMath::Max(0.f, TempHand.Y * Sign * Settings.ElbowYDistanceStart));

	return Angle;
}

float FAnimNode_UBIKSolver::RotateElbowByHandRotation(FTransform LowerArm, FRotator Hand)
{
	FRotator LowerArmRot = LowerArm.Rotator();

	FVector LowerArmRight = UKismetMathLibrary::GetRightVector(LowerArmRot);
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("GetRightVector:: %s"), *LowerArmRight.ToString());

	FVector ProjectedForward = UKismetMathLibrary::ProjectVectorOnToPlane(
		UKismetMathLibrary::GetForwardVector(Hand), LowerArmRight
	).GetSafeNormal();
	//UE_LOG(UBIKRuntimeLog, Display, TEXT("ProjectedForward:: %s"), *ProjectedForward.ToString());

	FVector LowerArmForward = UKismetMathLibrary::GetForwardVector(LowerArmRot);

	float TempAngle = UUBIK::CosineRule(1.f, 1.f, (ProjectedForward - LowerArmForward).Size());

	float Sign = (FVector::DotProduct(FVector::CrossProduct(ProjectedForward, LowerArmForward), LowerArmRight) < 0.f) ? (1.f) : (-1.f);
	TempAngle = TempAngle * Sign;
	TempAngle = TempAngle * Settings.ElbowRotFromHandRotAlpha;

	return TempAngle;
}

void FAnimNode_UBIKSolver::RotateElbow(float Angle, FTransform UpperArm, FTransform LowerArm, FVector HandLoc, bool IsLeftArm, FTransform& NewUpperArm, FTransform& NewLowerArm)
{
	FVector PivotLoc =
		UpperArm.GetTranslation() +
		UKismetMathLibrary::ProjectVectorOnToVector(UpperArm.GetTranslation() - LowerArm.GetTranslation(), UpperArm.GetTranslation() - HandLoc);

	FVector Forward = UpperArm.GetTranslation() - HandLoc;
	FVector Right = FVector::CrossProduct(UKismetMathLibrary::GetUpVector(UpperArm.Rotator()), Forward);
	FVector Up = UKismetMathLibrary::GetUpVector(UpperArm.Rotator());

	FRotator PivotRot = UKismetMathLibrary::MakeRotationFromAxes(Forward, Right, Up);

	float Roll = (IsLeftArm) ? (180.f - Angle) : (180.f + Angle);
	FRotator Delta = FRotator(0.f, 0.f, Roll);

	FTransform Pivot = FTransform(PivotRot, PivotLoc, FVector::OneVector);

	NewUpperArm = UUBIK::RotatePointAroundPivot(UpperArm, Pivot, Delta);
	NewLowerArm = UUBIK::RotatePointAroundPivot(LowerArm, Pivot, Delta);
}

FTransform FAnimNode_UBIKSolver::GetBaseCharTransform()
{
	return FTransform(ShoulderTransformC.GetRotation(), ShoulderTransformC.GetTranslation() + Settings.BaseCharOffset, FVector::OneVector);
}

void FAnimNode_UBIKSolver::DrawDebug()
{
	DrawAxes(LeftHandTransformW, true);
	DrawAxes(RightHandTransformW, true);
	//DrawDebugLine(GetWorld(), WorldTransform.GetLocation(), WorldTransform.GetLocation() + (WorldRot.GetUpVector() * 10.f), FColor::Blue);

}

void FAnimNode_UBIKSolver::DrawAxes(FTransform Transform, bool bDrawAxes)
{
	DrawDebugSphere(World, Transform.GetTranslation(), 3.0f, 20.f, FColor::Silver);

	if (bDrawAxes)
	{
		FQuat WorldRot = Transform.GetRotation();
		DrawDebugLine(World, Transform.GetLocation(), Transform.GetLocation() + (WorldRot.GetUpVector() * 10.f), FColor::Blue);
		DrawDebugLine(World, Transform.GetLocation(), Transform.GetLocation() + (WorldRot.GetForwardVector() * 10.f), FColor::Red);
		DrawDebugLine(World, Transform.GetLocation(), Transform.GetLocation() + (WorldRot.GetRightVector() * 10.f), FColor::Green);
	}
}

