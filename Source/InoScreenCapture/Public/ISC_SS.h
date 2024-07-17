/* Copyright (c) 2021-2024 by Inoland */

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ScreenCapture.h"

#include "ISC_Utils.h"

#include "ISC_SS.generated.h"

class UGameInstanceSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FScreenCapture, const FISC_MonitorInfo&, MonitorInfo, UTexture2D*, Texture2D);

UCLASS(Blueprintable, DisplayName= "Ino Screen Capture Subsystem", Category = "Ino Screen Capture")
class INOSCREENCAPTURE_API UInoScreenCaptureSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UInoScreenCaptureSubSystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
private:
	std::shared_ptr<SL::Screen_Capture::IScreenCaptureManager> SCLManager;

	UPROPERTY(Transient, DuplicateTransient)
	TMap<int, TObjectPtr<UTexture2D>> RenderedTextures;
	
	void SetupScreenCapture(int tick, TArray<int> monitors, bool CleanRenders, bool CleanCaptureCore);

	void ScreenCaptured(const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor);

	void InitTexture(const FISC_MonitorInfo& MInfo);
	
	void HandleTexture(TArray<FColor> Pixels, FISC_ImageInfo IInfo, const FISC_MonitorInfo& MInfo);

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Ino Screen Capture")
	TMap<FISC_MonitorInfo, UTexture2D*> MonitorsRefMap;

	UPROPERTY(BlueprintAssignable, Category = "Ino Screen Capture")
	FScreenCapture MonitorCaptureUpdateCallback;

	UPROPERTY(BlueprintAssignable, Category = "Ino Screen Capture")
	FScreenCapture MonitorCaptureInitCallback;
	
	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	void InitScreenCapture(int Tick, TArray<int> MonitorIDs, bool CleanRenders, bool CleanCaptureCore);

	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	void ResetScreenCapture(bool CleanRenders, bool CleanCaptureCore);
	
	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	bool GetStatus();

	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	bool ControlStatus(bool Enable);
	
	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	TMap<int,FISC_MonitorInfo> GetMonitors();
	
	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	FISC_MonitorInfo GetMonitorInfo(int ID);
	
	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	bool SetFrameChangeInterval(int Frame, int Mouse);
	
	UFUNCTION(BlueprintCallable, Category = "Ino Screen Capture")
	UTexture2D* GetCapturedScreen(const int MonitorID);
};
