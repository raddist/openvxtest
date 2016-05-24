//@file ref_FindContours.c
//@brief Contains implementation of Find Contours function
//@author Andrey Belyakov
//@date 15 May 2016

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

bool is_correct(int32_t x, int32_t y, const int32_t width, const int32_t height)
{
	if ((x < 0) || (y < 0) || (x >= width) || (y >= height))
		return false;
	return true;
}

vx_status ref_FindContours(const vx_image src_image,
	vx_image dst_image)
{
	const int32_t src_width = src_image->width;
	const int32_t src_height = src_image->height;
	const int32_t dst_width = dst_image->width;
	const int32_t dst_height = dst_image->height;

	uint32_t painter = 0;
	int32_t seq[8][2] = { { -1, 0 }, { -1, -1 }, {0, -1}, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1}, { -1, 1} };

	if (src_width != dst_width || src_height != dst_height)
	{
		return VX_ERROR_INVALID_PARAMETERS;
	}

	const uint8_t* src_data = src_image->data;
	//uint8_t* dst_data = dst_image->data;
	uint32_t* dst_data = dst_image->data;
	srand(time(0));
	
	for (int32_t i = 0; i < src_height; ++i)
	{
		for (int32_t j = 0; j < src_width; ++j)
		{
			if (src_data[i * src_width + j] == NOT_NULL && 
				dst_data[i  * src_width + j] == 0 )
			{
				dst_data[i*src_width + j] = 0;

				point sp;
				sp.x = j;
				sp.y = i;
				bool flag = true;

				/// @brief check: if left point in't null, we are IN area and not on border
				if (is_correct(sp.x + seq[0][0], sp.y + seq[0][1], src_width, src_height))
				{
					if (src_data[(sp.y + seq[0][1]) * src_width + (sp.x + seq[0][0])] == 0)
						flag = false;
				}
				if (flag)
					continue;

				/// @ note cp - current point
				point cp;
				cp.x = j;
				cp.y = i;
				//dst_data[i * src_width + j] = src_data[(i - 50) * src_width + j];

				painter = rand() % 255 << 0;
				painter |= rand() % 255 << 8;
				painter |= rand() % 255 << 16;


				dst_data[cp.y * src_width + cp.x] |= painter;
				int counter = 0;
				
				int last_ind = 0;
				do
				{
					int ind = last_ind;
					for (int8_t _k = 0; _k < 8; ++_k)
					{
						int k = (last_ind + 1 + _k) % 8;
						ind = (k + 7) % 8;
						if (is_correct(cp.x + seq[k][0], cp.y + seq[k][1], src_width, src_height))
						{
							bool is_correct_pre = !is_correct(cp.x + seq[ind][0], cp.y + seq[ind][1], src_width, src_height);
							if (!is_correct_pre)
								is_correct_pre = (src_data[(cp.y + seq[ind][1]) * src_width + (cp.x + seq[ind][0])] == 0);
							if ( (src_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] == NOT_NULL) &&
								(is_correct_pre) )
							{
								dst_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] |= painter;
								cp.x = cp.x + seq[k][0];
								cp.y = cp.y + seq[k][1];
								last_ind = ((k -(k%2)) + 6) % 8;
								break;
							}
						}
					}
					//counter++;
				} while (!equalPoint(sp, cp));
				
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