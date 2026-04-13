#include "generator.h"
#include "solverTD.h"
#include <filesystem>


bool isPreciselyRepresent(const Vector3r& vec)
{
    for (int i = 0; i < 3; i++) 
    {
        // 转换为浮点数再转回有理数
        double doubleVal = (double)vec[i];
        Rational roundTrip(doubleVal);

        Rational diff = vec[i] - roundTrip;
        std::cout << "Original: " << vec[i] << ", After round-trip: " << roundTrip << std::endl;
        std::cout << "Diff between offset: " << diff << std::endl;
        if (diff.abs() > Rational("0")) 
        {
            return false; // 不能被精确表示
        }
    }
    return true; // 所有分量都能被精确表示
}
//TODO:旋转至目标法线的同方向还是反方向，现在该函数是同方向，但传参是反方向

void rotatePatchToAlign(TriQuadBezier& patch, const Array2r& uv, const Vector3r& targetNormal) 
{
    // 获取当前法线
    Vector3r currentNormal = patch.evaluateNormal(uv);
    
    // 计算旋转矩阵
    Vector3r axis = currentNormal.cross(targetNormal);
    if (axis.norm() < 1e-10) {//已平行
        return;
    }
    axis.normalize();
    
    float cos_theta_d = (float)(currentNormal.dot(targetNormal) / 
                      (currentNormal.norm() * targetNormal.norm()));
    cos_theta_d = std::min(1.0f, std::max(-1.0f, cos_theta_d));
    float theta_d = std::acos(cos_theta_d);
    
    Eigen::Matrix3d K_d;
    K_d << 0, -(float)axis(2), (float)axis(1),
           (float)axis(2), 0, -(float)axis(0),
           -(float)axis(1), (float)axis(0), 0;
    
    Eigen::Matrix3d R_d = Eigen::Matrix3d::Identity() + 
                       std::sin(theta_d) * K_d + 
                       (1.0 - std::cos(theta_d)) * K_d * K_d;
    
    // 旋转ctrlp
    Vector3r center = patch.evaluatePatchPoint(uv); 
    for(int i = 0; i < patch.ctrlp.size(); i++)
    {
        Vector3d p_d((float)(patch.ctrlp[i](0) - center(0)), 
                     (float)(patch.ctrlp[i](1) - center(1)), 
                     (float)(patch.ctrlp[i](2) - center(2)));
        
        Vector3d rotated_p_d = R_d * p_d;
        Vector3r rotated_p(rotated_p_d(0), rotated_p_d(1), rotated_p_d(2));
        patch.ctrlp[i] = center + rotated_p;
    }
}



bool verifyCollision(const CollisionPoint& cp, const float &eps = 1e-10) 
{
    Vector3r p1 = cp.patch1.evaluatePatchPoint(cp.local_uv1);
    Vector3r p2 = cp.patch2.evaluatePatchPoint(cp.local_uv2);
    return (p1 - p2).norm() < eps;
}

bool testAdditionalCollisions(const CollisionPoint& cp, int taskType)
{
    std::cout << "test CCD...\n" << std::endl;
    
    // 针对 tasktype==12-17 (NearHit 系列) 的特殊处理
    if (taskType >= 12 && taskType <= 17) {
        std::cout << "=== Special handling for NearHit series (tasktype=" << taskType << ") ===" << std::endl;
        
        TriQuadBezier patch1 = cp.patch1, patch2 = cp.patch2;
        Array2r uv1 = cp.local_uv1, uv2 = cp.local_uv2;
        BoundingBoxType bb = BoundingBoxType::OBB;
        const double deltaDist = 1e-6;
        
        // 测试1: 正向检测 [0, 0.5]
        std::cout << "\n--- Test 1: Forward detection [0, 0.5] ---" << std::endl;
        TriQuadBezier patch1_forward = patch1, patch2_forward = patch2;
        Array2r uv1_forward = uv1, uv2_forward = uv2;
        
        auto start1 = std::chrono::high_resolution_clock::now();
        Rational collisionTime1 = SolverTD<TriQuadBezier, TriQuadBezier, TriParamBound, TriParamBound>::solveCCD(
            patch1_forward.ctrlp, patch1_forward.velp, patch2_forward.ctrlp, patch2_forward.velp, 
            uv1_forward, uv2_forward, bb, deltaDist, 0.5);
        auto end1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed1 = end1 - start1;
        
        std::cout << "Forward collision time: " << (double)collisionTime1 << std::endl;
        std::cout << "Forward detection time: " << elapsed1.count() << " seconds" << std::endl;
        
        // 测试2: 反向检测 [0, 0.5] (从t=1到t=0.5)
        std::cout << "\n--- Test 2: Backward detection [0, 0.5] (from t=1 to t=0.5) ---" << std::endl;
        TriQuadBezier patch1_backward = patch1, patch2_backward = patch2;
        Array2r uv1_backward = uv1, uv2_backward = uv2;
        
        // 平移到 t=1
        for(int i = 0; i < 6; i++) {
            patch1_backward.ctrlp[i] = patch1_backward.ctrlp[i] + patch1_backward.velp[i] * 1.0;
            patch2_backward.ctrlp[i] = patch2_backward.ctrlp[i] + patch2_backward.velp[i] * 1.0;
        }
        
        // 反向速度
        for(int i = 0; i < 6; i++) {
            patch1_backward.velp[i] = -patch1_backward.velp[i];
            patch2_backward.velp[i] = -patch2_backward.velp[i];
        }
        
        auto start2 = std::chrono::high_resolution_clock::now();
        Rational collisionTime2 = SolverTD<TriQuadBezier, TriQuadBezier, TriParamBound, TriParamBound>::solveCCD(
            patch1_backward.ctrlp, patch1_backward.velp, patch2_backward.ctrlp, patch2_backward.velp, 
            uv1_backward, uv2_backward, bb, deltaDist, 0.5);
        auto end2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed2 = end2 - start2;
        
        std::cout << "Backward collision time: " << (double)collisionTime2 << std::endl;
        std::cout << "Backward detection time: " << elapsed2.count() << " seconds" << std::endl;
        
        // 判断条件：两个碰撞时间都 >= 0.5
        std::cout << "\n--- Final judgment ---" << std::endl;
        bool forwardAtHalf = (collisionTime1 >= 0.5 || collisionTime1 < 0);
        bool backwardAtHalf = (collisionTime2 >= 0.5 || collisionTime2 < 0);
        
        std::cout << "Forward collision at t=0.5 only: " << (forwardAtHalf ? "YES" : "NO") << std::endl;
        std::cout << "Backward collision at t=0.5 only: " << (backwardAtHalf ? "YES" : "NO") << std::endl;
        
        if (forwardAtHalf && backwardAtHalf) {
            std::cout << "Result: Only collision at t=0.5 (instantaneous contact) -> return false" << std::endl;
            std::cout << "no collision" << std::endl;
            return false;
        } else {
            std::cout << "Result: Collision at other times detected -> return true" << std::endl;
            if (collisionTime1 > -1 && collisionTime1 < 0.5) {
                std::cout << "Forward collision detected at t=" << (double)collisionTime1 << std::endl;
                std::cout << "uv of patch1: (" << (double)uv1_forward[0] << ", " << (double)uv1_forward[1] << ")" << std::endl;
                std::cout << "uv of patch2: (" << (double)uv2_forward[0] << ", " << (double)uv2_forward[1] << ")" << std::endl;
            }
            if (collisionTime2 > -1 && collisionTime2 < 0.5) {
                std::cout << "Backward collision detected at t=" << (1.0 - (double)collisionTime2) << " (original time)" << std::endl;
                std::cout << "uv of patch1: (" << (double)uv1_backward[0] << ", " << (double)uv1_backward[1] << ")" << std::endl;
                std::cout << "uv of patch2: (" << (double)uv2_backward[0] << ", " << (double)uv2_backward[1] << ")" << std::endl;
            }
            return true;
        }
    }
    
    // 标准处理流程（其他tasktype）
    TriQuadBezier patch1 = cp.patch1, patch2 = cp.patch2;
    
    Array2r uv1 = cp.local_uv1, uv2 = cp.local_uv2;
    BoundingBoxType bb = BoundingBoxType::OBB;
    const double deltaDist = 1e-6;
    const double upperTime = 1e-6;
    const double rtime = 1;

    long long maxlen = -1;//分子分母的长度最大值

    for(int i = 0; i < 6; i++) 
    {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * upperTime;
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * upperTime;
        // for(int j = 0; j < 3; j++) 
        // {
        //     // 检查patch1的控制点
        //     maxlen = std::max(maxlen, (long long)patch1.ctrlp[i](j).numerator().length());
        //     maxlen = std::max(maxlen, (long long)patch1.ctrlp[i](j).denominator().length());
            
        //     // 检查patch2的控制点
        //     maxlen = std::max(maxlen, (long long)patch2.ctrlp[i](j).numerator().length());
        //     maxlen = std::max(maxlen, (long long)patch2.ctrlp[i](j).denominator().length());
        // }
        // debug << "patch1[" << i << "] = " << patch1.ctrlp[i].transpose() << std::endl;
        // debug << "patch2[" << i << "] = " << patch2.ctrlp[i].transpose() << std::endl;    
    }
    for(int i = 0; i < 6; i++) 
    {
        // patch1.velp[i] = -patch1.velp[i];
        // patch2.velp[i] = -patch2.velp[i];
        // for(int j = 0; j < 3; j++) 
        // {
        //     // 检查patch1的速度
        //     maxlen = std::max(maxlen, (long long)patch1.velp[i](j).numerator().length());
        //     maxlen = std::max(maxlen, (long long)patch1.velp[i](j).denominator().length());
            
        //     // 检查patch2的速度
        //     maxlen = std::max(maxlen, (long long)patch2.velp[i](j).numerator().length());
        //     maxlen = std::max(maxlen, (long long)patch2.velp[i](j).denominator().length());
        // }    
        // debug << "patch1[" << i << "] = " << patch1.velp[i].transpose() << std::endl;
        // debug << "patch2[" << i << "] = " << patch2.velp[i].transpose() << std::endl;
    }
    // debug << "maxlen: " << maxlen << std::endl;

    //计时
    auto start = std::chrono::high_resolution_clock::now();

    Rational collisionTime = SolverTD<TriQuadBezier, TriQuadBezier, TriParamBound, TriParamBound>::solveCCD(
        patch1.ctrlp, patch1.velp, patch2.ctrlp, patch2.velp, uv1, uv2, bb, deltaDist, rtime);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "collision time: " << (double)collisionTime << std::endl;

    if (collisionTime > -1) 
    {
        // std::cout << "collision time: " << (double)collisionTime << std::endl;
        std::cout << "uv of patch1: (" << (double)uv1[0] << ", " << (double)uv1[1] << ")" << std::endl;
        std::cout << "uv of patch2: (" << (double)uv2[0] << ", " << (double)uv2[1] << ")" << std::endl;
        std::cout << "uv of patch1: (" << uv1[0] << ", " << uv1[1] << ")" << std::endl;
        std::cout << "uv of patch2: (" << uv2[0] << ", " << uv2[1] << ")" << std::endl;
        std::cout << "collision point1 " << cp.patch1.evaluatePatchPoint(uv1).transpose() << std::endl;
        std::cout << "collision point2 " << cp.patch2.evaluatePatchPoint(uv2).transpose() << std::endl;
        std::cout << "collision point1 " << (double)cp.patch1.evaluatePatchPoint(uv1)[0] << ", "
                  << (double)cp.patch1.evaluatePatchPoint(uv1)[1] << ", "
                  << (double)cp.patch1.evaluatePatchPoint(uv1)[2] << std::endl;
        std::cout << "collision point2 " << (double)cp.patch2.evaluatePatchPoint(uv2)[0] << ", "
                  << (double)cp.patch2.evaluatePatchPoint(uv2)[1] << ", "
                  << (double)cp.patch2.evaluatePatchPoint(uv2)[2] << std::endl;
        return true;
    } 
    else 
    {
        std::cout << "no collision" << std::endl;
        return false;
    }
}

TriQuadBezier generatePatchThroughPoint(const Vector3r& point, const Array2r& uv, unsigned seed, auto &dist) 
{
    std::mt19937_64 engine(seed);

    std::array<Vector3r, 6> patch;
    for(int i = 0; i < 6; i++) 
        patch[i] << dist(engine), dist(engine), dist(engine);
    TriQuadBezier tempPatch(patch);
    Vector3r currentPoint = tempPatch.evaluatePatchPoint(uv);
    
    Vector3r offset = point - currentPoint;
    for(int i = 0; i < 6; i++) 
        patch[i] = patch[i] + offset;
    
    return TriQuadBezier(patch);
}

// 生成碰撞速度对
std::pair<Vector3r, Vector3r> generateCollisionVelocities(
    auto& engine, 
    const Vector3r& normal1,
    const Vector3r& normal2,
    bool sameDirection = false) 
{
    std::uniform_real_distribution<float> speedDist(0.5, 5.0);
    
    int sign = sameDirection ? -1 : 1;

    Vector3r dir1 = generateRandomDirectionInHalfSpace(engine, normal1);
    Vector3r dir2 = generateRandomDirectionInHalfSpace(engine, sign*normal2);
    
    // 生成大小
    Rational speed1 = speedDist(engine);
    std::uniform_real_distribution<float> speedDist2(0.5, speed1);
    Rational speed2 = speedDist(engine);
    // if(sameDirection)
    //     speed2 = speedDist2(engine);
    
    return {speed1 * dir1, speed2 * dir2};
}

// 在 generateCollisionVelocities 函数后添加

// 生成垂直于法线的速度（在法线的正交平面内）
// 使用patch的偏导数partialU和partialV作为切平面的基
Vector3r generateVelocityPerpendicularToNormal(
    auto& engine,
    const Vector3r& partialU,
    const Vector3r& partialV)
{
    std::uniform_real_distribution<float> speedDist(-10.0, 10.0);
    std::uniform_real_distribution<float> coeffDist(-1.0, 1.0);
    
    // 生成两个随机系数来线性组合partialU和partialV
    Rational coeff_u = Rational(speedDist(engine));
    Rational coeff_v = Rational(speedDist(engine));
    
    // 在切平面内生成速度方向
    Vector3r direction = coeff_u * partialU + coeff_v * partialV;
    double log2dir = std::log2((double)direction.norm());
    int n = static_cast<int>(std::round(log2dir));
    // direction = direction / Rational(1 << n);

    // 归一化方向
    // Rational dir_norm = direction.norm();
    // if (dir_norm > Rational(0)) {
    //     direction = direction / dir_norm;
    // } else {
    //     // 如果方向为零，只使用partialU
    //     direction = partialU;
    //     dir_norm = direction.norm();
    //     if (dir_norm > Rational(0)) {
    //         direction = direction / dir_norm;
    //     }
    // }
    
    // 生成速度大小
    // Rational speed = Rational(speedDist(engine));
    
    return direction;
}

// 通过采样确定两个patch的相对位置方向
// 返回: 从patch1指向patch2的方向向量（在采样点处）
Vector3r sampleSeparationDirection(const TriQuadBezier& patch1, const TriQuadBezier& patch2,
                                    const Array2r& uv1, const Array2r& uv2,
                                    const Vector3r& normal1)
{
    // 计算偏导数
    auto Su1 = patch1.evaluatePartialU(uv1);
    auto Sv1 = patch1.evaluatePartialV(uv1);
    auto Su2 = patch2.evaluatePartialU(uv2);
    auto Sv2 = patch2.evaluatePartialV(uv2);
    
    std::cout << "Su1: " << Su1.transpose() << ", norm: " << (double)Su1.norm() << std::endl;
    std::cout << "Sv1: " << Sv1.transpose() << std::endl;
    std::cout << "Su2: " << Su2.transpose() << std::endl;
    std::cout << "Sv2: " << Sv2.transpose() << std::endl;
    
    // 采样一个3D空间的方向 deltap（使用切向量）
    Rational Su1Norm = Su1.norm();
    if (Su1Norm == Rational(0)) {
        std::cerr << "Error: Su1 is zero vector, cannot sample direction" << std::endl;
        return Vector3r(0, 0, 0);
    }
    
    Vector3r deltap = (Su1 / Su1Norm) * Rational("1/10");
    std::cout << "deltap: " << deltap.transpose() << std::endl;
    
    // 构建投影矩阵 A = [Su, Sv]^T * [Su, Sv]
    Eigen::Matrix<Rational, 2, 2> A1, A2;
    A1 << Su1.dot(Su1), Su1.dot(Sv1),
          Sv1.dot(Su1), Sv1.dot(Sv1);
    A2 << Su2.dot(Su2), Su2.dot(Sv2),
          Sv2.dot(Su2), Sv2.dot(Sv2);
    
    std::cout << "A1 determinant: " << (double)A1.determinant() << std::endl;
    std::cout << "A2 determinant: " << (double)A2.determinant() << std::endl;
    
    // 检查矩阵是否可逆
    if (A1.determinant() == Rational(0)) {
        std::cerr << "Error: A1 is singular, cannot invert" << std::endl;
        return Vector3r(0, 0, 0);
    }
    if (A2.determinant() == Rational(0)) {
        std::cerr << "Error: A2 is singular, cannot invert" << std::endl;
        return Vector3r(0, 0, 0);
    }
    
    // 构建右端向量 b = [Su, Sv]^T * deltap
    Eigen::Matrix<Rational, 2, 1> b1, b2;
    b1 << Su1.dot(deltap), Sv1.dot(deltap);
    b2 << Su2.dot(deltap), Sv2.dot(deltap);
    
    std::cout << "b1: " << b1.transpose() << std::endl;
    std::cout << "b2: " << b2.transpose() << std::endl;
    
    // 解方程得到参数空间的偏移
    Eigen::Matrix<Rational, 2, 1> deltauv1 = A1.inverse() * b1;
    Eigen::Matrix<Rational, 2, 1> deltauv2 = A2.inverse() * b2;
    
    std::cout << "deltauv1: " << deltauv1.transpose() << std::endl;
    std::cout << "deltauv2: " << deltauv2.transpose() << std::endl;
    
    // 计算采样点的3D位置
    Array2r sampleUV1 = uv1 + Array2r(deltauv1(0), deltauv1(1));
    Array2r sampleUV2 = uv2 + Array2r(deltauv2(0), deltauv2(1));
    
    std::cout << "sampleUV1: " << sampleUV1.transpose() << std::endl;
    std::cout << "sampleUV2: " << sampleUV2.transpose() << std::endl;
    
    Vector3r samplePoint1 = patch1.evaluatePatchPoint(sampleUV1);
    Vector3r samplePoint2 = patch2.evaluatePatchPoint(sampleUV2);
    
    std::cout << "samplePoint1: " << samplePoint1.transpose() << std::endl;
    std::cout << "samplePoint2: " << samplePoint2.transpose() << std::endl;
    
    // 返回从patch1采样点指向patch2采样点的向量
    Vector3r result = samplePoint2 - samplePoint1;
    std::cout << "result direction: " << result.transpose() << ", norm: " << (double)result.norm() << std::endl;
    
    return result;
}

