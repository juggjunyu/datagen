# pragma once
#include "utils.h"

extern std::ofstream debug;
using Rational = rational::Rational;
using Vector3r = Eigen::Matrix<Rational, 3, 1>;
using Array2r = Eigen::Array<rational::Rational, 2, 1>;


template<typename ParamObj1, typename ParamObj2, typename ParamBound1, typename ParamBound2>
class SolverTD
{
	public:
		struct Line
		{
			Rational k, b;
			Line():k(0),b(0) {}
			void set(const Rational& _k, const Rational &_b)
			{
				k=_k,b=_b;
			}
			Line(const Rational& k,const Rational& b): k(k), b(b) {}
			bool operator<(const Line &l) const
			{
				return k < l.k || (k == l.k && b > l.b); // 相同斜率的直线中只有截距最大的被留下来
			}
			bool operator==(const Line &l) const
			{
				return k == l.k;
			}
		};

		static Array2r axisCheck(std::vector<Line> lines1, std::vector<Line> lines2,
		                         const Array2r& timeIntv) //求两个线段集的交集，集合1的min曲线与集合2的max曲线的交集
		{
			std::vector<Line> ch1, ch2;
			ch1.clear();
			ch2.clear();


			// for( auto& l:lines1)l.b = l.b + Rational(1e-12)*l.b.abs();
			// for( auto& l:lines2)l.b = l.b - Rational(1e-12)*l.b.abs();
			calcBoundaries(lines1, ch1, true, timeIntv);//获取集合1的max曲线
			calcBoundaries(lines2, ch2, false, timeIntv);//获取集合2的min曲线
			auto ok = boundaryIntersect(ch1, ch2, timeIntv);
			// debug<<"ch1:\n";
			// for(auto& l:ch1)debug<<l.k<<" "<<l.b<<std::endl;
			// debug<<"ch2:\n";
			// for(auto& l:ch2)debug<<l.k<<" "<<l.b<<std::endl;
			// std::cout<<"intv:"<<timeIntv[0]<<" "<<timeIntv[1]<<std::endl;
			// std::cout<<"ok:"<<ok<<std::endl;
			return ok;//求交
		};
		static void calcBoundaries(std::vector<Line>& lines, std::vector<Line>& ch,
		                           const bool getMaxCH, const Array2r& tIntv)
		{
			if(!getMaxCH)std::reverse(lines.begin(),lines.end());
			lines.erase(std::unique(lines.begin(), lines.end()), lines.end());
			ch.clear();
			ch.push_back(lines[0]);
			int alpha = 1;
			while(alpha < lines.size())
			{
				int beta = ch.size()-1;
				while(beta > 0)
				{
					Rational chfp = (ch[beta].k-ch[beta-1].k)*(lines[alpha].b-ch[beta-1].b)
					                -(lines[alpha].k-ch[beta-1].k)*(ch[beta].b-ch[beta-1].b);
					if(chfp>=0)
					{
						ch.pop_back();
						beta--;
					}
					else break;
				}
				if(beta==0)
				{
					Rational chStart = tIntv[0]*(lines[alpha].k-ch[0].k)+(lines[alpha].b-ch[0].b);
					if((getMaxCH&&chStart>=0)||(!getMaxCH&&chStart<=0))
						ch.pop_back();
				}
				if(ch.empty())ch.push_back(lines[alpha]);
				else
				{
					Rational chEnd = tIntv[1]*(lines[alpha].k-ch[beta].k)+(lines[alpha].b-ch[beta].b);
					if((getMaxCH&&chEnd>0)||(!getMaxCH&&chEnd<0))
						ch.push_back(lines[alpha]);
				}
				alpha++;
			}
			if(ch.empty())
			{
				std::cerr<<"empty CH!\n";
				exit(-1);
			}
		}
		static Array2r boundaryIntersect(const std::vector<Line>& ch1, const std::vector<Line>& ch2,
		                                 const Array2r& tIntv)
		{
			int id1=0, id2=0;
			Rational intvL=-1, intvR=-1;
			if(ch1[0].k*tIntv[0]+ch1[0].b<ch2[0].k*tIntv[0]+ch2[0].b)intvL=tIntv[0];//左端点就是交区间的左端点
			else
			{
				while(id1<ch1.size()&&id2<ch2.size())
				{
					if(ch1[id1].k>=ch2[id2].k)
					{
						break;
					}
					Rational hifp1, hifp2;
					if(id1<ch1.size()-1)
						hifp1=(ch1[id1+1].k-ch2[id2].k)*(ch1[id1].b-ch2[id2].b)
						      -(ch1[id1].k-ch2[id2].k)*(ch1[id1+1].b-ch2[id2].b);
					else
						hifp1=tIntv[1]*(ch1[id1].k-ch2[id2].k)+(ch1[id1].b-ch2[id2].b);
					if(id2<ch2.size()-1)
						hifp2=(ch1[id1].k-ch2[id2+1].k)*(ch1[id1].b-ch2[id2].b)
						      -(ch1[id1].k-ch2[id2].k)*(ch1[id1].b-ch2[id2+1].b);
					else
						hifp2=tIntv[1]*(ch1[id1].k-ch2[id2].k)+(ch1[id1].b-ch2[id2].b);
					if(hifp1<0) //讨论双指针的移动情况
					{
						if(hifp2<0)
						{
							intvL = -(ch1[id1].b-ch2[id2].b)/(ch1[id1].k-ch2[id2].k);
							break;
						}
						else id2++;
					}
					else
					{
						id1++;
						if(hifp2<0);
						else id2++;
					}
				}
				if(intvL==-1||intvL>=tIntv[1])return Array2r(-1,-1);
			}

			id1 = ch1.size()-1, id2 = ch2.size()-1;
			if((ch1[id1].k-ch2[id2].k)*tIntv[1]+(ch1[id1].b-ch2[id2].b)<0)intvR=tIntv[1];
			else
			{
				while(id1>=0&&id2>=0)
				{
					if(ch1[id1].k<=ch2[id2].k)
					{
						return Array2r(-1,-1);
					}
					Rational hifp1, hifp2;
					if(id1>0)
						hifp1=(ch1[id1].k-ch2[id2].k)*(ch1[id1-1].b-ch2[id2].b)
						      -(ch1[id1-1].k-ch2[id2].k)*(ch1[id1].b-ch2[id2].b);
					else
						hifp1=tIntv[0]*(ch1[id1].k-ch2[id2].k)+(ch1[id1].b-ch2[id2].b);
					if(id2>0)
						hifp2=(ch1[id1].k-ch2[id2].k)*(ch1[id1].b-ch2[id2-1].b)
						      -(ch1[id1].k-ch2[id2-1].k)*(ch1[id1].b-ch2[id2].b);
					else
						hifp2=tIntv[0]*(ch1[id1].k-ch2[id2].k)+(ch1[id1].b-ch2[id2].b);
					if(hifp1<0)
					{
						if(hifp2<0)
						{
							intvR = -(ch1[id1].b-ch2[id2].b)/(ch1[id1].k-ch2[id2].k);
							break;
						}
						else id2--;
					}
					else
					{
						id1--;
						if(hifp2<0);
						else id2--;
					}
				}
				if(intvR==-1)
				{
					std::cerr<<"intvL done but no intvR?\n";
					exit(-1);
				}
				if(intvR<=intvL)
					return Array2r(-1,-1);
			}

			intvL = std::max(intvL, tIntv[0]);
			intvR = std::min(intvR, tIntv[1]);
			if(intvL>intvR||intvL<tIntv[0]||intvR>tIntv[1])
			{
				return Array2r(-1,-1);
			}
			else return Array2r(intvL,intvR);
		}

