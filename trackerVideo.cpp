#include <stdio.h>
#include <unistd.h>
// Includes OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui_c.h>
// Includes OpenAL
#include <AL/al.h>
#include <AL/alc.h>

using namespace cv;


struct MoveCoordinates{
    Point centreGravite;
    int width;
    int height;
};






// Fonction permettant de récupérer les coordonnées du point supérieur gauche 
// et du point inférieur droit du rectangle englobant l'objet en mouvement (en blanc)
std::vector<Point> getEnglobingRectangle(Mat frame){
    Size s = frame.size();
    int iMax = s.height;
    int jMax = s.width;
    
    int minI = s.width+1, maxI = -1, minJ=s.height+1, maxJ=-1;
    
    for (int i = 30 ; i<iMax ; i++){
        for(int j = 25 ; j<jMax ; j++){
            if(frame.at<uchar>(j,i) > 30){
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

    //Déclaration des objets
    Mat currFrame, frameVide, dst, dstKp, leMask, hsv, RoI, hsvRoI, hsvHist, hist, dst2, backproj, finaleFrame;
    std::vector<MoveCoordinates> *posSauvegarde = new std::vector<MoveCoordinates>();
    MoveCoordinates tempMoveC;
    
    // On récuoère la vidéo
    VideoCapture *laVid = new VideoCapture();

    if(!laVid->open(argv[1]))
        printf("Problème lors de l'ouverture du fichier \n");

    if(laVid->isOpened())
        printf("Ouverture Vidéo OK \n");
    // Les infos relatives à la vidéo
    int maxFrame = laVid->get(CV_CAP_PROP_FRAME_COUNT);
    float fps = laVid->get(CV_CAP_PROP_FPS);
    float framePeriod = 1.0/fps;
    printf("Frameperiod %f \n" , framePeriod);


    for (int i = 0 ; i < maxFrame ; i++){ // Parcours des Frames

        // Lecture de la Frame
        laVid->read(currFrame) ;
        cv::Size s = currFrame.size();

        //
        Mat diffFrame(s,currFrame.type());
        currFrame.copyTo(dst);
        cv::Size gaussianSize(5,5);


        if (i==0){ // On enregistre la première frame dans la variable frameVide
            currFrame.copyTo(frameVide);
            std::vector<int> paramImWrite;
            paramImWrite.push_back(CV_IMWRITE_JPEG_QUALITY);
            paramImWrite.push_back(100);
            imwrite("frameArrierePlan.jpg", frameVide, paramImWrite);

            // Mask Initialization - Utilisé pour ignorer les modifications du timestamp de la vidéo
            cvtColor(currFrame,leMask, CV_BGR2GRAY);
            for (int i = 0 ; i <s.width ; i++){
                for(int j = 0 ; j< s.height ; j++){
                    if (j<50)
                        leMask.at<uchar>(j,i) = 0;
                    else
                        leMask.at<uchar>(j,i) = 255;
                }
            }
        }else{
            absdiff(currFrame,frameVide, diffFrame);
            //namedWindow( "Display Difference", CV_WINDOW_AUTOSIZE );
            //imshow( "Display Difference", diffFrame );

            cvtColor(diffFrame,diffFrame, CV_BGR2GRAY);

            // DeNoising
            //fastNlMeansDenoisingColored(diffFrame, dst, 50.00, 10.0);
            GaussianBlur(diffFrame,dst,gaussianSize,10);
            threshold(dst,dst,30,255,0);
            //namedWindow( "Display Denoised Difference", CV_WINDOW_AUTOSIZE );
            //imshow( "Display Denoised Difference", dst );


            // On récupère le rectangle englobant et r
            dst.copyTo(dstKp);
            std::vector<Point> CoordPoints = getEnglobingRectangle(dstKp);
            //printf("Rect : \nX0 : %d\nY0 : %d\nX1 : %d\nY1 : %d\n\n", CoordPoints[0].x, CoordPoints[0].y, CoordPoints[1].x, CoordPoints[1].y);

            //cvtColor(dstKp,dstKp, CV_GRAY2BGR);
            rectangle(dstKp,CoordPoints[0],CoordPoints[1], Scalar(255,0,0));
            namedWindow( "Display Denoised DifferenceKP", CV_WINDOW_AUTOSIZE );
            imshow( "Display Denoised DifferenceKP", dstKp );


            if (  CoordPoints[0].x != s.width+1 &&
                  CoordPoints[0].y != s.height+1 &&
                  CoordPoints[1].x != -1 && 
                  CoordPoints[1].y != -1 &&
                  CoordPoints[1].x - CoordPoints[0].x >=0 &&
                  CoordPoints[1].y - CoordPoints[0].y >=0 ){ // Seulement si Rectangle OK

                
                int  width = CoordPoints[1].x - CoordPoints[0].x ;
                int  height = CoordPoints[1].y - CoordPoints[0].y ;

                // Enregistrement des coordonnées du rectangle
                tempMoveC.centreGravite = Point(CoordPoints[0].x+height/2,CoordPoints[0].y+height/2);
                tempMoveC.width=width;
                tempMoveC.height=height;

                posSauvegarde->push_back(tempMoveC);


            }else{
                // Enregistrement de coordonnées bidons
                tempMoveC.centreGravite = Point(-1,-1);
                tempMoveC.width=-1;
                tempMoveC.height=-1;

                posSauvegarde->push_back(tempMoveC);

                }
            }

            namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
            imshow( "Display Image", currFrame );

            printf("Frame %d \n" , i);
            waitKey(1);
//            waitKey(framePeriod*1000);
        
    }


    return 0;
}


