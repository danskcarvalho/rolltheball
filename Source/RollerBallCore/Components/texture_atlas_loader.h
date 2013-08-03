//
//  texture_atlas_loader.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 26/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__texture_atlas_loader__
#define __RollerBallCore__texture_atlas_loader__

#include "components_base.h"

namespace rb {
    class texture_atlas;
    class texture_atlas_loader {
    private:
        static uint32_t _max_unused_atlases;
        static std::unordered_map<texture_atlas*, rb_string> _inv_map;
        static std::unordered_map<rb_string, texture_atlas*> _map;
        static std::unordered_map<rb_string, texture_atlas*> _to_be_deleted;
        static std::unordered_map<rb_string, uint32_t> _ref_count;
        static texture_atlas* _editor_atlas;
    private:
        static void release_atlases();
    public:
        static uint32_t max_unused_atlases();
        static uint32_t max_unused_atlases(const uint32_t value);
        static texture_atlas* load_atlas(const rb_string& url);
        static void release_atlas(texture_atlas** atlas);
    };
}

#endif /* defined(__RollerBallCore__texture_atlas_loader__) */