		static bool intvMerge(std::vector<Array2r>& feasibleIntvs, Array2r& colTime, const Array2r& initTimeIntv)
		{
			if (feasibleIntvs.size()==0)
			{
				colTime = initTimeIntv;
				return true;
			}
			Rational minT = initTimeIntv[0], maxT = initTimeIntv[1];
			std::sort(feasibleIntvs.begin(), feasibleIntvs.end(),
			          [](const Array2r& intv1, const Array2r& intv2)
			{
				return (intv1(0)<intv2(0));
			});
			if(feasibleIntvs[0](0)<=initTimeIntv[0])
			{
				minT = std::max(minT,feasibleIntvs[0](1));
				for(int i=1; i<feasibleIntvs.size(); i++)
					if(feasibleIntvs[i](0)<=minT)
						minT=std::max(minT, feasibleIntvs[i](1));
					else break;
			}
			if(minT > maxT)
			{
				colTime = Array2r(-1,-1);
				return false;
			}

			std::sort(feasibleIntvs.begin(), feasibleIntvs.end(),
			          [](const Array2r& intv1, const Array2r& intv2)
			{
				return (intv1(1)>intv2(1));
			});
			if(feasibleIntvs[0](1)>=initTimeIntv[1])
			{
				maxT = std::min(maxT, feasibleIntvs[0](0));
				for(int i=1; i<feasibleIntvs.size(); i++)
					if(feasibleIntvs[i](1)>=maxT)
						maxT=std::min(maxT, feasibleIntvs[i](0));
					else break;
			}
			
			if(initTimeIntv[0] >= maxT)
			{
				colTime = Array2r(-1,-1);
				return false;
			}
			if(minT >= maxT)
			{
				colTime = Array2r(maxT, minT);
			}
			else colTime = Array2r(minT, maxT);
			// debug << "maxT: " << (double)maxT << " minT: "<< (double)minT << std::endl;
			// debug << "colTime: " << (double)colTime[0] << " " << (double)colTime[1] << std::endl;
			
			return true;
		}

