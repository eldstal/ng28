#pragma once

#include "view.hpp"

class HomeView : public View {
    private:
    void button(int btn, ViewResponse& resp);
    void draw();

    public:
    virtual void event(const ViewEvent& ev, ViewResponse& resp);
};
