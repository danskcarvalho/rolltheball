//
//  scene_helpers.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-04-19.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#include "scene.h"
#include "node.h"
#include "polygon.h"
#include "polygon_path.h"
#include "polygon_component.h"
#include "smooth_curve.h"

using namespace rb;

void distribute_horizontally(std::vector<node*>& nodes, const rb::distribution_info& di){
    if(nodes.size() <= 1)
        return;
    float _min_x = std::numeric_limits<float>::max();
    float _max_x = std::numeric_limits<float>::min();
    for (auto _n : nodes){
        if(_n->transform().origin().x() < _min_x)
            _min_x = _n->transform().origin().x();
        if(_n->transform().origin().x() > _max_x)
            _max_x = _n->transform().origin().x();
    }
    std::sort(nodes.begin(), nodes.end(), [](node* n1, node* n2){
        return n1->transform().origin().x() < n2->transform().origin().x();
    });
    auto _diff_size = (di.max_size - di.min_size);
    _diff_size /= (float)(nodes.size() - 1);
    auto _init_size = di.min_size;
    
    if(di.space.has_value()){
        for (auto _n : nodes){
            if(di.change_size)
                _n->transform(_n->transform().moved(_min_x, _n->transform().origin().y()).scaled(_init_size));
            else
                _n->transform(_n->transform().moved(_min_x, _n->transform().origin().y()));
            _min_x += di.space.value();
            _init_size += _diff_size;
            if(di.clamp && _min_x > _max_x)
                return;
        }
    }
    else {
        if(nodes.size() <= 2)
            return;
        auto _space = fabsf(_max_x - _min_x);
        _space /= (float)(nodes.size() - 1);
        for (auto _n : nodes){
            if(di.change_size)
                _n->transform(_n->transform().moved(_min_x, _n->transform().origin().y()).scaled(_init_size));
            else
                _n->transform(_n->transform().moved(_min_x, _n->transform().origin().y()));
            _init_size += _diff_size;
            _min_x += _space;
        }
    }
}

void distribute_vertically(std::vector<node*>& nodes, const rb::distribution_info& di){
    if(nodes.size() <= 1)
        return;
    float _min_y = std::numeric_limits<float>::max();
    float _max_y = std::numeric_limits<float>::min();
    for (auto _n : nodes){
        if(_n->transform().origin().y() < _min_y)
            _min_y = _n->transform().origin().y();
        if(_n->transform().origin().y() > _max_y)
            _max_y = _n->transform().origin().y();
    }
    std::sort(nodes.begin(), nodes.end(), [](node* n1, node* n2){
        return n1->transform().origin().y() < n2->transform().origin().y();
    });
    auto _diff_size = (di.max_size - di.min_size);
    _diff_size /= (float)(nodes.size() - 1);
    auto _init_size = di.min_size;
    
    if(di.space.has_value()){
        for (auto _n : nodes){
            if(di.change_size)
                _n->transform(_n->transform().moved(_n->transform().origin().x(), _min_y).scaled(_init_size));
            else
                _n->transform(_n->transform().moved(_n->transform().origin().x(), _min_y));
            _min_y += di.space.value();
            _init_size += _diff_size;
            if(di.clamp && _min_y > _max_y)
                return;
        }
    }
    else {
        if(nodes.size() <= 2)
            return;
        auto _space = fabsf(_max_y - _min_y);
        _space /= (float)(nodes.size() - 1);
        for (auto _n : nodes){
            if(di.change_size)
                _n->transform(_n->transform().moved(_n->transform().origin().x(), _min_y).scaled(_init_size));
            else
                _n->transform(_n->transform().moved(_n->transform().origin().x(), _min_y));
            _init_size += _diff_size;
            _min_y += _space;
        }
    }
}

static polygon reconstruct_polygon(rb::polygon_component *pol_component, std::vector<vec2>& pts, const transform_space& ts){
    polygon _polygon;
    _polygon = pol_component->opened() ? polygon::build_open_polygon(pts, _polygon) : polygon::build_closed_polygon(pts, _polygon);
    ts.from_space_to_base().transform_polygon(_polygon);
    
    if (pol_component->smooth() && pts.size() >= 4){
        smooth_curve _sc;
        if(pol_component->opened())
            smooth_curve::build_open_curve(pts, _sc);
        else
            smooth_curve::build_closed_curve(pts, _sc);
        
        polygon _f_pol;
        _sc.to_polygon(_f_pol, pol_component->smooth_quality(), pol_component->smooth_divisions());
        ts.from_space_to_base().transform_polygon(_f_pol);
        return _f_pol;
    }
    else
        return _polygon;
}

void distribute_along_path(std::vector<node*>& nodes, polygon_component* pc, const rb::distribution_info& di){
    if(nodes.size() == 0)
        return;
    //1. reconstruct polygon...
    //get all the point's current situations
    std::vector<vec2> _pt_pos;
    for(auto _p : *pc)
        _pt_pos.push_back(_p->transform().origin());
    //get the current transformation of polygon
    auto _polygon = reconstruct_polygon(pc, _pt_pos, pc->transform());
    //2. do a length mapping
    polygon_path _path = polygon_path(_polygon);
    //size
    auto _diff_size = (di.max_size - di.min_size);
    _diff_size /= (float)(nodes.size() - 1);
    auto _init_size = di.min_size;
    
    auto _space = di.space.has_value() ? di.space.value() : _path.length() / ((float)nodes.size() - 1);
    auto _len = 0.0f;
    for (auto _n : nodes){
        auto _pt = _path.point_at(_len, true);
        if(di.change_size)
            _n->transform(_n->transform().moved(_pt).scaled(_init_size));
        else
            _n->transform(_n->transform().moved(_pt));
        _len += _space;
        _init_size += _diff_size;
        if(_len > _path.length())
            return;
    }
}

