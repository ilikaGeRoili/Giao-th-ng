////#include<opencv\cv.h>
////#include<opencv2\highgui\highgui.hpp>
////#include<opencv\ml.h>
////
////using namespace std;
////using namespace cv;
////
////int main()
////{
////	//Load ảnh lên
////	IplImage* img = cvLoadImage("asd1.jpg");
////	//cvShowImage("Tên", img);
////
////	//// Xám hóa ảnh
////	//IplImage* img2 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);// Tạo ảnh mới bằng cách resize ảnh gốc
////	//// có chiều dài và chiều rộng tự động định kích cỡ và truyền vào biến img2 là ảnh 8 bit loại unsign nên nó là hàm IPL_DEPTH_8U và có 1 kênh màu
////	//cvCvtColor(img, img2, CV_BGR2GRAY);
////	//cvShowImage("Xam anh", img2);
////
////	//// Nhị phân ảnh
////	//IplImage* img3 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
////	//cvThreshold(img2, img3, 50, 255, CV_THRESH_BINARY);
////	//cvShowImage("Anh nhi phan", img3);
////
////	// Dò/ tách biên bằng phương pháp Canny
////	IplImage* cannyImg = cvCreateImage(cvGetSize(img), img->depth, 1);
////	cvCanny(img, cannyImg, 100, 200);
////	cvShowImage("Canny", cannyImg);
////	cvWaitKey(0);
////
////	return 0;
////}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


int main()
{
	VideoCapture cap("MVI_1049.avi"); //capture the video from web cam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	int _rows = 100;

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	cvCreateTrackbar("Rows", "Control", &_rows, 1000);
	
	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video
		resize(imgOriginal, imgOriginal, Size(imgOriginal.rows, _rows));

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		namedWindow("Original", CV_WINDOW_FREERATIO);
		imshow("Original", imgOriginal); //show the original image
		

		Mat imgHSV;
	

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HLS); //Convert the captured frame from BGR to HSV
	    // recover size of imgThresholded by size of imgOriginal
		//resize(imgOriginal, imgThresholded, Size(imgOriginal.rows, imgOriginal.cols));
	
		Mat imgThresholded;


		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

																									  //morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));



		IplImage copy1 = imgThresholded; 
		IplImage* imgCanny = &copy1;// convert Mat to IplImage* 




		// Dò( tách) biên bằng phương pháp Canny
		IplImage* cannyImg = cvCreateImage(cvGetSize(imgCanny), imgCanny->depth, 1);
		cvCanny(imgCanny, cannyImg, 100, 200);

		////////////////


		
		Mat imgGray = cvarrToMat(cannyImg); // Convert IplImage to Mat
		


		// Reduce the noise so we avoid false circle detection
		GaussianBlur(imgGray, imgGray, Size(9, 9), 2, 2);

		vector<Vec3f> circles;

		// Apply the Hough Transform to find the circles
		
		HoughCircles(imgGray, circles, CV_HOUGH_GRADIENT, 1, 30, 200, 50, 0, 0);
	
		/* Arguments:
		src_gray: Input image (grayscale).
		circles: A vector that stores sets of 3 values: xc,yc,r for each detected circle.
		HOUGH_GRADIENT: Define the detection method. Currently this is the only one available in OpenCV.
		dp = 1: The inverse ratio of resolution.
		min_dist = 30: Minimum distance between detected centers.
		param_1 = 200: Upper threshold for the internal Canny edge detector.
		param_2 = 50*: Threshold for center detection.
		min_radius = 0: Minimum radio to be detected. If unknown, put zero as default.
		max_radius = 0: Maximum radius to be detected. If unknown, put zero as default.
		*/

		// Draw the circles detected
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
		//	circle(imgOriginal, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center     
			//circle(imgOriginal, center, radius, Scalar(0, 0, 255), 3, 8, 0);// circle outline
			cout << "center : " << center << "\nradius : " << radius << endl;

			Rect crop = Rect(center.x - radius, center.y - radius, 2 * radius, 2 * radius);
			Mat cropImg = imgOriginal(crop);
			namedWindow("cropoed", CV_WINDOW_FREERATIO);
			imshow("cropoed", cropImg);
			imwrite("D:/OpenCv/testDemo/img.jpg", cropImg);
			cout << cropImg.size();
			waitKey(0);
	

			
		}

		////////////////
		
	
		imshow("Thresholded Image", imgThresholded); //show the thresholded image
	

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// Find ellipes in image
//
///********************************************************************************
//*
//*
//*  This program is demonstration for ellipse fitting. Program finds
//*  contours and approximate it by ellipses.
//*
//*  Trackbar specify threshold parametr.
//*
//*  White lines is contours. Red lines is fitting ellipses.
//*
//*
//*  Autor:  Denis Burenkov.
//*
//*
//*
//********************************************************************************/
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <iostream>
//using namespace cv;
//using namespace std;
//
//
//
//int sliderPos = 70;
//
//Mat image;
//
//void processImage(int, void*);
//
//int main(int argc, char** argv)
//{
//
//	image = imread("asd2.png",0);
//	if (image.empty())
//	{
//		cout << "Couldn't open image " ;
//		return 0;
//	}
//
//	imshow("source", image);
//	namedWindow("result", 1);
//
//	// Create toolbars. HighGUI use.
//	createTrackbar("threshold", "result", &sliderPos, 255, processImage);
//	processImage(0, 0);
//
//	// Wait for a key stroke; the same function arranges events processing
//	waitKey();
//	return 0;
//}
//
//// Define trackbar callback functon. This function find contours,
//// draw it and approximate it by ellipses.
//void processImage(int /*h*/, void*)
//{
//	vector<vector<Point> > contours;
//	Mat bimage = image >= 70;
//
//	findContours(bimage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
//
//	Mat cimage = Mat::zeros(bimage.size(), CV_8UC3);
//
//	for (size_t i = 0; i < contours.size(); i++)
//	{
//		size_t count = contours[i].size();
//		if (count < 6)
//			continue;
//
//		Mat pointsf;
//		Mat(contours[i]).convertTo(pointsf, CV_32F);
//		RotatedRect box = fitEllipse(pointsf);
//
//		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
//			continue;
//		drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);
//
//		ellipse(cimage, box, Scalar(0, 0, 255), 1, CV_AA);
//		ellipse(cimage, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0, 255, 255), 1, CV_AA);
//		Point2f vtx[4];
//		box.points(vtx);
//		for (int j = 0; j < 4; j++)
//			line(cimage, vtx[j], vtx[(j + 1) % 4], Scalar(0, 255, 0), 1, CV_AA);
//	}
//
//	imshow("result", cimage);
//}
//
