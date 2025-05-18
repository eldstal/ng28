#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(viewmanager, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/device.h>


#include "viewmanager.hpp"
#include "view.hpp"

#define evkind ViewEvent::Kind
#define reskind ViewResponse::Kind

struct ViewStackEntry {
    // NULL for the home screen, which cannot terminate.
    ViewStackEntry* parent;
    View* view;
};

ViewStackEntry* view_stack;

void vman_start() {

    ViewStackEntry home = { };
    view_stack = &home;

}

void send_terminate(View* view) {
                    
    ViewEvent terminate = { .kind = evkind::TERMINATE };
    ViewResponse ignored = { .kind = reskind::NOP, .launch_view = NULL };

    if (ignored.launch_view != NULL) {
        LOG_ERR("A view responded to a TERMINATE with some heap allocation. You've got a leak.");
    }
    view->event(terminate, ignored);

}

void terminate_active_view() {
    if (view_stack->parent == NULL) {
        LOG_ERR("Tried to TERMINATE home view. Can't do that.\n");
    } else {
        send_terminate(view_stack->view);

        ViewStackEntry* old_head = view_stack;
        view_stack = old_head->parent;

        k_free(old_head->view);
        k_free(old_head);

        LOG_ERR("View terminated.");
    }
}

void vman_thread(void* d0, void* d1, void* d2) {

    while (true) {
        ViewEvent ev = { .kind = evkind::REDRAW };
        ViewResponse resp = { .kind = reskind::NOP };

        view_stack->view->event(ev, resp);

        switch (resp.kind) {
            case reskind::NOP:
                break;

            case reskind::TERMINATE:
                terminate_active_view();
                break;

            case reskind::LAUNCH:
                if(resp.launch_view == NULL) {
                    LOG_ERR("Tried to launch a NULL view. Don't do that.\n");
                } else {
                    ViewStackEntry* new_head = (ViewStackEntry*) k_malloc(sizeof(ViewStackEntry));
                    new_head->parent = view_stack;
                    new_head->view = resp.launch_view;
                    view_stack = new_head;

                    LOG_ERR("View launched.");
                }

            case reskind::HOME:
                while (view_stack->parent != NULL) {
                    terminate_active_view();
                }

        }


    }
}

K_THREAD_DEFINE(vman_tid, 1024,
               vman_thread, NULL, NULL, NULL,
               K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);
