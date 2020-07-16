#ifndef MCF_ROUTING_RTREE_H
#define MCF_ROUTING_RTREE_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/foreach.hpp>
#include <tuple>
#include <armadillo>

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>

#include <climits>
#include <cstring>

#include <memory>

#include <iomanip>

#include <stdlib.h>     /* srand, rand */

namespace bg  = boost::geometry;
namespace bgi = boost::geometry::index;

namespace UCal{
typedef unsigned int Idx;

typedef bg::model::point<float, 2, bg::cs::cartesian> Point2D;
typedef bg::model::point<unsigned int, 3, bg::cs::cartesian> Point3D;
typedef bg::model::box<Point2D> Box2D;
typedef std::pair<Box2D, Idx> Value;
typedef std::pair<Point2D, Point2D> Edge;
typedef Point2D Node;




class Point3DWrapper{
    public: 
        Point3DWrapper(Point3D p){
            _point = p;
        }
        Point3DWrapper(unsigned int x,unsigned int y,unsigned int z){
            bg::set<0>(_point, x);
            bg::set<1>(_point, y);
            bg::set<2>(_point, z);
        }
        ~Point3DWrapper(){}

        unsigned int getX(){return _point.get<0>();}
        unsigned int getY(){return _point.get<1>();}
        unsigned int getZ(){return _point.get<2>();}

        std::string convertToString(){
            std::string tmp = std::to_string(getX()) +" "+std::to_string(getY())+" "+std::to_string(getZ());
            return tmp;
        }

        Point3D getPoint(){
            return _point;
        }
                
        void report(){
            std::cout << getX() << " " << getY() << " " << getZ() << " " << std::endl;
        }

    private: 
        Point3D _point;
};//end Point3D

// it is a wrapper for box type
class Box2DWrapper{
    public: 
        Box2DWrapper(Box2D b) : _box(b){

            _hx = b.max_corner().get<0>();
            _hy = b.max_corner().get<1>();
            _lx = b.min_corner().get<0>();
            _ly = b.min_corner().get<1>();
            
            _width = std::abs(_hy - _ly);
            _height = std::abs(_hx - _lx);

            _center_X = _lx + 0.5;
            _center_y = _ly + 0.5;
        }
        Box2DWrapper(float lx,float ly,float hx,float hy) {

            _lx = std::min(lx,hx);
            _ly = std::min(ly,hy);

            _hx = std::max(lx,hx);
            _hy = std::max(ly,hy);

            Box2D tmp_b(Point2D(_lx,_ly),Point2D(_hx,_hy));
            _box = tmp_b;
            
            _width = std::abs(_hy - _ly);
            _height = std::abs(_hx - _lx);

            _center_X = _lx + 0.5;
            _center_y = _ly + 0.5;
        }

        Box2DWrapper() {}
        ~Box2DWrapper(){}

        Box2D getBox() {return _box;}

        float getXl(){return _lx;}
        float getYl(){return _ly;}
        float getXh(){return _hx;}
        float getYh(){return _hy;}

        Point2D getCenter(){
            Point2D p(_center_X,_center_y);
            return p;
        }//end get center

        unsigned int getWidth(){
            return _width;
        }
        unsigned int getHeight(){
            return _height;
        }

        bool isVer(Box2DWrapper b){
            if( this->_ly == b.getYl() && this->_hy == b.getYh() )
                return true;
            return false;
        }

        bool isEqual(Box2DWrapper b){
            if(this->_lx == b.getXl() && this->_hx == b.getXh() && 
            this->_ly == b.getYl() && this->_hy == b.getYh())
            return true;
            return false;
        }

        bool isHor(Box2DWrapper b){
            if( this->_lx == b.getXl() && this->_hx == b.getXh() )
                return true;
            return false;
        }

        bool isInside(Point2D& p){
            return boost::geometry::within(p, _box);
        }


