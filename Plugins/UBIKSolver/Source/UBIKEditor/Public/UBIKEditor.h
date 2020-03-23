// 2020 Sticky Snout Studio (Jonas Mølgaard)

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUBIKEditor : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
