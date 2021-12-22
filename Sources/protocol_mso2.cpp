#include "protocol_mso2.h"

#include <sstream>
#include <algorithm>
#include <chrono>

#include "log.h"

const std::string LOGMODULE="Protocol_MSO2";

const char DELIM = ' ';
const std::string WORD_A        = "a";
const std::string WORD_E        = "e";
const std::string WORD_SYN      = "syn";
const std::string WORD_GSYN     = "gsyn";
const std::string WORD_POS      = "pos";
const std::string WORD_VEL      = "vel";
const std::string WORD_GET      = "get";
const std::string WORD_SMIN     = "smin";
const std::string WORD_SMAX     = "smax";
const std::string WORD_GMIN     = "gmin";
const std::string WORD_GMAX     = "gmax";
const std::string WORD_VOL      = "vol";
const std::string WORD_SERIAL   = "s";
const std::string WORD_MODE     = "mode";
const std::string WORD_DATA     = "data";
const std::string WORD_I        = "i";
const std::string WORD_O        = "o";
const std::string WORD_G        = "g";
const std::string WORD_RESET    = "reset";
const std::string WORD_PERIOD   = "period";
const std::string WORD_INFO     = "info";
const std::string WORD_I_SET        = "iset";
const int MIN_VOL=225;
const int MAX_VOL=255;

inline std::string CreateAnswerString(std::string text)
{
    return "s0 data "+std::to_string(text.length())+" "+text+CONFIG::CONTROLLER::ENDL.Get();
}

inline int random_vol(int min=MIN_VOL,int max=MAX_VOL)
{
    int result=min+rand()%(max-min);
    return result;
}

Protocol_MSO2::~Protocol_MSO2()
{
    need_to_destroy_thread.store(true);
    if(thread_period!=nullptr && thread_period->joinable())
        thread_period->join();
}

void Protocol_MSO2::ProtocolInit()
{
    CreateCommandMap();
}

void Protocol_MSO2::CreateCommandMap()
{
    CommandMap.clear();
    CommandMap[""]=UNKNOWN;
    CommandMap[WORD_A+" "+WORD_SYN]=CommandType::AZIMUTH_SET_SYNCHRONIZATION;
    CommandMap[WORD_E+" "+WORD_SYN]=CommandType::ELEVATION_SET_SYNCHRONIZATION;
    CommandMap[WORD_A+" "+WORD_GSYN]=CommandType::AZIMUTH_GET_SYNCHRONIZATION;
    CommandMap[WORD_E+" "+WORD_GSYN]=CommandType::ELEVATION_GET_SYNCHRONIZATION;
    CommandMap[WORD_A+" "+WORD_POS]=CommandType::AZIMUTH_SET_POSITION;
    CommandMap[WORD_E+" "+WORD_POS]=CommandType::ELEVATION_SET_POSITION;
    CommandMap[WORD_A+" "+WORD_VEL]=CommandType::AZIMUTH_SET_VELOCITY;
    CommandMap[WORD_E+" "+WORD_VEL]=CommandType::ELEVATION_SET_VELOCITY;
    CommandMap[WORD_A+" "+WORD_GET]=CommandType::AZIMUTH_GET_POSITION;
    CommandMap[WORD_E+" "+WORD_GET]=CommandType::ELEVATION_GET_POSITION;
    CommandMap[WORD_E+" "+WORD_SMIN]=CommandType::ELEVATION_SET_MINIMUM_POSITION;
    CommandMap[WORD_E+" "+WORD_SMAX]=CommandType::ELEVATION_SET_MAXIMUM_POSITION;
    CommandMap[WORD_E+" "+WORD_GMIN]=CommandType::ELEVATION_GET_MINIMUM_POSITION;
    CommandMap[WORD_E+" "+WORD_GMAX]=CommandType::ELEVATION_GET_MAXIMUM_POSITION;
    CommandMap[WORD_A+" "+WORD_VOL]=CommandType::AZIMUTH_GET_VOLTAGE;
    CommandMap[WORD_E+" "+WORD_VOL]=CommandType::ELEVATION_GET_VOLTAGE;
    for(int i=0;i<=CONFIG::CONTROLLER::ADDITIONAL_SERIAL_LINES.Get();++i)
    {
        std::string s = WORD_SERIAL+std::to_string(i);
        CommandMap[s+" "+WORD_MODE]=CommandType::SERIAL_SET_MODE;
        CommandMap[s+" "+WORD_DATA]=CommandType::SERIAL_SEND_DATA;
    }
    CommandMap[WORD_I]=CommandType::INPUTS_GET;
    CommandMap[WORD_O]=CommandType::OUTPUTS_SET;
    CommandMap[WORD_G]=CommandType::OUTPUTS_GET;
    CommandMap[WORD_RESET]=CommandType::RESET;
    CommandMap[WORD_PERIOD]=CommandType::PERIOD;
    CommandMap[WORD_INFO]=CommandType::INFO;
    CommandMap[WORD_I_SET]=CommandType::INPUTS_SET;
}

