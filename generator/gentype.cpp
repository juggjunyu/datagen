#include <Eigen/Dense>
#include <rational/rational.hpp>
#include <random>
#include "collisionDetector.h"
#include "gentype.h"




Array2r computeLocalUV(const BaryCoord& originalCoord, const TriParamBound& bound) 
{
    Vector3r origP = originalCoord.u * Vector3r(1,0,0) + 
                    originalCoord.v * Vector3r(0,1,0) + 
                    originalCoord.w * Vector3r(0,0,1);

    Matrix3r M;
    M.col(0) = Vector3r(bound.nodes[0].u, bound.nodes[0].v, bound.nodes[0].w);
    M.col(1) = Vector3r(bound.nodes[1].u, bound.nodes[1].v, bound.nodes[1].w);
    M.col(2) = Vector3r(bound.nodes[2].u, bound.nodes[2].v, bound.nodes[2].w);

    // M * x = origP
    Vector3r localBary = M.inverse() * origP;

    // 转换回uv坐标
    Array2r localUV;
    localUV << localBary[0], localBary[1];
    return localUV;
}


std::array<Vector3r, 6> generateVelocityFieldIndependent(
    std::mt19937_64& engine,
    const Vector3r& normal) 
{
    std::uniform_real_distribution<float> dist(-10.0, 10.0);  // 速度分量范围
    std::array<Vector3r, 6> velocities;
    Vector3r standardNormal = normal;

    if(normal == Vector3r(0,0,0))
        standardNormal = Vector3r(0,0,1);
    std::cout << "velocity: " << std::endl;
    // 为每个控制点独立生成速度
    for(int i = 0; i < 6; i++) {
        Vector3r velocity;
        
        // 不断尝试直到生成的速度在法线半空间内
        do {
            // 直接随机生成速度的三个分量
            velocity = Vector3r(
                Rational(dist(engine)),
                Rational(dist(engine)),
                Rational(dist(engine))
            );
            
        } while (velocity.dot(standardNormal) <= Rational(0));  // 确保在半空间内
        
        velocities[i] = velocity;
        // std::cout << velocity << std::endl;
    }
    
    return velocities;
}


// 为Edge-Edge碰撞生成速度场，确保满足(v1-v2)·(n1+n2) > 0的条件
std::pair<std::array<Vector3r, 6>, std::array<Vector3r, 6>> generateVelocityFieldEdgeEdge(
    std::mt19937_64& engine,
    const TriQuadBezier& patch1,
    const TriQuadBezier& patch2,
    const Array2r& uv1,
    const Array2r& uv2)
{
    std::uniform_real_distribution<float> dist(-10.0, 10.0);  // 速度分量范围
    
    // 计算两个patch在碰撞点的法线
    Vector3r normal1 = patch1.evaluateNormal(uv1);
    Vector3r normal2 = patch2.evaluateNormal(uv2);
    Vector3r normalSum = normal1 + normal2;
    
    std::array<Vector3r, 6> velocities1;
    std::array<Vector3r, 6> velocities2;
    
    std::cout << "Generating Edge-Edge velocity field..." << std::endl;
    std::cout << "Normal1: " << normal1.transpose() << std::endl;
    std::cout << "Normal2: " << normal2.transpose() << std::endl;
    std::cout << "Normal sum: " << normalSum.transpose() << std::endl;
    
    // 为patch1的每个控制点生成速度
    for(int i = 0; i < 6; i++) {
        Vector3r velocity1;
        
        // 生成patch1的速度，倾向于在normalSum方向
        do {
            velocity1 = Vector3r(
                Rational(dist(engine)),
                Rational(dist(engine)),
                Rational(dist(engine))
            );
        } while (velocity1.dot(normalSum) <= Rational(0));  // 确保在normalSum半空间内
        
        velocities1[i] = velocity1;
    }
    
    // 为patch2的每个控制点生成速度
    for(int i = 0; i < 6; i++) {
        Vector3r velocity2;
        
        // 生成patch2的速度，倾向于在-normalSum方向
        do {
            velocity2 = Vector3r(
                Rational(dist(engine)),
                Rational(dist(engine)),
                Rational(dist(engine))
            );
        } while (velocity2.dot(-normalSum) <= Rational(0));  // 确保在-normalSum半空间内
        
        velocities2[i] = velocity2;
    }
    
    // 验证相对速度条件
    // 使用控制点的平均速度来近似验证
    Vector3r avgVel1 = Vector3r::Zero();
    Vector3r avgVel2 = Vector3r::Zero();
    for(int i = 0; i < 6; i++) {
        avgVel1 += velocities1[i];
        avgVel2 += velocities2[i];
    }
    avgVel1 /= Rational(6);
    avgVel2 /= Rational(6);
    
    Rational dotProduct = (avgVel1 - avgVel2).dot(normalSum);
    std::cout << "Average relative velocity dot normal sum: " << dotProduct << std::endl;
    
    if (dotProduct <= Rational(0)) {
        std::cout << "Warning: Average relative velocity condition not satisfied, swapping velocities..." << std::endl;
        // 如果平均速度不满足条件，交换两组速度
        std::swap(velocities1, velocities2);
    }
    
    return {velocities1, velocities2};
}


TriParamBound generateFixedParamBound() {
    // 使用简单分数作为顶点坐标
    Array2r pointA(Rational(1)/Rational(4), Rational(1)/Rational(4));
    Array2r pointB(Rational(3)/Rational(4), Rational(1)/Rational(4));
    Array2r pointC(Rational(1)/Rational(2), Rational(1)/Rational(2));
    
    return TriParamBound{pointA, pointB, pointC};
}

