[![](https://i.ytimg.com/vi/fFPdDQVyZrs/0.jpg)](https://www.youtube.com/watch?v=fFPdDQVyZrs)

# Terminal Player
A CLI video playback program in C, uses libavformat to read video data like framerate and duration, and uses ffmpeg to convert video to bmp. Yes, it is inefficient.

## Notes
- Sound works only on WSL at the moment, would take minimal change to make it work on linux using aplay or something. I haven't tested this on anything else.
- Aspect ratio may behave funnily if its anything other than 4:3, I took Bad Apple! as a reference while coding this, also line spacing plays a huge part on that, idk how im gonna
approach to this.

## Dependencies
- ffmpeg
- libavformat
- ncurses