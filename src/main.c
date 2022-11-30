#include "xilinx_encoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dlfcn.h>
#include<unistd.h>

int ReadYUV(char *ybuf, char *uvbuf, FILE *hInputYUVFile)
{
	int ylen = 3840 * 2160;
	int uvlen = 3840 * 2160 / 2; 
	
	fread(ybuf, ylen, 1, hInputYUVFile);
	fread(uvbuf, uvlen, 1, hInputYUVFile);

	return 0;
}

typedef struct XlnxAppTimeTracker
{
    struct timespec start_time;
    struct timespec curr_time;
    struct timespec segment_time;
    int    last_displayed_frame;
} XlnxAppTimeTracker;

typedef struct {
    XlnxAppTimeTracker    enc_timer;
    size_t                out_frame_cnt;
} XlnxEncoderCtxtime;

static void xlnx_utils_set_current_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->curr_time);
}

void xlnx_utils_set_segment_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->segment_time);
}

double xlnx_utils_get_segment_time(XlnxAppTimeTracker* timer)
{
    xlnx_utils_set_current_time(timer);
    double time = (timer->curr_time.tv_sec - timer->segment_time.tv_sec) *
                  1e6 + (timer->curr_time.tv_nsec - 
                  timer->segment_time.tv_nsec) / 1e3; 
    return time / 1000000.0;
}

double xlnx_utils_get_total_time(XlnxAppTimeTracker* timer)
{
    xlnx_utils_set_current_time(timer);
    double time = (timer->curr_time.tv_sec - timer->start_time.tv_sec) *
                  1e6 + (timer->curr_time.tv_nsec - 
                  timer->start_time.tv_nsec) / 1e3; 
    return time / 1000000.0;
}

static void xlnx_enc_print_segment_fps(XlnxEncoderCtxtime *enc_ctx)
{

    double segment_time = xlnx_utils_get_segment_time(&enc_ctx->enc_timer);
    if(segment_time < 1) {
        return;
    }
	fprintf(stderr,"\r ====== Encoder Frame=%5zu Total Encoder FPS=%.03f \r", 
					enc_ctx->out_frame_cnt, (float) enc_ctx->out_frame_cnt / xlnx_utils_get_total_time(&enc_ctx->enc_timer));	
	fflush(stderr);
    enc_ctx->enc_timer.last_displayed_frame = enc_ctx->out_frame_cnt;
    xlnx_utils_set_segment_time(&enc_ctx->enc_timer);

    return;
}

void xlnx_utils_start_tracking_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->start_time);
    xlnx_utils_set_current_time(timer);
    xlnx_utils_set_segment_time(timer);
    timer->last_displayed_frame = 0;
}

void* Enc_thread_1(void *cnt)
{
	printf("Start Encoding \n");
	char* outBuf = (char*)malloc(1920*1080*4);
	int outlen =0;

	static FILE* fin = NULL;
	if (!fin) fin = fopen("./4k30.yuv", "rb");
	
	static FILE* fin2 = NULL;
	if (!fin2) fin2 = fopen("./xilinx_output_thr.265", "wb");

	char* ybuf = (char*)malloc(1920*1080*4*3/2);
	char* uvbuf = (char*)malloc(1920*1080*4*3/2);
	

	XlnxEncoderCtx enc_ctx;
	XmaEncoderProperties xma_enc_props;
	XmaFilterProperties  xma_la_props;
	
	Encoder_Init(&enc_ctx,&xma_enc_props,&xma_la_props);
	
	struct timeval start, time_end;
	int interval =0;
	
	XlnxEncoderCtxtime enc_ctxt;
    memset(&enc_ctxt, 0, sizeof(enc_ctxt));
	xlnx_utils_start_tracking_time(&enc_ctxt.enc_timer);
	
	int cn =0;
	int thr_cnt=0;
	while(1)
	{
		thr_cnt++;
		cn++;
		gettimeofday(&start, NULL);
		
		ReadYUV(ybuf, uvbuf, fin);
		Encoder_frame(&enc_ctx,ybuf,uvbuf,outBuf,&outlen);

		enc_ctxt.out_frame_cnt++;
	
		gettimeofday(&time_end, NULL);
		interval = 1000000 * (time_end.tv_sec - start.tv_sec) + (time_end.tv_usec - start.tv_usec);
		
		xlnx_enc_print_segment_fps(&enc_ctxt);
		
		usleep(35);
		if(interval <33000)
		{
			usleep(33000-interval);
		}
		if(cn>900)
		{
			fseek(fin, 0, SEEK_SET);
			cn =0;
		}
	}
	
	printf("Encoding of input stream completed \n");

	free(outBuf);
	free(ybuf);
	free(uvbuf);
    Encoder_Release(&enc_ctx,&xma_enc_props,&xma_la_props);

}

