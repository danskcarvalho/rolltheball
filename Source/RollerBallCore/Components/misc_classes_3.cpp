//
//  misc_classes_3.cpp
//  RollerBallCore
//
//  Created by Danilo Carvalho on 04/08/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#include "misc_classes_3.h"
#include "generic_object.h"

using namespace rb;

node_without_transform::node_without_transform(class typed_object* n){
    _node = n;
}

node_without_transform::~node_without_transform(){
    if(_node)
        delete _node;
}

class typed_object* node_without_transform::node() {
    return _node;
}

void node_without_transform::describe_type() {
    std::vector<rb_string> _properties;
    _node->type_descriptor()->get_all_properties(_properties);
    
    start_type<node_without_transform>([](){ return nullptr; });
    for (auto& _p : _properties){
        if(_node->type_descriptor()->property_private(_p))
            continue;
        add_property(_node->type_descriptor(), _p);
    }
    end_type();
}

rb_string node_without_transform::type_name() const {
    return _node->type_name() + u"::@5710nwt";
}

rb_string node_without_transform::displayable_type_name() const {
    return _node->displayable_type_name();
}

void node_without_transform::add_nullable_property(const class type_descriptor *td, const rb_string &p){
    if(p == u"rb::transformation")
        return; //No transformation
    
    auto _gtd = td->property_type(p)->gtd();
    if(_gtd == general_type_descriptor::angle){
        nullable_angle_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), td->property_precision(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_single_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<float> value){
                site->_node->type_descriptor()->set_nullable_single_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::boolean){
        nullable_boolean_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_boolean_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<bool> value){
                site->_node->type_descriptor()->set_nullable_boolean_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::color){
        nullable_color_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_color_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<color> value){
                site->_node->type_descriptor()->set_nullable_color_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::enumeration){
        nullable_enumeration_property<node_without_transform, long>(p, td->property_display_name(p), td->property_type(p)->enumeration_values(), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_integer_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<long> value){
                site->_node->type_descriptor()->set_nullable_integer_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::image){
        nullable_image_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_string_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<rb_string> value){
                site->_node->type_descriptor()->set_nullable_string_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::integer){
        nullable_integer_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_integer_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<long> value){
                site->_node->type_descriptor()->set_nullable_integer_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::ranged){
        nullable_ranged_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), td->property_precision(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_single_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<float> value){
                site->_node->type_descriptor()->set_nullable_single_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::single){
        nullable_single_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_single_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<float> value){
                site->_node->type_descriptor()->set_nullable_single_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::string){
        nullable_string_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_string_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<rb_string> value){
                site->_node->type_descriptor()->set_nullable_string_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::vec2){
        nullable_vec2_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_vec2_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<vec2> value){
                site->_node->type_descriptor()->set_nullable_vec2_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::vector){
        vector_property<node_without_transform>(p, td->property_display_name(p), td->property_type(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_vector_property<typed_object>(site->_node, p);
            },
            [=](node_without_transform* site, const std::vector<typed_object*>& value){
                site->_node->type_descriptor()->set_vector_property<typed_object>(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::string_with_autocompletion){
        nullable_string_with_autocompletion_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_string_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<rb_string> value){
                site->_node->type_descriptor()->set_nullable_string_property(site->_node, p, value);
            }
        }, [=](const node_without_transform* site){
            return site->_node->type_descriptor()->get_autocomplete_suggestions(site->_node, p);
        });
    }
    else if(_gtd == general_type_descriptor::flags){
        nullable_flags_property<node_without_transform, long>(p, td->property_display_name(p), td->property_type(p)->flag_names(), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_nullable_integer_property(site->_node, p);
            },
            [=](node_without_transform* site, const nullable<long> value){
                site->_node->type_descriptor()->set_nullable_integer_property(site->_node, p, value);
            }
        });
    }
    else { //branch or object
        nullable_object_property<node_without_transform>(p, td->property_display_name(p), td->property_type(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_object_property<typed_object>(site->_node, p);
            },
            [=](node_without_transform* site, typed_object* value){
                site->_node->type_descriptor()->set_object_property<typed_object>(site->_node, p, value);
            }
        });
    }
}