void scene::distribute(const rb::distribution_info &di){
    std::vector<node*> _sel;
    if(!current()){
        alert(u"No current node selected!");
        return;
    }
    current()->fill_with_selection(_sel, node_filter::renderable);
    if(_sel.size() == 0){
        alert(u"No nodes selected!");
        return;
    }
    
    if(di.type == distribution_type::horizontally)
        distribute_horizontally(_sel, di);
    else if(di.type == distribution_type::vertically)
        distribute_vertically(_sel, di);
    else
    {
        std::vector<node*> _nodes;
        polygon_component* _pc = nullptr;
        for (auto _n : _sel){
            auto _temp_pc = dynamic_cast<polygon_component*>(_n);
            if(_temp_pc && _pc){
                alert(u"Two polygons selected!");
                return;
            }
            if(_temp_pc)
                _pc = _temp_pc;
            else
                _nodes.push_back(_n);
        }
        if(!_pc){
            alert(u"No polygon selected!");
            return;
        }
        distribute_along_path(_nodes, _pc, di);
    }
}

void scene::perform_action_with_class(const rb_string &cls, rb::action_type act){
    auto _nodes = node_with_one_class(cls);
    if(act == action_type::add_to_selection){
        for(auto _n : _nodes){
            if(_n->in_editor_hidden() || _n->blocked())
                continue;
            _n->add_to_selection();
        }
    }
    else if(act == action_type::block){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->blocked(true);
        }
    }
    else if(act == action_type::hide){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->in_editor_hidden(true);
        }
    }
    else if(act == action_type::select){
        if(current())
            current()->clear_selection();
        for(auto _n : _nodes){
            if(_n->in_editor_hidden() || _n->blocked())
                continue;
            _n->add_to_selection();
        }
    }
    else if(act == action_type::unblock){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->blocked(false);
        }
    }
    else if(act == action_type::unhide){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->in_editor_hidden(false);
        }
    }
    else if(act == action_type::unselect){
        for(auto _n : _nodes){
            if(_n->in_editor_hidden() || _n->blocked())
                continue;
            _n->remove_from_selection();
        }
    }
    else if(act == action_type::remove){
        for(auto _n : _nodes){
            _n->parent()->remove_node(_n, true);
        }
    }
    else {
        assert(false); //Never should be here...
    }
}

void scene::perform_action_with_id(const rb_string &idd, rb::action_type act){
    std::vector<node*> _nodes;
    auto _nn = node_with_name(idd);
    if (_nn)
        _nodes.push_back(_nn);
    if(act == action_type::add_to_selection){
        for(auto _n : _nodes){
            if(_n->in_editor_hidden() || _n->blocked())
                continue;
            _n->add_to_selection();
        }
    }
    else if(act == action_type::block){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->blocked(true);
        }
    }
    else if(act == action_type::hide){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->in_editor_hidden(true);
        }
    }
    else if(act == action_type::select){
        if(current())
            current()->clear_selection();
        for(auto _n : _nodes){
            if(_n->in_editor_hidden() || _n->blocked())
                continue;
            _n->add_to_selection();
        }
    }
    else if(act == action_type::unblock){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->blocked(false);
        }
    }
    else if(act == action_type::unhide){
        for(auto _n : _nodes){
            if(!_n->renderable())
                continue;
            _n->in_editor_hidden(false);
        }
    }
    else if(act == action_type::unselect){
        for(auto _n : _nodes){
            if(_n->in_editor_hidden() || _n->blocked())
                continue;
            _n->remove_from_selection();
        }
    }
    else if(act == action_type::remove){
        for(auto _n : _nodes){
            _n->parent()->remove_node(_n, true);
        }
    }
    else {
        assert(false); //Never should be here...
    }
}

void scene::join_selected(){
    if(!current()){
        alert(u"No current!");
        return;
    }
    std::vector<node*> _nodes;
    current()->fill_with_selection(_nodes, node_filter::renderable);
    if(_nodes.size() != 2){
        alert(u"Must select two nodes!");
        return;
    }
    node* _target = nullptr;
    node* _change = nullptr;
    if(_nodes[0]->has_class(u"@target")){
        _target = _nodes[0];
        _change = _nodes[1];
    }
    else if(_nodes[1]->has_class(u"@target")){
        _target = _nodes[1];
        _change = _nodes[0];
    }
    else {
        alert(u"No target!");
        return;
    }
    
    _change->transform(_change->transform().moved(_target->transform().origin()));
    //remove target class
    auto _classes = rb::tokenize(_target->classes());
    rb_string _strClass = u"";
    for (size_t i = 0; i < _classes.size(); i++){
        if(_classes[i] == u"@target")
            continue;
        if(i != 0)
            _strClass += u" ";
        
        _strClass += _classes[i];
    }
    _target->classes(_strClass);
}




















