//
//  node.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__node__
#define __RollerBallCore__node__

#include "components_base.h"
#include "node_container.h"
#include "transform_space.h"
#include "polygon.h"

namespace rb {
    class scene;
    class layer;
    class mesh;
    class texture_map;
    class rectangle;
    class touch;
    class component_id;
    class transformation_values;
    class node : public node_container {
    private:
        bool _blocked;
        layer* _parent_layer;
        node* _parent_node;
        bool _added;
        bool _active;
        int _move_flag;
        //identification
        component_id* _id;
        //transformation
        transformation_values* _transformation;
        bool _enabled_transformation_notifications;
        //update registration
        typedef std::multimap<int32_t, node*> ordered_nodes;
        nullable<ordered_nodes::iterator> _registration_ids[(size_t)registrable_event::count];
        nullable<ordered_nodes::iterator>& registration_id(const registrable_event event);
        //new template
        bool _new_template;
        
        //Meshes
        mesh* _selection_marquee;
        
        //enable/disable
        bool _capabilities[(size_t)node_capability::count];
    public:
        bool template_for_new() const;
    public:
        //friend classes
        friend class layer;
        friend class scene;
        friend class node_container;
        //constructors/destructors
        node();
        virtual ~node();
    public:
        //Name
        const rb_string& name() const;
        const rb_string& name(const rb_string& value);
        const rb_string& classes() const;
        const rb_string& classes(const rb_string& value);
        bool has_class(const rb_string& cls) const;
    public:
        //adding/removing nodes
        virtual bool add_node_at(node* n, uint32_t at) override;
        virtual bool remove_node(node* n, bool cleanup) override;
        virtual bool move_node_to(node_container* new_parent);
        
        //Reordering nodes
        virtual bool bring_to_front(node* n) override;
        virtual bool send_to_back(node* n) override;
        virtual bool send_backward(node* n) override;
        virtual bool bring_forward(node* n) override;
        
        //Parent Scene
    public:
        scene* parent_scene();
        const scene* parent_scene() const;
        layer* parent_layer();
        const layer* parent_layer() const;
        node* parent_node();
        const node* parent_node() const;
    public:
        bool in_editor() const;
        //set layer recursively
    private:
        void set_layer_recursively(layer* value);
        
        //Activation/Deactivation
        //Properties
    public:
        bool active() const;
        //Listeners
    private:
        void internal_before_becoming_active(bool node_was_moved);
        void internal_after_becoming_active(bool node_was_moved);
        void internal_before_becoming_inactive(bool node_was_moved);
        void internal_after_becoming_inactive(bool node_was_moved);
        //Protected Listeners
    protected:
        virtual void before_becoming_active(bool node_was_moved);
        virtual void after_becoming_active(bool node_was_moved);
        virtual void before_becoming_inactive(bool node_was_moved);
        virtual void after_becoming_inactive(bool node_was_moved);
        
    protected:
        //Enabling/Disabling
        void enabled(const node_capability capability, bool value);
    public:
        bool enabled(const node_capability capability) const;
        
        //Playing/Pause
    private:
        void scene_playing();
        void scene_paused();
    protected:
        virtual void playing();
        virtual void paused();
        bool is_playing() const;
        
        //Updating
        //Registering
        //This method can be called again to change the priority...
        void register_for(const registrable_event event, int32_t priority);
        void unregister_for(const registrable_event event);
        void unregister_for_all_events();
        //Callbacks
    protected:
        virtual void update(float dt);
        virtual void in_editor_update(float dt);
        //Rendering
    protected:
        static rb_string no_texture;
        void invalidate_buffers();
        //the mesh should be in layer space...
        void add_mesh_for_rendering(mesh* m, const rb_string& texture_name, bool wrapping_in_shader);
        void set_texture_for_mesh(mesh* m, const rb_string& texture_name, bool wrapping_in_shader);
        //the mesh should be in screen space...
        void add_gizmo(mesh* m, const rb_string& texture_name, bool wrapping_in_shader);
        const texture_map* create_mapping(const rb_string& texture_name, const transform_space& transform, texture_map* refurbished = nullptr);
        virtual void render(const bool refill_buffers);
        virtual void render_gizmo();
    private:
        void internal_render(const bool refill_buffers);
        void internal_render_gizmo();
        void render_selection_marquee();
        //Transform
    protected:
        virtual const node_container* parent_node_container() const override;
        virtual void space_changed();
    public:
        matrix3x3 from_node_space_to(const space another) const;
        matrix3x3 from_node_space_to(const node_container* another) const;
        //Bounds and Hit Test
    protected:
        //returns the bounds in self space...
        virtual rectangle bounds() const;
    private:
        //test against the bounding rectangle...
        //pt in screen coordinates...
        virtual bool fast_hit_test(const vec2& pt) const;
        //rc in screen coordinates...
        virtual bool fast_hit_test(const rectangle& rc) const;
    protected:
        //pt in screen coordinates
        virtual bool hit_test(const vec2& pt) const;
        //rc in screen coordinates
        virtual bool hit_test(const rectangle& rc) const;
        //selection
    public:
        bool is_selected() const;
        bool is_currently_selected() const;
        void add_to_selection();
        void remove_from_selection();
        //interactions
    protected:
        virtual void keydown(const uint32_t keycode, const keyboard_modifier modifier, bool& swallow);
        virtual void keyup(const uint32_t keycode, const keyboard_modifier modifier, bool& swallow);
        virtual void touches_began(const std::vector<touch>& touches, bool& swallow);
        virtual void touches_moved(const std::vector<touch>& touches, bool& swallow);
        virtual void touches_ended(const std::vector<touch>& touches, bool& swallow);
        virtual void touches_cancelled(const std::vector<touch>& touches, bool& swallow);
        virtual void mouse_down(const vec2& normalized_position, bool& swallow);
        virtual void mouse_up(const vec2& normalized_position, bool& swallow);
        virtual void mouse_dragged(const vec2& normalized_position, bool& swallow);
        //live editing
    protected:
        virtual void begin_live_edit(rb::live_edit kind);
        virtual void end_live_edit();
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
        //Transform
    public:
        virtual const matrix3x3& transform() const override;
        virtual const matrix3x3& transform(const rb::matrix3x3 &value) override;
    private:
        void shutdown_transform_notifications();
        void restore_transform_notification();
    protected:
        virtual void transform_changed();
    public:
        //alert and confirmation
        void alert(const rb_string& message) const;
        void confirm(const rb_string& message, std::function<void (bool)> result) const;
        void push_itself_in_property_inspector(const rb_string& title = u"");
    protected:
        virtual void set_children(const std::vector<typed_object*>& children) override;
        //Blocked
    public:
        bool blocked() const;
        bool blocked(const bool value);
    };
    
    bool is_node(const typed_object* obj);
}
    
#endif /* defined(__RollerBallCore__node__) */
