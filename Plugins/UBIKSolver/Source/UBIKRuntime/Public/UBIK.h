// 2020 Sticky Snout Studio (Jonas Molgaard)

#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "UBIK.generated.h"

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIKSettings
{
	GENERATED_USTRUCT_BODY()

public:
	/* Defaults */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (ToolTip = "Rotation multiplier, that determine how much Shoulder rotates to try reach hand, when it is moving away from ShoulderJoint. Usally keep between (0-70). 0 means the shoulder doesnt rotate, where 60 is a quite extreme shoulder rotation."))
	float DistinctShoulderRotationMultiplier = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float DistinctShoulderRotationLimit = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ClavicleOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowBaseOffsetAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowYDistanceStart = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowYWeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowHandsRotSpeed = 0.f;

	/** How much influence the hands rotation will have on elbow rotation. Set between 0-1. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowRotFromHandRotAlpha = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ArmLength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float UpperArmsDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float LowerArmLength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float UpperArmLength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float HeadHandAngleLimit = 0.f;

	/* Calculated based on the HeadHandAngleLimit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (ToolTip = ""))
	float HeadHandAngleLimitDot = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float OkSpanAngle = 0.f;

	/** Offset from Shoulder to Pelvis **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FVector BaseCharOffset = FVector::ZeroVector;

	/** Interp speed between each bone from head to Pelvis-chain. Higher values give a more stiff upperbody that rotate more 1:1 with head. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float BodyInterSpeed = 0.f;
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

	/** How much influence the hands rotation will have on elbow rotation. Set between 0-1. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ElbowRotFromHandRotAlpha = 0.6f;

	/** I may remove this setting actually, and just hardcode **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float HeadHandAngleLimit = 150.f;

	/** I may remove this setting actually, and just hardcode **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float OkSpanAngle = 80.f;

	/** Offset from Shoulder to Pelvis **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FVector BaseCharOffset = FVector(0.f, 0.f, -55.25f);

	/** Interp speed between each bone from head to Pelvis-chain. Higher values give a more stiff upperbody that rotate more 1:1 with head. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float BodyInterSpeed = 10.f;
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

	/** Call this to get the UBIKSettings struct, that is needed on the AnimGraph node. **/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetUBIKSettings", Keywords = "UBIK Calibrate"), Category = "UBIK")
	static FUBIKSettings Initialize(FUBIKDefaults Defaults, FUBIKCalibrationData Calibration);

	/** Adds Vector to Transform in LocalSpace. **/
	static FTransform AddLocalOffset(const FTransform Transform, const FVector Vector);

	/** Rotates the Point around Pivot, by given Delta rotation. **/
	static FTransform RotatePointAroundPivot(FTransform Point, FTransform Pivot, FRotator Delta);

	/** Angle snapping mitigation function.
	* Supply last angle, current angle and what threshold not to pass.
	* If outside that limit, it will keep the last angle, otherwise the current. **/
	static float SafeguardAngle(float Last, float Current, float Threshold);

	/** Given lengths of the different sides in a triangle,
	* the angle between Adjacent1 and Adjacent2 are returned. **/
	static float CosineRule(float Adjacent1, float Adjacent2, float Opposite);
};
