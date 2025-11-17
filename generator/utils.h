# pragma once
#include"config.h"

using Rational = rational::Rational;
using Vector3r = Eigen::Matrix<Rational, 3, 1>;
using Array2r = Eigen::Array<rational::Rational, 2, 1>;  

template<typename T>
T max(const T& a, const T& b) {
    return (a > b) ? a : b;
}

template<typename T> 
T min(const T& a, const T& b) {
    return (a < b) ? a : b;
}

template<typename ParamObj1, typename ParamObj2>
void setAxes(const std::array<Vector3r, ParamObj1::cntCp>& ptPos1, 
				const std::array<Vector3r, ParamObj2::cntCp>& ptPos2,
				std::vector<Vector3r>& axes,
				const BoundingBoxType& bb){	
	if(bb==BoundingBoxType::AABB){
		axes = {Vector3r::Unit(0), Vector3r::Unit(1), Vector3r::Unit(2)};
	}
	else if(bb==BoundingBoxType::OBB){
		Vector3r lu1 = ParamObj1::axisU(ptPos1);
		Vector3r lv1tmp = ParamObj1::axisV(ptPos1);
		Vector3r ln1 = lu1.cross(lv1tmp);
		Vector3r lv1 = ln1.cross(lu1);

		// Vector3r lu2 = ParamObj2::axisU(ptPos2);
		// Vector3r lv2tmp = ParamObj2::axisV(ptPos2);
		// Vector3r ln2 = lu2.cross(lv2tmp);
		// Vector3r lv2 = ln2.cross(lu2);

		axes = {lu1,lv1,ln1
			// ,lu2,lv2,ln2, 
			// lu1.cross(lu2), lu1.cross(lv2), lu1.cross(ln2), 
			// lv1.cross(lu2), lv1.cross(lv2), lv1.cross(ln2), 
			// ln1.cross(lu2), ln1.cross(lv2), ln1.cross(ln2)
		};
	}
}

template<typename ParamObj1, typename ParamObj2>
void setAxes(const std::array<Vector3r, ParamObj1::cntCp>& ptPos1, 
				const std::array<Vector3r, ParamObj1::cntCp>& ptVel1, 
				const std::array<Vector3r, ParamObj2::cntCp>& ptPos2,
				const std::array<Vector3r, ParamObj2::cntCp>& ptVel2,
				std::vector<Vector3r>& axes,
				const BoundingBoxType& bb,
				const Rational t = 0){	
	if(bb==BoundingBoxType::AABB){
		axes = {Vector3r::Unit(0), Vector3r::Unit(1), Vector3r::Unit(2)};
	}
	else if(bb==BoundingBoxType::OBB){
		float ft = t;
		// ft = ((int)(ft*100))*0.01;
		// std::cout<<"t: "<<ft<<std::endl;
		// Rational t = Rational(ft);
		Vector3r lu1 = ParamObj1::axisU(ptPos1) + t*ParamObj1::axisU(ptVel1);
		Vector3r lv1tmp = ParamObj1::axisV(ptPos1) + t*ParamObj1::axisV(ptVel1);
		Vector3r ln1 = lu1.cross(lv1tmp);
		Vector3r lv1 = ln1.cross(lu1);

		Vector3r lu2 = ParamObj2::axisU(ptPos2) + t*ParamObj2::axisU(ptVel2);
		Vector3r lv2tmp = ParamObj2::axisV(ptPos2) + t*ParamObj2::axisU(ptVel2);
		Vector3r ln2 = lu2.cross(lv2tmp);
		Vector3r lv2 = ln2.cross(lu2);

		axes = {lu1,lv1,ln1
			,lu2,lv2,ln2, 
			lu1.cross(lu2), lu1.cross(lv2), lu1.cross(ln2), 
			lv1.cross(lu2), lv1.cross(lv2), lv1.cross(ln2), 
			ln1.cross(lu2), ln1.cross(lv2), ln1.cross(ln2)
		};
		// Vector3d lu = ParamObj1::axisU(ptPos2) + initTimeIntv[0]*ParamObj1::axisU(ptVel2);
		// Vector3d lvtmp = ParamObj1::axisV(ptPos2) + initTimeIntv[0]*ParamObj1::axisV(ptVel2);
		// Vector3d ln = lu.cross(lvtmp);
		// Vector3d lv = ln.cross(lu);
		// axes = {lu, lv, ln};
	}
}

