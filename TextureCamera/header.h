#pragma once
#ifndef _HEADER_H_

#define _HEADER_H_
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<io.h>
#include<stdlib.h>
#include<GL/freeglut.h>
#include <windows.h>
#include <direct.h> 
#include <GL/glaux.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <GL/FreeImage.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <nlohmann/json.hpp>
#include <random>
using namespace std;
using json = nlohmann::json;

random_device rd;  // 将用于获得随机数引擎的种子
mt19937 gen(rd()); // 以 rd() 播种的标准 mersenne_twister_engine
uniform_real_distribution<> disangle(0.0, 3.0);
uniform_real_distribution<> dis(0.0, 1.0);

#endif // !_HEADER_H_

