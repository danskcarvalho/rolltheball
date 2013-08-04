//
//  misc_classes_2.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__misc_classes_2__
#define __RollerBallCore__misc_classes_2__

#include "components_base.h"
#include "typed_object.h"

namespace rb {
    class node;
    class transformation_values;
    class component_id : public typed_object {
    private:
        rb_string _name;
        rb_string _classes;
        transformation_values* _internal;
    public:
        node* named_node;
        inline const rb_string& name() const {
            return _name;
        }
        const rb_string& name(const rb_string& value);
        inline const rb_string& classes() const {
            return _classes;
        }
        const rb_string& classes(const rb_string& value);
        //Constructors
        component_id();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    };
    
    class transformation_values : public typed_object {
    private:
        vec2 _translation;
        vec2 _scale;
        vec2 _rotation;
        bool _orthogonal;
        bool _uniform;
    public:
        node* transformed_node;
        inline const vec2& translation() const {
            return _translation;
        }
        
        inline const vec2& scale() const {
            return _scale;
        }
        
        inline const vec2& rotation() const {
            return _rotation;
        }
        
        inline const vec2& translation(const vec2& value) {
            _translation = value;
            notify_property_changed(u"translation");
            return _translation;
        }
        
        inline const vec2& scale(const vec2& value) {
            _scale = value;
            notify_property_changed(u"scale");
            //We won't enforce uniformity...
            //            if(almost_equal(value.x(), value.y()))
            //                uniform(true);
            //            else
            //                uniform(false);
            return _scale;
        }
        
    private:
        inline float normalize_angle(float angle){
            float angleValue = angle;
            if(angleValue > 0)
            {
                angleValue = angleValue - floorf(angleValue / (2 * 180)) * (2 * 180);
            }
            else if(angleValue < 0){
                angleValue = -angleValue;
                angleValue = angleValue - floorf(angleValue / (2 * 180)) * (2 * 180);
                angleValue = -angleValue;
                angleValue = (2 * 180) + angleValue;
            }
            return angleValue;
        }
    public:
        inline const vec2& rotation(const vec2& value) {
            _rotation = value;
            notify_property_changed(u"rotation");
            //we won't enforce orthogonality...
            //            auto _diff = fabs(normalize_angle(_rotation.x()) - normalize_angle(_rotation.y()));
            //            if(almost_equal(_diff, 90))
            //                orthogonal(true);
            //            else
            //                orthogonal(false);
            return _rotation;
        }
        
        inline bool orthogonal() const {
            return _orthogonal;
        }
        
        inline bool orthogonal(const bool value){
            _orthogonal = value;
            notify_property_changed(u"orthogonal");
            return _orthogonal;
        }
        
        inline bool uniform() const {
            return _uniform;
        }
        
        inline bool uniform(const bool value){
            _uniform = value;
            notify_property_changed(u"uniform");
            return _uniform;
        }
        void sync_values();
        //Constructors
        transformation_values();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    };
}

#endif /* defined(__RollerBallCore__misc_classes_2__) */