        std::string boxToString(){
            std::ostringstream out;
            out.precision(1);
            out << std::fixed << _lx << "_" << _ly << "_" << 
                                _hx << "_" << _hy;
            return out.str();
        }

        //report 
        void report(){
            std::cout << "Box: " << _lx << " " << _ly << " " << _hx << " " << _hy << std::endl;
        }//end report 

        
    private:

        float _lx;
        float _ly;
        float _hx;
        float _hy;

        float _width;
        float _height;

        unsigned int _center_X;
        unsigned int _center_y;

        Box2D _box;

};//end class Box


class RTreeDB{
    public:
        RTreeDB(){}
        ~RTreeDB(){}

        void insert(Box2DWrapper& b,Idx idx){
            Box2D b_tmp = b.getBox();
            
            _rtree.insert(std::make_pair(b_tmp, idx));
        }//end insert function 

        void insert(Box2DWrapper& b){
            Box2D b_tmp = b.getBox();
            _rtree.insert(std::make_pair(b_tmp, _rtree.size()));
        }//end insert function 

        // std::vector<Box2DWrapper>& query(Box2DWrapper& b_org){
        //     _box2d_wrappers.clear();
        //     std::vector<Value> result_s;
        //     Box2D b = b_org.getBox();
        //     _rtree.query(bgi::intersects(b), std::back_inserter(result_s));

            
        //     BOOST_FOREACH(Value const& v, result_s){
        //         Box2DWrapper b_tmp = getBox2DToWrapper(v.first);
        //         _box2d_wrappers.push_back(b_tmp);
        //     }
                
        //     return _box2d_wrappers;
        // }//end query function 

        std::vector<Value> query(Box2DWrapper b_org){
            std::vector<Value> result_s;
            Box2D b = b_org.getBox();
            _rtree.query(bgi::intersects(b), std::back_inserter(result_s));
                
            return result_s;
        }//end query function 

        

        std::vector<Box2DWrapper>& query(Point2D& p,int num_nearest){
            _box2d_wrappers.clear();
            std::vector<Value> result_n;
            _rtree.query(bgi::nearest(p, num_nearest), std::back_inserter(result_n));
            BOOST_FOREACH(Value const& v, result_n){
                Box2DWrapper b_tmp = getBox2DToWrapper(v.first);
                _box2d_wrappers.push_back(b_tmp);
            }
            return _box2d_wrappers;
        }//end query function 

        void remove(Box2DWrapper b, unsigned int idx){
            std::vector<Value> values = query(b);
            for(auto tmp_value : values)
            {
                if(tmp_value.second == idx)
                    _rtree.remove(tmp_value);
            }//end for 
                
        }//end remove function 

        std::vector<Value> getValues(){
            std::vector<Value> vec;
            std::copy(_rtree.begin(), _rtree.end(), std::back_inserter(vec));
            return vec;
        }


        // std::vector<Value> queryValue(Box2D& b){
        //     std::vector<Value> result_s;
        //     _rtree.query(bgi::intersects(b), std::back_inserter(result_s)); 
        //     return result_s;
        //     }

        //     std::vector<Box2DWrapper>& getAsVector(){
        //         _box2d_wrappers.clear();
        //         std::vector<Value> vec;
        //         std::copy(_rtree.begin(), _rtree.end(), std::back_inserter(vec));
        //         BOOST_FOREACH(Value const& v, vec){
        //             // std::cout << bg::wkt<box>(v.first) << " - " << v.second << std::endl;
        //             unsigned int max_x = v.first.max_corner().get<0>();
        //             unsigned int max_y = v.first.max_corner().get<1>();
        //             unsigned int min_x = v.first.min_corner().get<0>();
        //             unsigned int min_y = v.first.min_corner().get<1>();
        //             Box2D b_tmp(Point2D(min_x,min_y),Point2D(max_x,max_y));
        //             _boxs.push_back(b_tmp);
        //         }
        //         return _boxs;
        // }

