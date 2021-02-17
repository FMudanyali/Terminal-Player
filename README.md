[![](https://i.ytimg.com/vi/m54ED2pGbQY/0.jpg)](https://youtu.be/m54ED2pGbQY)

# Terminal Player
A CLI video playback program in C, uses libavformat to read video data like framerate and duration, and uses ffmpeg to convert video to bmp. Yes, it is inefficient.

## Notes
- Works on WSL and macOS, haven't tried under linux but I think it should work just fine.
- Resource heavy, the framerate may fluctuate or even entire thing can freeze if your computer is not powerful enough, I get full speeds with i7 2600k.
- Planning to decode videos natively without depending on ffmpeg externally, also some threading or something.
- Only WSL requires external audio playback, might use miniaudio or something on other systems.
- WSL is also really hacky because of two reasons:
- - powershell can't play the audio inside WSL directory because CMD cannot access network locations, yes, WSL path is in "\\$WSL", which is a network format. So it moves the audio to Windows temp folder first, then plays it back.
- - I/O speed is crippled outside WSL directories, resulting in slow framerate, so the temporary bmp files are extracted in home folder instead of relative folder.
- Since I am reading colors in 8bpp, and terminal has 8 colors only, it is hard to approximate colors, which means there are color artifacts.
- Aspect ratio might be wrong if its anything other than 4:3, I took Bad Apple! as a reference while coding this, also line spacing plays a huge part on that, idk how im gonna
approach to this.

## Compilation instructions
```
mkdir build
cd build
cmake .. // add '-DWSL=1' without quotes if you want to compile for WSL.
make
```

## Usage
```
./termplayer videofile.mp4 // plays in monochrome
./termplayer videofile.mp4 ANY_COLOR // plays in monochrome but whites are in specified color
./termplayer videofile.mp4 color // plays in 8 colors.
```
## Dependencies
- ffmpeg
- libavformat
- ncurses