#pragma once

UENUM()
enum class EAnnouncementType : uint8
{
	EANMT_MatchStart,
	EANMT_Survive,
	EANMT_Kill,
	EANMT_Dead,
	EANMT_MatchEnd,

	EANMT_Max,
};