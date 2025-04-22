// Fill out your copyright notice in the Description page of Project Settings.


#include "OTLoadingUI.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"


void UOTLoadingUI::HideLoadingUI()
{
    if (LoadingBoarder)
    {
        LoadingBoarder->SetVisibility(ESlateVisibility::Hidden);
    }

    if (LoadingText)
    {
        LoadingText->SetVisibility(ESlateVisibility::Hidden);
    }
}
