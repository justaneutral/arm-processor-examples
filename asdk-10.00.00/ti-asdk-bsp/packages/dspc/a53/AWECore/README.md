----------------------- DSP Concepts AWECore -----------------------
 
 Open Docs/AWECore_API_Doc.html for the AWECore API and User Guide.

LINUX:
There is both a static (.a) and a shared (.so) library included in this package, and either can be used to link the users application.
 This package includes a C-code reference integration of the AWECore called LinuxApp designed for a Linux based OS. This application open up a TCP/IP socket on port 15092 that can be used to communicate with AWE Server / Designer. The application does not connect to any real-time audio input or output devices. This is left to the user.

A precompiled version of LinuxApp is in the AWECore/Bin folder, and the source code is under AWECore/Source/LinuxApp.c. To run LinuxApp, execute 'sudo ./LinuxApp' from the AWECore/Bin folder, then connect AWE Server to the target using the TCP/IP connection type with the target's IP address and port 15092. The 'sudo' is required if the user does not have root permissions to run the application at the required priority levels.
Note that if LinuxApp has been linked with the shared library (libAWECore.so), you may have to update the system's linker path to find the library at runtime. Issue the command 'export LD_LIBRARY_PATH=./Lib' if running the executuable from the AWECore folder.

A multi-instance version of LinuxApp is also available called LinuxAppMulti.c. This simulates multiple instances of AWE in a Linux environment, and is executed in a similar way to LinuxApp: 'sudo ./LinuxAppMulti'. The same TCP/IP port and interface as LinuxApp is used for LinuxAppMulti.

There is also a sample application called ControlApp that shows how to use some additional AWECore API's, mostly related to controlling module variables and loading awb's. This application will process audio files through a provided .awb while reading and writing to some module variables. 


A CMakeLists.txt file is provided to allow the user to recompile both applications. To recompile the programs, do the following from the AWECore folder:
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

This will generate new LinuxApp, LinuxAppMulti, and ControlApp executables in the Bin folder. Note that a number of packages must be installed on the target for the build to work. For example, on an Ubuntu system, run the following command to install the necessary packages:
    $ sudo apt-get install build-essential cmake gcc

WINDOWS:
Only the ControlApp is available to build and run in Windows packages.
If using a Windows AWECore package, the following commands can be used to build the CMake project:
    $ mkdir build
    $ cd build
    $ cmake -G "Visual Studio 10 2010" ..
    $ cmake --build .

"Visual Studio 10 2010" can be replaced with whatever VS version is desired. 
