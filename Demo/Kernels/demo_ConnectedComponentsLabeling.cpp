//@file demo_ConnectedComponentsLabeling.cpp
//@brief Contains demonstration of ConnectedComponentsLabeling function in comparing with OpenCV
//@author Andrey Belyakov
//@date 22 May 2016

#include "../stdafx.h"

#include <opencv2/opencv.hpp>

extern "C"
{
#include "Lib/Kernels/ref.h"
#include "Lib/Common/types.h"
}

#include "../DemoEngine.h"

///////////////////////////////////////////////////////////////////////////////
//@brief Demonstration of ConnectedComponentsLabeling function
class demo_ConnectedComponentsLabeling : public IDemoCase
{
public:
	///@brief default ctor
	demo_ConnectedComponentsLabeling()
		: m_threshold(127)
	{
		// nothing to do
	}

	///@see IDemoCase::ReplyName
	virtual std::string ReplyName() const override
	{
		return "Connected Components Labeling";
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
	const std::string m_binaryWindow = "binary";
}

///////////////////////////////////////////////////////////////////////////////
void demo_ConnectedComponentsLabeling::execute()
{
	cv::namedWindow(m_originalWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_openVXWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_openCVWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_binaryWindow, CV_WINDOW_NORMAL);

	//const std::string imgPath = "..\\Image\\test.jpg";
	//const std::string imgPath = "..\\Image\\frac2.png";
	const std::string imgPath = "..\\Image\\Solvay_conference_1927.png";
	m_srcImage = cv::imread(imgPath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(m_originalWindow, m_srcImage);

	cv::createTrackbar("Threshold:", m_originalWindow, &m_threshold, 255, applyParameters, static_cast<void*>(this));
	applyParameters(m_threshold, this);

	cv::waitKey(0);
}

///////////////////////////////////////////////////////////////////////////////
void demo_ConnectedComponentsLabeling::applyParameters(int, void* data)
{
	auto demo = static_cast<demo_ConnectedComponentsLabeling*>(data);

	const cv::Size imgSize(demo->m_srcImage.cols, demo->m_srcImage.rows);

	///@{ OPENCV
	cv::Mat binaryImage;
	cv::Mat labels(imgSize, CV_32S);
	cv::Mat cvImage = cv::Mat::zeros(demo->m_srcImage.rows, demo->m_srcImage.cols, CV_8UC3);
	cv::threshold(demo->m_srcImage, binaryImage, demo->m_threshold, 255, CV_THRESH_BINARY);

	int nLabels = connectedComponents(binaryImage, labels, 8);
	std::vector<cv::Vec3b> colors(nLabels);
	//background
	colors[0] = cv::Vec3b(0, 0, 0);			
	for (int label = 1; label < nLabels; ++label){
		colors[label] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}

	for (int r = 0; r < cvImage.rows; ++r){
		for (int c = 0; c < cvImage.cols; ++c){
			int label = labels.at<int>(r, c);
			cv::Vec3b &pixel = cvImage.at<cv::Vec3b>(r, c);
			pixel = colors[label];
		}
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

	uint8_t* binaryVXImage = static_cast<uint8_t*>(calloc(imgSize.width * imgSize.height, sizeof(uint8_t)));
	_vx_image dstVXImage = {
		binaryVXImage,
		imgSize.width,
		imgSize.height,
		VX_DF_IMAGE_U8,
		VX_COLOR_SPACE_DEFAULT
	};

	uint32_t* outVXImage = static_cast<uint32_t*>(calloc(imgSize.width * imgSize.height, sizeof(uint32_t)));
	_vx_image finalVXImage = {
		outVXImage,
		imgSize.width,
		imgSize.height,
		VX_DF_IMAGE_RGBX,
		VX_COLOR_SPACE_DEFAULT
	};

	ref_Threshold(&srcVXImage, &dstVXImage, &vxThresh);
	ref_ConnectedComponentsLabeling(&dstVXImage, &finalVXImage);

	const cv::Mat vxImage = cv::Mat(imgSize, CV_8UC4, outVXImage);
	const cv::Mat binaryOutImage = cv::Mat(imgSize, CV_8UC1, binaryVXImage);

	cv::imshow(m_openVXWindow, vxImage);
	cv::imshow(m_binaryWindow, binaryOutImage);
}

///////////////////////////////////////////////////////////////////////////////
IDemoCasePtr CreateConnectedComponentsLabelingDemo()
{
	return std::make_unique<demo_ConnectedComponentsLabeling>();
}
