#pragma once

#include "DrawDebugHelpers.h"
#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(),Location,25.f,24,FColor::Blue,false,30);
#define DRAW_SPHERE_SingleFrame(Location) if(GetWorld()) DrawDebugSphere(GetWorld(),Location,25.f,24,FColor::Blue,false,-1.f,0,1.f);
#define DRAW_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(),Location,150,FColor::Blue,false,30.F);
#define DRAW_POINT_SingleFrame(Location) if(GetWorld()) DrawDebugPoint(GetWorld(),Location,150,FColor::Blue,false,-1.f);
#define DRAW_LINE(StartLocation,EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation,EndLocation , FColor::Red, false, 30.f, 0,1.f);
#define DRAW_LINE_SingleFrame(StartLocation,EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation,EndLocation , FColor::Red, false, -1.f, 0,1.f);
#define DRAW_VECTOR(StartLocation,EndLocation) if(GetWorld())\
{\
DrawDebugLine(GetWorld(), StartLocation,EndLocation , FColor::Red, false, 30.f, 0,1.f);\
DrawDebugPoint(GetWorld(),EndLocation,150,FColor::Blue,false,30.F);\
};
#define DRAW_VECTOR_SingleFrame(StartLocation,EndLocation) if(GetWorld())\
{\
DrawDebugLine(GetWorld(), StartLocation,EndLocation , FColor::Red, false, -1.f, 0,1.f);\
DrawDebugPoint(GetWorld(),EndLocation,150,FColor::Blue,false,-1.f);\
};
#define DRAW_RECTANGLE(Location,ExtentLocation) if(GetWorld())\
{\
DrawDebugBox(GetWorld(),Location,ExtentLocation,FColor::Black,false,30.f,10.f);\
};