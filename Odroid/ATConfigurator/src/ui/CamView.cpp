/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

#include "ui/CamView.h"
#include <memory>
#include <fstream>
#include <cstring>
#include <od/ConferenceData.h>

ui::CamView::CamView(void) { CamView(80, 20); }

ui::CamView::CamView(int x, int y) : xsize(x - 2), ysize(y - 2),
                                     _camview(newwin(ysize - 2, xsize - 16, 1, 16)),
                                     image("No image available.") {}

void ui::CamView::refresh(void) {
    wmove(_camview, 0, 0);
    wclrtobot(_camview);
    if(od::ConferenceData::instance()->isCamView() && std::ifstream("camview.jpg")) {
        std::string tempimg = loadImage();

        if(std::strcmp(tempimg.c_str(), "Premature end of JPEG file") != 0 &&
                std::strcmp(tempimg.c_str(), "Empty input file") != 0)
            image = tempimg;

        mvwaddstr(_camview, 1, 0, image.c_str());
    } else {
        mvwaddstr(_camview, 1, 1, "To load up an image press ENTER!");
    }

    wrefresh(_camview);
}

void ui::CamView::select(void) { od::ConferenceData::instance()->setCamView(true); }

void ui::CamView::unselect(void) { od::ConferenceData::instance()->setCamView(false); }

void ui::CamView::selUp(void) {}
void ui::CamView::selDn(void) {}
void ui::CamView::selLeft(void) {}
void ui::CamView::selRight(void) {}
void ui::CamView::incr(void) {}
void ui::CamView::decr(void) {}

std::string ui::CamView::loadImage() {
    std::string cmd = "jp2a --width=";
    cmd.append(std::to_string(xsize - 18));
    cmd.append(" camview.jpg 2> /dev/null");

    //http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
    char buffer[4096];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }

    return result;
}