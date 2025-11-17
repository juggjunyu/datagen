# pragma once
#include"paramBound.h"

class TriLinearBezier{
public:
	static const int order = 1;
	static const int cntCp = 3;
	// control point order: 001, 100, 010
	std::array<Vector3r, 3> ctrlp;
	TriLinearBezier() {}
	TriLinearBezier(const std::array<Vector3r, 3>& p): ctrlp(p) {}

	Vector3r blossomBilinearBezier(const std::array<Vector3r, 3> &b, const BaryCoord& coord) const {
		// if(std::abs(coord.u + coord.v + coord.w - 1) > 1e-12){
		// 	std::cerr<<coord.u + coord.v + coord.w<<"param coord wrong!\n";
		// 	exit(-1);
		// }
		return coord.w * b[0] + coord.u * b[1] + coord.v * b[2];
	}

	Vector3r evaluatePatchPoint(const Array2r& uv) const {
		BaryCoord coord(uv);
		return blossomBilinearBezier(ctrlp, coord);
	}

	Rational feasibleUpperV(const Rational& u) const { return Rational(1) - u; }
	static Vector3r axisU(const std::array<Vector3r, 3>& pt) {
		return pt[1]-pt[0];
	}
	static Vector3r axisV(const std::array<Vector3r, 3>& pt) {
		return pt[2]-pt[0];
	}

	// control point order: 001, 100, 010
	std::array<Vector3r, 3> divideBezierPatch(const TriParamBound& coords) const {
		std::array<Vector3r, 3> divCp;
		divCp[0] = blossomBilinearBezier(ctrlp, coords.nodes[2]);
		divCp[1] = blossomBilinearBezier(ctrlp, coords.nodes[0]);
		divCp[2] = blossomBilinearBezier(ctrlp, coords.nodes[1]);
		return divCp;
	}
};

class TriQuadBezier{
public:
	static const int order = 2;
	static const int cntCp = 6;
	// control point order: 002, 101, 200, 011, 110, 020
	std::array<Vector3r, 6> ctrlp;
	std::array<Vector3r, 6> velp;
	TriQuadBezier() {}
	TriQuadBezier(const std::array<Vector3r, 6>& p): ctrlp(p) {}

	Vector3r triLerp(const Vector3r& b0, const Vector3r& b1, const Vector3r& b2, const BaryCoord& coord) const {
		// if(std::abs(coord.u + coord.v + coord.w - 1) > 1e-12){
		// 	std::cerr<<coord.u + coord.v + coord.w<<"param coord wrong!\n";
		// 	exit(-1);
		// }
		return coord.w * b0 + coord.u * b1 + coord.v * b2;
	}

	Vector3r blossomBiquadBezier(const std::array<Vector3r, 6>& a, const BaryCoord& coord0, const BaryCoord& coord1) const {
		Vector3r b[3] = { triLerp(a[0], a[1], a[3], coord0), triLerp(a[1], a[2], a[4], coord0), triLerp(a[3], a[4], a[5], coord0) };
		return triLerp(b[0], b[1], b[2], coord1);
	}

	Vector3r evaluatePatchPoint(const Array2r& uv) const {
		BaryCoord coord(uv);
		return blossomBiquadBezier(ctrlp, coord, coord);
	}

	Vector3r evaluatePatchVel(const Array2r& uv) const {
		BaryCoord coord(uv);
		return blossomBiquadBezier(velp, coord, coord);
	}

	Rational feasibleUpperV(const Rational& u) const { return Rational(1) - u; }
	static Vector3r axisU(const std::array<Vector3r, 6>& pt) {
		return pt[2]-pt[0];
	}
	static Vector3r axisV(const std::array<Vector3r, 6>& pt) {
		return pt[5]-pt[0];
	}

	// control point order: 002, 101, 200, 011, 110, 020
	std::array<Vector3r, 6> divideBezierPatch(const TriParamBound& coords) const {
		std::array<Vector3r, 6> divCp;
		divCp[0] = blossomBiquadBezier(ctrlp, coords.nodes[2], coords.nodes[2]);
		divCp[1] = blossomBiquadBezier(ctrlp, coords.nodes[0], coords.nodes[2]);
		divCp[2] = blossomBiquadBezier(ctrlp, coords.nodes[0], coords.nodes[0]);
		divCp[3] = blossomBiquadBezier(ctrlp, coords.nodes[1], coords.nodes[2]);
		divCp[4] = blossomBiquadBezier(ctrlp, coords.nodes[0], coords.nodes[1]);
		divCp[5] = blossomBiquadBezier(ctrlp, coords.nodes[1], coords.nodes[1]);
		return divCp;
	}

