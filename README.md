1) Compile from source

1.1) Dependencies
    
   The TLCVis code has the following dependencies

   - CMake (Minimum 2.6)
   - Qt 4 (4.8.5 or higher)
   - Qt 4 Webkit (4.8.5 or higher)
   - OpenGL/GLEW (1.10.0-3 or higher)
   - Boost (Minimum 1.42, required components system iostreams filesystem timer)

1.2) Compiling with CMake
  
   If all dependencies are installed, just go to the src/TaxiVis directory and do:
  
   mkdir build
   cd    build
   cmake ../
   Make

1.3) Compiling with QMake
   
   After changing the TaxiVis.pro file to properly reference the dependencies, just go to the src/TaxiVis directory and do:

   qmake
   make 

2) Running
        
2.1) Data
    
   The default data directory used TaxiVis is data directory included in the repository.
   You can configure this in either CMakeLists.txt file or TLCVis.pro one.

   A sample data set consistings of 10000 trips on Jan 2013 (obtained from http://www.andresmh.com/nyctaxitrips/) is provided. This data has 
   been already indexed and can be found in the data directory in the sample_merged_1.kdtrip
   file.
      
   The doc directory contains information on how to use the code in the src/preprocess folder
   to index new taxi data to be included in the system.

2.2) Executing
   Assuming that the data is in the right location, you can execute the TaxiVis file created
   in the build directory.