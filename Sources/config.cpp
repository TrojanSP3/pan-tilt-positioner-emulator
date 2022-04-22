#include "config.h"

#include <sstream>

Property<std::string> CONFIG::FILENAME("config.cfg");

Property<std::string> CONFIG::CONTROLLER::INIT_INPUTS_STATE ("0000000000000000");
Property<std::string> CONFIG::CONTROLLER::INIT_OUTPUTS_STATE("0000000000000000");
Property<bool> CONFIG::CONTROLLER::AZIMUTH_SYNCED_ON_INIT(false);
Property<bool> CONFIG::CONTROLLER::ELEVATION_SYNCED_ON_INIT(false);
Property<bool> CONFIG::CONTROLLER::FAST_SYNCHRONIZATION(false);
PropertyReadOnly<int> CONFIG::CONTROLLER::ADDITIONAL_SERIAL_LINES(5);
PropertyReadOnly<int> CONFIG::CONTROLLER::TICK_MS(10);
Property<std::string> CONFIG::CONTROLLER::ENDL("\r\n");

Property<int> CONFIG::TCP::PORT(10000);
Property<uint32_t> CONFIG::TCP::MAXBUFFERSIZE(10);

PropertyReadOnly<int> CONFIG::ENGINE::MAX_AZIMUTH(62832);
Property<int> CONFIG::ENGINE::ZERO_ELEVATION(31417);
PropertyReadOnly<int> CONFIG::ENGINE::HARD_LIMIT_MAX_ELEVATION(CONFIG::ENGINE::ZERO_ELEVATION.Get()+CONFIG::ENGINE::MAX_AZIMUTH.Get()/4);
PropertyReadOnly<int> CONFIG::ENGINE::HARD_LIMIT_MIN_ELEVATION(CONFIG::ENGINE::ZERO_ELEVATION.Get()-CONFIG::ENGINE::MAX_AZIMUTH.Get()/4);
PropertyAtomic<int> CONFIG::ENGINE::LIMIT_MAX_ELEVATION(CONFIG::ENGINE::HARD_LIMIT_MAX_ELEVATION.Get());
PropertyAtomic<int> CONFIG::ENGINE::LIMIT_MIN_ELEVATION(CONFIG::ENGINE::HARD_LIMIT_MIN_ELEVATION.Get());
Property<int> CONFIG::ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND(Utils::DegreesToMsoRad(36));
Property<int> CONFIG::ENGINE::MAX_ELEVATION_SPEED_PER_SECOND(Utils::DegreesToMsoRad(36));
Property<double> CONFIG::ENGINE::VEL_TO_EPS_COEFFICIENT(16.0/1000);
PropertyReadOnly<int> CONFIG::ENGINE::TICK_MS(10);

PropertyReadOnly<int> CONFIG::DRAWING::HEIGHT(21);
PropertyReadOnly<char> CONFIG::DRAWING::CHAR_EMPTY(' ');
PropertyReadOnly<char> CONFIG::DRAWING::CHAR_BORDER('#');
PropertyReadOnly<char> CONFIG::DRAWING::CHAR_CIRCLE('O');
PropertyReadOnly<char> CONFIG::DRAWING::CHAR_CENTER('X');
PropertyReadOnly<char> CONFIG::DRAWING::CHAR_POINTER('*');

PropertyReadOnly<std::string> CONFIG::LOG::FILENAME("data.log");
PropertyReadOnly<std::string> CONFIG::LOG::TEST_FILENAME("./mso_test_log.log");

Property<uint64_t> CONFIG::LOG::MAX_SIZE_BYTES(5*1024*1024);

std::string CONFIG::ToString()
{
    std::stringstream msg;
    msg<<"===CONFIG BEGIN==="<<std::endl;
    msg<<"FILENAME: "<<FILENAME.Get()<<std::endl;
    msg<<CONTROLLER::ToString()<<std::endl;
    msg<<TCP::ToString()<<std::endl;
    msg<<ENGINE::ToString()<<std::endl;
    msg<<DRAWING::ToString()<<std::endl;
    msg<<LOG::ToString()<<std::endl;
    msg<<"===CONFIG END==="<<std::endl;
    return msg.str();


}

