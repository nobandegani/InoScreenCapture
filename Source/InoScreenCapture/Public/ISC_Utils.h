/* Copyright (c) 2021-2024 by Inoland */

#pragma once

#include "ISC_Utils.generated.h"


USTRUCT(BlueprintType, Category = "ISC", meta = (DisplayName = "Ino Monitor Info"))
struct FISC_MonitorInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|MonitorInfo")
	int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|MonitorInfo")
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|MonitorInfo")
	int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|MonitorInfo")
	int32 Height;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|MonitorInfo")
	FString Name;

	FISC_MonitorInfo()
		: Index(-1),
		  ID(-1),
		Width(-1),
		Height(-1),
		Name(TEXT("N"))
	{
	}
	FISC_MonitorInfo(int inIndex, int inID, int inWidth, int inHeight, FString inName)
		: Index(inIndex),
		  ID(inID),
		Width(inWidth),
		Height(inHeight),
		Name(inName)
	{
	}

	//---------------Custom----------------------------------
	
	// Hash function
	friend uint32 GetTypeHash(const FISC_MonitorInfo& k)
	{
		// This combines the X and Y hash values.
		return FCrc::MemCrc32(&k, sizeof(FISC_MonitorInfo));
	}

	friend bool operator==(const FISC_MonitorInfo& lhs, const FISC_MonitorInfo& rhs)
	{
		return lhs.Index == rhs.Index && 
			   lhs.ID == rhs.ID &&
			   lhs.Width == rhs.Width &&
			   lhs.Height == rhs.Height &&
			   lhs.Name == rhs.Name;
	}
	
	FString ToString() const
	{
		return FString::Printf(TEXT("index: %d --id: %d-- width: %d --height: %d"), Index, ID, Width, Height);
	}

	
};

USTRUCT(BlueprintType, Category = "ISC", meta = (DisplayName = "Ino Image Info"))
struct FISC_ImageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|ImageInfo")
	int32 Left;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|ImageInfo")
	int32 Top;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|ImageInfo")
	int32 Bottom;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|ImageInfo")
	int32 Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|ImageInfo")
	int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|ImageInfo")
	int32 Height;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ISC|ImageInfo")
	int32 Area;

	FISC_ImageInfo()
		: Left(-1),
		  Top(-1),
		Bottom(-1),
		Right(-1),
		Width(-1),
		Height(-1),
		Area(-1)
	{
	}
	
	FISC_ImageInfo(int inLeft, int inTop, int inBottom, int inRight)
		: Left(inLeft),
		  Top(inTop),
		Bottom(inBottom),
		Right(inRight),
		Width(Right - Left),
		Height(Bottom - Top),
		Area(Width * Height)
	{
	}
};