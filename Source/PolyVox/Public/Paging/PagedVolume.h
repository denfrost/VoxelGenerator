// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseVolume.h"
#include "Pager.h"
#include "PagedChunk.h"
#include "PagedVolume.generated.h"

/**
 * 
 */
 UCLASS(Blueprintable)
class POLYVOX_API APagedVolume : public ABaseVolume
{
	GENERATED_BODY()
public:
	APagedVolume();
	~APagedVolume();

	UFUNCTION(BlueprintCallable, Category = "Volume")
	// 256 * 1024 * 1024 = 268435456
	void InitializeVolume(TSubclassOf<UPager> VolumePager, int32 TargetMemoryUsageInBytes = 268435456, uint8 VolumeChunkSideLength = 32);

	UFUNCTION(BlueprintPure, Category = "Volume|Voxels")
	virtual FVoxel GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos) override;
	UFUNCTION(BlueprintPure, Category = "Volume|Voxels")
	virtual FVoxel GetVoxelByVector(const FVector& Coordinates) override;

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	virtual void SetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos, const FVoxel& Voxel) override;
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	virtual void SetVoxelByVector(const FVector& Coordinates, const FVoxel& Voxel) override;

	/// Tries to ensure that the voxels within the specified Region are loaded into memory.
	UFUNCTION(BlueprintCallable, Category = "Volume|Utility")
	void Prefetch(FRegion PrefetchRegion);
	/// Removes all voxels from memory
	UFUNCTION(BlueprintCallable, Category = "Volume|Utility")
	void FlushAll();

	UFUNCTION(BlueprintPure, Category = "Volume|Utility")
	virtual int32 CalculateSizeInBytes() const override;

	virtual uint8 GetSideLengthPower() const override;
	bool CanReuseLastAccessedChunk(int32 iChunkX, int32 iChunkY, int32 iChunkZ) const;
	UPagedChunk* GetChunk(int32 uChunkX, int32 uChunkY, int32 uChunkZ);
	UPROPERTY()
	UPagedChunk* LastAccessedChunk = nullptr;

private:
	UPROPERTY()
	int32 LastAccessedChunkX = 0;
	UPROPERTY()
	int32 LastAccessedChunkY = 0;
	UPROPERTY()
	int32 LastAccessedChunkZ = 0;

	UPROPERTY()
	int32 Timestamper = 0;

	UPROPERTY()
	int32 ChunkCountLimit = 0;

	// Chunks are stored in the following array which is used as a hash-table. Conventional wisdom is that such a hash-table
	// should not be more than half full to avoid conflicts, and a practical chunk size seems to be 64^3. With this configuration
	// there can be up to 32768*64^3 = 8 gigavoxels (with each voxel perhaps being many bytes). This should effectively make use 
	// of even high end machines. Of course, the user can choose to limit the memory usage in which case much less of the chunk 
	// array will actually be used. None-the-less, we have chosen to use a fixed size array (rather than a vector) as it appears to 
	// be slightly faster (probably due to the extra pointer indirection in a vector?) and the actual size of this array should
	// just be 1Mb or so.
	static const uint32 CHUNK_ARRAY_SIZE = 65536;
	UPROPERTY()
	TArray<UPagedChunk*> ArrayChunks;

	// The size of the chunks
	UPROPERTY()
	uint8 ChunkSideLength;
	UPROPERTY()
	uint8 ChunkSideLengthPower;
	UPROPERTY()
	int32 ChunkMask;

	UPROPERTY()
	UPager* Pager = nullptr;
};