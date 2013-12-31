//
//  node_container.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 08/11/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__node_container__
#define __RollerBallCore__node_container__

#include "components_base.h"
#include "transform_space.h"
#define BOOST_DISABLE_THREADS
#include <boost/pool/pool_alloc.hpp>
#include "typed_object.h"

namespace rb {
    class node;
    class scene;
    class layer;
    class group_component;
    class transform_gizmo;
    class node_container : public typed_object {
    public:
        typedef std::vector<node*, boost::pool_allocator<node*>> node_list;
        //friend classes:
    public:
        friend class node;
        friend class layer;
        friend class scene;
    private:
        std::unordered_set<node*> _selection;
        node_list _child_nodes;
        transform_space _transform;
        bool _dirty_transform;
        bool _in_editor_hidden;
        rb::nullable<bool> _selected;
        //Constructor
    public:
        node_container();
    protected:
        //deletion
        virtual void commit_suicide();
    public:
        bool add_node(node* n);
        const node* node_at(uint32_t at) const;
        node* node_at(uint32_t at);
        virtual bool add_node_at(node* n, uint32_t at);
        virtual bool remove_node(node* n, bool cleanup);
        bool remove_node_at(uint32_t at, bool cleanup);
        uint32_t node_count() const;
        node* first();
        const node* first() const;
        node* last();
        const node* last() const;
        nullable<uint32_t> search_node(const node* n) const;
        node_list::iterator begin();
        node_list::iterator end();
        node_list::const_iterator begin() const;
        node_list::const_iterator end() const;
        //reordering functions
        virtual bool bring_to_front(node* n);
        virtual bool send_to_back(node* n);
        virtual bool send_backward(node* n);
        virtual bool bring_forward(node* n);
        virtual ~node_container();
        void copy_nodes_to_vector(std::vector<node*>& nodes, const node_filter filter = node_filter::all) const;
        //transform
    private:
        void clear_dirty_transform();
    protected:
        virtual const scene* parent_scene() const = 0;
        virtual const layer* parent_layer() const = 0;
        virtual const node_container* parent_node_container() const = 0;
    public:
        const node_container* parent() const;
        node_container* parent();
    private:
        transform_space from_space_to(const space another) const;
        bool has_space_changed() const;
    public:
        virtual const transform_space& transform() const;
        virtual const transform_space& transform(const rb::transform_space &value);
        //current
    private:
        void internal_became_current();
        void internal_resign_current();
    protected:
        virtual void became_current();
        virtual void resign_current();
    public:
        bool is_current() const;
        //selection
        void add_to_selection(node* n);
        void remove_from_selection(node* n);
    private:
        std::unordered_set<node*>::iterator internal_remove_from_selection(std::unordered_set<node*>::iterator& pos);
    public:
        void clear_selection();
        void clear_selection_container();
        void fill_with_selection(std::vector<node*>& selection, const node_filter filter = node_filter::all, bool in_order = false) const;
        uint32_t selection_count(const node_filter filter = node_filter::all) const;
        bool is_selected(const node* n) const;
        virtual bool renderable() const;
    public:
        group_component* group_nodes(const std::vector<node*>& nodes);
        group_component* group_selected();
        //Transformations by the user...
    public:
        virtual std::vector<rb_string> transformables();
        virtual void start_transformation(long index);
        bool adjust_transformation(const rb::transform_space &transform);
    public:
        virtual typed_object* clone() const override;
    protected:
        virtual typed_object* clone_children(node_container* cloned) const;
    protected:
        virtual void fill_vector_with_children(std::vector<typed_object*>& children) const override;
        virtual void set_children(const std::vector<typed_object*>& children) override;
    //Alignment
    private:
        std::vector<rectangle> calc_bounds(const std::vector<node*>& nodes) const;
    public:
        virtual void hor_align_by_center(const std::vector<node*>& nodes);
        virtual void hor_align_by_left_edge(const std::vector<node*>& nodes);
        virtual void hor_align_by_right_edge(const std::vector<node*>& nodes);
        
        virtual void ver_align_by_center(const std::vector<node*>& nodes);
        virtual void ver_align_by_top_edge(const std::vector<node*>& nodes);
        virtual void ver_align_by_bottom_edge(const std::vector<node*>& nodes);
    //Blocking
    public:
        void unblock_all_children();
    //In-Editor Hidden
    public:
        virtual bool in_editor_hidden() const;
        virtual bool in_editor_hidden(const bool value);
        bool is_hidden_in_editor() const;
        void unhide_all_children();
    protected:
        //Typed Object
        virtual void describe_type() override;
        //serialization
    protected:
        virtual void was_deserialized() override;
        //Degeneration
    public:
        virtual bool is_degenerated() const;
    private:
        void test_degeneration(std::vector<node*>& nodes);
    };
}

#endif /* defined(__RollerBallCore__node_container__) */
