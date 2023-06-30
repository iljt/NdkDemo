# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.
project("ndkdemo")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#从native-lib.cpp生成动态链接库libndkdemo.so
file(GLOB allCPP *.cpp)

#从native-lib.cpp生成动态链接库libndkdemo.so
add_library( # Sets the name of the library.
        ndkdemo
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${allCPP})
# 引入libgetndk.so的头文件
#include_directories(${CMAKE_SOURCE_DIR}/include)

# 1、so直接放到app/libs目录下 build.gradle 不用设置sourceSets.main.jniLibs.srcDirs = ['libs']   ../上一级目录
# 2、so直接放到main/xxx目录下 ,xxx目录只要不是jniLibs就可以了,build.gradle 还是不用设置sourceSets.main.jniLibs.srcDirs = ['libs']
# 3、so直接放到main/jniLibs目录下 build.gradle 必须设置sourceSets.main.jniLibs.srcDirs = ['libs'] 否则运行报错如下:
#2 files found with path 'jniLibs/arm64-v8a/libgetndk.so' from inputs:
#- F:\as2\NdkDemo\app\build\intermediates\merged_jni_libs\debug\out\arm64-v8a\libgetndk.so
#- F:\as2\NdkDemo\app\build\intermediates\cxx\Debug\6f2p73e4\obj\arm64-v8a\libgetndk.so
#If you are using jniLibs and CMake IMPORTED targets, see
#原因：在gradle plugln 4.0及以上build.gradle自动会帮助我们寻找src/main/jniLibs文件夹(相当于build.gradle默认设置了sourceSets.main.jniLibs.srcDirs = ['src/main/jniLibs'])，找到文件夹下的相应abi下的.a或者.so去加载和优化,而在这里我们又重新指定src/main/jniLibs文件夹去加载第三方库，重复了
#解决办法：使用上面说的方式
# 引入libgetndk.so的库文件  运行报错先注释
#add_library(getndk SHARED IMPORTED)
#set_target_properties(getndk PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../aaa/${CMAKE_ANDROID_ARCH_ABI}/libgetndk.so)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
#就是找到已有的库并赋值给一个变量，这里因为系统库(liblog.so)默认在ndk的cmake的搜索路径中，所以无需指定路径，指定库名即可
#这里ndk会到对应目录(E:\Android\Sdk\ndk\21.4.7075529\platforms\android-30\arch-arm64\usr\lib)寻找lib前缀+find_library中第二个参数名称对应的库(liblog.so)
find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
#然后将系统的liblog.so库链接到libndkdemo.so
target_link_libraries( # Specifies the target library.
        ndkdemo
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} # liblog库 链接到 总库 libndkdemo.so
        #  getndk # libgetndk库 链接到 总库 libndkdemo.so
        )