	public:
		static bool primitiveCheck(const ParamObj1 &CpPos1, const ParamObj1 &CpVel1,
		                           const ParamObj2 &CpPos2, const ParamObj2 &CpVel2,
		                           const ParamBound1 &divUvB1, const ParamBound2 &divUvB2,
		                           Array2r& colTime,
		                           const BoundingBoxType& bb,
		                           const Array2r& initTimeIntv = Array2r(0,DeltaT))
		{

			// debug << "primitive check" << std::endl;

			auto ptPos1 = CpPos1.divideBezierPatch(divUvB1);
			auto ptVel1 = CpVel1.divideBezierPatch(divUvB1);
			auto ptPos2 = CpPos2.divideBezierPatch(divUvB2);
			auto ptVel2 = CpVel2.divideBezierPatch(divUvB2);

			// debug << "ptPos1: " << ptPos1[0] << " " << ptPos1[1] << " " << ptPos1[2] << std::endl;
			// debug << "ptVel1: " << ptVel1[0] << " " << ptVel1[1] << " " << ptVel1[2] << std::endl;
			// debug << "ptPos2: " << ptPos2[0] << " " << ptPos2[1] << " " << ptPos2[2] << std::endl;
			// debug << "ptVel2: " << ptVel2[0] << " " << ptVel2[1] << " " << ptVel2[2] << std::endl;

			// debug << "pt Done."<<std::endl;

			// Enlarge the time interval by a small margin so that the end points are correctly treated
			const Rational initeps = Rational(0.00000000001);
			// Array2r timeIntv(initTimeIntv[0]-initeps, initTimeIntv[1]+initeps);
			double lowerTBound = initTimeIntv[0] - initeps, upperTBound = initTimeIntv[1] + initeps;
			// lowerTBound = ((int)((float)(lowerTBound*1000000)))*0.000001;
			// upperTBound = ((int)((float)(upperTBound*1000000)))*0.000001;
			lowerTBound = std::max(0.0, lowerTBound);
			upperTBound = std::min(upperTBound, 1.0);
			Array2r timeIntv(lowerTBound, upperTBound);
			// Array2r timeIntv(initTimeIntv[0], initTimeIntv[1]);
			// debug <<"time Interval: "<<std::endl;
			// debug << timeIntv[0] << " " << timeIntv[1] << std::endl;
			// debug << initTimeIntv[0] << " " << initTimeIntv[1] << std::endl;

			std::vector<Vector3r> axes;
			setAxes<ParamObj1, ParamObj2>(ptPos1, ptVel1, ptPos2, ptVel2, axes, bb, timeIntv[0]);

			std::vector<Array2r> feasibleIntvs;
			feasibleIntvs.clear();

			const Rational eps = Rational(0.000000000001);
			// debug << "eps: " << eps << std::endl;

			// auto AxisCheck=[&](std::vector<Line> lines1, std::vector<Line> lines2)
			// {
			// 	std::vector<Line> ch1, ch2;
			// 	ch1.clear();
			// 	ch2.clear();
			// 	for( auto& l:lines1)l.b = l.b + eps * l.b.abs();
			// 	for( auto& l:lines2)l.b = l.b - eps * l.b.abs();
			// 	// debug << "lines1: " << std::endl;
			// 	// for(auto &l:lines1){
			// 	// 	debug << l.k << " " << l.b << std::endl;
			// 	// }
			// 	// debug << "lines2: " << std::endl;
			// 	// for(auto &l:lines2){
			// 	// 	debug << l.k << " " << l.b << std::endl;
			// 	// }
			// 	calcBoundaries(lines1, ch1, true, timeIntv);
			// 	calcBoundaries(lines2, ch2, false, timeIntv);
			// 	const auto intvT = boundaryIntersect(ch1, ch2, timeIntv);
			// // debug << "intvT: " << intvT[0] << " " << intvT[1] << std::endl;
			// 	if(intvT[0]!=-1)feasibleIntvs.push_back(intvT);
			// };

			// debug<<"axis start"<<std::endl;

			for(const auto& axis:axes)
			{
				// debug << "axis: " << axis[0] << " " << axis[1] << " " << axis[2] << std::endl;
				std::vector<Line> ptLines1, ptLines2;
				ptLines1.clear();
				ptLines2.clear();
				for(int i = 0; i < ParamObj1::cntCp; i++)
				{
					// debug << "ptVel1[i]: " << ptVel1[i][0] << " " <<ptVel1[i][1] << " " << ptVel1[i][2] << " ptPos1[i]: " << ptPos1[i][0] << " " << ptPos1[i][1] << " " << ptPos1[i][2] << " axis: " << axis[0] << " " << axis[1] << " " << axis[2] << " ptVel1[i].dot(axis): " << (ptVel1[i].dot(axis)) << " ptPos1[i].dot(axis): " << (ptPos1[i].dot(axis)) << std::endl;
					ptLines1.emplace_back(ptVel1[i].dot(axis), ptPos1[i].dot(axis));
				}
				for(int i = 0; i < ParamObj2::cntCp; i++)
				{
					// debug << "ptVel2[i]: " << ptVel2[i][0] << " " <<ptVel2[i][1] << " " << ptVel2[i][2] << " ptPos2[i]: " << ptPos2[i][0] << " " << ptPos2[i][1] << " " << ptPos2[i][2] << " axis: " << axis[0] << " " << axis[1] << " " << axis[2] << " ptVel2[i].dot(axis): " << (ptVel2[i].dot(axis)) << " ptPos2[i].dot(axis): " << (ptPos2[i].dot(axis)) << std::endl;
					ptLines2.emplace_back(ptVel2[i].dot(axis), ptPos2[i].dot(axis));
				}
				std::sort(ptLines1.begin(), ptLines1.end());
				std::sort(ptLines2.begin(), ptLines2.end());
				// std::cout << "ptLines1: " << ptLines1.size() << " " << ptLines2.size() << std::endl;
				auto intvT = axisCheck(ptLines1, ptLines2, timeIntv);
				if(intvT[0]!=-1)feasibleIntvs.push_back(intvT);
				// std::cout << "check done1." << std::endl;
				intvT = axisCheck(ptLines2, ptLines1, timeIntv);
				if(intvT[0]!=-1)feasibleIntvs.push_back(intvT);
				// debug << "axis: " << axis << std::endl;
				// debug << "ptLines1: " << std::endl;
				// for(auto &l:ptLines1){
				// 	double dbk=l.k, dbb=l.b;
				// 	debug << dbk << " " << dbb << std::endl;
				// 	// debug << l.k << " " << l.b << std::endl;
				// }
				// debug << "ptLines2: " << std::endl;
				// for(auto &l:ptLines2){
				// 	double dbk=l.k, dbb=l.b;
				// 	debug << dbk << " " << dbb << std::endl;
				// 	// debug << l.k << " " << l.b << std::endl;
				// }
				// debug << "feasible intv size: " << feasibleIntvs.size() << std::endl;
			}
			auto b = intvMerge(feasibleIntvs, colTime, initTimeIntv);
			if(colTime[0]==-1||colTime[1]==-1)
			    return false;
			if(colTime[0]<0 || colTime[1]<0)
			{
				debug<<"Error: colTime<0" <<std::endl;
				exit(-1);
			}
			double t1 = colTime[0]-initeps, t2 = colTime[1]+initeps;
			// t1 = ((int)(t1*1000000))*0.000001;
			// t2 = ((int)(t2*1000000))*0.000001;
			t1=std::max(0.0, t1);
			t2=std::min(1.0, t2);
			colTime = Array2r(t1, t2);
			return b;
		}