Protocol_Base::Command Protocol_MSO2::RecognizeCommand(const std::string command) const
{
    const std::string LOGNAME="RecognizeCommand";
    Protocol_Base::Command result;

    {
        std::istringstream iss(command);
        for (std::string token; std::getline(iss, token, DELIM); )
        {
            result.parameters.push_back(move(token));
        }
    }

    {
        if(result.parameters.size())
        {
            std::string param_1 = result.parameters[0];
            bool azimuth_command = (param_1 == WORD_A);
            bool elevation_command = (param_1 == WORD_E);
            bool serial_command = (param_1[0]  == WORD_SERIAL[0]);
            if( (azimuth_command || elevation_command || serial_command)
                && (result.parameters.size()>1))
            {
                std::string param_2 = result.parameters[1];
                std::string multi_word_command = param_1+DELIM+param_2;
                std::unordered_map<std::string, Protocol_Base::CommandType>::const_iterator ptr = CommandMap.find(multi_word_command);
                if(ptr != CommandMap.end())
                {
                     result.command=ptr->second;
                }
            }
            else
            {
                std::unordered_map<std::string, Protocol_Base::CommandType>::const_iterator ptr = CommandMap.find(param_1);
                if(ptr != CommandMap.end())
                {
                     result.command=ptr->second;
                }
            }
        }
    }

    {
        if(result.command==UNKNOWN)
        {
            std::string header="Unknown command: "+command+"\r\n";
            std::string code_header="LENGTH="+std::to_string(command.length())+" CODE:\r\n";
            std::string code="";
            for(unsigned int i=0;i<command.length();++i)
            {
                int val = command[i];
                code+="["+std::to_string(val)+"] ";
            }
            std::string msg=header+code_header+code;
            LOG.WriteError(LOGMODULE,LOGNAME,msg);
        }
    }

    return result;
}

void Protocol_MSO2::ProtocolTick()
{
    state.timestamp=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    state.voltage=random_vol();

    state.azimuth_position=engine.GetAzimuthPosition();
    state.azimuth_speed=engine.GetAzimuthSpeed();
    state.elevation_position=engine.GetElevationPosition();
    state.elevation_speed=engine.GetElevationSpeed();

    if(tcp_server.IsDataAvailable())
    {
        ProcessTcpIncomingData();
    }

    if(isProcessing_azimuth_synchronization)
        Process_azimuth_synchronization();
    if(isProcessing_elevation_synchronization)
        Process_elevation_synchronization();

    if(isProcessing_azimuth_positioning
            && (state.azimuth_position==processing_azimuth_positioning_target))
    {
        isProcessing_azimuth_positioning=false;

        std::string tcp_message=CreateAnswerString("a end");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);

    }

    if(isProcessing_elevation_positioning
            && (state.elevation_position==processing_elevation_positioning_target))
    {
        isProcessing_elevation_positioning=false;

        std::string tcp_message=CreateAnswerString("e end");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
    }
}

