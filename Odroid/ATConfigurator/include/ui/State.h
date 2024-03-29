/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Class to represent a ncurses window for changing the state.
 */

#ifndef ATCONFIGURATOR_STATE_H
#define ATCONFIGURATOR_STATE_H

#include "ui/ATCWindow.h"
#include <ncurses.h>

namespace ui {
    class State : public ATCWindow {
    public:
        State(void);
        State(int, int);
        // Overriding the virtual methods
        void refresh(void);
        void select(void);
        void unselect(void);
        void selUp(void);
        void selDn(void);
        void selLeft(void);
        void selRight(void);
        void incr(void);
        void decr(void);
    private:
        int xsize;
        int ysize;
        bool active;
        int selectedItem;
        WINDOW* _state;
        void printStates(void);
    };
}

#endif //ATCONFIGURATOR_STATE_H
