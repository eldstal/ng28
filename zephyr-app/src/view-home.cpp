#include "view-home.hpp"

#include "display.hpp"

#define evkind ViewEvent::Kind
#define reskind ViewResponse::Kind

void HomeView::button(int btn, ViewResponse& resp) {
    resp.kind = reskind::NOP;
}

void HomeView::draw() {
    uint16_t w, h;
    display_size(w, h);

    display_fill(false);

    for (int x=0; x<w; x++) {
        display_px(x, 0, true);
        display_px(x, h-1, true);
    }

    for (int y=0; y<h; y++) {
        display_px(0,   y, true);
        display_px(w-1, y, true);
    }
}

void HomeView::event(const ViewEvent& ev, ViewResponse& resp) {

    resp.kind = reskind::NOP;

    switch (ev.kind) {

        case evkind::TERMINATE:
            // This should never happen to the home view, but OK.
            return;

        case evkind::BTN_RELEASE:
            button(ev.btn, resp);
            break;

        case evkind::REDRAW:
            draw();
            break;
        
        default:
            break;
    }
}

