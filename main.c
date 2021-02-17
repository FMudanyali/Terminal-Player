#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <ncurses.h>
#include <libavformat/avformat.h>

int max_y = 0, max_x = 0, curr_y = 0, curr_x = 0;

typedef struct {
    unsigned short red:3;
    unsigned short green:3;
    unsigned short blue:2;
} color8bpp;

void get_color(unsigned char red, unsigned char green, unsigned char blue){
    short has_red = 0, has_green = 0, has_blue = 0;
    if(red > 3){
        has_red = 4;
    } if(green > 3){
        has_green = 2;
    } if(blue > 1){
        has_blue = 1;
    }
    attron(COLOR_PAIR(has_red + has_green + has_blue));
}

void draw_bmp(int duration, int framerate, char *color){
    FILE *bmpHandle;
    int width, height, offset;
    color8bpp pixel;
    unsigned short brightness;
    struct timeval tv;
    unsigned long begin, end;
    unsigned short use_colors = 0;
    if(!strcmp(color, "color")) use_colors = 1;
    else if(!strcmp(color, "blue")) attron(COLOR_PAIR(1));
    else if(!strcmp(color, "green")) attron(COLOR_PAIR(2));
    else if(!strcmp(color, "cyan")) attron(COLOR_PAIR(3));
    else if(!strcmp(color, "red")) attron(COLOR_PAIR(4));
    else if(!strcmp(color, "magenta")) attron(COLOR_PAIR(5));
    else if(!strcmp(color, "yellow")) attron(COLOR_PAIR(6));
    for (int i = 1; i < duration * framerate; ++i){
        gettimeofday(&tv,NULL);
        begin = 1000000 * tv.tv_sec + tv.tv_usec;
        getmaxyx(stdscr, max_y, max_x);
        if (max_y != curr_y || max_x != curr_x){
            curr_y = max_y;
            curr_x = max_x;
            clear();
        }
        char buf[0x100];
        #ifdef WSL
        snprintf(buf, sizeof(buf), "%s/temp-termplayer/bitmap%d.bmp", getenv("HOME"), i);
        #else
        snprintf(buf, sizeof(buf), "temp-termplayer/bitmap%d.bmp", i);
        #endif
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
                fread(&pixel,1,1,bmpHandle);
                if(use_colors) get_color(pixel.red,pixel.green,pixel.blue);
                brightness = (pixel.red<<5)|(pixel.green<<3)|pixel.blue;
                move(j,k+(max_x-true_width)/2);
                if (brightness > 148) addch('a');
                else if (brightness > 96) addch(':');
                else if (brightness > 0)  addch('.');
                else addch(' ');
                offset += jump_width;
            }
            offset += width - (jump_width * true_width);
            offset -= width * jump_height;
        }
        fclose(bmpHandle);
        refresh();
        gettimeofday(&tv,NULL);
        end = 1000000 * tv.tv_sec + tv.tv_usec;
        usleep(1000000/framerate - (end - begin));
    }
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Provide a video please.\n");
        return 0;
    }

    AVFormatContext *pFormatCtx = NULL;
    if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0)
        return -1;
    
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        return -1;
    
    int duration = (int)(pFormatCtx->duration/1000000);
    int framerate = pFormatCtx->streams[0]->r_frame_rate.num / pFormatCtx->streams[0]->r_frame_rate.den;
    char buf[0x100];
    #ifdef WSL
    system("mkdir ~/temp-termplayer/");
    snprintf(buf, sizeof(buf), "ffmpeg -i \"%s\" -pix_fmt bgr8 -y ~/temp-termplayer/bitmap%%01d.bmp", argv[1]);
    system(buf);
    snprintf(buf, sizeof(buf), "ffmpeg -i \"%s\" -y audio.wav", argv[1]);
    #else
    system("mkdir temp-termplayer/");
    snprintf(buf, sizeof(buf), "ffmpeg -i \"%s\" -pix_fmt bgr8 -y temp-termplayer/bitmap%%01d.bmp", argv[1]);
    system(buf);
    snprintf(buf, sizeof(buf), "ffmpeg -i \"%s\" -y temp-termplayer/audio.wav", argv[1]);
    #endif
    system(buf);
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    intrflush(stdscr, FALSE);
    curs_set(FALSE);
    nonl();
    start_color();
    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    clear();
    refresh();

    int pid = fork();
    if (pid == 0){
        #ifdef WSL
        system("powershell.exe -c \"del \\$env:temp\\audio.wav ; mv audio.wav \\$env:temp\\audio.wav ; (New-Object Media.SoundPlayer \\$env:temp\\audio.wav).PlaySync();\" >/dev/null 2>/dev/null");
        #elif __linux
        system("aplay temp-termplayer/audio.wav");
        #else
        system("afplay temp-termplayer/audio.wav");
        #endif
        return 0;
    }
    if (argc > 2){
        draw_bmp(duration, framerate, argv[2]);
    } else {
        draw_bmp(duration, framerate, "monochrome");
    }
    endwin();
    #ifdef WSL
    system("rm -r ~/temp-termplayer/");
    #else
    system("rm -r temp-termplayer/");
    #endif
	return 0;
}