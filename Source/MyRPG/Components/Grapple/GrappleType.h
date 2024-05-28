#pragma once

UENUM()
enum class EGrappleType :uint8
{
	EGT_Inactive,
	EGT_Firing,
	EGT_NearTarget,
	EGT_Waiting
};