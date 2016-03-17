////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

string type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}

void ballfinder(Mat& img, int H_low, int H_high)
{
	Mat img_threshold, img_close;
	vector<Vec3f> circles; /*This vector stores x,y and radius of the circles found with Hough Transform*/

						   //Apply threshold by fcn input
	inRange(img, Scalar(H_low, 30, 30), Scalar(H_high, 255, 255), img_threshold);
	imshow("Yellow balls results", img_threshold);

	//Close threshold image
	dilate(img_threshold, img_close, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
	erode(img_close, img_close, getStructuringElement(MORPH_ELLIPSE, Size(10, 10))); /*Close?*/

																					 //Find circles
	HoughCircles(img_close, circles, CV_HOUGH_GRADIENT, 1, img_close.rows / 8, 200, 20, 0, 0); /*Hough circle detection*/

																							   //Draw circles in original
	int count = 0;
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]); /*Take the radius from circles detected*/
											 // circle center
		circle(img, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(img, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		count++;
	}

	cout << count << " yellow balls found\n" << endl;
}


int main(int argc, char** argv)
{
	Mat img, img_r, img_y, img_g, img_b, imgHSV;
	Mat img_ye, img_ye_channels;
	int low_h, high_h, low_s, high_s, low_v, high_v;

	img = imread(argv[1], 1);

	//Determine img.type() (For Debug)
	string ty = type2str(img.type());
	printf("Matrix: %s %dx%d \n", ty.c_str(), img.cols, img.rows);

	//Create window with trackbar
	namedWindow("Control", CV_WINDOW_NORMAL);
	//Create trackbar
	cvCreateTrackbar("Hue lower threshold", "Control", &low_h, 179);
	cvCreateTrackbar("Hue higher threshold", "Control", &high_h, 179);
	cvCreateTrackbar("Saturation lower threshold", "Control", &low_s, 255);
	cvCreateTrackbar("Saturation Higher Threshold", "Control", &high_s, 255);
	cvCreateTrackbar("Value Lower Threshold", "Control", &low_v, 255);
	cvCreateTrackbar("Value Higher Threshold", "Control", &high_v, 255);

	//Check if img is OK
	if (img.empty())
	{
		cout << "Error: image empty" << endl;
		return 1;
	}

	cvtColor(img, imgHSV, COLOR_BGR2GRAY);

	//AMARILLAS H : 0 - 40
	//			S : 30 - 250
	//			V : 30 - 250

	
	//Show output image with threshold
	inRange(imgHSV, Scalar(low_h, low_s, low_v), Scalar(high_h, high_s, high_v), img_r);
	//imshow("Image with trackbar threshold", img_r);
	imshow("Circles detected", img);

	ballfinder(img, 1, 40);

	while (true)
	{
		//Check esc key each 30ms
		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}