bool penetrateCheck(const CollisionPoint& cp, bool ok) 
{
    auto patch1 = cp.patch1;
    auto patch2 = cp.patch2;
    auto uv = cp.local_uv1;
    auto Su1 = patch1.evaluatePartialU(uv);
    auto Sv1 = patch1.evaluatePartialV(uv);
    auto Su2 = patch2.evaluatePartialU(uv);
    auto Sv2 = patch2.evaluatePartialV(uv);
    auto Suu1 = patch1.evaluatePartialUU(uv);
    auto Svv1 = patch1.evaluatePartialVV(uv);
    auto Suu2 = patch2.evaluatePartialUU(uv);
    auto Svv2 = patch2.evaluatePartialVV(uv);
    auto Suv1 = patch1.evaluatePartialUV(uv);
    auto Suv2 = patch2.evaluatePartialUV(uv);
    //采样数个deltap方向
    Vector3r dirp[6] = {
        Su1.normalized(), Vector3r(0,1,0), Vector3r(1,0,0), Vector3r(0,0,1)
    };
    dirp[1] = dirp[0].cross(cp.normal1).normalized();
    dirp[2] = (dirp[0] + dirp[1]).normalized();
    dirp[3] = -dirp[0];
    dirp[4] = -dirp[1];
    dirp[5] = -dirp[2];
    Eigen::Matrix<Rational, 2, 2> A1, A2;
    A1 << Su1.dot(Su1), Su1.dot(Sv1),
            Sv1.dot(Su1), Sv1.dot(Sv1);
    A2 << Su2.dot(Su2), Su2.dot(Sv2),
            Sv2.dot(Su2), Sv2.dot(Sv2);
    Eigen::Matrix<Rational, 2, 2> B1, B2;
    B1 << Suu1.dot(cp.normal1), Suv1.dot(cp.normal1), 
            Suv1.dot(cp.normal1), Svv1.dot(cp.normal1);
    B2 << Suu2.dot(cp.normal2), Suv2.dot(cp.normal2),
            Suv2.dot(cp.normal2), Svv2.dot(cp.normal2);
    
    for(Rational depth = 0.1; depth <= 0.5; depth = depth + Rational(0.1)) 
    {
        for(int i=0; i<6; i++)
        {
            Vector3r deltap = dirp[i] * depth;
            //解方程：Ax=b
            //A = [Su, Sv]^T[Su, Sv]
            //b = [Su, Sv]^Tdeltap
            
            Eigen::Matrix<Rational, 2, 1> b1, b2;
            b1 << Su1.dot(deltap), Sv1.dot(deltap);
            b2 << Su2.dot(deltap), Sv2.dot(deltap);
            Eigen::Matrix<Rational, 2, 1> deltauv1 = A1.inverse()*b1;    
            Eigen::Matrix<Rational, 2, 1> deltauv2 = A2.inverse()*b2;
            Eigen::Matrix<Rational, 1, 2> trans_deltauv1 = deltauv1.transpose();
            Eigen::Matrix<Rational, 1, 2> product1 = trans_deltauv1 * B1;
            Rational k1v = product1 * deltauv1;
            Rational k1 = Rational(0.5) * k1v;
            
            // 同样处理第二个表达式
            Eigen::Matrix<Rational, 1, 2> trans_deltauv2 = deltauv2.transpose();
            Eigen::Matrix<Rational, 1, 2> product2 = trans_deltauv2 * B2;
            Rational k2v = product2 * deltauv2;
            Rational k2 = Rational(0.5) * k2v;
            
            // 计算差值
            Rational dis = k1 - k2;
            if(!ok)
                dis = k1 + k2;
            if(dis < 0) 
            {
                std::cout << "Penetration detected at deltap: " << deltap << std::endl;
                return false; 
            }
        }
    }
    std::cout << "No penetration detected." << std::endl;
    return true;
}

//产生边面碰撞
CollisionPoint generateEF(unsigned seed)
{
    std::cout<<"tasktype: edge-face"<<std::endl;
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);//分布范围
    
    Array2r uv1=generateValidTriangleUV(engine), uv2=edgeface::genUV(engine);
    
    auto patch1 = generateRandomPatch(engine, dist, dist, dist);
    auto patch2 = generateRandomPatch(engine, dist, dist, dist);

    Vector3r targetNormal =  Vector3r(0, 0, 1);

    for(int i=0; i<6; i++)
        std::cout<<"patch1 ctrlp "<<i<<": "<<patch1.ctrlp[i].transpose()<<std::endl;
    for(int i=0; i<6; i++)
        std::cout<<"patch2 ctrlp "<<i<<": "<<patch2.ctrlp[i].transpose()<<std::endl;

    // auto tagent = computeBoundaryTangent(patch2, uv2);

    //调整patch1法向
    adjustPatchToOrthogonalDirectSolve(patch1, uv1, targetNormal);

    std::cout << "After adjusting patch1 to target normal:" << std::endl;
    Vector3r actualNormal1 = patch1.evaluateNormal(uv1);
    std::cout << "Actual patch1 normal: " << actualNormal1.transpose() << std::endl;
    std::cout << "Normal error: " << (actualNormal1 - targetNormal).norm() << std::endl;

    for(int i=0; i<6; i++)
        std::cout<<"patch1 ctrlp "<<i<<": "<<patch1.ctrlp[i].transpose()<<std::endl;

    //调整patch2切向量
    edgeface::adjustEF(patch2, uv2, targetNormal);

    for(int i=0; i<6; i++)
        std::cout<<"patch2 ctrlp "<<i<<": "<<patch2.ctrlp[i].transpose()<<std::endl;


    std::cout << "After adjusting patch2 tangent:" << std::endl;
    Vector3r tangent2 = computeBoundaryTangent(patch2, uv2);
    std::cout << "Patch2 tangent: " << tangent2.transpose() << std::endl;
    std::cout << "Tangent · Normal: " << tangent2.dot(targetNormal) << std::endl;

    Vector3r collisionPoint = patch2.evaluatePatchPoint(uv2); 

    Vector3r offset = collisionPoint - patch1.evaluatePatchPoint(uv1);
    for(int i = 0; i < 6; i++) 
        patch1.ctrlp[i] = patch1.ctrlp[i] + offset;
    
    auto patchNormal1 = patch1.evaluateNormal(uv1);
    auto [vel1, vel2] = edgeface::genColVel(engine, patchNormal1, patch2, uv2);
    std::cout << "vel1: " << vel1.transpose() << std::endl;
    std::cout << "vel2: " << vel2.transpose() << std::endl;
    patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);

    // patch1.velp = generateVelocityFieldIndependent(engine, actualNormal1);
    // patch2.velp = generateVelocityFieldIndependent(engine, -actualNormal1);


    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = edgeface::genLocalParam(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);

    // std::cout << "local_uv1: " << local_uv1[0] << " " << local_uv1[1] << std::endl;
    // std::cout << "local_uv2: " << local_uv2[0] << " " << local_uv2[1] << std::endl;


    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);

    localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);


    std::cout << "Local patch1 control points:" << std::endl;
    for(int i=0; i<6; i++)
        std::cout<<"local patch1 ctrlp "<<i<<": "<<localPatch1.ctrlp[i].transpose()<<std::endl;
    std::cout << "Local patch2 control points:" << std::endl;
    for(int i=0; i<6; i++)
        std::cout<<"local patch2 ctrlp "<<i<<": "<<localPatch2.ctrlp[i].transpose()<<std::endl;

    std::cout << "Local patch1 velocities:" << std::endl;  
    for(int i=0; i<6; i++)
        std::cout<<"local patch1 velp "<<i<<": "<<localPatch1.velp[i].transpose()<<std::endl;
    std::cout << "Local patch2 velocities:" << std::endl;
    for(int i=0; i<6; i++)
        std::cout<<"local patch2 velp "<<i<<": "<<localPatch2.velp[i].transpose()<<std::endl;

    CollisionPoint cp = { localPatch1, localPatch2, uv1, uv2, local_uv1, local_uv2, targetNormal, localPatch2.evaluateNormal(local_uv2), vel1, vel2};
    
    if(checkCollisionPointPrecision(cp)) {
        std::cout << "Precision check passed." << std::endl;
    }
    else {
        std::cout << "Precision check failed!" << std::endl;
        cp.vel1 = Vector3r(0,0,0);
        cp.vel2 = Vector3r(0,0,0);
    }

    std::cout << "collision point1: " << localPatch1.evaluatePatchPoint(local_uv1).transpose() << std::endl;
    std::cout << "collision point2: " << localPatch2.evaluatePatchPoint(local_uv2).transpose() << std::endl;
   
    return cp;
}

CollisionPoint generateEE(unsigned seed)
{
    std::cout << "tasktype: edge-edge" << std::endl;
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);
    
    // 生成两个边界参数坐标
    Array2r uv1 = edgeface::genUV(engine);
    Array2r uv2 = edgeface::genUV(engine);
    
    // 生成两个随机patch（控制点都是简单有理数）
    auto patch1 = generateRandomPatch(engine, dist, dist, dist);
    auto patch2 = generateRandomPatch(engine, dist, dist, dist);
    
    std::cout << "uv1: " << uv1.transpose() << std::endl;
    std::cout << "uv2: " << uv2.transpose() << std::endl;
    
    // 计算碰撞点
    Vector3r collisionPoint = patch2.evaluatePatchPoint(uv2);
    
    // 平移patch1使碰撞点重合（唯一的几何约束）
    Vector3r offset = collisionPoint - patch1.evaluatePatchPoint(uv1);
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + offset;
    }
    
    std::cout << "After translation:" << std::endl;
    std::cout << "Patch1 collision point: " << patch1.evaluatePatchPoint(uv1).transpose() << std::endl;
    std::cout << "Patch2 collision point: " << patch2.evaluatePatchPoint(uv2).transpose() << std::endl;
    std::cout << "Distance: " << (patch1.evaluatePatchPoint(uv1) - patch2.evaluatePatchPoint(uv2)).norm() << std::endl;
    
    // 生成满足分离条件的速度
    auto [vel1, vel2] = edgeedge::genColVel(engine, patch1, patch2, uv1, uv2);
    // std::cout << "vel1: " << vel1.transpose() << std::endl;
    // std::cout << "vel2: " << vel2.transpose() << std::endl;
    
    // 生成速度场
    patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);

    std::cout << "patch1 normal: " << patch1.evaluateNormal(uv1).transpose() << std::endl;
    std::cout << "patch2 normal: " << patch2.evaluateNormal(uv2).transpose() << std::endl;

    Vector3r usedNormal = patch1.evaluateNormal(uv1);
    if(usedNormal == Vector3r(0,0,0))
        usedNormal = patch2.evaluateNormal(uv2);

    // patch1.velp = generateVelocityFieldIndependent(engine, usedNormal);
    // patch2.velp = generateVelocityFieldIndependent(engine, -usedNormal);

    // 局部切分（使用统一的边界参数域生成）
    TriParamBound bound1 = edgeface::genLocalParam(uv1);
    TriParamBound bound2 = edgeface::genLocalParam(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);
    
    std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;

    // 验证local_uv是否在合理范围内
    if (local_uv1[0] < Rational(0) || local_uv1[1] < Rational(0) || 
        local_uv1[0] + local_uv1[1] > Rational(1)) {
        std::cerr << "Warning: local_uv1 out of valid range!" << std::endl;
    }
    
    if (local_uv2[0] < Rational(0) || local_uv2[1] < Rational(0) || 
        local_uv2[0] + local_uv2[1] > Rational(1)) {
        std::cerr << "Warning: local_uv2 out of valid range!" << std::endl;
    }

    // 切分曲面
    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);

    // 切分速度场
    localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);

    std::cout << "localpatch1 control points:" << std::endl;   
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.ctrlp[i].transpose() << std::endl;
    std::cout << "localpatch2 control points:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "localpatch1 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.velp[i].transpose() << std::endl;
    std::cout << "localpatch2 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.velp[i].transpose() << std::endl;

    // 构建碰撞点结构
    CollisionPoint cp = { 
        localPatch1, localPatch2, 
        uv1, uv2, 
        local_uv1, local_uv2, 
        localPatch1.evaluateNormal(local_uv1), 
        localPatch2.evaluateNormal(local_uv2), 
        vel1, vel2
    };
    
    // 最终验证
    std::cout << "Final verification:" << std::endl;
    std::cout << "Local collision point1: " << localPatch1.evaluatePatchPoint(local_uv1).transpose() << std::endl;
    std::cout << "Local collision point2: " << localPatch2.evaluatePatchPoint(local_uv2).transpose() << std::endl;
    std::cout << "Local distance: " << (localPatch1.evaluatePatchPoint(local_uv1) - 
                                         localPatch2.evaluatePatchPoint(local_uv2)).norm() << std::endl;
    
    // 输出边界切向量信息（用于调试）
    Vector3r tangent1 = computeBoundaryTangent(localPatch1, local_uv1);
    Vector3r tangent2 = computeBoundaryTangent(localPatch2, local_uv2);
    std::cout << "Edge1 tangent: " << tangent1.transpose() << std::endl;
    std::cout << "Edge2 tangent: " << tangent2.transpose() << std::endl;
    std::cout << "Tangent dot product: " << tangent1.dot(tangent2) << std::endl;
   
    // 精度检测
    if(checkCollisionPointPrecision(cp)) {
        std::cout << "Precision check passed." << std::endl;
    }
    else {
        std::cout << "Precision check failed!" << std::endl;
        cp.vel1 = Vector3r(0,0,0);
        cp.vel2 = Vector3r(0,0,0);
    }
   
    return cp;
}

CollisionPoint generateVF(unsigned seed)
{
    std::cout << "tasktype: vertex-face" << std::endl;
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);//分布范围
    Array2r uv1=generateValidTriangleUV(engine), uv2=vertexface::genUV(engine);
    auto patch1 = generateRandomPatch(engine, dist, dist, dist);
    auto patch2 = generateRandomPatch(engine, dist, dist, dist);
    Vector3r collisionPoint = patch2.evaluatePatchPoint(uv2); 

    // auto tagent2 = computeBoundaryTangent(patch2, uv2);
    
    // edgeface::adjustEF(patch1, uv1, tagent2);
    Vector3r offset = collisionPoint - patch1.evaluatePatchPoint(uv1);
    for(int i = 0; i < 6; i++) 
        patch1.ctrlp[i] = patch1.ctrlp[i] + offset;
    
    auto patchNormal1 = patch1.evaluateNormal(uv1);
    auto [vel1, vel2] = vertexface::genColVel(engine, patchNormal1, patch2, uv2);
    // std::cout << "vel1: " << vel1.transpose() << std::endl;
    // std::cout << "vel2: " << vel2.transpose() << std::endl;
    patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);
    // patch1.velp = generateVelocityFieldIndependent(engine, patchNormal1);
    // patch2.velp = generateVelocityFieldIndependent(engine, -patchNormal1);

    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);

    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);

    localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);

    std::cout << "localpatch1 control points:" << std::endl;   
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.ctrlp[i].transpose() << std::endl;
    std::cout << "localpatch2 control points:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "localpatch1 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.velp[i].transpose() << std::endl;
    std::cout << "localpatch2 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.velp[i].transpose() << std::endl;

    CollisionPoint cp = { localPatch1, localPatch2, uv1, uv2, local_uv1, local_uv2, patchNormal1, localPatch2.evaluateNormal(local_uv2), vel1, vel2};
    
    std::cout << "collision point1: " << localPatch1.evaluatePatchPoint(local_uv1).transpose() << std::endl;
    std::cout << "collision point2: " << localPatch2.evaluatePatchPoint(local_uv2).transpose() << std::endl;
   
    // 精度检测
    if(checkCollisionPointPrecision(cp)) {
        std::cout << "Precision check passed." << std::endl;
    }
    else {
        std::cout << "Precision check failed!" << std::endl;
        cp.vel1 = Vector3r(0,0,0);
        cp.vel2 = Vector3r(0,0,0);
    }

    return cp;
}

CollisionPoint generateVE(unsigned seed)
{
    std::cout << "tasktype: vertex-edge" << std::endl;
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);//分布范围
    Array2r uv1=edgeface::genUV(engine), uv2=vertexface::genUV(engine);
    auto patch1 = generateRandomPatch(engine, dist, dist, dist);
    auto patch2 = generateRandomPatch(engine, dist, dist, dist);
    Vector3r collisionPoint = patch2.evaluatePatchPoint(uv2); 

    std::cout << "uv1: " << uv1[0] << " " << uv1[1] << std::endl;
    std::cout << "uv2: " << uv2[0] << " " << uv2[1] << std::endl;

    // auto tagent2 = computeBoundaryTangent(patch2, uv2);
    
    // edgeface::adjustEF(patch1, uv1, tagent2);
    Vector3r offset = collisionPoint - patch1.evaluatePatchPoint(uv1);
    for(int i = 0; i < 6; i++) 
        patch1.ctrlp[i] = patch1.ctrlp[i] + offset;
    
    auto patchNormal1 = patch1.evaluateNormal(uv1);

    std::cout << "patchNormal1: " << patchNormal1.transpose() << std::endl;
    Vector3r usedNormal = patchNormal1;
    if(usedNormal == Vector3r(0,0,0))
        usedNormal = patch2.evaluateNormal(uv2);

    auto [vel1, vel2] = vertexedge::genColVel(engine, patch1, patch2, uv1, uv2);
    // std::cout << "vel1: " << vel1.transpose() << std::endl;
    // std::cout << "vel2: " << vel2.transpose() << std::endl;
    patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);

    // patch1.velp = generateVelocityFieldIndependent(engine, usedNormal);
    // patch2.velp = generateVelocityFieldIndependent(engine, -usedNormal);

    TriParamBound bound1 = edgeface::genLocalParam(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);

    std::cout << "local_uv1: " << local_uv1[0] << " " << local_uv1[1] << std::endl;
    std::cout << "local_uv2: " << local_uv2[0] << " " << local_uv2[1] << std::endl;

    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);

    localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);

    std::cout << "localpatch1 control points:" << std::endl;   
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.ctrlp[i].transpose() << std::endl;
    std::cout << "localpatch2 control points:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "localpatch1 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.velp[i].transpose() << std::endl;
    std::cout << "localpatch2 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.velp[i].transpose() << std::endl;

    CollisionPoint cp = { localPatch1, localPatch2, uv1, uv2, local_uv1, local_uv2, localPatch1.evaluateNormal(local_uv1), localPatch2.evaluateNormal(local_uv2), vel1, vel2};
    
    std::cout << "collision point1: " << localPatch1.evaluatePatchPoint(local_uv1).transpose() << std::endl;
    std::cout << "collision point2: " << localPatch2.evaluatePatchPoint(local_uv2).transpose() << std::endl;
   
    // 精度检测
    if(checkCollisionPointPrecision(cp)) {
        std::cout << "Precision check passed." << std::endl;
    }
    else {
        std::cout << "Precision check failed!" << std::endl;
        cp.vel1 = Vector3r(0,0,0);
        cp.vel2 = Vector3r(0,0,0);
    }

    return cp;
}

