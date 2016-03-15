#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <windows.h>
#include <stdio.h>
#include <fstream>

using namespace std;

int main(int argc, char** argv){
	if (argc == 1) return 1;

	const string path(argv[1]);
	const char *windowName = "leavesDetection";
	string outStreamFileName = path + "_info.dat";
	string outStreamBadFileName = path + "_bad.txt";
	
	ofstream outStream;
	outStream.open(outStreamFileName.c_str(), std::ofstream::out);

	ofstream outStreamBad;
	outStreamBad.open(outStreamBadFileName.c_str(), std::ofstream::out);

	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	int imax;
	double maxarea = -1, area;
	int contourCount;

	cv::namedWindow(windowName, cv::WINDOW_NORMAL);

	char search_path[200], file[300];
	sprintf(search_path, "%s/*.*", argv[1]);
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				sprintf(file, "%s/%s", argv[1], fd.cFileName);
				printf("%s\n", file);
				

				cv::Mat res;
				cv::Mat image = cv::imread(file, cv::IMREAD_GRAYSCALE);
				res = image.clone();

				cv::GaussianBlur(image, image, cv::Size(15, 15), 0);
				cv::threshold(image, image, 240, 255, cv::THRESH_BINARY_INV);
				cv::Canny(image, image, 80, 100);
				cv::findContours(image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

				maxarea = -1;
				imax = -1;
				contourCount = contours.size();
				for (int j = 0; j < contourCount; j++){
					area = cv::contourArea(contours[j]);
					if (area > maxarea){
						maxarea = area;
						imax = j;
					}
				}

				if (area > 0){
					cv::Rect bounding_rect = cv::boundingRect(contours[imax]);
					cv::rectangle(res, bounding_rect, cv::Scalar(0, 255, 0), 4);
					cv::imshow(windowName, res);
					cv::resizeWindow(windowName, 400, 300);
					char c = cvWaitKey(0);

					if (c == 'n'){
						outStreamBad << file << std::endl;
					}
					else{
						outStream << file << " 1 " << bounding_rect.x << " " << bounding_rect.y << " " << bounding_rect.width << " " << bounding_rect.height << std::endl;
					}
					
				}
				//cv::drawContours(res, contours, imax, cv::Scalar(0, 0, 255));
				else{
					cv::imshow(windowName, res);
					cv::resizeWindow(windowName, 400, 300);
					cv::waitKey(0);
					outStreamBad << file << std::endl;
				}
				image.release();
				res.release();
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	cv::destroyWindow(windowName);
	outStream.close();
	outStreamBad.close();
	
	return 0;
}

