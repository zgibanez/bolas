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
		imshow("Image with trackbar threshold", img_r);

		//Yellow balls results
		vector<Vec3f> circles; /*This vector stores x,y and radius of the circles found with Hough Transform*/

		inRange(img, Scalar(0, 30, 30), Scalar(40, 250, 250), img_y);
		imshow("Yellow balls results", img_y);

		dilate(img_y, img_ye, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_ye, img_ye, getStructuringElement(MORPH_ELLIPSE, Size(5, 5))); /*Close?*/
		


		//split(img_ye, img_ye_channels);
		HoughCircles(img_ye, circles, CV_HOUGH_GRADIENT, 1, img_ye.rows / 8, 40, 40, 0, 0); /*Hough circle detection*/
		
		//Draw circles
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]); /*Take the radius from circles detected*/
			// circle center
			circle(img, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			circle(img, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		}
		//imshow("Yellow balls results 2", img_ye);
		//imshow("Yellow balls results -- Opening", img);

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
