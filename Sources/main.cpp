#define VERSION (__DATE__ " " __TIME__)

#include <string.h>
#include <vector>
#include <iostream>
#include <unistd.h>

#include "main.h"
#include "config.h"
#include "log.h"
#include "drawing.h"
#include "configfile.h"
#include "protocol_mso2.h"

const std::string LOGMODULE="Main";

int main(int argc, char *argv[])
{
    int exit_code = Main::main(argc, argv);
    std::cout.flush();
    std::cerr.flush();
    LOG.Stop();
    return exit_code;
}
const std::string KEYWORD_HELP    = "--help";
const std::string KEYWORD_VERSION = "--version";
const std::string KEYWORD_PROTOCOL_MSO2 = "mso2";

int Main::main(int argc, char **argv)
{
    try
    {
        ChangeWorkingDir(argv[0]);
    }
    catch(std::exception& e)
    {
        std::cerr<<e.what()<<std::endl;
        return EXIT_FAILURE;
    }

    if(argc==2 || argc==3)
    {
        std::string parameter_1 = argv[1];
        std::string parameter_2 = "./"+CONFIG::FILENAME.Get();
        if(argc==3)
            parameter_2 = argv[2];
        if(parameter_1==KEYWORD_HELP)
        {
            PrintHelp(argv[0]);
        }
        else if(parameter_1==KEYWORD_VERSION)
        {
            std::cout<<VERSION<<std::endl;
        }
        else if(parameter_1==KEYWORD_PROTOCOL_MSO2)
        {
            return Main::StartMSO2(parameter_2);
        }
        else
        {
            PrintHelp(argv[0]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        PrintHelp(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void Main::ChangeWorkingDir(char *argv_0)
{
    if(argv_0==nullptr)
        throw std::runtime_error("ChangeWorkingDir: null argv_0");

    std::string new_dir = argv_0;
    size_t pos = new_dir.find_last_of('/');
    if(pos==std::string::npos)
        throw std::runtime_error("ChangeWorkingDir: symbol not found");

    new_dir = new_dir.substr(0,pos+1);
    if(new_dir.empty())
        throw std::runtime_error("ChangeWorkingDir: path is empty");

    int result = chdir(new_dir.c_str());
    if(result)
    {
        std::string errno_message(strerror(errno));
        std::string error_message=
                "Error on changing working directory: "+new_dir+"\r\n"
                +errno_message;
        throw std::runtime_error(error_message);
    }
}

void Main::PrintHelp(const std::string program)
{
    std::string tmp;
    std::vector<std::string> lines;

    tmp=std::string("Version: ")+VERSION;
    lines.push_back(tmp);

    tmp="Usage:";
    lines.push_back(tmp);

    tmp=program+" [PROTOCOL] [CONFIGFILE] - For normal execution";
    lines.push_back(tmp);

    tmp="Default config file: "+CONFIG::FILENAME.Get();
    lines.push_back(tmp);

    tmp="Protocols:";
    lines.push_back(tmp);

    tmp=KEYWORD_PROTOCOL_MSO2+" - MSO2 Protocol";
    lines.push_back(tmp);

    for(auto it=lines.begin(); it!=lines.end(); it++)
        std::cout<<*it<<std::endl;

}

bool Main::Start_Config(const std::string config_path)
{
    const std::string LOGNAME = "Start_Config";
    ConfigFile config_file(config_path);

    config_file.AddLink(ConfigPropertyLink("CONTROLLER::INIT_INPUTS_STATE",
        &CONFIG::CONTROLLER::INIT_INPUTS_STATE));
    config_file.AddLink(ConfigPropertyLink("CONTROLLER::INIT_OUTPUTS_STATE",
        &CONFIG::CONTROLLER::INIT_OUTPUTS_STATE));
    config_file.AddLink(ConfigPropertyLink("CONTROLLER::AZIMUTH_SYNCED_ON_INIT",
        &CONFIG::CONTROLLER::AZIMUTH_SYNCED_ON_INIT));
    config_file.AddLink(ConfigPropertyLink("CONTROLLER::ELEVATION_SYNCED_ON_INIT",
        &CONFIG::CONTROLLER::ELEVATION_SYNCED_ON_INIT));
    config_file.AddLink(ConfigPropertyLink("CONTROLLER::FAST_SYNCHRONIZATION",
        &CONFIG::CONTROLLER::FAST_SYNCHRONIZATION));
    config_file.AddLink(ConfigPropertyLink("CONTROLLER::ENDL",
        &CONFIG::CONTROLLER::ENDL));
    config_file.AddLink(ConfigPropertyLink("TCP::PORT",
        &CONFIG::TCP::PORT));
    config_file.AddLink(ConfigPropertyLink("TCP::MAXBUFFERSIZE",
        &CONFIG::TCP::MAXBUFFERSIZE));
    config_file.AddLink(ConfigPropertyLink("ENGINE::ZERO_ELEVATION",
        &CONFIG::ENGINE::ZERO_ELEVATION));
    config_file.AddLink(ConfigPropertyLink("ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND",
        &CONFIG::ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND));
    config_file.AddLink(ConfigPropertyLink("ENGINE::MAX_ELEVATION_SPEED_PER_SECOND",
        &CONFIG::ENGINE::MAX_ELEVATION_SPEED_PER_SECOND));
    config_file.AddLink(ConfigPropertyLink("ENGINE::VEL_TO_EPS_COEFFICIENT",
        &CONFIG::ENGINE::VEL_TO_EPS_COEFFICIENT));
    config_file.AddLink(ConfigPropertyLink("LOG::MAX_SIZE_BYTES",
        &CONFIG::LOG::MAX_SIZE_BYTES));

    if(config_file.isFileExist())
    {
        config_file.ReadConfig();
        config_file.SyncToConfig();
    }
    else
    {
        if(config_path==("./"+CONFIG::FILENAME.Get()))
        {
            config_file.OverWriteConfig();
        }
        else
        {
            std::cerr<<"Error on reading config file"<<std::endl;
            return false;
        }
    }

    LOG.Start(CONFIG::LOG::FILENAME.Get(),static_cast<ssize_t>(CONFIG::LOG::MAX_SIZE_BYTES.Get()));
    LOG.WriteInfo(LOGMODULE,LOGNAME,VERSION);
    LOG.WriteDebug(LOGMODULE,LOGNAME,CONFIG::ToString());
    return true;
}

bool Main::Start_Protocol(Protocol_Base& protocol)
{
    MSO_State state;
    state.azimuth_syncronized=CONFIG::CONTROLLER::AZIMUTH_SYNCED_ON_INIT.Get();
    state.elevation_syncronized=CONFIG::CONTROLLER::ELEVATION_SYNCED_ON_INIT.Get();
    state.elevation_position=CONFIG::ENGINE::ZERO_ELEVATION.Get();
    std::string input_output_tmp_string = CONFIG::CONTROLLER::INIT_INPUTS_STATE.Get();
    for(size_t i=0; i<input_output_tmp_string.length();++i)
        state.inputs.push_back(input_output_tmp_string[i]=='0'?false:true);
    input_output_tmp_string = CONFIG::CONTROLLER::INIT_OUTPUTS_STATE.Get();
    for(size_t i=0; i<input_output_tmp_string.length();++i)
        state.outputs.push_back(input_output_tmp_string[i]=='0'?false:true);

    int port = CONFIG::TCP::PORT.Get();
    protocol.Init(state,port);
    return true;
}

inline std::string period_position_to_string(int val)
{
    std::string pos=std::to_string(val);
    while(pos.length()<5)
        pos.insert(pos.begin(),'0');
    return pos;
}

inline std::string period_speed_to_string(int val)
{
    std::string vel=std::to_string(val);
    if(val>=0)
        vel.insert(vel.begin(),'+');
    while(vel.length()<5)
        vel.insert(vel.begin()+1,'0');
    return vel;
}

bool Main::Start_Loop(Protocol_Base& protocol)
{
    const std::string LOGNAME = "Start_Loop";
    try
    {
        bool need_to_redraw_picture = true;
        Drawing picture(CONFIG::DRAWING::HEIGHT.Get());
        MSO_State previous_state, current_state;
        int sleep_time = CONFIG::CONTROLLER::TICK_MS.Get();
        while(true)
        {
            previous_state = current_state;
            current_state = protocol.Tick();
            if(previous_state.isChanged(current_state))
            {
                need_to_redraw_picture=true;
                if(current_state.azimuth_position!=previous_state.azimuth_position)
                    picture.SetAzimuth(current_state.azimuth_position);
                if(current_state.elevation_position!=previous_state.elevation_position)
                    picture.SetElevation(current_state.elevation_position);
            }
            if( (previous_state.tcp_last_got_data!=current_state.tcp_last_got_data)
              ||
                (previous_state.tcp_last_sent_data!=current_state.tcp_last_sent_data))
            {
                need_to_redraw_picture=true;
            }

            if(need_to_redraw_picture)
            {
                std::string timestamp ="Time: "+Utils::GetCurrentTime();
                std::string text_a_pos=
                        period_position_to_string(current_state.azimuth_position);
                std::string text_e_pos=
                        period_position_to_string(current_state.elevation_position);
                std::string text_a_vel=
                        period_speed_to_string(current_state.azimuth_speed);
                std::string text_e_vel=
                        period_speed_to_string(current_state.elevation_speed);

                std::string position="Azimuth: "+text_a_pos+" ("+text_a_vel+") "
                    +(current_state.azimuth_syncronized?"":"NOSYNC")
                    +" Elevation: "+text_e_pos+" ("+text_e_vel+") "
                    +(current_state.elevation_syncronized?"":"NOSYNC");

                std::string text_io_nums="|";
                std::string text_i_stat="|";
                std::string text_o_stat="|";
                for(int i=static_cast<int>(current_state.inputs.size())-1;i>=0;--i)
                {
                    if(current_state.inputs.size()>=current_state.outputs.size())
                        text_io_nums+=(i<10?"0":"")+std::to_string(i)+"|";
                    text_i_stat+=(current_state.inputs[static_cast<size_t>(i)]?"ON|":"##|");
                }

                for(int i=static_cast<int>(current_state.outputs.size())-1;i>=0;--i)
                {
                    if(current_state.inputs.size()<current_state.outputs.size())
                        text_io_nums+=(i<10?"0":"")+std::to_string(i)+"|";
                    text_o_stat+=(current_state.outputs[static_cast<size_t>(i)]?"ON|":"##|");
                }

                std::string picture_header=
                    timestamp+"\r\n"
                    +"Last command: "+current_state.tcp_last_got_data+"\r\n"
                    +position+"\r\n"
                    +"INPUTS:  "+text_io_nums+"\r\n"
                    +"         "+text_i_stat +"\r\n"
                    +"OUTPUTS: "+text_io_nums+"\r\n"
                    +"         "+text_o_stat +"\r\n"
                    +"\r\n";
                std::string picture_body = picture.PaintAll();

                Utils::ClearScreen();
                std::cout<<picture_header<<picture_body<<std::endl;
            }

            Utils::Sleep(sleep_time);
            need_to_redraw_picture=false;
        }
    }
    catch(std::exception& e)
    {
        LOG.WriteCrit(LOGMODULE,LOGNAME,"Exception: "+std::string(e.what()));
        return false;
    }
    catch(...)
    {
        LOG.WriteCrit(LOGMODULE,LOGNAME,"Unknown error");
        return false;
    }
    return true;
}







int Main::StartMSO2(std::string config_path)
{
    const std::string LOGNAME = "StartMSO2";
    if(!Start_Config(config_path))
        return EXIT_FAILURE;
    Protocol_MSO2 protocol;
    if(!Start_Protocol(protocol))
        return EXIT_FAILURE;
    if(!Start_Loop(protocol))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}


