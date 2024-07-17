/* Copyright (c) 2021-2024 by Inoland */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FInoScreenCaptureModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
