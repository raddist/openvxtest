//@file ref_FindContours.c
//@brief Contains implementation of Find Contours function
//@author Andrey Belyakov
//@date 15 May 2016

#include <stdlib.h>
#include "../ref.h"
#include <time.h>

#define NOT_NULL 255



vx_status ref_FindContours(const vx_image src_image,
	vx_image dst_image,
	const uint8_t iter)
{
	const int32_t src_width = src_image->width;
	const int32_t src_height = src_image->height;
	const int32_t dst_width = dst_image->width;
	const int32_t dst_height = dst_image->height;

	if (src_width != dst_width || src_height != dst_height || iter <1 || iter > 4)
	{
		return VX_ERROR_INVALID_PARAMETERS;
	}

	uint32_t painter = 0;
	int32_t seq[8][2] = { { -1, 0 }, { -1, -1 }, {0, -1}, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1}, { -1, 1} };

	const uint8_t* src_data = src_image->data;
	uint32_t* dst_data = dst_image->data;
	srand((unsigned int)time(0));
	
	/// @note seq[beg_ind] shows side with whom we should start looking for contour point
	int beg_ind = 0;

	for (int __i = 0; __i < iter; ++__i)
	{
		for (int32_t i = 0; i < src_height; ++i)
		{
			for (int32_t j = 0; j < src_width; ++j)
			{
				if (src_data[i * src_width + j] == NOT_NULL &&
					dst_data[i  * src_width + j] == 0)
				{
					dst_data[i*src_width + j] = 0;

					/// @ note cp - start point of contour
					vx_coordinates2d_t sp;
					sp.x = j;
					sp.y = i;

					/// @brief check: if left point in't null, we are IN area and not on border
					if (is_correct(sp.x + seq[beg_ind][0], sp.y + seq[beg_ind][1], src_width, src_height))
					{
						if (src_data[(sp.y + seq[beg_ind][1]) * src_width + (sp.x + seq[beg_ind][0])] == NOT_NULL)
						{
							continue;
						}
					}


					/// @ note cp - current point of contour
					vx_coordinates2d_t cp;
					cp.x = j;
					cp.y = i;

					/// @brief inicialization of contour color
					painter = rand() % (205 + 50) << 0;
					painter |= rand() % (205 + 50) << 8;
					painter |= rand() % (205 + 50) << 16;

					dst_data[cp.y * src_width + cp.x] |= painter;

					/// @note seq[last_ind] shows side with whom we should start looking for next contour point
					int last_ind = beg_ind;

					bool flag;

					do
					{
						/// @note ind prevoius seq[ind] shows latest audited side of point
						int ind = last_ind;

						/// @note flag prevents an infinite loop
						flag = false;

						for (int8_t _k = 0; _k < 8; ++_k)
						{
							/// @note var k iterates clockwise all neighbors
							int k = (last_ind + 1 + _k) % 8;
							ind = (k + 7) % 8;

							if (is_correct(cp.x + seq[k][0], cp.y + seq[k][1], src_width, src_height))
							{
								/// @brief we need to previous point was black (==0) or out of image
								bool is_correct_pre = !is_correct(cp.x + seq[ind][0], cp.y + seq[ind][1], src_width, src_height);
								if (!is_correct_pre)
									is_correct_pre = (src_data[(cp.y + seq[ind][1]) * src_width + (cp.x + seq[ind][0])] == 0);

								/// @brief point must be correct and points on 2,3,4 iterations can't repaint pixels
								if ((src_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] == NOT_NULL) &&
									(is_correct_pre) &&
									((dst_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] == 0) ||
									(__i == 0)))
								{
									dst_data[(cp.y + seq[k][1]) * src_width + (cp.x + seq[k][0])] |= painter;

									cp.x = cp.x + seq[k][0];
									cp.y = cp.y + seq[k][1];

									last_ind = ((k - (k % 2)) + 6) % 8;
									flag = true;
									break;
								}
							}
						}
					} while (!equalPoints(sp, cp) && flag);

				}
				if (src_data[i * src_width + j] == 0)
				{
					dst_data[i*src_width + j] = 0;
				}
			}
		}
		/// @brief on next iteration we will looking for contour point with other side of pixels
		beg_ind += 2;
	}

	return VX_SUCCESS;
}