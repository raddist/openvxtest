//@file check_elem.h
//@brief declaration of additional functions
//@author Andrey Belyakov
//@date 28 May 2016

#include "types.h"


///@brief checks 2 numbers X and Y to correct coordinate or not
bool is_correct(int32_t x, int32_t y, const int32_t width, const int32_t height);

///@brief checks two points to equality
bool equalPoints(const vx_coordinates2d_t left, const vx_coordinates2d_t right);



