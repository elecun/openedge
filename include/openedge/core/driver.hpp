
/**
 * @file    driver.hpp
 * @brief   task driver class
 * @author  
 */

namespace oe {
    namespace core {
        class taskDriver {
            public:
                explicit taskDriver();
                ~taskDriver();

                bool configure();
                void execute();
                void cleanup();
        }
    }
}