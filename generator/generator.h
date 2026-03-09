#pragma once
#include "gentype.h"

// using Eigen::Array2d;
// using Eigen::Vector3d;
// using Eigen::Vector4d;
// using Eigen::MatrixXd;
// using Rational = rational::Rational;
// using Vector3r = Eigen::Matrix<Rational, 3, 1>;
// using Array2r = Eigen::Matrix<Rational, 2, 1>;

//TODO: 改成OOP

// 测试一个有理数向量是否能被浮点数精确表示
bool isPreciselyRepresent(const Vector3r& vec);

// 随机三维有理向量
// Vector3r randomVector3r(auto &engine, auto &dist1, auto &dist2, auto &dist3);
// 随机法线方向
Vector3r generateIntegerPowerOfTwoNormal(auto& engine);
// 法线约束之旋转
void rotatePatchToAlign(TriQuadBezier& patch, const Array2r& uv, const Vector3r& targetNormal);
// 验证是否碰撞
bool verifyCollision(const CollisionPoint& cp, const float &eps);
// 额外碰撞检测
bool testAdditionalCollisions(const CollisionPoint& cp, int taskType = -1);
// 生成一个随机的patch
// TriQuadBezier generateRandomPatch(auto &engine, auto &dist1, auto &dist2, auto &dist3);
// 生成一个穿过点的patch
TriQuadBezier generatePatchThroughPoint(const Vector3r& point, const Array2r& uv, unsigned seed, auto &dist);
// 生成碰撞速度对
std::pair<Vector3r, Vector3r> generateCollisionVelocities(
    auto& engine, 
    const Vector3r& normal1,
    const Vector3r& normal2,
    bool sameDirection);
// 生成与法线垂直的速度
Vector3r generateVelocityPerpendicularToNormal(
    auto& engine,
    const Vector3r& partialU,
    const Vector3r& partialV);
// 生成碰撞速度场
std::array<Vector3r, 6> generateVelocityField(
    auto& engine,
    const Vector3r& collisionVel,
    const Array2r& uv,
    const std::array<Vector3r, 6>& controlPoints)
{
    std::uniform_real_distribution<float> dist(-5.0, 5.0);
    std::array<Vector3r, 6> velocities;
    
    // 随机生成前5个控制点的速度
    for(int i = 0; i < 5; i++) 
        velocities[i] = Vector3r(dist(engine), dist(engine), dist(engine));

    BaryCoord coord(uv);
    Rational w = coord.w;
    Rational u = coord.u;
    Rational v = coord.v;
    
    // 计算基函数系数
    Rational coeffs[6] = {
        w*w,             // 控制点0的系数
        Rational(2)*w*u, // 控制点1的系数
        u*u,             // 控制点2的系数
        Rational(2)*w*v, // 控制点3的系数
        Rational(2)*u*v, // 控制点4的系数
        v*v              // 控制点5的系数
    };
    
    // 找到系数绝对值最大的控制点作为求解控制点
    int maxIndex = 0;
    Rational maxCoeff = std::abs(coeffs[0]);
    
    for (int i = 1; i < 6; i++) {
        if (std::abs(coeffs[i]) > maxCoeff) {
            maxIndex = i;
            maxCoeff = std::abs(coeffs[i]);
        }
    }
    
    // 如果最大系数仍然太小，直接设置最后一个控制点的速度为碰撞速度
    const Rational epsilon = Rational(1)/Rational(10000);
    if (maxCoeff < epsilon) {
        velocities[5] = collisionVel;
        return velocities;
    }
    
    // 计算选中的控制点速度
    Vector3r sumOtherTerms = Vector3r::Zero();
    for (int i = 0; i < 6; i++) {
        if (i != maxIndex) {
            sumOtherTerms = sumOtherTerms + coeffs[i] * velocities[i];
        }
    }
    
    // 求解选中的控制点速度
    velocities[maxIndex] = (collisionVel - sumOtherTerms) / coeffs[maxIndex];
    
    return velocities;
}

// 面面碰撞穿透检测
bool penetrateCheck(const CollisionPoint& cp, bool ok);
// 通过采样确定两个patch的相对位置方向
Vector3r sampleSeparationDirection(const TriQuadBezier& patch1, const TriQuadBezier& patch2,
                                    const Array2r& uv1, const Array2r& uv2,
                                    const Vector3r& normal1);
//产生边面碰撞
CollisionPoint generateEF(unsigned seed);
//产生边边碰撞
CollisionPoint generateEE(unsigned seed);
//产生点面碰撞
CollisionPoint generateVF(unsigned seed);
//产生点边碰撞
CollisionPoint generateVE(unsigned seed);
//产生点点碰撞
CollisionPoint generateVV(unsigned seed);
//擦肩而过
CollisionPoint generateNearMissFF(unsigned seed, Rational gap = Rational("1/131072"));
// Edge-Face擦肩而过
CollisionPoint generateNearMissEF(unsigned seed, Rational gap = Rational("1/131072"));
// Edge-Edge擦肩而过
CollisionPoint generateNearMissEE(unsigned seed, Rational gap = Rational("1/131072"));
// Vertex-Face擦肩而过
CollisionPoint generateNearMissVF(unsigned seed, Rational gap = Rational("1/131072"));
// Vertex-Edge擦肩而过
CollisionPoint generateNearMissVE(unsigned seed, Rational gap = Rational("1/131072"));
// Vertex-Vertex擦肩而过
CollisionPoint generateNearMissVV(unsigned seed, Rational gap = Rational("1/131072"));
// Face-Face恰好碰到（Near Hit）
CollisionPoint generateNearHitFF(unsigned seed);

CollisionPoint generateNearHitEF(unsigned seed);

CollisionPoint generateNearHitEE(unsigned seed);

CollisionPoint generateNearHitVF(unsigned seed);

CollisionPoint generateNearHitVE(unsigned seed);

CollisionPoint generateNearHitVV(unsigned seed);

// 生成退化情况//产生随机2面碰撞（面面及其他）
CollisionPoint generateSeparatedRandomBezierPatches(unsigned seed, int tasktype);


namespace genStandardData
{
    inline unsigned extractSeedFromFolderName(const std::string& folderName);
    inline bool processDataFolder(const std::filesystem::path& folderPath, int taskType, const std::string& datasetFilePath);
    inline void saveControlPointsData(const CollisionPoint& cp, const std::string& outputDir, 
                                 const std::string& datasetFilePath, unsigned seed, int taskType);
    void batchProcessFolders(const std::string& baseDir, int taskType);
}