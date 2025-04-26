#include "OTPortal.h"
#include "OneTwoBreakFree/Character/OTCitizenCharacter.h"
#include "Components/SphereComponent.h"

AOTPortal::AOTPortal()
{
	PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
    RootComponent = TriggerSphere;
    TriggerSphere->SetSphereRadius(100.0f);

    TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    TriggerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AOTPortal::OnOverlapBegin);
}

void AOTPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AOTCitizenCharacter* CitizenCharacter = Cast<AOTCitizenCharacter>(OtherActor))
    {
        CitizenCharacter->HandleEscape();
    }
}

