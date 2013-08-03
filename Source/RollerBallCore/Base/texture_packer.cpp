//
//  texture_packer.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 11/09/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#include "texture_packer.h"
#include <tuple>

using namespace rb;


//the coordinates of texture packer assumes that the y-axis grows in the downward direction...
namespace rb {
    struct table_cell;
    struct table_cell_manager {
    private:
        std::vector<table_cell*> _allocations;
    public:
        table_cell* alloc();
        void dealloc(table_cell* cell);
        ~table_cell_manager();
    };
    
    struct rectangle_area_comp {
    public:
        bool operator()(const std::tuple<uint32_t, vec2> &a, const std::tuple<uint32_t, vec2> &b){
            auto _v1 = std::get<1>(a);
            auto _v2 = std::get<1>(b);
            
            return (_v1.x() * _v1.y()) < (_v2.x() * _v2.y());
        }
    };
    
    struct table_cell {
        static const uint32_t padding_index;
        
        table_cell* left_cell;
        table_cell* right_cell;
        table_cell* upper_cell;
        table_cell* bottom_cell;
        table_cell_manager* manager;
        
        float width, height;
        nullable<uint32_t> texture;
        
        table_cell(){
            left_cell = nullptr;
            right_cell = nullptr;
            upper_cell = nullptr;
            bottom_cell = nullptr;
            width = height = 0;
            texture = nullptr;
        }
        
        bool is_free(){
            return !texture.has_value();
        }
        
        bool is_boundary(){
            return left_cell == nullptr || right_cell == nullptr || upper_cell == nullptr || bottom_cell == nullptr;
        }
        
        bool is_front_corner(){
            if(!is_free())
                return false; //i must be free...
            
            if(left_cell != nullptr && left_cell->is_free())
                return false;
            if(upper_cell != nullptr && upper_cell->is_free())
                return false;
            if(right_cell != nullptr && !right_cell->is_free())
                return false;
            if(bottom_cell != nullptr && !bottom_cell->is_free())
                return false;
            
            return true;
        }
        
        //subdivides the cell vertically...
        //gets the previous upper right cell and
        //returns the new right cell...
        table_cell* vertical_subdivide(float width_preservation, table_cell* upper_right_cell){
            if(width_preservation >= this->width)
                return nullptr;
            class table_cell* _new_right = manager->alloc();
            _new_right->manager = manager;
            //we shift current right content...
            _new_right->right_cell = right_cell;
            _new_right->left_cell = this;
            _new_right->upper_cell = upper_right_cell;
            
            if(bottom_cell != nullptr)
                _new_right->bottom_cell = bottom_cell->vertical_subdivide(width_preservation, _new_right);
            else
                _new_right->bottom_cell = nullptr;
            
            //set my original right cell...
            if(right_cell)
                right_cell->left_cell = _new_right;
            //set myself...
            right_cell = _new_right;
            //set other attributes
            _new_right->width = width - width_preservation;
            _new_right->height = height;
            _new_right->texture = texture;
            width = width_preservation;
            return _new_right;
        }
        
        void vertical_merge(){
            auto _new_width = width + right_cell->width;
            //points back to myself...
            if (right_cell->right_cell)
                right_cell->right_cell->left_cell = this;
            
            auto _previous = right_cell;
            right_cell = right_cell->right_cell;
            width = _new_width;
            manager->dealloc(_previous);
            
            if(bottom_cell)
                bottom_cell->vertical_merge();
        }
        
        //subdivides the cell horizontally...
        //gets the previous left bottom cell and
        //returns the new bottom cell...
        table_cell* horizontal_subdivide(float height_preservation, table_cell* left_bottom_cell){
            if(height_preservation >= this->height)
                return nullptr;
            class table_cell* _new_bottom = manager->alloc();
            _new_bottom->manager = manager;
            //we shift current bottom content...
            _new_bottom->bottom_cell = bottom_cell;
            _new_bottom->upper_cell = this;
            _new_bottom->left_cell = left_bottom_cell;
            
            if(right_cell != nullptr)
                _new_bottom->right_cell = right_cell->horizontal_subdivide(height_preservation, _new_bottom);
            else
                _new_bottom->right_cell = nullptr;
            
            //set my original bottom cell...
            if(bottom_cell)
                bottom_cell->upper_cell = _new_bottom;
            //set myself...
            bottom_cell = _new_bottom;
            //set other attributes
            _new_bottom->height = height - height_preservation;
            _new_bottom->width = width;
            _new_bottom->texture = texture;
            height = height_preservation;
            return _new_bottom;
        }
        
