#pragma once
#include <Eigen/Dense>
#include <rational/rational.hpp>
#include <random>
#include "triBezier.h"
#include <algorithm>
#include <queue>
#include "paramBound.h"
#include <fstream>
#include <chrono>

static std::ofstream debug("debug.txt");   

// using Eigen::Array2d;
// using Eigen::Vector3d;
// using Eigen::Vector4d;
// using Eigen::MatrixXd;
// using Rational = rational::Rational;
// using Vector3r = Eigen::Matrix<Rational, 3, 1>;
// using Array2r = Eigen::Matrix<Rational, 2, 1>;
// enum class BoundingBoxType { AABB, OBB };

class patchpair
{
public:
    TriParamBound bound1;
    TriParamBound bound2;
    int depth;
    double overlapDegree;
    patchpair(const TriParamBound& b1, const TriParamBound& b2, const int &dep=1, const double& overlap=0)
        : bound1(b1), bound2(b2), depth(dep), overlapDegree(overlap){}
    bool operator<(patchpair const &o) const
    {
        return overlapDegree > o.overlapDegree;
    }
    Rational calcWidth() const
    {
        return std::max(bound1.width(), bound2.width());
    }
    bool isPointInBound(const Array2r& uv, bool isPatch1) const 
    {
        BaryCoord point(uv);  
        const TriParamBound& bound = isPatch1 ? bound1 : bound2;
        
        const Array2r p1(bound.nodes[0].u, bound.nodes[0].v);
        const Array2r p2(bound.nodes[1].u, bound.nodes[1].v);
        const Array2r p3(bound.nodes[2].u, bound.nodes[2].v);
        
        // 通过叉积判断点是否在三角形内
        auto cross2d = [](const Array2r& v1, const Array2r& v2) {
            return v1[0] * v2[1] - v1[1] * v2[0];
        };
        
        // 计算点相对于三条边的位置
        Rational cross1 = cross2d(p2 - p1, uv - p1);
        Rational cross2 = cross2d(p3 - p2, uv - p2);
        Rational cross3 = cross2d(p1 - p3, uv - p3);
        
        // 如果点在所有边的同一侧，则在三角形内
        return ((cross1 >= 0 && cross2 >= 0 && cross3 >= 0) ||
                (cross1 <= 0 && cross2 <= 0 && cross3 <= 0));
    }
};

class StaticCollisionDetector
{
public:
    static std::vector<TriParamBound> generateSubdomainsExcludingPoint(Rational radius = Rational(1)/Rational(100)) 
    {
        std::vector<TriParamBound> subdomains;
        
        // 中心点(1/3, 1/3)
        BaryCoord center(Rational(1)/Rational(3), Rational(1)/Rational(3), Rational(1)/Rational(3));
        
        // 定义要排除的小三角形区域的三个顶点
        BaryCoord p1(center.u + radius, center.v, center.w - radius);
        BaryCoord p2(center.u - radius/Rational(2), center.v + radius, center.w - radius/Rational(2));
        BaryCoord p3(center.u - radius/Rational(2), center.v - radius/Rational(2), center.w + radius);
        
        // 原始三角形的三个顶点
        BaryCoord v1(1, 0, 0); // (1,0)
        BaryCoord v2(0, 1, 0); // (0,1)
        BaryCoord v3(0, 0, 1); // (0,0) 在重心坐标中是(0,0,1)
        
        subdomains.push_back(TriParamBound({v1, p1, p3}));
        subdomains.push_back(TriParamBound({v1, p3, v3}));

        subdomains.push_back(TriParamBound({v2, p2, p1}));
        subdomains.push_back(TriParamBound({v2, p1, v1}));
        // 创建子域3：连接v3, p2, p3
        subdomains.push_back(TriParamBound({v3, p3, p2}));
        subdomains.push_back(TriParamBound({v3, p2, v2}));
        
        
        return subdomains;
    }

