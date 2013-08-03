//
//  transform_gizmo.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 29/05/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__transform_gizmo__
#define __RollerBallCore__transform_gizmo__

#include "components_base.h"
#include "node.h"
#include "rectangle.h"
#include "transform_space.h"

namespace rb {
    class mesh;
    class transform_gizmo : public node {
    private:
        mesh* _gizmo_mesh;
        rectangle _bounds;
        bool _live;
        bool _in_live;
        std::function<void (transform_gizmo*, const transform_space&)> _update_function;
        node_container* _saved_current;
        bool _activated;
        bool _resetting;
    private:
        transform_gizmo(const rectangle& bounds, const bool live,
                        std::function<void (transform_gizmo*, const transform_space&)> update_function);
    public:
        virtual ~transform_gizmo();
        static void start_transformation(node_container* parent, const transform_space& transform, const rectangle& bounds, const bool live, std::function<void (transform_gizmo*, const transform_space&)> update_function);
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    protected:
        virtual void in_editor_update(float dt) override;
        virtual void after_becoming_active(bool node_was_moved) override;
        virtual void transform_changed() override;
        virtual void begin_live_edit(rb::live_edit kind) override;
        virtual void end_live_edit() override;
        virtual void render_gizmo() override;
        virtual rectangle bounds() const override;
    };
    
    class transform_gizmo_space : public node {
    private:
        node_container* _saved_current;
    public:
        friend class transform_gizmo;
        transform_gizmo_space();
        virtual ~transform_gizmo_space();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        //Resigned as current...
    protected:
        virtual void resign_current() override;
    };
}

#endif /* defined(__RollerBallCore__transform_gizmo__) */