TriParamBound generateLocalParamBound(const Array2r& uv, Rational radius) 
{
    // 将输入的uv转换为重心坐标
    BaryCoord center(uv);
    
    // 确保半径是2的方幂分数
    int powerOfTwo = 4; // 对应1/16的精度
    Rational powerRadius = Rational(1) / Rational(1 << powerOfTwo); // 1/16
    
    // 调整powerOfTwo获取合适的半径
    if (radius < powerRadius) {
        while (radius < powerRadius && powerOfTwo < 8) {
            powerOfTwo++;
            powerRadius = Rational(1) / Rational(1 << powerOfTwo);
        }
    }
    else if (radius > powerRadius * Rational(2)) {
        while (radius > powerRadius * Rational(2) && powerOfTwo > 2) {
            powerOfTwo--;
            powerRadius = Rational(1) / Rational(1 << powerOfTwo);
        }
    }
    
    // 确保三角形不会超出参数域边界
    Rational ru = std::min(powerRadius, std::min(center.u, Rational(1)-center.u));
    Rational rv = std::min(powerRadius, std::min(center.v, Rational(1)-center.v));
    Rational rw = std::min(powerRadius, std::min(center.w, Rational(1)-center.w));
    
    // 转换到2的方幂分数
    int ruPower = powerOfTwo;
    int rvPower = powerOfTwo;
    int rwPower = powerOfTwo;
    
    while ((Rational(1) / Rational(1 << ruPower)) > ru && ruPower < 8) ruPower++;
    while ((Rational(1) / Rational(1 << rvPower)) > rv && rvPower < 8) rvPower++;
    while ((Rational(1) / Rational(1 << rwPower)) > rw && rwPower < 8) rwPower++;
    
    ru = Rational(1) / Rational(1 << ruPower);
    rv = Rational(1) / Rational(1 << rvPower);
    rw = Rational(1) / Rational(1 << rwPower);
    
    // 关键修改：使用重心坐标直接创建三角形
    // 使用原始点作为三角形内一点，通过重心坐标混合创建三角形
    std::array<BaryCoord, 3> vertices;
    
    // 首先创建三个方向的基本顶点，确保都是2的方幂分数
    BaryCoord dirU(Rational(1), Rational(0), Rational(0));
    BaryCoord dirV(Rational(0), Rational(1), Rational(0));
    BaryCoord dirW(Rational(0), Rational(0), Rational(1));
    
    // 设置三个顶点，使center点位于三角形内部
    // 这里使用三分之一权重确保center在三角形中心
    Rational weight = Rational(1)/Rational(3);
    
    vertices[0] = BaryCoord(
        center.u * weight + dirU.u * (Rational(1) - weight),
        center.v * weight + dirU.v * (Rational(1) - weight),
        center.w * weight + dirU.w * (Rational(1) - weight)
    );
    
    vertices[1] = BaryCoord(
        center.u * weight + dirV.u * (Rational(1) - weight),
        center.v * weight + dirV.v * (Rational(1) - weight),
        center.w * weight + dirV.w * (Rational(1) - weight)
    );
    
    vertices[2] = BaryCoord(
        center.u * weight + dirW.u * (Rational(1) - weight),
        center.v * weight + dirW.v * (Rational(1) - weight),
        center.w * weight + dirW.w * (Rational(1) - weight)
    );
    
    // 将所有顶点调整为2的方幂分数，同时保持center在三角形内
    for(int i = 0; i < 3; i++) {
        // 调整u坐标为2的方幂分数
        int denom = 1;
        while (Rational(1) / Rational(1 << denom) > vertices[i].u) denom++;
        Rational frac = Rational(1) / Rational(1 << denom);
        int num = static_cast<int>(vertices[i].u / frac);
        vertices[i].u = Rational(num) * frac;
        
        // 调整v坐标为2的方幂分数
        denom = 1;
        while (Rational(1) / Rational(1 << denom) > vertices[i].v) denom++;
        frac = Rational(1) / Rational(1 << denom);
        num = static_cast<int>(vertices[i].v / frac);
        vertices[i].v = Rational(num) * frac;
        
        // 确保u+v不超过1
        if(vertices[i].u + vertices[i].v > Rational(1)) {
            // 如果超过1，按比例缩小，但使用2的方幂缩放因子
            int scalePower = 1;
            while(Rational(1) - Rational(1) / Rational(1 << scalePower) < vertices[i].u + vertices[i].v) {
                scalePower++;
            }
            Rational scale = Rational(1) - Rational(1) / Rational(1 << scalePower);
            vertices[i].u = vertices[i].u * scale;
            vertices[i].v = vertices[i].v * scale;
            
            // 重新调整为2的方幂分数
            for(int j = 0; j < 2; j++) {
                Rational& coord = (j == 0) ? vertices[i].u : vertices[i].v;
                denom = 1;
                while (Rational(1) / Rational(1 << denom) > coord) denom++;
                frac = Rational(1) / Rational(1 << denom);
                num = static_cast<int>(coord / frac);
                coord = Rational(num) * frac;
            }
        }
        
        // 重新计算w确保总和为1
        vertices[i].w = Rational(1) - vertices[i].u - vertices[i].v;
    }
    
    // 最终验证：确保center点在三角形内部
    Vector3r centerPos(center.u, center.v, center.w);
    Matrix3r M;
    M.col(0) = Vector3r(vertices[0].u, vertices[0].v, vertices[0].w);
    M.col(1) = Vector3r(vertices[1].u, vertices[1].v, vertices[1].w);
    M.col(2) = Vector3r(vertices[2].u, vertices[2].v, vertices[2].w);
    
    // 解方程 M * bary = centerPos
    Vector3r bary = M.inverse() * centerPos;
    
    // 检查重心坐标是否都是正的(允许小误差)
    bool centerInside = (bary[0] >= -1e-10) && (bary[1] >= -1e-10) && (bary[2] >= -1e-10);
    
    // 如果center不在三角形内，使用简单的内接三角形
    if(!centerInside) {
        // 创建一个以center为重心的小三角形
        return TriParamBound{
            BaryCoord(center.u + Rational(1)/Rational(16), center.v, center.w - Rational(1)/Rational(16)),
            BaryCoord(center.u, center.v + Rational(1)/Rational(16), center.w - Rational(1)/Rational(16)),
            BaryCoord(center.u - Rational(1)/Rational(32), center.v - Rational(1)/Rational(32), center.w + Rational(1)/Rational(16))
        };
    }
    
    return TriParamBound(vertices);
}

