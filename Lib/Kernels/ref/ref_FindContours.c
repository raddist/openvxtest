/*
File: ref_Threshold.c
Содержит эталонную реализацию пороговой обработки.

Author: Панфилова Кристина

Date: 19 Августа 2015
*/

#include <stdlib.h>
#include "../ref.h"
#include <time.h>

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
	uint32_t indicator = 256;
	uint32_t painter = 0;
	uint32_t seq[8][2] = { { -1, 0 }, { -1, -1 }, {0, -1}, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1}, { -1, 1} };

	if (src_width != dst_width || src_height != dst_height)
	{
		return VX_ERROR_INVALID_PARAMETERS;
	}

	const uint8_t* src_data = src_image->data;
	//uint8_t* dst_data = dst_image->data;
	uint32_t* dst_data = dst_image->data;
	srand(time(0));
	
	for (uint32_t i = 0; i < src_height; ++i)
	{
		for (uint32_t j = 0; j < src_width; ++j)
		{
			/*dst_data[i*src_width + j] = dst_data[i*src_width + j] | (200<<0);
			dst_data[i*src_width + j] = dst_data[i*src_width + j] | (200 << 8);

			if ((dst_data[i*src_width + j]>>8)&(255) == 200)
				dst_data[i*src_width + j] = dst_data[i*src_width + j] | (200 << 16);*/

			//uint8_t* r = ((uint8_t*)(dst_data[i*src_width + j]))[4];
			//r = 255;
			
			if (src_data[i * src_width + j] == NOT_NULL && 
				dst_data[i  * src_width + j] == 0 )
			{
				dst_data[i*src_width + j] = 0;

				point sp;
				sp.x = j;
				sp.y = i;
				bool flag = true;

				if (is_correct(sp.x + seq[0][0], sp.y + seq[0][1], src_width, src_height))
				{
					if (src_data[(sp.y + seq[0][1]) * src_width + (sp.x + seq[0][0])] == 0)
						//dst_data[sp.y*src_width + sp.x] = indicator;
						flag = false;
				}
				if (flag)
					continue;

				/// @ note cp - current point
				point cp;
				cp.x = j;
				cp.y = i;
				//dst_data[i * src_width + j] = src_data[(i - 50) * src_width + j];

				uint8_t rnd = rand()%3;

				painter = 255 << 8*rnd;
				int counter = 0;

				int last_ind = 0;
				do
				{
					flag = true;

					int ind = 7;
					//k = 0;
					for (uint8_t k = 0; k < 8; ++k)
					{
						if (is_correct(cp.x + seq[k][0], cp.y + seq[k][1], src_width, src_height))
						{
							if ( (src_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] == NOT_NULL) &&
								(src_data[(cp.y + seq[ind][1]) * src_width + (cp.x + seq[ind][0])] == 0) &&
								dst_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] == 0)
							{
								dst_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] |= painter;
								cp.x = cp.x + seq[k][0];
								cp.y = cp.y + seq[k][1];
								flag = false;
								break;
							}
						}
						ind = (ind + 1) % 8;
					}
					counter++;
				} while (!equalPoint(sp, cp) && !flag && counter < 100000000);
				
			}
			if (src_data[i * src_width + j] == 0)
			{
				//if (src_data[i*src_width + j] == 0)
				dst_data[i*src_width + j] = 0;
			}
		}
	}

	return VX_SUCCESS;
}