        void horizontal_merge(){
            auto _new_height = height + bottom_cell->height;
            //points back to myself...
            if (bottom_cell->bottom_cell)
                bottom_cell->bottom_cell->upper_cell = this;
            
            auto _previous = bottom_cell;
            bottom_cell = bottom_cell->bottom_cell;
            height = _new_height;
            manager->dealloc(_previous); //the manager may remove this cell from the front corner cells list...
            
            if(right_cell)
                right_cell->horizontal_merge();
        }
        
        static void extend_area(vec2& extended_area, const vec2& filled_area, const vec2& new_position, const vec2& new_size){
            float _new_width = std::max(filled_area.x(), new_position.x() + new_size.x());
            float _new_height = std::max(filled_area.y(), new_position.y() + new_size.y());
            extended_area.x(_new_width);
            extended_area.y(_new_height);
        }
        
        //this function returns the most appropriate front corner cell.
        //this is the cell that minimizes the area growth.
        static void rank_front_corner_cells(const vec2& filled_area,
                                     const vec2& area_to_fill,
                                     const vec2& max_size,
                                     const std::vector<table_cell*>& front_corners,
                                     std::unordered_map<table_cell*, vec2>& cell_positions,
                                     std::vector<table_cell*>& _best_cells){
            std::map<float, table_cell*> _sorted_map;
            vec2 _extended_area;
            
            for(auto _cell : front_corners){
                extend_area(_extended_area, filled_area, cell_positions[_cell], area_to_fill);
                if(_extended_area.x() > max_size.x() || _extended_area.y() > max_size.y())
                    continue; //we can't choose this front corner cell...
                _sorted_map.insert({{_extended_area.x() * _extended_area.y(), _cell}});
            }
            
            if(_sorted_map.size() == 0)
                _best_cells.clear();
            else
            {
                _best_cells.clear();
                for(auto _key : _sorted_map){
                    _best_cells.push_back(_key.second);
                }
            }
        }
        
        table_cell* get_upper_boundary(){
            if(this->upper_cell == nullptr)
                return this;
            else
                return this->upper_cell->get_upper_boundary();
        }
        
        table_cell* get_left_boundary(){
            if(this->left_cell == nullptr)
                return this;
            else
                return this->left_cell->get_left_boundary();
        }
        
        table_cell* until_width(float& total_width, float width_sum, const float width)
        {
            total_width = width_sum + this->width;
            if(total_width >= width)
                return this;
            else //we assume there is space... so we don't check for null...
                return this->right_cell->until_width(total_width, total_width, width);
        }
        
        table_cell* until_height(float& total_height, float height_sum, const float height)
        {
            total_height = height_sum + this->height;
            if(total_height >= height)
                return this;
            else //we assume there is space... so we don't check for null...
                return this->bottom_cell->until_height(total_height, total_height, height);
        }
        
        static bool test_possible_front_corner(table_cell* cell, std::unordered_set<table_cell*>& filled){
            bool    _filled_up = cell->upper_cell == nullptr || !cell->upper_cell->is_free() || filled.count(cell->upper_cell) >= 1,
                    _filled_bottom = cell->bottom_cell == nullptr || !cell->bottom_cell->is_free() || filled.count(cell->bottom_cell) >= 1,
                    _filled_left = cell->left_cell == nullptr || !cell->left_cell->is_free() || filled.count(cell->left_cell) >= 1,
                    _filled_right = cell->right_cell == nullptr || !cell->right_cell->is_free() || filled.count(cell->right_cell) >= 1,
                    _filled_self = !cell->is_free() || filled.count(cell) >= 1;
            
            if(_filled_self)
                return false;
            
            if(!_filled_left)
                return false;
            if(!_filled_up)
                return false;
            if(_filled_bottom)
                return false;
            if(_filled_right)
                return false;
            
            return true;
        }
        
        void add_padding(table_cell* cell, std::unordered_set<table_cell*>& padding_cells, std::unordered_set<table_cell*>& filled){
            table_cell* _current = cell->left_cell;
            bool _filled = _current == nullptr || !_current->is_free() || filled.count(_current) >= 1;
            
            //add padding to left...
            while (!_filled) {
                padding_cells.insert(_current);
                _current = _current->left_cell;
                _filled = _current == nullptr || !_current->is_free() || filled.count(_current) >= 1;
            }
            
            //add padding to up...
            _current = cell->upper_cell;
            _filled = _current == nullptr || !_current->is_free() || filled.count(_current) >= 1;
            
            while (!_filled) {
                padding_cells.insert(_current);
                _current = _current->upper_cell;
                _filled = _current == nullptr || !_current->is_free() || filled.count(_current) >= 1;
            }
        }
        
