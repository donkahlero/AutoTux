#include <iostream>

#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opendavinci/odcore/base/KeyValueConfiguration.h>

#include "camera/OpenCVCamera.h"

namespace proxy {
    namespace camera {
        using namespace std;
        using namespace odcore::base;

        OpenCVCamera::OpenCVCamera(const string &name, const uint32_t &id, const uint32_t &width, const uint32_t &height, const uint32_t &bpp) :
            Camera(name, id, width, height, bpp),
            m_vc(),
            m_mat() {
                m_vc.open(id);
                if(m_vc.isOpened()) {
                    m_vc.set(CV_CAP_PROP_FRAME_HEIGHT, height);
                    m_vc.set(CV_CAP_PROP_FRAME_WIDTH, width);
                    m_vc.set(CV_CAP_PROP_FPS, 30);
                } else {
                    cerr << "CameraProxy: Could not open camera '" << name << "' with ID: " << id << endl;
                }
        }

        OpenCVCamera::~OpenCVCamera() {
            if(m_vc.isOpened()) {
                m_vc.release();
            }
        }

        bool OpenCVCamera::isValid() const {
            return m_vc.isOpened();
        }

        bool OpenCVCamera::captureFrame() {
            bool returnVal = false;
            if (m_vc.isOpened()) {
                if (m_vc.grab()) {
                    m_vc.retrieve(m_mat);
                }
                returnVal = true;
            }
            return returnVal;
        }


        bool OpenCVCamera::copyImageTo(char *dest, const uint32_t &size) {
            bool returnVal = false;
            if((dest != NULL) && (size > 0)) {
                memcpy(dest, m_mat.data+640*240*3, size);
                returnVal = true;
            }
            return returnVal;
        }
    }
}
