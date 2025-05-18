#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(viewmanager, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/device.h>


#include "viewmanager.hpp"
#include "view.hpp"
#include "view-home.hpp"

#define evkind ViewEvent::Kind
#define reskind ViewResponse::Kind

struct ViewStackEntry {
    // NULL for the home screen, which cannot terminate.
    ViewStackEntry* parent;
    View* view;
};

ViewStackEntry* view_stack;

static void push_and_handle_event(const ViewEvent& ev);

static void send_terminate(View* view) {
                    
    ViewEvent terminate;
    terminate.kind = evkind::TERMINATE;

    ViewResponse ignored;
    ignored.kind = reskind::NOP;
    

    if (ignored.launch_view != NULL) {
        LOG_ERR("A view responded to a TERMINATE with some heap allocation. You've got a leak.");
    }
    view->event(terminate, ignored);

}

static void terminate_active_view() {
    if (view_stack->parent == NULL) {
        LOG_ERR("Tried to TERMINATE home view. Can't do that.\n");
    } else {
        send_terminate(view_stack->view);

        ViewStackEntry* old_head = view_stack;
        view_stack = old_head->parent;

        delete old_head->view;
        delete old_head;

        LOG_ERR("View terminated.");

    }
}

static void redraw_active_view() {
    // Make the reopened view perform a full draw
    ViewEvent redraw;
    redraw.kind = evkind::REDRAW;
    push_and_handle_event(redraw);
}


static void push_and_handle_event(const ViewEvent& ev) {
        ViewResponse resp;
        resp.kind = reskind::NOP;

        view_stack->view->event(ev, resp);

        switch (resp.kind) {
            case reskind::NOP:
                break;

            case reskind::TERMINATE:
                terminate_active_view();
                redraw_active_view();

                break;

            case reskind::HOME:
                while (view_stack->parent != NULL) {
                    terminate_active_view();
                }
                redraw_active_view();

                break;

            case reskind::LAUNCH:
                if(resp.launch_view == NULL) {
                    LOG_ERR("Tried to launch a NULL view. Don't do that.\n");
                } else {
                    ViewStackEntry* new_head = new ViewStackEntry();
                    new_head->parent = view_stack;
                    new_head->view = resp.launch_view;
                    view_stack = new_head;

                    LOG_ERR("View launched.");

                    redraw_active_view();

                }
                break;

        }


}

void vman_thread(void* d0, void* d1, void* d2) {

    while (true) {
        ViewEvent ev;
        ev.kind = evkind::REDRAW;

        push_and_handle_event(ev);

        k_sleep(K_MSEC(100));
    }
}

K_THREAD_DEFINE(vman_tid, 1024,
               vman_thread, NULL, NULL, NULL,
               K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);


void vman_start() {

    HomeView* home = new HomeView();

    ViewStackEntry* head = new ViewStackEntry();
    head->parent = NULL;
    head->view = home;

    view_stack = head;

    k_thread_start(vman_tid);
}