std::pair<Rational, Rational> calculatePrincipalCurvatures(const TriQuadBezier& patch, const Array2r& uv) 
{
    BaryCoord coord(uv);
    Rational u = coord.u;
    Rational v = coord.v;
    Rational w = coord.w;
    
    // 计算第一基本形式 (E, F, G)
    // u方向导数的基函数系数
    std::vector<Rational> dBu = {
        -Rational("2")*w,                // dB_002/du = -2w
        Rational(2)*w - Rational(2)*u, // dB_101/du = 2w - 2u
        Rational(2)*u,                 // dB_200/du = 2u
        -Rational(2)*v,                // dB_011/du = -2v
        Rational(2)*v,                 // dB_110/du = 2v
        Rational(0)                    // dB_020/du = 0
    };
    
    // v方向导数的基函数系数
    std::vector<Rational> dBv = {
        -Rational(2)*w,                // dB_002/dv = -2w
        -Rational(2)*u,                // dB_101/dv = -2u
        Rational(0),                   // dB_200/dv = 0
        Rational(2)*w - Rational(2)*v, // dB_011/dv = 2w - 2v
        Rational(2)*u,                 // dB_110/dv = 2u
        Rational(2)*v                  // dB_020/dv = 2v
    };
    
    // 计算二阶导数的基函数系数
    std::vector<Rational> dBuu = {
        Rational(2),      // d²B_002/du² = 2
        -Rational(4),     // d²B_101/du² = -4
        Rational(2),      // d²B_200/du² = 2
        Rational(0),      // d²B_011/du² = 0
        Rational(0),      // d²B_110/du² = 0
        Rational(0)       // d²B_020/du² = 0
    };
    
    std::vector<Rational> dBvv = {
        Rational(2),      // d²B_002/dv² = 2
        Rational(0),      // d²B_101/dv² = 0
        Rational(0),      // d²B_200/dv² = 0
        -Rational(4),     // d²B_011/dv² = -4
        Rational(0),      // d²B_110/dv² = 0
        Rational(2)       // d²B_020/dv² = 2
    };
    
    std::vector<Rational> dBuv = {
        Rational(2),      // d²B_002/dudv = 2
        -Rational(2),     // d²B_101/dudv = -2
        Rational(0),      // d²B_200/dudv = 0
        -Rational(2),     // d²B_011/dudv = -2
        Rational(2),      // d²B_110/dudv = 2
        Rational(0)       // d²B_020/dudv = 0
    };
    
    // 计算曲面导数
    Vector3r du = Vector3r::Zero();
    Vector3r dv = Vector3r::Zero();
    for(int i = 0; i < 6; i++) {
        du = du + dBu[i] * patch.ctrlp[i];
        dv = dv + dBv[i] * patch.ctrlp[i];
    }
    
    // 计算法向量
    Vector3r normal = du.cross(dv);
    normal.normalize();
    
    // 计算二阶导数
    Vector3r duu = Vector3r::Zero();
    Vector3r dvv = Vector3r::Zero();
    Vector3r duv = Vector3r::Zero();
    
    for(int i = 0; i < 6; i++) {
        duu = duu + dBuu[i] * patch.ctrlp[i];
        dvv = dvv + dBvv[i] * patch.ctrlp[i];
        duv = duv + dBuv[i] * patch.ctrlp[i];
    }
    
    // 第一基本形式
    Rational E = du.dot(du);
    Rational F = du.dot(dv);
    Rational G = dv.dot(dv);
    
    // 第二基本形式
    Rational L = normal.dot(duu);
    Rational M = normal.dot(duv);
    Rational N = normal.dot(dvv);
    
    // 计算主曲率
    // 高斯曲率 K = (LN - M²)/(EG - F²)
    Rational K = (L*N - M*M)/(E*G - F*F);
    
    // 平均曲率 H = (EN - 2FM + GL)/(2(EG - F²))
    Rational H = (E*N - Rational(2)*F*M + G*L)/(Rational(2)*(E*G - F*F));
    
    // 主曲率 k₁, k₂ = H ± √(H² - K)
    // 因为我们不想使用sqrt，可以直接返回H和K，或者近似计算
    // 这里我们使用平均曲率和高斯曲率推算主曲率
    
    // 转换为浮点数进行开方运算
    double h = (double)H;
    double k = (double)K;
    double k1 = h + std::sqrt(h*h - k);
    double k2 = h - std::sqrt(h*h - k);
    
    return {Rational(k1), Rational(k2)};
}

Vector3r computeBoundaryTangent(const TriQuadBezier& patch, const Array2r& uv) 
{
    // 确定点在哪条边上
    int edge = -1;
    const Rational epsilon = 1e-10;
    
    if (std::abs(uv[1]) < epsilon) 
        // 底边: v = 0
        edge = 0;
    else if (std::abs(uv[0]) < epsilon) 
        // 左边: u = 0
        edge = 1;
    else if (std::abs(uv[0] + uv[1] - Rational(1)) < epsilon) 
        // 斜边: u + v = 1
        edge = 2;
    
    // 根据边确定参数方向
    Vector2r paramDir;
    switch (edge) {
        case 0: // 底边: 沿u方向
            paramDir = Vector2r(1, 0);
            break;
        case 1: // 左边: 沿v方向
            paramDir = Vector2r(0, 1);
            break;
        case 2: // 斜边: 保持u+v=1的方向
            paramDir = Vector2r(1, -1);
            break;
        default:
            // 点不在边界上
            std::cerr << "point isn't on boundary" << std::endl;
            return Vector3r::Zero();
    }
    
    // 计算曲面在该点的偏导数
    Vector3r Su = patch.evaluatePartialU(uv);
    Vector3r Sv = patch.evaluatePartialV(uv);
    
    std::cout << "Su: " << Su.transpose() << std::endl;
    std::cout << "Sv: " << Sv.transpose() << std::endl;

    // 计算3D空间中的切向量
    Vector3r tangent = Su * paramDir[0] + Sv * paramDir[1];
    std::cout << "tangent: " << tangent.transpose() << std::endl;
    
    return tangent;
}

