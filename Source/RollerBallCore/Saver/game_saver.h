//
//  game_server.h
//  RollerBallCore
//
//  Created by Danilo Carvalho on 2014-06-02.
//  Copyright (c) 2014 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__game_server__
#define __RollerBallCore__game_server__

namespace rb {
    struct saved_data_v1 {
        uint32_t version;
        uint32_t hearts;
        uint32_t set2Availability;
        uint32_t reserved[33];
    };
    class game_saver {
    public:
        static void load_saved(saved_data_v1* data);
        static void save(saved_data_v1* data);
    };
}

#endif /* defined(__RollerBallCore__game_server__) */
