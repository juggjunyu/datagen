#pragma once
#include <Eigen/Dense>
#include <rational/rational.hpp>
#include <random>
#include <iomanip>
#include "collisionDetector.h"

// =====================================================================
// 精度检测工具函数：用于验证有理数在转换为 double 后是否能精确还原
// =====================================================================

/**
 * @brief 检测单个 Rational 是否能被 double 精确表示
 * @param r 要检测的有理数
 * @return true 表示可以精确表示，false 表示会有精度损失
 * 
 * 原理：将 Rational 转为 double，再用 double 构造新的 Rational，
 *       比较两者是否相等。如果不等，说明转换过程中发生了精度损失。
 */
inline bool isDoublePrecise(const Rational& r) {
    double d = static_cast<double>(r);
    Rational r_restored(d);
    return r == r_restored;
}

/**
 * @brief 检测 Vector3r 中的所有分量是否都能被 double 精确表示
 * @param v 要检测的三维向量
 * @return true 表示所有分量都可以精确表示
 */
inline bool isDoublePrecise(const Vector3r& v) {
    return isDoublePrecise(v[0]) && isDoublePrecise(v[1]) && isDoublePrecise(v[2]);
}

/**
 * @brief 检测 Array2r 中的所有分量是否都能被 double 精确表示
 * @param arr 要检测的二维数组
 * @return true 表示所有分量都可以精确表示
 */
inline bool isDoublePrecise(const Array2r& arr) {
    return isDoublePrecise(arr[0]) && isDoublePrecise(arr[1]);
}

/**
 * @brief 检测 TriQuadBezier patch 的所有控制点和速度是否都能被 double 精确表示
 * @param patch 要检测的贝塞尔曲面
 * @param checkVelocity 是否同时检测速度场（默认 true）
 * @return true 表示所有数据都可以精确表示
 */