        float calc_padding_area(std::unordered_set<table_cell*>& padding_cells){
            float _area_sum = 0;
            
            for(auto _cell : padding_cells){
                _area_sum += _cell->width * _cell->height;
            }
            return _area_sum;
        }
        
        //we assume we are front corner cell...
        //we assume there is available space...
        bool try_fill(const vec2& area_to_fill, const uint32_t texture_index, const float max_padding){
            assert(is_front_corner()); //just for debugging purposes...
            
            std::unordered_set<table_cell*> _filled_cells;
            std::unordered_set<table_cell*> _padding_cells;
            
            //1. we subdivide to get the area...
            float total_width, total_height;
            table_cell* _r_corner = until_width(total_width, 0, area_to_fill.x());
            table_cell* _b_corner = until_height(total_height, 0, area_to_fill.y());
            table_cell* _r_bound = _r_corner->get_upper_boundary();
            table_cell* _b_bound = _b_corner->get_left_boundary();
            
            _r_bound->vertical_subdivide(_r_bound->width - (total_width - area_to_fill.x()), nullptr);
            _b_bound->horizontal_subdivide(_b_bound->height - (total_height - area_to_fill.y()), nullptr);
            
            //next we determine if we can fill all the cells...
            table_cell* _current = this;
            table_cell* _current_row = this;
            float _sum_width = 0, _sum_height = 0;
            while (_sum_height < area_to_fill.y()) {
                //we test if we can fill the current cell...
                if(test_possible_front_corner(_current, _filled_cells)){
                    _filled_cells.insert(_current);
                    _sum_width += _current->width;
                    _current = _current->right_cell;
                }
                else {
                    //add padding
                    add_padding(_current, _padding_cells, _filled_cells);
                    _filled_cells.insert(_current); //we fill anyway...
                    _sum_width += _current->width;
                    _current = _current->right_cell;
                }
                
                if(_sum_width >= area_to_fill.x()){
                    _sum_height += _current_row->height;
                    _current = _current_row->bottom_cell;
                    _current_row = _current;
                    _sum_width = 0;
                }
            }
            
            //at this point we have all the filled_cells and paddings...
            auto _padding_area = calc_padding_area(_padding_cells);
            if (_padding_area > max_padding) //can't fill
            {
                //we reverse the subdivisions
                _b_bound->horizontal_merge();
                _r_bound->vertical_merge();
                return false;
            }
            
            //we store the changes
            for(auto _cell : _filled_cells){
                _cell->texture = texture_index;
            }
            for(auto _cell : _padding_cells){
                _cell->texture = padding_index;
            }
            
            return true;
        }
        
        static bool try_pack(vec2& filled_area,
                             const vec2& area_to_fill,
                             vec2& filled_position,
                             const vec2& max_size,
                             table_cell* upper_leftmost_cell,
                             std::vector<table_cell*>& front_corners,
                             std::unordered_map<table_cell*, vec2>& cell_positions,
                             const uint32_t texture_index, const float max_padding){
            
            std::vector<table_cell*> _best_cells;
            rank_front_corner_cells(filled_area, area_to_fill, max_size, front_corners, cell_positions, _best_cells);
            
            if(_best_cells.size() == 0)
                return false;
            
            for(auto _cell : _best_cells){
                if(_cell->try_fill(area_to_fill, texture_index, max_padding)){ //filled
                    vec2 _extended_area;
                    extend_area(_extended_area, filled_area, cell_positions[_cell], area_to_fill);
                    filled_area = _extended_area; //copied to filled_area...
                    filled_position = cell_positions[_cell]; //the cell position for _cell is still valid since subdivisions will always occur to the right and bellow _cell...
                    
                    //we update front corners and cell positions
                    front_corners.clear();
                    cell_positions.clear();
                    
                    table_cell* _current = upper_leftmost_cell;
                    table_cell* _current_row = upper_leftmost_cell;
                    
                    float _sum_width = 0, _sum_height = 0;
                    while (true) {
                        if(_current->is_front_corner())
                            front_corners.push_back(_current);
                        cell_positions.insert({{_current, vec2(_sum_width, _sum_height)}});
                        
                        _sum_width += _current->width;
                        _current = _current->right_cell;
                        if(_current == nullptr){
                            _sum_width = 0;
                            _sum_height += _current_row->height;
                            _current = _current_row->bottom_cell;
                            _current_row = _current;
                            
                            if(_current == nullptr)
                                break;
                        }
                    }
                    
                    //we then return
                    return true;
                }
            }
            
            return false;
        }
        
        
    };
}

table_cell* table_cell_manager::alloc(){
    _allocations.push_back(new table_cell());
    return _allocations[_allocations.size() - 1];
}

