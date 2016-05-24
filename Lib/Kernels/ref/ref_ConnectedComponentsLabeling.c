//@file ref_ConnectedComponentsLabeling.c
//@brief implementation of the ConnectedComponentsLabeling algorithm
//@author ANdrey Belyakov
//@date 24 May 2016


#include <stdlib.h>
#include "../ref.h"
#include <time.h>
#include <malloc.h>

bool is_correct1(int32_t x, int32_t y, const int32_t width, const int32_t height)
{
	if ((x < 0) || (y < 0) || (x >= width) || (y >= height))
		return false;
	return true;
}

vx_status ref_ConnectedComponentsLabeling(const vx_image src_image,
	vx_image dst_image)
{
	const int32_t src_width = src_image->width;
	const int32_t src_height = src_image->height;
	const int32_t dst_width = dst_image->width;
	const int32_t dst_height = dst_image->height;

	int32_t seq[4][2] = { { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };

	if (src_width != dst_width || src_height != dst_height)
	{
		return VX_ERROR_INVALID_PARAMETERS;
	}

	const uint8_t* src_data = src_image->data;
	uint32_t* dst_data = dst_image->data;

	uint32_t mark_counter = 1;

	srand(time(0));


	uint32_t* marked_img = (uint32_t*)calloc(src_width * src_height, sizeof(uint32_t));
	uint32_t* table_of_mark = (uint32_t*)calloc(src_width * src_height, sizeof(uint32_t));
	table_of_mark[0] = 0;

	for (int32_t i = 0; i < src_height; ++i)
	{
		for (int32_t j = 0; j < src_width; ++j)
		{
			marked_img[i  * src_width + j] = 0;

			if (src_data[i  * src_width + j] != 0)
			{
				uint32_t temp_mark = mark_counter;


				for (uint8_t k = 0; k < 4; k++)
				{
					bool test = false;
					test = is_correct1(j + seq[k][0], i + seq[k][1], src_width, src_height);
					if (test)
					if ((marked_img[(i + seq[k][1]) * src_width + (j + seq[k][0])] != 0) &&
						(marked_img[(i + seq[k][1]) * src_width + (j + seq[k][0])] < temp_mark))
					{
						temp_mark = marked_img[(i + seq[k][1]) * src_width + (j + seq[k][0])];
					}
				}
				marked_img[i  * src_width + j] = temp_mark;

					dst_data[i  * src_width + j] |= 255;
				if (temp_mark == mark_counter)
				{
					table_of_mark[mark_counter] = mark_counter;
					mark_counter++;
				}
				{
					for (uint8_t k = 0; k < 4; k++)
					{
						if (is_correct1(j + seq[k][0], i + seq[k][1], src_width, src_height))
						{

							uint32_t index = marked_img[(i + seq[k][1]) * src_width + (j + seq[k][0])];

							if ((index != 0) &&
								(index > temp_mark))
							{
								table_of_mark[index] = temp_mark;
							}
						}
					}
				}
			}
		}
	}
	

	uint32_t* table_of_color = (uint32_t*)calloc(mark_counter, sizeof(uint32_t));
	table_of_color[0] = 0;

	for (int32_t i = 1; i < mark_counter; ++i)
	{
		table_of_mark[i] = table_of_mark[table_of_mark[i]];

		int32_t painter = 0;
		painter = rand() % 255 << 0;
		painter |= rand() % 255 << 8;
		painter |= rand() % 255 << 16;


		table_of_color[i] |= painter;
	}

	for (int32_t i = 0; i < src_height; ++i)
	{
		for (int32_t j = 0; j < src_width; ++j)
		{
			if ((marked_img[i  * src_width + j] != 0))
			{
				marked_img[i  * src_width + j] = table_of_mark[marked_img[i  * src_width + j]];

				dst_data[i  * src_width + j] |= table_of_color[marked_img[i  * src_width + j]];
			}
		}
	}
	return VX_SUCCESS;
}