        std::vector<Box2DWrapper> getNeighbourHor(Box2DWrapper b){
            
            std::vector<Box2DWrapper> box_h;
            std::vector<Value> boxs = query(b);

            for(auto b_tmp : boxs){
                Box2DWrapper b_new(b_tmp.first);
                if(b.isHor(b_new) && !b.isEqual(b_new))
                    box_h.push_back(b_new);
            }

            return box_h;
        }//end getHorizontalNeighbour


        std::vector<Box2DWrapper> getNeighbourVer(Box2DWrapper b){
            
            std::vector<Box2DWrapper> box_v;
            std::vector<Value> boxs = query(b);

            for(auto b_tmp : boxs){
                Box2DWrapper b_new(b_tmp.first);
                if(b.isVer(b_new) && !b.isEqual(b_new))
                    box_v.push_back(b_new);
            }

            return box_v;
        }//end getHorizontalNeighbour

        void report(){
            // Copy all values into the vector
            std::vector<Value> vec;
            std::copy(_rtree.begin(), _rtree.end(), std::back_inserter(vec));
            BOOST_FOREACH(Value const& v, vec){
                std::cout << bg::wkt<Box2D>(v.first) << " - " << v.second << ": ";
                Box2DWrapper b_tmp = getBox2DToWrapper(v.first);
                b_tmp.report();
            }
        }//end report function 

        private: 
            Box2DWrapper getBox2DToWrapper(Box2D box2D){
                float max_x = box2D.max_corner().get<0>();
                float max_y = box2D.max_corner().get<1>();
                float min_x = box2D.min_corner().get<0>();
                float min_y = box2D.min_corner().get<1>();
                Box2D b(Point2D(min_x,min_y),Point2D(max_x,max_y));
                Box2DWrapper b_tmp(b);
                return b_tmp;
            }
            // create the rtree using default constructor
            bgi::rtree< Value, bgi::quadratic<16> > _rtree;
            std::vector<Box2DWrapper> _box2d_wrappers;

};//end class RTree




class Segment{
public:
    Segment(Point3DWrapper p_A, Point3DWrapper p_B, Idx idx) :
        _point_A(p_A),
        _point_B(p_B),
        _idx(idx)
    {
        _congestion = 0;
    }
    ~Segment(){

    }

    Idx getIdx() {return _idx;}

    Point3DWrapper& getPointA() {
        return _point_A;
    }

    Point3DWrapper& getPointB() {
        return _point_B;
    }

    bool isSegmentInSameLayer(){
        if(_point_A.getZ() == _point_B.getZ())
            return true;
        return false;
    }


    std::string convertToString(){
        
        Point3DWrapper& p_a = getPointA();
        Point3DWrapper& p_b = getPointB();
        std::string p_a_str = p_a.convertToString();
        std::string p_b_str = p_b.convertToString();

        std::string seg_str = p_a_str +" "+ p_b_str;
        return seg_str;
    }//end convertToString

    void setCongestion(unsigned int cong){_congestion =cong; }
    unsigned int getCongestion(){return _congestion; }


    Box2DWrapper getBox(){
        float min_x = std::min(_point_A.getX(),_point_B.getX());
        float min_y = std::min(_point_A.getY(),_point_B.getY());
        float max_x = std::max(_point_A.getX(),_point_B.getX());
        float max_y = std::max(_point_A.getY(),_point_B.getY());
        Box2DWrapper b(min_x,min_y,max_x,max_y);
        return b;
    }

    void report(){
        std::cout << "Seg: \n";
        _point_A.report();

        _point_B.report();
        std::cout << "\n";
    }

private:
    Idx _idx;
    Point3DWrapper _point_A;
    Point3DWrapper _point_B;
    //each edge has a congestion 
    unsigned int _congestion;
};//end Segmenet Class 

};//end namespace 


#endif