namespace edgeface {


// 在 gentype.cpp 的 edgeface 命名空间中

TriParamBound genLocalParam(const Array2r& uv, Rational radius) 
{
    // 步骤1: 判断点在哪条边上
    int edge = -1;
    const Rational epsilon = Rational(1) / Rational(1000000000);
    
    if (std::abs(uv[1]) < epsilon) 
        edge = 0; // 底边: v = 0
    else if (std::abs(uv[0]) < epsilon) 
        edge = 1; // 左边: u = 0
    else if (std::abs(uv[0] + uv[1] - Rational(1)) < epsilon) 
        edge = 2; // 斜边: u + v = 1
    else {
        std::cerr << "Error: point not on boundary in genLocalParam" << std::endl;
        // 降级为标准方法
        return generateLocalParamBound(uv, radius);
    }
    
    std::cout << "Point on edge " << edge << ", uv = (" << uv[0] << ", " << uv[1] << ")" << std::endl;
    
    // 步骤2: 提取边界参数t（在该边上的位置参数）
    Rational t;
    if (edge == 0) t = uv[0];        // 底边上，t = u
    else if (edge == 1) t = uv[1];   // 左边上，t = v
    else t = uv[0];                  // 斜边上，t = u
    
    // 步骤3: 将t调整为2的方幂分数（如果还不是）
    // 找到最接近的2的方幂分数
    int powerOfTwo = 1;
    while (Rational(1) / Rational(1 << powerOfTwo) > t && powerOfTwo < 10) {
        powerOfTwo++;
    }
    
    // 使用分母为2^powerOfTwo的分数逼近t
    Rational denominator = Rational(1 << powerOfTwo);
    int numerator = static_cast<int>((double)t * (1 << powerOfTwo) + 0.5);
    t = Rational(numerator) / denominator;
    
    std::cout << "Adjusted t = " << t << " (power of 2: 2^-" << powerOfTwo << ")" << std::endl;
    
    // 步骤4: 确定边界方向和垂直方向的扩展半径
    // 确保半径也是2的方幂
    int radiusPower = 3; // 默认 1/8
    Rational powerRadius = Rational(1) / Rational(1 << radiusPower);
    
    // 调整半径使其接近输入的radius
    // while (powerRadius > radius && radiusPower < 6) {
    //     radiusPower++;
    //     powerRadius = Rational(1) / Rational(1 << radiusPower);
    // }
    
    // 边界方向的扩展距离（沿着边）
    Rational edgeRadius = std::min(powerRadius, std::min(t, Rational(1) - t));
    
    // 确保edgeRadius也是2的方幂
    // int edgeRadiusPower = radiusPower;
    // while (Rational(1) / Rational(1 << edgeRadiusPower) > edgeRadius) {
    //     edgeRadiusPower++;
    // }
    // edgeRadius = Rational(1) / Rational(1 << edgeRadiusPower);
    
    // 垂直方向的扩展距离（进入三角形内部）
    Rational innerRadius = powerRadius;
    
    std::cout << "Edge radius = " << edgeRadius << ", Inner radius = " << innerRadius << std::endl;
    
    // 步骤5: 根据边的类型生成三个顶点
    std::array<BaryCoord, 3> vertices;
    
    switch (edge) {
        case 0: { // 底边: v = 0
            // 三个顶点：沿边的两个点 + 一个内部点
            Rational t1 = std::max(Rational(0), t - edgeRadius);
            Rational t2 = std::min(Rational(1), t + edgeRadius);
            
            vertices[0] = BaryCoord(t1, Rational(0), Rational(1) - t1);
            vertices[1] = BaryCoord(t2, Rational(0), Rational(1) - t2);
            vertices[2] = BaryCoord(t, innerRadius, Rational(1) - t - innerRadius);
            
            std::cout << "Bottom edge triangle:" << std::endl;
            break;
        }
        
        case 1: { // 左边: u = 0
            Rational t1 = std::max(Rational(0), t - edgeRadius);
            Rational t2 = std::min(Rational(1), t + edgeRadius);
            
            vertices[0] = BaryCoord(Rational(0), t1, Rational(1) - t1);
            vertices[1] = BaryCoord(Rational(0), t2, Rational(1) - t2);
            vertices[2] = BaryCoord(innerRadius, t, Rational(1) - innerRadius - t);
            
            std::cout << "Left edge triangle:" << std::endl;
            break;
        }
        
        case 2: { // 斜边: u + v = 1
            Rational t1 = std::max(Rational(0), t - edgeRadius);
            Rational t2 = std::min(Rational(1), t + edgeRadius);
            
            vertices[0] = BaryCoord(t1, Rational(1) - t1, Rational(0));
            vertices[1] = BaryCoord(t2, Rational(1) - t2, Rational(0));
            vertices[2] = BaryCoord(t, Rational(1) - t - innerRadius, innerRadius);
            
            std::cout << "Hypotenuse edge triangle:" << std::endl;
            break;
        }
    }
    
    // 步骤6: 验证所有顶点的重心坐标
    for (int i = 0; i < 3; i++) {
        std::cout << "Vertex " << i << ": u=" << vertices[i].u 
                  << ", v=" << vertices[i].v 
                  << ", w=" << vertices[i].w << std::endl;
        
        // 确保u, v, w都在[0,1]范围内
        if (vertices[i].u < Rational(0) || vertices[i].u > Rational(1) ||
            vertices[i].v < Rational(0) || vertices[i].v > Rational(1) ||
            vertices[i].w < Rational(0) || vertices[i].w > Rational(1)) {
            std::cerr << "Warning: Vertex " << i << " out of bounds!" << std::endl;
        }
    }
    
    // 步骤7: 验证原始点在生成的三角形内
    BaryCoord originalCoord(uv);
    Vector3r origP(originalCoord.u, originalCoord.v, originalCoord.w);
    
    Matrix3r M;
    M.col(0) = Vector3r(vertices[0].u, vertices[0].v, vertices[0].w);
    M.col(1) = Vector3r(vertices[1].u, vertices[1].v, vertices[1].w);
    M.col(2) = Vector3r(vertices[2].u, vertices[2].v, vertices[2].w);
    
    Vector3r localBary = M.inverse() * origP;
    
    std::cout << "Local barycentric coordinates: " << localBary.transpose() << std::endl;
    
    // 检查是否在三角形内
    if (localBary[0] < -Rational(1)/Rational(1000) || 
        localBary[1] < -Rational(1)/Rational(1000) || 
        localBary[2] < -Rational(1)/Rational(1000)) {
        std::cerr << "Warning: Original point not inside generated triangle!" << std::endl;
        std::cerr << "Trying with larger radius..." << std::endl;
        
        // 递归尝试更大的半径
        return genLocalParam(uv, radius * Rational(2));
    }
    
    // 步骤8: 检验local_uv是否仍在边界上
    Array2r local_uv;
    local_uv << localBary[0], localBary[1];
    
    bool stillOnBoundary = false;
    if (edge == 0 && std::abs(local_uv[1]) < epsilon) {
        stillOnBoundary = true;
        std::cout << "✓ Point still on bottom edge in local coordinates" << std::endl;
    } else if (edge == 1 && std::abs(local_uv[0]) < epsilon) {
        stillOnBoundary = true;
        std::cout << "✓ Point still on left edge in local coordinates" << std::endl;
    } else if (edge == 2 && std::abs(local_uv[0] + local_uv[1] - Rational(1)) < epsilon) {
        stillOnBoundary = true;
        std::cout << "✓ Point still on hypotenuse in local coordinates" << std::endl;
    }
    
    if (!stillOnBoundary) {
        std::cerr << "Warning: Point no longer on boundary in local coordinates!" << std::endl;
        std::cerr << "local_uv = (" << local_uv[0] << ", " << local_uv[1] << ")" << std::endl;
    }
    
    return TriParamBound(vertices);
}

void adjustEF(TriQuadBezier& patch, const Array2r& uv, const Vector3r& targetNormal) {
    // 步骤1: 判断点在哪条边上
    BaryCoord coord(uv);
    const Rational epsilon = Rational(1) / Rational(1000);
    
    int edge = -1;
    if(coord.w < epsilon) {
        edge = 0; // w=0边界 (u+v=1, 斜边)
    } else if(coord.u < epsilon) {
        edge = 1; // u=0边界 (左边)
    } else if(coord.v < epsilon) {
        edge = 2; // v=0边界 (底边)
    } else {
        std::cerr << "Error: uv point is not on boundary in adjustEF!" << std::endl;
        return;
    }
    
    std::cout << "Adjusting edge " << edge << " to be perpendicular to target normal" << std::endl;
    
    // 步骤2: 准备基函数导数系数
    Rational u = coord.u;
    Rational v = coord.v;
    Rational w = coord.w;
    
    std::vector<Rational> dBu = {
        -Rational(2)*w,
        Rational(2)*w - Rational(2)*u,
        Rational(2)*u,
        -Rational(2)*v,
        Rational(2)*v,
        Rational(0)
    };
    
    std::vector<Rational> dBv = {
        -Rational(2)*w,
        -Rational(2)*u,
        Rational(0),
        Rational(2)*w - Rational(2)*v,
        Rational(2)*u,
        Rational(2)*v
    };
    
    // 步骤3: 根据边的类型确定切向量的基函数系数
    std::vector<Rational> dBtangent(6);
    
    switch(edge) {
        case 0: // 斜边 (u+v=1): tangent = ∂S/∂u - ∂S/∂v
            for(int i = 0; i < 6; i++) {
                dBtangent[i] = dBu[i] - dBv[i];
            }
            break;
            
        case 1: // 左边 (u=0): tangent = ∂S/∂v
            dBtangent = dBv;
            break;
            
        case 2: // 底边 (v=0): tangent = ∂S/∂u
            dBtangent = dBu;
            break;
    }
    
    // 步骤4: 计算当前切向量与目标法向量的点积（这是我们要消除的误差）
    Vector3r currentTangent = Vector3r::Zero();
    for(int i = 0; i < 6; i++) {
        currentTangent = currentTangent + dBtangent[i] * patch.ctrlp[i];
    }

    std::cout << "initial tangent: " << currentTangent.transpose() << std::endl;
    
    Rational tangent_dot = currentTangent.dot(targetNormal);
    std::cout << "Initial tangent * normal = " << tangent_dot << std::endl;
    
    if(std::abs(tangent_dot) < Rational(1)/Rational(1000)) {
        std::cout << "Already perpendicular, no adjustment needed" << std::endl;
        return;
    }
    
    // 步骤5: 构建线性约束系统
    // 我们需要一个约束方程：tangent · targetNormal = 0
    // 即：Σ dBtangent[i] · P[i] · targetNormal = 0
    
    // 枚举两个控制点进行调整
    bool solutionFound = false;
    
    for(int i = 0; i < 6; i++) {
        for(int j = i+1; j < 6; j++) {
            // 构建1×6线性系统（只有一个约束方程）
            Eigen::Matrix<Rational, 1, 6> A = Eigen::Matrix<Rational, 1, 6>::Zero();
            Eigen::Matrix<Rational, 1, 1> b;
            
            // 填充矩阵A：∂tangent/∂P[i] · targetNormal 和 ∂tangent/∂P[j] · targetNormal
            for(int k = 0; k < 3; k++) {
                // 第一个控制点 (i) 的贡献
                A(0, k) = dBtangent[i] * targetNormal(k);
                
                // 第二个控制点 (j) 的贡献
                A(0, k+3) = dBtangent[j] * targetNormal(k);
            }
            
            // 约束向量b
            b(0) = -tangent_dot;

            //输出A和b
            std::cout << "Trying control points " << i << " and " << j << std::endl;
            std::cout << "Matrix A: " << A << std::endl;
            std::cout << "Vector b: " << b.transpose() << std::endl;

            
            // 尝试求解：我们有1个方程但6个未知数
            // 策略：固定其他4个坐标为0，只调整选定的2个坐标中的某个分量
            
            // 找到系数最大的那个分量
            int maxIdx = 0;
            Rational maxCoeff = Rational(0);
            for(int k = 0; k < 6; k++) {
                if(std::abs(A(0, k)) > maxCoeff) {
                    maxCoeff = std::abs(A(0, k));
                    maxIdx = k;
                }
            }
            
            if(maxCoeff < Rational(1)/Rational(1000000)) {
                continue; // 系数太小，跳过这组控制点
            }
            
            // 只调整系数最大的那个分量
            Rational delta = b(0) / A(0, maxIdx);
            
            // 构造调整向量
            Vector3r adjustment1 = Vector3r::Zero();
            Vector3r adjustment2 = Vector3r::Zero();
            
            if(maxIdx < 3) {
                adjustment1[maxIdx] = delta;
            } else {
                adjustment2[maxIdx - 3] = delta;
            }

            std::cout << "adjustment: " << adjustment1 << "\n" << adjustment2 << std::endl;


            // 应用调整到临时patch
            TriQuadBezier tempPatch = patch;
            tempPatch.ctrlp[i] = tempPatch.ctrlp[i] + adjustment1;
            tempPatch.ctrlp[j] = tempPatch.ctrlp[j] + adjustment2;
            
            // 验证调整效果
            Vector3r newTangent = Vector3r::Zero();
            for(int k = 0; k < 6; k++) {
                newTangent = newTangent + dBtangent[k] * tempPatch.ctrlp[k];
            }
            
            Rational newDot = newTangent.dot(targetNormal);
            std::cout << "After adjusting control points " << i << " and " << j 
                      << ": tangent * normal = " << newDot << std::endl;
            if(std::abs(newDot) == 0) 
            {
                std::cout << "Solution found with control points " << i 
                            << " and " << j << std::endl;
                patch = tempPatch;
                solutionFound = true;
                return;
            }
        }
    }
    if(!solutionFound)
        std::cout << "Warning: Could not find good solution for adjustEF" << std::endl;
}

bool penEFCheck(const CollisionPoint& cp, bool ok) 
{
    auto facePatch = cp.patch1;  // 面片
    auto edgePatch = cp.patch2;  // 边所在的曲面
    auto faceUV = cp.local_uv1;  // 面片上的参数坐标
    auto edgeUV = cp.local_uv2;  // 边上的参数坐标
    
    // 确定边的类型
    int edgeType = -1;
    if (edgeUV[1] == Rational(0)) {
        edgeType = 0; // 底边: v = 0
    } else if (edgeUV[0] == Rational(0)) {
        edgeType = 1; // 左边: u = 0
    } else if (edgeUV[0] + edgeUV[1] == Rational(1)) {
        edgeType = 2; // 斜边: u + v = 1
    } else {
        std::cerr << "penEFCheck: 碰撞点不在边界上!" << std::endl;
        return false;
    }
    
    // 获取面片法线和碰撞点
    Vector3r faceNormal = facePatch.evaluateNormal(faceUV);
    Vector3r collisionPoint = facePatch.evaluatePatchPoint(faceUV);
    
    // 标准化法线
    if (faceNormal.norm() != Rational(0)) {
        faceNormal = faceNormal / faceNormal.norm();
    }
    
    // 获取边界切向量
    Vector3r edgeTangent = computeBoundaryTangent(edgePatch, edgeUV);
    
    // 计算垂直于边和面法线的方向（用于检测穿透）
    Vector3r perpDir = edgeTangent.cross(faceNormal);
    if (perpDir.norm() != Rational(0)) {
        perpDir = perpDir / perpDir.norm();
    }
    
    // 在边上均匀采样点
    const int numSamples = 10;
    bool penetrate = false;
    
    for (int i = 0; i < numSamples; i++) {
        Rational t = Rational(i) / Rational(numSamples - 1);
        Array2r sampleUV;
        
        // 根据边类型计算参数坐标
        switch (edgeType) {
            case 0: // 底边
                sampleUV = Array2r(t, Rational(0));
                break;
            case 1: // 左边
                sampleUV = Array2r(Rational(0), t);
                break;
            case 2: // 斜边
                sampleUV = Array2r(Rational(1) - t, t);
                break;
        }
        
        // 计算边上的点
        Vector3r edgePoint = edgePatch.evaluatePatchPoint(sampleUV);
        
        // 沿穿透检测方向偏移（边两侧）
        const Rational epsilon = Rational(1) / Rational(1000);
        Vector3r point1 = edgePoint + perpDir * epsilon;
        Vector3r point2 = edgePoint - perpDir * epsilon;
        
        // 计算点到面片的有符号距离
        Rational dist1 = (point1 - collisionPoint).dot(faceNormal);
        Rational dist2 = (point2 - collisionPoint).dot(faceNormal);
        
        // 检查点是否在面片两侧
        // 如果两个点的距离符号不同，且与期望相反，则表示有穿透
        if (dist1 * dist2 <= Rational(0)) {
            // 两点在面片的不同侧或一个点在面上
            // 进一步判断：根据期望方向检查
            if ((ok && dist1 > Rational(0)) || (!ok && dist1 < Rational(0))) {
                penetrate = true;
                break;
            }
        }
    }
    
    return penetrate;
}

};