		static Rational solveCCD(const ParamObj1 &CpPos1, const ParamObj1 &CpVel1,
		                         const ParamObj2 &CpPos2, const ParamObj2 &CpVel2,
		                         Array2r& uv1, Array2r& uv2,
		                         const BoundingBoxType& bb,
		                         const Rational deltaDist,
		                         const Rational upperTime = DeltaT)
		{
			struct PatchPair
			{
				ParamBound1 pb1;
				ParamBound2 pb2;
				Array2r tIntv;
				int depth;
				PatchPair(const ParamBound1& c1, const ParamBound2& c2,
				          const Array2r& t = Array2r(0,DeltaT), const int &depth=1): pb1(c1), pb2(c2), tIntv(t), depth(depth) {}
				bool operator<(PatchPair const &o) const
				{
					//TODO是不是不在乎是否最早
					return tIntv[0]==o.tIntv[0] ? depth < o.depth : tIntv[0] > o.tIntv[0];
					// return depth == o.depth ? tIntv[0] > o.tIntv[0] : depth < o.depth;
				}
				Rational calcWidth() const
				{
					const Rational w1 = pb1.width(), w2 = pb2.width();
					// return std::min(std::max(w1, w2), tIntv[1]-tIntv[0]);
					return max(w1, w2);
				}
				Rational calcAABBWidth(const std::array<Vector3r, ParamObj1::cntCp> &ptPos1,
				                       const std::array<Vector3r, ParamObj2::cntCp> &ptPos2) const
				{
					// auto ptPos1 = CpPos1.divideBezierPatch(pb1);
					// auto ptPos2 = CpPos2.divideBezierPatch(pb2);
					const Rational w1 = calcAAExtent<ParamObj1>(ptPos1), w2 = calcAAExtent<ParamObj2>(ptPos2);
					// return max(std::max(w1, w2), tIntv[1]-tIntv[0]);
					// return min(max(w1, w2), tIntv[1]-tIntv[0]);
					return max(w1, w2);
				}
			};

			// debug << "start solving "<< deltaDist << "\n";
			std::priority_queue<PatchPair> heap;
			ParamBound1 initParam1;
			ParamBound2 initParam2;

			auto ptPos1 = CpPos1.divideBezierPatch(initParam1);
			auto ptVel1 = CpVel1.divideBezierPatch(initParam1);
			auto ptPos2 = CpPos2.divideBezierPatch(initParam2);
			auto ptVel2 = CpVel2.divideBezierPatch(initParam2);
			// debug << "init param1 pts: " << ptPos1[0] << " " << ptPos1[1] << " " << ptPos1[2] << "\n";
			// debug << "init param1 vels: " << ptVel1[0] << " " << ptVel1[1] << " " << ptVel1[2] << "\n";
			// debug << "init param2 pts: " << ptPos2[0] << " " << ptPos2[1] << " " << ptPos2[2] << "\n";
			// debug << "init param2 vels: " << ptVel2[0] << " " << ptVel2[1] << " " << ptVel2[2] << "\n";
			// debug.flush();

			// debug<<"start init"<<std::endl;

			auto start = std::chrono::high_resolution_clock::now();
			Array2r initTimeIntv(Rational(0),upperTime), colTime;
			if (SolverTD<ParamObj1, ParamObj2, ParamBound1, ParamBound2>::primitiveCheck(CpPos1, CpVel1, CpPos2, CpVel2, initParam1, initParam2, colTime, bb, initTimeIntv))
				heap.emplace(initParam1, initParam2, colTime, 1);
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = end - start;
			debug << "Time: " << elapsed.count() << " seconds" << std::endl;
			int cnt = 0;
			// debug<<"\nstart heap"<<std::endl;

			while (!heap.empty())
			{
				cnt++;
				if(cnt>1000)
					return 0;
				auto const cur = heap.top();
				heap.pop();
				ptPos1 = CpPos1.divideBezierPatch(cur.pb1);
				ptVel1 = CpVel1.divideBezierPatch(cur.pb1);
				ptPos2 = CpPos2.divideBezierPatch(cur.pb2);
				ptVel2 = CpVel2.divideBezierPatch(cur.pb2);
				double width = cur.calcWidth();
				// debug << "cur width: " << cur.calcWidth() << "\n";
				debug << "depth: " << cur.depth <<"cur width: " << width << " time intv: " << cur.tIntv[0] << " " << cur.tIntv[1] << std::endl;
				// debug << "cur param1 pts: " << ptPos1[0] << " " << ptPos1[1] << " " << ptPos1[2] << "\n";
				// debug << "cur param2 pts: " << ptPos2[0] << " " << ptPos2[1] << " " << ptPos2[2] << "\n";

				// debug.flush();
				// Meets the precision requirement ptPos1, ptPos2 ptPos1, ptPos2
				if (cur.calcWidth() < deltaDist)
				{
					// if (cur.calcWidth() < deltaDist) {
					uv1 = cur.pb1.centerParam();
					uv2 = cur.pb2.centerParam();
					debug << "return:" << cur.tIntv[0] << "\n";
					return cur.tIntv[0];
				}
				
				for (int i = 0; i < 4; i++) 
				{
					ParamBound1 divUvB1(cur.pb1.interpSubpatchParam(i));
					for (int j = 0; j < 4; j++) 
					{
						ParamBound2 divUvB2(cur.pb2.interpSubpatchParam(j));
						start = std::chrono::high_resolution_clock::now();

						if (primitiveCheck(CpPos1, CpVel1, CpPos2, CpVel2, divUvB1, divUvB2, colTime, bb, cur.tIntv)){
							heap.emplace(divUvB1, divUvB2, colTime, cur.depth+1);
						}
						end = std::chrono::high_resolution_clock::now();
						elapsed = end - start;
						// debug << "Time: " << elapsed.count() << " seconds" << std::endl;
					}
				}

				// debug << "division:" << "\n";
				// Divide the current patch into four-to-four pieces
				// if(calcAAExtent<ParamObj1>(ptPos1) > calcAAExtent<ParamObj2>(ptPos2))
				// {
				// 	for (int i = 0; i < 4; i++)
				// 	{
				// 		ParamBound1 divUvB1(cur.pb1.interpSubpatchParam(i));
				// 		// ptPos1 = CpPos1.divideBezierPatch(divUvB1);
				// 		if (primitiveCheck(CpPos1, CpVel1, CpPos2, CpVel2, divUvB1, cur.pb2, colTime, bb, cur.tIntv))
				// 		{
				// 			// debug << " push:" << colTime[0] << " " << colTime[1] << "\n";
				// 			heap.emplace(divUvB1, cur.pb2, colTime, cur.depth+1);
				// 		}
				// 	}
				// }
				// else
				// {
				// 	for (int j = 0; j < 4; j++) 
				// 	{
				// 		ParamBound2 divUvB2(cur.pb2.interpSubpatchParam(j));
				// 		// ptPos2 = CpPos2.divideBezierPatch(divUvB2);
				// 		// debug << "ptPos2:\n" << ptPos2[0] << " " << ptPos2[1] << " " << ptPos2[2] << std::endl;
				// 		// debug << calcAAExtent<ParamObj2>(ptPos2) << "\n";
				// 		// debug << "div2: " << ptPos2[0] << " " << ptPos2[1] << " " << ptPos2[2];
				// 		if (primitiveCheck(CpPos1, CpVel1, CpPos2, CpVel2, cur.pb1, divUvB2, colTime, bb, cur.tIntv))
				// 		{
				// 			// debug << " push:" << colTime[0] << " " << colTime[1] << "\n";
				// 			heap.emplace(cur.pb1, divUvB2, colTime, cur.depth+1);
				// 		}
				// 		// debug << "\n";
				// 	}
				// }
			}

			return -1;
		}
};
