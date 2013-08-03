//
//  xnode.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 18/10/12.
//  Copyright (c) 2012 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__xnode__
#define __RollerBallCore__xnode__

#include "base.h"

@class NSURL;
@class XMLWriter;

namespace rb {
    
    class xnode {
    private:
        std::unordered_map<rb_string, rb_string> _attributes;
        rb_string _name;
        std::vector<xnode> _children;
    public:
        bool has_attribute(const rb_string& attr_name) const;
        void get_attributes(std::vector<rb_string>& attributes) const;
        rb_string get_attribute_value(const rb_string& attr_name) const;
        void set_attribute_value(const rb_string& attr_name, const rb_string& attr_value);
        void clear_attributes();
        const rb_string& name() const;
        const rb_string& name(const rb_string& value);
        
        const std::vector<xnode>& children() const;
        std::vector<xnode>& children();
        xnode& get_children_by_tag_name(const rb_string& name);
        
        static xnode read_from_string(const rb_string& str);
        static xnode read_from_url(NSURL* url);
        void write_to_url(NSURL* url);
        rb_string to_string() const;
    private:
        void write_to_writer(XMLWriter* writer);
    };
}

#endif /* defined(__RollerBallCore__xnode__) */
