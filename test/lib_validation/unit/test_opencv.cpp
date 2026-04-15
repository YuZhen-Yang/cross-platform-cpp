#include <gtest/gtest.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

TEST(OpenCV, MatCreationAndDimensions)
{
	cv::Mat m = cv::Mat::zeros(4, 6, CV_64F);
	EXPECT_EQ(m.rows, 4);
	EXPECT_EQ(m.cols, 6);
	EXPECT_EQ(m.type(), CV_64F);
	EXPECT_DOUBLE_EQ(m.at<double>(0, 0), 0.0);
}

TEST(OpenCV, MatArithmetic)
{
	cv::Mat a = cv::Mat::ones(2, 2, CV_32F) * 3.0f;
	cv::Mat b = cv::Mat::ones(2, 2, CV_32F) * 2.0f;
	cv::Mat c = a + b;
	EXPECT_FLOAT_EQ(c.at<float>(0, 0), 5.0f);
	EXPECT_FLOAT_EQ(c.at<float>(1, 1), 5.0f);
}

TEST(OpenCV, GaussianBlurSmoothesPeak)
{
	cv::Mat src = cv::Mat::ones(9, 9, CV_32F);
	src.at<float>(4, 4) = 100.0f;
	cv::Mat dst;
	cv::GaussianBlur(src, dst, cv::Size(5, 5), 1.0);
	// Center pixel must be reduced by the blur
	EXPECT_LT(dst.at<float>(4, 4), 100.0f);
	EXPECT_GT(dst.at<float>(4, 4), 1.0f);
}

TEST(OpenCV, ConvertToGrayscale)
{
	cv::Mat bgr(4, 4, CV_8UC3, cv::Scalar(200, 100, 50));
	cv::Mat gray;
	cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);
	EXPECT_EQ(gray.channels(), 1);
	EXPECT_EQ(gray.rows, 4);
	EXPECT_EQ(gray.cols, 4);
}