    // 使用新的分割策略处理碰撞检测的曲面
    static std::vector<TriQuadBezier> divideAndParameterizePatch(const TriQuadBezier& patch) {
        std::vector<TriQuadBezier> subpatches;
        
        // 获取子域
        auto subdomains = generateSubdomainsExcludingPoint();
        
        // 对每个子域应用分割
        for (const auto& subdomain : subdomains) {
            subpatches.push_back(patch.divideBezierPatch(subdomain));
        }
        
        return subpatches;
    }
    static bool improvedCollisionDetection(const TriQuadBezier& patch1, const TriQuadBezier& patch2) {
        // 将两个曲面分割成子曲面，避开(1/3, 1/3)
        auto subpatches1 = divideAndParameterizePatch(patch1);
        auto subpatches2 = divideAndParameterizePatch(patch2);
        
        // 对所有子曲面对进行碰撞检测
        for (const auto& subpatch1 : subpatches1) {
            for (const auto& subpatch2 : subpatches2) {
                if (StaticCollisionDetector::detectPatchCollision(subpatch1, subpatch2)) {   
                    return true;
                }
                // debug << "subpatch1: " << subpatch1.ctrlp[0].transpose() << std::endl;
                // debug << "subpatch2: " << subpatch2.ctrlp[0].transpose() << std::endl;
            }
        }
        
        return false;
    }
    static bool detectPatchCollision(
        const TriQuadBezier& patch1,
        const TriQuadBezier& patch2,
        const BoundingBoxType& bb = BoundingBoxType::OBB, 
        const double deltadist = 1e-5)
    {
        //检测除了centre外是否有其他uv位置的碰撞
        std::priority_queue<patchpair> Q;
        TriParamBound initParam;
        const Array2r centre = Array2r("1/3", "1/3");
        int cnt = 0;
        auto start = std::chrono::high_resolution_clock::now();
    
        auto [collides, overlap] = FFcoltest(patch1.ctrlp, patch2.ctrlp, bb);
        if(collides)
            Q.push(patchpair(initParam, initParam, 1, overlap));
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        debug << "init Time: " << elapsed.count() << std::endl;       

        while(!Q.empty())
        {
            auto const cur = Q.top();
            Q.pop();

            cnt++;
            debug << "dep: " << cur.depth << " width: " << cur.calcWidth()<<std::endl;
            if(cnt==300000)
            {
                end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;
                debug<<"30w: "<<cur.calcWidth()<<" " << elapsed << std::endl;
            }

            if(cur.calcWidth() < deltadist)
            {
                //检测中心点是否在该patch内
                debug<< "patch1: " << cur.bound1.nodes[0].u << " " << cur.bound1.nodes[0].v << std::endl;
                debug<< "patch1: " << cur.bound1.nodes[1].u << " " << cur.bound1.nodes[1].v << std::endl;
                debug<< "patch1: " << cur.bound1.nodes[2].u << " " << cur.bound1.nodes[2].v << std::endl;
                debug<< "patch2: " << cur.bound2.nodes[0].u << " " << cur.bound2.nodes[0].v << std::endl;
                debug<< "patch2: " << cur.bound2.nodes[1].u << " " << cur.bound2.nodes[1].v << std::endl;
                debug<< "patch2: " << cur.bound2.nodes[2].u << " " << cur.bound2.nodes[2].v << std::endl;
                debug<< "check result: patch1: " << cur.isPointInBound(centre, true) << " patch2: " << cur.isPointInBound(centre, false) << std::endl; 
                if((!cur.isPointInBound(centre, true)) && (!cur.isPointInBound(centre, false)))
                {
                    //输出现在的patch的uv
                    return true;
                }
                continue;
            }
            // if(cur.bound1.width() > cur.bound2.width())
            // {
            //     // patch1划分
            //     for(int j=0; j<4; j++)
            //     {
            //         TriParamBound uv1 = cur.bound1.interpSubpatchParam(j);
            //         auto pos1 = patch1.divideBezierPatch(uv1);
            //         auto pos2 = patch2.divideBezierPatch(cur.bound2);
            //         if(FFcoltest(pos1, pos2, bb))
            //         {
            //             // if(j!=3)
            //             //     return true;
            //             Q.push(patchpair(uv1, cur.bound2));
            //         }
            //     }
            // }
            // else
            // {
            //     // patch2划分
            //     for(int k=0; k<4; k++)
            //     {
            //         TriParamBound uv2 = cur.bound2.interpSubpatchParam(k);
            //         auto pos1 = patch1.divideBezierPatch(cur.bound1);
            //         auto pos2 = patch2.divideBezierPatch(uv2);
            //         if(FFcoltest(pos1, pos2, bb))
            //         {
            //             // if(k!=3)
            //             //     return true;
            //             Q.push(patchpair(cur.bound1, uv2));
            //         }
            //     }
            // }
            for (int i = 0; i < 4; i++) 
            {
                TriParamBound uv1(cur.bound1.interpSubpatchParam(i));
                auto pos1 = patch1.divideBezierPatch(cur.bound1);
                for (int j = 0; j < 4; j++) 
                {
                    TriParamBound uv2(cur.bound2.interpSubpatchParam(j));
                    auto pos2 = patch2.divideBezierPatch(uv2);
                    //计时
                    start = std::chrono::high_resolution_clock::now();

                    auto [collides, overlap] = FFcoltest(pos1, pos2, bb);
                    if (collides)
                        Q.push(patchpair(uv1, uv2, cur.depth+1, overlap));
                    end = std::chrono::high_resolution_clock::now();
                    elapsed = end - start;
                    // debug << "Time: " << elapsed.count() << " seconds" << std::endl;
                }
            }
        }
        return false;
    }
    static std::pair<bool, double> FFcoltest(
        const std::array<Vector3r, TriQuadBezier::cntCp>& patch1,
        const std::array<Vector3r, TriQuadBezier::cntCp>& patch2,
        const BoundingBoxType& bb)
    {
        const double deltadist = 1e-5;
        std::vector<Vector3r> axes;
        if(bb == BoundingBoxType::AABB)
            axes = {Vector3r::Unit(0), Vector3r::Unit(1), Vector3r::Unit(2)};
        else if(bb == BoundingBoxType::OBB) 
        {
            Vector3r lu1 = TriQuadBezier::axisU(patch1);
            Vector3r lv1tmp = TriQuadBezier::axisV(patch1);
            Vector3r ln1 = lu1.cross(lv1tmp);
            Vector3r lv1 = ln1.cross(lu1);

            Vector3r lu2 = TriQuadBezier::axisU(patch2);
            Vector3r lv2tmp = TriQuadBezier::axisV(patch2);
            Vector3r ln2 = lu2.cross(lv2tmp);
            Vector3r lv2 = ln2.cross(lu2);

            axes = {lu1,lv1,ln1
                ,lu2,lv2,ln2, 
                lu1.cross(lu2), lu1.cross(lv2), lu1.cross(ln2), 
                lv1.cross(lu2), lv1.cross(lv2), lv1.cross(ln2), 
                ln1.cross(lu2), ln1.cross(lv2), ln1.cross(ln2)
            };
        }

        double overlapVolume = 1;
        Rational inf = 1000000000;
        for(const auto& axis : axes) 
        {
            Rational maxProj1 = -inf;
            Rational minProj1 = inf;
            for(const auto& p : patch1) 
            {
                Rational proj = p.dot(axis);
                maxProj1 = std::max(maxProj1, proj);
                minProj1 = std::min(minProj1, proj);
            }

            Rational maxProj2 = -inf;
            Rational minProj2 = inf;
            for(const auto& p : patch2) 
            {
                Rational proj = p.dot(axis);
                maxProj2 = std::max(maxProj2, proj);
                minProj2 = std::min(minProj2, proj);
            }

            if(maxProj2 < minProj1 || maxProj1 < minProj2)
                return {false, 0};

            double overlap = std::min(maxProj1, maxProj2) - std::max(minProj1, minProj2);
    
            // 重叠长度与该轴最大长度比例
            double length1 = maxProj1 - minProj1;
            double length2 = maxProj2 - minProj2;
            double totalLength = std::max(length1, length2);
            
            if(totalLength > 0)
                overlapVolume *= (overlap / totalLength);
        }
        
        return {true, overlapVolume};  
    }
};