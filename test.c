#include <stdio.h>
#include <time.h>

 
int main ()
{
	 
	 
	unsigned int x_minutes=0;
	unsigned int x_seconds=0;
	unsigned int x_milliseconds=0;
	unsigned int totaltime=0,count_down_time_in_secs=0,time_left=0;
 
	clock_t x_startTime,x_countTime;
	count_down_time_in_secs=10;  // 1 minute is 60, 1 hour is 3600

    printf("Hello");
    x_startTime=clock();  // start clock
    time_left=count_down_time_in_secs-x_seconds;   // update timer

	while (time_left>0) 
	{
		x_countTime=clock(); // update timer difference
		x_milliseconds=x_countTime-x_startTime;
		x_seconds=(x_milliseconds/(CLOCKS_PER_SEC))-(x_minutes*60);
		x_minutes=(x_milliseconds/(CLOCKS_PER_SEC))/60;
 

	     int print = 1;
 
		time_left=count_down_time_in_secs-x_seconds; // subtract to get difference 

		if(time_left == 3 && print == 1){
		//printf( "\nYou have %d seconds left ( %d ) count down timer by TopCoder\n" ,time_left,count_down_time_in_secs);
		 print=0;
		}
	}
 

	printf( "\n\n\nTime's out\n\n\n");
 
return 0;
}