std::string CONFIG::CONTROLLER::ToString()
{
    std::stringstream msg;
    msg<<">CONTROLLER"<<std::endl;
    msg<<"INIT_INPUTS_STATE: "<<INIT_INPUTS_STATE.Get()<<std::endl;
    msg<<"INIT_OUTPUTS_STATE: "<<INIT_OUTPUTS_STATE.Get()<<std::endl;
    msg<<"AZIMUTH_SYNCED_ON_INIT: "<<AZIMUTH_SYNCED_ON_INIT.GetString()<<std::endl;
    msg<<"ELEVATION_SYNCED_ON_INIT: "<<ELEVATION_SYNCED_ON_INIT.GetString()<<std::endl;
    msg<<"FAST_SYNCHRONIZATION: "<<FAST_SYNCHRONIZATION.GetString()<<std::endl;
    msg<<"ADDITIONAL_SERIAL_LINES: "<<ADDITIONAL_SERIAL_LINES.Get()<<std::endl;
    msg<<"TICK_MS: "<<TICK_MS.Get()<<std::endl;
    std::string tmp_endl=ENDL.Get();
    Utils::ControlCharsToString(tmp_endl);
    msg<<"ENDL: "<<"\""<<tmp_endl<<"\""<<std::endl;
    return msg.str();
}

std::string CONFIG::TCP::ToString()
{
    std::stringstream msg;
    msg<<">TCP"<<std::endl;
    msg<<"PORT: "<<PORT.Get()<<std::endl;
    msg<<"MAXBUFFERSIZE: "<<MAXBUFFERSIZE.Get()<<std::endl;
    return msg.str();
}

std::string CONFIG::ENGINE::ToString()
{
    std::stringstream msg;
    msg<<">ENGINE"<<std::endl;
    msg<<"MAX_AZIMUTH: "<<MAX_AZIMUTH.Get()<<std::endl;
    msg<<"ZERO_ELEVATION: "<<ZERO_ELEVATION.Get()<<std::endl;
    msg<<"HARD_LIMIT_MAX_ELEVATION: "<<HARD_LIMIT_MAX_ELEVATION.Get()<<std::endl;
    msg<<"HARD_LIMIT_MIN_ELEVATION: "<<HARD_LIMIT_MIN_ELEVATION.Get()<<std::endl;
    msg<<"LIMIT_MAX_ELEVATION: "<<LIMIT_MAX_ELEVATION.Get()<<std::endl;
    msg<<"LIMIT_MIN_ELEVATION: "<<LIMIT_MIN_ELEVATION.Get()<<std::endl;
    msg<<"MAX_AZIMUTH_SPEED_PER_SECOND: "<<MAX_AZIMUTH_SPEED_PER_SECOND.Get()<<std::endl;
    msg<<"MAX_ELEVATION_SPEED_PER_SECOND: "<<MAX_ELEVATION_SPEED_PER_SECOND.Get()<<std::endl;
    msg<<"VEL_TO_EPS_COEFFICIENT: "<<VEL_TO_EPS_COEFFICIENT.Get()<<std::endl;
    msg<<"TICK_MS: "<<TICK_MS.Get()<<std::endl;

    return msg.str();
}

std::string CONFIG::DRAWING::ToString()
{
    std::stringstream msg;
    msg<<">DRAWING"<<std::endl;
    msg<<"HEIGHT: "<<HEIGHT.Get()<<std::endl;
    msg<<"CHAR_EMPTY: "<<'\''<<CHAR_EMPTY.Get()<<'\''<<std::endl;
    msg<<"CHAR_BORDER: "<<'\''<<CHAR_BORDER.Get()<<'\''<<std::endl;
    msg<<"CHAR_CIRCLE: "<<'\''<<CHAR_CIRCLE.Get()<<'\''<<std::endl;
    msg<<"CHAR_CENTER: "<<'\''<<CHAR_CENTER.Get()<<'\''<<std::endl;
    msg<<"CHAR_POINTER: "<<'\''<<CHAR_POINTER.Get()<<'\''<<std::endl;
    return msg.str();
}

std::string CONFIG::LOG::ToString()
{
    std::stringstream msg;
    msg<<">LOG"<<std::endl;
    msg<<"FILENAME: "<<FILENAME.Get()<<std::endl;
    msg<<"TEST_FILENAME: "<<TEST_FILENAME.Get()<<std::endl;
    msg<<"MAX_SIZE_BYTES: "<<MAX_SIZE_BYTES.Get()<<std::endl;
    return msg.str();
}




