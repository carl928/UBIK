// 2020 Sticky Snout Studio (Jonas Mølgaard)

#include "UBIK.h"

FUBIKSettings UUBIK::Initialize(FUBIKDefaults Defaults, FUBIKCalibrationData Calibration)
{
	FUBIKSettings Settings;
	Settings.ArmLength = (Calibration.Height / 2) - Calibration.UpperArmsDistance;

	return Settings;
}
