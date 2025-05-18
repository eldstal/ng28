#pragma once

#include <cstddef>

class View;

struct ViewEvent {
    enum Kind {
        // Button down
        BTN_PRESS,
        BTN_RELEASE,

        // This is a helpful reminder that you've just regained
        // focus after someone else has been touching the screen.
        // Redraw your entire UI once, please.
        REDRAW,

        // If you receive this, you've been asked to terminate
        // your view. No matter what you return, your view will
        // be delete'd when you do.
        TERMINATE
    };

    Kind kind = REDRAW;

    int btn = -1;
};

struct ViewResponse {

    enum Kind {
        // No change, you stay active
        NOP,

        // Terminate yourself and return to the parent view
        // You'll get one last event of TERMINATE to do your cleanup.
        TERMINATE,

        // Terminate yourself and all parents, return to the home screen
        HOME,

        // Launch a child view. When it terminates, control will return to you.
        // use new to allocate the new view and return it in launch_view
        LAUNCH
    };

    Kind kind = NOP;

    // If you return action==LAUNCH
    // create a new view and return it here.
    // The manager will call delete on this when it terminates.
    View* launch_view = NULL;
};

class View {
    public:

    // TODO: What if you don't want an event driven view?
    // If we're making games, this probably won't be good enough.

    // TODO: What if you need refreshes every now and then? Like the clock on the home screen

    // You're the foreground view
    // Something happened
    // What do you do?
    virtual void event(const ViewEvent& ev, ViewResponse& resp) = 0;

    virtual ~View();

};
