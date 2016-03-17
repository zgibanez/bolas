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


int main(int argc, char** argv)
{
	Mat img, img_r, img_y, img_g, img_b, imgHSV;
	Mat img_ye;
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

	while (true)
	{
		//Show output image with threshold
		inRange(imgHSV, Scalar(low_h, low_s, low_v), Scalar(high_h, high_s, high_v), img_r);
		imshow("Image with trackbar threshold", img_r);

		//Yellow balls results
		inRange(img, Scalar(0, 30, 30), Scalar(40, 250, 250), img_y);
		erode(img_y, img_ye, getStructuringElement(MORPH_ELLIPSE, Size(7, 7))); /*Opening*/
		dilate(img_ye, img_ye, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
		imshow("Yellow balls results", img_y);
		imshow("Yellow balls results -- Opening", img_ye);

		//Check esc key each 30ms
		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}
