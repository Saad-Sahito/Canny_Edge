/**
*/
#include <ctime>
#include <iostream>
#include <unistd.h>
#include "opencv2/opencv.hpp"
#include "canny_util.h"
#include <sys/time.h>

using namespace std;
using namespace cv;

/* Possible options: 320x240, 640x480, 1024x768, 1280x1040, and so on. */
/* Pi Camera MAX resolution: 2592x1944 */

#define WIDTH 640
#define HEIGHT 480
#define NFRAME 1.0
  
int main(int argc, char **argv)
{


 
   char* dirfilename;        /* Name of the output gradient direction image */
   char outfilename[128];    /* Name of the output "edge" image */
   char composedfname[128];  /* Name of the output "direction" image */
   unsigned char *image;     /* The input image */
   unsigned char *edge;      /* The output edge image */
   int rows, cols;           /* The dimensions of the image. */
   float sigma,              /* Standard deviation of the gaussian kernel. */
	 tlow,               /* Fraction of the high threshold in hysteresis. */
	 thigh;              /* High hysteresis threshold control. The actual
			        threshold is the (100 * thigh) percentage point
			        in the histogram of the magnitude of the
			        gradient image that passes non-maximal
			        suppression. */

   
   /****************************************************************************
   * Get the command line arguments.
   ****************************************************************************/
     if(argc < 4){
   fprintf(stderr,"\n<USAGE> %s sigma tlow thigh [writedirim]\n",argv[0]);
      fprintf(stderr,"      sigma:      Standard deviation of the gaussian");
      fprintf(stderr," blur kernel.\n");
      fprintf(stderr,"      tlow:       Fraction (0.0-1.0) of the high ");
      fprintf(stderr,"edge strength threshold.\n");
      fprintf(stderr,"      thigh:      Fraction (0.0-1.0) of the distribution");
      fprintf(stderr," of non-zero edge\n                  strengths for ");
      fprintf(stderr,"hysteresis. The fraction is used to compute\n");
      fprintf(stderr,"                  the high edge strength threshold.\n");
      fprintf(stderr,"      writedirim: Optional argument to output ");
      fprintf(stderr,"a floating point");
      fprintf(stderr," direction image.\n\n");
      exit(1);
   }

   sigma = atof(argv[1]);
   tlow = atof(argv[2]);
   thigh = atof(argv[3]);
   rows = HEIGHT;
   cols = WIDTH;

   if(argc == 5) dirfilename = (char *) "dummy";
	 else dirfilename = NULL;

   VideoCapture cap;
   // open the default camera (/dev/video0)
   // Check VideoCapture documentation for more details
   if(!cap.open(0)){
				cout<<"Failed to open /dev/video0"<<endl;
        return 0;
	 }
	 cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
   cap.set(CV_CAP_PROP_FRAME_HEIGHT,HEIGHT);

	 Mat frame, grayframe;

   printf("[INFO] (On the pop-up window) Press ESC to start Canny edge detection...\n");
int i =0;
double TFRAME = 0.000;
Mat grayframes[150];
struct timeval start, stop;
   double WallTime, WallTime_c,WallTime_p,WallTime_s,TotalWall;
     clock_t begin, mid,mid_end, end;
   double time_elapsed, time_capture, time_process, time_save, totalCPUtime;

   for(;;)
   {


			cap >> frame;
	 		if( frame.empty() ) break; // end of video stream
	 		imshow("[RAW] this is you, smile! :)", frame);
	 		if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
   }

   for(;;){
      if( frame.empty() ) break;
   imshow("[READY] Canny edge started!",frame);
   i++;
  
   begin = clock();
   //capture
  gettimeofday(&start, NULL);
	cap >> frame; 
	 mid = clock();
gettimeofday(&stop, NULL);
WallTime_c = (stop.tv_sec*1000000 + stop.tv_usec)-(start.tv_sec*1000000 + start.tv_usec);
WallTime_c = WallTime_c/1000000;
printf("Wall Time_c is: %lf\n",WallTime_c);

	 gettimeofday(&start, NULL);
	 cvtColor(frame, grayframe, CV_BGR2GRAY);
	 grayframes[i] = grayframe;
	 image = grayframe.data;

   /****************************************************************************
   * Perform the edge detection. All of the work takes place here.
   ****************************************************************************/
   if(VERBOSE) printf("Starting Canny edge detection.\n");
   if(dirfilename != NULL){
      sprintf(composedfname, "frame%03d.fim",
      i);
      dirfilename = composedfname;
   }
   canny(image, rows, cols, sigma, tlow, thigh, &edge, dirfilename);
mid_end = clock();
gettimeofday(&stop, NULL);
WallTime_p = (stop.tv_sec*1000000 + stop.tv_usec)-(start.tv_sec*1000000 + start.tv_usec);
WallTime_p = WallTime_p/1000000;
printf("Wall Time_p is: %lf\n",WallTime_p);
gettimeofday(&start, NULL);
   /****************************************************************************
   * Write out the edge image to a file.
   ****************************************************************************/
   sprintf(outfilename, "frame%03d.pgm", i);
   if(VERBOSE) printf("Writing the edge iname in the file %s.\n", outfilename);
   if(write_pgm_image(outfilename, edge, rows, cols, NULL, 255) == 0){
      fprintf(stderr, "Error writing the edge image, %s.\n", outfilename);
      exit(1);
   }
   end = clock();
gettimeofday(&stop, NULL);
WallTime_s = (stop.tv_sec*1000000 + stop.tv_usec)-(start.tv_sec*1000000 + start.tv_usec);
WallTime_s = WallTime_s/1000000;
printf("Wall Time_s is: %lf\n",WallTime_s);
WallTime = WallTime_c+WallTime_p+WallTime_s;
printf("Wall Time_total is: %lf\n",WallTime);
TotalWall = WallTime + TotalWall;

   time_elapsed = (double) (end - begin) / CLOCKS_PER_SEC;
   time_capture = (double) (mid - begin) / CLOCKS_PER_SEC;
   time_process = (double) (mid_end - mid) / CLOCKS_PER_SEC;
time_save = (double) (end - mid_end) / CLOCKS_PER_SEC;
	 totalCPUtime = totalCPUtime + time_elapsed;
   printf("Elapsed time for capturing+processing+save one frame: %lf + %lf + %lf => %lf seconds\n", time_capture, time_process,time_save, time_elapsed);
   printf("FPS: %01lf\n", NFRAME/WallTime);
TFRAME = TFRAME + NFRAME/WallTime;


if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
}
  
//printf("[INFO] (On the pop-up window) Press ESC to terminate the program...\n");
	/* for(;;){for(int k = 0 ;k++;i){
                 imshow("[GRAYSCALE] this is you, smile! :)", grayframes[1]);
		 grayframe.data = edge;
		 imshow("[EDGE] this is you, smile! :)", grayframe);
keys = SDL_GetKeyState(NULL); 
		 pollKeys(keys);
		 if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
	 }*/

    //free resrources    
//		grayframe.release();
//    delete image;
printf("Average FPS is: %01lf\n",TFRAME/(i));
printf("Average WallTime is: %lf \n",TotalWall/(i));
printf("Average CPU Time is: %lf \n",totalCPUtime/(i));
return 0;
}
