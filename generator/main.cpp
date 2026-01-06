#include <rational/rational.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include "generator.h"
#include "paramMesh.h"
#include "solverTD.h"
#include "argsParser.h"
#include <random>
#include <chrono>
#include <fstream>
#include <filesystem>

// using Rational = rational::Rational;
// using Vector3r = Eigen::Matrix<Rational, 3, 1>;
// using Array2r = Eigen::Matrix<Rational, 2, 1>;

//实现从argparser
inline std::unique_ptr<ArgsParser> BuildArgsParser()
{
	auto parser = std::make_unique<ArgsParser>();
    parser->addArgument<int>("generate", 'g', "work mode: 0=test, 1=generate single case, 2=batch process folders", 1);
    parser->addArgument<std::string>("output", 'o', "output dir", "Animation");
    parser->addArgument<std::string>("input", 'i', "test input file", "tests");
    parser->addArgument<int>("type", 't', "type of generator", 0);
    return parser;
}

void generateAnimationSequence(
    const CollisionPoint& cp,
    double timeStart,    // 起始时间
    double timeEnd,      // 结束时间
    double deltaTime,    // 时间步长
    std::string outputDir = "Animation",
    bool generateStartEnd = true,
    unsigned seed = 0
) 
{
    outputDir = outputDir + "_seed" + std::to_string(seed);
    std::cout<< "Generating animation sequence in" << outputDir << "..." << std::endl;
    std::filesystem::create_directories(outputDir);
    static ParamMesh<TriQuadBezier> meshPatch1Start(1);
    static ParamMesh<TriQuadBezier> meshPatch1End(1);
    meshPatch1Start.patches[0] = cp.patch1;
    for (int i = 0; i < 6; i++) 
        meshPatch1End.patches[0].ctrlp[i] = cp.patch1.ctrlp[i] + cp.patch1.velp[i] * timeEnd;
    static ParamMesh<TriQuadBezier> meshPatch2Start(1);
    static ParamMesh<TriQuadBezier> meshPatch2End(1);
    meshPatch2Start.patches[0] = cp.patch2;
    for (int i = 0; i < 6; i++) 
        meshPatch2End.patches[0].ctrlp[i] = cp.patch2.ctrlp[i] + cp.patch2.velp[i] * timeEnd;
    meshPatch1Start.writeObj(outputDir + "/patch1_start.obj");
    meshPatch1End.writeObj(outputDir + "/patch1_end.obj");
    meshPatch2Start.writeObj(outputDir + "/patch2_start.obj");
    meshPatch2End.writeObj(outputDir + "/patch2_end.obj");

    if(generateStartEnd) {
        return;
    }

    static ParamMesh<TriQuadBezier> mesh(2);
    int frameCount = 0;
    
    for(double t = timeStart; t <= timeEnd; t += deltaTime) {
        // 更新两个patch的位置
        TriQuadBezier patch1 = cp.patch1;
        TriQuadBezier patch2 = cp.patch2;
        
        // 更新控制点位置
        for(int i = 0; i < 6; i++) {
            patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * t;
            patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * t;
        }
        
        // if(t - deltaTime <= timeStart)
        // {
        //     //输出ctrlp
        //     std::cout<<"patch1:"<< std::endl;
        //     for(int i = 0; i < 6; i++)
        //         std::cout << patch1.ctrlp[i][0] << " " << patch1.ctrlp[i][1] << " " << patch1.ctrlp[i][2] << std::endl;
        //     std::cout << "patch2:" << std::endl;
        //     for(int i = 0; i < 6; i++)
        //         std::cout << patch2.ctrlp[i][0] << " " << patch2.ctrlp[i][1] << " " << patch2.ctrlp[i][2] << std::endl;
        // }

        // 写入当前帧
        mesh.patches[0] = patch1;
        mesh.patches[1] = patch2;
        
         // 生成帧文件名（添加前导零以确保正确排序）
         std::string filename = outputDir + "/frame_" + 
         std::to_string(frameCount).insert(0, 4 - std::to_string(frameCount).length(), '0') + 
         ".obj";        
         mesh.writeObj(filename);
        
        frameCount++;
    }
    
    std::cout << "Generated " << frameCount << " animation frames." << std::endl;
}