CollisionPoint generateVV(unsigned seed)
{
    std::cout << "tasktype: vertex-vertex" << std::endl;
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);//分布范围
    Array2r uv1=vertexface::genUV(engine), uv2=vertexface::genUV(engine);
    auto patch1 = generateRandomPatch(engine, dist, dist, dist);
    auto patch2 = generateRandomPatch(engine, dist, dist, dist);
    Vector3r collisionPoint = patch2.evaluatePatchPoint(uv2); 

    // auto tagent2 = computeBoundaryTangent(patch2, uv2);
    
    // edgeface::adjustEF(patch1, uv1, tagent2);
    Vector3r offset = collisionPoint - patch1.evaluatePatchPoint(uv1);
    for(int i = 0; i < 6; i++) 
        patch1.ctrlp[i] = patch1.ctrlp[i] + offset;
    
    auto patchNormal1 = patch1.evaluateNormal(uv1);
    std::cout << "patchNormal1: " << patchNormal1.transpose() << std::endl;
    Vector3r usedNormal = patchNormal1;
    if(usedNormal == Vector3r(0,0,0))
        usedNormal = patch2.evaluateNormal(uv2);

    auto [vel1, vel2] = vertexvertex::genColVel(engine, patch1, patch2, uv1, uv2);
    // std::cout << "vel1: " << vel1.transpose() << std::endl;
    // std::cout << "vel2: " << vel2.transpose() << std::endl;
    patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);

    // patch1.velp = generateVelocityFieldIndependent(engine, usedNormal);
    // patch2.velp = generateVelocityFieldIndependent(engine, -usedNormal);

    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);

    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);

    localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);

    std::cout << "localpatch1 control points:" << std::endl;   
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.ctrlp[i].transpose() << std::endl;
    std::cout << "localpatch2 control points:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "localpatch1 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch1.velp[i].transpose() << std::endl;
    std::cout << "localpatch2 velocities:" << std::endl;
    for(int i = 0; i < 6; i++)
        std::cout << localPatch2.velp[i].transpose() << std::endl;

    CollisionPoint cp = { localPatch1, localPatch2, uv1, uv2, local_uv1, local_uv2, localPatch1.evaluateNormal(local_uv1), localPatch2.evaluateNormal(local_uv2), vel1, vel2};
    
    std::cout << "collision point1: " << localPatch1.evaluatePatchPoint(local_uv1).transpose() << std::endl;
    std::cout << "collision point2: " << localPatch2.evaluatePatchPoint(local_uv2).transpose() << std::endl;
   
    // 精度检测
    if(checkCollisionPointPrecision(cp)) {
        std::cout << "Precision check passed." << std::endl;
    }
    else {
        std::cout << "Precision check failed!" << std::endl;
        cp.vel1 = Vector3r(0,0,0);
        cp.vel2 = Vector3r(0,0,0);
    }

    return cp;
}



// 生成擦肩而过的情况 (Near-Miss)
CollisionPoint generateNearMissFF(unsigned seed, Rational gap)
{
    std::cout << "tasktype: near-miss FF (based on standard FF)" << std::endl;
    std::mt19937_64 engine(seed);
    
    // ========== 1. 调用标准FF生成器（tasktype=0） ==========
    // 这会生成两个法线反向、恰好相切的patch
    CollisionPoint standardFF = generateSeparatedRandomBezierPatches(seed, 0);

    if(standardFF.local_uv1[0] == 0 && standardFF.local_uv1[1] == 0 && standardFF.local_uv2[0] == 0 && standardFF.local_uv2[1] == 0) {
        std::cerr << "Failed to generate valid standard FF collision point." << std::endl;
        return CollisionPoint();
    }

    
    // if(!testAdditionalCollisions(standardFF)) {
    //     std::cerr << "Standard FF generation produced additional collisions. Aborting near-miss generation." << std::endl;
    // }

    // 提取生成的patches和UV坐标
    TriQuadBezier patch1 = standardFF.patch1;
    TriQuadBezier patch2 = standardFF.patch2;
    Array2r uv1 = standardFF.local_uv1;
    Array2r uv2 = standardFF.local_uv2;
    
    std::cout << "Standard FF patches generated" << std::endl;
    std::cout << "uv1: " << uv1.transpose() << std::endl;
    std::cout << "uv2: " << uv2.transpose() << std::endl;
    
    // ========== 2. 计算法线和偏导数 ==========
    Vector3r patch1Normal = standardFF.normal1;
    Vector3r patch2Normal = standardFF.normal2;
    
    Vector3r partialU1 = patch1.evaluatePartialU(uv1);
    Vector3r partialV1 = patch1.evaluatePartialV(uv1);
    Vector3r partialU2 = patch2.evaluatePartialU(uv2);
    Vector3r partialV2 = patch2.evaluatePartialV(uv2);
    
    // std::cout << "\n=== Normals and Partial Derivatives ===" << std::endl;
    // std::cout << "patch1 normal: " << patch1Normal.transpose() << std::endl;
    // std::cout << "patch1 partialU: " << partialU1.transpose() << std::endl;
    // std::cout << "patch1 partialV: " << partialV1.transpose() << std::endl;
    // std::cout << "patch2 normal: " << patch2Normal.transpose() << std::endl;
    // std::cout << "patch2 partialU: " << partialU2.transpose() << std::endl;
    // std::cout << "patch2 partialV: " << partialV2.transpose() << std::endl;
    
    // ========== 3. 通过采样确定分离方向，然后沿法线拉开 ==========
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    
    std::cout << "\nBefore separation:" << std::endl;
    std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Distance: " << (double)(collisionPoint1 - collisionPoint2).norm() << std::endl;
    
    // 归一化法线
    Vector3r normalizedNormal1 = patch1Normal;
    // if (normalizedNormal1.norm() > Rational(0)) {
    //     normalizedNormal1 = normalizedNormal1 / normalizedNormal1.norm();
    // }
    
    std::cout << "\n=== Sampling to Determine Separation Direction ===" << std::endl;
    
    // 调用采样函数获取从patch1指向patch2的方向
    Vector3r sampleDirection = sampleSeparationDirection(patch1, patch2, uv1, uv2, patch1Normal);
    
    std::cout << "Sample direction (patch1 -> patch2): " << sampleDirection.transpose() << std::endl;
    std::cout << "Sample direction norm: " << (double)sampleDirection.norm() << std::endl;
    
    // 计算采样方向与法线的点积
    Rational normalDotSample = normalizedNormal1.dot(sampleDirection);
    
    std::cout << "Normal · SampleDirection: " << (double)normalDotSample << std::endl;
    
    // 如果点积为正，说明法线指向与分离方向相反，需要反转
    // 我们希望法线指向patch2的方向（分离方向）
    if (normalDotSample > Rational(0)) {
        std::cout << "Reversing normal direction for separation" << std::endl;
        normalizedNormal1 = -normalizedNormal1;
    } else {
        std::cout << "Normal direction is correct for separation" << std::endl;
    }

    double normNormal = (double)normalizedNormal1.norm();
    double log2norm = std::log2(normNormal);
    int n = static_cast<int>(std::round(log2norm));
    Rational scale = Rational(1) / Rational(1 << n);
    std::cout << "n: " << n << ", scale: " << scale << std::endl;
    std::cout << "Normal * scale: " << (normalizedNormal1 * scale).transpose() << std::endl;      

    // Vector3r separationOffset = gap * normalizedNormal1 * Rational(512);
    Vector3r separationOffset = gap * normalizedNormal1 * scale;
    

    std::cout << "\n=== Applying Separation ===" << std::endl;
    std::cout << "Gap distance: " << (double)gap << std::endl;
    std::cout << "Final separation direction: " << normalizedNormal1.transpose() << std::endl;
    std::cout << "Separation offset: " << separationOffset.transpose() << std::endl;
    
    // 将patch2沿法线方向移动
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + separationOffset;
    }
    
    // 验证分离后的距离
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualGap = (collisionPoint1 - collisionPoint2).norm();
    
    // std::cout << "\nAfter separation:" << std::endl;
    // std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    // std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    // std::cout << "Actual gap: " << (double)actualGap << std::endl;
    // std::cout << "Gap error: " << (double)(actualGap - gap) << std::endl;
    
    // ========== 4. 生成垂直于法线的速度（擦肩而过） ==========
    // 使用partialU和partialV的线性组合，它们天然在切平面内
    Vector3r vel1 = generateVelocityPerpendicularToNormal(engine, partialU1, partialV1);
    Vector3r vel2 = generateVelocityPerpendicularToNormal(engine, partialU2, partialV2);
    
    // std::cout << "\n=== Velocity Information ===" << std::endl;
    // std::cout << "vel1: " << vel1.transpose() << std::endl;
    // std::cout << "vel2: " << vel2.transpose() << std::endl;
    // std::cout << "vel1 · normal1: " << (double)vel1.dot(patch1Normal) << " (should be ~0)" << std::endl;
    // std::cout << "vel2 · normal2: " << (double)vel2.dot(patch2Normal) << " (should be ~0)" << std::endl;
    
    // Vector3r relativeVel = vel1 - vel2;
    // std::cout << "Relative velocity: " << relativeVel.transpose() << std::endl;
    // std::cout << "Relative speed: " << (double)relativeVel.norm() << std::endl;
    // std::cout << "Relative vel · normal1: " << (double)relativeVel.dot(patch1Normal) 
    //           << " (should be ~0 for near-miss)" << std::endl;
  


    // ========== 5. 生成速度场 ==========
    for(int i=0; i<6; i++)
        patch1.velp[i] = vel1;
    for(int i=0; i<6; i++)
        patch2.velp[i] = vel2;
    
    // std::cout << "patch1 ctrlp: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch1.ctrlp[i].transpose() << std::endl;
    // std::cout << "patch2 ctrlp: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch2.ctrlp[i].transpose() << std::endl;
    
    // std::cout << "patch1 velp: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch1.velp[i].transpose() << std::endl;
    // std::cout << "patch2 velp: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch2.velp[i].transpose() << std::endl;

    // 适应batchProcess folder的设定，2个patch沿速度反向运动1/2s
    for(int i=0; i<6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * Rational("-1/2");
    }

    // std::cout << "patch1 ctrlp after setting: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch1.ctrlp[i].transpose() << std::endl;
    // std::cout << "patch2 ctrlp after setting: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch2.ctrlp[i].transpose() << std::endl;
    
    // patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    // patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);
    
    // ========== 6. 局部切分 ==========
    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);
    
    // std::cout << "\n=== Local Parameters ===" << std::endl;
    // std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    // std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;
    
    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);
    
    // localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    // localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);
    
    // // ========== 7. 最终验证 ==========
    // Vector3r localPoint1 = localPatch1.evaluatePatchPoint(local_uv1);
    // Vector3r localPoint2 = localPatch2.evaluatePatchPoint(local_uv2);
    // Rational localGap = (localPoint1 - localPoint2).norm();
    
    // std::cout << "\n=== Final Verification ===" << std::endl;
    // std::cout << "Local collision point1: " << localPoint1.transpose() << std::endl;
    // std::cout << "Local collision point2: " << localPoint2.transpose() << std::endl;
    // std::cout << "Local gap: " << (double)localGap << std::endl;
    
    Vector3r localNormal1 = localPatch1.evaluateNormal(local_uv1);
    Vector3r localNormal2 = localPatch2.evaluateNormal(local_uv2);
    // std::cout << "Local normal1: " << localNormal1.transpose() << std::endl;
    // std::cout << "Local normal2: " << localNormal2.transpose() << std::endl;
    
    // Vector3r localVel1 = localPatch1.evaluatePatchVel(local_uv1);
    // Vector3r localVel2 = localPatch2.evaluatePatchVel(local_uv2);
    // std::cout << "Local vel1: " << localVel1.transpose() << std::endl;
    // std::cout << "Local vel2: " << localVel2.transpose() << std::endl;
    // std::cout << "Local vel1 · normal1: " << (double)localVel1.dot(localNormal1) << std::endl;
    // std::cout << "Local vel2 · normal2: " << (double)localVel2.dot(localNormal2) << std::endl;
    
    // ========== 8. 构建CollisionPoint ==========
    CollisionPoint cp = {
        patch1,
        patch2,
        uv1,
        uv2,
        local_uv1,
        local_uv2,
        localNormal1,
        localNormal2,
        vel1,
        vel2
    };
    
    std::cout << "\n=== Near-Miss Generation Summary ===" << std::endl;
    std::cout << "Configuration: Near-miss with perpendicular velocities" << std::endl;
    std::cout << "Based on standard FF collision (tasktype=0)" << std::endl;
    std::cout << "Target gap: " << (double)gap << std::endl;
    // std::cout << "Actual gap: " << (double)localGap << std::endl;
    std::cout << "Velocities are perpendicular to normals (sliding motion)" << std::endl;
    
    return cp;
}

// 生成面面恰好碰到的情况 (Near-Hit FF)
// 与NearMissFF的区别：不进行平移，两个patch保持相切状态，速度垂直于法线
CollisionPoint generateNearHitFF(unsigned seed)
{
    std::cout << "tasktype: near-hit FF (based on standard FF)" << std::endl;
    std::mt19937_64 engine(seed);
    
    // ========== 1. 调用标准FF生成器（tasktype=0） ==========
    // 这会生成两个法线反向、恰好相切的patch
    CollisionPoint standardFF = generateSeparatedRandomBezierPatches(seed, 0);

    if(standardFF.local_uv1[0] == 0 && standardFF.local_uv1[1] == 0 && 
       standardFF.local_uv2[0] == 0 && standardFF.local_uv2[1] == 0) {
        std::cerr << "Failed to generate valid standard FF collision point." << std::endl;
        return CollisionPoint();
    }

    // 提取生成的patches和UV坐标
    TriQuadBezier patch1 = standardFF.patch1;
    TriQuadBezier patch2 = standardFF.patch2;
    Array2r uv1 = standardFF.local_uv1;
    Array2r uv2 = standardFF.local_uv2;
    
    std::cout << "Standard FF patches generated for near-hit" << std::endl;
    std::cout << "uv1: " << uv1.transpose() << std::endl;
    std::cout << "uv2: " << uv2.transpose() << std::endl;
    
    // ========== 2. 计算法线和偏导数 ==========
    Vector3r patch1Normal = standardFF.normal1;
    Vector3r patch2Normal = standardFF.normal2;
    
    Vector3r partialU1 = patch1.evaluatePartialU(uv1);
    Vector3r partialV1 = patch1.evaluatePartialV(uv1);
    Vector3r partialU2 = patch2.evaluatePartialU(uv2);
    Vector3r partialV2 = patch2.evaluatePartialV(uv2);
    
    // ========== 3. 验证碰撞点位置（不做平移，保持相切） ==========
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    
    std::cout << "\nCollision points (no separation):" << std::endl;
    std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Distance: " << (double)(collisionPoint1 - collisionPoint2).norm() << std::endl;
    
    // ========== 4. 生成垂直于法线的速度（恰好碰到） ==========
    // 使用partialU和partialV的线性组合，它们天然在切平面内
    Vector3r vel1 = generateVelocityPerpendicularToNormal(engine, partialU1, partialV1);
    Vector3r vel2 = generateVelocityPerpendicularToNormal(engine, partialU2, partialV2);
    
    // ========== 5. 生成速度场 ==========
    for(int i = 0; i < 6; i++)
        patch1.velp[i] = vel1;
    for(int i = 0; i < 6; i++)
        patch2.velp[i] = vel2;
    
    // 适应batchProcess folder的设定，2个patch沿速度反向运动1/2s
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * Rational("-1/2");
    }
    
    // ========== 6. 局部切分 ==========
    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);
    
    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);
    
    Vector3r localNormal1 = localPatch1.evaluateNormal(local_uv1);
    Vector3r localNormal2 = localPatch2.evaluateNormal(local_uv2);
    
    // ========== 7. 构建CollisionPoint ==========
    CollisionPoint cp = {
        patch1,
        patch2,
        uv1,
        uv2,
        local_uv1,
        local_uv2,
        localNormal1,
        localNormal2,
        vel1,
        vel2
    };
    
    std::cout << "\n=== Near-Hit Generation Summary ===" << std::endl;
    std::cout << "Configuration: Near-hit with perpendicular velocities" << std::endl;
    std::cout << "Based on standard FF collision (tasktype=0)" << std::endl;
    std::cout << "Gap: 0 (patches are tangent)" << std::endl;
    std::cout << "Velocities are perpendicular to normals (touching motion)" << std::endl;
    
    
    return cp;
}

// 生成边-面恰好碰到的情况 (Near-Hit EF)
// 核心思路：调用 NearMissEF，然后撤销平移操作
CollisionPoint generateNearHitEF(unsigned seed)
{
    std::cout << "tasktype: near-hit EF (calling near-miss EF then removing separation)" << std::endl;
    
    // ========== 1. 调用 NearMissEF 生成器 ==========
    Rational gap = Rational("1/10");  // 使用默认 gap
    CollisionPoint nearMiss = generateNearMissEF(seed, gap);
    
    // if(nearMiss.local_uv1[0] == 0 && nearMiss.local_uv1[1] == 0 && 
    //    nearMiss.local_uv2[0] == 0 && nearMiss.local_uv2[1] == 0) {
    //     std::cerr << "Failed to generate valid near-miss EF collision point." << std::endl;
    //     return CollisionPoint();
    // }
    
    // ========== 2. 提取 patches 和 UV 坐标 ==========
    TriQuadBezier patch1 = nearMiss.patch1;
    TriQuadBezier patch2 = nearMiss.patch2;
    Array2r uv1 = nearMiss.local_uv1;
    Array2r uv2 = nearMiss.local_uv2;
    
    // ========== 3. 计算平移量（通过碰撞点差值） ==========
    // 注意：nearMiss 中的 patch 已经被平移过，且已经反向运动了 1/2s
    // 我们需要先恢复到 t=0 的状态，计算平移量，然后撤销平移，再重新反向运动
    
    // 先恢复到 t=0
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - patch2.velp[i] * Rational("-1/2");
    }
    
    // 计算当前碰撞点（已平移状态）
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    
    // 计算平移量（patch2 被平移的量）
    Vector3r separationOffset = collisionPoint2 - collisionPoint1;
    
    std::cout << "\n=== Removing Separation ===" << std::endl;
    std::cout << "Separation offset to remove: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // ========== 4. 撤销平移（让 patch2 回到相切状态） ==========
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] - separationOffset;
    }
    
    // 验证撤销结果
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualDistance = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "After removing separation:" << std::endl;
    std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Distance: " << (double)actualDistance << " (should be ~0)" << std::endl;
    
    // ========== 5. 重新应用反向运动 ==========
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * Rational("-1/2");
    }
    
    // ========== 6. 构建 CollisionPoint（复用 NearMiss 的其他信息） ==========
    CollisionPoint cp = nearMiss;  // 复用速度、局部UV等信息
    cp.patch1 = patch1;  // 更新 patch（已撤销平移）
    cp.patch2 = patch2;
    
    std::cout << "\n=== Near-Hit EF Generation Summary ===" << std::endl;
    std::cout << "Configuration: Near-hit (separation removed from near-miss)" << std::endl;
    std::cout << "Gap: 0 (edge and face are tangent)" << std::endl;
    std::cout << "Velocities: inherited from near-miss EF" << std::endl;
    
    return cp;
}

