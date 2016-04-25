//@file ref_ViolaJonesDetector.c
//@brief Contains implementation of Viola Jones detector
//@author Andrey Belyakov
//@date 25 April 2016


#include "../ref.h"

vx_status ref_ViolaJonesDetector(const vx_image src_image,
	vx_image dst_image)
{
	const uint32_t src_width = src_image->width;
	const uint32_t src_height = src_image->height;
	const uint32_t dst_width = dst_image->width;
	const uint32_t dst_height = dst_image->height;

	if (src_width != dst_width || src_height != dst_height)
	{
		return VX_ERROR_INVALID_PARAMETERS;
	}

	//const uint8_t* src_data = src_image->data;
	//uint8_t* dst_data = dst_image->data;

	for (uint32_t ind = 0; ind <= src_width * src_height; ++ind)
	{
	}

	return VX_SUCCESS;
};