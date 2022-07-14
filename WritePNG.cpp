#include <iostream>
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
Mat frame_01;
Mat frame_02;

int END = 0;


static void compressPicture(cv::Mat img, int presslev = 4)
{
	if (!img.data) {
		printf("invalid image!\n");
		return;
	}
	int width = img.cols;
	int height = img.rows;
	int shorter = width > height ? height : width;
	double ratio;
	switch (presslev) {
	case 1:
		ratio = 0.2; break;
	case 2:
		ratio = 0.4; break;
	case 3:
		ratio = 0.6; break;
	case 4:
		ratio = 0.8; break;
	default:
		ratio = 0.2; break;
	}

	if (shorter * ratio <= 64) {
		ratio = 64.0 / shorter;
		cv::resize(img, img, cv::Size(width * ratio, height * ratio));
		return;
	} else {
		cv::resize(img, img, cv::Size(width * ratio, height * ratio));
		return;
	}
}

int process(VideoCapture& capture) {
    string str;
    string suffix = ".png";
    unsigned int i = 0;
    
    while(1) {
        if(END) break;

        capture >> frame_01;
        
        frame_02.create(frame_01.size(), frame_01.type());
        //cout << frame_01.size() << " " << frame_01.type() << endl; // [720 x 1280] 16
	    int height = frame_02.rows,
            width = frame_02.cols,
            cn = frame_01.channels(),
            a = 256 * 0.3,
            b = 256 * 0.6,
            c = 256 * 0.1,
            d = 256 * 0.9,
            fa = (d - c) / (b - a),
            fb = (255 - d) / (255 - b);
	    for (int row = 0; row < height; row++) {
		    for (int col = 0; col < width; col++) {
			    if (cn == 1) {
				    int gray = frame_02.at<uchar>(row, col); 
				    frame_02.at<uchar>(row, col) = fa * (gray - a) + c; 
			    } else if (cn == 3) { 
                    int blue = frame_01.at<Vec3b>(row, col)[0];
                    int green = frame_01.at<Vec3b>(row, col)[1];
                    int red = frame_01.at<Vec3b>(row, col)[2];
                    if (blue< a)
                        frame_02.at<Vec3b>(row, col)[0] = (c/a)*blue;
                    else if (blue > b)
                        frame_02.at<Vec3b>(row, col)[0] = fb *(blue - b)+d;
                    else
                        frame_02.at<Vec3b>(row, col)[0] = fa *(blue - a) + c;

                    if (green < a) 
                        frame_02.at<Vec3b>(row, col)[1] = (c / a)*green;
                    else if (green > b) 
                        frame_02.at<Vec3b>(row, col)[1] = fb * (green - b) + d;
                    else
                        frame_02.at<Vec3b>(row, col)[1] = fa * (green - a) + c;

                    if (red < a) 
                        frame_02.at<Vec3b>(row, col)[2] = (c / a)*red;
                    else if (red > b)
                        frame_02.at<Vec3b>(row, col)[2] = fb * (red - b) + d;
                    else
                        frame_02.at<Vec3b>(row, col)[2] = fa * (red - a) + c;
			    }
		    }
	    }

        if (frame_01.empty())
            break;

        /*JPEG smaller than PNG */ 
        suffix = ".jpeg";
        str = to_string(i++) + suffix;
        if (i == 10)
            i = 0;


		imshow("video test", frame_01);

        compressPicture(frame_02, 1);
        imshow("video augmented", frame_02);

        //imwrite(str, frame_01);
        //imwrite(str, frame_02);

        imwrite(str, frame_02);
        waitKey(10);
    }

    return 0;
}

int main() {
    VideoCapture capture("461983258_nb2-1-64.flv");
    if(!capture.isOpened()) {
        cerr << "open error \n" << endl;
        return 1;
    }
    
    return process(capture);
}