// 生成边-边恰好碰到的情况 (Near-Hit EE)
// 核心思路：调用 NearMissEE，然后撤销平移操作
CollisionPoint generateNearHitEE(unsigned seed)
{
    std::cout << "tasktype: near-hit EE (calling near-miss EE then removing separation)" << std::endl;
    
    // ========== 1. 调用 NearMissEE 生成器 ==========
    Rational gap = Rational("1/131072");  // 使用默认 gap
    CollisionPoint nearMiss = generateNearMissEE(seed);
    
    // if(nearMiss.local_uv1[0] == 0 && nearMiss.local_uv1[1] == 0 && 
    //    nearMiss.local_uv2[0] == 0 && nearMiss.local_uv2[1] == 0) {
    //     std::cerr << "Failed to generate valid near-miss EE collision point." << std::endl;
    //     return CollisionPoint();
    // }
    
    // ========== 2. 提取 patches 和 UV 坐标 ==========
    TriQuadBezier patch1 = nearMiss.patch1;
    TriQuadBezier patch2 = nearMiss.patch2;
    Array2r uv1 = nearMiss.local_uv1;
    Array2r uv2 = nearMiss.local_uv2;
    
    // ========== 3. 计算平移量（通过碰撞点差值） ==========
    // 先恢复到 t=0
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - patch2.velp[i] * Rational("-1/2");
    }
    
    // 计算当前碰撞点（已平移状态）
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    
    // 计算平移量（patch2 被平移的量）
    Vector3r separationOffset = collisionPoint2 - collisionPoint1;
    
    std::cout << "\n=== Removing Separation ===" << std::endl;
    std::cout << "Separation offset to remove: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // ========== 4. 撤销平移（让 patch2 回到相切状态） ==========
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] - separationOffset;
    }
    
    // 验证撤销结果
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualDistance = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "After removing separation:" << std::endl;
    std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Distance: " << (double)actualDistance << " (should be ~0)" << std::endl;
    
    // ========== 5. 重新应用反向运动 ==========
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * Rational("-1/2");
    }
    
    // ========== 6. 构建 CollisionPoint（复用 NearMiss 的其他信息） ==========
    CollisionPoint cp = nearMiss;  // 复用速度、局部UV等信息
    cp.patch1 = patch1;  // 更新 patch（已撤销平移）
    cp.patch2 = patch2;
    
    std::cout << "\n=== Near-Hit EE Generation Summary ===" << std::endl;
    std::cout << "Configuration: Near-hit (separation removed from near-miss)" << std::endl;
    std::cout << "Gap: 0 (edges are tangent)" << std::endl;
    std::cout << "Velocities: inherited from near-miss EE" << std::endl;
    
    return cp;
}

// 生成点-面恰好碰到的情况 (Near-Hit VF)
// 核心思路：调用 NearMissVF，然后撤销平移操作
CollisionPoint generateNearHitVF(unsigned seed)
{
    std::cout << "tasktype: near-hit VF (calling near-miss VF then removing separation)" << std::endl;
    
    // ========== 1. 调用 NearMissVF 生成器 ==========
    Rational gap = Rational("1/131072");  // 使用默认 gap
    CollisionPoint nearMiss = generateNearMissVF(seed, gap);
    
    // if(nearMiss.local_uv1[0] == 0 && nearMiss.local_uv1[1] == 0 && 
    //    nearMiss.local_uv2[0] == 0 && nearMiss.local_uv2[1] == 0) {
    //     std::cerr << "Failed to generate valid near-miss VF collision point." << std::endl;
    //     return CollisionPoint();
    // }
    
    // ========== 2. 提取 patches 和 UV 坐标 ==========
    TriQuadBezier patch1 = nearMiss.patch1;
    TriQuadBezier patch2 = nearMiss.patch2;
    Array2r uv1 = nearMiss.local_uv1;
    Array2r uv2 = nearMiss.local_uv2;
    
    // ========== 3. 计算平移量（通过碰撞点差值） ==========
    // 先恢复到 t=0
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - patch2.velp[i] * Rational("-1/2");
    }
    
    // 计算当前碰撞点（已平移状态）
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    
    // 计算平移量（patch2 被平移的量）
    Vector3r separationOffset = collisionPoint2 - collisionPoint1;
    
    std::cout << "\n=== Removing Separation ===" << std::endl;
    std::cout << "Separation offset to remove: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // ========== 4. 撤销平移（让 patch2 回到相切状态） ==========
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] - separationOffset;
    }
    
    // 验证撤销结果
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualDistance = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "After removing separation:" << std::endl;
    std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Distance: " << (double)actualDistance << " (should be ~0)" << std::endl;
    
    // ========== 5. 重新应用反向运动 ==========
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * Rational("-1/2");
    }
    
    // ========== 6. 构建 CollisionPoint（复用 NearMiss 的其他信息） ==========
    CollisionPoint cp = nearMiss;  // 复用速度、局部UV等信息
    cp.patch1 = patch1;  // 更新 patch（已撤销平移）
    cp.patch2 = patch2;
    
    std::cout << "\n=== Near-Hit VF Generation Summary ===" << std::endl;
    std::cout << "Configuration: Near-hit (separation removed from near-miss)" << std::endl;
    std::cout << "Gap: 0 (vertex and face are tangent)" << std::endl;
    std::cout << "Velocities: inherited from near-miss VF" << std::endl;
    
    return cp;
}

// 生成点-边恰好碰到的情况 (Near-Hit VE)
// 核心思路：调用 NearMissVE，然后撤销平移操作
CollisionPoint generateNearHitVE(unsigned seed)
{
    std::cout << "tasktype: near-hit VE (calling near-miss VE then removing separation)" << std::endl;
    
    // ========== 1. 调用 NearMissVE 生成器 ==========
    Rational gap = Rational("1/131072");  // 使用默认 gap
    CollisionPoint nearMiss = generateNearMissVE(seed, gap);
    
    // if(nearMiss.local_uv1[0] == 0 && nearMiss.local_uv1[1] == 0 && 
    //    nearMiss.local_uv2[0] == 0 && nearMiss.local_uv2[1] == 0) {
    //     std::cerr << "Failed to generate valid near-miss VE collision point." << std::endl;
    //     return CollisionPoint();
    // }
    
    // ========== 2. 提取 patches 和 UV 坐标 ==========
    TriQuadBezier patch1 = nearMiss.patch1;
    TriQuadBezier patch2 = nearMiss.patch2;
    Array2r uv1 = nearMiss.local_uv1;
    Array2r uv2 = nearMiss.local_uv2;
    
    // ========== 3. 计算平移量（通过碰撞点差值） ==========
    // 先恢复到 t=0
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - patch2.velp[i] * Rational("-1/2");
    }
    
    // 计算当前碰撞点（已平移状态）
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    
    // 计算平移量（patch2 被平移的量）
    Vector3r separationOffset = collisionPoint2 - collisionPoint1;
    
    std::cout << "\n=== Removing Separation ===" << std::endl;
    std::cout << "Separation offset to remove: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // ========== 4. 撤销平移（让 patch2 回到相切状态） ==========
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] - separationOffset;
    }
    
    // 验证撤销结果
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualDistance = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "After removing separation:" << std::endl;
    std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Distance: " << (double)actualDistance << " (should be ~0)" << std::endl;
    
    // ========== 5. 重新应用反向运动 ==========
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * Rational("-1/2");
    }
    
    // ========== 6. 构建 CollisionPoint（复用 NearMiss 的其他信息） ==========
    CollisionPoint cp = nearMiss;  // 复用速度、局部UV等信息
    cp.patch1 = patch1;  // 更新 patch（已撤销平移）
    cp.patch2 = patch2;
    
    std::cout << "\n=== Near-Hit VE Generation Summary ===" << std::endl;
    std::cout << "Configuration: Near-hit (separation removed from near-miss)" << std::endl;
    std::cout << "Gap: 0 (vertex and edge are tangent)" << std::endl;
    std::cout << "Velocities: inherited from near-miss VE" << std::endl;
    
    return cp;
}

// 生成点-点恰好碰到的情况 (Near-Hit VV)
// 核心思路：调用 NearMissVV，然后撤销平移操作
CollisionPoint generateNearHitVV(unsigned seed)
{
    std::cout << "tasktype: near-hit VV (calling near-miss VV then removing separation)" << std::endl;
    
    // ========== 1. 调用 NearMissVV 生成器 ==========
    Rational gap = Rational("1/131072");  // 使用默认 gap
    CollisionPoint nearMiss = generateNearMissVV(seed, gap);
    
    // if(nearMiss.local_uv1[0] == 0 && nearMiss.local_uv1[1] == 0 && 
    //    nearMiss.local_uv2[0] == 0 && nearMiss.local_uv2[1] == 0) {
    //     std::cerr << "Failed to generate valid near-miss VV collision point." << std::endl;
    //     return CollisionPoint();
    // }
    
    // ========== 2. 提取 patches 和 UV 坐标 ==========
    TriQuadBezier patch1 = nearMiss.patch1;
    TriQuadBezier patch2 = nearMiss.patch2;
    Array2r uv1 = nearMiss.local_uv1;
    Array2r uv2 = nearMiss.local_uv2;
    
    // ========== 3. 计算平移量（通过碰撞点差值） ==========
    // 先恢复到 t=0
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - patch2.velp[i] * Rational("-1/2");
    }
    
    // 计算当前碰撞点（已平移状态）
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    
    // 计算平移量（patch2 被平移的量）
    Vector3r separationOffset = collisionPoint2 - collisionPoint1;
    
    std::cout << "\n=== Removing Separation ===" << std::endl;
    std::cout << "Separation offset to remove: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // ========== 4. 撤销平移（让 patch2 回到相切状态） ==========
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] - separationOffset;
    }
    
    // 验证撤销结果
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualDistance = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "After removing separation:" << std::endl;
    std::cout << "Point1: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Point2: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Distance: " << (double)actualDistance << " (should be ~0)" << std::endl;
    
    // ========== 5. 重新应用反向运动 ==========
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] + patch1.velp[i] * Rational("-1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] + patch2.velp[i] * Rational("-1/2");
    }
    
    // ========== 6. 构建 CollisionPoint（复用 NearMiss 的其他信息） ==========
    CollisionPoint cp = nearMiss;  // 复用速度、局部UV等信息
    cp.patch1 = patch1;  // 更新 patch（已撤销平移）
    cp.patch2 = patch2;
    
    std::cout << "\n=== Near-Hit VV Generation Summary ===" << std::endl;
    std::cout << "Configuration: Near-hit (separation removed from near-miss)" << std::endl;
    std::cout << "Gap: 0 (vertices are tangent)" << std::endl;
    std::cout << "Velocities: inherited from near-miss VV" << std::endl;
    
    return cp;
}

// 在 generateNearMissFF 函数后添加

// 生成边-面擦肩而过的情况 (Near-Miss EF)
CollisionPoint generateNearMissEF(unsigned seed, Rational gap)
{
    std::cout << "tasktype: near-miss EF (based on standard EF)" << std::endl;
    std::mt19937_64 engine(seed);
    
    // ========== 1. 调用标准EF生成器（tasktype=1） ==========
    // 这会生成一个标准的边-面碰撞（法线垂直于边切向量）
    CollisionPoint standardEF = generateEF(seed);

    if(standardEF.local_uv1[0] == 0 && standardEF.local_uv1[1] == 0 && 
       standardEF.local_uv2[0] == 0 && standardEF.local_uv2[1] == 0) {
        std::cerr << "Failed to generate valid standard EF collision point." << std::endl;
        return CollisionPoint();
    }

    // 提取生成的patches和UV坐标
    TriQuadBezier patch1 = standardEF.patch1;  // 面片
    TriQuadBezier patch2 = standardEF.patch2;  // 边所在的patch
    Array2r uv1 = standardEF.local_uv1;        // 面片上的UV
    Array2r uv2 = standardEF.local_uv2;        // 边上的UV
    
    std::cout << "Standard EF patches generated" << std::endl;
    std::cout << "uv1 (face): " << uv1.transpose() << std::endl;
    std::cout << "uv2 (edge): " << uv2.transpose() << std::endl;
    
    // ========== 2. 计算面片法线和边的切向量 ==========
    Vector3r faceNormal = standardEF.normal1;
    Vector3r edgeTangent = computeBoundaryTangent(patch2, uv2);
    
    // 计算面片在碰撞点的偏导数
    Vector3r partialU1 = patch1.evaluatePartialU(uv1);
    Vector3r partialV1 = patch1.evaluatePartialV(uv1);
    
    // std::cout << "\n=== Geometry Information ===" << std::endl;
    // std::cout << "Face normal: " << faceNormal.transpose() << std::endl;
    // std::cout << "Edge tangent: " << edgeTangent.transpose() << std::endl;
    // std::cout << "Face partialU: " << partialU1.transpose() << std::endl;
    // std::cout << "Face partialV: " << partialV1.transpose() << std::endl;
    // std::cout << "Tangent · Normal: " << (double)edgeTangent.dot(faceNormal) 
    //           << " (should be ~0)" << std::endl;
    
    // ========== 3. 验证初始接触状态 ==========    
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational initialDistance = (collisionPoint1 - collisionPoint2).norm();
    
    // std::cout << "\nInitial collision state:" << std::endl;
    // std::cout << "Face point: " << collisionPoint1.transpose() << std::endl;
    // std::cout << "Edge point: " << collisionPoint2.transpose() << std::endl;
    // std::cout << "Initial distance: " << (double)initialDistance << std::endl;
    
    if (initialDistance > Rational("1/100")) {
        std::cerr << "Standard EF generation failed: initial distance too large (" 
                  << (double)initialDistance << ")" << std::endl;
        return CollisionPoint();
    }
    
    // ========== 4. 使用面法线作为分离方向（不归一化） ==========
    // 沿着面法线方向移动边所在的patch，使其与面片分离
    // 不进行归一化，直接使用原始法线方向，大小通过gap参数控制
    
    if (faceNormal.norm() == Rational(0)) {
        std::cerr << "Error: face normal is zero!" << std::endl;
        return CollisionPoint();
    }
    
    std::cout << "\n=== Applying Separation ===" << std::endl;
    std::cout << "Gap parameter: " << (double)gap << std::endl;
    std::cout << "Separation direction (face normal, not normalized): " << faceNormal.transpose() << std::endl;
    std::cout << "Face normal magnitude: " << (double)faceNormal.norm() << std::endl;
    
    double normNormal = (double)faceNormal.norm();
    double log2norm = std::log2(normNormal);
    int n = static_cast<int>(std::round(log2norm));
    Rational scale = Rational(1) / Rational(1 << n);

    // 计算分离偏移量（沿面法线方向，不归一化）
    Vector3r separationOffset = gap * faceNormal * scale;
    std::cout << "Separation offset: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation offset magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // 移动边所在的patch（patch2）
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] + separationOffset;
    }
    
    // ========== 5. 验证分离结果 ==========
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualGap = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "\nAfter separation:" << std::endl;
    std::cout << "Face point: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Edge point: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Actual gap: " << (double)actualGap << std::endl;
    std::cout << "Gap error: " << (double)(actualGap - gap) << std::endl;
    
    // ========== 6. 生成擦肩而过的速度 ==========
    // 面片速度：在切平面内（垂直于法线）
    Vector3r vel1 = generateVelocityPerpendicularToNormal(engine, partialU1, partialV1);
    // Vector3r vel2 = generateVelocityPerpendicularToNormal(engine, partialU2, partialV2);
    
    // 边的速度：沿边的切向量方向（平行于边，不归一化）
    // 使用随机系数控制速度大小和方向
    std::uniform_real_distribution<float> coefDist(-5.0, 5.0);
    Rational edgeSpeedCoef = Rational(coefDist(engine));
    
    // 确保系数不为0
    while (edgeSpeedCoef == Rational(0)) {
        edgeSpeedCoef = Rational(coefDist(engine));
    }
    
    // 直接使用切向量乘以系数，不归一化
    Vector3r vel2 = edgeSpeedCoef * edgeTangent;
    
    // std::cout << "\n=== Velocity Information ===" << std::endl;
    // std::cout << "Face velocity: " << vel1.transpose() << std::endl;
    // std::cout << "Face velocity magnitude: " << (double)vel1.norm() << std::endl;
    // std::cout << "Edge tangent: " << edgeTangent.transpose() << std::endl;
    // std::cout << "Edge tangent magnitude: " << (double)edgeTangent.norm() << std::endl;
    // std::cout << "Edge speed coefficient: " << (double)edgeSpeedCoef << std::endl;
    // std::cout << "Edge velocity: " << vel2.transpose() << std::endl;
    // std::cout << "Edge velocity magnitude: " << (double)vel2.norm() << std::endl;
    // std::cout << "Face vel · normal: " << (double)vel1.dot(faceNormal) 
    //           << " (should be ~0)" << std::endl;
    // std::cout << "Edge vel · tangent / ||tangent||^2: " 
    //           << (double)(vel2.dot(edgeTangent) / edgeTangent.dot(edgeTangent))
    //           << " (should be coefficient)" << std::endl;
    // std::cout << "Edge vel · normal: " << (double)vel2.dot(faceNormal) 
    //           << " (parallel to edge means perpendicular to normal)" << std::endl;
    
    Vector3r relativeVel = vel1 - vel2;
    std::cout << "Relative velocity: " << relativeVel.transpose() << std::endl;
    std::cout << "Relative speed: " << (double)relativeVel.norm() << std::endl;
    std::cout << "Relative vel · normal: " << (double)relativeVel.dot(faceNormal) 
              << " (near-miss characteristic)" << std::endl;
    
    // ========== 7. 设置速度场 ==========
    for(int i = 0; i < 6; i++) {
        patch1.velp[i] = vel1;
        patch2.velp[i] = vel2;
    }

    // std::cout << "patch1 ctrlp: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch1.ctrlp[i].transpose() << std::endl;
    // std::cout << "patch2 ctrlp: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch2.ctrlp[i].transpose() << std::endl; 
    
    // 适应batchProcess folder的设定，2个patch沿速度反向运动1/2s
    for(int i=0; i<6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - vel1 * Rational(1)/Rational(2);
        patch2.ctrlp[i] = patch2.ctrlp[i] - vel2 * Rational(1)/Rational(2);
    }

    // std::cout << "patch1 ctrlp after setting: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch1.ctrlp[i].transpose() << std::endl;
    // std::cout << "patch2 ctrlp after setting: " << std::endl;
    // for(int i=0; i<6; i++)
    //     std::cout << patch2.ctrlp[i].transpose() << std::endl;

    // ========== 8. 局部切分 ==========
    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = edgeface::genLocalParam(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);
    
    std::cout << "\n=== Local Parameters ===" << std::endl;
    std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;
    
    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);
    
    Vector3r localNormal1 = localPatch1.evaluateNormal(local_uv1);
    Vector3r localNormal2 = localPatch2.evaluateNormal(local_uv2);
    
    std::cout << "Local face normal: " << localNormal1.transpose() << std::endl;
    std::cout << "Local edge patch normal: " << localNormal2.transpose() << std::endl;
    
    // ========== 9. 最终验证 ==========
    // Vector3r localPoint1 = localPatch1.evaluatePatchPoint(local_uv1);
    // Vector3r localPoint2 = localPatch2.evaluatePatchPoint(local_uv2);
    // Rational localGap = (localPoint1 - localPoint2).norm();
    
    // std::cout << "\n=== Final Verification ===" << std::endl;
    // std::cout << "Local face point: " << localPoint1.transpose() << std::endl;
    // std::cout << "Local edge point: " << localPoint2.transpose() << std::endl;
    // std::cout << "Local gap: " << (double)localGap << std::endl;
    
    // Vector3r localEdgeTangent = computeBoundaryTangent(localPatch2, local_uv2);
    // std::cout << "Local edge tangent: " << localEdgeTangent.transpose() << std::endl;
    // std::cout << "Local tangent · normal: " << (double)localEdgeTangent.dot(localNormal1) 
    //           << " (should be ~0)" << std::endl;
    
    // ========== 10. 构建CollisionPoint ==========
    CollisionPoint cp = {
        patch1,
        patch2,
        uv1,
        uv2,
        local_uv1,
        local_uv2,
        localNormal1,
        localNormal2,
        vel1,
        vel2
    };
    
    std::cout << "\n=== Near-Miss EF Generation Summary ===" << std::endl;
    std::cout << "Configuration: Edge-Face near-miss" << std::endl;
    std::cout << "Target gap: " << (double)gap << std::endl;
    std::cout << "Achieved gap: " << (double)actualGap << std::endl;
    std::cout << "Face velocity: perpendicular to normal (sliding)" << std::endl;
    std::cout << "Edge velocity: along edge tangent (sliding along edge)" << std::endl;
    std::cout << "Result: Edge slides past face without collision" << std::endl;
    
    return cp;
}

