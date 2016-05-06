/*!
 * USBHandler header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_USBHANDLER_H
#define AUTOTUXPROXY_USBHANDLER_H

#include <atomic>
#include "serial/SerialBuffer.h"
#include "serial/SerialIOInterface.h"

namespace serial
{
    class SerialHandler
    {
    public:
        /*! constructor */
        SerialHandler();
        /*! destructor */
        ~SerialHandler();
        /*! run function for the thread */
        void run();
        /*! stops the handler, sets the loop control variable to false */
        void stop();
        /*! sets the usb connector for this handler */
        void set_serialio(std::shared_ptr<serial::SerialIOInterface>);
        /*! sets the usb connector for this handler */
        void set_buffer(std::shared_ptr<serial::SerialBuffer>);
        //void set_serialio(serial::conninter::SerialIOInterface*);
        /*! sets verbose */
        void set_verbose(bool);
        /*! returns running state */
        bool get_running();
    private:
        /*! reconnects the usb */
        void reconnect();
        /*! returns true if reconnection needed, false otherwise*/
        bool is_reconnect(int);
    private:
        /*! is it verbose mode */
        bool verbose;
        /*! loop control */
        volatile bool running;
        /*! the serial io */
        std::shared_ptr<serial::SerialIOInterface> pserio;
        /*! the serial buffer */
        std::shared_ptr<serial::SerialBuffer> pserbuf;
    };
} // namespace serial

#endif	// AUTOTUXPROXY_USBHANDLER_H