inline bool isDoublePrecise(const TriQuadBezier& patch, bool checkVelocity = true) {
    // 检测所有控制点
    for (int i = 0; i < 6; i++) {
        if (!isDoublePrecise(patch.ctrlp[i])) {
            return false;
        }
    }
    // 检测速度场
    if (checkVelocity) {
        for (int i = 0; i < 6; i++) {
            if (!isDoublePrecise(patch.velp[i])) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief 打印有理数的精度诊断信息
 * @param name 变量名称（用于输出）
 * @param r 要诊断的有理数
 * 
 * 输出内容包括：原始值、分子、分母、转 double 后的值、还原后的值、是否精确
 */
inline void printPrecisionDiagnostic(const std::string& name, const Rational& r) {
    double d = static_cast<double>(r);
    Rational r_restored(d);
    bool precise = (r == r_restored);
    
    std::cout << "=== Precision Diagnostic for " << name << " ===" << std::endl;
    std::cout << "  Original Rational: " << r << std::endl;
    std::cout << "  Numerator:   " << r.numerator_str() << std::endl;
    std::cout << "  Denominator: " << r.denominator_str() << std::endl;
    std::cout << "  As double:   " << std::fixed << std::setprecision(17) << d << std::endl;
    std::cout << "  Restored:    " << r_restored << std::endl;
    std::cout << "  Is Precise:  " << (precise ? "YES" : "NO *** PRECISION LOSS ***") << std::endl;
    if (!precise) {
        Rational diff = r - r_restored;
        std::cout << "  Difference:  " << diff << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief 打印 Vector3r 的精度诊断信息
 * @param name 变量名称
 * @param v 要诊断的向量
 */
inline void printPrecisionDiagnostic(const std::string& name, const Vector3r& v) {
    std::cout << "=== Vector3r Precision Diagnostic for " << name << " ===" << std::endl;
    for (int i = 0; i < 3; i++) {
        printPrecisionDiagnostic(name + "[" + std::to_string(i) + "]", v[i]);
    }
}

/**
 * @brief 打印 patch 的精度诊断信息，找出所有不精确的控制点/速度
 * @param name patch 名称
 * @param patch 要诊断的贝塞尔曲面
 * @param checkVelocity 是否检测速度场
 * @return 不精确的数量
 */
inline int printPrecisionDiagnostic(const std::string& name, const TriQuadBezier& patch, bool checkVelocity = true) {
    int impreciseCount = 0;
    std::cout << "=== Patch Precision Diagnostic for " << name << " ===" << std::endl;
    
    // 检测控制点
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            if (!isDoublePrecise(patch.ctrlp[i][j])) {
                printPrecisionDiagnostic(name + ".ctrlp[" + std::to_string(i) + "][" + std::to_string(j) + "]", 
                                         patch.ctrlp[i][j]);
                impreciseCount++;
            }
        }
    }
    
    // 检测速度场
    if (checkVelocity) {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                if (!isDoublePrecise(patch.velp[i][j])) {
                    printPrecisionDiagnostic(name + ".velp[" + std::to_string(i) + "][" + std::to_string(j) + "]", 
                                             patch.velp[i][j]);
                    impreciseCount++;
                }
            }
        }
    }
    
    if (impreciseCount == 0) {
        std::cout << "  All values are double-precise!" << std::endl;
    } else {
        std::cout << "  Total imprecise values: " << impreciseCount << std::endl;
    }
    std::cout << std::endl;
    
    return impreciseCount;
}

/**
 * @brief 检测 CollisionPoint 的所有数据是否都能被 double 精确表示
 * @param cp 要检测的碰撞点
 * @return true 表示所有数据都可以精确表示
 */
inline bool isDoublePrecise(const struct CollisionPoint& cp);  // 前向声明，在 CollisionPoint 定义后实现

// =====================================================================

struct CollisionPoint 
{
    TriQuadBezier patch1;
    TriQuadBezier patch2;
    Array2r uv1;            //原先的uv
    Array2r uv2;
    Array2r local_uv1;      // 局部参数域中的坐标
    Array2r local_uv2;
    Vector3r normal1;  
    Vector3r normal2;
    Vector3r vel1;
    Vector3r vel2;
};

// CollisionPoint 的精度检测函数实现（需要在 CollisionPoint 定义之后）
inline bool isDoublePrecise(const CollisionPoint& cp) {
    return isDoublePrecise(cp.patch1, true) && 
           isDoublePrecise(cp.patch2, true) &&
           isDoublePrecise(cp.uv1) &&
           isDoublePrecise(cp.uv2) &&
           isDoublePrecise(cp.local_uv1) &&
           isDoublePrecise(cp.local_uv2) &&
           isDoublePrecise(cp.normal1) &&
           isDoublePrecise(cp.normal2) &&
           isDoublePrecise(cp.vel1) &&
           isDoublePrecise(cp.vel2);
}

/**
 * @brief 打印 CollisionPoint 的精度诊断信息
 * @param name 名称
 * @param cp 要诊断的碰撞点
 * @return 不精确的数量
 */
inline int printPrecisionDiagnostic(const std::string& name, const CollisionPoint& cp) {
    int impreciseCount = 0;
    std::cout << "=== CollisionPoint Precision Diagnostic for " << name << " ===" << std::endl;
    
    impreciseCount += printPrecisionDiagnostic(name + ".patch1", cp.patch1, true);
    impreciseCount += printPrecisionDiagnostic(name + ".patch2", cp.patch2, true);
    
    // 检测 UV 参数
    if (!isDoublePrecise(cp.uv1)) {
        printPrecisionDiagnostic(name + ".uv1[0]", cp.uv1[0]);
        printPrecisionDiagnostic(name + ".uv1[1]", cp.uv1[1]);
        impreciseCount += 2;
    }
    if (!isDoublePrecise(cp.uv2)) {
        printPrecisionDiagnostic(name + ".uv2[0]", cp.uv2[0]);
        printPrecisionDiagnostic(name + ".uv2[1]", cp.uv2[1]);
        impreciseCount += 2;
    }
    
    // 检测法线
    if (!isDoublePrecise(cp.normal1)) {
        printPrecisionDiagnostic(name + ".normal1", cp.normal1);
        impreciseCount += 3;
    }
    if (!isDoublePrecise(cp.normal2)) {
        printPrecisionDiagnostic(name + ".normal2", cp.normal2);
        impreciseCount += 3;
    }
    
    // 检测速度
    if (!isDoublePrecise(cp.vel1)) {
        printPrecisionDiagnostic(name + ".vel1", cp.vel1);
        impreciseCount += 3;
    }
    if (!isDoublePrecise(cp.vel2)) {
        printPrecisionDiagnostic(name + ".vel2", cp.vel2);
        impreciseCount += 3;
    }
    
    std::cout << "Total imprecise values in CollisionPoint: " << impreciseCount << std::endl;
    std::cout << std::endl;
    return impreciseCount;
}

/**
 * @brief 综合检测 CollisionPoint 在保存到 CSV 时的精度
 * 
 * 检测内容包括：
 * 1. 运动 1s 后的 patch 位置 (ctrlp + velp)
 * 2. 实际保存到 CSV 的数据 ((ctrlp + velp) * SCALE_FACTOR)
 * 
 * @param cp 要检测的碰撞点
 * @param scaleFactor 缩放因子，默认 1/131072
 * @param verbose 是否输出详细诊断信息
 * @return true 表示所有数据都可以被 double 精确表示
 */
inline bool checkCollisionPointPrecision(const CollisionPoint& cp, 
                                         const Rational& scaleFactor = Rational("1/131072"),
                                         bool verbose = true) {
    bool allPrecise = true;
    
    if (verbose) {
        std::cout << "\n========== COLLISION POINT PRECISION CHECK ==========" << std::endl;
    }
    
    // 1. 检测运动 1s 后的 patch (ctrlp + velp * 1)
    if (verbose) {
        std::cout << "\n[1] Checking patch after 1s movement (ctrlp + velp):" << std::endl;
    }
    bool precise_1s = true;
    for (int i = 0; i < 6; i++) {
        Vector3r pos1_after = cp.patch1.ctrlp[i] + cp.patch1.velp[i];
        Vector3r pos2_after = cp.patch2.ctrlp[i] + cp.patch2.velp[i];
        
        for (int j = 0; j < 3; j++) {
            if (!isDoublePrecise(pos1_after[j])) {
                if (verbose) {
                    std::cout << "  [IMPRECISE] patch1.ctrlp[" << i << "][" << j << "] + velp:" << std::endl;
                    printPrecisionDiagnostic("patch1_after_1s[" + std::to_string(i) + "][" + std::to_string(j) + "]", pos1_after[j]);
                }
                precise_1s = false;
                allPrecise = false;
            }
            if (!isDoublePrecise(pos2_after[j])) {
                if (verbose) {
                    std::cout << "  [IMPRECISE] patch2.ctrlp[" << i << "][" << j << "] + velp:" << std::endl;
                    printPrecisionDiagnostic("patch2_after_1s[" + std::to_string(i) + "][" + std::to_string(j) + "]", pos2_after[j]);
                }
                precise_1s = false;
                allPrecise = false;
            }
        }
    }
    if (verbose && precise_1s) {
        std::cout << "  [OK] All positions after 1s are double-precise." << std::endl;
    }
    
    // 2. 检测实际保存的数据 (ctrlp + velp) * scaleFactor 和 ctrlp * scaleFactor
    if (verbose) {
        std::cout << "\n[2] Checking saved data (with scale factor " << scaleFactor << "):" << std::endl;
    }
    bool precise_scaled = true;
    for (int i = 0; i < 6; i++) {
        // startPos = (ctrlp + velp) * scaleFactor（运动起点，即 t=1s 时刻位置经缩放）
        Vector3r startPos1 = (cp.patch1.ctrlp[i] + cp.patch1.velp[i]) * scaleFactor;
        Vector3r startPos2 = (cp.patch2.ctrlp[i] + cp.patch2.velp[i]) * scaleFactor;
        // endPos = ctrlp * scaleFactor（运动终点，即 t=0 时刻位置经缩放）
        Vector3r endPos1 = cp.patch1.ctrlp[i] * scaleFactor;
        Vector3r endPos2 = cp.patch2.ctrlp[i] * scaleFactor;
        
        for (int j = 0; j < 3; j++) {
            if (!isDoublePrecise(startPos1[j])) {
                if (verbose) {
                    std::cout << "  [IMPRECISE] startPos1[" << i << "][" << j << "]:" << std::endl;
                    printPrecisionDiagnostic("startPos1", startPos1[j]);
                }
                precise_scaled = false;
                allPrecise = false;
            }
            if (!isDoublePrecise(startPos2[j])) {
                if (verbose) {
                    std::cout << "  [IMPRECISE] startPos2[" << i << "][" << j << "]:" << std::endl;
                    printPrecisionDiagnostic("startPos2", startPos2[j]);
                }
                precise_scaled = false;
                allPrecise = false;
            }
            if (!isDoublePrecise(endPos1[j])) {
                if (verbose) {
                    std::cout << "  [IMPRECISE] endPos1[" << i << "][" << j << "]:" << std::endl;
                    printPrecisionDiagnostic("endPos1", endPos1[j]);
                }
                precise_scaled = false;
                allPrecise = false;
            }
            if (!isDoublePrecise(endPos2[j])) {
                if (verbose) {
                    std::cout << "  [IMPRECISE] endPos2[" << i << "][" << j << "]:" << std::endl;
                    printPrecisionDiagnostic("endPos2", endPos2[j]);
                }
                precise_scaled = false;
                allPrecise = false;
            }
        }
    }
    if (verbose && precise_scaled) {
        std::cout << "  [OK] All saved positions are double-precise." << std::endl;
    }
    
    if (verbose) {
        std::cout << "\n========== PRECISION CHECK RESULT: " 
                  << (allPrecise ? "ALL PRECISE" : "PRECISION LOSS DETECTED") 
                  << " ==========" << std::endl << std::endl;
    }
    
    return allPrecise;
}

Rational generateSimpleRational(auto& engine) {
    // 生成-4000到4000的整数
    std::uniform_int_distribution<int> dist(-1000<<10, 1000<<10);
    int randomValue = dist(engine);

    // 除以4转换为我们需要的格式（整数或整数加0.25, 0.5, 0.75）
    return Rational(randomValue)/Rational((1<<15));  // 相当于randomValue/4，自动约分
}


Vector3r randomVector3r(auto &engine)
{
    // Vector3r result = Vector3r(dist1(engine), dist2(engine), dist3(engine));
    Vector3r result;
    for(int i = 0; i < 3; i++)
        result[i] = generateSimpleRational(engine);
    //约分result每一维
    for(int i = 0; i < 3; i++) 
        result[i].canonicalize();
    return result;
}

Vector3r randomVector3r(auto &engine, auto &dist1, auto &dist2, auto &dist3)
{
    Vector3r result = Vector3r(dist1(engine), dist2(engine), dist3(engine));
    for(int i = 0; i < 3; i++) 
        result[i].canonicalize();
    return result;
}

TriQuadBezier generateRandomPatch(auto &engine, auto &dist1, auto &dist2, auto &dist3) 
{
    std::array<Vector3r, 6> patch;
    for(int i = 0; i < 6; i++) 
        patch[i] = randomVector3r(engine, dist1, dist2, dist3);
        // patch[i] = randomVector3r(engine);
    return TriQuadBezier(patch);
}


TriParamBound generateFixedParamBound();

Vector3r generateRandomDirectionInHalfSpace(auto &engine, const Vector3r& normal) 
{
    std::uniform_real_distribution<float> dist(-1, 1);
    

    Vector3r direction;
    do 
    {
        direction = Vector3r(dist(engine), dist(engine), dist(engine));
        // direction.normalize();
    } while (direction.dot(normal) <= 0); 
    
    return direction;
}

// Array2r generateValidTriangleUV(auto &engine) 
// {
//     std::uniform_real_distribution<float> dist(0, 1-(1e-5));
    
//     float u = dist(engine);
//     std::uniform_real_distribution<float> distV(0, (1-1e-5)-u);
//     float v = distV(engine);
//     Array2r uv;
//     uv << u, v;
//     return uv;
// }

Array2r generateValidTriangleUV(auto &engine) 
{
    // 只使用1/2, 1/4, 1/8
    std::array<Rational, 2> fractions = {
        Rational(1) / Rational(2),  
        Rational(1) / Rational(4) 
        // Rational(1) / Rational(8)   
    };
    
    // 随机选择u的值
    std::uniform_int_distribution<int> fractionSelector(0, 1); // 选择1/2, 1/4或1/8
    Rational u = fractions[fractionSelector(engine)];
    

    std::vector<Rational> validVOptions;
    
    // 添加可能的v值，确保u+v <= 1
    for (const auto& frac : fractions) {
        if (u + frac <= Rational(1)) {
            validVOptions.push_back(frac);
        }
    }
    
    // 如果没有有效选项，使用v=0
    if (validVOptions.empty()) {
        return Array2r(u, Rational(0));
    }
    
    // 从可用选项中随机选择v
    std::uniform_int_distribution<int> vSelector(0, validVOptions.size() - 1);
    Rational v = validVOptions[vSelector(engine)];
    
    return Array2r(u, v);
}


std::array<Vector3r, 6> generateVelocityFieldIndependent(
    std::mt19937_64& engine,
    const Vector3r& normal);

// 为Edge-Edge碰撞生成速度场，确保满足(v1-v2)·(n1+n2) > 0的条件
std::pair<std::array<Vector3r, 6>, std::array<Vector3r, 6>> generateVelocityFieldEdgeEdge(
    std::mt19937_64& engine,
    const TriQuadBezier& patch1,
    const TriQuadBezier& patch2,
    const Array2r& uv1,
    const Array2r& uv2);

Array2r computeLocalUV(const BaryCoord& originalCoord, const TriParamBound& bound);

TriParamBound generateLocalParamBound(const Array2r& uv, Rational radius = 0.1);

std::pair<Rational, Rational> calculatePrincipalCurvatures(const TriQuadBezier& patch, const Array2r& uv);

Vector3r generateRandomDirectionInHalfSpace(auto &engine, const Vector3r& normal);

Vector3r computeBoundaryTangent(const TriQuadBezier& patch, const Array2r& uv); 

namespace edgeface {

    inline Array2r genUV(auto& engine) {
        // 边界条件: u=0, v=0, 或 u+v=1
        std::uniform_int_distribution<int> edgeDist(0, 2);
        int edge = edgeDist(engine);
        std::uniform_int_distribution<int> powerDist(1, 2); // 1,2 -> 1/2,1/4
        int power = powerDist(engine);
        Rational t = Rational(1) / Rational(1 << power);
        
        // 随机选择具体值
        std::uniform_int_distribution<int> numDist(1, (1 << power));
        // Rational param = Rational(numDist(engine)) / Rational(1 << power);
        Rational param = t;


        Array2r uv;
        if(edge == 0) {
            // w=0边界: u+v=1
            uv[0] = param;          // u
            uv[1] = Rational(1) - param; // v=1-u
        } else if(edge == 1) {
            // u=0边界
            uv[0] = Rational(0);
            uv[1] = param;          // v
        } else {
            // v=0边界
            uv[0] = param;          // u
            uv[1] = Rational(0);
        }
        
        return uv;
    }

std::pair<Vector3r, Vector3r> genColVel(
    auto& engine,
    const Vector3r& faceNormal,        // 面片法线
    const TriQuadBezier& edgePatch,    // 边界曲面
    const Array2r& collisionUV         // 边界点参数
) 
{
    try {
        std::uniform_real_distribution<float> speedDist(0.5, 5.0);
        
        // 面片(patch1)的速度生成 - 与标准情况相同
        Vector3r dir1 = generateRandomDirectionInHalfSpace(engine, faceNormal);
        Rational speed1 = speedDist(engine);
        
        // 通过局部采样确定边界曲面的大致朝向
        const int numSamples = 3;  // 减少采样点数量
        int sameDirectionCount = 0;
        
        // 确定当前点在哪条边上
        int edge = -1;
        const Rational epsilon = Rational(1) / Rational(10000); // 简化精度要求
        
        if (std::abs(collisionUV[1]) < epsilon) 
            edge = 0; // 底边: v = 0
        else if (std::abs(collisionUV[0]) < epsilon) 
            edge = 1; // 左边: u = 0
        else if (std::abs(collisionUV[0] + collisionUV[1] - Rational(1)) < epsilon) 
            edge = 2; // 斜边: u + v = 1
        else {
            std::cerr << "error: not on boundary" << std::endl;
            // 非边界点时使用适当默认值
            return {speed1 * dir1, speed1 * (-faceNormal.normalized())};
        }
        
        // 碰撞点法线
        Vector3r collisionNormal;
        try {
            collisionNormal = edgePatch.evaluateNormal(collisionUV);
            sameDirectionCount = sameDirectionCount + (collisionNormal.dot(faceNormal) > 0) ? 1 : 0;
        } catch (...) {
            // 如果计算法线失败，默认使用反向速度
            std::cerr << "Failed to evaluate normal at collision point" << std::endl;
            return {speed1 * dir1, speed1 * (-faceNormal.normalized())};
        }
        
        // 仅采样2个额外点 - 一个在参数较小处，一个在参数较大处
        Rational safeOffset = Rational(1) / Rational(20); // 5%的偏移
        std::array<Array2r, 2> samplePoints;
        
        // 根据边缘类型创建采样点
        switch (edge) {
            case 0: { // 底边
                Rational t = collisionUV[0];
                // 确保安全范围内
                Rational t1 = std::max(Rational(1)/Rational(20), std::min(t - safeOffset, Rational(19)/Rational(20)));
                Rational t2 = std::max(Rational(1)/Rational(20), std::min(t + safeOffset, Rational(19)/Rational(20)));
                samplePoints[0] = Array2r(t1, 0);
                samplePoints[1] = Array2r(t2, 0);
                break;
            }
            case 1: { // 左边
                Rational t = collisionUV[1];
                Rational t1 = std::max(Rational(1)/Rational(20), std::min(t - safeOffset, Rational(19)/Rational(20)));
                Rational t2 = std::max(Rational(1)/Rational(20), std::min(t + safeOffset, Rational(19)/Rational(20)));
                samplePoints[0] = Array2r(0, t1);
                samplePoints[1] = Array2r(0, t2);
                break;
            }
            case 2: { // 斜边
                Rational t = collisionUV[0];
                Rational t1 = std::max(Rational(1)/Rational(20), std::min(t - safeOffset, Rational(19)/Rational(20)));
                Rational t2 = std::max(Rational(1)/Rational(20), std::min(t + safeOffset, Rational(19)/Rational(20)));
                samplePoints[0] = Array2r(t1, Rational(1) - t1);
                samplePoints[1] = Array2r(t2, Rational(1) - t2);
                break;
            }
        }
        
        // 对附加点采样
        for (const auto& uv : samplePoints) {
            try {
                Vector3r normal = edgePatch.evaluateNormal(uv);
                sameDirectionCount = sameDirectionCount + (normal.dot(faceNormal) > 0) ? 1 : 0;
            } catch (...) {
                // 忽略采样点错误
                std::cerr << "Failed to evaluate normal at sample point" << std::endl;
                continue;
            }
        }
        
        // 根据采样结果确定朝向
        bool mostlySameDirection = (sameDirectionCount > numSamples / 2);
        
        // 根据朝向选择半空间
        Vector3r referenceNormal = mostlySameDirection ? -faceNormal : faceNormal;
        
        // 在确定的半空间内生成随机方向
        Vector3r dir2 = generateRandomDirectionInHalfSpace(engine, referenceNormal);
        Rational speed2 = speedDist(engine);
        dir1 = generateRandomDirectionInHalfSpace(engine, -referenceNormal);

        return {speed1 * dir1, speed2 * dir2};
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in genColVel: " << e.what() << std::endl;
        // 发生异常时返回默认值
        return {faceNormal, -faceNormal};
    }
    catch (...) {
        std::cerr << "Unknown exception in genColVel" << std::endl;
        // 发生异常时返回默认值
        return {faceNormal, -faceNormal};
    }
}

TriParamBound genLocalParam(const Array2r& uv, Rational radius=0.1) ;

void adjustEF(TriQuadBezier& facePatch, const Array2r& faceUV, const Vector3r& edgeTangent);

bool penEFCheck(const CollisionPoint& cp, bool ok); 

};

namespace edgeedge
{
    std::pair<Vector3r, Vector3r> genColVel(
        auto& engine,
        const TriQuadBezier& patch1,
        const TriQuadBezier& patch2,    
        const Array2r& uv1,
        const Array2r& uv2
    ) 
    {
        try {
            std::uniform_real_distribution<float> speedDist(0.5, 5.0);
            
            // 计算两个边的切向量
            Vector3r tangent1 = computeBoundaryTangent(patch1, uv1);
            Vector3r tangent2 = computeBoundaryTangent(patch2, uv2);
            
            // 计算两个边的法线
            Vector3r normal1 = patch1.evaluateNormal(uv1);
            Vector3r normal2 = patch2.evaluateNormal(uv2);
            
            // 归一化法线和切向量
            if (normal1.norm() != Rational(0)) {
                normal1 = normal1 / normal1.norm();
            }
            if (normal2.norm() != Rational(0)) {
                normal2 = normal2 / normal2.norm();
            }
            if (tangent1.norm() != Rational(0)) {
                tangent1 = tangent1 / tangent1.norm();
            }
            if (tangent2.norm() != Rational(0)) {
                tangent2 = tangent2 / tangent2.norm();
            }
            
            // 计算法线和
            Vector3r normalSum = normal1 + normal2;
            
            // 如果法线和接近零（两法线几乎相反），使用切向量叉积作为碰撞方向
            if (normalSum.squaredNorm() < Rational("1/100")) {
                Vector3r crossProduct = tangent1.cross(tangent2);
                if (crossProduct.squaredNorm() > Rational("1/100")) {
                    // 根据切向量叉积方向确定法线和的方向
                    if (crossProduct.dot(normal1) > Rational(0)) {
                        normalSum = crossProduct;
                    } else {
                        normalSum = -crossProduct;
                    }
                } else {
                    // 如果叉积也接近零（两切向量平行），则使用任一法线
                    normalSum = normal1;
                }
            }
            
            // 确保normalSum不为零向量
            if (normalSum.squaredNorm() < Rational("1/1000")) {
                // 构建任意非零向量作为后备
                normalSum = Vector3r(Rational(0), Rational(0), Rational(1));
            }
            
            // 生成速度大小
            Rational speed1 = speedDist(engine);
            Rational speed2 = speedDist(engine);
            
            // 生成随机速度方向，确保满足(v1-v2)·(n1+n2) > 0的条件
            Vector3r dir1 = generateRandomDirectionInHalfSpace(engine, normalSum);
            Vector3r dir2 = generateRandomDirectionInHalfSpace(engine, -normalSum);
            
            Vector3r vel1 = speed1 * dir1;
            Vector3r vel2 = speed2 * dir2;
            
            // 验证相对速度与法线和的点积是否为正
            Rational dotProduct = (vel1 - vel2).dot(normalSum);
            
            // 如果不满足条件，交换方向
            if (dotProduct <= Rational(0)) {
                std::swap(dir1, dir2);
                vel1 = speed1 * dir1;
                vel2 = speed2 * dir2;
                
                // 再次验证
                dotProduct = (vel1 - vel2).dot(normalSum);
                if (dotProduct <= Rational(0)) {
                    // 如果仍然不满足，直接设置满足条件的速度
                    vel1 = speed1 * normalSum.normalized();
                    vel2 = speed2 * (-normalSum.normalized());
                }
            }
            
            std::cout << "Edge-Edge collision velocity generated." << std::endl;
            std::cout << "Normal sum: " << normalSum.transpose() << std::endl;
            std::cout << "Relative velocity dot normal sum: " << dotProduct << std::endl;
            
            return {vel1, vel2};
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in edgeedge::genColVel: " << e.what() << std::endl;
            // 发生异常时返回默认值，确保满足约束条件
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
        catch (...) {
            std::cerr << "Unknown exception in edgeedge::genColVel" << std::endl;
            // 发生异常时返回默认值
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
    }
};

namespace vertexface
{
    inline Array2r genUV(std::mt19937_64& engine) 
    {
        // 随机选择一个顶点索引 (0, 1, 2)
        std::uniform_int_distribution<int> vertexSelector(0, 2);
        int selectedVertex = vertexSelector(engine);
        
        // 根据选择的顶点索引返回对应的UV坐标
        Array2r uv;
        switch (selectedVertex) {
            case 0: // 第一个顶点 (0, 0)
                uv = Array2r(0.0, 0.0);
                break;
            case 1: // 第二个顶点 (1, 0)
                uv = Array2r(1.0, 0.0);
                break;
            case 2: // 第三个顶点 (0, 1)
                uv = Array2r(0.0, 1.0);
                break;
        }
        
        return uv;
    }

    TriParamBound genLocalParam(const Array2r& uv, Rational radius = 0.1);

    std::pair<Vector3r, Vector3r> genColVel(
        auto& engine,
        const Vector3r& faceNormal,        // 面片法线
        const TriQuadBezier& vertexPatch,  // 顶点所在曲面
        const Array2r& collisionUV         // 顶点参数
    ) 
    {
        try {
            std::uniform_real_distribution<float> speedDist(0.5, 5.0);
            
            // 识别是哪个顶点
            int vertex = -1;
            const Rational epsilon = Rational("1/1000000");
            
            if (collisionUV[0] < epsilon && collisionUV[1] < epsilon) {
                vertex = 0; // 原点 (0,0)
            } else if (std::abs(collisionUV[0] - Rational(1)) < epsilon && collisionUV[1] < epsilon) {
                vertex = 1; // (1,0) 点
            } else if (collisionUV[0] < epsilon && std::abs(collisionUV[1] - Rational(1)) < epsilon) {
                vertex = 2; // (0,1) 点
            } else {
                std::cerr << "Error: not a vertex point in genColVel" << std::endl;
                return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)}; // 默认值
            }
            
            // 归一化面片法线
            Vector3r normalizedFaceNormal = faceNormal;
            if (normalizedFaceNormal.norm() != Rational(0)) {
                normalizedFaceNormal = normalizedFaceNormal / normalizedFaceNormal.norm();
            }
            
            // 计算顶点位置（碰撞点）
            Vector3r vertexPoint = vertexPatch.evaluatePatchPoint(collisionUV);
            
            // 在顶点周围采样多个点，判断patch2在patch1的哪一侧
            const int numSamples = 5;  // 顶点周围采样点数量
            int sameDirectionCount = 0;
            int validSamples = 0;
            
            // 基于顶点类型定义采样偏移量
            Rational safeOffset = Rational(1) / Rational(50); // 参数域内的安全偏移
            std::vector<Array2r> sampleOffsets;
            
            switch (vertex) {
                case 0: // (0,0)
                    sampleOffsets = {
                        Array2r(safeOffset, 0),
                        Array2r(0, safeOffset),
                        Array2r(safeOffset, safeOffset),
                        Array2r(safeOffset/Rational(2), safeOffset/Rational(2)),
                        Array2r(safeOffset*Rational(2), safeOffset/Rational(2))
                    };
                    break;
                    
                case 1: // (1,0) 
                    sampleOffsets = {
                        Array2r(Rational(1)-safeOffset, 0),
                        Array2r(Rational(1), safeOffset),
                        Array2r(Rational(1)-safeOffset, safeOffset),
                        Array2r(Rational(1)-safeOffset/Rational(2), safeOffset/Rational(2)),
                        Array2r(Rational(1)-safeOffset*Rational(2), safeOffset/Rational(2))
                    };
                    break;
                    
                case 2: // (0,1)
                    sampleOffsets = {
                        Array2r(0, Rational(1)-safeOffset),
                        Array2r(safeOffset, Rational(1)),
                        Array2r(safeOffset, Rational(1)-safeOffset),
                        Array2r(safeOffset/Rational(2), Rational(1)-safeOffset/Rational(2)),
                        Array2r(safeOffset/Rational(2), Rational(1)-safeOffset*Rational(2))
                    };
                    break;
            }
            
            // 在采样点计算连线向量并判断与面片法线的关系
            for (const auto& offset : sampleOffsets) {
                try {
                    Vector3r samplePoint = vertexPatch.evaluatePatchPoint(offset);
                    
                    // 计算从顶点指向采样点的向量
                    Vector3r directionVector = samplePoint - vertexPoint;
                    
                    // 归一化方向向量
                    if (directionVector.norm() != Rational(0)) {
                        directionVector = directionVector / directionVector.norm();
                        validSamples++;
                        
                        // 检查连线方向是否与面片法线同向
                        if (directionVector.dot(normalizedFaceNormal) > Rational(0)) {
                            sameDirectionCount++;
                        }
                    }
                } catch (...) {
                    std::cerr << "Failed to evaluate point at offset " << offset[0] << "," << offset[1] << std::endl;
                    continue;
                }
            }
            
            std::cout << "Same direction samples: " << sameDirectionCount << "/" << validSamples << std::endl;
            
            // 判断patch2在patch1的哪一侧
            bool mostlySameDirection = (sameDirectionCount > validSamples / 2);
            
            // 根据相对位置选择合适的半空间生成速度
            // 如果顶点曲面大部分在法线正方向，则第二个速度在法线负方向生成
            Vector3r referenceNormal = mostlySameDirection ? -normalizedFaceNormal : normalizedFaceNormal;
            
            // 生成速度大小
            Rational speed1 = speedDist(engine);
            Rational speed2 = speedDist(engine);
            
            // 在相反的半空间生成速度方向
            Vector3r dir1 = generateRandomDirectionInHalfSpace(engine, referenceNormal);
            Vector3r dir2 = generateRandomDirectionInHalfSpace(engine, -referenceNormal);
            
            // 计算最终速度
            Vector3r vel1 = speed1 * dir1;
            Vector3r vel2 = speed2 * dir2;
            
            // 验证相对速度与参考法线的点积为正
            Rational dotProduct = (vel1 - vel2).dot(referenceNormal);
            
            // 如果不满足条件，调整速度方向
            if (dotProduct <= Rational(0)) {
                // 直接使用确定性方向
                vel1 = speed1 * (-referenceNormal);
                vel2 = speed2 * referenceNormal;
                dotProduct = (vel1 - vel2).dot(referenceNormal);
            }
            
            std::cout << "Vertex-Face collision velocity generated." << std::endl;
            std::cout << "Reference normal: " << referenceNormal.transpose() << std::endl;
            std::cout << "Same direction: " << (mostlySameDirection ? "Yes" : "No") << std::endl;
            std::cout << "Relative velocity dot normal: " << dotProduct << std::endl;
            
            return {vel1, vel2};
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in vertexface::genColVel: " << e.what() << std::endl;
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
        catch (...) {
            std::cerr << "Unknown exception in vertexface::genColVel" << std::endl;
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
    }
};

namespace vertexedge
{
    std::pair<Vector3r, Vector3r> genColVel(
        auto& engine,
        const TriQuadBezier& patch1,  // 边所在曲面
        const TriQuadBezier& patch2,  // 顶点所在曲面
        const Array2r& uv1,          // 边上的参数坐标
        const Array2r& uv2           // 顶点的参数坐标
    ) 
    {
        try {
            std::uniform_real_distribution<float> speedDist(0.5, 5.0);
            
            // 1. 确认碰撞点为顶点
            int vertex = -1;
            const Rational epsilon = Rational("1/1000000");
            
            if (uv2[0] < epsilon && uv2[1] < epsilon) {
                vertex = 0; // 原点 (0,0)
            } else if (std::abs(uv2[0] - Rational(1)) < epsilon && uv2[1] < epsilon) {
                vertex = 1; // (1,0) 点
            } else if (uv2[0] < epsilon && std::abs(uv2[1] - Rational(1)) < epsilon) {
                vertex = 2; // (0,1) 点
            } else {
                std::cerr << "Error: not a vertex point in vertexedge::genColVel" << std::endl;
                return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)}; // 默认值
            }
            
            // // 2. 确认碰撞点在边上
            // int edge = -1;
            // if (std::abs(uv1[1]) < epsilon) {
            //     edge = 0; // 底边: v = 0
            // } else if (std::abs(uv1[0]) < epsilon) {
            //     edge = 1; // 左边: u = 0
            // } else if (std::abs(uv1[0] + uv1[1] - Rational(1)) < epsilon) {
            //     edge = 2; // 斜边: u + v = 1
            // } else {
            //     std::cerr << "Error: not an edge point in vertexedge::genColVel" << std::endl;
            //     return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)}; // 默认值
            // }
            
            // 3. 计算顶点位置（碰撞点）
            Vector3r vertexPoint = patch2.evaluatePatchPoint(uv2);
            // Vector3r edgePoint = patch1.evaluatePatchPoint(uv1);
            
            // // 确认两点确实重合
            // if ((vertexPoint - edgePoint).norm() > Rational("1/1000")) {
            //     std::cerr << "Warning: vertex and edge points do not coincide" << std::endl;
            // }
            
            // 4. 在顶点附近采样多个点
            const int numSamples = 5;  // 顶点周围采样点数量
            Vector3r avgDirection = Vector3r::Zero();
            int validSamples = 0;
            
            // 基于顶点类型定义采样偏移量
            Rational safeOffset = Rational(1) / Rational(50); // 参数域内的安全偏移
            std::vector<Array2r> sampleOffsets;
            
            switch (vertex) {
                case 0: // (0,0)
                    sampleOffsets = {
                        Array2r(safeOffset, 0),
                        Array2r(0, safeOffset),
                        Array2r(safeOffset, safeOffset),
                        Array2r(safeOffset/Rational(2), safeOffset/Rational(2)),
                        Array2r(safeOffset*Rational(2), safeOffset/Rational(2))
                    };
                    break;
                    
                case 1: // (1,0) 
                    sampleOffsets = {
                        Array2r(Rational(1)-safeOffset, 0),
                        Array2r(Rational(1), safeOffset),
                        Array2r(Rational(1)-safeOffset, safeOffset),
                        Array2r(Rational(1)-safeOffset/Rational(2), safeOffset/Rational(2)),
                        Array2r(Rational(1)-safeOffset*Rational(2), safeOffset/Rational(2))
                    };
                    break;
                    
                case 2: // (0,1)
                    sampleOffsets = {
                        Array2r(0, Rational(1)-safeOffset),
                        Array2r(safeOffset, Rational(1)),
                        Array2r(safeOffset, Rational(1)-safeOffset),
                        Array2r(safeOffset/Rational(2), Rational(1)-safeOffset/Rational(2)),
                        Array2r(safeOffset/Rational(2), Rational(1)-safeOffset*Rational(2))
                    };
                    break;
            }
            
            // 5. 计算从顶点到采样点的连线向量并取平均
            for (const auto& offset : sampleOffsets) {
                try {
                    Vector3r samplePoint = patch2.evaluatePatchPoint(offset);
                    
                    // 计算从顶点指向采样点的向量
                    Vector3r directionVector = samplePoint - vertexPoint;
                    
                    // 归一化方向向量
                    if (directionVector.norm() != 0) {
                        directionVector = directionVector / directionVector.norm();
                        avgDirection = avgDirection + directionVector;
                        validSamples++;
                    }
                } catch (...) {
                    std::cerr << "Failed to evaluate point at offset " << offset[0] << "," << offset[1] << std::endl;
                    continue;
                }
            }
            
            // 6. 计算平均方向作为分离方向
            Vector3r separationDirection;
            if (validSamples > 0) {
                avgDirection = avgDirection / Rational(validSamples);
                if (avgDirection.norm() != 0) {
                    separationDirection = avgDirection / avgDirection.norm();
                } else {
                    // 如果平均为零，使用边的法线
                    separationDirection = patch1.evaluateNormal(uv1);
                    if (separationDirection.norm() != 0) {
                        separationDirection = separationDirection / separationDirection.norm();
                    } else {
                        separationDirection = Vector3r(0, 0, 1); // 最后的备选
                    }
                }
            } else {
                // 如果没有有效样本，使用边的法线
                separationDirection = patch1.evaluateNormal(uv1);
                if (separationDirection.norm() != Rational(0)) {
                    separationDirection = separationDirection / separationDirection.norm();
                } else {
                    separationDirection = Vector3r(0, 0, 1); // 最后的备选
                }
            }
            
            // 7. 在分离方向的半空间生成速度
            Rational speed1 = speedDist(engine);
            Rational speed2 = speedDist(engine);
            
            Vector3r dir2 = generateRandomDirectionInHalfSpace(engine, separationDirection);
            Vector3r dir1 = generateRandomDirectionInHalfSpace(engine, -separationDirection);
            
            Vector3r vel2 = speed2 * dir2;
            Vector3r vel1 = speed1 * dir1;
            
            // 确保相对速度与分离方向点积为正
            Rational dotProduct = (vel2 - vel1).dot(separationDirection);
            
            if (dotProduct <= Rational(0)) {
                // 如果不满足条件，直接使用确定性方向
                vel2 = speed2 * separationDirection;
                vel1 = speed1 * (-separationDirection);
            }
            
            std::cout << "Vertex-Edge collision velocity generated." << std::endl;
            std::cout << "Separation direction: " << separationDirection.transpose() << std::endl;
            std::cout << "Valid samples: " << validSamples << "/" << sampleOffsets.size() << std::endl;
            std::cout << "Relative velocity dot separation: " << (vel2 - vel1).dot(separationDirection) << std::endl;
            
            return {vel1, vel2};
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in vertexedge::genColVel: " << e.what() << std::endl;
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
        catch (...) {
            std::cerr << "Unknown exception in vertexedge::genColVel" << std::endl;
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
    }
};

// =====================================================================
// vertexvertex 命名空间：专门用于顶点-顶点(VV)碰撞的速度生成
// 核心思想：同时在两个顶点周围采样，计算两侧的平均展开方向差作为分离方向
// =====================================================================
namespace vertexvertex
{
    /**
     * @brief 识别UV坐标对应的顶点类型
     * @param uv 参数坐标
     * @return 0: (0,0), 1: (1,0), 2: (0,1), -1: 非顶点
     */
    inline int identifyVertex(const Array2r& uv) {
        const Rational epsilon = Rational("1/1000000");
        if (uv[0] < epsilon && uv[1] < epsilon) return 0;           // (0,0)
        if (std::abs(uv[0] - Rational(1)) < epsilon && uv[1] < epsilon) return 1;  // (1,0)
        if (uv[0] < epsilon && std::abs(uv[1] - Rational(1)) < epsilon) return 2;  // (0,1)
        return -1; // 不是顶点
    }

    /**
     * @brief 获取顶点周围的采样偏移量
     * @param vertex 顶点类型 (0, 1, 2)
     * @param safeOffset 安全偏移量
     * @return 采样点坐标列表
     */
    inline std::vector<Array2r> getSampleOffsets(int vertex, const Rational& safeOffset) {
        std::vector<Array2r> offsets;
        switch (vertex) {
            case 0: // (0,0)
                offsets = {
                    Array2r(safeOffset, Rational(0)),
                    Array2r(Rational(0), safeOffset),
                    Array2r(safeOffset, safeOffset),
                    Array2r(safeOffset/Rational(2), safeOffset/Rational(2)),
                    Array2r(safeOffset*Rational(2), safeOffset/Rational(2))
                };
                break;
            case 1: // (1,0) 
                offsets = {
                    Array2r(Rational(1)-safeOffset, Rational(0)),
                    Array2r(Rational(1), safeOffset),
                    Array2r(Rational(1)-safeOffset, safeOffset),
                    Array2r(Rational(1)-safeOffset/Rational(2), safeOffset/Rational(2)),
                    Array2r(Rational(1)-safeOffset*Rational(2), safeOffset/Rational(2))
                };
                break;
            case 2: // (0,1)
                offsets = {
                    Array2r(Rational(0), Rational(1)-safeOffset),
                    Array2r(safeOffset, Rational(1)),
                    Array2r(safeOffset, Rational(1)-safeOffset),
                    Array2r(safeOffset/Rational(2), Rational(1)-safeOffset/Rational(2)),
                    Array2r(safeOffset/Rational(2), Rational(1)-safeOffset*Rational(2))
                };
                break;
            default:
                // 非顶点情况，返回空
                break;
        }
        return offsets;
    }

    /**
     * @brief 计算顶点周围采样点的平均方向
     * @param patch 贝塞尔曲面
     * @param uv 顶点参数坐标
     * @param vertexPoint 顶点3D位置
     * @param vertex 顶点类型
     * @return 平均方向向量（未归一化）
     */
    inline Vector3r computeAverageDirection(
        const TriQuadBezier& patch,
        const Array2r& uv,
        const Vector3r& vertexPoint,
        int vertex
    ) {
        Rational safeOffset = Rational(1) / Rational(50);
        std::vector<Array2r> sampleOffsets = getSampleOffsets(vertex, safeOffset);
        
        Vector3r avgDir = Vector3r::Zero();
        int validSamples = 0;
        
        for (const auto& offset : sampleOffsets) {
            try {
                Vector3r samplePoint = patch.evaluatePatchPoint(offset);
                Vector3r dir = samplePoint - vertexPoint;
                
                if (dir.norm() != Rational(0)) {
                    dir = dir / dir.norm();
                    avgDir = avgDir + dir;
                    validSamples++;
                }
            } catch (...) {
                continue;
            }
        }
        
        if (validSamples > 0) {
            avgDir = avgDir / Rational(validSamples);
        }
        
        return avgDir;
    }

    /**
     * @brief 为顶点-顶点碰撞生成碰撞速度
     * 
     * 核心算法：
     * 1. 在两个顶点周围分别采样，计算各自的平均展开方向
     * 2. 分离方向 = avgDir2 - avgDir1（两侧展开方向的差异）
     * 3. 在分离方向的相反半空间生成两个速度
     * 
     * @param engine 随机数引擎
     * @param patch1 第一个顶点所在曲面
     * @param patch2 第二个顶点所在曲面
     * @param uv1 第一个顶点的参数坐标
     * @param uv2 第二个顶点的参数坐标
     * @return {vel1, vel2} 两个碰撞速度
     */
    std::pair<Vector3r, Vector3r> genColVel(
        auto& engine,
        const TriQuadBezier& patch1,  // 第一个顶点所在曲面
        const TriQuadBezier& patch2,  // 第二个顶点所在曲面
        const Array2r& uv1,           // 第一个顶点参数坐标
        const Array2r& uv2            // 第二个顶点参数坐标
    ) 
    {
        try {
            std::uniform_real_distribution<float> speedDist(0.5, 5.0);
            
            // 1. 识别两个顶点类型
            int vertex1 = identifyVertex(uv1);
            int vertex2 = identifyVertex(uv2);
            
            if (vertex1 == -1 || vertex2 == -1) {
                std::cerr << "Error: not vertex points in vertexvertex::genColVel" << std::endl;
                std::cerr << "  uv1: (" << uv1[0] << ", " << uv1[1] << "), vertex1=" << vertex1 << std::endl;
                std::cerr << "  uv2: (" << uv2[0] << ", " << uv2[1] << "), vertex2=" << vertex2 << std::endl;
                return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
            }
            
            // 2. 获取两个顶点的3D位置
            Vector3r point1 = patch1.evaluatePatchPoint(uv1);
            Vector3r point2 = patch2.evaluatePatchPoint(uv2);
            
            // 3. 在两个顶点周围分别采样，计算平均方向
            Vector3r avgDir1 = computeAverageDirection(patch1, uv1, point1, vertex1);
            Vector3r avgDir2 = computeAverageDirection(patch2, uv2, point2, vertex2);
            
            // 4. 计算分离方向 = patch2的展开方向 - patch1的展开方向
            // 物理意义：从patch1"指向"patch2的方向
            Vector3r separationDir = avgDir2 - avgDir1;
            
            // 5. 处理分离方向为零的情况（两个顶点展开方向相同）
            if (separationDir.squaredNorm() < Rational("1/10000")) {
                // 备选方案：使用两个法线的平均
                Vector3r n1 = patch1.evaluateNormal(uv1);
                Vector3r n2 = patch2.evaluateNormal(uv2);
                separationDir = n1 + n2;
                
                if (separationDir.squaredNorm() < Rational("1/10000")) {
                    // 如果法线和也接近零（法线几乎相反），使用n1
                    separationDir = n1;
                    
                    if (separationDir.squaredNorm() < Rational("1/10000")) {
                        // 最后备选：使用默认方向
                        separationDir = Vector3r(Rational(0), Rational(0), Rational(1));
                    }
                }
            }
            
            // 归一化分离方向
            separationDir = separationDir / separationDir.norm();
            
            // 6. 生成速度大小
            Rational speed1 = speedDist(engine);
            Rational speed2 = speedDist(engine);
            
            // 7. 在相反半空间生成速度方向
            // vel1 在 -separationDir 半空间（patch1侧）
            // vel2 在 +separationDir 半空间（patch2侧）
            Vector3r dir1 = generateRandomDirectionInHalfSpace(engine, -separationDir);
            Vector3r dir2 = generateRandomDirectionInHalfSpace(engine, separationDir);
            
            Vector3r vel1 = speed1 * dir1;
            Vector3r vel2 = speed2 * dir2;
            
            // 8. 验证约束：(vel2 - vel1) · separationDir > 0
            Rational dotProduct = (vel2 - vel1).dot(separationDir);
            
            if (dotProduct <= Rational(0)) {
                // 如果不满足条件，直接使用确定性方向
                vel1 = speed1 * (-separationDir);
                vel2 = speed2 * separationDir;
                dotProduct = (vel2 - vel1).dot(separationDir);
            }
            
            std::cout << "Vertex-Vertex collision velocity generated." << std::endl;
            std::cout << "  Vertex1 type: " << vertex1 << ", Vertex2 type: " << vertex2 << std::endl;
            std::cout << "  avgDir1: " << avgDir1.transpose() << std::endl;
            std::cout << "  avgDir2: " << avgDir2.transpose() << std::endl;
            std::cout << "  Separation direction: " << separationDir.transpose() << std::endl;
            std::cout << "  Relative velocity dot separation: " << dotProduct << std::endl;
            
            return {vel1, vel2};
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in vertexvertex::genColVel: " << e.what() << std::endl;
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
        catch (...) {
            std::cerr << "Unknown exception in vertexvertex::genColVel" << std::endl;
            return {Vector3r(0, 0, 1), Vector3r(0, 0, -1)};
        }
    }
};

namespace degenerate
{
    inline CollisionPoint   generatePartiallyCoincidentPatches(unsigned seed) 
    {
        std::mt19937_64 engine(seed);
        CollisionPoint cp;
        
        std::uniform_real_distribution<float> dist(-5, 5);

        // // 1. 生成基础曲面
        // cp.patch1 = generateRandomPatch(engine, dist, dist, dist);
        // cp.patch2 = cp.patch1; // 先完全复制
        
        // // 2. 随机选择部分控制点保持重合
        // std::uniform_int_distribution<int> coincidentCount(2, 4); // 决定有几个点保持重合
        // int numCoincident = coincidentCount(engine);
        
        // // 创建控制点索引的随机排列
        // std::array<int, 6> indices = {0, 1, 2, 3, 4, 5};
        // std::shuffle(indices.begin(), indices.end(), engine);
        
        // // 3. 修改非重合控制点的位置
        // std::normal_distribution<double> offset(0.0, 1);
        // for (int i = numCoincident; i < 6; ++i) {
        //     int idx = indices[i];
            
        //     // 给非重合控制点添加随机偏移
        //     Vector3r delta(offset(engine), offset(engine), offset(engine));
        //     cp.patch2.ctrlp[idx] += delta;
            
        //     // 确保速度场也不同
        //     Vector3r velDelta(offset(engine)/2, offset(engine)/2, offset(engine)/2);
        //     cp.patch2.velp[idx] += velDelta;
        // }
        
        // // 4. 生成碰撞参数
        // // 优先在重合区域寻找碰撞点
        // Array2r uv = findIntersectionParameter(cp.patch1, cp.patch2);
        // cp.uv1 = cp.uv2 = uv;
        
        // // 5. 生成局部参数边界
        // cp.local_uv1 = cp.uv1;
        // cp.local_uv2 = cp.uv2;
        
        // // 6. 计算法线
        // cp.normal1 = cp.patch1.evaluateNormal(cp.uv1);
        // cp.normal2 = cp.patch2.evaluateNormal(cp.uv2);
        
        // // 7. 生成相对速度场 - 保证分离
        // std::uniform_real_distribution<float> speedDist(0.5, 2.0);
        // Rational speed = speedDist(engine);
        
        // // 使用法线方向作为分离方向
        // Vector3r separationDir = cp.normal1;
        // if (separationDir.norm() != Rational(0)) {
        //     separationDir = separationDir / separationDir.norm();
        // } else {
        //     separationDir = Vector3r(0, 0, 1);
        // }
        
        // cp.vel1 = speed * separationDir;
        // cp.vel2 = -speed * separationDir;
        
        return cp;
    }

    inline CollisionPoint generateFullyCoincidentPatches(unsigned seed)
    {
        std::mt19937_64 engine(seed);
        CollisionPoint cp;
        
        std::uniform_real_distribution<float> dist(-5, 5);
        
        // 1. 生成基础曲面
        cp.patch1 = generateRandomPatch(engine, dist, dist, dist);
        cp.patch2 = cp.patch1; // 完全复制控制点位置
        
        cp.uv1 = cp.uv2 = generateValidTriangleUV(engine);
        cp.normal1 = cp.patch1.evaluateNormal(cp.uv1);
        cp.normal2 = cp.patch2.evaluateNormal(cp.uv2);

        // 2. 生成不同的速度场
        std::uniform_real_distribution<float> speedDist(0.5, 3.0);
        
        // 给两个面片设置不同的速度
        for (int i = 0; i < 6; ++i) {
            
            // 在法线方向所在半空间随机速度
            Vector3r dir = generateRandomDirectionInHalfSpace(engine, cp.normal1);
            Rational speed = speedDist(engine);
            cp.patch1.velp[i] = speed * dir;
            // 直接取反
            cp.patch2.velp[i] = -cp.patch1.velp[i];
        }
        
        // 3. 生成碰撞参数 - 可以是曲面上任意点
        
        // 插值计算vel
        cp.vel1 = cp.patch1.evaluatePatchVel(cp.uv1);
        cp.vel2 = -cp.vel1;
        
        // 4. 将原patch按照uv1和uv2切分局部
        TriParamBound bound1 = generateLocalParamBound(cp.uv1, Rational("1/9"));
        TriParamBound bound2 = generateLocalParamBound(cp.uv2, Rational("1/9"));
        
        TriQuadBezier localPatch1 = cp.patch1.divideBezierPatch(bound1);
        TriQuadBezier localPatch2 = cp.patch2.divideBezierPatch(bound2);
        localPatch1.velp = cp.patch1.divideBezierPatch(bound1, cp.patch1.velp);
        localPatch2.velp = cp.patch2.divideBezierPatch(bound2, cp.patch2.velp);
        cp.patch1 = localPatch1;
        cp.patch2 = localPatch2;
        

        return cp;
    }

    // 生成面-线接触情形：一个曲面退化为直线，另一个正常曲面与直线相切
    inline CollisionPoint generateSurfaceLineContact(unsigned seed)
    {
        std::mt19937_64 engine(seed);
        CollisionPoint cp;
        
        std::uniform_real_distribution<float> dist(-5, 5);
        
        // 1. 首先生成一条直线(退化的曲面)
        // 随机生成两个端点
        Vector3r lineStart(dist(engine), dist(engine), dist(engine));
        Vector3r lineDir(dist(engine), dist(engine), dist(engine));
        lineDir = lineDir / lineDir.norm(); // 归一化方向向量
        
        // 计算直线上的几个点作为控制点
        std::array<Vector3r, 6> degeneratedPoints;
        std::uniform_real_distribution<float> lineDist(0.2, 1.0);
        for (int i = 0; i < 6; ++i) {
            // 沿直线均匀分布控制点，添加微小扰动避免数值问题
            Rational t = Rational(i) / Rational(5); // 0到1均匀分布
            degeneratedPoints[i] = lineStart + t * lineDir * lineDist(engine);
        }
        
        // 2. 创建退化的曲面(实际是一条直线)
        cp.patch1 = TriQuadBezier(degeneratedPoints);
        
        // 3. 生成正常曲面
        cp.patch2 = generateRandomPatch(engine, dist, dist, dist);
        
        // 4. 计算直线上的任意一点作为碰撞点
        std::uniform_real_distribution<float> collisionDist(0.3, 0.7);
        Rational t = collisionDist(engine);
        Vector3r collisionPoint = lineStart + t * lineDir * lineDist(engine);
        
        // 5. 选择正常曲面上的一点作为碰撞点
        cp.uv2 = generateValidTriangleUV(engine);
        Vector3r patch2Point = cp.patch2.evaluatePatchPoint(cp.uv2);
        
        // 6. 移动正常曲面使其与直线相交
        Vector3r offset = collisionPoint - patch2Point;
        for (int i = 0; i < 6; ++i) {
            cp.patch2.ctrlp[i] = cp.patch2.ctrlp[i] + offset;
        }
        
        // 7. 计算正常曲面在接触点的法线
        Vector3r patch2Normal = cp.patch2.evaluateNormal(cp.uv2);
        
        // 8. 调整正常曲面使其法线垂直于直线方向(实现相切)
        // 我们需要旋转patch2使其法线与lineDir垂直
        Rational dotProduct = patch2Normal.dot(lineDir);
        
        if (std::abs(dotProduct) > Rational("1/1000")) {
            // 创建旋转矩阵使patch2Normal垂直于lineDir
            // 首先计算旋转轴(法线与直线的叉积)
            Vector3r rotAxis = patch2Normal.cross(lineDir);
            if (rotAxis.norm() < Rational("1/1000")) {
                // 如果平行，选择一个任意垂直轴
                Vector3r temp(1, 0, 0);
                if (std::abs(lineDir.dot(temp)) > Rational("0.9")) {
                    temp = Vector3r(0, 1, 0);
                }
                rotAxis = lineDir.cross(temp);
            }
            rotAxis = rotAxis / rotAxis.norm();
            
            // 计算旋转角度(弧度)
            double angle = std::acos(std::clamp(static_cast<double>(patch2Normal.dot(lineDir)), -1.0, 1.0));
            angle = 3.14159265358979323846/2 - angle; // 我们需要旋转到垂直状态
            
            // 应用旋转到patch2
            Eigen::AngleAxisd rotation(angle, Eigen::Vector3d(rotAxis[0], 
                                                             rotAxis[1], 
                                                             rotAxis[2]));
            Eigen::Matrix3d rotMatrix = rotation.toRotationMatrix();
            
            // 获取接触点位置(旋转中心)
            Vector3r pivot = cp.patch2.evaluatePatchPoint(cp.uv2);
            
            // 旋转每个控制点
            for (int i = 0; i < 6; ++i) {
                Vector3r p = cp.patch2.ctrlp[i] - pivot; // 移动到原点
                Eigen::Vector3d p_double(p[0], 
                                        p[1], 
                                        p[2]);
                Eigen::Vector3d rotated = rotMatrix * p_double;
                cp.patch2.ctrlp[i] = pivot + Vector3r(Rational(rotated[0]), 
                                                     Rational(rotated[1]), 
                                                     Rational(rotated[2]));
            }
        }
        
        // 9. 设置碰撞参数
        // 为退化曲面找一个合理的参数坐标
        cp.uv1 = Array2r(Rational("1/2"), Rational("0"));  // 底边中点
        
        // 10. 生成速度
        std::uniform_real_distribution<float> speedDist(0.5, 3.0);
        Rational speed1 = speedDist(engine);
        Rational speed2 = speedDist(engine);
        
        // 计算分离向量(从直线指向曲面)
        Vector3r separationDir = cp.patch2.evaluateNormal(cp.uv2);
        if (separationDir.dot(lineDir) > Rational(0)) {
            separationDir = -separationDir; // 确保分离方向朝向远离直线
        }
        
        // 设置速度
        cp.vel1 = speed1 * (-separationDir);  // 直线速度
        cp.vel2 = speed2 * separationDir;     // 曲面速度
        
        // 根据速度设置速度场
        for (int i = 0; i < 6; ++i) {
            cp.patch1.velp[i] = cp.vel1;
            cp.patch2.velp[i] = cp.vel2;
        }
        
        // 11. 计算局部参数域
        TriParamBound bound1 = generateLocalParamBound(cp.uv1, Rational("1/5"));
        TriParamBound bound2 = generateLocalParamBound(cp.uv2, Rational("1/5"));
        cp.local_uv1 = computeLocalUV(BaryCoord(cp.uv1), bound1);
        cp.local_uv2 = computeLocalUV(BaryCoord(cp.uv2), bound2);
        
        // 12. 切割出局部曲面
        TriQuadBezier localPatch1 = cp.patch1.divideBezierPatch(bound1);
        TriQuadBezier localPatch2 = cp.patch2.divideBezierPatch(bound2);
        localPatch1.velp = cp.patch1.divideBezierPatch(bound1, cp.patch1.velp);
        localPatch2.velp = cp.patch2.divideBezierPatch(bound2, cp.patch2.velp);
        
        // 13. 更新法线
        cp.normal1 = localPatch1.evaluateNormal(cp.local_uv1);
        cp.normal2 = localPatch2.evaluateNormal(cp.local_uv2);
        
        // 更新碰撞点结构
        cp.patch1 = localPatch1;
        cp.patch2 = localPatch2;
        
        std::cout << "Generated surface-line contact case" << std::endl;
        std::cout << "Line direction: " << lineDir.transpose() << std::endl;
        std::cout << "Surface normal: " << cp.normal2.transpose() << std::endl;
        std::cout << "Dot product: " << cp.normal2.dot(lineDir) << std::endl;
        
        return cp;
    }

    // 生成共面但有缝隙的两个片段（例如，两个曲面沿边缘相接）
    inline CollisionPoint generateCoplanarWithGapPatches(unsigned seed)
    {
        std::mt19937_64 engine(seed);
        CollisionPoint cp;
        
        std::uniform_real_distribution<float> dist(-5, 5);

        // 1. 生成基础曲面
        cp.patch1 = generateRandomPatch(engine, dist, dist, dist);
        
        // 2. 选择一条边，创建沿这条边对齐的第二个曲面
        std::uniform_int_distribution<int> edgeSelector(0, 2);
        int selectedEdge = edgeSelector(engine);
        
        // 复制第一个曲面，然后对其进行变换
        cp.patch2 = cp.patch1;
        
        return cp;
    }

    // 生成控制点有一部分完全共线的两个曲面
    inline CollisionPoint generatePartiallyColinearControlPoints(unsigned seed)
    {
        std::mt19937_64 engine(seed);
        CollisionPoint cp;
        
        return cp;
    }
}


//解方程
bool solve2x6LinearSystem(
    const Eigen::Matrix<Rational, 2, 6>& A, 
    const Eigen::Matrix<Rational, 2, 1>& b,
    std::array<Vector3r, 2>& solution);
// 法线约束之解方程
void adjustPatchToOrthogonalDirectSolve(TriQuadBezier& patch, const Array2r& uv, const Vector3r& targetNormal);
// 切向量约束
// 调整patch使其边界切向量为目标切向量
void adjustPatchBoundaryTangent(TriQuadBezier& patch, const Array2r& uv, const Vector3r& targetTangent);






