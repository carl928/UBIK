// 2020 Sticky Snout Studio (Jonas M�lgaard)

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
