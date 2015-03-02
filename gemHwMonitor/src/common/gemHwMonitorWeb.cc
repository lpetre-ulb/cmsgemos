#include "gem/hwMonitor/gemHwMonitorWeb.h"

XDAQ_INSTANTIATOR_IMPL(gem::hwMonitor::gemHwMonitorWeb)

gem::hwMonitor::gemHwMonitorWeb::gemHwMonitorWeb(xdaq::ApplicationStub * s)
    throw (xdaq::exception::Exception):
        xdaq::WebApplication(s)
{
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::Default, "Default");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::controlPanel, "Control Panel");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::setConfFile,"setConfFile");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::getCratesConfiguration,"getCratesConfiguration");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::expandCrate,"expandCrate");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::cratePanel,"cratePanel");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::expandGLIB,"expandGLIB");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::glibPanel,"glibPanel");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::expandOH,"expandOH");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::ohPanel,"ohPanel");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::expandVFAT,"expandVFAT");
    xgi::framework::deferredbind(this, this, &gemHwMonitorWeb::vfatPanel,"vfatPanel");
    gemHwMonitorSystem_ = new gemHwMonitorSystem();
    gemHwMonitorCrate_ = new gemHwMonitorCrate();
    gemHwMonitorGLIB_ = new gemHwMonitorGLIB();
    gemHwMonitorOH_ = new gemHwMonitorOH();
    gemHwMonitorVFAT_ = new gemHwMonitorVFAT();
    gemSystemHelper_ = new gemHwMonitorHelper(gemHwMonitorSystem_);
    crateCfgAvailable_ = false;
}

gem::hwMonitor::gemHwMonitorWeb::~gemHwMonitorWeb()
{
    delete gemHwMonitorSystem_;
    delete gemHwMonitorCrate_;
    delete gemHwMonitorGLIB_;
    delete gemHwMonitorOH_;
    delete gemHwMonitorVFAT_;
    delete gemSystemHelper_;
}
void gem::hwMonitor::gemHwMonitorWeb::Default(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    this->controlPanel(in,out);
}
/* Generates the main page interface. Allows to choose the configuration file, then shows the availability of crates corresponding to this configuration.
 * Allows to launch the test utility to check the crates state.
 * */
