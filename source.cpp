#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat img, img_r, img_g, img_b, imgHSV;
	int low_h, high_h, low_s, high_s, low_v, high_v;

	img = imread(argv[1], 0);
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

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

	while (true)
	{
		//Show output image with threshold
		inRange(img, Scalar(low_h, low_s, low_v), Scalar(high_h, high_s, high_v), img_r);
		imshow("Image with threshold", img_r);

		//Check esc key each 30ms
		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}
