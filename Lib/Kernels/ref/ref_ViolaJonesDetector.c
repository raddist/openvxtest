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

	const uint8_t* src_data = src_image->data;
	uint8_t* dst_data = dst_image->data;

	for (uint32_t ind = 0; ind < src_height; ++ind)
	for (uint32_t jnd = 0; jnd < src_width; ++jnd)
	{
		dst_data[ind * src_width + jnd] = 255 - src_data[ind * src_width + jnd];
	}

	//create integral view of image

	/*vx_image* srcVXImage = {
		src_image->data,
		src_image->width,
		src_image->height,
		VX_DF_IMAGE_U8,
		VX_COLOR_SPACE_DEFAULT
	};*/

	for (uint32_t ind = 0; ind < src_height; ++ind)
	for (uint32_t jnd = 0; jnd < src_width; ++jnd)
	{
		dst_data[ind * src_width + jnd] = 255 - src_data[ind * src_width + jnd];
	}


	return VX_SUCCESS;
};