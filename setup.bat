@ECHO OFF
powershell -Command "Invoke-WebRequest https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip -OutFile package_glfw.zip"
powershell -Command "Expand-Archive package_glfw.zip -DestinationPath glfw_binaries"

xcopy /y /i "glfw_binaries\glfw-3.3.8.bin.WIN64\include\GLFW" "fractal-viewer\Dependencies\include\GLFW\"
xcopy /y /i "glfw_binaries\glfw-3.3.8.bin.WIN64\lib-vc2022\glfw3.lib" "fractal-viewer\Dependencies\lib\"

del "package_glfw.zip"
rmdir /Q /S glfw_binaries
PAUSE