	std::array<Vector3r, 6> divideBezierPatch(
        const TriParamBound& coords, 
        const std::array<Vector3r, 6>& velocityField) const 
    {
        std::array<Vector3r, 6> divVel;
        divVel[0] = blossomBiquadBezier(velocityField, coords.nodes[2], coords.nodes[2]);
        divVel[1] = blossomBiquadBezier(velocityField, coords.nodes[0], coords.nodes[2]);
        divVel[2] = blossomBiquadBezier(velocityField, coords.nodes[0], coords.nodes[0]);
        divVel[3] = blossomBiquadBezier(velocityField, coords.nodes[1], coords.nodes[2]);
        divVel[4] = blossomBiquadBezier(velocityField, coords.nodes[0], coords.nodes[1]);
        divVel[5] = blossomBiquadBezier(velocityField, coords.nodes[1], coords.nodes[1]);
        return divVel;
    }

	Vector3r evaluatePartialU(const Array2r& uv) const 
	{
        BaryCoord coord(uv);
        Vector3r b[3] = { 
            triLerp(ctrlp[0], ctrlp[1], ctrlp[3], coord),
            triLerp(ctrlp[1], ctrlp[2], ctrlp[4], coord),
            triLerp(ctrlp[3], ctrlp[4], ctrlp[5], coord)
        };
        return 2 * ((Rational(1)-coord.v) * (b[1]-b[0]) + coord.v * (b[2]-b[1]));
    }

    Vector3r evaluatePartialV(const Array2r& uv) const 
	{
        BaryCoord coord(uv);
        Vector3r b[3] = {
            triLerp(ctrlp[0], ctrlp[1], ctrlp[3], coord),
            triLerp(ctrlp[1], ctrlp[2], ctrlp[4], coord),
            triLerp(ctrlp[3], ctrlp[4], ctrlp[5], coord)
        };
        return 2 * ((Rational(1)-coord.u) * (b[2]-b[0]) + coord.u * (b[2]-b[1]));
    }

	// 计算u方向的二阶偏导数 ∂²S/∂u²
	Vector3r evaluatePartialUU(const Array2r& uv) const 
	{
		// 二阶导数关于u的系数
		const std::vector<Rational> dBuu = {
			Rational(2),      // d²B_002/du² = 2
			-Rational(4),     // d²B_101/du² = -4
			Rational(2),      // d²B_200/du² = 2
			Rational(0),      // d²B_011/du² = 0
			Rational(0),      // d²B_110/du² = 0
			Rational(0)       // d²B_020/du² = 0
		};
		
		// 计算二阶导数
		Vector3r duu = Vector3r::Zero();
		for(int i = 0; i < 6; i++) {
			duu = duu + dBuu[i] * ctrlp[i];
		}
		
		return duu;
	}

	// 计算u,v的混合二阶偏导数 ∂²S/∂u∂v
	Vector3r evaluatePartialUV(const Array2r& uv) const 
	{
		// 混合二阶导数的系数
		const std::vector<Rational> dBuv = {
			Rational(2),      // d²B_002/dudv = 2
			-Rational(2),     // d²B_101/dudv = -2
			Rational(0),      // d²B_200/dudv = 0
			-Rational(2),     // d²B_011/dudv = -2
			Rational(2),      // d²B_110/dudv = 2
			Rational(0)       // d²B_020/dudv = 0
		};
		
		// 计算混合二阶导数
		Vector3r duv = Vector3r::Zero();
		for(int i = 0; i < 6; i++) {
			duv = duv + dBuv[i] * ctrlp[i];
		}
		
		return duv;
	}

	// 计算v方向的二阶偏导数 ∂²S/∂v²
	Vector3r evaluatePartialVV(const Array2r& uv) const 
	{
		// 二阶导数关于v的系数
		const std::vector<Rational> dBvv = {
			Rational(2),      // d²B_002/dv² = 2
			Rational(0),      // d²B_101/dv² = 0
			Rational(0),      // d²B_200/dv² = 0
			-Rational(4),     // d²B_011/dv² = -4
			Rational(0),      // d²B_110/dv² = 0
			Rational(2)       // d²B_020/dv² = 2
		};
		
		// 计算二阶导数
		Vector3r dvv = Vector3r::Zero();
		for(int i = 0; i < 6; i++) {
			dvv = dvv + dBvv[i] * ctrlp[i];
		}
		
		return dvv;
	}

	Rational evaluateRMSCurvature(const Array2r& uv) const
	{
		BaryCoord coord(uv);
		
		// 计算一阶偏导数
		Vector3r du = evaluatePartialU(uv);
		Vector3r dv = evaluatePartialV(uv);
		
		// 计算法向量
		Vector3r normal = du.cross(dv);
		Rational normalLength = normal.norm();
		
		// 如果法向量太小，返回0（几乎是退化的点）
		if (normalLength < Rational("1/1000000")) {
			return Rational(0);
		}
		
		// 归一化法向量
		normal = normal / normalLength;
		
		// 计算二阶导数
		Vector3r duu = evaluatePartialUU(uv);
		Vector3r dvv = evaluatePartialVV(uv);
		Vector3r duv = evaluatePartialUV(uv);
		
		// 计算第一基本形式系数
		Rational E = du.dot(du);
		Rational F = du.dot(dv);
		Rational G = dv.dot(dv);
		
		// 计算第二基本形式系数
		Rational L = normal.dot(duu);
		Rational M = normal.dot(duv);
		Rational N = normal.dot(dvv);
		
		// 计算主曲率
		Rational K = (L*N - M*M) / (E*G - F*F); // 高斯曲率
		Rational H = (E*N - Rational(2)*F*M + G*L) / (Rational(2)*(E*G - F*F)); // 平均曲率
		
		Rational h_squared = H*H;
		Rational k = K;
		Rational rms_squared = h_squared - k;
		
		// return std::sqrt((double)rms_squared);
		return K;
	}

