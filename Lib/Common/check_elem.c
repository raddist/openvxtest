//@file check_elem.c
//@brief implementation of additional functions
//@author Andrey Belyakov
//@date 28 May 2016

#include "types.h"
#include "check_elem.h"

///@brief checks 2 numbers X and Y to correct coordinate or not
bool is_correct(int32_t x, int32_t y, const int32_t width, const int32_t height)
{
	if ((x < 0) || (y < 0) || (x >= width) || (y >= height))
		return false;
	return true;
};

///@brief checks two points to equality
bool equalPoints(const vx_coordinates2d_t left, const vx_coordinates2d_t right)
{
	if ((left.x == right.x) && (left.y == right.y))
		return true;
	return false;
}