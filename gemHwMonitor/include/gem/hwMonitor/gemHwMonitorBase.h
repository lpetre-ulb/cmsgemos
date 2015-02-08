#ifndef gem_hwMonitor_gemHwMonitorBase_h
#define gem_hwMonitor_gemHwMonitorBase_h

#include <string>

#include "xdaq/Application.h"
#include "xgi/framework/Method.h"

namespace gem {
    namespace base {
        namespace utils {
            class gemXMLparcer;
        }
    }
    namespace hwMonitor {
        class gemHwMonitorBase//: public xdaq::Application//, public xdata::ActionListener
        {
            public:
                //XDAQ_INSTANTIATOR();
                //gemHwMonitorBase(xdaq::ApplicationStub * s)
                gemHwMonitorBase()
                    throw (xdaq::exception::Exception);

                ~gemHwMonitorBase(){}
                /**
                 *   Select one of available crates
                 */
                void setCurrentCrate(std::string crateID)
                    throw (xgi::exception::Exception)
		            {currentCrateID_ = crateID;}
                /**
                 *   Get selected crate
                 */
                std::string getCurrentCrate()
                    throw (xgi::exception::Exception)
		            {return currentCrateID_;}
                /**
                 *   Set XML configuration file
                 */
                void setXMLconfigFile (std::string inputXMLfilename)
                    throw (xgi::exception::Exception)
		            {xmlConfigFileName_ = inputXMLfilename;}
                /**
                 *   Get XML configuration file
                 */
                std::string getXMLconfigFile ()
                    throw (xgi::exception::Exception)
		            {return xmlConfigFileName_;}
                /**
                 *   Select database configuration file
                 */
                void setDBSconfigFile (std::string inputDBSfilename)
                    throw (xgi::exception::Exception){}
                /**
                 *   Board status
                 */
                unsigned int getBoardStatus ()
                    throw (xgi::exception::Exception)
		            {return boardStatus_;}
                /**
                 *   Access to board utils
                virtual void boardUtils ()
                    throw (xgi::exception::Exception);
                 */

            protected:
            private:
                std::string currentCrateID_;
                unsigned int boardStatus_;
                std::string boardID_;
                std::string xmlConfigFileName_;
        }; // end namespace hwMon
    }
} // end namespace gem
#endif
