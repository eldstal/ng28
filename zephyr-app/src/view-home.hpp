#pragma once

#include "view.hpp"

class HomeView : public View {
    public:
    virtual void event(const ViewEvent& ev, ViewResponse& resp);
};
