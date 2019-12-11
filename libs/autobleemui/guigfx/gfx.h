//
// Created by screemer on 2019-12-11.
//

#pragma once


#include <memory>

class Gfx {
public:
    Gfx(Gfx const &) = delete;
    Gfx &operator=(Gfx const &) = delete;

    static std::shared_ptr<Gfx> getInstance() {
        static std::shared_ptr<Gfx> gfx{new Gfx};
        return gfx;
    }

    static void flip();
private:
    Gfx( ){};


};