// 生成边-边擦肩而过的情况 (Near-Miss EE)
CollisionPoint generateNearMissEE(unsigned seed, Rational gap)
{
    std::cout << "tasktype: near-miss EE (based on standard EE)" << std::endl;
    std::mt19937_64 engine(seed);
    
    // ========== 1. 调用标准EE生成器（tasktype=2） ==========
    // 这会生成一个标准的边-边碰撞（两条边共面且相交）
    CollisionPoint standardEE = generateEE(seed);

    if(standardEE.local_uv1[0] == 0 && standardEE.local_uv1[1] == 0 && 
       standardEE.local_uv2[0] == 0 && standardEE.local_uv2[1] == 0) {
        std::cerr << "Failed to generate valid standard EE collision point." << std::endl;
        return CollisionPoint();
    }

    // 提取生成的patches和UV坐标
    TriQuadBezier patch1 = standardEE.patch1;  // 第一条边所在的patch
    TriQuadBezier patch2 = standardEE.patch2;  // 第二条边所在的patch
    Array2r uv1 = standardEE.local_uv1;        // 第一条边上的UV
    Array2r uv2 = standardEE.local_uv2;        // 第二条边上的UV
    
    std::cout << "Standard EE patches generated" << std::endl;
    std::cout << "uv1 (edge1): " << uv1.transpose() << std::endl;
    std::cout << "uv2 (edge2): " << uv2.transpose() << std::endl;
    
    // ========== 2. 计算两条边的几何信息 ==========
    Vector3r edge1Tangent = computeBoundaryTangent(patch1, uv1);
    Vector3r edge2Tangent = computeBoundaryTangent(patch2, uv2);
    
    // 计算两条边的公共法线（垂直于两条边）
    Vector3r commonNormal = edge1Tangent.cross(edge2Tangent);
    
    std::cout << "\n=== Geometry Information ===" << std::endl;
    std::cout << "Edge1 tangent: " << edge1Tangent.transpose() << std::endl;
    std::cout << "Edge1 tangent magnitude: " << (double)edge1Tangent.norm() << std::endl;
    std::cout << "Edge2 tangent: " << edge2Tangent.transpose() << std::endl;
    std::cout << "Edge2 tangent magnitude: " << (double)edge2Tangent.norm() << std::endl;
    std::cout << "Common normal (edge1 × edge2): " << commonNormal.transpose() << std::endl;
    std::cout << "Common normal magnitude: " << (double)commonNormal.norm() << std::endl;
    std::cout << "Tangent1 · Tangent2: " << (double)edge1Tangent.dot(edge2Tangent) 
              << " (indicates angle between edges)" << std::endl;
    
    // ========== 3. 验证初始接触状态 ==========
    Vector3r collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational initialDistance = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "\nInitial collision state:" << std::endl;
    std::cout << "Edge1 point: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Edge2 point: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Initial distance: " << (double)initialDistance << std::endl;
    
    if (initialDistance > Rational("1/100")) {
        std::cerr << "Standard EE generation failed: initial distance too large (" 
                  << (double)initialDistance << ")" << std::endl;
        return CollisionPoint();
    }
    
    // ========== 4. 使用公共法线作为分离方向（不归一化） ==========
    // 沿着公共法线方向移动其中一条边，使两条边分离
    
    if (commonNormal.norm() == Rational(0)) {
        std::cerr << "Error: common normal is zero (edges are parallel)!" << std::endl;
        return CollisionPoint();
    }

    Vector3f tmpNormal = commonNormal.cast<float>();

    double normNormal = (double)commonNormal.norm();
    double log2val = std::log2(normNormal);
    int n = static_cast<int>(std::round(log2val));
    Rational scale = Rational(1) / Rational(1 << n);
    
    std::cout << "\n=== Applying Separation ===" << std::endl;
    std::cout << "float normal: " << tmpNormal.transpose() << std::endl;
    std::cout << "Gap parameter: " << (double)gap << std::endl;
    std::cout << "Scale for common normal: " << scale << std::endl;
    std::cout << "Separation direction (common normal, not normalized): " << commonNormal.transpose() << std::endl;
    std::cout << "Common normal magnitude: " << (double)commonNormal.norm() << std::endl;
    
    commonNormal = tmpNormal.cast<Rational>();

    // 计算分离偏移量（沿公共法线方向，不归一化）
    Vector3r separationOffset = gap * commonNormal * scale;
    std::cout << "Separation offset: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation offset magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // 移动第二条边所在的patch（patch2）
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] + separationOffset;
    }
    
    // ========== 5. 验证分离结果 ==========
    collisionPoint1 = patch1.evaluatePatchPoint(uv1);
    collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    Rational actualGap = (collisionPoint1 - collisionPoint2).norm();
    
    std::cout << "\nAfter separation:" << std::endl;
    std::cout << "Edge1 point: " << collisionPoint1.transpose() << std::endl;
    std::cout << "Edge2 point: " << collisionPoint2.transpose() << std::endl;
    std::cout << "Actual gap: " << (double)actualGap << std::endl;
    std::cout << "Gap error: " << (double)(actualGap - separationOffset.norm()) << std::endl;
    
    // ========== 6. 生成擦肩而过的速度 ==========
    // 两条边的速度都沿着各自的切向量方向（沿边滑动）
    // 使用随机系数控制速度大小和方向
    
    std::uniform_real_distribution<float> coefDist(-5.0, 7.0);
    
    // 第一条边的速度
    Rational edge1SpeedCoef = Rational(coefDist(engine));
    while (edge1SpeedCoef == Rational(0)) {
        edge1SpeedCoef = Rational(coefDist(engine));
    }
    Vector3r vel1 = edge1SpeedCoef * edge1Tangent;
    
    // 第二条边的速度
    Rational edge2SpeedCoef = Rational(coefDist(engine));
    while (edge2SpeedCoef == Rational(0)) {
        edge2SpeedCoef = Rational(coefDist(engine));
    }
    Vector3r vel2 = edge2SpeedCoef * edge2Tangent;
    
    std::cout << "\n=== Velocity Information ===" << std::endl;
    std::cout << "Edge1 speed coefficient: " << (double)edge1SpeedCoef << std::endl;
    std::cout << "Edge1 velocity: " << vel1.transpose() << std::endl;
    std::cout << "Edge1 velocity magnitude: " << (double)vel1.norm() << std::endl;
    std::cout << "Edge2 speed coefficient: " << (double)edge2SpeedCoef << std::endl;
    std::cout << "Edge2 velocity: " << vel2.transpose() << std::endl;
    std::cout << "Edge2 velocity magnitude: " << (double)vel2.norm() << std::endl;
    
    std::cout << "\nVelocity verification:" << std::endl;
    std::cout << "vel1 · tangent1 / ||tangent1||^2: " 
              << (double)(vel1.dot(edge1Tangent) / edge1Tangent.dot(edge1Tangent))
              << " (should be edge1SpeedCoef)" << std::endl;
    std::cout << "vel2 · tangent2 / ||tangent2||^2: " 
              << (double)(vel2.dot(edge2Tangent) / edge2Tangent.dot(edge2Tangent))
              << " (should be edge2SpeedCoef)" << std::endl;
    std::cout << "vel1 · commonNormal: " << (double)vel1.dot(commonNormal) 
              << " (should be ~0)" << std::endl;
    std::cout << "vel2 · commonNormal: " << (double)vel2.dot(commonNormal) 
              << " (should be ~0)" << std::endl;
    
    Vector3r relativeVel = vel1 - vel2;
    std::cout << "\nRelative velocity: " << relativeVel.transpose() << std::endl;
    std::cout << "Relative speed: " << (double)relativeVel.norm() << std::endl;
    std::cout << "Relative vel · commonNormal: " << (double)relativeVel.dot(commonNormal) 
              << " (should be ~0, near-miss characteristic)" << std::endl;

    // ========== 7. 设置速度场 ==========
    for(int i = 0; i < 6; i++) {
        patch1.velp[i] = vel1;
        patch2.velp[i] = vel2;
    }
    
    // 适应batchProcess folder的设定，2个patch沿速度反向运动1/2s
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - vel1 * Rational("1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - vel2 * Rational("1/2");
    }

    std::cout << "patch1 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.ctrlp[i].transpose() << std::endl;
    std::cout << "patch2 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "patch1 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.velp[i].transpose() << std::endl;
    std::cout << "patch2 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.velp[i].transpose() << std::endl;

    // ========== 8. 局部切分 ==========
    TriParamBound bound1;// = edgeface::genLocalParam(uv1);
    TriParamBound bound2;// = edgeface::genLocalParam(uv2);
    Array2r local_uv1;// = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2;// = computeLocalUV(BaryCoord(uv2), bound2);
    
    // std::cout << "\n=== Local Parameters ===" << std::endl;
    // std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    // std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;
    
    TriQuadBezier localPatch1;// = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2;// = patch2.divideBezierPatch(bound2);
    
    Vector3r localNormal1;// = localPatch1.evaluateNormal(local_uv1);
    Vector3r localNormal2;// = localPatch2.evaluateNormal(local_uv2);
    
    // std::cout << "Local patch1 normal: " << localNormal1.transpose() << std::endl;
    // std::cout << "Local patch2 normal: " << localNormal2.transpose() << std::endl;
    
    // ========== 9. 最终验证 ==========
    // Vector3r localPoint1 = localPatch1.evaluatePatchPoint(local_uv1);
    // Vector3r localPoint2 = localPatch2.evaluatePatchPoint(local_uv2);
    // Rational localGap = (localPoint1 - localPoint2).norm();
    
    // std::cout << "\n=== Final Verification ===" << std::endl;
    // std::cout << "Local edge1 point: " << localPoint1.transpose() << std::endl;
    // std::cout << "Local edge2 point: " << localPoint2.transpose() << std::endl;
    // std::cout << "Local gap: " << (double)localGap << std::endl;
    
    // Vector3r localEdge1Tangent = computeBoundaryTangent(localPatch1, local_uv1);
    // Vector3r localEdge2Tangent = computeBoundaryTangent(localPatch2, local_uv2);
    // Vector3r localCommonNormal = localEdge1Tangent.cross(localEdge2Tangent);
    
    // std::cout << "Local edge1 tangent: " << localEdge1Tangent.transpose() << std::endl;
    // std::cout << "Local edge2 tangent: " << localEdge2Tangent.transpose() << std::endl;
    // std::cout << "Local common normal: " << localCommonNormal.transpose() << std::endl;
    
    // ========== 10. 构建CollisionPoint ==========
    CollisionPoint cp = {
        patch1,
        patch2,
        uv1,
        uv2,
        local_uv1,
        local_uv2,
        localNormal1,
        localNormal2,
        vel1,
        vel2
    };
    
    std::cout << "\n=== Near-Miss EE Generation Summary ===" << std::endl;
    std::cout << "Configuration: Edge-Edge near-miss" << std::endl;
    std::cout << "Target gap parameter: " << (double)gap << std::endl;
    std::cout << "Achieved gap: " << (double)actualGap << std::endl;
    std::cout << "Edge1 velocity: along edge1 tangent (sliding along edge1)" << std::endl;
    std::cout << "Edge2 velocity: along edge2 tangent (sliding along edge2)" << std::endl;
    std::cout << "Result: Two edges slide past each other without collision" << std::endl;
    
    return cp;
}

// 生成点-面擦肩而过的情况 (Near-Miss VF)
CollisionPoint generateNearMissVF(unsigned seed, Rational gap)
{
    std::cout << "tasktype: near-miss VF (based on standard VF)" << std::endl;
    std::mt19937_64 engine(seed);
    
    // ========== 1. 调用标准VF生成器（tasktype=3） ==========
    // 这会生成一个标准的点-面碰撞（点在面上）
    CollisionPoint standardVF = generateVF(seed);

    if(standardVF.local_uv1[0] == 0 && standardVF.local_uv1[1] == 0 && 
       standardVF.local_uv2[0] == 0 && standardVF.local_uv2[1] == 0) {
        std::cerr << "Failed to generate valid standard VF collision point." << std::endl;
        return CollisionPoint();
    }

    // 提取生成的patches和UV坐标
    TriQuadBezier patch1 = standardVF.patch1;  // 面片
    TriQuadBezier patch2 = standardVF.patch2;  // 点所在的patch（点是patch的顶点）
    Array2r uv1 = standardVF.local_uv1;        // 面片上的UV坐标
    Array2r uv2 = standardVF.local_uv2;        // 点的UV坐标（应该是顶点：(0,0), (1,0), 或 (0,1)）
    
    std::cout << "Standard VF patches generated" << std::endl;
    std::cout << "uv1 (face): " << uv1.transpose() << std::endl;
    std::cout << "uv2 (vertex): " << uv2.transpose() << std::endl;
    
    // ========== 2. 计算面片的几何信息 ==========
    Vector3r faceNormal = standardVF.normal1;
    Vector3r partialU1 = patch1.evaluatePartialU(uv1);
    Vector3r partialV1 = patch1.evaluatePartialV(uv1);
    
    std::cout << "\n=== Geometry Information ===" << std::endl;
    std::cout << "Face normal: " << faceNormal.transpose() << std::endl;
    std::cout << "Face normal magnitude: " << (double)faceNormal.norm() << std::endl;
    std::cout << "Face partialU: " << partialU1.transpose() << std::endl;
    std::cout << "Face partialV: " << partialV1.transpose() << std::endl;
    
    // ========== 3. 验证初始接触状态 ==========
    Vector3r facePoint = patch1.evaluatePatchPoint(uv1);
    Vector3r vertexPoint = patch2.evaluatePatchPoint(uv2);
    Rational initialDistance = (facePoint - vertexPoint).norm();
    
    std::cout << "\nInitial collision state:" << std::endl;
    std::cout << "Face point: " << facePoint.transpose() << std::endl;
    std::cout << "Vertex point: " << vertexPoint.transpose() << std::endl;
    std::cout << "Initial distance: " << (double)initialDistance << std::endl;
    
    if (initialDistance > Rational("1/100")) {
        std::cerr << "Standard VF generation failed: initial distance too large (" 
                  << (double)initialDistance << ")" << std::endl;
        return CollisionPoint();
    }
    
    // ========== 4. 使用面法线作为分离方向（不归一化） ==========
    // 沿着面法线方向移动点所在的patch，使点与面片分离
    
    if (faceNormal.norm() == Rational(0)) {
        std::cerr << "Error: face normal is zero!" << std::endl;
        return CollisionPoint();
    }
    
    double normNormal = (double)faceNormal.norm();
    double log2val = std::log2(normNormal);
    int n = static_cast<int>(std::round(log2val));
    Rational scale = Rational(1) / Rational(1 << n);

    Vector3f tmpNormal = faceNormal.cast<float>();

    std::cout << "\n=== Applying Separation ===" << std::endl;
    std::cout << "Gap parameter: " << (double)gap << std::endl;
    std::cout << "Scale factor: " << (double)scale << std::endl;
    std::cout << "Separation direction (face normal, not normalized): " << faceNormal.transpose() << std::endl;
    std::cout << "Face normal magnitude: " << (double)faceNormal.norm() << std::endl;

    faceNormal = tmpNormal.cast<Rational>();    
    
    // 计算分离偏移量（沿面法线方向，不归一化）
    Vector3r separationOffset = gap * faceNormal * scale;
    std::cout << "Separation offset: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation offset magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // 移动点所在的patch（patch2）
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] + separationOffset;
    }
    
    // ========== 5. 验证分离结果 ==========
    facePoint = patch1.evaluatePatchPoint(uv1);
    vertexPoint = patch2.evaluatePatchPoint(uv2);
    Rational actualGap = (facePoint - vertexPoint).norm();
    
    std::cout << "\nAfter separation:" << std::endl;
    std::cout << "Face point: " << facePoint.transpose() << std::endl;
    std::cout << "Vertex point: " << vertexPoint.transpose() << std::endl;
    std::cout << "Actual gap: " << (double)actualGap << std::endl;
    std::cout << "Gap error: " << (double)(actualGap - separationOffset.norm()) << std::endl;
    
    // ========== 6. 生成擦肩而过的速度 ==========
    // 面片速度：在切平面内（垂直于法线）
    // 点的速度：也在面的切平面内（垂直于法线），模拟点在面的切平面上滑动
    
    std::uniform_real_distribution<float> coefDist(-5.0, 7.0);
    
    // 面片的速度：在切平面内随机方向
    Rational coef_u1 = Rational(coefDist(engine));
    Rational coef_v1 = Rational(coefDist(engine));
    
    // 确保不全为0
    while (coef_u1 == Rational(0) && coef_v1 == Rational(0)) {
        coef_u1 = Rational(coefDist(engine));
        coef_v1 = Rational(coefDist(engine));
    }
    
    Vector3r vel1 = coef_u1 * partialU1 + coef_v1 * partialV1;
    
    // 点的速度：也在切平面内，但方向不同
    Rational coef_u2 = Rational(coefDist(engine));
    Rational coef_v2 = Rational(coefDist(engine));
    
    while (coef_u2 == Rational(0) && coef_v2 == Rational(0)) {
        coef_u2 = Rational(coefDist(engine));
        coef_v2 = Rational(coefDist(engine));
    }
    
    Vector3r vel2 = coef_u2 * partialU1 + coef_v2 * partialV1;
    
    std::cout << "\n=== Velocity Information ===" << std::endl;
    std::cout << "Face velocity coefficients: (" << (double)coef_u1 << ", " << (double)coef_v1 << ")" << std::endl;
    std::cout << "Face velocity: " << vel1.transpose() << std::endl;
    std::cout << "Face velocity magnitude: " << (double)vel1.norm() << std::endl;
    std::cout << "Vertex velocity coefficients: (" << (double)coef_u2 << ", " << (double)coef_v2 << ")" << std::endl;
    std::cout << "Vertex velocity: " << vel2.transpose() << std::endl;
    std::cout << "Vertex velocity magnitude: " << (double)vel2.norm() << std::endl;
    
    std::cout << "\nVelocity verification:" << std::endl;
    std::cout << "vel1 · normal: " << (double)vel1.dot(faceNormal) 
              << " (should be ~0, vertex moves in tangent plane)" << std::endl;
    std::cout << "vel2 · normal: " << (double)vel2.dot(faceNormal) 
              << " (should be ~0, face moves in tangent plane)" << std::endl;
    
    Vector3r relativeVel = vel1 - vel2;
    std::cout << "\nRelative velocity: " << relativeVel.transpose() << std::endl;
    std::cout << "Relative speed: " << (double)relativeVel.norm() << std::endl;
    std::cout << "Relative vel · normal: " << (double)relativeVel.dot(faceNormal) 
              << " (should be ~0, near-miss characteristic)" << std::endl;
    
    // ========== 7. 设置速度场 ==========
    for(int i = 0; i < 6; i++) {
        patch1.velp[i] = vel1;
        patch2.velp[i] = vel2;
    }
    
    // 适应batchProcess folder的设定，2个patch沿速度反向运动1/2s
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - vel1 * Rational("1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - vel2 * Rational("1/2");
    }

    std::cout << "patch1 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.ctrlp[i].transpose() << std::endl;
    std::cout << "patch2 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "patch1 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.velp[i].transpose() << std::endl;
    std::cout << "patch2 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.velp[i].transpose() << std::endl;

    // ========== 8. 局部切分 ==========
    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);
    
    std::cout << "\n=== Local Parameters ===" << std::endl;
    std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;
    
    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);
    
    Vector3r localNormal1 = localPatch1.evaluateNormal(local_uv1);
    Vector3r localNormal2 = localPatch2.evaluateNormal(local_uv2);
    
    std::cout << "Local face normal (patch1): " << localNormal1.transpose() << std::endl;
    std::cout << "Local vertex patch normal (patch2): " << localNormal2.transpose() << std::endl;
    
    // ========== 9. 最终验证 ==========
    Vector3r localFacePoint = localPatch1.evaluatePatchPoint(local_uv1);
    Vector3r localVertexPoint = localPatch2.evaluatePatchPoint(local_uv2);
    Rational localGap = (localFacePoint - localVertexPoint).norm();
    
    std::cout << "\n=== Final Verification ===" << std::endl;
    std::cout << "Local face point: " << localFacePoint.transpose() << std::endl;
    std::cout << "Local vertex point: " << localVertexPoint.transpose() << std::endl;
    std::cout << "Local gap: " << (double)localGap << std::endl;
    
    // ========== 10. 构建CollisionPoint ==========
    CollisionPoint cp = {
        patch1,
        patch2,
        uv1,
        uv2,
        local_uv1,
        local_uv2,
        localNormal1,
        localNormal2,
        vel1,
        vel2
    };
    
    std::cout << "\n=== Near-Miss VF Generation Summary ===" << std::endl;
    std::cout << "Configuration: Vertex-Face near-miss" << std::endl;
    std::cout << "Target gap parameter: " << (double)gap << std::endl;
    std::cout << "Achieved gap: " << (double)localGap << std::endl;
    std::cout << "Face velocity (patch1): in tangent plane (sliding)" << std::endl;
    std::cout << "Vertex velocity (patch2): also in tangent plane (sliding)" << std::endl;
    std::cout << "Result: Vertex slides past face without collision" << std::endl;
    
    return cp;
}

