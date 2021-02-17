#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <libavformat/avformat.h>

int max_y = 0, max_x = 0, curr_y = 0, curr_x = 0;

void draw_bmp(int duration, int framerate){
    FILE *bmpHandle;
    int width, height, offset;
    unsigned char color;
    clock_t begin, end;
    for (int i = 1; i < duration * framerate; ++i){
        begin = clock();
        getmaxyx(stdscr, max_y, max_x);
        if (max_y != curr_y || max_x != curr_x){
            curr_y = max_y;
            curr_x = max_x;
            clear();
        }
        char buf[0x100];
        snprintf(buf, sizeof(buf), "%s/temp-termplayer/bitmap%d.bmp", getenv("HOME"), i);
        bmpHandle = fopen(buf,"r");
        
        fseek(bmpHandle,0x0A,SEEK_SET);
        fread(&offset,4,1,bmpHandle);
        fseek(bmpHandle,0x12,SEEK_SET);
        fread(&width,4,1,bmpHandle);
        fread(&height,4,1,bmpHandle);

        int true_width = max_y * 3;
        int jump_width = width/true_width;
        int jump_height = height/max_y; 

        offset += width * height - width;
        for(int j = 0; j < max_y; ++j){
            for(int k = 0; k < true_width; ++k){
                fseek(bmpHandle,offset,SEEK_SET);
                fread(&color,1,1,bmpHandle);
                move(j,k+(max_x-true_width)/2);
                if (color > 176) addch('a');
                else if (color > 128) addch(':');
                else if (color > 0)  addch('.');
                else addch(' ');
                offset += jump_width;
            }
            offset += width - (jump_width * true_width);
            offset -= width * jump_height;
        }
        fclose(bmpHandle);
        refresh();
        end = clock();
        usleep(1000000/framerate - (end - begin));
    }
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Provide a video please.\n");
        return 0;
    }

    av_register_all();
    AVFormatContext *pFormatCtx = NULL;
    if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0)
        return -1;
    
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        return -1;
    
    int duration = (int)(pFormatCtx->duration/1000000);
    int framerate = pFormatCtx->streams[0]->r_frame_rate.num / pFormatCtx->streams[0]->r_frame_rate.den;
    char buf[0x100];
    system("mkdir ~/temp-termplayer/");
    snprintf(buf, sizeof(buf), "ffmpeg -i %s -pix_fmt bgr8 -y ~/temp-termplayer/bitmap%%01d.bmp", argv[1]);
    system(buf);
    snprintf(buf, sizeof(buf), "ffmpeg -i %s -y audio.wav", argv[1]);
    system(buf);
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    intrflush(stdscr, FALSE);
    curs_set(FALSE);
    nonl();
    clear();
    refresh();

    int pid = fork();
    if (pid == 0){
        system("powershell.exe -c \"mv audio.wav \\$env:temp\\audio.wav ; (New-Object Media.SoundPlayer \\$env:temp\\audio.wav).PlaySync();\" >/dev/null 2>/dev/null");
    } else {
        draw_bmp(duration, framerate);
    }
    endwin();
    system("rm -r ~/temp-termplayer");
	return 0;
}