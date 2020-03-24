// 2020 Sticky Snout Studio (Jonas Mølgaard)

#pragma once

#include "UBIK.generated.h"

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIKSettings
{
	GENERATED_USTRUCT_BODY()

public:
	/* Defaults */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault, ToolTip = "Rotation multiplier, that determine how much Shoulder rotates to try reach hand, when it is moving away from ShoulderJoint. Usally keep between (0-70). 0 means the shoulder doesnt rotate, where 60 is a quite extreme shoulder rotation."))
	float DistinctShoulderRotationMultiplier = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float DistinctShoulderRotationLimit = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float ClavicleOffset = -32.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float ElbowBaseOffsetAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float ElbowYDistanceStart = .2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float ElbowYWeight = 130.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float ElbowHandsRotSpeed = 15.f;

	/* CalibrationData */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = ""))
	float ArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault, ToolTip = ""))
	float UpperArmsDistance = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = ""))
	float LowerArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = ""))
	float UpperArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = ""))
	float HeadHandAngleLimit;

	/* Calculated based on the HeadHandAngleLimit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = ""))
	float HeadHandAngleLimitDot;
};

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIKDefaults
{
	GENERATED_USTRUCT_BODY()
		
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (ToolTip = "Rotation multiplier, that determine how much Shoulder rotates to try reach hand, when it is moving away from ShoulderJoint. Usally keep between (0-70). 0 means the shoulder doesnt rotate, where 60 is a quite extreme shoulder rotation."))
	float DistinctShoulderRotationMultiplier = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float DistinctShoulderRotationLimit = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ClavicleOffset = -32.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowBaseOffsetAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowYDistanceStart = .2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowYWeight = 130.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowHandsRotSpeed = 15.f;

	// TODO: I may remove this setting actually, and just hardcode this plus the previous OkSpanAngle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = ""))
	float HeadHandAngleLimit = 150.f;
};

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIKCalibrationData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Actual Player height in cm."))
	float Height = 184.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault, ToolTip = ""))
	float UpperArmsDistance = 30.f;
};

/**
 * UBIK Initializer class
 */
UCLASS()
class UBIKRUNTIME_API UUBIK : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetUBIKSettings", Keywords = "UBIK Calibrate"), Category = "UBIK")
	static FUBIKSettings Initialize(FUBIKDefaults Defaults, FUBIKCalibrationData Calibration);
};
