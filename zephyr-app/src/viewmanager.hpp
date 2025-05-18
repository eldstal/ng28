#pragma once

/*
 * The view manager is the glue between visible views
 * It will run your view (e.g. a menu) until it needs to
 * either quit (go back to the previous view) or launch
 * a new view (start the snake game).
 *
 * The view manager maintains a stack of views, e.g.
 *   0 home view, which launched
 *   1 main menu view, which launched
 *   2 settings menu view, which launched
 *   3 system info view
 *
 *  When a view terminates, the previous view in the
 *  stack resumes where it was.
 */

// Start the view manager and show the home screen
void vman_start();