namespace edgeedge
{
    
};

namespace vertexface{

    

    TriParamBound genLocalParam(const Array2r& uv, Rational radius) 
    {
        // 确定是哪个顶点
        int vertex = -1;
        const Rational epsilon = Rational("1/100000000000");
        
        if (uv[0] < epsilon && uv[1] < epsilon) {
            vertex = 0; // 第一个顶点 (0,0)
        } else if (std::abs(uv[0] - Rational(1)) < epsilon && uv[1] < epsilon) {
            vertex = 1; // 第二个顶点 (1,0)
        } else if (uv[0] < epsilon && std::abs(uv[1] - Rational(1)) < epsilon) {
            vertex = 2; // 第三个顶点 (0,1)
        } else {
            std::cerr << "Error: not a vertex point" << std::endl;
            return edgeface::genLocalParam(uv, radius); // 降级为边界处理
        }
        
        // 确保半径不会太小
        Rational minRadius = Rational(1) / Rational(20);
        radius = std::max(minRadius, radius);
        
        // 创建顶点周围的三角形
        std::array<BaryCoord, 3> vertices;
        
        switch (vertex) {
            case 0: // 原点 (0,0)
                vertices[0] = BaryCoord(Rational(0), Rational(0), Rational(1));           // 顶点本身
                vertices[1] = BaryCoord(radius, Rational(0), Rational(1) - radius);       // u方向
                vertices[2] = BaryCoord(Rational(0), radius, Rational(1) - radius);       // v方向
                break;
                
            case 1: // (1,0)点
                vertices[0] = BaryCoord(Rational(1), Rational(0), Rational(0));           // 顶点本身
                vertices[1] = BaryCoord(Rational(1) - radius, Rational(0), radius);       // -u方向
                vertices[2] = BaryCoord(Rational(1) - radius, radius, Rational(0));       // v方向
                break;
                
            case 2: // (0,1)点
                vertices[0] = BaryCoord(Rational(0), Rational(1), Rational(0));           // 顶点本身
                vertices[1] = BaryCoord(Rational(0), Rational(1) - radius, radius);       // -v方向
                vertices[2] = BaryCoord(radius, Rational(1) - radius, Rational(0));       // u方向
                break;
        }
        
        // 确保三角形不会退化
        bool nearlyDegenerate = false;
        if ((vertices[0].u - vertices[1].u).abs() + (vertices[0].v - vertices[1].v).abs() < Rational(1)/Rational(100) ||
            (vertices[1].u - vertices[2].u).abs() + (vertices[1].v - vertices[2].v).abs() < Rational(1)/Rational(100) ||
            (vertices[2].u - vertices[0].u).abs() + (vertices[2].v - vertices[0].v).abs() < Rational(1)/Rational(100)) {
            nearlyDegenerate = true;
        }
        
        // 如果三角形接近退化，增加半径重新尝试
        if (nearlyDegenerate) {
            std::cerr << "Warning: nearly degenerate vertex triangle" << std::endl;
            return genLocalParam(uv, radius * Rational(2));
        }
        
        return TriParamBound(vertices);
    }

    
    
};