void* Enc_thread_2(void *cnt)
{
	printf("Start Encoding \n");
	char* outBuf = (char*)malloc(1920*1080*4);
	int outlen =0;

	static FILE* fin = NULL;
	if (!fin) fin = fopen("./4k30.yuv", "rb");
	
	static FILE* fin2 = NULL;
	if (!fin2) fin2 = fopen("./xilinx_output_thr.265", "wb");

	char* ybuf = (char*)malloc(1920*1080*4*3/2);
	char* uvbuf = (char*)malloc(1920*1080*4*3/2);
	

	XlnxEncoderCtx enc_ctx;
	XmaEncoderProperties xma_enc_props;
	XmaFilterProperties  xma_la_props;
	
	Encoder_Init(&enc_ctx,&xma_enc_props,&xma_la_props);

	struct timeval start, time_end;
	int interval =0;
	
	XlnxEncoderCtxtime enc_ctxt;
    memset(&enc_ctxt, 0, sizeof(enc_ctxt));
	xlnx_utils_start_tracking_time(&enc_ctxt.enc_timer);
	
	int cn =0;
	int thr_cnt=0;
	while(1)
	{
		thr_cnt++;
		cn++;
		gettimeofday(&start, NULL);
		
		ReadYUV(ybuf, uvbuf, fin);
		Encoder_frame(&enc_ctx,ybuf,uvbuf,outBuf,&outlen);

		enc_ctxt.out_frame_cnt++;
	
		gettimeofday(&time_end, NULL);
		interval = 1000000 * (time_end.tv_sec - start.tv_sec) + (time_end.tv_usec - start.tv_usec);
		
		xlnx_enc_print_segment_fps(&enc_ctxt);
		
		usleep(35);
		if(interval <33000)
		{
			usleep(33000-interval);
		}
		if(cn>900)
		{
			fseek(fin, 0, SEEK_SET);
			cn =0;
		}
		if(thr_cnt>900*4)
		{
			//break;
		}
	}
	
	printf("Encoding of input stream completed \n");

	free(outBuf);
	free(ybuf);
	free(uvbuf);
    Encoder_Release(&enc_ctx,&xma_enc_props,&xma_la_props);

}

void* Enc_thread_3(void *cnt)
{
	printf("Start Encoding \n");
	char* outBuf = (char*)malloc(1920*1080*4);
	int outlen =0;

	static FILE* fin = NULL;
	if (!fin) fin = fopen("./4k30.yuv", "rb");
	
	static FILE* fin2 = NULL;
	if (!fin2) fin2 = fopen("./xilinx_output_thr.265", "wb");

	char* ybuf = (char*)malloc(1920*1080*4*3/2);
	char* uvbuf = (char*)malloc(1920*1080*4*3/2);
	

	XlnxEncoderCtx enc_ctx;
	XmaEncoderProperties xma_enc_props;
	XmaFilterProperties  xma_la_props;
	
	Encoder_Init(&enc_ctx,&xma_enc_props,&xma_la_props);

	struct timeval start, time_end;
	int interval =0;
	
	XlnxEncoderCtxtime enc_ctxt;
    memset(&enc_ctxt, 0, sizeof(enc_ctxt));
	xlnx_utils_start_tracking_time(&enc_ctxt.enc_timer);
	
	int cn =0;
	int thr_cnt=0;
	while(1)
	{
		thr_cnt++;
		cn++;
		gettimeofday(&start, NULL);
		
		ReadYUV(ybuf, uvbuf, fin);
		Encoder_frame(&enc_ctx,ybuf,uvbuf,outBuf,&outlen);

		enc_ctxt.out_frame_cnt++;
	
		gettimeofday(&time_end, NULL);
		interval = 1000000 * (time_end.tv_sec - start.tv_sec) + (time_end.tv_usec - start.tv_usec);
		
		xlnx_enc_print_segment_fps(&enc_ctxt);
		
		usleep(35);
		if(interval <33000)
		{
			usleep(33000-interval);
		}
		if(cn>900)
		{
			fseek(fin, 0, SEEK_SET);
			cn =0;
		}
		if(thr_cnt>900*6)
		{
			//break;
		}
	}
	
	printf("Encoding of input stream completed \n");

	free(outBuf);
	free(ybuf);
	free(uvbuf);
    Encoder_Release(&enc_ctx,&xma_enc_props,&xma_la_props);

}

