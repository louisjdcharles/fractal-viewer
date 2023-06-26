# fractal-viewer
> Program that renders interactive fractals using compute shaders

![](https://i.imgur.com/uOzPlmy.png)
*mandelbrot*

![](https://i.imgur.com/JtwxncR.png)
*newton fractal*

See [Releases](https://github.com/louisjdcharles/fractal-viewer/releases/tag/Release) to download the application.

## Features
- Includes mandelbrot set and the newton fractal for cubics
- At 1080p (tested with rtx 2060, float32 precision)
  - Mandelbrot: up to 10,000 iterations
  - Newton Fractal: up to ~250 iterations
- Locations of solutions in the newton fractal can be moved using the mouse
- Panning and zooming
- Setting panel to change parameters i.e maximum number of iterations

## Future Plans
- [ ] Add more fractals i.e. julia sets
- [ ] Add option to render using float64

## Build Instructions (Visual Studio)
- Clone Repository
- Run `setup.bat`
- Open solution in visual studio and run!
