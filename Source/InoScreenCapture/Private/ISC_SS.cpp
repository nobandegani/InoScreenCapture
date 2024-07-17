/* Copyright (c) 2021-2024 by Inoland */

#include "ISC_SS.h"

#include "Engine/Texture2D.h"

#include "RHI.h"
#include "RHICommandList.h"

#include "Async/Async.h"
#include "Async/AsyncWork.h"

UInoScreenCaptureSubSystem::UInoScreenCaptureSubSystem()
{
	SCLManager = nullptr;
}

void UInoScreenCaptureSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UInoScreenCaptureSubSystem::Deinitialize()
{
	Super::Deinitialize();

	ResetScreenCapture(true, true);
}

void UInoScreenCaptureSubSystem::SetupScreenCapture(int tick, TArray<int> monitors, bool CleanRenders, bool CleanCaptureCore)
{
	ResetScreenCapture(CleanRenders, CleanCaptureCore);
	
	SCLManager = SL::Screen_Capture::CreateCaptureConfiguration([monitors]() {
		auto CurrentMonitors = SL::Screen_Capture::GetMonitors();
	
		std::vector<SL::Screen_Capture::Monitor> SelectedMonitors;
		
		for (const auto& monitor : CurrentMonitors) {
			if (monitors.Find(monitor.Id) != -1)
			{
				SelectedMonitors.push_back(monitor);
			}
		}
		
		return SelectedMonitors;
	})->onFrameChanged([this](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor)
			{
				ScreenCaptured(img, monitor);
			})->start_capturing();
	
	/*
		->onNewFrame([this](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
			ScreenCaptured(img, monitor);
		})->start_capturing();
	
		->onFrameChanged([this](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor)
			{
				ScreenCaptured(img, monitor);
			})->start_capturing();
	*/
	SCLManager->setFrameChangeInterval(std::chrono::milliseconds(tick)); // For real-time
}

void UInoScreenCaptureSubSystem::ResetScreenCapture(bool CleanRenders, bool CleanCaptureCore)
{
	if (CleanRenders)
	{
		MonitorsRefMap.Empty();
		for (const auto& pair : RenderedTextures)
		{
			pair.Value->RemoveFromRoot();
		}
		RenderedTextures.Empty();
	}
	
	if (CleanCaptureCore)
	{
		if (SCLManager != nullptr)
		{
			SCLManager->pause();
			SCLManager.reset();
			SCLManager = nullptr;
		}
	}
}

void UInoScreenCaptureSubSystem::ScreenCaptured(const SL::Screen_Capture::Image& img,
                                                const SL::Screen_Capture::Monitor& monitor)
{
	//UE_LOG(LogTemp, Warning, TEXT("-----------------------------------------------------------------------"));
	
	FISC_MonitorInfo MInfo(monitor.Index, monitor.Id, monitor.Width, monitor.Height, monitor.Name);
	
	FISC_ImageInfo IInfo(img.Bounds.left, img.Bounds.top, img.Bounds.right, img.Bounds.bottom);
	//UE_LOG(LogTemp, Log, TEXT("img: %i:%i--%i:%i----width:%i--height:%i--area:%i"), IInfo.Left, IInfo.Top, IInfo.Right, IInfo.Bottom, IInfo.Width, IInfo.Height, IInfo.Area);
	
	int32 PixelLength;
	TArray<FColor> Pixels;

	PixelLength = IInfo.Area;
	Pixels.Reserve(PixelLength);
		
	for (int32 i = 0; i < IInfo.Height; ++i)
	{
		for (int32 j = 0; j < IInfo.Width; ++j)
		{
			int32 s = (i * MInfo.Width) + j;
			auto& p = img.Data[s];
			FColor color(p.R, p.G, p.B, 255);
			Pixels.Add(color);
		}
	}
	AsyncTask(ENamedThreads::GameThread, [this, pixels = MoveTemp(Pixels), iinfo = MoveTemp(IInfo), minfo = MoveTemp(MInfo)]()
	{
		HandleTexture(pixels, iinfo, minfo);
	});
}

void UInoScreenCaptureSubSystem::InitTexture(const FISC_MonitorInfo& MInfo)
{
	RenderedTextures.Add(MInfo.ID, UTexture2D::CreateTransient(MInfo.Width, MInfo.Height, PF_B8G8R8A8));
	RenderedTextures[MInfo.ID]->SRGB=true;
	//RenderedTextures[MInfo.ID]->MipGenSettings = TMGS_NoMipmaps;
	RenderedTextures[MInfo.ID]->NeverStream = true;
	RenderedTextures[MInfo.ID]->AddToRoot();
	RenderedTextures[MInfo.ID]->UpdateResource();

	MonitorsRefMap.Add(MInfo, RenderedTextures[MInfo.ID]);

	MonitorCaptureInitCallback.Broadcast(MInfo, RenderedTextures[MInfo.ID]);
}