// 生成点-边擦肩而过的情况 (Near-Miss VE)
CollisionPoint generateNearMissVE(unsigned seed, Rational gap)
{
    std::cout << "tasktype: near-miss VE (based on standard VE)" << std::endl;
    std::mt19937_64 engine(seed);
    
    // ========== 1. 调用标准VE生成器（tasktype=4） ==========
    // 这会生成一个标准的点-边碰撞（点在边上）
    CollisionPoint standardVE = generateVE(seed);

    if(standardVE.local_uv1[0] == 0 && standardVE.local_uv1[1] == 0 && 
       standardVE.local_uv2[0] == 0 && standardVE.local_uv2[1] == 0) {
        std::cerr << "Failed to generate valid standard VE collision point." << std::endl;
        return CollisionPoint();
    }

    // 提取生成的patches和UV坐标
    TriQuadBezier patch1 = standardVE.patch1;  // 边所在的patch
    TriQuadBezier patch2 = standardVE.patch2;  // 点所在的patch（点是patch的顶点）
    Array2r uv1 = standardVE.local_uv1;        // 边上的UV坐标
    Array2r uv2 = standardVE.local_uv2;        // 点的UV坐标（应该是顶点）
    
    std::cout << "Standard VE patches generated" << std::endl;
    std::cout << "uv1 (edge): " << uv1.transpose() << std::endl;
    std::cout << "uv2 (vertex): " << uv2.transpose() << std::endl;
    
    // ========== 2. 计算边的几何信息 ==========
    Vector3r edgeTangent = computeBoundaryTangent(patch1, uv1);
    Vector3r edgeNormal = patch1.evaluateNormal(uv1);
    
    // 计算垂直于边的方向（在边所在patch的切平面内）
    // 使用边的法线和切向量的叉积
    Vector3r perpToEdge = edgeNormal.cross(edgeTangent);
    
    std::cout << "\n=== Geometry Information ===" << std::endl;
    std::cout << "Edge tangent: " << edgeTangent.transpose() << std::endl;
    std::cout << "Edge tangent magnitude: " << (double)edgeTangent.norm() << std::endl;
    std::cout << "Edge patch normal: " << edgeNormal.transpose() << std::endl;
    std::cout << "Perpendicular to edge: " << perpToEdge.transpose() << std::endl;
    std::cout << "Perpendicular magnitude: " << (double)perpToEdge.norm() << std::endl;
    
    Vector3f tmpPerp = perpToEdge.cast<float>();
    tmpPerp.normalize();

    // ========== 3. 验证初始接触状态 ==========
    Vector3r edgePoint = patch1.evaluatePatchPoint(uv1);
    Vector3r vertexPoint = patch2.evaluatePatchPoint(uv2);
    Rational initialDistance = (edgePoint - vertexPoint).norm();
    
    std::cout << "\nInitial collision state:" << std::endl;
    std::cout << "Edge point: " << edgePoint.transpose() << std::endl;
    std::cout << "Vertex point: " << vertexPoint.transpose() << std::endl;
    std::cout << "Initial distance: " << (double)initialDistance << std::endl;
    
    if (initialDistance > Rational("1/100")) {
        std::cerr << "Standard VE generation failed: initial distance too large (" 
                  << (double)initialDistance << ")" << std::endl;
        return CollisionPoint();
    }
    
    // ========== 4. 使用垂直于边的方向作为分离方向（不归一化） ==========
    // 沿着垂直于边的方向移动点所在的patch，使点与边分离
    
    if (perpToEdge.norm() == Rational(0)) {
        std::cerr << "Error: perpendicular direction is zero!" << std::endl;
        return CollisionPoint();
    }
    
    std::cout << "\n=== Applying Separation ===" << std::endl;
    std::cout << "Gap parameter: " << (double)gap << std::endl;
    std::cout << "Separation direction (perpendicular to edge, not normalized): " << perpToEdge.transpose() << std::endl;
    std::cout << "Separation direction magnitude: " << (double)perpToEdge.norm() << std::endl;

    perpToEdge = tmpPerp.cast<Rational>();

    // 计算分离偏移量（沿垂直于边的方向，不归一化）
    Vector3r separationOffset = gap * perpToEdge;
    std::cout << "Separation offset: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation offset magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // 移动点所在的patch（patch2）
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] + separationOffset;
    }
    
    // ========== 5. 验证分离结果 ==========
    edgePoint = patch1.evaluatePatchPoint(uv1);
    vertexPoint = patch2.evaluatePatchPoint(uv2);
    Rational actualGap = (edgePoint - vertexPoint).norm();
    
    std::cout << "\nAfter separation:" << std::endl;
    std::cout << "Edge point: " << edgePoint.transpose() << std::endl;
    std::cout << "Vertex point: " << vertexPoint.transpose() << std::endl;
    std::cout << "Actual gap: " << (double)actualGap << std::endl;
    std::cout << "Gap error: " << (double)(actualGap - separationOffset.norm()) << std::endl;
    
    // ========== 6. 生成擦肩而过的速度 ==========
    // 边的速度：沿边的切向量方向（沿边滑动）
    // 点的速度：也沿边的切向量方向（沿边滑动），或在垂直于边的平面内
    
    std::uniform_real_distribution<float> coefDist(-5.0, 5.0);
    
    // 边的速度：沿边切向量
    Rational edgeSpeedCoef = Rational(coefDist(engine));
    while (edgeSpeedCoef == Rational(0)) {
        edgeSpeedCoef = Rational(coefDist(engine));
    }
    Vector3r vel1 = edgeSpeedCoef * edgeTangent;
    
    // 点的速度：也沿边切向量（平行运动）
    Rational vertexSpeedCoef = Rational(coefDist(engine));
    while (vertexSpeedCoef == Rational(0)) {
        vertexSpeedCoef = Rational(coefDist(engine));
    }
    Vector3r vel2 = vertexSpeedCoef * edgeTangent;
    
    std::cout << "\n=== Velocity Information ===" << std::endl;
    std::cout << "Edge speed coefficient: " << (double)edgeSpeedCoef << std::endl;
    std::cout << "Edge velocity: " << vel1.transpose() << std::endl;
    std::cout << "Edge velocity magnitude: " << (double)vel1.norm() << std::endl;
    std::cout << "Vertex speed coefficient: " << (double)vertexSpeedCoef << std::endl;
    std::cout << "Vertex velocity: " << vel2.transpose() << std::endl;
    std::cout << "Vertex velocity magnitude: " << (double)vel2.norm() << std::endl;
    
    std::cout << "\nVelocity verification:" << std::endl;
    std::cout << "vel1 · tangent / ||tangent||^2: " 
              << (double)(vel1.dot(edgeTangent) / edgeTangent.dot(edgeTangent))
              << " (should be edgeSpeedCoef)" << std::endl;
    std::cout << "vel2 · tangent / ||tangent||^2: " 
              << (double)(vel2.dot(edgeTangent) / edgeTangent.dot(edgeTangent))
              << " (should be vertexSpeedCoef)" << std::endl;
    std::cout << "vel1 · perpToEdge: " << (double)vel1.dot(perpToEdge) 
              << " (should be ~0, parallel to edge)" << std::endl;
    std::cout << "vel2 · perpToEdge: " << (double)vel2.dot(perpToEdge) 
              << " (should be ~0, parallel to edge)" << std::endl;
    
    Vector3r relativeVel = vel1 - vel2;
    std::cout << "\nRelative velocity: " << relativeVel.transpose() << std::endl;
    std::cout << "Relative speed: " << (double)relativeVel.norm() << std::endl;
    std::cout << "Relative vel · perpToEdge: " << (double)relativeVel.dot(perpToEdge) 
              << " (should be ~0, near-miss characteristic)" << std::endl;
    
    // ========== 7. 设置速度场 ==========
    for(int i = 0; i < 6; i++) {
        patch1.velp[i] = vel1;
        patch2.velp[i] = vel2;
    }
    
    // 适应batchProcess folder的设定，2个patch沿速度反向运动1/2s
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - vel1 * Rational("1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - vel2 * Rational("1/2");
    }

    std::cout << "patch1 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.ctrlp[i].transpose() << std::endl;
    std::cout << "patch2 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "patch1 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.velp[i].transpose() << std::endl;
    std::cout << "patch2 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.velp[i].transpose() << std::endl;

    // ========== 8. 局部切分 ==========
    TriParamBound bound1 = edgeface::genLocalParam(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);
    
    std::cout << "\n=== Local Parameters ===" << std::endl;
    std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;
    
    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);
    
    Vector3r localNormal1 = localPatch1.evaluateNormal(local_uv1);
    Vector3r localNormal2 = localPatch2.evaluateNormal(local_uv2);
    
    std::cout << "Local edge patch normal: " << localNormal1.transpose() << std::endl;
    std::cout << "Local vertex patch normal: " << localNormal2.transpose() << std::endl;
    
    // ========== 9. 最终验证 ==========
    // Vector3r localEdgePoint = localPatch1.evaluatePatchPoint(local_uv1);
    // Vector3r localVertexPoint = localPatch2.evaluatePatchPoint(local_uv2);
    // Rational localGap = (localEdgePoint - localVertexPoint).norm();
    
    // std::cout << "\n=== Final Verification ===" << std::endl;
    // std::cout << "Local edge point: " << localEdgePoint.transpose() << std::endl;
    // std::cout << "Local vertex point: " << localVertexPoint.transpose() << std::endl;
    // std::cout << "Local gap: " << (double)localGap << std::endl;
    
    // Vector3r localEdgeTangent = computeBoundaryTangent(localPatch1, local_uv1);
    // std::cout << "Local edge tangent: " << localEdgeTangent.transpose() << std::endl;
    
    // ========== 10. 构建CollisionPoint ==========
    CollisionPoint cp = {
        patch1,
        patch2,
        uv1,
        uv2,
        local_uv1,
        local_uv2,
        localNormal1,
        localNormal2,
        vel1,
        vel2
    };
    
    std::cout << "\n=== Near-Miss VE Generation Summary ===" << std::endl;
    std::cout << "Configuration: Vertex-Edge near-miss" << std::endl;
    std::cout << "Target gap parameter: " << (double)gap << std::endl;
    std::cout << "Achieved gap: " << (double)actualGap << std::endl;
    std::cout << "Edge velocity (patch1): along edge tangent (sliding along edge)" << std::endl;
    std::cout << "Vertex velocity (patch2): also along edge tangent (sliding along edge)" << std::endl;
    std::cout << "Result: Vertex slides past edge without collision" << std::endl;
    
    return cp;
}

// 生成点-点擦肩而过的情况 (Near-Miss VV)
CollisionPoint generateNearMissVV(unsigned seed, Rational gap)
{
    std::cout << "tasktype: near-miss VV (based on standard VV)" << std::endl;
    std::mt19937_64 engine(seed);
    
    // ========== 1. 调用标准VV生成器（tasktype=5） ==========
    // 这会生成一个标准的点-点碰撞（两个点重合）
    CollisionPoint standardVV = generateVV(seed);

    if(standardVV.local_uv1[0] == 0 && standardVV.local_uv1[1] == 0 && 
       standardVV.local_uv2[0] == 0 && standardVV.local_uv2[1] == 0) {
        std::cerr << "Failed to generate valid standard VV collision point." << std::endl;
        return CollisionPoint();
    }

    // 提取生成的patches和UV坐标
    TriQuadBezier patch1 = standardVV.patch1;  // 第一个点所在的patch
    TriQuadBezier patch2 = standardVV.patch2;  // 第二个点所在的patch
    Array2r uv1 = standardVV.local_uv1;        // 第一个点的UV坐标（顶点）
    Array2r uv2 = standardVV.local_uv2;        // 第二个点的UV坐标（顶点）
    
    std::cout << "Standard VV patches generated" << std::endl;
    std::cout << "uv1 (vertex1): " << uv1.transpose() << std::endl;
    std::cout << "uv2 (vertex2): " << uv2.transpose() << std::endl;
    
    // ========== 2. 计算两个点的信息 ==========
    Vector3r normal1 = patch1.evaluateNormal(uv1);
    Vector3r normal2 = patch2.evaluateNormal(uv2);
    
    // 计算任意分离方向（可以使用normal1，或者normal1和normal2的组合）
    Vector3r separationDir = normal1 + normal2;
    
    // 如果两个法线恰好相反，使用其他方向
    if (separationDir.norm() < Rational("1/1000")) {
        std::cout << "Normals are opposite, using alternative separation direction" << std::endl;
        // 使用垂直于normal1的方向
        Vector3r partialU1 = patch1.evaluatePartialU(uv1);
        if (partialU1.norm() > Rational(0)) {
            separationDir = partialU1;
        } else {
            separationDir = Vector3r(1, 0, 0); // 备用方向
        }
    }
    
    std::cout << "\n=== Geometry Information ===" << std::endl;
    std::cout << "Vertex1 patch normal: " << normal1.transpose() << std::endl;
    std::cout << "Vertex1 normal magnitude: " << (double)normal1.norm() << std::endl;
    std::cout << "Vertex2 patch normal: " << normal2.transpose() << std::endl;
    std::cout << "Vertex2 normal magnitude: " << (double)normal2.norm() << std::endl;
    std::cout << "Separation direction (normal1 + normal2): " << separationDir.transpose() << std::endl;
    std::cout << "Separation direction magnitude: " << (double)separationDir.norm() << std::endl;
    
    // ========== 3. 验证初始接触状态 ==========
    Vector3r vertex1 = patch1.evaluatePatchPoint(uv1);
    Vector3r vertex2 = patch2.evaluatePatchPoint(uv2);
    Rational initialDistance = (vertex1 - vertex2).norm();
    
    std::cout << "\nInitial collision state:" << std::endl;
    std::cout << "Vertex1 point: " << vertex1.transpose() << std::endl;
    std::cout << "Vertex2 point: " << vertex2.transpose() << std::endl;
    std::cout << "Initial distance: " << (double)initialDistance << std::endl;
    
    if (initialDistance > Rational("1/100")) {
        std::cerr << "Standard VV generation failed: initial distance too large (" 
                  << (double)initialDistance << ")" << std::endl;
        return CollisionPoint();
    }
    
    // ========== 4. 使用计算出的方向作为分离方向（不归一化） ==========
    // 移动第二个点所在的patch，使两个点分离
    
    if (separationDir.norm() == Rational(0)) {
        std::cerr << "Error: separation direction is zero!" << std::endl;
        return CollisionPoint();
    }
    
    std::cout << "\n=== Applying Separation ===" << std::endl;
    std::cout << "Gap parameter: " << (double)gap << std::endl;
    std::cout << "Separation direction (not normalized): " << separationDir.transpose() << std::endl;
    std::cout << "Separation direction magnitude: " << (double)separationDir.norm() << std::endl;
    
    Vector3f tmpSeparationDir = separationDir.cast<float>();
    tmpSeparationDir.normalize();
    separationDir = tmpSeparationDir.cast<Rational>(); 

    // 计算分离偏移量（沿分离方向，不归一化）
    Vector3r separationOffset = gap * separationDir;
    std::cout << "Separation offset: " << separationOffset.transpose() << std::endl;
    std::cout << "Separation offset magnitude: " << (double)separationOffset.norm() << std::endl;
    
    // 移动第二个点所在的patch（patch2）
    for(int i = 0; i < 6; i++) {
        patch2.ctrlp[i] = patch2.ctrlp[i] + separationOffset;
    }
    
    // ========== 5. 验证分离结果 ==========
    vertex1 = patch1.evaluatePatchPoint(uv1);
    vertex2 = patch2.evaluatePatchPoint(uv2);
    Rational actualGap = (vertex1 - vertex2).norm();
    
    std::cout << "\nAfter separation:" << std::endl;
    std::cout << "Vertex1 point: " << vertex1.transpose() << std::endl;
    std::cout << "Vertex2 point: " << vertex2.transpose() << std::endl;
    std::cout << "Actual gap: " << (double)actualGap << std::endl;
    std::cout << "Gap error: " << (double)(actualGap - separationOffset.norm()) << std::endl;
    
    // ========== 6. 生成擦肩而过的速度 ==========
    // 两个点的速度都垂直于分离方向（在垂直于分离方向的平面内运动）
    // 这样相对速度也垂直于分离方向，实现擦肩而过
    
    std::uniform_real_distribution<float> coefDist(-5.0, 5.0);
    
    // 构建垂直于separationDir的两个正交向量
    Vector3r tangent1, tangent2;
    
    // 使用patch1的偏导数作为切向量
    Vector3r partialU1 = patch1.evaluatePartialU(uv1);
    Vector3r partialV1 = patch1.evaluatePartialV(uv1);
    
    // 将partialU1投影到垂直于separationDir的平面
    Rational projCoef = partialU1.dot(separationDir) / separationDir.dot(separationDir);
    tangent1 = partialU1 - projCoef * separationDir;
    
    if (tangent1.norm() < Rational("1/1000")) {
        // 如果partialU1几乎平行于separationDir，使用partialV1
        projCoef = partialV1.dot(separationDir) / separationDir.dot(separationDir);
        tangent1 = partialV1 - projCoef * separationDir;
    }
    
    // tangent2 = separationDir × tangent1
    tangent2 = separationDir.cross(tangent1);
    Vector3f tmpTangent2 = tangent2.cast<float>();
    tmpTangent2.normalize();
    tangent2 = tmpTangent2.cast<Rational>();
    Vector3f tmpTangent1 = tangent1.cast<float>();
    tmpTangent1.normalize();
    tangent1 = tmpTangent1.cast<Rational>();
    
    std::cout << "\n=== Tangent Vectors (perpendicular to separation) ===" << std::endl;
    std::cout << "tangent1: " << tangent1.transpose() << std::endl;
    std::cout << "tangent1 magnitude: " << (double)tangent1.norm() << std::endl;
    std::cout << "tangent2: " << tangent2.transpose() << std::endl;
    std::cout << "tangent2 magnitude: " << (double)tangent2.norm() << std::endl;
    std::cout << "tangent1 · separationDir: " << (double)tangent1.dot(separationDir) << " (should be ~0)" << std::endl;
    std::cout << "tangent2 · separationDir: " << (double)tangent2.dot(separationDir) << " (should be ~0)" << std::endl;
    
    // 第一个点的速度：在tangent平面内
    Rational coef1_u = Rational(coefDist(engine));
    Rational coef1_v = Rational(coefDist(engine));
    
    while (coef1_u == Rational(0) && coef1_v == Rational(0)) {
        coef1_u = Rational(coefDist(engine));
        coef1_v = Rational(coefDist(engine));
    }
    
    Vector3r vel1 = coef1_u * tangent1 + coef1_v * tangent2;
    
    // 第二个点的速度：也在tangent平面内，但方向不同
    Rational coef2_u = Rational(coefDist(engine));
    Rational coef2_v = Rational(coefDist(engine));
    
    while (coef2_u == Rational(0) && coef2_v == Rational(0)) {
        coef2_u = Rational(coefDist(engine));
        coef2_v = Rational(coefDist(engine));
    }
    
    Vector3r vel2 = coef2_u * tangent1 + coef2_v * tangent2;
    
    std::cout << "\n=== Velocity Information ===" << std::endl;
    std::cout << "Vertex1 velocity coefficients: (" << (double)coef1_u << ", " << (double)coef1_v << ")" << std::endl;
    std::cout << "Vertex1 velocity: " << vel1.transpose() << std::endl;
    std::cout << "Vertex1 velocity magnitude: " << (double)vel1.norm() << std::endl;
    std::cout << "Vertex2 velocity coefficients: (" << (double)coef2_u << ", " << (double)coef2_v << ")" << std::endl;
    std::cout << "Vertex2 velocity: " << vel2.transpose() << std::endl;
    std::cout << "Vertex2 velocity magnitude: " << (double)vel2.norm() << std::endl;
    
    std::cout << "\nVelocity verification:" << std::endl;
    std::cout << "vel1 · separationDir: " << (double)vel1.dot(separationDir) 
              << " (should be ~0, perpendicular to separation)" << std::endl;
    std::cout << "vel2 · separationDir: " << (double)vel2.dot(separationDir) 
              << " (should be ~0, perpendicular to separation)" << std::endl;
    
    Vector3r relativeVel = vel1 - vel2;
    std::cout << "\nRelative velocity: " << relativeVel.transpose() << std::endl;
    std::cout << "Relative speed: " << (double)relativeVel.norm() << std::endl;
    std::cout << "Relative vel · separationDir: " << (double)relativeVel.dot(separationDir) 
              << " (should be ~0, near-miss characteristic)" << std::endl;
    
    // ========== 7. 设置速度场 ==========
    for(int i = 0; i < 6; i++) {
        patch1.velp[i] = vel1;
        patch2.velp[i] = vel2;
    }

    // 适应batchProcess folder的设定，2个patch沿速度反向运动1/2s
    for(int i = 0; i < 6; i++) {
        patch1.ctrlp[i] = patch1.ctrlp[i] - vel1 * Rational("1/2");
        patch2.ctrlp[i] = patch2.ctrlp[i] - vel2 * Rational("1/2");
    }
    std::cout << "patch1 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.ctrlp[i].transpose() << std::endl;
    std::cout << "patch2 ctrlp after setting: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.ctrlp[i].transpose() << std::endl;
    
    std::cout << "patch1 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch1.velp[i].transpose() << std::endl;
    std::cout << "patch2 velp: " << std::endl;
    for(int i=0; i<6; i++)
        std::cout << patch2.velp[i].transpose() << std::endl;

    // ========== 8. 局部切分 ==========
    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);
    
    std::cout << "\n=== Local Parameters ===" << std::endl;
    std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;
    
    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);
    
    Vector3r localNormal1 = localPatch1.evaluateNormal(local_uv1);
    Vector3r localNormal2 = localPatch2.evaluateNormal(local_uv2);
    
    std::cout << "Local vertex1 patch normal: " << localNormal1.transpose() << std::endl;
    std::cout << "Local vertex2 patch normal: " << localNormal2.transpose() << std::endl;
    
    // ========== 9. 最终验证 ==========
    Vector3r localVertex1 = localPatch1.evaluatePatchPoint(local_uv1);
    Vector3r localVertex2 = localPatch2.evaluatePatchPoint(local_uv2);
    Rational localGap = (localVertex1 - localVertex2).norm();
    
    std::cout << "\n=== Final Verification ===" << std::endl;
    std::cout << "Local vertex1 point: " << localVertex1.transpose() << std::endl;
    std::cout << "Local vertex2 point: " << localVertex2.transpose() << std::endl;
    std::cout << "Local gap: " << (double)localGap << std::endl;
    
    // ========== 10. 构建CollisionPoint ==========
    CollisionPoint cp = {
        patch1,
        patch2,
        uv1,
        uv2,
        local_uv1,
        local_uv2,
        localNormal1,
        localNormal2,
        vel1,
        vel2
    };
    
    std::cout << "\n=== Near-Miss VV Generation Summary ===" << std::endl;
    std::cout << "Configuration: Vertex-Vertex near-miss" << std::endl;
    std::cout << "Target gap parameter: " << (double)gap << std::endl;
    std::cout << "Achieved gap: " << (double)localGap << std::endl;
    std::cout << "Vertex1 velocity: perpendicular to separation direction" << std::endl;
    std::cout << "Vertex2 velocity: also perpendicular to separation direction" << std::endl;
    std::cout << "Result: Two vertices slide past each other without collision" << std::endl;
    
    return cp;
}

