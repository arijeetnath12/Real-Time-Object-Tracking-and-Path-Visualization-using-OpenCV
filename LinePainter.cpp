#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

const int max_value_H = 179;
const int max_value = 255;


vector<vector<int>> colorDetect{ {26,159,134,128,255,255} }; //blue
									

vector<Scalar> colorDraw{ {255,0,0} }; // blue
							
vector<vector<int>> pointDraw; // {{x,y,0/1},{x,y,0/1},{x,y,0/1}....}

Mat img; // initialize an image variable

Point getContours(Mat imgDil)
{
	Mat imgGray, imgBlur, imgCanny;
	vector<vector<Point>> contours;
	Point points(0, 0);


	// Get contours
	findContours(imgDil, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> contourPoly(contours.size()); // you have to initialize the variables after findContours because contours.size() is defined only after findContours.
	vector<Rect> boundBox(contours.size());




	//find area of contours and filter unncessary contours
	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);


		if (area > 1000)
		{
			float peri = arcLength(contours[i], true); // to find the length of the perimeter of contours
			approxPolyDP(contours[i], contourPoly[i], 0.02 * peri, true); // approximates the contour with another polygon with less vertices. 

			// bounding box
			boundBox[i] = boundingRect(contourPoly[i]);

			// get coordinates of top mid of bound box
			points.x = boundBox[i].x + boundBox[i].width / 2;
			points.y = boundBox[i].y;

			drawContours(img, contourPoly, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundBox[i].tl(), boundBox[i].br(), Scalar(0, 255, 0), 5);


		}

	}
	return points;
} 


vector<vector<int>> ColorDetector(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < colorDetect.size(); i++)
	{
		Scalar lower(colorDetect[i][0], colorDetect[i][1], colorDetect[i][2]);
		Scalar upper(colorDetect[i][3], colorDetect[i][4], colorDetect[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		Point contourPoint = getContours(mask);
		if (contourPoint.x != 0 && contourPoint.y != 0)
		{
			pointDraw.push_back({ contourPoint.x,contourPoint.y,i });
		}
		
		//imshow(to_string(i), mask);
		
	}
	
	return pointDraw;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar>colorDraw) // newPoints : {{x,y,0/1},{x,y,0/1},{x,y,0/1}....}
{
	
	
	if (!newPoints.empty()) // so we don't have the vector subscript out of range issue
	{ 
		vector<Point> initial_points{ {newPoints[0][0],newPoints[0][1]} };

		for (int i = 1; i < newPoints.size(); i++)
		{


			line(img, Point(newPoints[i][0], newPoints[i][1]), Point(initial_points[i-1].x, initial_points[i-1].y), colorDraw[newPoints[i][2]], 5);
			initial_points.push_back({ newPoints[i][0], newPoints[i][1] });

		}
		

	}

	
}

int main()
{
	VideoCapture cap(0);
	
	
	while (true)
	{
		cap >> img;
		

		vector<vector<int>> newPoints = ColorDetector(img);
		drawOnCanvas(newPoints, colorDraw);

		imshow("Video", img);
		waitKey(1);

	}


	
	return 0;
}