	Vector3r evaluateNormal(const Array2r& uv) const
	{
		Vector3r partialU = evaluatePartialU(uv);
		Vector3r partialV = evaluatePartialV(uv);

		return partialU.cross(partialV);//.normalized();
	}
};

class TriCubicBezier {
public:
	static const int order = 3;
	static const int cntCp = 10;
	// control point order: 003, 102, 201, 300, 012, 111, 210, 021, 120, 030
	std::array<Vector3r, 10> ctrlp;

	TriCubicBezier() {}
	TriCubicBezier(int randSeed){
		if(randSeed<0) std::srand(std::time(nullptr));
		else std::srand(randSeed);
		for (int i = 0; i < 10; i++)
			ctrlp[i] = Vector3r::Random();
	}

	TriCubicBezier(const std::array<Vector3r, 10>& p): ctrlp(p) {}

	Vector3r triLerp(const Vector3r& b0, const Vector3r& b1, const Vector3r& b2, const BaryCoord& coord) const {
		// if(std::abs(coord.u + coord.v + coord.w - 1) > 1e-12){
		// 	std::cerr<<coord.u + coord.v + coord.w<<"param coord wrong!\n";
		// 	exit(-1);
		// }
		return coord.w * b0 + coord.u * b1 + coord.v * b2;
	}

	Vector3r blossomBiquadBezier(const std::array<Vector3r, 6>& a, const BaryCoord& coord0, const BaryCoord& coord1) const {
		Vector3r b[3] = { triLerp(a[0], a[1], a[3], coord0), triLerp(a[1], a[2], a[4], coord0), triLerp(a[3], a[4], a[5], coord0) };
		return triLerp(b[0], b[1], b[2], coord1);
	}

	Vector3r blossomBicubicBezier(const std::array<Vector3r, 10>& p, const BaryCoord& coord0, const BaryCoord& coord1, const BaryCoord& coord2) const {
		std::array<Vector3r,6> a = { triLerp(p[0], p[1], p[4], coord0), triLerp(p[1], p[2], p[5], coord0), triLerp(p[2], p[3], p[6], coord0),
						 triLerp(p[4], p[5], p[7], coord0), triLerp(p[5], p[6], p[8], coord0), triLerp(p[7], p[8], p[9], coord0) };
		return blossomBiquadBezier(a, coord1, coord2);
	}

	Vector3r evaluatePatchPoint(const Array2r& uv) const {
		BaryCoord coord(uv);
		return blossomBicubicBezier(ctrlp, coord, coord, coord);
	}

	Rational feasibleUpperV(const Rational& u) const { return Rational(1) - u; }
	static Vector3r axisU(const std::array<Vector3r, 10>& pt) {
		return pt[3]-pt[0];
	}
	static Vector3r axisV(const std::array<Vector3r, 10>& pt) {
		return pt[9]-pt[0];
	}

	// control point order: 003, 102, 201, 300, 012, 111, 210, 021, 120, 030
	std::array<Vector3r, 10> divideBezierPatch(const TriParamBound& coords) const {
		std::array<Vector3r, 10> divCp;
		divCp[0] = blossomBicubicBezier(ctrlp, coords.nodes[2], coords.nodes[2], coords.nodes[2]);
		divCp[1] = blossomBicubicBezier(ctrlp, coords.nodes[0], coords.nodes[2], coords.nodes[2]);
		divCp[2] = blossomBicubicBezier(ctrlp, coords.nodes[0], coords.nodes[0], coords.nodes[2]);
		divCp[3] = blossomBicubicBezier(ctrlp, coords.nodes[0], coords.nodes[0], coords.nodes[0]);
		divCp[4] = blossomBicubicBezier(ctrlp, coords.nodes[1], coords.nodes[2], coords.nodes[2]);
		divCp[5] = blossomBicubicBezier(ctrlp, coords.nodes[0], coords.nodes[1], coords.nodes[2]);
		divCp[6] = blossomBicubicBezier(ctrlp, coords.nodes[0], coords.nodes[0], coords.nodes[1]);
		divCp[7] = blossomBicubicBezier(ctrlp, coords.nodes[1], coords.nodes[1], coords.nodes[2]);
		divCp[8] = blossomBicubicBezier(ctrlp, coords.nodes[0], coords.nodes[1], coords.nodes[1]);
		divCp[9] = blossomBicubicBezier(ctrlp, coords.nodes[1], coords.nodes[1], coords.nodes[1]);
		return divCp;
	}
};
