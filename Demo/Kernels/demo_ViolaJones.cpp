//@file demo_Viola_Jones.cpp
//@brief Contains demonstration of Viola Jones detector in comparing with OpenCV analog
//@author Andrey Belyakov
//@date 31 March 2016

#include "../stdafx.h"

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>


extern "C"
{
#include "Lib/Kernels/ref.h"
#include "Lib/Common/types.h"
}

#include "../DemoEngine.h"

///////////////////////////////////////////////////////////////////////////////
///@brief Demonstration of Viola Jones detector
class demo_Viola_Jones : public IDemoCase
{
	virtual std::string ReplyName() const override
	{
		return "Viola_Jones";
	}

private:
	///@see IDemoCase::execute
	virtual void execute() override;

	///@brief provide interactive demo
	static void apply(void* data);

private:
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
void demo_Viola_Jones::execute()
{
	cv::namedWindow(m_originalWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_openVXWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_openCVWindow, CV_WINDOW_NORMAL);
	cv::namedWindow(m_diffWindow, CV_WINDOW_NORMAL);

	const std::string imgPath = "..\\Image\\love1.png";
	m_srcImage = cv::imread(imgPath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(m_originalWindow, m_srcImage);
	apply(this);

	cv::waitKey(0);
}

///////////////////////////////////////////////////////////////////////////////
void demo_Viola_Jones::apply(void* data)
{
	auto demo = static_cast<demo_Viola_Jones*>(data);

	const cv::Size imgSize(demo->m_srcImage.cols, demo->m_srcImage.rows);

	cv::Mat cvImage = demo->m_srcImage;

	///@{ OPENCV

	///test

	const std::string face_cascade_name = "..\\Lib\\haarcascade_frontalface_default.xml";
	const std::string eyes_cascade_name = "..\\Lib\\haarcascade_eye_tree_eyeglasses.xml";

	cv::CascadeClassifier face_cascade;
	if (face_cascade.load(face_cascade_name) == 0)
		cv::imshow(m_openCVWindow, demo->m_srcImage);

	cv::CascadeClassifier eyes_cascade;
	eyes_cascade.load(eyes_cascade_name);

	std::vector<cv::Rect> faces;
	cv::Mat frame_gray = cvImage.clone();
	//cvtColor(cvImage, frame_gray, cv::COLOR_BGR2GRAY);

	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		cv::ellipse(cvImage, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(255, 0, 0), 4, 8, 0);

		cv::Mat faceROI = frame_gray(faces[i]);
		std::vector<cv::Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.3, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

		for (size_t j = 0; j < eyes.size(); j++)
		{
			cv::Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(cvImage, eye_center, radius, cv::Scalar(255, 0, 0), 4, 8, 0);
		}
	}
	//-- Show what you got
	cv::imshow(m_openCVWindow, cvImage);
	///end test

	//cv::imshow(m_openCVWindow, demo->m_srcImage);
	///@}

	_vx_image srcVXImage = {
		demo->m_srcImage.data,
		imgSize.width,
		imgSize.height,
		VX_DF_IMAGE_U8,
		VX_COLOR_SPACE_DEFAULT
	};

	uint8_t* outVXImage = static_cast<uint8_t*>(calloc(imgSize.width* imgSize.height, sizeof(uint8_t)));
	_vx_image dstVXImage = {
		outVXImage,
		imgSize.width,
		imgSize.height,
		VX_DF_IMAGE_U8,
		VX_COLOR_SPACE_DEFAULT
	};

	//ref_ViolaJonesDetector(&srcVXImage, &dstVXImage);

	const cv::Mat vxImage = cv::Mat(imgSize, CV_8UC1, outVXImage);
	cv::imshow(m_openVXWindow, vxImage);
}
////////////////////////////////////////////////////////////////////////////////////////
IDemoCasePtr CreateViolaJonesDemo()
{
	return std::make_unique<demo_Viola_Jones>();
}