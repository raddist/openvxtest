/*
File: ref_Threshold.c
Содержит эталонную реализацию пороговой обработки.

Author: Панфилова Кристина

Date: 19 Августа 2015
*/

#include <stdlib.h>
#include "../ref.h"

#define NOT_NULL 255

struct point
{
	uint32_t x;
	uint32_t y;
} typedef point;

bool equalPoint(const point left, const point right)
{
	if ( (left.x == right.x) && (left.y == right.y) )
		return true;
	return false;
}

bool is_correct(uint32_t x, uint32_t y, const uint32_t width, const uint32_t height)
{
	if ( (x < 0) || (y < 0) || (x >= width) || (y >= height) )
		return false;
	return true;
}
vx_status ref_FindContours(const vx_image src_image,
	vx_image dst_image)
{
	const uint32_t src_width = src_image->width;
	const uint32_t src_height = src_image->height;
	const uint32_t dst_width = dst_image->width;
	const uint32_t dst_height = dst_image->height;
	uint8_t indicator = 255;
	uint8_t seq[8][2] = { { -1, 0 }, { -1, -1 }, {0, -1}, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1}, { -1, 1} };

	if (src_width != dst_width || src_height != dst_height)
	{
		return VX_ERROR_INVALID_PARAMETERS;
	}

	const uint8_t* src_data = src_image->data;
	uint8_t* dst_data = dst_image->data;

	for (uint32_t i = 0; i < src_height; ++i)
	{
		for (uint32_t j = 0; j < src_width; ++j)
		{
			if (src_data[i * src_width + j] == NOT_NULL)
			{
				dst_data[i*src_width + j] = 255;

				point sp = {
					j,
					i
				};
				bool flag = false;
				//for (int k = 0; k < 8; ++k)
				//{
					//if (is_correct(sp.x + seq[k][0], sp.y + seq[k][1], src_width, src_height) &&
					//if (src_data[(sp.y + seq[2][1]) * src_width + (sp.x + seq[2][0])] == 0)
					//{
					//	flag = true;
					//}
				//}
				if (src_data[(i - 1) * src_width + j] == 0)
					dst_data[i*src_width + j] = 0;
				if (flag)
					continue;

				/// @ note cp - current point
				point cp = {
					j,
					i
				};
				//dst_data[i * src_width + j] = src_data[(i - 50) * src_width + j];
				/*do
				{
				for (int k = 0; k < 8; ++k)
				if (is_correct(cp.x + seq[k][0], cp.y + seq[k][1], src_width, src_height) &&
				(src_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] == NOT_NULL))
				{
				dst_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] = indicator;
				cp.x = cp.x + seq[k][0];
				cp.y = cp.y + seq[k][1];
				break;
				}
				} while (!equalPoint(sp, cp));*/

			}
			if (src_data[i * src_width + j] == 0)
			{
				//if (src_data[i*src_width + j] == 0)
				dst_data[i*src_width + j] = 255;
			}
		}
	}
	/*for (uint32_t i = 0; i < src_height; ++i)
	{
		for (uint32_t j = 0; j < src_width; ++j)
		{
			if (i == 0)
				dst_data[i * src_width + j] = indicator;
			else
				dst_data[i * src_width + j] = dst_data[(i - 1) * src_width + j];
			indicator = (indicator + 1) % 256;
		}
	}*/
	return VX_SUCCESS;
}