void* Enc_thread_4(void *cnt)
{
	printf("Start Encoding \n");
	char* outBuf = (char*)malloc(1920*1080*4);
	int outlen =0;

	static FILE* fin = NULL;
	if (!fin) fin = fopen("./4k30.yuv", "rb");
	
	static FILE* fin2 = NULL;
	if (!fin2) fin2 = fopen("./xilinx_output_thr.265", "wb");

	char* ybuf = (char*)malloc(1920*1080*4*3/2);
	char* uvbuf = (char*)malloc(1920*1080*4*3/2);
	

	XlnxEncoderCtx enc_ctx;
	XmaEncoderProperties xma_enc_props;
	XmaFilterProperties  xma_la_props;
	
	Encoder_Init(&enc_ctx,&xma_enc_props,&xma_la_props);

	struct timeval start, time_end;
	int interval =0;
	
	XlnxEncoderCtxtime enc_ctxt;
    memset(&enc_ctxt, 0, sizeof(enc_ctxt));
	xlnx_utils_start_tracking_time(&enc_ctxt.enc_timer);
	
	int cn =0;
	int thr_cnt=0;
	while(1)
	{
		thr_cnt++;
		cn++;
		gettimeofday(&start, NULL);
		
		ReadYUV(ybuf, uvbuf, fin);
		Encoder_frame(&enc_ctx,ybuf,uvbuf,outBuf,&outlen);

		enc_ctxt.out_frame_cnt++;
	
		gettimeofday(&time_end, NULL);
		interval = 1000000 * (time_end.tv_sec - start.tv_sec) + (time_end.tv_usec - start.tv_usec);
		
		xlnx_enc_print_segment_fps(&enc_ctxt);
		
		usleep(35);
		if(interval <33000)
		{
			usleep(33000-interval);
		}
		if(cn>900)
		{
			fseek(fin, 0, SEEK_SET);
			cn =0;
		}
		if(thr_cnt>900*8)
		{
			//break;
		}
	}
	
	printf("Encoding of input stream completed \n");

	free(outBuf);
	free(ybuf);
	free(uvbuf);
    Encoder_Release(&enc_ctx,&xma_enc_props,&xma_la_props);

}

#if 1 //for pthread
int main()
{
	printf("Start Encoding \n");
	
	FPGA_Init();
	
	pthread_t tid1, tid2, tid3, tid4;

	char* rev=NULL;

	pthread_attr_t attr={0};
	pthread_attr_init(&attr);
	size_t stacksize=20480;
	pthread_attr_setstacksize(&attr,stacksize);
	int i0=0,i1=1,i2=2,i3=3;
	
	pthread_create(&tid1, NULL, (void*)Enc_thread_1, (void *)i0);sleep(2);
	pthread_create(&tid2, NULL, (void*)Enc_thread_2, (void *)i1);sleep(2);
	pthread_create(&tid3, NULL, (void*)Enc_thread_3, (void *)i2);sleep(2);
	pthread_create(&tid4, NULL, (void*)Enc_thread_4, (void *)i3);sleep(2);

	pthread_attr_destroy(&attr);

	pthread_join(tid1,(void*)&rev);sleep(2);
	pthread_join(tid2,(void*)&rev);sleep(2);
	pthread_join(tid3,(void*)&rev);sleep(2);
	pthread_join(tid4,(void*)&rev);sleep(2);

    return 0;
}
#else //for process
int main()
{
	printf("Start Encoding \n");
	char* outBuf = (char*)malloc(1920*1080*4);
	int outlen =0;

	static FILE* fin = NULL;
	if (!fin) fin = fopen("./4k30.yuv", "rb");
	
	static FILE* fin2 = NULL;
	if (!fin2) fin2 = fopen("./xilinx_output_u30.265", "wb");
	
	char* ybuf = (char*)malloc(1920*1080*4*3/2);
	char* uvbuf = (char*)malloc(1920*1080*4*3/2);
	
	FPGA_Init();
	
	XlnxEncoderCtx enc_ctx;
	XmaEncoderProperties xma_enc_props;
	XmaFilterProperties  xma_la_props;
	
	Encoder_Init(&enc_ctx,&xma_enc_props,&xma_la_props);
	
	struct timeval start, end;
	int interval =0;
	int cnt=0;
	XlnxEncoderCtxtime enc_ctxt;
    memset(&enc_ctxt, 0, sizeof(enc_ctxt));
	xlnx_utils_start_tracking_time(&enc_ctxt.enc_timer);
	while(1)
	{
		
		cnt++;
		gettimeofday(&start, NULL);	
		
		ReadYUV(ybuf, uvbuf, fin);
		Encoder_frame(&enc_ctx,ybuf,uvbuf,outBuf,&outlen);
	
		gettimeofday(&end, NULL);
		interval = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
		
		if(interval <33000)
		{
			usleep(33000-interval);
		}
		if(cnt>900)
			fseek(fin, 0, SEEK_SET);
		enc_ctxt.out_frame_cnt++;
		xlnx_enc_print_segment_fps(&enc_ctxt);
	}
	
	printf("Encoding of input stream completed \n");

	free(outBuf);
	free(ybuf);
	free(uvbuf);
    Encoder_Release(&enc_ctx,&xma_enc_props,&xma_la_props);
	return 0;
}
#endif