table_cell_manager::~table_cell_manager(){
    for (auto _cell : this->_allocations)
        delete _cell;
}

void table_cell_manager::dealloc(rb::table_cell *cell){
    //we currently do nothing...
}

const uint32_t table_cell::padding_index = 0xFFFFFFFF;

void get_texture_rectange(rectangle& rc, const float pos_x, const float pos_y, const float screen_height, const float width, const float height){
    //we don't use screen height...
    //float _new_y = screen_height - pos_y;
    rc = rectangle(pos_x + (width / 2), pos_y + (height / 2), width, height);
}

texture_packer::texture_packer(){
    _tex_destinations = std::vector<nullable<rectangle>>();
}
texture_packer texture_packer::pack_textures(const std::vector<vec2>& texture_sizes, const vec2& space_available, bool may_rotate){
    //we order the texture sizes from largest to tiniest...
    std::vector<std::tuple<uint32_t, vec2>> _sizes;
    for (int32_t i = 0; i < texture_sizes.size(); i++) {
        _sizes.push_back(std::make_tuple((uint32_t)i, texture_sizes[i]));
    }
    std::sort(_sizes.begin(), _sizes.end(), rectangle_area_comp());
    std::reverse(_sizes.begin(), _sizes.end());
    
    //we then make a list because we are going to manipulate the list a lot...
    std::list<std::tuple<uint32_t, vec2>> _sizes_list;
    for(auto _t : _sizes){
        _sizes_list.push_back(_t);
    }
    
    const float _max_padding = (space_available.x() * space_available.y()) * 0.025; //2,5% of the area...
    std::list<std::tuple<uint32_t, vec2>> _unpacked;
    
    //we initialize our table
    table_cell_manager _cell_manager;
    auto _primary_cell = _cell_manager.alloc();
    _primary_cell->manager = &_cell_manager;
    _primary_cell->width = space_available.x();
    _primary_cell->height = space_available.y(); //we fill the first cell with the space available...
    //due to the nature of subdivisions _primary_cell will always be the top leftmost cell...
    
    vec2 _filled_area = vec2::zero; //no filled area
    vec2 _filled_position;
    std::vector<table_cell *> _front_corners;
    std::unordered_map<table_cell*, vec2> _cell_positions;
    _front_corners.push_back(_primary_cell);
    _cell_positions.insert({{_primary_cell, vec2::zero}});
    
    //we initialize our destination vector
    texture_packer _packer;
    for (int32_t i = 0; i < texture_sizes.size(); ++i) {
        _packer._tex_destinations.push_back(nullptr);
    }
    
    while (true){
        auto _ts = _sizes_list.front();
        auto _index = std::get<0>(_ts);
        _sizes_list.pop_front();
        
        auto _current_padding = std::count(_unpacked.begin(), _unpacked.end(), _ts) >= 1 ? (space_available.x() * space_available.y()) :
            _max_padding;
        
        vec2 _tx_size = texture_sizes[_index];
        bool _packed = table_cell::try_pack(_filled_area, _tx_size, _filled_position, space_available, _primary_cell, _front_corners, _cell_positions, std::get<0>(_ts), _current_padding);
        
        if(!_packed && may_rotate){
            _tx_size = vec2(_tx_size.y(), _tx_size.x());
            _packed = table_cell::try_pack(_filled_area, _tx_size, _filled_position, space_available, _primary_cell, _front_corners, _cell_positions, std::get<0>(_ts), _current_padding);
        }
        
        if(_packed){
            //the we are done with this texture
            rectangle _tex_rc;
            get_texture_rectange(_tex_rc, _filled_position.x(), _filled_position.y(), space_available.y(), _tx_size.x(), _tx_size.y());
            _packer._tex_destinations[std::get<0>(_ts)] = _tex_rc;
            //we clear the unpacked list...
            _unpacked.clear();
        }
        else {
            //we move to the end of the list and add to the unpacked list
            _sizes_list.push_back(_ts);
            _unpacked.push_back(_ts);
        }
        
        if(_unpacked.size() == _sizes_list.size())
            break;
    }
    
    _packer._full_filled_area = _filled_area;
    
    return _packer;
}
const nullable<rectangle>& texture_packer::texture_rectangle(uint32_t at) const{
    assert(at >= 0);
    assert(at < _tex_destinations.size());
    return _tex_destinations[at];
}
bool texture_packer::has_been_packed(uint32_t at) const{
    assert(at >= 0);
    assert(at < _tex_destinations.size());
    return _tex_destinations[at].has_value();
}

const vec2& texture_packer::filled_area() const{
    return _full_filled_area;
}

uint32_t texture_packer::texture_count() const{
    return (uint32_t)_tex_destinations.size();
}