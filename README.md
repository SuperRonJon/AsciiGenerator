# AsciiGen
CLI Ascii Art Generator from image file.

## Installation
Pre-build binaries available for Windows & Linux x86_64 from the [releases page](https://github.com/SuperRonJon/AsciiGenerator/releases)

It can also be easily built from source. Building requires only gcc, and optionally make. Clone the repository and run `mkdir build && make` or `gcc -O2 -o asciigen main.c -lm` with gcc only.

## Options/Usage
```
-> $ asciigen --help
Usage:
       asciigen [options] image.png
Options:
    -i              inverts light and dark colors. Brightest pixels use densest characters
    -w scale        Width scaling factor. Output's width will be original_width * scale
    -h scale        Height scaling factor. Output's height will be original_height * scale
    -s scale        Even scaling factor. Output's dimensions will be original * scale
    -v, --version   Prints version
    -H, --help      Prints help
```

Scaling factor values are floating point values that indicate the amount to scale the original image by.

Ex. `asciigen -w 0.2 -h 0.1 image.png` will scale the ascii-art output's width down to 20% and height to 10% the original image.png's pixel resolution. This is equivalent to  `asciigen -wh 0.2 0.1 image.png`

Ex. `asciigen -i -s 0.015 high-res-image.png` will scale both the height and width of high-res-image.png to 1.5% the original image's pixel resolution, as well as invert the color scale, so that bright colors to use the densest characters rather than vice-versa. This is equivalent to `asciigen -is 0.015 high-res-image.png`

## Example
```
-> $ asciigen -i -w 0.015 -h 0.01 saturn.jpg
-> $ asciigen -iwh 0.015 0.01 saturn.jpg

                        .....
                ....:-=+***##*+-:...
           ...::::::---==++*##%%#+-::::...
       ...:::---::     .::-=+*##%%#-----:::..
     .::::--::..          .:-=+*##%*:.::---:::..
   .:::---:.                .:-=+*#%=  ..:---:.:.
  .:..---:.                   .:=+*##.   .:---:.:.
  ::.:---.                     .:-+*#.    .:---..:.
  ::.:---:                      ..-++     .:---.::.
  .::.:--                         .=-    .:---:.:.
   .::.:.                          :  ..:---:..:.
     ....                        ...::----::.:..
       .....    ....:::::::::::::----:::::...
           .....:::::::::::::::::::::.....
                 ...................
```