void UInoScreenCaptureSubSystem::HandleTexture(TArray<FColor> Pixels, FISC_ImageInfo IInfo, const FISC_MonitorInfo& MInfo)
{
	//UE_LOG(LogTemp, Warning, TEXT("--------------Start handle texture"));
	if (!RenderedTextures.Contains(MInfo.ID))
	{
		InitTexture( MInfo );
	}
	else if (1 && (MInfo.Width != RenderedTextures[MInfo.ID]->GetSizeX() || MInfo.Height != RenderedTextures[MInfo.ID]->GetSizeY()) )
	{
		RenderedTextures[MInfo.ID]->RemoveFromRoot();
		RenderedTextures[MInfo.ID] = nullptr;
		RenderedTextures.Remove(MInfo.ID);
		InitTexture( MInfo );
	}
	
	if (RenderedTextures[MInfo.ID] == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("failed to create texture."));
		return;
	}
	
	//UE_LOG(LogTemp, Log, TEXT("Texture size: %d x %d"), RenderedTextures[MInfo.ID]->GetSizeX(), RenderedTextures[MInfo.ID]->GetSizeY());
	
	/* --------------------------------------if u want memcopy method, and only work with new frame
		FTexture2DMipMap& Mip = RenderedTextures[MInfo.ID]->GetPlatformData()->Mips[0]; //PlatformData->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		
		if (Mip.BulkData.GetBulkDataSize() == Pixels.Num() * sizeof(FColor)) 
		{
			FMemory::Memcpy(Data, Pixels.GetData(), Mip.BulkData.GetBulkDataSize());
			//FMemory::Memmove(Data, pixels.GetData(), Mip.BulkData.GetBulkDataSize());
		} 
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot copy to texture: Size mismatch."));
		}
		Mip.BulkData.Unlock();
		       
		RenderedTextures[MInfo.ID]->UpdateResource();
		UE_LOG(LogTemp, Log, TEXT("update whole frame"));
	*/
	
	UTexture2D* LocalTexture = RenderedTextures[MInfo.ID];
		
	AsyncTask(ENamedThreads::ActualRenderingThread, [LocalTexture, pixels = MoveTemp(Pixels), iinfo = MoveTemp(IInfo)]()
	{
		FUpdateTextureRegion2D UpdateRegion(iinfo.Left,iinfo.Top, 0,0, iinfo.Width, iinfo.Height);

		TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> DataCleanupFunc = [](uint8* SrcData, const FUpdateTextureRegion2D* Regions) {
			//delete[] SrcData;
			//delete[] Regions;
		};
			
		LocalTexture->UpdateTextureRegions(0,1, &UpdateRegion,iinfo.Width * 4, 4, (uint8*)pixels.GetData(), DataCleanupFunc);
	});
	
	//UE_LOG(LogTemp, Log, TEXT("update region"));
	MonitorCaptureUpdateCallback.Broadcast(MInfo, RenderedTextures[MInfo.ID]);
}

void UInoScreenCaptureSubSystem::InitScreenCapture(int Tick, TArray<int> MonitorIDs, bool CleanRenders, bool CleanCaptureCore)
{
	SetupScreenCapture(Tick, MonitorIDs, CleanRenders, CleanCaptureCore);
}

bool UInoScreenCaptureSubSystem::GetStatus()
{
	if (SCLManager == nullptr)
	{
		return false;
	}
	return !SCLManager->isPaused();
}

bool UInoScreenCaptureSubSystem::ControlStatus(bool Enable)
{
	if (SCLManager == nullptr)
	{
		return false;
	}

	if (Enable)
	{
		SCLManager->resume();
	}else
	{
		SCLManager->pause();
	}
	
	return !SCLManager->isPaused();
}

TMap<int,FISC_MonitorInfo> UInoScreenCaptureSubSystem::GetMonitors()
{
	TMap<int,FISC_MonitorInfo> temp;
	
	auto monitors = SL::Screen_Capture::GetMonitors();
	
	if (monitors.size() < 1)
	{
		return temp;
	}
	
	for (const auto& monitor : monitors) {
		temp.Add(monitor.Id, FISC_MonitorInfo(monitor.Index, monitor.Id, monitor.Width, monitor.Height, monitor.Name));
	}
	
	return temp;
}

FISC_MonitorInfo UInoScreenCaptureSubSystem::GetMonitorInfo(int ID)
{
	auto monitors = SL::Screen_Capture::GetMonitors();
	
	if (monitors.size() < 1)
	{
		return FISC_MonitorInfo();
	}
	
	return FISC_MonitorInfo(monitors[ID].Index, monitors[ID].Id, monitors[ID].Width, monitors[ID].Height, monitors[ID].Name);
}

bool UInoScreenCaptureSubSystem::SetFrameChangeInterval(int Frame, int Mouse)
{
	if (SCLManager == nullptr)
	{
		return false;
	}
	
	SCLManager->setFrameChangeInterval(std::chrono::milliseconds(Frame));
	SCLManager->setMouseChangeInterval(std::chrono::milliseconds(Mouse));
	
	return true;
}

UTexture2D* UInoScreenCaptureSubSystem::GetCapturedScreen(const int MonitorID)
{
	if (!RenderedTextures.Contains(MonitorID))
	{
		return nullptr;
	}
	
	return RenderedTextures[MonitorID];
}