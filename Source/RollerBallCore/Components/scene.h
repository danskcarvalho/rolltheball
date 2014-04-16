//
//  scene.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 07/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__scene__
#define __RollerBallCore__scene__

#include "components_base.h"
#include "responder.h"
#include "transform_space.h"
#include "color.h"
#include "vec2.h"
#include "touch.h"
#include "rectangle.h"
#include "typed_object.h"
#define BOOST_DISABLE_THREADS
#include <boost/pool/pool_alloc.hpp>

namespace rb {
    class director;
    class layer;
    class node;
    class texture_atlas;
    class program_manager;
    class extended_dynamic_mesh_batch;
    class node_container;
    class type_descriptor;
    class node_without_transform;
    class component_id;
    class mesh;
    class dynamic_mesh_batch;
    class basic_process;
    class scene : private responder, public typed_object {
    private:
        typedef std::unordered_set<node*, std::hash<node*>, std::equal_to<node*>, boost::pool_allocator<node*>> node_set;
        typedef std::unordered_map<rb_string, node_set, std::hash<rb_string>, std::equal_to<rb_string>,
            boost::pool_allocator<std::pair<const rb_string, node_set>>> node_dictionary;
    private:
        void rename_nodes_recursively(node* n);
        //name seed
        uint64_t _n_seed;
        //fading
        color _fade_color;
        mesh* _fade_mesh;
        dynamic_mesh_batch* _fade_batch;
        basic_process* _textureless_process;
        //names and classes
        node_dictionary _by_name;
        node_dictionary _by_class;
        //saved scene
        rb_string _saved_scene;
        //saved current
        nullable<rb_string> _saved_current;
        //fields
        bool _dirty_transform;
        //registered nodes to send update...
        typedef std::multimap<int32_t, node*> ordered_nodes;
        ordered_nodes _registered_nodes[(size_t)registrable_event::count];
        ordered_nodes& registered_nodes(const registrable_event event);
        std::vector<std::pair<registrable_event, ordered_nodes::iterator>> _to_be_unregistrated;
        std::set<std::pair<registrable_event, node*>> _to_be_unregistrated2;
        bool _active;
        bool _playing;
        bool _in_editor;
        layer* _layers[MAX_LAYERS + 1];
        //gizmo layer
        extended_dynamic_mesh_batch* _gizmo_layer;
        //current
        node_container* _current_node;
        
        //rendering
        texture_atlas* _texture_atlas;
        rb_string _atlas_path;
        program_manager* _program_manager;
        
        //activation listeners
        void before_becoming_active();
        void after_becoming_active();
        void before_becoming_inactive();
        void after_becoming_inactive();
        
        //camera
        bool _enabled_aspect_correction;
        transform_space _camera;
        navigation_mode _navigation_mode;
        //background color
        color _background_color;
        //gizmo color
        color _gizmo_color;
        color _alternate_gizmo_color;
        //selection color
        color _selection_color;
        //viewport
        vec2 _viewport_size;
        
        //creation mode...
        node_without_transform* _new_template;
        vec2 _new_start_point;
        node* _current_new; //the current node being manipulated by the user
        //locked selection
        node* _locked_selection;
        //fading..
    private:
        void create_fading_machinery();
    public:
        const color& fade_color() const;
        const color& fade_color(const color& value);
        //locked selection
    public:
        node* locked_selection() const;
        void lock_selection();
        void unlock_selection();
    public:
        void enter_new_mode(const class type_descriptor* td);
        void exit_new_mode();
        typed_object* template_for_new() const;
    public:
        //friend classes
        friend class director;
        friend class node;
        friend class layer;
        friend class node_container;
        friend class component_id;
        
        //constructors/destructors
        scene();
        ~scene();
        
        //in-editor
        bool in_editor() const;
        
        //methods
        bool active() const;
        
        //layer
        class layer* layer(uint32_t index);
        const class layer* layer(uint32_t index) const;
        class layer* hidden_layer();
        const class layer* hidden_layer() const;
        
        //play/pause
        bool playing() const;
        void playing(const bool value);
        //updating
        //registering for updates
    private:
        void unregister_nodes();
        bool unregistered(const registrable_event event, node* n);
        ordered_nodes::iterator register_for(const registrable_event event, int32_t priority, node* n);
        void unregister_for(const registrable_event event,ordered_nodes::iterator it);
        //update methods
    private:
        //update called by the outside...
        void update() override;
        //update called at 30fps
        void fixed_update(float dt);
        //update called in-editor...
        void in_editor_update(float dt);
    public:
        //rendering
        const rb_string& atlas_path() const;
        const texture_atlas* atlas() const;
        const texture_atlas* atlas(const texture_atlas* value);
        void load_atlas(const rb_string& url);
        //camera
        const transform_space& camera() const;
        const transform_space& camera(const transform_space& value);
        bool camera_aspect_correction() const;
        bool camera_aspect_correction(bool enabled);
        enum navigation_mode navigation_mode() const;
        enum navigation_mode navigation_mode(enum navigation_mode value);
        //background color
        const color& background_color() const;
        const color& background_color(const color& value);
        const color& gizmo_color() const;
        const color& gizmo_color(const color& value);
        const color& alternate_gizmo_color() const;
        const color& alternate_gizmo_color(const color& value);
        const color& selection_color() const;
        const color& selection_color(const color& value);
        bool in_live_selection() const;
        //rendering callback
        virtual void render();
    private:
        void render_selection_rectangle();
        void render_handles();
        //viewport
    public:
        const vec2& viewport_size() const;
    private:
        virtual void viewport_resized();
        //spaces
    public:
        transform_space from_space_to_another(const space from, const space to);
        //current
    public:
        node_container* current() const;
        node_container* current(node_container* value);
        //responder
    public:
        responder* get_responder();
        //interactions
        //when a key is pressed on the keyboard
        //never delivered in iOS...
    private:
        virtual void keydown(const uint32_t keycode, const keyboard_modifier modifier) override;
        virtual void keyup(const uint32_t keycode, const keyboard_modifier modifier) override;
        
