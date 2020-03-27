// 2020 Sticky Snout Studio (Jonas Mølgaard)

#include "UBIK.h"

FUBIKSettings UUBIK::Initialize(FUBIKDefaults Defaults, FUBIKCalibrationData Calibration)
{
	const float LowerArmRatio = 0.48f;

	FUBIKSettings Settings;

	Settings.ArmLength = (Calibration.Height / 2) - Calibration.UpperArmsDistance;
	Settings.LowerArmLength = Settings.ArmLength * LowerArmRatio;
	Settings.UpperArmLength = Settings.ArmLength * (1 - LowerArmRatio);
	Settings.HeadHandAngleLimit = Defaults.HeadHandAngleLimit;
	Settings.HeadHandAngleLimitDot = FMath::Acos(Defaults.HeadHandAngleLimit);

	Settings.ClavicleOffset = Defaults.ClavicleOffset;
	Settings.DistinctShoulderRotationLimit = Defaults.DistinctShoulderRotationLimit;
	Settings.DistinctShoulderRotationMultiplier = Defaults.DistinctShoulderRotationMultiplier;
	Settings.ElbowBaseOffsetAngle = Defaults.ElbowBaseOffsetAngle;
	Settings.ElbowHandsRotSpeed = Defaults.ElbowHandsRotSpeed;
	Settings.ElbowYDistanceStart = Defaults.ElbowYDistanceStart;
	Settings.ElbowYWeight = Defaults.ElbowYWeight;
	Settings.UpperArmsDistance = Calibration.UpperArmsDistance;

	return Settings;
}

FTransform UUBIK::AddLocalOffset(const FTransform Transform, const FVector Vector)
{
	FTransform Inversed = Transform.Inverse();
	return FTransform(
		Inversed.GetRotation(),
		Inversed.GetTranslation() + Vector,
		Inversed.GetScale3D()
	).Inverse();
}

FTransform UUBIK::RotatePointAroundPivot(FTransform Point, FTransform Pivot, FRotator Delta)
{
	FTransform PointInPivotSpace = Point * Pivot.Inverse();
	FTransform RotatedInPivotSpace = PointInPivotSpace * FTransform(Delta, FVector::ZeroVector, FVector::OneVector);
	return RotatedInPivotSpace * Pivot;
}
