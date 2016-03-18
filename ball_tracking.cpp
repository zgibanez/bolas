#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

/*Function to draw ball*/
void ballfinder(Mat& img, Mat& display, int H_low, int H_high)
{
	Mat img_threshold, img_close;
	vector<Vec3f> circles; /*This vector stores x,y and radius of the circles found with Hough Transform*/

						   //Apply threshold by fcn input
	inRange(img, Scalar(H_low, 180, 25), Scalar(H_high, 255, 255), img_threshold);

	//Close threshold image
	dilate(img_threshold, img_close, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(img_close, img_close, getStructuringElement(MORPH_ELLIPSE, Size(3, 3))); /*Close?*/
	//imshow("closed", img_close);

	//Find circles
	HoughCircles(img_close, circles, CV_HOUGH_GRADIENT, 1, img_threshold.rows / 8, 200, 15, 0, 0);

	//Draw and count circles in original
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]); /*Take the radius from circles detected*/
											 // circle center
		circle(display, center, 3, Scalar(255, 255, 255), -1, 8, 0);
		// circle outline
		circle(display, center, radius, Scalar(0, 0, 255), 3, 8, 0);

	}

}




int main(int argc, char** argv)
{
	Mat frame, frameHSV, frame_threshold, transformed;
	int low_h = 0, high_h = 250, low_s = 0, high_s = 250, low_v = 0, high_v = 250;

	//Create window with trackbar
	namedWindow("Control", CV_WINDOW_NORMAL);
	//Create trackbar
	cvCreateTrackbar("Hue lower threshold", "Control", &low_h, 179);
	cvCreateTrackbar("Hue higher threshold", "Control", &high_h, 179);
	cvCreateTrackbar("Saturation lower threshold", "Control", &low_s, 255);
	cvCreateTrackbar("Saturation Higher Threshold", "Control", &high_s, 255);
	cvCreateTrackbar("Value Lower Threshold", "Control", &low_v, 255);
	cvCreateTrackbar("Value Higher Threshold", "Control", &high_v, 255);

	VideoCapture capture("C:/images/2dball.mp4"); //CHANGE
	

	if (!capture.isOpened())
		throw "Error when reading video";
	/* /////////////////OUTPUT////////////
	//Preparing output: 
	VideoWriter output;
	string filename = "C:/images/output.mp4";
	int fcc = CV_FOURCC('D', 'I', 'V', 'X');
	double fps = 20;
	Size frameSize(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));

	output = VideoWriter(filename, fcc, fps, frameSize);
	/////////////////OUTPUT////////////*/

	namedWindow("w", 1);
	for (; ; )
	{
		capture >> frame;
		if (frame.empty())
			break;

		blur(frame, frame, Size(5, 5));

		//transform video frame into something else
		cvtColor(frame, frameHSV, COLOR_BGR2HSV);
		inRange(frameHSV, Scalar(low_h, low_s, low_v), Scalar(high_h, high_s, high_v), frame_threshold);
		ballfinder(frameHSV, frame, 0, 20);
		//imshow("w", frame_threshold);
		imshow("detection", frame);

		////////////////OUTPUT////////////////
		/*//copy the transformed frame to output
		output.write(transformed);*/
		//////////////////////////////////////

		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	capture.release();
	//output.release();
	destroyAllWindows();
	return 0;
}
