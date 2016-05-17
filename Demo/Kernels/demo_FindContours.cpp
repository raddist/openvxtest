//@file demo_FindContours.h
//@brief Contains demonstration of FInd contours function in comparing with OpenCV
//@author Andrey Belyakov
//@date 15 May 2016

#include "../stdafx.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv/highgui.h>
#include <stdlib.h>

extern "C"
{
#include "Lib/Kernels/ref.h"
#include "Lib/Common/types.h"
}

#include "../DemoEngine.h"

///////////////////////////////////////////////////////////////////////////////
//@brief Demonstration of Threshold function
class demo_FindContours : public IDemoCase
{
public:
	///@brief default ctor
	demo_FindContours()
		: m_threshold(127)
	{
		// nothing to do
	}

	///@see IDemoCase::ReplyName
	virtual std::string ReplyName() const override
	{
		return "Find contours";
	}

private:
	///@see IDemoCase::execute
	virtual void execute() override;

	///@brief provide interactive demo
	static void applyParameters(int pos, void* data);

private:
	int m_threshold;
	cv::Mat m_srcImage;
};

///////////////////////////////////////////////////////////////////////////////
namespace
{
	const std::string m_openVXWindow = "openVX";
	const std::string m_openCVWindow = "openCV";
	const std::string m_originalWindow = "original";
	const std::string m_diffWindow = m_openVXWindow + "-" + m_openCVWindow;
}

///////////////////////////////////////////////////////////////////////////////
void demo_FindContours::execute()
{
	cv::namedWindow(m_originalWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_openVXWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_openCVWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_diffWindow, CV_WINDOW_NORMAL);

	const std::string imgPath = "..\\Image\\apple.png";
	m_srcImage = cv::imread(imgPath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(m_originalWindow, m_srcImage);

	cv::createTrackbar("Threshold:", m_originalWindow, &m_threshold, 255, applyParameters, static_cast<void*>(this));
	applyParameters(m_threshold, this);

	cv::waitKey(0);
}

///////////////////////////////////////////////////////////////////////////////
void demo_FindContours::applyParameters(int, void* data)
{
	auto demo = static_cast<demo_FindContours*>(data);

	const cv::Size imgSize(demo->m_srcImage.cols, demo->m_srcImage.rows);

	///@{ OPENCV
	cv::Mat binaryImage;
	cv::Mat cvImage = cv::Mat::zeros(demo->m_srcImage.rows, demo->m_srcImage.cols, CV_8UC3);
	cv::threshold(demo->m_srcImage, binaryImage, demo->m_threshold, 255, CV_THRESH_BINARY);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(binaryImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < static_cast<int>(contours.size()); ++i)
	{
		cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
		drawContours(cvImage, contours, i, color, 1, 8, hierarchy);
	}

	cv::imshow(m_openCVWindow, cvImage);
	///@}

	
	///@{ OPENVX
	_vx_threshold vxThresh = { VX_THRESHOLD_TYPE_BINARY, uint8_t(demo->m_threshold), 0/* dummy value */, 255 /* dummy value */ };
	_vx_image srcVXImage = {
		demo->m_srcImage.data,
		imgSize.width,
		imgSize.height,
		VX_DF_IMAGE_U8,
		VX_COLOR_SPACE_DEFAULT
	};

	uint8_t* tempVXImage = static_cast<uint8_t*>(calloc(imgSize.width * imgSize.height, sizeof(uint8_t)));
	_vx_image dstVXImage = {
		tempVXImage,
		imgSize.width,
		imgSize.height,
		VX_DF_IMAGE_U8,
		VX_COLOR_SPACE_DEFAULT
	};

	uint8_t* outVXImage = static_cast<uint8_t*>(calloc(imgSize.width * imgSize.height, sizeof(uint8_t)));
	_vx_image finalVXImage = {
		outVXImage,
		imgSize.width,
		imgSize.height,
		VX_DF_IMAGE_U8,
		VX_COLOR_SPACE_DEFAULT
	};

	ref_Threshold(&srcVXImage, &dstVXImage, &vxThresh);
	ref_FindContours(&dstVXImage, &finalVXImage);

	const cv::Mat vxImage = cv::Mat(imgSize, CV_8UC1, outVXImage);
	cv::imshow(m_openVXWindow, vxImage);
	///@}

	// Show difference of OpenVX and OpenCV
	//const cv::Mat diffImage(imgSize, CV_8UC1);
	//cv::absdiff(vxImage, cvImage, diffImage);
	//cv::imshow(m_diffWindow, diffImage);
	
}

///////////////////////////////////////////////////////////////////////////////
IDemoCasePtr CreateFindContoursDemo()
{
	return std::make_unique<demo_FindContours>();
}