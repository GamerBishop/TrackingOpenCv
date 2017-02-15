#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <unistd.h>

using namespace cv;

std::vector<Point> getEnglobingRectangle(Mat frame){

    Size s = frame.size();
    int iMax = s.height;
    int jMax = s.width;
    
    int minI = 9999, maxI = -9999, minJ=9999, maxJ=-9999;
    
    
    
    for (int i = 0 ; i<iMax ; i++){
        for(int j = 30 ; j<jMax ; j++){
        if(frame.at<uchar>(j,i) != 0)
         if(i<minI)
            minI = i ;
         if(i>maxI)
            maxI = i ;   
        if(j<minJ)
            minJ = j ;
         if(j>maxJ)
            maxJ = j ;
        }
    }
    
    std::vector<Point> result;
    result.push_back(Point(minJ,minI));
    result.push_back(Point(maxJ,maxI));
    
    return result;
}



int main( int argc, char** argv )
{
  Mat currFrame, frameVide, edges, dst, dstKp, leMask;
  VideoCapture *laVid = new VideoCapture();
  
//        SimpleBlobDetector detector;
        std::vector<KeyPoint> keypoints;

// BLOB PARAMETRISATION
// Setup SimpleBlobDetector parameters.
SimpleBlobDetector::Params params;

params.minDistBetweenBlobs = 50.0f;
 
// Change thresholds
params.minThreshold = 30;
params.maxThreshold = 255;
 
// Filter by Area.
params.filterByArea = true;
params.minArea = 200;
 
 // Filter by Color
 params.filterByColor = true;
 params.blobColor = 255;
 
 
// Filter by Circularity
params.filterByCircularity = false;
params.minCircularity = 0.1;
 
// Filter by Convexity
params.filterByConvexity = false;
params.minConvexity = 0.87;
 
// Filter by Inertia
params.filterByInertia = false;
params.minInertiaRatio = 0.01;
 
  // Set up detector with params
  Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
 


 
 
 if(!laVid->open(argv[1]))
 printf("Problème lors de l'ouverture du fichier \n");
 
 if(laVid->isOpened())
 printf("Ouverture Vidéo OK \n");
  
  int maxFrame = laVid->get(CV_CAP_PROP_FRAME_COUNT);
  float fps = laVid->get(CV_CAP_PROP_FPS);
  float framePeriod = 1.0/fps;

  printf("Frameperiod %f \n" , framePeriod);

  
  for (int i = 0 ; i < maxFrame ; i++){
    	
  
  laVid->read(currFrame) ;
  cv::Size s = currFrame.size();
  Mat diffFrame(s,currFrame.type());
  currFrame.copyTo(dst);
  cv::Size gaussianSize(5,5);
 
  
                   
 
 
 	if (i==0){
     	currFrame.copyTo(frameVide);
     	std::vector<int> paramImWrite;
     	paramImWrite.push_back(CV_IMWRITE_JPEG_QUALITY);
     	paramImWrite.push_back(100); 	
     	imwrite("frameArrierePlan.jpg", frameVide, paramImWrite);
     	
     	 // Mask Initialization
 	     	    	cvtColor(currFrame,leMask, CV_BGR2GRAY);
 	    for (int i = 0 ; i <s.width ; i++){
 	     	    	    for(int j = 0 ; j< s.height ; j++){
 	     	    	    if (j<50)
 	     	    	        leMask.at<uchar>(j,i) = 0;
     	    	        else
     	    	            leMask.at<uchar>(j,i) = 255;
 	     	    	    }
 	     	    	}
  	}
 	else{
 	    absdiff(currFrame,frameVide, diffFrame);
        namedWindow( "Display Difference", CV_WINDOW_AUTOSIZE );
        imshow( "Display Difference", diffFrame );
 	    //waitKey(framePeriod*100); 
 	    
 	    
 	    cvtColor(diffFrame,diffFrame, CV_BGR2GRAY);
 	    // DeNoising
 	        //fastNlMeansDenoisingColored(diffFrame, dst, 50.00, 10.0);
        GaussianBlur(diffFrame,dst,gaussianSize,5);
        threshold(dst,dst,30,255,0);
        
        namedWindow( "Display Denoised Difference", CV_WINDOW_AUTOSIZE );
        imshow( "Display Denoised Difference", dst );
 	     	    	
 	     	    	detector->detect(dst,keypoints,leMask);
 	     	    	
 	     	    	int minX = 9999, maxX = -1 , minY = 9999 , maxY = -1;
 	     	    	for (int i=0; i<keypoints.size(); i++){
                       float X = keypoints[i].pt.x; 
                       float Y = keypoints[i].pt.y;
                       if (keypoints[i].pt.x < minX)
                           minX = keypoints[i].pt.x;
                       if (keypoints[i].pt.x > maxX)
                           maxX = keypoints[i].pt.x;
                       if (keypoints[i].pt.y < minY)
                           minY = keypoints[i].pt.y;
                       if (keypoints[i].pt.y > maxY)
                           maxY = keypoints[i].pt.y;
                    }
                    
                    
 	     	    	
 	     	    	
 	     	    	
        	    	
 	     	    	dst.copyTo(dstKp);
 	     	    	
 	     	    	cvtColor(dstKp,dstKp, CV_GRAY2BGR);
 	     	    	
 	     	    	
 	     	    	rectangle(dstKp, Point(minX,minY), Point(maxX,maxY), Scalar(0,0,255));
 	     	    	//drawKeypoints(dst,keypoints,dstKp, Scalar(255,0,0),DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
 	     	    	

 	     	    	//std::vector<Point> CoordPoints = getEnglobingRectangle(dstKp);
 	     	    	//rectangle(dstKp,CoordPoints[0],CoordPoints[1], Scalar(255,0,0));
 	     	    	
        namedWindow( "Display Denoised DifferenceKP", CV_WINDOW_AUTOSIZE );
        imshow( "Display Denoised DifferenceKP", dstKp );
 	     	    	
 	     	    	
 	     	    	 
 	}

 
  namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
  imshow( "Display Image", currFrame );

  printf("Frame %d \n" , i);
  
  waitKey(framePeriod*1000);
}


  return 0;
}