namespace vertexedge
{
    

};



void adjustPatchToOrthogonalDirectSolve(TriQuadBezier& patch, const Array2r& uv, const Vector3r& targetNormal) 
{
    BaryCoord coord(uv);
    
    // 归一化目标法线，全部使用 Rational
    Vector3r normalizedTarget = targetNormal;
    // Rational normLength = normalizedTarget.norm();
    // normalizedTarget = normalizedTarget / normLength;

    // 使用 Rational 计算重心坐标
    Rational u = coord.u;
    Rational v = coord.v;
    Rational w = Rational(1) - u - v;
    
    // u方向导数的基函数系数 (全部使用 Rational)
    std::vector<Rational> dBu = {
        -Rational(2)*w,                // dB_002/du = -2w
        Rational(2)*w - Rational(2)*u, // dB_101/du = 2w - 2u
        Rational(2)*u,                 // dB_200/du = 2u
        -Rational(2)*v,                // dB_011/du = -2v
        Rational(2)*v,                 // dB_110/du = 2v
        Rational(0)                    // dB_020/du = 0
    };
    
    // v方向导数的基函数系数 (全部使用 Rational)
    std::vector<Rational> dBv = {
        -Rational(2)*w,                // dB_002/dv = -2w
        -Rational(2)*u,                // dB_101/dv = -2u
        Rational(0),                   // dB_200/dv = 0
        Rational(2)*w - Rational(2)*v, // dB_011/dv = 2w - 2v
        Rational(2)*u,                 // dB_110/dv = 2u
        Rational(2)*v                  // dB_020/dv = 2v
    };
    
    // 计算当前导数向量
    Vector3r du = Vector3r::Zero();
    Vector3r dv = Vector3r::Zero();
    
    for(int i = 0; i < 6; i++) {
        du = du + dBu[i] * patch.ctrlp[i];
        dv = dv + dBv[i] * patch.ctrlp[i];
    }
    
    // 计算约束误差
    Rational du_dot = du.dot(normalizedTarget);
    Rational dv_dot = dv.dot(normalizedTarget);
    
    // 选择两个控制点进行精确调整
    // 枚举所有可能的控制点对组合
    bool solutionFound = false;
    int best_i = 0, best_j = 0;
    Rational bestErrorSum = Rational(1e10); // 初始设一个很大的值
    
    // 尝试所有可能的控制点对组合
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            if(i == j) continue; // 跳过相同的控制点
            
            // 构建2x6线性系统 Ax = b
            Eigen::Matrix<Rational, 2, 6> A = Eigen::Matrix<Rational, 2, 6>::Zero();
            Eigen::Matrix<Rational, 2, 1> b;
            
            // 填充矩阵A，只考虑两个选定控制点的影响
            for(int k = 0; k < 3; k++) {
                // 第一行: u方向正交约束
                A(0, k) = (i < 3 ? dBu[i] * normalizedTarget(k) : Rational(0));
                A(0, k+3) = (j < 3 ? dBu[j] * normalizedTarget(k) : Rational(0));
                A(0, k) = (j >= 3 && j-3 == k ? dBu[j] * normalizedTarget(k) : A(0, k));
                A(0, k+3) = (i >= 3 && i-3 == k ? dBu[i] * normalizedTarget(k) : A(0, k+3));
                
                // 第二行: v方向正交约束
                A(1, k) = (i < 3 ? dBv[i] * normalizedTarget(k) : Rational(0));
                A(1, k+3) = (j < 3 ? dBv[j] * normalizedTarget(k) : Rational(0));
                A(1, k) = (j >= 3 && j-3 == k ? dBv[j] * normalizedTarget(k) : A(1, k));
                A(1, k+3) = (i >= 3 && i-3 == k ? dBv[i] * normalizedTarget(k) : A(1, k+3));
            }
            
            // 设置约束向量b
            b(0) = -du_dot;
            b(1) = -dv_dot;
            
            // 打印A和b用于调试
            // std::cout << "A for control points " << i << ", " << j << ":" << std::endl;
            // std::cout << A << std::endl;
            // std::cout << "b:" << std::endl;
            // std::cout << b << std::endl;
            
            // 尝试求解线性系统
            std::array<Vector3r, 2> adjustments;
            if(solve2x6LinearSystem(A, b, adjustments)) {
                // 应用调整
                TriQuadBezier tempPatch = patch;

                // 应用调整
                tempPatch.ctrlp[i] = tempPatch.ctrlp[i] + adjustments[0];
                tempPatch.ctrlp[j] = tempPatch.ctrlp[j] + adjustments[1];

                // 重新计算调整后的导数向量
                Vector3r new_du = Vector3r::Zero();
                Vector3r new_dv = Vector3r::Zero();
                
                for(int k = 0; k < 6; k++) {
                    new_du = new_du + dBu[k] * tempPatch.ctrlp[k];
                    new_dv = new_dv + dBv[k] * tempPatch.ctrlp[k];
                }
                
                // 计算调整后的误差
                Rational final_error_u = std::abs(new_du.dot(normalizedTarget));
                Rational final_error_v = std::abs(new_dv.dot(normalizedTarget));
                Rational errorSum = final_error_u + final_error_v;
                
                std::cout << "diff after try to adjust:" 
                << "u = " << final_error_u 
                << ", v = " << final_error_v << std::endl;

                // 如果误差小于当前最佳值，更新最佳解
                if(errorSum < bestErrorSum) {
                    bestErrorSum = errorSum;
                    best_i = i;
                    best_j = j;
                    solutionFound = true;
                    
                    // 如果误差足够小，直接应用调整并返回
                    if(final_error_u == 0 && 
                       final_error_v ==0) {
                        patch = tempPatch;
                        std::cout << "Solution found with control points " << i << " and " << j << std::endl;
                        std::cout << "adjustments: " << adjustments[0] << ", " << adjustments[1] << std::endl;
                        std::cout << "diff after adjust: u = " << final_error_u 
                                  << ", v = " << final_error_v << std::endl;
                        return;
                    }
                }
            }
        }
    }
    std::cout << "No solution found for any control point pair" << std::endl;

}


