#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "redpitaya/rp.h"
#include "common.h"

int main (int argc, char **argv) {
    uint32_t start1=0, end1=8191;
    uint32_t start2=8192, end2=16383;
    int version = 1;
    int nCount;
//    uint32_t size = 8191;
    uint32_t buff_size = 8192;
    char *fpname;
    int16_t buffer[buff_size+1];
//    bool debug = true;
    bool debug = false;
    int decimation;

    rp_channel_t channel1=RP_CH_1;
    rp_channel_t channel2=RP_CH_2;
    FILE *fp;

// percentage can be provided as an argument
    if (argc ==  4) {
        decimation = atoi(argv[1]);
        fpname = argv[2];
        nCount = atoi(argv[3]);
    } else {
        printf("%s decimation outputFileName count\n",argv[0]);
        return -1;
    }
    fp = fopen(fpname, "wb");
    if (fp == NULL) {
        printf("sorry can't open %s\n", fpname);
        return 1;
    }
    // Initialization of API
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return EXIT_FAILURE;
    }
    // write header for binary output file
    fwrite(&version,sizeof(version),1,fp);
    fwrite(&decimation,sizeof(decimation),1,fp);
    fwrite(&nCount,sizeof(nCount),1,fp);
    
    printf("version size %d\n", sizeof(version));
    printf("decimation size %d\n", sizeof(decimation));
    printf("nCount size %d\n", sizeof(nCount));
    printf("buff size: %d\n", buff_size);
    printf("buffer size: %d\n", sizeof(buffer));
    printf("sizeof(int16_t) %d\n", sizeof(int16_t));
    printf("version: %d\n", version);
    printf("decimation: %d\n", decimation);
    printf("nCount: %d\n", nCount);


    uint32_t wPos;
    ECHECK(rp_AcqReset());
    ECHECK(rp_AcqSetDecimation(decimation));
    ECHECK(rp_AcqStart());
    int region = 0;
    float percent = 0;
    uint32_t counter = 0;
    while(rp_AcqGetWritePointer(&wPos) == RP_OK){
        if(region == 0 ){
            if(wPos > 8191){
                // channel 1
                if(rp_AcqGetDataPosRaw(channel1, start1, end1, buffer, &buff_size)!=RP_OK){
                    fprintf(stderr, "reading 1 failed!\nwPos: %d\nstartPos %d\nendPos %d\nerror Code %d\n",wPos,start1,end1,
rp_AcqGetDataPosRaw(channel1, start1, end1, buffer, &buff_size));
                    return EXIT_FAILURE;
                }
                fwrite(buffer,sizeof(int16_t),buff_size,fp);
                // channel 2
                if(rp_AcqGetDataPosRaw(channel2, start1, end1, buffer, &buff_size)!=RP_OK){
                    fprintf(stderr, "reading 2 failed!\n");
                    return EXIT_FAILURE;
                }
                fwrite(buffer,sizeof(int16_t),buff_size,fp);
                for (int i=0; i<8; i++) {
                   if (percent > (i*(100.0/8))) {
                      rp_DpinSetState(i+RP_LED0, RP_HIGH);
                   } else {
                      rp_DpinSetState(i+RP_LED0, RP_LOW);
                   }
                }
                percent = percent + 12.5;
                if(percent>101.0){
                   percent = 0;
                }
                printf("count: %d\n",counter);
                region = 1;
                counter ++;
            }
        }else{
            if(wPos < 8192){
                // channel 1
                if(rp_AcqGetDataPosRaw(channel1, start2, end2, buffer, &buff_size)!=RP_OK){
                    fprintf(stderr, "reading 3 failed!\n");
                    return EXIT_FAILURE;
                }
                fwrite(buffer,sizeof(int16_t),buff_size,fp);
                // channel 2
                if(rp_AcqGetDataPosRaw(channel2, start2, end2, buffer, &buff_size)!=RP_OK){
                    fprintf(stderr, "reading 4 failed!\n");
                    return EXIT_FAILURE;
                }
                fwrite(buffer,sizeof(int16_t),buff_size,fp);
                if (debug){
                    printf("%d\n",counter);
                }
                region = 0;
                counter ++;
                if(counter > nCount){
                    break;
                }
            }
        }
   }
    // Releasing resources
    rp_Release();
    fclose(fp);

    return EXIT_SUCCESS;
}

