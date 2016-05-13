/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

#ifndef ATCONFIGURATOR_HEADER_H
#define ATCONFIGURATOR_HEADER_H

#include <ncurses.h>

namespace ui {
    class Header {
    public:
        Header(void);
        Header(int, int);
        void refresh(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _header;
    };
}

#endif //ATCONFIGURATOR_HEADER_H
