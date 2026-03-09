#pragma once
#include "gentype.h"
#include "solverTD.h"
#include "generator.h"
#include <filesystem>
#include <fstream>

CollisionPoint generateOriginalEE(unsigned seed)
{
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);//分布范围
    Array2r uv1=edgeface::genUV(engine), uv2=edgeface::genUV(engine);
    auto patch1 = generateRandomPatch(engine, dist, dist, dist);
    auto patch2 = generateRandomPatch(engine, dist, dist, dist);
    Vector3r collisionPoint = patch2.evaluatePatchPoint(uv2); 

    auto tagent2 = computeBoundaryTangent(patch2, uv2);
    
    // edgeface::adjustEF(patch1, uv1, tagent2);
    Vector3r offset = collisionPoint - patch1.evaluatePatchPoint(uv1);
    for(int i = 0; i < 6; i++) 
    patch1.ctrlp[i] = patch1.ctrlp[i] + offset;
    
    auto patchNormal1 = patch1.evaluateNormal(uv1);
    auto [vel1, vel2] = edgeedge::genColVel(engine, patch1, patch2, uv1, uv2);
    std::cout << "vel1: " << vel1.transpose() << std::endl;
    std::cout << "vel2: " << vel2.transpose() << std::endl;
    patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);

    TriParamBound bound1 = edgeface::genLocalParam(uv1);
    TriParamBound bound2 = edgeface::genLocalParam(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);

    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);

    localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);

    CollisionPoint cp = { localPatch1, localPatch2, uv1, uv2, local_uv1, local_uv2, localPatch1.evaluateNormal(local_uv1), localPatch2.evaluateNormal(local_uv2), vel1, vel2};
    
    std::cout << "collision point1: " << localPatch1.evaluatePatchPoint(local_uv1).transpose() << std::endl;
    std::cout << "collision point2: " << localPatch2.evaluatePatchPoint(local_uv2).transpose() << std::endl;
   
    return cp;
}

// 将生成的CollisionPoint数据保存到CSV文件
// outputDir: 输出目录
// seed: 随机种子
// 返回生成的CSV文件路径
inline std::string saveOriginalEEToCSV(const CollisionPoint& cp, const std::string& outputDir, unsigned seed)
{
    // 创建CSV数据集文件路径
    std::string datasetFilePath = outputDir + "/fp_dataset_RestoreEE.csv";
    
    // 如果文件已存在，先备份
    if (std::filesystem::exists(datasetFilePath)) {
        std::string backupPath = datasetFilePath + ".bak";
        std::filesystem::copy_file(datasetFilePath, backupPath, 
                                  std::filesystem::copy_options::overwrite_existing);
        std::cout << "Existing dataset file backed up to: " << backupPath << std::endl;
    }
    
    // 创建输出目录（如果不存在）
    std::filesystem::create_directories(outputDir);
    
    // 创建新的数据集文件并添加标题行
    std::ofstream csvFile(datasetFilePath);
    csvFile << "# Dataset for RestoreEE collision type" << std::endl;
    csvFile << "# Format: x_numerator,x_denominator,y_numerator,y_denominator,z_numerator,z_denominator,ground_truth" << std::endl;
    csvFile << "# Lines 1-6: Patch1 start positions (t=1)" << std::endl;
    csvFile << "# Lines 7-12: Patch2 start positions (t=1)" << std::endl;
    csvFile << "# Lines 13-18: Patch1 end positions (t=0)" << std::endl;
    csvFile << "# Lines 19-24: Patch2 end positions (t=0)" << std::endl;
    csvFile.close();
    
    // 使用genStandardData::saveControlPointsData保存数据到CSV
    // tasktype=2 表示 EdgeEdge 类型
    genStandardData::saveControlPointsData(cp, outputDir, datasetFilePath, seed, 2);
    
    std::cout << "Data saved to: " << datasetFilePath << std::endl;
    
    return datasetFilePath;
}