// Copyright 2024 XpycT. All Rights Reserved.


#include "ECCameraBlockingVolume.h"

AECCameraBlockingVolume::AECCameraBlockingVolume()
{
	Tags.Add("EnhancedCamera#CameraBlocking");
    
	if (UPrimitiveComponent* PrimitiveComponent = this->FindComponentByClass<UPrimitiveComponent>())
	{
		PrimitiveComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName, false);
	}
}