void Protocol_MSO2::ProcessTcpIncomingData()
{
    std::string LOGNAME ="ProcessTcpIncomingData";
    std::string command_line = GetCommandString();
    state.tcp_last_got_data = command_line;
    LOG.WriteDebug(LOGMODULE,LOGNAME,command_line);

    Command command = RecognizeCommand(command_line);

    switch(command.command)
    {
    case Protocol_Base::AZIMUTH_SET_SYNCHRONIZATION:
        isProcessing_azimuth_synchronization = true;
        break;

    case Protocol_Base::ELEVATION_SET_SYNCHRONIZATION:
        isProcessing_elevation_synchronization = true;
        break;

    case Protocol_Base::AZIMUTH_GET_SYNCHRONIZATION:
    {
        std::string tcp_message=CreateAnswerString(std::string("a syn ")+(state.azimuth_syncronized?"1":"0"));
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::ELEVATION_GET_SYNCHRONIZATION:
    {
        std::string tcp_message=CreateAnswerString(std::string("e syn ")+(state.elevation_syncronized?"1":"0"));
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::AZIMUTH_SET_POSITION:
        Cmd_a_pos(command);
        break;

    case Protocol_Base::ELEVATION_SET_POSITION:
        Cmd_e_pos(command);
        break;

    case Protocol_Base::AZIMUTH_GET_POSITION:
    {
        int value = state.azimuth_position;
        std::string text = std::to_string(value);
        std::string tcp_message=CreateAnswerString("a get "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::ELEVATION_GET_POSITION:
    {
        int value = state.elevation_position;
        std::string text = std::to_string(value);
        std::string tcp_message=CreateAnswerString("e get "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::AZIMUTH_SET_VELOCITY:
        Cmd_a_vel(command);
        break;

    case Protocol_Base::ELEVATION_SET_VELOCITY:
        Cmd_e_vel(command);
        break;

    case Protocol_Base::ELEVATION_SET_MINIMUM_POSITION:
        Cmd_e_smin(command);
        break;

    case Protocol_Base::ELEVATION_GET_MINIMUM_POSITION:
    {
        int value = CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get();
        std::string text = std::to_string(value);
        std::string tcp_message=CreateAnswerString("e gmin "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::ELEVATION_SET_MAXIMUM_POSITION:
        Cmd_e_smax(command);
        break;

    case Protocol_Base::ELEVATION_GET_MAXIMUM_POSITION:
    {
        int value = CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get();
        std::string text = std::to_string(value);
        std::string tcp_message=CreateAnswerString("e gmax "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::AZIMUTH_GET_VOLTAGE:
    {
        int value = state.voltage;
        std::string text = std::to_string(value);
        std::string tcp_message=CreateAnswerString("a vol "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::ELEVATION_GET_VOLTAGE:
    {
        int value = state.voltage;
        std::string text = std::to_string(value);
        std::string tcp_message=CreateAnswerString("e vol "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::INPUTS_SET:
        Cmd_i_set(command);
        break;

    case Protocol_Base::INPUTS_GET:
    {
        std::string text = "";
        for(uint32_t idx=0; idx<state.inputs.size();++idx)
        {
            if(state.inputs[idx])
                text+='1';
            else
                text+='0';
        }
        std::string tcp_message=CreateAnswerString("i "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::OUTPUTS_SET:
        Cmd_o_set(command);
        break;

    case Protocol_Base::OUTPUTS_GET:
    {
        std::string text = "";
        for(uint32_t idx=0; idx<state.outputs.size();++idx)
        {
            if(state.outputs[idx])
                text+='1';
            else
                text+='0';
        }
        std::string tcp_message=CreateAnswerString("g "+text);
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);
        break;
    }

    case Protocol_Base::RESET:
        Cmd_reset();
        break;

    case Protocol_Base::PERIOD:
        Cmd_period(command);
        break;
    case Protocol_Base::INFO:
    {
        std::string L1 =">>type             mso-2, los";
        std::string L2 =">>en303 version    3.3.0.0";
        std::string L3 =">>dsp version      2.5";
        std::string L4 =">>serial number    00000000000000";
        std::string L5 =">>MAC address      00-00-00-00-00-00";
        std::string L6 =">>IP address             0.0.0.0";
        std::string L7 =">>IP net mask    255.255.255.255";
        std::string L8 =">>gateway              0.0.0.0";
        std::string L9=">>file size        221252";
        std::string L10=">>file date         2005-09-14 14:14:50";
        std::string L11=">>CRC              A9340E6B";

        tcp_server.Send(CreateAnswerString(L1));
        tcp_server.Send(CreateAnswerString(L2));
        Utils::Sleep(5);
        tcp_server.Send(CreateAnswerString(L3));
        tcp_server.Send(CreateAnswerString(L4));
        Utils::Sleep(5);
        tcp_server.Send(CreateAnswerString(L5));
        tcp_server.Send(CreateAnswerString(L6));
        Utils::Sleep(5);
        tcp_server.Send(CreateAnswerString(L7));
        tcp_server.Send(CreateAnswerString(L8));
        Utils::Sleep(5);
        tcp_server.Send(CreateAnswerString(L9));
        tcp_server.Send(CreateAnswerString(L10));
        Utils::Sleep(5);
        tcp_server.Send(CreateAnswerString(L11));
        Utils::Sleep(5);
        state.tcp_last_sent_data = CreateAnswerString(L11);
        break;
    }

    default: break;
    }
}

std::string Protocol_MSO2::GetCommandString()
{
    std::string command=tcp_server.Recv();
    if(!command.empty())
    {
        std::string::size_type pos=command.find('\r');
        while(pos!=std::string::npos)
        {
            command.erase(pos,1);
            //command.replace(pos,1,"\0");
            pos=command.find('\r');
        }
        pos=command.find('\n');
        while(pos!=std::string::npos)
        {
            command.erase(pos,1);
            //command.replace(pos,1,"\0");
            pos=command.find('\n');
        }

        std::for_each(command.begin(), command.end(), [](char & c){
                c = static_cast<char>(::tolower(c));
            });
    }
    return command;
}

void Protocol_MSO2::Process_azimuth_synchronization()
{
    static int previous_position=0;

    if(!state.azimuth_syncronized)
    {
        if(CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Get())
        {
            engine.SetAzimuthPosition(0);
            if(engine.GetAzimuthPosition()==0)
                state.azimuth_syncronized=true;
        }
        else
        {
            int max_speed = Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND.Get());
            int current_position=engine.GetAzimuthPosition();
            engine.SetAzimuthSpeed(max_speed);
            if(current_position<previous_position)
            {
                engine.SetAzimuthPosition(0);
                if(current_position==0)
                    state.azimuth_syncronized=true;
            }
            previous_position=current_position;
        }
    }
    else
    {
        std::string tcp_message=CreateAnswerString("a syn ok");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);

        isProcessing_azimuth_synchronization=false;
        previous_position=0;
    }
}

void Protocol_MSO2::Process_elevation_synchronization()
{
    static int zero_speed_counter = 0;
    static char direction = +1;
    static bool return_to_zero = false;
    const int ZERO_SPEED_LIMIT =3;

    if(!state.elevation_syncronized)
    {
        if(CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Get())
        {
            int position = CONFIG::ENGINE::ZERO_ELEVATION.Get();
            engine.SetElevationPosition(position);
            if(engine.GetElevationPosition()==position)
                state.elevation_syncronized=true;
        }
        else
        {
            if(return_to_zero)
            {
                int zero_elevation = CONFIG::ENGINE::ZERO_ELEVATION.Get();
                engine.SetElevationPosition(zero_elevation);
                if(engine.GetElevationPosition()==zero_elevation)
                    state.elevation_syncronized=true;
            }
            else
            {
                int max_speed = Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_ELEVATION_SPEED_PER_SECOND.Get());
                engine.SetElevationSpeed(max_speed*direction);
                int current_speed = engine.GetElevationSpeed();

                if(current_speed==0)
                {
                    ++zero_speed_counter;
                    if(zero_speed_counter>=ZERO_SPEED_LIMIT)
                    {
                        zero_speed_counter=0;
                        if(direction>0)
                        {
                            direction=-1;
                        }
                        else
                        {
                            return_to_zero=true;
                        }
                    }
                }
                else
                {
                    zero_speed_counter=0;
                }
            }
        }
    }
    else
    {
        std::string tcp_message=CreateAnswerString("e syn ok");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);

        isProcessing_elevation_synchronization=false;
        return_to_zero=false;
        zero_speed_counter = 0;
        direction = +1;
    }
}

void Protocol_MSO2::Cmd_a_pos(const Command& command)
{
    if(state.azimuth_syncronized)
    {
        if(command.parameters.size()>2)
        {
            try
            {
                int val = std::stoi( command.parameters[2] );
                isProcessing_azimuth_positioning=true;
                processing_azimuth_positioning_target=val;
                engine.SetAzimuthPosition(val);
            }
            catch(...)
            {
            }
        }
    }
    else
    {
        std::string tcp_message=CreateAnswerString("a err 2");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);

    }
}

void Protocol_MSO2::Cmd_e_pos(const Command& command)
{
    if(state.elevation_syncronized)
    {
        if(command.parameters.size()>2)
        {
            try
            {
                int val = std::stoi( command.parameters[2] );
                isProcessing_elevation_positioning=true;
                processing_elevation_positioning_target=val;
                engine.SetElevationPosition(val);
            }
            catch(...)
            {
            }
        }
    }
    else
    {
        std::string tcp_message=CreateAnswerString("e err 2");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);

    }
}

void Protocol_MSO2::Cmd_a_vel(const Command& command)
{
    if(state.azimuth_syncronized)
    {
        if(command.parameters.size()>2)
        {
            try
            {
                int val = std::stoi( command.parameters[2] );
                engine.SetAzimuthSpeed(val);
                isProcessing_azimuth_positioning=false;
            }
            catch(...)
            {
            }
        }
    }
    else
    {
        std::string tcp_message=CreateAnswerString("a err 2");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);

    }
}

void Protocol_MSO2::Cmd_e_vel(const Command& command)
{
    if(state.elevation_syncronized)
    {
        if(command.parameters.size()>2)
        {
            try
            {
                int val = std::stoi( command.parameters[2] );
                engine.SetElevationSpeed(val);
                isProcessing_elevation_positioning=false;
            }
            catch(...)
            {
            }
        }
    }
    else
    {
        std::string tcp_message=CreateAnswerString("e err 2");
        state.tcp_last_sent_data = tcp_message;
        tcp_server.Send(tcp_message);

    }
}

void Protocol_MSO2::Cmd_e_smin(const Command& command)
{
    try
    {
        if(command.parameters.size()>2)
        {
            int val = std::stoi( command.parameters[2] );
            if(val<=CONFIG::ENGINE::ZERO_ELEVATION.Get() && val>=CONFIG::ENGINE::HARD_LIMIT_MIN_ELEVATION.Get())
            {
                CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Set(val);
            }
        }
    }
    catch(...)
    {
    }
}

void Protocol_MSO2::Cmd_e_smax(const Command& command)
{
    try
    {
        if(command.parameters.size()>2)
        {
            int val = std::stoi( command.parameters[2] );
            if(val>=CONFIG::ENGINE::ZERO_ELEVATION.Get() && val<=CONFIG::ENGINE::HARD_LIMIT_MAX_ELEVATION.Get())
            {
                CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Set(val);
            }
        }
    }
    catch(...)
    {
    }
}

void Protocol_MSO2::Cmd_i_set(const Command& command)
{
    try
    {
        if(command.parameters.size()>2)
        {
            int32_t num = std::stoi( command.parameters[1] );
            int32_t val_int = std::stoi( command.parameters[2] );
            if(val_int==0 || val_int==1)
            {
                bool val = val_int;
                if(num>=0 && num<=NUMBER_OF_INPUTS)
                {
                    state.inputs[static_cast<uint32_t>(num)]=val;
                }
            }
        }
        else
        {
            if(command.parameters.size()==2)
                if(command.parameters[1]=="all")
                    for(int i=0;i<NUMBER_OF_INPUTS;++i)
                    {
                        state.inputs[static_cast<uint32_t>(i)]=true;
                    }
        }

    }
    catch(...)
    {
    }
}

void Protocol_MSO2::Cmd_o_set(const Command& command)
{
    try
    {
        if(command.parameters.size()>2)
        {
            int32_t num = std::stoi( command.parameters[1] );
            int32_t val_int = std::stoi( command.parameters[2] );
            if(val_int==0 || val_int==1)
            {
                bool val = val_int;
                if(num>=0 && num<=NUMBER_OF_INPUTS)
                {
                    state.outputs[static_cast<uint32_t>(num)]=val;
                }
            }
        }
    }
    catch(...)
    {
    }
}

void Protocol_MSO2::Cmd_reset()
{
    //const int port = tcp_server.Port();
    //tcp_server.Stop();
    while(tcp_server.IsDataAvailable())
        tcp_server.Recv();
    engine.Stop();

    state.azimuth_syncronized = CONFIG::CONTROLLER::AZIMUTH_SYNCED_ON_INIT.Get()?true:false;
    state.elevation_syncronized = CONFIG::CONTROLLER::ELEVATION_SYNCED_ON_INIT.Get()?true:false;
    for(uint32_t k=0;k<state.inputs.size();++k)
    {
        state.inputs[k]=(CONFIG::CONTROLLER::INIT_INPUTS_STATE.Get()[k]=='1');
    }
    for(uint32_t k=0;k<state.outputs.size();++k)
    {
        state.outputs[k]=(CONFIG::CONTROLLER::INIT_OUTPUTS_STATE.Get()[k]=='1');
    }

    isProcessing_azimuth_synchronization = false;
    isProcessing_elevation_synchronization = false;
    isProcessing_azimuth_positioning = false;
    isProcessing_elevation_positioning = false;
    need_to_send_period.store(false);

    Utils::Sleep(100);
    engine.Start();
    //tcp_server.Start(port);
}

void Protocol_MSO2::Cmd_period(const Command& command)
{
    if(command.parameters.size()>1)
    {
        try
        {
            int val = std::stoi( command.parameters[1] );

            if(val)
                need_to_send_period.store(true);
                else
                need_to_send_period.store(false);

            period_intervals_by_10_ms.store(val);

            if(thread_period==nullptr)
            {
                thread_period = new std::thread(Protocol_MSO2::period_thread_procedure, this);
            }
        }
        catch(...)
        {
        }
    }
}

void Protocol_MSO2::period_thread_procedure(Protocol_MSO2* obj)
{
    const std::string LOGNAME="period_thread_procedure";
    if(obj == nullptr)
        return;
    try
    {
        Utils::SetThreadName("period");
        obj->PeriodLoop();
    }
    catch(std::exception& ex)
    {
        std::string msg = std::string("PeriodThreadException: ")+ex.what();
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }
    catch(...)
    {
        std::string msg = "PeriodThreadException";
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }
}

void Protocol_MSO2::PeriodLoop()
{
    int interval=0;
    while(!need_to_destroy_thread.load())
    {
        if(need_to_send_period.load())
        {
            ++interval;
            if(interval>=period_intervals_by_10_ms.load())
            {
                interval=0;
                Send_period();
            }
        }
        else
        {
            interval=0;
        }
        Utils::Sleep(MIN_PERIOD_INTERVAL_MS);
    }
}

void Protocol_MSO2::Send_period()
{
    //s0 data 48 per <a pos> <e pos> <a vel> <e vel> <vol> <inputs>
    //s0 data 48 per 18987 15502 +0000 +0000 207 1111111100111111
    std::string a_pos = std::to_string(engine.GetAzimuthPosition());
    while(a_pos.length()<5)
        a_pos.insert(a_pos.begin(),'0');

    std::string e_pos = std::to_string(engine.GetElevationPosition());
    while(e_pos.length()<5)
        e_pos.insert(e_pos.begin(),'0');

    int a_speed = engine.GetAzimuthSpeed();
    std::string a_vel = std::to_string(a_speed);
    while(a_vel.length()<4)
        a_vel.insert(a_vel.begin(),'0');

    int e_speed = engine.GetElevationSpeed();
    std::string e_vel = std::to_string(e_speed);
    while(e_vel.length()<4)
        e_vel.insert(e_vel.begin(),'0');

    std::string vol = std::to_string(random_vol());
    while(vol.length()<3)
        vol.insert(vol.begin(),'0');

    std::string inputs ="";
    for(uint32_t k=0; k<state.inputs.size();++k)
        if(state.inputs[k])
            inputs.append("1");
            else
            inputs.append("0");

    std::string period = "per "+a_pos + " " + e_pos
            + (a_speed>=0?" +":" -") + a_vel + (e_speed>=0?" +":" -") + e_vel
            + " " + vol + " " + inputs;

    period=CreateAnswerString(period);
    state.tcp_last_sent_data=period;
    tcp_server.Send(period);
}
