
/**
 * @brief Waterlinked's M64 Acoutstic Model Control & Management Task
 * @author Byunghun Hwang <bh.hwang@iae.re.kr>
 */

#ifndef _OPENEDGE_WATERLINK_M64_TASK_HPP_
#define _OPENEDGE_WATERLINK_M64_TASK_HPP_

#include <openedge/core.hpp>
#include <string>
#include <vector>
#include <map>

using namespace oe;
using namespace std;

class waterlinkM64Task : public oe::core::task::runnable_nt {

    public:
        waterlinkM64Task();
        virtual ~waterlinkM64Task();

        /* component interface functions for runnable nt*/
        bool configure() override;
        void execute() override;
        void cleanup() override;
        void pause() override;
        void resume() override;

        /* event callback functions */
        void on_request(std::any data) override;

        /* private functions */
    private:
        

};

EXPORT_TASK_API

#endif