bool solve2x6LinearSystem(
    const Eigen::Matrix<Rational, 2, 6>& A, 
    const Eigen::Matrix<Rational, 2, 1>& b,
    std::array<Vector3r, 2>& solution)
{
    const Rational epsilon = Rational(1) / Rational(1000000);
    bool foundSolution = false;
    Vector3r adjustment1 = Vector3r::Zero();
    Vector3r adjustment2 = Vector3r::Zero();
    
    // 枚举所有可能的列组合
    for(int i = 0; i < 6; i++) {
        for(int j = i+1; j < 6; j++) {
            // 构建2×2子矩阵
            Eigen::Matrix<Rational, 2, 2> subA;
            subA(0, 0) = A(0, i);
            subA(0, 1) = A(0, j);
            subA(1, 0) = A(1, i);
            subA(1, 1) = A(1, j);
            
            // 检查子矩阵是否可逆
            Rational det = subA.determinant();
            if(std::abs(det) < epsilon) continue;
            
            // 求解2×2系统
            Eigen::Matrix<Rational, 2, 1> delta;
            delta(0) = (subA(1, 1) * b(0) - subA(0, 1) * b(1)) / det;
            delta(1) = (subA(0, 0) * b(1) - subA(1, 0) * b(0)) / det;
            
            // 构造调整向量
            Vector3r tempAdjust1 = Vector3r::Zero();
            Vector3r tempAdjust2 = Vector3r::Zero();
            
            if(i < 3)
                tempAdjust1[i] = delta(0);
            else
                tempAdjust2[i-3] = delta(0);
                
            if(j < 3)
                tempAdjust1[j] = delta(1);
            else
                tempAdjust2[j-3] = delta(1);
                
            // 找到解，记录调整量并退出循环
            adjustment1 = tempAdjust1;
            adjustment2 = tempAdjust2;
            foundSolution = true;
            break;
        }
        if(foundSolution) break;
    }
    
    // 返回结果
    if(foundSolution) {
        solution[0] = adjustment1;
        solution[1] = adjustment2;
    }
    
    return foundSolution;
}
