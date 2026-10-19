set level=Scroll

..\RGBGFX -u ..\Art\%level%\%level%Tiles.png -o ..\Art\%level%\%level%Tiles.2bpp

pause

..\RGBGFX -u -i ..\Art\%level%\%level%Tiles.2bpp -t ..\Art\%level%\%level%Map.tilemap ..\Art\%level%\%level%Map.png

pause