CollisionPoint generateSeparatedRandomBezierPatches(unsigned seed, int tasktype) 
{
    if(tasktype==1)
        return generateEF(seed);
    if(tasktype==2)
        return generateEE(seed);
    if(tasktype==3)
        return generateVF(seed);
    if(tasktype==4)
        return generateVE(seed);
    if(tasktype==5)
        return generateVV(seed);
    if(tasktype==6)
        return generateNearMissFF(seed);
    if(tasktype==7)
        return generateNearMissEF(seed);
    if(tasktype==8)
        return generateNearMissEE(seed);
    if(tasktype==9)
        return generateNearMissVF(seed);
    if(tasktype==10)
        return generateNearMissVE(seed);
    if(tasktype==11)
        return generateNearMissVV(seed);
    if(tasktype==12)
        return generateNearHitFF(seed);
    if(tasktype==13)
        return generateNearHitEF(seed);
    if(tasktype==14)
        return generateNearHitEE(seed);
    if(tasktype==15)
        return generateNearHitVF(seed);
    if(tasktype==16)
        return generateNearHitVE(seed);
    if(tasktype==17)
        return generateNearHitVV(seed);
    if(tasktype==18)
        return degenerate::generateSurfaceLineContact(seed);
    if(tasktype==19)
        return degenerate::generatePartiallyColinearControlPoints(seed);
    if(tasktype==20)
        return degenerate::generateFullyCoincidentPatches(seed);
    std::mt19937_64 engine(seed);
    std::uniform_real_distribution<float> dist1(0, 1), dist(-1, 1);//分布范围

    Array2r uv1=generateValidTriangleUV(engine), uv2=generateValidTriangleUV(engine);
   
    auto patch1 = generateRandomPatch(engine, dist, dist, dist);

    auto patch2 = generateRandomPatch(engine, dist, dist, dist);

    // 对齐法线
    // auto patch1Normal = patch1.evaluateNormal(uv1);
    // auto patch2Normal = patch2.evaluateNormal(uv2);
 
    Vector3r patch1Normal = generateIntegerPowerOfTwoNormal(engine);
    Vector3r original_patch1Normal = patch1Normal;
    // Vector3r patch1Normal = Vector3r(-1024, 256, 65536);
    Vector3r patch2Normal = -patch1Normal; 
    Vector3r original_patch2Normal = patch2Normal;

    // patch1Normal = patch1Normal / patch1Normal.norm();
    // patch2Normal = patch2Normal / patch2Normal.norm();

    std::cout << "patch1Normal: " << patch1Normal.transpose() << std::endl;
    std::cout << "patch2Normal: " << patch2Normal.transpose() << std::endl;

    std::cout << "patch1: " << std::endl;
    std::cout << "du: " << patch1.evaluatePartialU(uv1).transpose() << std::endl;
    std::cout << "dv: " << patch1.evaluatePartialV(uv1).transpose() << std::endl;

    std::cout << "patch2: " << std::endl;
    std::cout << "du: " << patch2.evaluatePartialU(uv2).transpose() << std::endl;
    std::cout << "dv: " << patch2.evaluatePartialV(uv2).transpose() << std::endl;



    // rotatePatchToAlign(patch2, uv2, -patch1Normal);
    adjustPatchToOrthogonalDirectSolve(patch1, uv1, patch1Normal);
    std::cout << "after adjust patch1:" << std::endl;
    for (int i = 0; i < 6; i++) 
        std::cout << patch1.ctrlp[i].transpose() << std::endl;

    adjustPatchToOrthogonalDirectSolve(patch2, uv2, patch2Normal);
    std::cout << "after adjust patch2:" << std::endl;
    for (int i = 0; i < 6; i++) 
        std::cout << patch2.ctrlp[i].transpose() << std::endl;

    patch1Normal = patch1.evaluateNormal(uv1);
    patch2Normal = patch2.evaluateNormal(uv2);
    // Vector3r ratio = (patch1Normal.array() / patch2Normal.array()).matrix();

    //再平移
    Vector3r collisionPoint = patch1.evaluatePatchPoint(uv1); 
    Vector3r collisionPoint2 = patch2.evaluatePatchPoint(uv2);
    for(int i=0; i<3; i++)
        collisionPoint2[i].canonicalize();

    Vector3r offset = collisionPoint - collisionPoint2;
    for(int i=0; i<3; i++)
        offset[i].canonicalize();

    for(int i = 0; i < 6; i++) 
        patch2.ctrlp[i] = patch2.ctrlp[i] + offset;
    
    // std::cout << collisionPoint << std::endl;
    // std::cout << patch2.evaluatePatchPoint(uv2) << std::endl;

    //test offset是否会导致精度问题 
    std::cout << "offset: " << offset << std::endl; 
    bool isPrecise = isPreciselyRepresent(offset);


    // pushSurfaceApart(patch2, uv2, -patch1Normal);
    auto [k1_patch1, k2_patch1] = calculatePrincipalCurvatures(patch1, uv1);
    auto [k1_patch2, k2_patch2] = calculatePrincipalCurvatures(patch2, uv2);

    //TODO:反向
    // 生成碰撞速度
    auto [vel1, vel2] = generateCollisionVelocities(engine, patch1Normal, patch2Normal, 0);
    // patch1.velp = generateVelocityField(engine, vel1, uv1, patch1.ctrlp);
    // patch2.velp = generateVelocityField(engine, vel2, uv2, patch2.ctrlp);
    patch1.velp = generateVelocityFieldIndependent(engine, patch1Normal);
    patch2.velp = generateVelocityFieldIndependent(engine, patch2Normal);

    Vector3r actual_vel1 = patch1.evaluatePatchVel(uv1);
    Vector3r actual_vel2 = patch2.evaluatePatchVel(uv2);

    // 切分
    TriParamBound bound1 = generateLocalParamBound(uv1);
    TriParamBound bound2 = generateLocalParamBound(uv2);
    Array2r local_uv1 = computeLocalUV(BaryCoord(uv1), bound1);
    Array2r local_uv2 = computeLocalUV(BaryCoord(uv2), bound2);

    // std::cout<< "bound1: " << bound1.u_min << " " << bound1.u_max << " " << bound1.v_min << " " << bound1.v_max << std::endl;
    // std::cout<< "bound2: " << bound2.u_min << " " << bound2.u_max << " " << bound2.v_min << " " << bound2.v_max << std::endl;

    TriQuadBezier localPatch1 = patch1.divideBezierPatch(bound1);
    TriQuadBezier localPatch2 = patch2.divideBezierPatch(bound2);

    // if (localPatch1.evaluateNormal(local_uv1).dot(patch1Normal) < 0) 
    // {
    //     Vector3r center = localPatch1.evaluatePatchPoint(local_uv1);
    //     for(int i = 0; i < 6; i++) 
    //         localPatch1.ctrlp[i] = center * 2 - localPatch1.ctrlp[i]; 
    //     // localPatch2Normal = localPatch2.evaluateNormal(local_uv2);
    // }

    // if (localPatch2.evaluateNormal(local_uv2).dot(patch2Normal) < 0) 
    // {
    //     Vector3r center = localPatch2.evaluatePatchPoint(local_uv2);
    //     for(int i = 0; i < 6; i++) 
    //         localPatch2.ctrlp[i] = center * 2 - localPatch2.ctrlp[i]; 
    //     // localPatch2Normal = localPatch2.evaluateNormal(local_uv2);
    // }

    localPatch1.velp = patch1.divideBezierPatch(bound1, patch1.velp);
    localPatch2.velp = patch2.divideBezierPatch(bound2, patch2.velp);

    std::cout << "Generated velocity field for local patch1" << std::endl;
    for (int i=0; i<6; i++)
        std::cout << localPatch1.velp[i][0] << " " << localPatch1.velp[i][1] << " " << localPatch1.velp[i][2] << std::endl;
    std::cout << "Generated velocity field for local patch2" << std::endl;
    for (int i=0; i<6; i++)
        std::cout << localPatch2.velp[i][0] << " " << localPatch2.velp[i][1] << " " << localPatch2.velp[i][2] << std::endl;

    CollisionPoint cp = { localPatch1, localPatch2, uv1, uv2, local_uv1, local_uv2, original_patch1Normal, original_patch2Normal, actual_vel1, actual_vel2};
    // cp = { patch1, patch2, uv1, uv2, local_uv1, local_uv2, patch1Normal, patch2Normal, vel1, vel2};
    // return cp;

    // std::cout << "Generated velocity field for patch1" << std::endl;
    std::cout << "Velocity1 at collision point: " << actual_vel1.transpose() << std::endl;
    std::cout << "Dot with normal: " << actual_vel1.dot(patch1Normal) << std::endl;
    // for (int i=0; i<6; i++)
    //     std::cout << patch1.velp[i][0] << " " << patch1.velp[i][1] << " " << patch1.velp[i][2] << std::endl;

    // std::cout << "Generated velocity field for patch2" << std::endl;
    std::cout << "Velocity2 at collision point: " << actual_vel2.transpose() << std::endl;
    std::cout << "Dot with normal: " << actual_vel2.dot(patch2Normal) << std::endl;
    // for (int i=0; i<6; i++)
        // std::cout << patch2.velp[i][0] << " " << patch2.velp[i][1] << " " << patch2.velp[i][2] << std::endl;



    auto [k1_local_patch1, k2_local_patch1] = calculatePrincipalCurvatures(localPatch1, local_uv1);
    auto [k1_local_patch2, k2_local_patch2] = calculatePrincipalCurvatures(localPatch2, local_uv2);
    Rational krms_patch1 = std::sqrt((k1_patch1*k1_patch1 + k2_patch1*k2_patch1) / Rational(2));
    Rational krms_patch2 = std::sqrt((k1_patch2*k1_patch2 + k2_patch2*k2_patch2) / Rational(2));

    std::cout << "Patch1 主曲率: k1 = " << (float)k1_patch1 << ", k2 = " << (float)k2_patch1 << std::endl;
    std::cout << "Patch2 主曲率: k1 = " << (float)k1_patch2 << ", k2 = " << (float)k2_patch2 << std::endl;
    std::cout << "Local Patch1 主曲率: k1 = " << (float)k1_local_patch1 << ", k2 = " << (float)k2_local_patch1 << std::endl;
    std::cout << "Local Patch2 主曲率: k1 = " << (float)k1_local_patch2 << ", k2 = " << (float)k2_local_patch2 << std::endl;
 
    
    std::ofstream culout1("culvature.txt", std::ios::app), culout2("culvaturelocal.txt", std::ios::app);
    // culout1 << (double)patch1.evaluateRMSCurvature(uv1) << " " << (double)patch2.evaluateRMSCurvature(uv2) << std::endl;
    // culout2 << (double)localPatch1.evaluateRMSCurvature(local_uv1) << " " << (double)localPatch2.evaluateRMSCurvature(local_uv2) << std::endl;
    culout1 << (double)krms_patch1 << " " << (double)krms_patch2 << std::endl;

    
    // patch1Normal = localPatch1.evaluateNormal(local_uv1);
    // patch2Normal = localPatch2.evaluateNormal(local_uv2);
    // CollisionPoint cp = {patch1, patch2, uv1, uv2, local_uv1, local_uv2, patch1Normal, -patch1Normal, vel1, vel2};
    // if(!penetrateCheck(cp, ratio[0]>0)) 
    if(!penetrateCheck(cp, 0)) 
    {
        std::cout << "penetrate!" << std::endl;
        return CollisionPoint();
    } 
    

    // std::cout << "Velocity1: " << vel1.transpose() << 
    // " (dot with normal: " << vel1.dot(patch1Normal) << ")" << std::endl;
    // std::cout << "Velocity1: " << (float)vel1.transpose()[0] << " " << (float)vel1.transpose()[1] << " " << (float)vel1.transpose()[2] << std::endl;
    // std::cout << "Velocity2: " << vel2.transpose() << 
    // " (dot with normal: " << vel2.dot(patch2Normal) << ")" << std::endl;
    // std::cout << "Velocity2: " << (float)vel2.transpose()[0] << " " << (float)vel2.transpose()[1] << " " << (float)vel2.transpose()[2] << std::endl;
    std::cout << "uv1: " << uv1.transpose() << std::endl;
    std::cout << "uv2: " << uv2.transpose() << std::endl;
    std::cout << "local_uv1: " << local_uv1.transpose() << std::endl;
    std::cout << "local_uv2: " << local_uv2.transpose() << std::endl;
    std::cout << "collision point: " << collisionPoint.transpose() << std::endl;
    std::cout << "collision point: " << (float)collisionPoint.transpose()[0] << " " << (float)collisionPoint.transpose()[1] << " " << (float)collisionPoint.transpose()[2] << std::endl;
    std::cout << "patch1.normal: " << patch1Normal.transpose()[0] << " " << patch1Normal.transpose()[1] << " " << patch1Normal.transpose()[2] << std::endl;
    std::cout << "patch2.normal: " << patch2Normal.transpose()[0] << " " << patch2Normal.transpose()[1] << " " << patch2Normal.transpose()[2] << std::endl;
    // std::cout << "local patch1.normal: " << (float)localPatch1.evaluateNormal(local_uv1).transpose()[0] << " " << (float)localPatch1.evaluateNormal(local_uv1).transpose()[1] << " " << (float)localPatch1.evaluateNormal(local_uv1).transpose()[2] << std::endl;    
    // std::cout << "local patch2.normal: " << (float)localPatch2.evaluateNormal(local_uv2).transpose()[0] << " " << (float)localPatch2.evaluateNormal(local_uv2).transpose()[1] << " " << (float)localPatch2.evaluateNormal(local_uv2).transpose()[2] << std::endl;
    // std::cout << "patch1.normal: " << cp.normal1.transpose() << std::endl;
    // std::cout << "patch2.normal: " << cp.normal2.transpose() << std::endl;
    std::cout << "Local patch1 normal: " << localPatch1.evaluateNormal(local_uv1).transpose() << std::endl;
    std::cout << "Local patch2 normal: " << localPatch2.evaluateNormal(local_uv2).transpose() << std::endl;
    std::cout << "local patch1.normal: " << (float)localPatch1.evaluateNormal(local_uv1).transpose()[0] << " " << (float)localPatch1.evaluateNormal(local_uv1).transpose()[1] << " " << (float)localPatch1.evaluateNormal(local_uv1).transpose()[2] << std::endl;
    std::cout << "local patch2.normal: " << (float)localPatch2.evaluateNormal(local_uv2).transpose()[0] << " " << (float)localPatch2.evaluateNormal(local_uv2).transpose()[1] << " " << (float)localPatch2.evaluateNormal(local_uv2).transpose()[2] << std::endl;
    // std::cout << "verify normal: " << ratio.transpose()[0] << " " << ratio.transpose()[1] << " " << ratio.transpose()[2] << std::endl;
    // std::cout << "verify normal: " << (float)ratio.transpose()[0] << " " << (float)ratio.transpose()[1] << " " << (float)ratio.transpose()[2] << std::endl;
    std::cout << "local collison point1: " << (float)localPatch1.evaluatePatchPoint(local_uv1).transpose()[0] << " " << (float)localPatch1.evaluatePatchPoint(local_uv1).transpose()[1] << " " << (float)localPatch1.evaluatePatchPoint(local_uv1).transpose()[2] << std::endl;
    std::cout << "local collison point2: " << (float)localPatch2.evaluatePatchPoint(local_uv2).transpose()[0] << " " << (float)localPatch2.evaluatePatchPoint(local_uv2).transpose()[1] << " " << (float)localPatch2.evaluatePatchPoint(local_uv2).transpose()[2] << std::endl;
    std::cout << "local collison point1: " << localPatch1.evaluatePatchPoint(local_uv1).transpose() << std::endl;
    std::cout << "local collison point2: " << localPatch2.evaluatePatchPoint(local_uv2).transpose() << std::endl;
    std::cout << "Result is " << verifyCollision(cp) << std::endl;

    // auto start = std::chrono::high_resolution_clock::now();
    // std::cout << "The result of other collision detection (O) is " << StaticCollisionDetector::detectPatchCollision(localPatch1, localPatch2) << std::endl;   
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = end - start;
    // std::cout << "Time taken for collision detection(O): " << elapsed.count() << " seconds" << std::endl;


    // start = std::chrono::high_resolution_clock::now();
    // std::cout << "The result of other collision detection (N) is " << StaticCollisionDetector::improvedCollisionDetection(localPatch1, localPatch2) << std::endl;   
    // end = std::chrono::high_resolution_clock::now();
    // elapsed = end - start;
    // std::cout << "Time taken for collision detection(N): " << elapsed.count() << " seconds" << std::endl;

    // 检测所有控制点和速度是否能被浮点数精确表示
    auto checkPatchPrecise = [](const TriQuadBezier& patch, const std::string& name) -> bool {
        for (int i = 0; i < 6; i++) {
            if (!isPreciselyRepresent(patch.ctrlp[i])) {
                std::cout << name << " ctrlp[" << i << "] 无法被浮点数精确表示，丢弃该样本" << std::endl;
                return false;
            }
            if (!isPreciselyRepresent(patch.velp[i])) {
                std::cout << name << " velp[" << i << "] 无法被浮点数精确表示，丢弃该样本" << std::endl;
                return false;
            }
        }
        return true;
    };
    if (!checkPatchPrecise(localPatch1, "localPatch1") || !checkPatchPrecise(localPatch2, "localPatch2"))
        return CollisionPoint();

    return cp;
    // return {TriQuadBezier(patch1), TriQuadBezier(patch2)};
}

