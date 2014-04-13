//
//  matrix3x3.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 02/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__matrix3x3__
#define __RollerBallCore__matrix3x3__

#include "base.h"
#include "vec2.h"

namespace rb {
    class rectangle;
    class ray;
    class circle;
    class polygon;
    class smooth_curve;
    class mesh;
    union _matrix3x3
    {
        struct
        {
            float m00, m10, m20;
            float m01, m11, m21;
            float m02, m12, m22;
        };
        float m[9];
    };
    class matrix3x3 {
    private:
        //row 01
        vec2 _x_vector;
        vec2 _y_vector;
        vec2 _translation;
        void _invert(matrix3x3* m_ptr, bool set);
    public:
        //constants
        const static matrix3x3 identity;
        
        //constructors
        matrix3x3();
        matrix3x3(const matrix3x3& m);
        matrix3x3(matrix3x3&& m);
        matrix3x3(const vec2& x_vector, const vec2& y_vector, const vec2& translation);
        
        //assignment operators
        const matrix3x3& operator=(const matrix3x3& other);
        inline __attribute__ ((always_inline)) const matrix3x3& operator*=(const rb::matrix3x3 &other){
            float m00 = _x_vector.x()*other._x_vector.x() + _y_vector.x()*other._x_vector.y();
            float m01 = _x_vector.x()*other._y_vector.x() + _y_vector.x()*other._y_vector.y();
            float m02 = _x_vector.x()*other._translation.x() + _y_vector.x()*other._translation.y() + _translation.x();
            
            float m10 = _x_vector.y()*other._x_vector.x() + _y_vector.y()*other._x_vector.y();
            float m11 = _x_vector.y()*other._y_vector.x() + _y_vector.y()*other._y_vector.y();
            float m12 = _x_vector.y()*other._translation.x() + _y_vector.y()*other._translation.y() + _translation.y();
            
            _x_vector.x(m00);
            _x_vector.y(m10);
            _translation.x(m02);
            
            _y_vector.x(m01);
            _y_vector.y(m11);
            _translation.y(m12);
            
            return *this;
        }
        
        //get methods
        inline const vec2& x_vector() const{
            return _x_vector;
        }
        
        inline const vec2& y_vector() const{
            return _y_vector;
        }
        
        inline const vec2& translation() const{
            return _translation;
        }
        
        //non-const get methods
        inline const vec2& x_vector(const vec2& value) {
            _x_vector = value;
            return _x_vector;
        }
        
        inline const vec2& y_vector(const vec2& value) {
            _y_vector = value;
            return _y_vector;
        }
        
        inline const vec2& translation(const vec2& value) {
            _translation = value;
            return _translation;
        }
        
        void set_matrix3x3(_matrix3x3& matrix) const;
        
        bool is_identity() const;
        bool is_inversible() const;
        
        //other operations
        matrix3x3& invert();
        matrix3x3 inverse() const;
        float determinant() const;
        
        //transform
        vec2 transformed_vector(const vec2& vec) const;
        vec2& transform_vector(vec2& vec) const;
        vec2 transformed_point(const vec2& vec) const;
        vec2& transform_point(vec2& vec) const;
        rectangle transformed_rectangle(const rectangle& rc) const;
        rectangle& transform_rectangle(rectangle& rc) const;
        ray transformed_ray(const ray& r) const;
        ray& transform_ray(ray& r) const;
        circle transformed_circle(const circle& c) const;
        circle& transform_circle(circle& c) const;
        polygon transformed_polygon(const polygon& p) const;
        polygon& transform_polygon(polygon& p) const;
        smooth_curve transformed_smooth_curve(const smooth_curve& c) const;
        smooth_curve& transform_smooth_curve(smooth_curve& c) const;
        mesh transformed_mesh(const mesh& m) const;
        mesh& transform_mesh(mesh& m) const;
        mesh transformed_texture(const mesh& m) const;
        mesh& transform_texture(mesh& m) const;
        
        //build constructors
        static matrix3x3 build_rotation(const float angle);
        static matrix3x3 build_translation(const vec2& translation);
        static matrix3x3 build_translation(const float tx, const float ty);
        static matrix3x3 build_scale(const vec2& scale);
        static matrix3x3 build_scale(const float sx, const float sy);
        static matrix3x3 build_skew(const vec2& skew);
        static matrix3x3 build_skew(const float sx, const float sy);
        
        //to_string
        rb_string to_string() const;
        
        //friends
        friend matrix3x3 operator *(const matrix3x3 &, const matrix3x3 &);
        friend bool operator ==(const matrix3x3&, const matrix3x3&);
        friend bool operator!=(const matrix3x3&, const matrix3x3&);
    };
    
    //other operators
    inline __attribute__ ((always_inline)) matrix3x3 operator*(const matrix3x3 & first, const matrix3x3 & second){
        float m00 = first._x_vector.x()*second._x_vector.x() + first._y_vector.x()*second._x_vector.y();
        float m01 = first._x_vector.x()*second._y_vector.x() + first._y_vector.x()*second._y_vector.y();
        float m02 = first._x_vector.x()*second._translation.x() + first._y_vector.x()*second._translation.y() + first._translation.x();
        
        float m10 = first._x_vector.y()*second._x_vector.x() + first._y_vector.y()*second._x_vector.y();
        float m11 = first._x_vector.y()*second._y_vector.x() + first._y_vector.y()*second._y_vector.y();
        float m12 = first._x_vector.y()*second._translation.x() + first._y_vector.y()*second._translation.y() + first._translation.y();
        
        return matrix3x3(vec2(m00, m10), vec2(m01, m11), vec2(m02, m12));
    }
    bool operator ==(const matrix3x3& first, const matrix3x3& second);
    bool operator!=(const matrix3x3& first, const matrix3x3& second);
}

CLASS_NAME(rb::matrix3x3);

#endif /* defined(__RollerBallCore__matrix3x3__) */
