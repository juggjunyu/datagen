# pragma once
#include "solverTD.h"

//实现从一个obj文件读取patch的初始位置
//实现从两个obj文件读取patch的位置并计算顶点速度
//实现调用solverTD计算有无碰撞


void doTest(std::string inputDir)
{
    // 位置格式 inputDir/two_patch.obj
    // 速度格式 inputDir/Animation/frame_0000.obj 

    // 读取两个patch的初始位置，
    TriQuadBezier patch1, patch2;


}