// 如果您只想要整数2的方幂(不要分数),使用这个简化版本
Vector3r generateIntegerPowerOfTwoNormal(auto& engine) {
    std::uniform_int_distribution<int> powerDist(0, 3); // 生成0到3,对应1,2,4,8
    std::uniform_int_distribution<int> zeroOrNotDist(0, 4); // 20%概率为0
    std::uniform_int_distribution<int> signDist(0, 1);
    
    Vector3r normal;
    for(int i = 0; i < 3; i++) {
        // 20%概率生成0
        if(zeroOrNotDist(engine) == 0) {
            normal[i] = Rational(0);
        } else {
            int power = powerDist(engine);
            int value = (1 << power); // 2^power: 1, 2, 4, 8
            
            // 随机决定正负
            if(signDist(engine) == 0) {
                value = -value;
            }
            
            normal[i] = Rational(value);
        }
    }
    
    // 确保至少有一个维度非零
    if(normal[0] == Rational(0) && normal[1] == Rational(0) && normal[2] == Rational(0)) {
        normal[2] = Rational(1);
    }
    
    return normal;
}


namespace genStandardData
{
    // 从文件名中提取seed值
    inline unsigned extractSeedFromFolderName(const std::string& folderName) 
    {
        size_t seedPos = folderName.find("_seed");
        // if (seedPos == std::string::npos) 
        //     seedPos = folderName.find("_"); // 尝试仅寻找下划线

        if (seedPos != std::string::npos) {
            try {
                // 提取下划线后的数字
                std::string seedStr = folderName.substr(seedPos + (folderName.find("_seed") != std::string::npos ? 5 : 1));
                return static_cast<unsigned>(std::stoul(seedStr));
            } catch (const std::exception& e) {
                std::cerr << "Error extracting seed from folder name '" << folderName 
                          << "': " << e.what() << std::endl;
            }
        }
        return 0; // 默认种子值
    }

    // 处理单个数据文件夹
    inline bool processDataFolder(const std::filesystem::path& folderPath, int taskType, const std::string& datasetFilePath)     {
        std::string folderName = folderPath.filename().string();
        unsigned seed = extractSeedFromFolderName(folderName);
        

        std::string errorLogPath = "process_error.log";
        if (seed == 0) {
            std::cerr << "Invalid seed for folder: " << folderName << std::endl;
            std::ofstream log(errorLogPath, std::ios::app);
            log << "Invalid seed for folder: " << folderName << std::endl;
            return false;
        }

        std::cout << "Processing " << folderName << " with seed: " << seed << std::endl;

        // 根据taskType调用相应的生成函数
        CollisionPoint cp;
        if (taskType == 0) {
            cp = generateSeparatedRandomBezierPatches(seed, taskType);
        } else if (taskType == 1) {
            cp = generateEF(seed);
        } else if (taskType == 2) {
            cp = generateEE(seed);
        } else if (taskType == 3) {
            cp = generateVF(seed);
        } else if (taskType == 4) {
            cp = generateVE(seed);
        } else if (taskType == 5) {
            cp = generateVV(seed);
        }
        else if (taskType == 6) {
            cp = generateNearMissFF(seed);
        }
        else if (taskType == 7) {
            cp = generateNearMissEF(seed);
        }
        else if (taskType == 8) {
            cp = generateNearMissEE(seed);
        }
        else if (taskType == 9) {
            cp = generateNearMissVF(seed);
        }
        else if (taskType == 10) {
            cp = generateNearMissVE(seed);
        }
        else if (taskType == 11) {
            cp = generateNearMissVV(seed);
        }
        else if (taskType == 12) {
            cp = generateNearHitFF(seed);
        }
        else if (taskType == 13) {
            cp = generateNearHitEF(seed);
        }
        else if (taskType == 14) {
            cp = generateNearHitEE(seed);
        }
        else if (taskType == 15) {
            cp = generateNearHitVF(seed);
        }
        else if (taskType == 16) {
            cp = generateNearHitVE(seed);
        }
        else if (taskType == 17) {
            cp = generateNearHitVV(seed);
        }

        else {
            std::cerr << "Unsupported task type: " << taskType << std::endl;
            std::ofstream log(errorLogPath, std::ios::app);
            log << "Unsupported task type: " << taskType << " for folder: " << folderName << std::endl;
            return false;
        }

        if (cp.vel1 == Vector3r::Zero() && cp.vel2 == Vector3r::Zero()) {
            std::cout << "Generate failed for seed: " << seed << std::endl;
            std::ofstream log(errorLogPath, std::ios::app);
            log << "Generate failed for seed: " << seed << " in folder: " << folderName << std::endl;
            return false;
        }

        // 创建结果目录
        std::filesystem::path outputPath = folderPath / "processed";
        std::filesystem::create_directories(outputPath);

        // 保存控制点位置和速度
        saveControlPointsData(cp, outputPath.string(), datasetFilePath, seed, taskType);

        // 保存碰撞验证结果
        // bool collisionResult = verifyCollision(cp);
        // std::ofstream resultFile(outputPath / "collision_result.txt");
        // resultFile << "Collision verification result: " << (collisionResult ? "True" : "False") << std::endl;
        // resultFile.close();

        return true;
    }

    // 保存控制点数据
    inline void saveControlPointsData(const CollisionPoint& cp, const std::string& outputDir, 
                                 const std::string& datasetFilePath, unsigned seed, int tasktype) 
    {
        std::cout<<"start save data"<<std::endl;
        // 创建或打开数据集CSV文件（追加模式）
        std::ofstream csvFile(datasetFilePath, std::ios::app);
        if (!csvFile.is_open()) {
            std::cerr << "Error: Could not open dataset file: " << datasetFilePath << std::endl;
            return;
        }
        
        // GT=1表示真实碰撞(不加偏移), GT=0表示需要加偏移
        int GT = 1;
        if (tasktype >= 6 && tasktype <= 11)
            GT = 0;
        
        // 缩放比例配置（可根据需要调整）
        Rational SCALE_FACTOR = Rational("1/131072");  // 例如: Rational(1, 2) 表示缩小到一半

        // 计算开始位置 = 初始位置 + 速度 × 1秒
        std::array<Vector3r, 6> startPos1, startPos2, endPos1, endPos2;
        for (int i = 0; i < 6; i++) {
            startPos1[i] = cp.patch1.ctrlp[i] + cp.patch1.velp[i] * Rational(1);
            startPos2[i] = cp.patch2.ctrlp[i] + cp.patch2.velp[i] * Rational(1);
            endPos1[i] = cp.patch1.ctrlp[i];
            endPos2[i] = cp.patch2.ctrlp[i];
        }

        // GT=0时需要加上与速度相关的偏移W
        if (GT == 0 && tasktype < 6)
        {
            for (int i = 0; i < 6; i++)
            {
                startPos1[i] = startPos1[i] + cp.patch1.velp[i] * Rational("1/131072");
                startPos2[i] = startPos2[i] + cp.patch2.velp[i] * Rational("1/131072");
                endPos1[i] = endPos1[i] + cp.patch1.velp[i] * Rational("1/131072");
                endPos2[i] = endPos2[i] + cp.patch2.velp[i] * Rational("1/131072");
            }
        }

        // 应用缩放比例
        for (int i = 0; i < 6; i++)
        {
            startPos1[i] = startPos1[i] * SCALE_FACTOR;
            startPos2[i] = startPos2[i] * SCALE_FACTOR;
            endPos1[i] = endPos1[i] * SCALE_FACTOR;
            endPos2[i] = endPos2[i] * SCALE_FACTOR;
        }


        // 将数据写入CSV文件格式: 每行为一个控制点，格式为x_num,x_denom,y_num,y_denom,z_num,z_denom,gt
        // 先写入patch1的起始位置（6个控制点）
        for (int i = 0; i < 6; i++) {
            csvFile << startPos1[i](0).numerator_str() << "," << startPos1[i](0).denominator_str() << ","
                    << startPos1[i](1).numerator_str() << "," << startPos1[i](1).denominator_str() << ","
                    << startPos1[i](2).numerator_str() << "," << startPos1[i](2).denominator_str() << "," << GT << std::endl;
        }

        // 再写入patch2的起始位置（6个控制点）
        for (int i = 0; i < 6; i++) {
            csvFile << startPos2[i](0).numerator_str() << "," << startPos2[i](0).denominator_str() << ","
                    << startPos2[i](1).numerator_str() << "," << startPos2[i](1).denominator_str() << ","
                    << startPos2[i](2).numerator_str() << "," << startPos2[i](2).denominator_str() << "," << GT << std::endl;
        }

        // 写入patch1的结束位置（初始位置，6个控制点）
        for (int i = 0; i < 6; i++) {
            csvFile << endPos1[i](0).numerator_str() << "," << endPos1[i](0).denominator_str() << ","
                    << endPos1[i](1).numerator_str() << "," << endPos1[i](1).denominator_str() << ","
                    << endPos1[i](2).numerator_str() << "," << endPos1[i](2).denominator_str() << "," << GT << std::endl;
        }

        // 写入patch2的结束位置（初始位置，6个控制点）
        for (int i = 0; i < 6; i++) {
            csvFile << endPos2[i](0).numerator_str() << "," << endPos2[i](0).denominator_str() << ","
                    << endPos2[i](1).numerator_str() << "," << endPos2[i](1).denominator_str() << ","
                    << endPos2[i](2).numerator_str() << "," << endPos2[i](2).denominator_str() << "," << GT << std::endl;
        }
        
        csvFile.close();
        
        // 为了向后兼容，仍然保存原始格式的数据
        std::ofstream posFile(outputDir + "/control_points_positions.txt");
        posFile << "# Patch 1 control points positions\n";
        for (int i = 0; i < 6; i++) {
            posFile << i << ": " << cp.patch1.ctrlp[i].transpose() << "\n";
        }
        posFile << "\n# Patch 2 control points positions\n";
        for (int i = 0; i < 6; i++) {
            posFile << i << ": " << cp.patch2.ctrlp[i].transpose() << "\n";
        }
        posFile.close();

        // 保存控制点速度
        std::ofstream velFile(outputDir + "/control_points_velocities.txt");
        velFile << "# Patch 1 control points velocities\n";
        for (int i = 0; i < 6; i++) {
            velFile << i << ": " << cp.patch1.velp[i].transpose() << "\n";
        }
        velFile << "\n# Patch 2 control points velocities\n";
        for (int i = 0; i < 6; i++) {
            velFile << i << ": " << cp.patch2.velp[i].transpose() << "\n";
        }
        velFile.close();
    }

    // 批处理主函数
    void batchProcessFolders(const std::string& baseDir, int taskType) 
    {
        try {
            std::cout << "Scanning directory: " << baseDir << std::endl;
            
            if (!std::filesystem::exists(baseDir)) {
                std::cerr << "Directory does not exist: " << baseDir << std::endl;
                return;
            }
            
            std::string taskTypeStr="";
            if(taskType==0)
                taskTypeStr="FaceFace";
            else if(taskType==1)
                taskTypeStr="EdgeFace";
            else if(taskType==2)
                taskTypeStr="EdgeEdge";
            else if(taskType==3)
                taskTypeStr="VertexFace";
            else if(taskType==4)
                taskTypeStr="VertexEdge";
            else if(taskType==5)
                taskTypeStr="VertexVertex";
            else if(taskType==6)
                taskTypeStr="NearMissFaceFace";
            else if(taskType==7)
                taskTypeStr="NearMissEdgeFace";
            else if(taskType==8)
                taskTypeStr="NearMissEdgeEdge";
            else if(taskType==9)
                taskTypeStr="NearMissVertexFace";
            else if(taskType==10)
                taskTypeStr="NearMissVertexEdge";
            else if(taskType==11)
                taskTypeStr="NearMissVertexVertex";
            else if(taskType==12)
                taskTypeStr="NearHitFaceFace";
            else if(taskType==13)
                taskTypeStr="NearHitEdgeFace";
            else if(taskType==14)
                taskTypeStr="NearHitEdgeEdge";
            else if(taskType==15)
                taskTypeStr="NearHitVertexFace";
            else if(taskType==16)
                taskTypeStr="NearHitVertexEdge";
            else if(taskType==17)
                taskTypeStr="NearHitVertexVertex";
            else if(taskType==18)
                taskTypeStr="SurfaceLineContact";
            else if(taskType==19)
                taskTypeStr="PartiallyColinearControlPoints";
            else if(taskType==20)
                taskTypeStr="FullyCoincidentPatches";
            // else
            //     taskTypeStr="RandomSeparatedPatches";

            // 创建数据集文件路径
            std::string datasetFilePath = baseDir + "/fn_dataset_type" + taskTypeStr + ".csv";
            
            // 如果数据集文件已存在，先备份
            if (std::filesystem::exists(datasetFilePath)) {
                std::string backupPath = datasetFilePath + ".bak";
                std::filesystem::copy_file(datasetFilePath, backupPath, 
                                        std::filesystem::copy_options::overwrite_existing);
                std::cout << "Existing dataset file backed up to: " << backupPath << std::endl;
            }
            
            // 创建一个新的数据集文件并添加标题行
            std::ofstream csvFile(datasetFilePath);
            csvFile << "# Dataset for collision type " << taskType << std::endl;
            csvFile << "# Format: x_numerator,x_denominator,y_numerator,y_denominator,z_numerator,z_denominator,ground_truth" << std::endl;
            csvFile << "# Lines 1-6: Patch1 start positions (t=1)" << std::endl;
            csvFile << "# Lines 7-12: Patch2 start positions (t=1)" << std::endl;
            csvFile << "# Lines 13-18: Patch1 end positions (t=0)" << std::endl;
            csvFile << "# Lines 19-24: Patch2 end positions (t=0)" << std::endl;
            csvFile.close();
            
            // 创建成功 seed 记录文件
            std::string successSeedsPath = baseDir + "/success_seeds_type" + taskTypeStr + ".txt";
            std::ofstream successSeedsFile(successSeedsPath);
            successSeedsFile << "# Successfully processed seeds for task type " << taskType << " (" << taskTypeStr << ")" << std::endl;
            successSeedsFile << "# Each line contains one seed value" << std::endl;
            successSeedsFile.close();
            
            int processedCount = 0;
            int successCount = 0;
            
            // 遍历目录，处理每个子文件夹
            for (const auto& entry : std::filesystem::directory_iterator(baseDir)) {
                if(successCount >= 100) break; // 控制生成数量
                if (entry.is_directory()) {
                    std::string folderName = entry.path().filename().string();
                    std::cout << "Found folder: " << folderName << std::endl;
                    processedCount++;
                    if (processDataFolder(entry.path(), taskType, datasetFilePath)) {
                        successCount++;
                        
                        // 提取并记录成功的 seed
                        unsigned seed = extractSeedFromFolderName(folderName);
                        if (seed != 0) {
                            std::ofstream seedFile(successSeedsPath, std::ios::app);
                            seedFile << seed << std::endl;
                            seedFile.close();
                            std::cout << "Recorded success seed: " << seed << std::endl;
                        }
                    }
                }
            }
            
            
            std::cout << "Batch processing complete." << std::endl;
            std::cout << "Total folders processed: " << processedCount << std::endl;
            std::cout << "Successful processing: " << successCount << std::endl;
            std::cout << "Dataset saved to: " << datasetFilePath << std::endl;
            std::cout << "Success seeds saved to: " << successSeedsPath << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error during batch processing: " << e.what() << std::endl;
        }
    }
}
