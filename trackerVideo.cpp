#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <unistd.h>

using namespace cv;

std::vector<Point> getEnglobingRectangle(Mat frame){

    Size s = frame.size();
    int iMax = s.height;
    int jMax = s.width;
    
    int minI = s.width+1, maxI = -1, minJ=s.height+1, maxJ=-1;
    
    for (int i = 30 ; i<iMax ; i++){
        for(int j = 25 ; j<jMax ; j++){
            if(frame.at<uchar>(j,i) > 200){
                 if(i<minI){
                    minI = i ;
                    }
                 if(i>maxI){
                    maxI = i ;   
                    }
                if(j<minJ){
                    minJ = j ;
                    }
                 if(j>maxJ){
                    maxJ = j ;
                }
            }
        }
    }
    
    std::vector<Point> result;
    result.push_back(Point(minI,minJ));
    result.push_back(Point(maxI,maxJ));
    
    return result;
}



int main( int argc, char** argv )
{
  Mat currFrame, frameVide, edges, dst, dstKp, leMask, hsv, RoI, hsvRoI, hsvHist, hist, dst2, backproj, finaleFrame;

  VideoCapture *laVid = new VideoCapture();
  
//        SimpleBlobDetector detector;
//        std::vector<KeyPoint> keypoints;




 
 
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
//        namedWindow( "Display Difference", CV_WINDOW_AUTOSIZE );
//        imshow( "Display Difference", diffFrame );
 	    //waitKey(framePeriod*100); 
 	    
 	    
 	    cvtColor(diffFrame,diffFrame, CV_BGR2GRAY);
 	    // DeNoising
 	        //fastNlMeansDenoisingColored(diffFrame, dst, 50.00, 10.0);
        GaussianBlur(diffFrame,dst,gaussianSize,10);
        threshold(dst,dst,30,255,0);
        
//        namedWindow( "Display Denoised Difference", CV_WINDOW_AUTOSIZE );
//        imshow( "Display Denoised Difference", dst );
 	     	    	
// 	     	    	detector->detect(dst,keypoints,leMask);
 	     	    	
 //	     	    	int minX = 9999, maxX = -1 , minY = 9999 , maxY = -1;
 	//     	    	for (int i=0; i<keypoints.size(); i++){
      //                 float X = keypoints[i].pt.x; 
        //               float Y = keypoints[i].pt.y;
          //             if (keypoints[i].pt.x < minX)
            //               minX = keypoints[i].pt.x;
              //         if (keypoints[i].pt.x > maxX)
                //           maxX = keypoints[i].pt.x;
                  //     if (keypoints[i].pt.y < minY)
                    //       minY = keypoints[i].pt.y;
                      // if (keypoints[i].pt.y > maxY)
                        //   maxY = keypoints[i].pt.y;
                    //}
                    
                 
 	     	    	
 	     	    	
 	     	    	
        	    	
 	     	    	dst.copyTo(dstKp);
 	     	    	
 	     	    	
 	     	  
 	     	        std::vector<Point> CoordPoints = getEnglobingRectangle(dstKp);
// 	     	    	cvtColor(dstKp,dstKp, CV_GRAY2BGR);
 	     	      	rectangle(dstKp,CoordPoints[0],CoordPoints[1], Scalar(255,0,0));
 	                namedWindow( "Display Denoised DifferenceKP", CV_WINDOW_AUTOSIZE );
                    imshow( "Display Denoised DifferenceKP", dstKp );
         	  
 	     	  //if (CoordPoints[0].x != s.width+1 && CoordPoints[0].y != s.height+1 && 
 	     	    //    CoordPoints[1].x != -1 && CoordPoints[1].y != -1 && 
 	     	      //  CoordPoints[1].x - CoordPoints[0].x !=0 && CoordPoints[1].y - CoordPoints[0].y !=0 ){ // Seulement si Rectangle OK
 	     	  
 	     	    //waitKey(0);
 	     	  
 	     	     // int  width = CoordPoints[1].x - CoordPoints[0].x ;
 	     	     // int  height = CoordPoints[1].y - CoordPoints[0].y ;
 	     	      
 	     	    //Rect *trackingWindow = new Rect(CoordPoints[0].y, CoordPoints[0].x, width, height);
 	     	    
 	     	    
 	     	    //Size *test = new Size(width,height);
 	     	    
 	     	    //printf("Rect : \nX : %d\nY : %d\nWidth : %d\nHeight : %d\n\n", CoordPoints[0].x, CoordPoints[0].y, width, height);
 	     	    //Mat RoI(*test,1);
 	     	    

 	     	   // RoI(Rect(CoordPoints[0].x, CoordPoints[0].y, width, height));
 	     	   // dst.copyTo(RoI);
 	     	   // cvtColor(RoI, hsvRoI, COLOR_BGR2HSV);
 	     	    
 	     	   // int bins[] = { 50 };
 	     	   // float hrange[] = {0.0 , 256.0};
 	     	   // const float* phranges = hrange;
 	     	   // const float* ranges{hrange};
 	     	   // int channels[] = {0,1};
 	     	    
 	     	   // calcHist(&hsvRoI, 1, channels, Mat(), hist, 1, bins, &ranges, true, false);
 	     	   // normalize(hist, hist, 0, 255, NORM_MINMAX);
 	     	    
 	     	    
 	     	    
 	     	    
 	     	    
 	     	    
 	     	    // cvtColor(dst,dst,COLOR_GRAY2BGR);
 	     	    // cvtColor(dst,hsv,COLOR_BGR2HSV);
 	     	    
 	     	    // calcBackProject(&hsv, 1,0, hist, backproj, &phranges);
 	     	    
 	     	    // meanShift(backproj,*trackingWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1 ));
 	     	    
 	     	    // Point* Point1 = new Point(trackingWindow->x,trackingWindow->y);
 	     	    // Point* Point2 = new Point(trackingWindow->x+trackingWindow->width,trackingWindow->y+trackingWindow->height);
 	     	    
 	     	    
 	     	    // dst.copyTo(finaleFrame);
 	     	    // rectangle(finaleFrame,*trackingWindow,Scalar(255,0,0));
        
                
        //namedWindow( "Resultat Meanshift", CV_WINDOW_AUTOSIZE );
        //imshow( "Resultat Meanshift", finaleFrame );
            //}
 	     	    	  	
        
 	     	    	
 	     	    	
 	     	    	 
 	}

 
  namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
  imshow( "Display Image", currFrame );

  printf("Frame %d \n" , i);
  
  waitKey(framePeriod*1000);
}


  return 0;
}


