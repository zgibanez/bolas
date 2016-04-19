////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/types_c.h"
#include "opencv2/core/types.hpp"


using namespace cv;
using namespace std;

char* detect(vector<Point> poligon, Mat display);

int main(int argc, char** argv)
{
	Mat original, thresh;
	vector<vector<Point>> cnts;
	vector<Point> poligon;

	//load image
	original = imread(argv[1]);

	//convert img to HSV, blur it, threshold it
	cvtColor(original, original, COLOR_BGR2HSV);
	GaussianBlur(original,original,Size(5,5),0);
	inRange(original, Scalar(0,0,100), Scalar(255,255,255),thresh);

	//find contours on image
	findContours(thresh.clone(), cnts, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	cout << cnts.size() << endl;

	//approximate contours
	int i;
	double peri;
	char shape[10];
	for (i = 0; i < cnts.size(); i++) {
		peri = arcLength(cnts[i], 1);
		approxPolyDP(cnts[i], poligon, peri*0.04, 1);

		//find center of contour
		Moments M; float cX, cY;
		M = moments(cnts[i]);
		cX = M.m10/M.m00;
		cY = M.m01/M.m00;

		//find which shape fits best and
		//write it on img
		strcpy_s(shape,detect(poligon, original));
		putText(original, shape, Point(cX, cY), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255));

	}

	//draw contours found
	drawContours(original, cnts, -1, Scalar(100, 0, 100));
	imshow("original", original);
	waitKey(0);
	
	return 0;
}

char* detect(vector<Point> poligon, Mat display)
{

	if (poligon.size() == 3)
		return "triangle";
	if (poligon.size() == 4)
		return "square";
	if (poligon.size() > 5)
		return "circle"; //crimen

	
}