        virtual void begin_gesture() override;
        virtual void end_gesture() override;
        
        //gestures
        virtual void magnify(const float delta) override;
        virtual void rotate(const float delta) override;
        virtual void swipe(const vec2& delta) override;
        virtual void scroll(const vec2& delta) override;
        
        //touches
        virtual void touches_began(const std::vector<touch>& touches) override;
        virtual void touches_moved(const std::vector<touch>& touches) override;
        virtual void touches_ended(const std::vector<touch>& touches) override;
        virtual void touches_cancelled(const std::vector<touch>& touches) override;
        
        void cancel_user_selection();
        
        //basic mouse events
        //those events isn't delivered in iOS...
        virtual void mouse_down(const vec2& normalized_position) override;
        virtual void mouse_up(const vec2& normalized_position) override;
        virtual void mouse_dragged(const vec2& normalized_position) override;
        
        
        //mouse dragging
        vec2 _initial_mouse_pos;
        vec2 _last_mouse_pos;
        //selection
        nullable<rectangle> _selection_rectangle;
        std::unordered_set<node*> _saved_selection;
        mesh* _selection_marquee;
        bool _dragged_mouse;
        
        //transformation
        bool _in_transformation;
        enum {
            tm_none,
            tm_move,
            tm_scale,
            tm_rotate
        } _transform_mode;
        nullable<vec2> _handler_transformation;
        transform_space _delta_transform;
        rectangle _selected_nodes_bounds; //in current space
        std::map<node*, transform_space> _original_transforms;
        
        //handles
    private:
        struct handle {
            mesh* display_mesh;
            mesh* display_mesh_copy;
            vec2 axis;
        };
        mesh* _selection_indicator;
        handle _handles[HANDLE_COUNT];
        mesh* _m_x_axis;
        mesh* _m_y_axis;
        
        //methods
        void render_selection_indicator();
        nullable<vec2> hit_test_handler(const vec2& normalized_position);
        
        //possible interactions
    private:
        //returns true if the user started dragging the mouse to select one or more nodes; false if it's gonna transform the nodes
        transform_space get_current_transform(bool live = false);
        bool start_selection(const vec2& normalized_position);
        void try_select_one(const vec2& normalized_position);
        void delete_selected();
        void drag_selection_marquee(const vec2& normalized_position);
        void start_transformation();
        void transform_nodes(const std::vector<node *>& selection);
        void move_selected(const vec2& dir);
        //return bounds in current space
        rectangle compute_selection_bounds(const std::vector<node*>& selection, const bool live = false);
        //delta is in screen coordinates
        void drag_selected(const vec2& delta);
        void handler_transform(const vec2& normalized_position);
        void scale_selected(const vec2& delta);
        void rotate_selected(const vec2& normalized_position);
    protected:
        //Typed Object
        virtual void describe_type() override;
    public:
        virtual rb_string type_name() const override;
        virtual rb_string displayable_type_name() const override;
    public:
        //alert and confirmation
        void alert(const rb_string& message) const;
        void confirm(const rb_string& message, std::function<void (bool)> result) const;
    protected:
        virtual void fill_vector_with_children(std::vector<typed_object*>& children) const override;
        virtual void set_children(const std::vector<typed_object*>& children) override;
        //copy/paste
    public:
        rb_string copy_selected_nodes();
        void paste_nodes(rb_string copied);
        //serialization
    protected:
        virtual void was_deserialized() override;
    public:
        virtual typed_object* clone() const override;
    private:
        typed_object* clone_children(scene* cloned) const;
        //camera match current
    public:
        void camera_to_match_current_rotation();
        //Blocking
    public:
        void unblock_all();
        //Hiding
    public:
        void unhide_all();
        //Reordering Layers
    public:
        void bring_to_front(uint32_t layer);
        void send_to_back(uint32_t layer);
        void send_backward(uint32_t layer);
        void bring_forward(uint32_t layer);
        //Current Serialization
    private:
        nullable<rb_string> serialize_current() const;
        void set_current_from_string(const nullable<rb_string>& str);
        //Naming
    private:
        void add_named_node(const rb_string& name, const node* n);
        void add_node_with_class(const rb_string& classes, const node* n);
        void remove_named_node(const node* n);
        void remove_node_with_class(const node* n);
    public:
        const node* node_with_name(const rb_string& name) const;
        node* node_with_name(const rb_string& name);
        std::vector<const node*> node_with_all_classes(const rb_string& classes) const;
        std::vector<node*> node_with_all_classes(const rb_string& classes);
        std::vector<const node*> node_with_one_class(const rb_string& classes) const;
        std::vector<node*> node_with_one_class(const rb_string& classes);
        //Centering Camera
    public:
        void center_camera_on_selection();
        //removing degenerated
    public:
        uint32_t remove_degenerated();
    };
}

#endif /* defined(__RollerBallCore__scene__) */