template<typename ObjType>
static Rational calcAAExtent(const std::array<Vector3r, ObjType::cntCp>& ptPos) {
	Rational d=0;
	for(int axis=0;axis<3;axis++){
		Rational maxv = ptPos[0][axis], minv=maxv;
		for(int i = 1; i < ObjType::cntCp; i++) {
			maxv=max(maxv, ptPos[i][axis]);
			minv=min(minv, ptPos[i][axis]);
		}
		d=max(d,maxv-minv);
	}
	return d;
}


template<typename ObjType1, typename ObjType2>
static void readinDoFs(std::array<Vector3r, ObjType1::cntCp>& CpPos1, 
					std::array<Vector3r, ObjType1::cntCp>& CpVel1,
					std::array<Vector3r, ObjType2::cntCp>& CpPos2, 
					std::array<Vector3r, ObjType2::cntCp>& CpVel2,
					const std::string& filename="DoFs.dat"){
	std::ifstream readin(filename, std::ios::binary);
	for (int i = 0; i < ObjType1::cntCp; i++)
		for(int k=0;k<3;k++)
			readin.read(reinterpret_cast<char *>(&CpPos1[i][k]), sizeof(Rational));
	for (int i = 0; i < ObjType1::cntCp; i++)
		for(int k=0;k<3;k++)
			readin.read(reinterpret_cast<char *>(&CpVel1[i][k]), sizeof(Rational));
	for (int i = 0; i < ObjType2::cntCp; i++)
		for(int k=0;k<3;k++)
			readin.read(reinterpret_cast<char *>(&CpPos2[i][k]), sizeof(Rational));
	for (int i = 0; i < ObjType2::cntCp; i++)
		for(int k=0;k<3;k++)
			readin.read(reinterpret_cast<char *>(&CpVel2[i][k]), sizeof(Rational));
	readin.close();
}

template<typename ObjType1, typename ObjType2>
static void saveDoFs(const std::array<Vector3r, ObjType1::cntCp>& CpPos1, 
					const std::array<Vector3r, ObjType1::cntCp>& CpVel1,
					const std::array<Vector3r, ObjType2::cntCp>& CpPos2, 
					const std::array<Vector3r, ObjType2::cntCp>& CpVel2,
					const std::string& filename="DoFs.dat"){
	std::ofstream outfile("filename.dat", std::ios::out|std::ios::trunc|std::ios::binary);
	for(int pid = 0; pid < ObjType1::cntCp; ++pid)
		for(int dim=0; dim < 3; ++dim) 
			outfile.write(reinterpret_cast<const char*>(&CpPos1[pid][dim]),sizeof(Rational));
	for(int pid = 0; pid < ObjType1::cntCp; ++pid)
		for(int dim=0; dim < 3; ++dim) 
			outfile.write(reinterpret_cast<const char*>(&CpVel1[pid][dim]),sizeof(Rational));
	for(int pid = 0; pid < ObjType2::cntCp; ++pid)
		for(int dim=0; dim < 3; ++dim) 
			outfile.write(reinterpret_cast<const char*>(&CpPos2[pid][dim]),sizeof(Rational));
	for(int pid = 0; pid < ObjType2::cntCp; ++pid)
		for(int dim=0; dim < 3; ++dim) 
			outfile.write(reinterpret_cast<const char*>(&CpVel2[pid][dim]),sizeof(Rational));
	outfile.close();
}