void gem::hwMonitor::gemHwMonitorWeb::controlPanel(xgi::Input * in, xgi::Output * out )
    throw (xgi::exception::Exception)
{
    try {

         *out << "    <link rel=\"stylesheet\" href=\"/gemdaq/gemhardware/html/css/vfat/vfatmanager.css\"/>"          << std::endl
            << "    <link rel=\"stylesheet\" href=\"/gemdaq/gemhardware/html/css/vfat/vfatcontrolregisters.css\"/>" << std::endl
            << "    <link rel=\"stylesheet\" href=\"/gemdaq/gemhardware/html/css/vfat/vfatglobalsettings.css\"/>"   << std::endl
            << "    <link rel=\"stylesheet\" href=\"/gemdaq/gemhardware/html/css/vfat/vfatchannelregister.css\"/>"  << std::endl
            << "    <link rel=\"stylesheet\" href=\"/gemdaq/gemhardware/html/css/vfat/vfatcommands.css\"/>"         << std::endl;

        ///////////////////////////////////////////////
        //
        // GEM System Configuration
        //
        ///////////////////////////////////////////////

        *out << cgicc::h1("GEM System Configuration")<< std::endl;
        //
        *out << cgicc::span().set("style","color:blue");
        *out << cgicc::b(cgicc::i("Current configuration file: ")) ;
        *out << gemSystemHelper_->getXMLconfigFile() << cgicc::span() << std::endl ;
        *out << cgicc::br()<< std::endl;
        *out << cgicc::br()<< std::endl;

        std::string methodText = toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","POST").set("action",methodText) << std::endl ;
        *out << cgicc::input().set("type","text").set("name","xmlFilename").set("size","80").set("ENCTYPE","multipart/form-data").set("value",gemSystemHelper_->getXMLconfigFile()) << std::endl;
        *out << cgicc::input().set("type","submit").set("value","Set configuration file") << std::endl ;
        *out << cgicc::form() << std::endl ;

        std::string methodUpload = toolbox::toString("/%s/uploadConfFile",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","POST").set("enctype","multipart/form-data").set("action",methodUpload) << std::endl ;
        *out << cgicc::input().set("type","file").set("name","xmlFilenameUpload").set("size","80") << std::endl;
        *out << cgicc::input().set("type","submit").set("value","Submit") << std::endl ;
        *out << cgicc::form() << std::endl ;

        *out << cgicc::hr()<< std::endl;
        *out << cgicc::br();

        *out << cgicc::h1("Crates configuration")<< std::endl;
	    this->showCratesAvailability(in,out);
        *out << cgicc::hr()<< std::endl;
        *out << cgicc::br();

    }

    catch (const xgi::exception::Exception& e) {
        LOG4CPLUS_INFO(this->getApplicationLogger(),"Something went wrong displaying ControlPanel xgi: " << e.what());
        XCEPT_RAISE(xgi::exception::Exception, e.what());
    }
    catch (const std::exception& e) {
        LOG4CPLUS_INFO(this->getApplicationLogger(),"Something went wrong displaying the ControlPanel: " << e.what());
        XCEPT_RAISE(xgi::exception::Exception, e.what());
    }
}
void gem::hwMonitor::gemHwMonitorWeb::showCratesAvailability(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    std::string methodGetCratesCfg = toolbox::toString("/%s/getCratesConfiguration", getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","POST").set("action", methodGetCratesCfg) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Get crates configuration") << "    " << std::endl;
    *out << cgicc::form() << std::endl ;
    // If crates config is not available yet prompt to get it
    if (!crateCfgAvailable_) {
        *out << "Crate configuration isn't available. Please, get the crates configuration" << cgicc::br() <<std::endl;
    } else {
        *out << "&nbsp" << cgicc::br() << std::endl;
        *out << "Crates configuration is taken from XML. In order to check their availability please select needed crates and press 'Check selected crates availability' button. " << 
        "To have more information about their state press 'Test selected crates' button" << 
            cgicc::br() << std::endl;
        std::string methodExpandCrate = toolbox::toString("/%s/expandCrate", getApplicationDescriptor()->getURN().c_str());
        std::string methodSelectCrate = toolbox::toString("/%s/selectCrate", getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::table().set("border","0");
        for (int i=0; i<nCrates_; i++) {
            std::string currentCrateID;
            currentCrateID += gemHwMonitorSystem_->getCurrentSubDeviceId(i);
            *out << cgicc::td();
                *out << cgicc::table().set("border","0");
                *out << cgicc::tr();
                    *out << cgicc::td().set("align","center");
                    *out << cgicc::form().set("method","POST").set("action", methodSelectCrate) << std::endl ;
                    *out << cgicc::input().set("type","checkbox").set("name","crateCheckBox").set("value",currentCrateID) << std::endl;
                    *out << cgicc::form() << std::endl ;
                    *out << cgicc::td();
                *out << cgicc::tr();
                *out << cgicc::tr();
                    *out << cgicc::form().set("method","POST").set("action", methodExpandCrate) << std::endl ;
                    *out << cgicc::input().set("type","submit").set("name","crateButton").set("value",currentCrateID) << std::endl;
                    *out << cgicc::form() << std::endl ;
                *out << cgicc::tr();
                *out << cgicc::tr();
                    *out << cgicc::img().set("src", "/gemdaq/gemHwMonitor/html/images/green.jpg").set("width","84") << std::endl;
                *out << cgicc::tr();
                *out << cgicc::table();
            *out << cgicc::td();
        }
        *out << cgicc::table();
        *out << cgicc::br();

        *out << cgicc::table().set("border","0");
        *out << cgicc::td();
        std::string b22 = toolbox::toString("/%s/Dummy",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","GET").set("action",b22) << std::endl ;
        *out << cgicc::input().set("type","submit").set("value","Check availability of selected crates") << std::endl ;
        *out << cgicc::form();
        *out << cgicc::td();
        
        *out << cgicc::td();
        std::string b23 = toolbox::toString("/%s/Dummy",getApplicationDescriptor()->getURN().c_str());
        *out << cgicc::form().set("method","GET").set("action",b23) << std::endl ;
        *out << cgicc::input().set("type","submit").set("value","Test selected crates") << std::endl ;
        *out << cgicc::form();
        *out << cgicc::td();
        
        *out << cgicc::table();
        *out << cgicc::br();
    }
}

void gem::hwMonitor::gemHwMonitorWeb::setConfFile(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);
    std::string newFile = cgi.getElement("xmlFilename")->getValue();
    struct stat buffer;
    if (stat(newFile.c_str(), &buffer) == 0) {
        gemSystemHelper_->setXMLconfigFile(newFile.c_str());
    }
    else {
        XCEPT_RAISE(xgi::exception::Exception, "File not found");
    }
    this->controlPanel(in,out);
}

void gem::hwMonitor::gemHwMonitorWeb::uploadConfFile(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);
    std::string newFile = cgi.getElement("xmlFilenameUpload")->getValue();
    struct stat buffer;
    if (stat(newFile.c_str(), &buffer) == 0) {
        gemSystemHelper_->setXMLconfigFile(newFile.c_str());
    }
    else {
        XCEPT_RAISE(xgi::exception::Exception, "File not found");
    }
    this->controlPanel(in,out);
}
void gem::hwMonitor::gemHwMonitorWeb::getCratesConfiguration(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    gemSystemHelper_->configure();
    crateCfgAvailable_ = true;
    nCrates_ = gemHwMonitorSystem_->getNumberOfSubDevices();
    this->controlPanel(in,out);
}
void gem::hwMonitor::gemHwMonitorWeb::expandCrate(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);
    crateToShow_ = cgi.getElement("crateButton")->getValue();
//for (auto i = gemHwMonitorSystem_->getDevice()->getSubDevicesRefs().begin(); i != gemHwMonitorSystem_->getDevice()->getSubDevicesRefs().end(); i++) {
    //if (i->getDeviceId() == crateToShow_) {gemHwMonitorCrate_->setDeviceConfiguration(*i);}
    // Auto-pointer doesn't work for some reason. Improve this later.
    for (int i = 0; i != gemHwMonitorSystem_->getDevice()->getSubDevicesRefs().size(); i++) 
    {
        if (gemHwMonitorSystem_->getDevice()->getSubDevicesRefs().at(i)->getDeviceId() == crateToShow_) 
        {
            gemHwMonitorCrate_->setDeviceConfiguration(*gemHwMonitorSystem_->getDevice()->getSubDevicesRefs().at(i));
        }
    }
    this->cratePanel(in,out);
}
void gem::hwMonitor::gemHwMonitorWeb::cratePanel(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    *out << cgicc::h1("Crate ID: "+crateToShow_)<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Basic crate variables")<< std::endl;
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Connected AMC13")<< std::endl;
    *out << cgicc::br()<< std::endl;
    *out << "There are no AMC13 boards" << cgicc::br();
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Connected GLIB's")<< std::endl;
    std::string methodExpandGLIB = toolbox::toString("/%s/expandGLIB", getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::table().set("border","0");
    for (int i=0; i<gemHwMonitorCrate_->getNumberOfSubDevices(); i++) {
        std::string currentGLIBId;
        currentGLIBId += gemHwMonitorCrate_->getCurrentSubDeviceId(i);
        *out << cgicc::td();
            *out << cgicc::form().set("method","POST").set("action", methodExpandGLIB) << std::endl ;
            *out << cgicc::input().set("type","submit").set("name","glibButton").set("value",currentGLIBId) << std::endl;
            *out << cgicc::form() << std::endl;
        *out << cgicc::td();
    }
    *out << cgicc::table();
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
}

void gem::hwMonitor::gemHwMonitorWeb::expandGLIB(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);
    glibToShow_ = cgi.getElement("glibButton")->getValue();
    // Auto-pointer doesn't work for some reason. Improve this later.
    for (int i = 0; i != gemHwMonitorCrate_->getDevice()->getSubDevicesRefs().size(); i++) 
    {
        if (gemHwMonitorCrate_->getDevice()->getSubDevicesRefs().at(i)->getDeviceId() == glibToShow_) 
        {
            gemHwMonitorGLIB_->setDeviceConfiguration(*gemHwMonitorCrate_->getDevice()->getSubDevicesRefs().at(i));
        }
    }
    this->glibPanel(in,out);
}
void gem::hwMonitor::gemHwMonitorWeb::glibPanel(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    *out << cgicc::h1("GLIB ID: "+glibToShow_)<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Basic glib variables")<< std::endl;
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Connected OH's")<< std::endl;
    std::string methodExpandOH = toolbox::toString("/%s/expandOH", getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::table().set("border","0");
    for (int i=0; i<gemHwMonitorGLIB_->getNumberOfSubDevices(); i++) {
        std::string currentOHId;
        currentOHId += gemHwMonitorGLIB_->getCurrentSubDeviceId(i);
        *out << cgicc::td();
            *out << cgicc::form().set("method","POST").set("action", methodExpandOH) << std::endl ;
            *out << cgicc::input().set("type","submit").set("name","ohButton").set("value",currentOHId) << std::endl;
            *out << cgicc::form() << std::endl;
        *out << cgicc::td();
    }
    *out << cgicc::table();
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
}

void gem::hwMonitor::gemHwMonitorWeb::expandOH(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);
    ohToShow_ = cgi.getElement("ohButton")->getValue();
    // Auto-pointer doesn't work for some reason. Improve this later.
    for (int i = 0; i != gemHwMonitorGLIB_->getDevice()->getSubDevicesRefs().size(); i++) 
    {
        if (gemHwMonitorGLIB_->getDevice()->getSubDevicesRefs().at(i)->getDeviceId() == ohToShow_) 
        {
            gemHwMonitorOH_->setDeviceConfiguration(*gemHwMonitorGLIB_->getDevice()->getSubDevicesRefs().at(i));
        }
    }
    this->ohPanel(in,out);
}
void gem::hwMonitor::gemHwMonitorWeb::ohPanel(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    *out << cgicc::h1("OH ID: "+ohToShow_)<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Basic OH variables")<< std::endl;
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Connected VFAT's")<< std::endl;
    std::string methodExpandVFAT = toolbox::toString("/%s/expandVFAT", getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::table().set("border","0");
    for (int i=0; i<gemHwMonitorOH_->getNumberOfSubDevices(); i++) {
        std::string currentVFATId;
        currentVFATId += gemHwMonitorOH_->getCurrentSubDeviceId(i);
        *out << cgicc::td();
            *out << cgicc::form().set("method","POST").set("action", methodExpandVFAT) << std::endl ;
            *out << cgicc::input().set("type","submit").set("name","vfatButton").set("value",currentVFATId) << std::endl;
            *out << cgicc::form() << std::endl;
        *out << cgicc::td();
    }
    *out << cgicc::table();
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
}

void gem::hwMonitor::gemHwMonitorWeb::expandVFAT(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    cgicc::Cgicc cgi(in);
    vfatToShow_ = cgi.getElement("vfatButton")->getValue();
    // Auto-pointer doesn't work for some reason. Improve this later.
    for (int i = 0; i != gemHwMonitorOH_->getDevice()->getSubDevicesRefs().size(); i++) 
    {
        if (gemHwMonitorOH_->getDevice()->getSubDevicesRefs().at(i)->getDeviceId() == vfatToShow_) 
        {
            gemHwMonitorVFAT_->setDeviceConfiguration(*gemHwMonitorOH_->getDevice()->getSubDevicesRefs().at(i));
        }
    }
    this->vfatPanel(in,out);
}
void gem::hwMonitor::gemHwMonitorWeb::vfatPanel(xgi::Input * in, xgi::Output * out )
throw (xgi::exception::Exception)
{
    *out << cgicc::h1("VFAT ID: "+vfatToShow_)<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::h2("Basic VFAT variables")<< std::endl;
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
    *out << cgicc::br()<< std::endl;
    *out << cgicc::hr()<< std::endl;
}
