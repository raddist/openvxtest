//@file ref_ConnectedComponentsLabeling.c
//@brief implementation of the ConnectedComponentsLabeling algorithm
//@author Andrey Belyakov
//@date 24 May 2016


#include <stdlib.h>
#include "../ref.h"
#include <time.h>
#include <malloc.h>

/// @brief finds nimimal equal mark
static uint32_t getMinIndex(uint32_t* arr, uint32_t ind)
{
	return (arr[ind] == ind) ? ind : getMinIndex(arr, arr[ind]);
}

vx_status ref_ConnectedComponentsLabeling(const vx_image src_image,
	vx_image dst_image)
{
	const int32_t src_width = src_image->width;
	const int32_t src_height = src_image->height;
	const int32_t dst_width = dst_image->width;
	const int32_t dst_height = dst_image->height;

	/// @ note seq - array of relative paths to 4 (left, top left to top right) neighbor pixels
	int32_t seq[4][2] = { { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };
	
	if (src_width != dst_width || src_height != dst_height)
	{
		return VX_ERROR_INVALID_PARAMETERS;
	}

	const uint8_t* src_data = src_image->data;
	uint32_t* dst_data = dst_image->data;

	/// @note count number of firstly detected areas
	uint32_t mark_counter = 1;

	srand((unsigned int)time(0));

	/// @note every pixels of this temp image marked with number of area which they belong
	uint32_t* marked_img = (uint32_t*)calloc(src_width * src_height, sizeof(uint32_t));

	/// @note t_o_m[ind] = val means area marked with ind equalent to val area (ind > val always)
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


				/// @brief define minimal neighbor mark
				for (uint8_t k = 0; k < 4; k++)
				{
					if (is_correct(j + seq[k][0], i + seq[k][1], src_width, src_height))
					{
						uint32_t index = marked_img[(i + seq[k][1]) * src_width + (j + seq[k][0])];
						table_of_mark[index] = getMinIndex(table_of_mark, index);

						if ((index != 0) &&
							(table_of_mark[index] < temp_mark))
						{
							temp_mark = table_of_mark[index];
						}
					}
				}
				marked_img[i  * src_width + j] = temp_mark;

				/// @note if pixel have no marked neighbors 
				if (temp_mark == mark_counter)
				{
					table_of_mark[mark_counter] = mark_counter;
					mark_counter++;
				}
				else
				{
					/// @brief if one of neughbor marks is lowest then other
					/// then other mark will be equal to minimal
					for (uint8_t k = 0; k < 4; k++)
					{
						if (is_correct(j + seq[k][0], i + seq[k][1], src_width, src_height))
						{

							uint32_t index = marked_img[(i + seq[k][1]) * src_width + (j + seq[k][0])];

							if ((index != 0) &&
								(table_of_mark[index] > temp_mark))
							{
								table_of_mark[index] = temp_mark;
							}
						}
					}
				}

			}
		}
	}

	///@note array of colors for each mark of areas
	uint32_t* table_of_color = (uint32_t*)calloc(mark_counter, sizeof(uint32_t));
	table_of_color[0] = 0;


	/// @brief for every mark find minimal equal mark
	/// @note it's garanted that all previous marks have already finded this ones
	for (uint32_t i = 1; i < mark_counter; ++i)
	{
		table_of_mark[i] = table_of_mark[table_of_mark[i]];


		int32_t painter = 0;
		painter = rand() % (205 + 50) << 0;
		painter |= rand() % (205 + 50) << 8;
		painter |= rand() % (205 + 50) << 16;


		table_of_color[i] |= painter;
	}

	///@brief painting image
	for (int32_t i = 0; i < src_height; ++i)
	{
		for (int32_t j = 0; j < src_width; ++j)
		{
			if ((marked_img[i  * src_width + j] != 0))
			{
				///@ note define area of pixel
				marked_img[i  * src_width + j] = table_of_mark[marked_img[i  * src_width + j]];

				dst_data[i  * src_width + j] |= table_of_color[marked_img[i  * src_width + j]];
			}
		}
	}

	free(marked_img);
	free(table_of_mark);
	free(table_of_color);

	return VX_SUCCESS;
}