void node_without_transform::add_non_nullable_property(const class type_descriptor *td, const rb_string &p){
    if(p == u"rb::transformation")
        return; //No transformation
    
    auto _gtd = td->property_type(p)->gtd();
    if(_gtd == general_type_descriptor::angle){
        angle_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), td->property_precision(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_single_property(site->_node, p);
            },
            [=](node_without_transform* site, const float value){
                site->_node->type_descriptor()->set_single_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::boolean){
        boolean_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_boolean_property(site->_node, p);
            },
            [=](node_without_transform* site, const bool value){
                site->_node->type_descriptor()->set_boolean_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::color){
        color_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_color_property(site->_node, p);
            },
            [=](node_without_transform* site, const color value){
                site->_node->type_descriptor()->set_color_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::enumeration){
        enumeration_property<node_without_transform, long>(p, td->property_display_name(p), td->property_type(p)->enumeration_values(), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_integer_property(site->_node, p);
            },
            [=](node_without_transform* site, const long value){
                site->_node->type_descriptor()->set_integer_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::image){
        image_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_string_property(site->_node, p);
            },
            [=](node_without_transform* site, const rb_string value){
                site->_node->type_descriptor()->set_string_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::integer){
        integer_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_integer_property(site->_node, p);
            },
            [=](node_without_transform* site, const long value){
                site->_node->type_descriptor()->set_integer_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::ranged){
        ranged_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), td->property_precision(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_single_property(site->_node, p);
            },
            [=](node_without_transform* site, const float value){
                site->_node->type_descriptor()->set_single_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::single){
        single_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_single_property(site->_node, p);
            },
            [=](node_without_transform* site, const float value){
                site->_node->type_descriptor()->set_single_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::string){
        string_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), td->property_continuous(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_string_property(site->_node, p);
            },
            [=](node_without_transform* site, const rb_string value){
                site->_node->type_descriptor()->set_string_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::vec2){
        vec2_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_vec2_property(site->_node, p);
            },
            [=](node_without_transform* site, const vec2 value){
                site->_node->type_descriptor()->set_vec2_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::vector){
        vector_property<node_without_transform>(p, td->property_display_name(p), td->property_type(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_vector_property<typed_object>(site->_node, p);
            },
            [=](node_without_transform* site, const std::vector<typed_object*>& value){
                site->_node->type_descriptor()->set_vector_property<typed_object>(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::string_with_autocompletion){
        string_with_autocompletion_property<node_without_transform>(p, td->property_display_name(p), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_string_property(site->_node, p);
            },
            [=](node_without_transform* site, const rb_string value){
                site->_node->type_descriptor()->set_string_property(site->_node, p, value);
            }
        }, [=](const node_without_transform* site){
            return site->_node->type_descriptor()->get_autocomplete_suggestions(site->_node, p);
        });
    }
    else if(_gtd == general_type_descriptor::flags){
        flags_property<node_without_transform, long>(p, td->property_display_name(p), td->property_type(p)->flag_names(), td->property_editable(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_integer_property(site->_node, p);
            },
            [=](node_without_transform* site, const long value){
                site->_node->type_descriptor()->set_integer_property(site->_node, p, value);
            }
        });
    }
    else if(_gtd == general_type_descriptor::action){
        action<node_without_transform>(p, td->property_display_name(p), td->property_type(p)->action_flags(), td->property_type(p)->action_names(),
                                       [=](node_without_transform* site, const rb_string& action_name){
                                           site->_node->type_descriptor()->call_action(site->_node, p, action_name);
                                       });
    }
    else { //branch or object
        object_property<node_without_transform>(p, td->property_display_name(p), td->property_type(p), td->property_inlined(p), {
            [=](const node_without_transform* site){
                return site->_node->type_descriptor()->get_object_property<typed_object>(site->_node, p);
            },
            [=](node_without_transform* site, typed_object* value){
                site->_node->type_descriptor()->set_object_property<typed_object>(site->_node, p, value);
            }
        });
    }
}

void node_without_transform::add_property(const class type_descriptor *td, const rb_string &p){
    if(td->property_type(p)->nullable())
        add_nullable_property(td, p);
    else
        add_non_nullable_property(td, p);
}