int main(int argc, char *argv[])
{
    auto parser = BuildArgsParser();
	parser->parse(argc, argv);

    const auto workType = std::any_cast<int>(parser->getValueByName("generate"));
    const auto outputDir = std::any_cast<std::string>(parser->getValueByName("output"));
    const auto inputDir = std::any_cast<std::string>(parser->getValueByName("input"));
    const auto taskType = std::any_cast<int>(parser->getValueByName("type"));

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // seed = 4288348963;
    std::cout << "seed: " << seed << std::endl;

    // 批处理模式：扫描文件夹并生成CSV
    if (workType == 2) {
        std::cout << "=== Batch Processing Mode ===" << std::endl;
        std::cout << "Input directory: " << inputDir << std::endl;
        std::cout << "Task type: " << taskType << std::endl;
        
        genStandardData::batchProcessFolders(inputDir, taskType);
        
        std::cout << "Batch processing completed!" << std::endl;
        return 0;
    }
    // std::mt19937_64 engine(seed);
    // std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);
    // int ans=0;

    // ans++;
    // float f1 = dist(engine), f2 = dist(engine), f3 = dist(engine);
    // float f4 = dist(engine), f5 = dist(engine), f6 = dist(engine);
    // Rational r1 = f1, r2 = f2, r3 = f3;
    // Rational r4 = f4, r5 = f5, r6 = f6;
    // //化简r1, r2, r3
    // // r1.canonicalize();
    // // r2.canonicalize();
    // // r3.canonicalize();
    
    // std::cout << "f1: " << f1 << ", r1: " << r1 << std::endl;
    // std::cout << "f2: " << f2 << ", r2: " << r2 << std::endl;
    // std::cout << "f3: " << f3 << ", r3: " << r3 << std::endl;
    // isPreciselyRepresent(Vector3r(r1, r2, r3));

    // Vector3r offs = Vector3r(r1-r4, r2-r5, r3-r6);
    // for(int i=0; i<3; i++)
    //     offs[i].canonicalize();
    // std::cout << "offset: " << offs.transpose() << std::endl;
    //     bool ok = isPreciselyRepresent(offs);

    // std::cout<<ans<<std::endl;
    auto cp = generateSeparatedRandomBezierPatches(seed, taskType);
    if(cp.vel1 == Vector3r::Zero() && cp.vel2 == Vector3r::Zero())
    {
        std::cout << "generateSeparatedRandomBezierPatches failed!(zero velocity)" << std::endl;
        return 0;
    }
    //输出两个patch
    TriQuadBezier patch1 = cp.patch1;
    TriQuadBezier patch2 = cp.patch2;
    static ParamMesh<TriQuadBezier> mesh(2);
    mesh.patches[0] = patch1;
    mesh.patches[1] = patch2;
    mesh.writeObj("two_patch.obj"); 
    
    // for (int i = 0; i < 6; i++)
    // {
    //     //约分
    //     for(int j=0; j<2; j++)
    //         patch1.ctrlp[i][j].canonicalize();
    //     std::cout << "patch1[" << i << "] = " << patch1.ctrlp[i].transpose() << std::endl;
    // }
    // for (int i = 0; i < 6; i++)
    // {
    //     for(int j=0; j<2; j++)
    //         patch2.ctrlp[i][j].canonicalize();
    //     std::cout << "patch2[" << i << "] = " << patch2.ctrlp[i].transpose() << std::endl;
    // }
    // //再转double输出小数点后六位
    // for (int i = 0; i < 6; i++)
    // {
    //     std::cout << "patch1[" << i << "] = " << std::fixed << std::setprecision(6) << patch1.ctrlp[i].cast<double>().transpose() << std::endl;
    // }
    // for (int i = 0; i < 6; i++)
    // {
    //     std::cout << "patch2[" << i << "] = " << std::fixed << std::setprecision(6) << patch2.ctrlp[i].cast<double>().transpose() << std::endl;
    // }

    auto ok = testAdditionalCollisions(cp);
    if(!ok)
        generateAnimationSequence(cp, 0, 1.0, 0.01, outputDir, false, seed);
}
// EF_2322930470