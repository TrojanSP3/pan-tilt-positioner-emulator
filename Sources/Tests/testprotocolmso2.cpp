#include "testprotocolmso2.h"

#include <sstream>

#include "../protocol_mso2.h"

using namespace UnitTests;

const std::string CLRF = "\r\n";
const int TestProtocolMSO2::SHORT_SLEEP=100;
const int TestProtocolMSO2::LONG_TIMEOUT=5000;
const int TestProtocolMSO2::BASE_PORT=21000;
int TestProtocolMSO2::PORT_CNT=0;
//const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;

std::vector<TestCase> TestProtocolMSO2::GetTestCases()
{
    const std::string LOGMODULE="TestProtocolMSO2";
    std::vector<TestCase> result;
    result.push_back(TESTCASE(Create_Destroy));
    result.push_back(TESTCASE(Init));
    result.push_back(TESTCASE(Init_ticks));
    result.push_back(TESTCASE(Client_connected));
    result.push_back(TESTCASE(Info_and_Voltage));
    result.push_back(TESTCASE(Azimuth_syn));
    result.push_back(TESTCASE(Azimuth_syn_fast));
    result.push_back(TESTCASE(Elevation_syn));
    result.push_back(TESTCASE(Elevation_syn_fast));
    result.push_back(TESTCASE(Positioning));
    result.push_back(TESTCASE(Speed));
    result.push_back(TESTCASE(Elevation_limits));
    result.push_back(TESTCASE(Inputs_outputs));
    result.push_back(TESTCASE(Period));
    result.push_back(TESTCASE(Reset));

    return result;
}

inline MSO_State GetInitState()
{
    MSO_State result;
    for(int i=0;i<Protocol_MSO2::NUMBER_OF_INPUTS;i++)
    {
        result.inputs.push_back(false);
    }
    for(int i=0;i<Protocol_MSO2::NUMBER_OF_INPUTS;i++)
    {
        result.outputs.push_back(false);
    }
    result.elevation_position=CONFIG::ENGINE::ZERO_ELEVATION.Get();
    return result;
}

inline void PrintMSOState(const MSO_State& state)
{
    std::stringstream inputs;
    for(uint32_t i=0; i<state.inputs.size();++i)
    {
        inputs<<(state.inputs[i]?"1":"0");
    }
    std::stringstream outputs;
    for(uint32_t i=0; i<state.outputs.size();++i)
    {
        outputs<<(state.outputs[i]?"1":"0");
    }
    std::stringstream ss;
    ss<<"Current MSO state ["<<std::to_string(state.timestamp)<<"]:"<<std::endl
      <<"Azimuth: "<<std::to_string(state.azimuth_position)
      <<" ("<<std::to_string(state.azimuth_speed)<<") "
      <<(state.azimuth_syncronized?"SYNC":"")<<std::endl
      <<"Elevation: "<<std::to_string(state.elevation_position)
      <<" ("<<std::to_string(state.elevation_speed)<<") "
      <<(state.elevation_syncronized?"SYNC":"")<<std::endl
      <<"Voltage: "
      <<std::to_string(state.voltage)<<std::endl
      <<"Inputs : "<<inputs.str()<<std::endl
      <<"Outputs: "<<outputs.str()<<std::endl
      <<"TCP in : "<<state.tcp_last_got_data<<std::endl
      <<"TCP out: "<<state.tcp_last_sent_data<<std::endl;
    TestFramework::LogMsg(ss.str());

}

void TestProtocolMSO2::Create_Destroy()
{
    Protocol_MSO2* obj=new Protocol_MSO2();
    Protocol_Base* ptr = obj;
    delete ptr;
}

void TestProtocolMSO2::Init()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);
}

void TestProtocolMSO2::Init_ticks()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    Protocol_MSO2 protocol;
    MSO_State base_state = GetInitState();
    protocol.Init(base_state,PORT);

    for(int i=1; i<=100;i++)
    {
        MSO_State new_state = protocol.Tick();
        ASSERT(!base_state.isChanged(new_state));
    }
}

void TestProtocolMSO2::Client_connected()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const std::string MSG = "hello";

    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);
    protocol.Tick();

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    protocol.Tick();

    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);
    client.WriteLine(MSG+CLRF);
    SLEEPMS(SHORT_SLEEP);
    MSO_State state = protocol.Tick();

    client.Close();
    state=protocol.Tick();

    TestFramework::LogMsg("Tcp last: "+state.tcp_last_got_data);
    TestFramework::LogMsg("MSG: "+MSG);
    ASSERT(state.tcp_last_got_data==MSG)
}

void TestProtocolMSO2::Info_and_Voltage()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;

    const int MSG_info_lines        = 11;
    const std::string MSG_info      = "info";
    const std::string MSG_voltage   = "a vol";

    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    client.WriteLine(MSG_info+CLRF);
    SLEEPMS(SHORT_SLEEP);
    protocol.Tick();

    int cnt = 0;
    TIMEOUT(client.BytesAvailable()>0,LONG_TIMEOUT);
    while(client.BytesAvailable()>0)
    {
        TestFramework::LogMsg("Reading line...");
        std::string line = client.ReadLine();
        TestFramework::LogMsg(line);
        cnt++;
        SLEEPMS(SHORT_SLEEP);
    }

    if(cnt != MSG_info_lines)
        EXCEPTION("Wrong info answer");

    client.WriteLine(MSG_voltage+CLRF);
    SLEEPMS(SHORT_SLEEP);
    MSO_State state = protocol.Tick();

    TIMEOUT(client.BytesAvailable()>0,LONG_TIMEOUT);

    std::string line = client.ReadLine();//s0 data 9 a vol 000
    line=line.substr(16,3);
    int val = 0;
    try
    {
        val = std::stoi(line);
    }
    catch(...)
    {
        EXCEPTION("Wrong conversion");
    }

    ASSERT(state.voltage==val);

    client.Close();
}

void TestProtocolMSO2::Azimuth_syn()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const int TIMEOUT_SEC = 60;
    const std::string MSG_syn      = "a syn\r\n";

    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Set(false);
    MSO_State state = protocol.Tick();
    ASSERT(!state.azimuth_syncronized);
    client.WriteLine(MSG_syn);
    SLEEPMS(SHORT_SLEEP);

    int counter = 0;
    while(counter<TIMEOUT_SEC)
    {
        counter++;
        state = protocol.Tick();
        std::string msg = "["+std::to_string(counter)
                +"/"+std::to_string(TIMEOUT_SEC)+"] "
                +"Azimuth: "+std::to_string(state.azimuth_position)
                +" Speed: "+std::to_string(state.azimuth_speed)
                +" Sync: "+(state.azimuth_syncronized?"YES":"NO");
        TestFramework::LogMsg(msg);
        if(state.azimuth_syncronized)
            break;
        SLEEPMS(1000);
    }
    client.Close();
    ASSERT(state.azimuth_syncronized);
    ASSERT(state.azimuth_position==0);
    ASSERT(state.azimuth_speed==0);
}

void TestProtocolMSO2::Azimuth_syn_fast()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const std::string MSG_syn      = "a syn\r\n";

    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Set(true);
    MSO_State state = protocol.Tick();
    ASSERT(!state.azimuth_syncronized);
    client.WriteLine(MSG_syn);
    SLEEPMS(SHORT_SLEEP);

    client.Close();
    TIMEOUT(protocol.Tick().azimuth_syncronized,LONG_TIMEOUT);
    ASSERT(state.azimuth_position==0);
    ASSERT(state.azimuth_speed==0);
}

void TestProtocolMSO2::Elevation_syn()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const int TIMEOUT_SEC = 60;
    const std::string MSG_syn      = "e syn\r\n";

    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Set(false);
    MSO_State state = protocol.Tick();
    ASSERT(!state.elevation_syncronized);
    client.WriteLine(MSG_syn);
    SLEEPMS(SHORT_SLEEP);

    int counter = 0;
    while(counter<TIMEOUT_SEC)
    {
        counter++;
        state = protocol.Tick();
        std::string msg = "["+std::to_string(counter)
                +"/"+std::to_string(TIMEOUT_SEC)+"] "
                +"Elevation: "+std::to_string(state.elevation_position)
                +" Speed: "+std::to_string(state.elevation_speed)
                +" Sync: "+(state.elevation_syncronized?"YES":"NO");
        TestFramework::LogMsg(msg);
        if(state.elevation_syncronized)
            break;
        SLEEPMS(1000);
    }
    client.Close();
    ASSERT(state.elevation_syncronized);
    ASSERT(state.elevation_position==CONFIG::ENGINE::ZERO_ELEVATION.Get());
    ASSERT(state.elevation_speed==0);
}

void TestProtocolMSO2::Elevation_syn_fast()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const std::string MSG_syn      = "e syn\r\n";

    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Set(true);
    MSO_State state = protocol.Tick();
    ASSERT(!state.elevation_syncronized);
    client.WriteLine(MSG_syn);
    SLEEPMS(SHORT_SLEEP);

    state = protocol.Tick();
    client.Close();
    TIMEOUT(protocol.Tick().elevation_syncronized,LONG_TIMEOUT);
    ASSERT(state.elevation_position==CONFIG::ENGINE::ZERO_ELEVATION.Get());
    ASSERT(state.elevation_speed==0);
}

void TestProtocolMSO2::Positioning()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const int DIF_VALUE = 1000;
    const std::string MSG_syn      = "a syn\r\ne syn\r\n";
    const int POS_A_1 = +DIF_VALUE;
    const int POS_A_2 = CONFIG::ENGINE::MAX_AZIMUTH.Get()/2+DIF_VALUE;
    const std::string MSG_a_pos_1      = "a pos "+std::to_string(POS_A_1)+"\r\n";
    const std::string MSG_a_pos_2      = "a pos "+std::to_string(POS_A_2)+"\r\n";
    const int POS_E_1 = CONFIG::ENGINE::HARD_LIMIT_MIN_ELEVATION.Get()+DIF_VALUE;
    const int POS_E_2 = CONFIG::ENGINE::HARD_LIMIT_MAX_ELEVATION.Get()-DIF_VALUE;
    const std::string MSG_e_pos_1      = "e pos "+std::to_string(POS_E_1)+"\r\n";
    const std::string MSG_e_pos_2      = "e pos "+std::to_string(POS_E_2)+"\r\n";
    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);
    CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Set(true);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    client.WriteLine(MSG_syn);
    TIMEOUT(protocol.Tick().azimuth_syncronized,LONG_TIMEOUT);
    TIMEOUT(protocol.Tick().elevation_syncronized,LONG_TIMEOUT);

    TestFramework::LogMsg("apos1");
    ASSERT(protocol.Tick().azimuth_position!=POS_A_1);
    client.WriteLine(MSG_a_pos_1);
    TIMEOUT(protocol.Tick().azimuth_position==POS_A_1,LONG_TIMEOUT);

    TestFramework::LogMsg("apos2");
    ASSERT(protocol.Tick().azimuth_position!=POS_A_2);
    client.WriteLine(MSG_a_pos_2);
    TIMEOUT(protocol.Tick().azimuth_position==POS_A_2,LONG_TIMEOUT*2);

    TestFramework::LogMsg("epos1");
    ASSERT(protocol.Tick().elevation_position!=POS_E_1);
    client.WriteLine(MSG_e_pos_1);
    TIMEOUT(protocol.Tick().elevation_position==POS_E_1,LONG_TIMEOUT);

    TestFramework::LogMsg("epos2");
    ASSERT(protocol.Tick().elevation_position!=POS_E_2);
    client.WriteLine(MSG_e_pos_2);
    TIMEOUT(protocol.Tick().elevation_position==POS_E_2,LONG_TIMEOUT);

    client.Close();
}

void TestProtocolMSO2::Speed()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const int DIF_VALUE = 100;
    const std::string MSG_syn      = "a syn\r\ne syn\r\n";
    const int SPEED_A_1 = +DIF_VALUE;
    const int SPEED_A_2 = -DIF_VALUE;
    const std::string MSG_a_vel_1      = "a vel "+std::to_string(SPEED_A_1)+"\r\n";
    const std::string MSG_a_vel_2      = "a vel "+std::to_string(SPEED_A_2)+"\r\n";
    const int SPEED_E_1 = +DIF_VALUE;
    const int SPEED_E_2 = -DIF_VALUE;
    const std::string MSG_e_vel_1      = "e vel "+std::to_string(SPEED_E_1)+"\r\n";
    const std::string MSG_e_vel_2      = "e vel "+std::to_string(SPEED_E_2)+"\r\n";
    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);
    CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Set(true);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    client.WriteLine(MSG_syn);
    TIMEOUT(protocol.Tick().azimuth_syncronized,LONG_TIMEOUT);
    TIMEOUT(protocol.Tick().elevation_syncronized,LONG_TIMEOUT);

    TestFramework::LogMsg("avel1");
    ASSERT(protocol.Tick().azimuth_speed!=SPEED_A_1);
    client.WriteLine(MSG_a_vel_1);
    TIMEOUT(protocol.Tick().azimuth_speed==SPEED_A_1,LONG_TIMEOUT);

    TestFramework::LogMsg("avel2");
    ASSERT(protocol.Tick().azimuth_speed!=SPEED_A_2);
    client.WriteLine(MSG_a_vel_2);
    TIMEOUT(protocol.Tick().azimuth_speed==SPEED_A_2,LONG_TIMEOUT);

    TestFramework::LogMsg("evel1");
    ASSERT(protocol.Tick().elevation_speed!=SPEED_E_1);
    client.WriteLine(MSG_e_vel_1);
    TIMEOUT(protocol.Tick().elevation_speed==SPEED_E_1,LONG_TIMEOUT);

    TestFramework::LogMsg("evel2");
    ASSERT(protocol.Tick().elevation_speed!=SPEED_E_2);
    client.WriteLine(MSG_e_vel_2);
    TIMEOUT(protocol.Tick().elevation_speed==SPEED_E_2,LONG_TIMEOUT);

    client.Close();
}

void TestProtocolMSO2::Elevation_limits()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;
    const int DIF_VALUE = 100;
    const std::string MSG_syn      = "a syn\r\ne syn\r\n";
    const int SPEED_E_1 = +DIF_VALUE;
    const int SPEED_E_2 = -DIF_VALUE;
    const std::string MSG_e_vel_1      = "e vel "+std::to_string(SPEED_E_1)+"\r\n";
    const std::string MSG_e_vel_2      = "e vel "+std::to_string(SPEED_E_2)+"\r\n";
    const std::string MSG_e_pos_0      = "e pos "+std::to_string(CONFIG::ENGINE::ZERO_ELEVATION.Get())+"\r\n";
    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);
    CONFIG::CONTROLLER::FAST_SYNCHRONIZATION.Set(true);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    client.WriteLine(MSG_syn);
    TIMEOUT(protocol.Tick().azimuth_syncronized,LONG_TIMEOUT);
    TIMEOUT(protocol.Tick().elevation_syncronized,LONG_TIMEOUT);
    MSO_State state;

    TestFramework::LogMsg("Go up [HARD LIMIT]");
    client.WriteLine(MSG_e_vel_1);
    TIMEOUT(protocol.Tick().elevation_speed==SPEED_E_1,LONG_TIMEOUT);
    state = protocol.Tick();
    while(state.elevation_speed==SPEED_E_1)
    {
        state = protocol.Tick();
        std::string msg = "Pos: "+std::to_string(state.elevation_position)
                +"/"+std::to_string(CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get());
        TestFramework::LogMsg(msg);
        SLEEPMS(100);
    }
    ASSERT(state.elevation_position==CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get());

    TestFramework::LogMsg("Go home");
    client.WriteLine(MSG_e_pos_0);
    TIMEOUT(protocol.Tick().elevation_position==CONFIG::ENGINE::ZERO_ELEVATION.Get(),LONG_TIMEOUT);
    TIMEOUT(protocol.Tick().elevation_speed==0,LONG_TIMEOUT);
    PrintMSOState(protocol.Tick());

    TestFramework::LogMsg("Go down [HARD LIMIT]");
    client.WriteLine(MSG_e_vel_2);
    TIMEOUT(protocol.Tick().elevation_speed==SPEED_E_2,LONG_TIMEOUT);
    state = protocol.Tick();
    while(state.elevation_speed==SPEED_E_2)
    {
        state = protocol.Tick();
        std::string msg = "Pos: "+std::to_string(state.elevation_position)
                +"/"+std::to_string(CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get());
        TestFramework::LogMsg(msg);
        SLEEPMS(100);

    }
    ASSERT(state.elevation_position==CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get());

    TestFramework::LogMsg("Go home");
    client.WriteLine(MSG_e_pos_0);
    TIMEOUT(protocol.Tick().elevation_position==CONFIG::ENGINE::ZERO_ELEVATION.Get(),LONG_TIMEOUT);
    TIMEOUT(protocol.Tick().elevation_speed==0,LONG_TIMEOUT);
    PrintMSOState(protocol.Tick());

    int new_max = (CONFIG::ENGINE::HARD_LIMIT_MAX_ELEVATION.Get()+CONFIG::ENGINE::ZERO_ELEVATION.Get())/2;
    int new_min = (CONFIG::ENGINE::HARD_LIMIT_MIN_ELEVATION.Get()+CONFIG::ENGINE::ZERO_ELEVATION.Get())/2;
    CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Set(new_max);
    CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Set(new_min);

    TestFramework::LogMsg("Go up [SOFT LIMIT]");
    client.WriteLine(MSG_e_vel_1);
    TIMEOUT(protocol.Tick().elevation_speed==SPEED_E_1,LONG_TIMEOUT);
    state = protocol.Tick();
    while(state.elevation_speed==SPEED_E_1)
    {
        state = protocol.Tick();
        std::string msg = "Pos: "+std::to_string(state.elevation_position)
                +"/"+std::to_string(CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get());
        TestFramework::LogMsg(msg);
        SLEEPMS(100);
    }
    ASSERT(state.elevation_position==CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get());

    TestFramework::LogMsg("Go home");
    client.WriteLine(MSG_e_pos_0);
    TIMEOUT(protocol.Tick().elevation_position==CONFIG::ENGINE::ZERO_ELEVATION.Get(),LONG_TIMEOUT);
    TIMEOUT(protocol.Tick().elevation_speed==0,LONG_TIMEOUT);
    PrintMSOState(protocol.Tick());

    TestFramework::LogMsg("Go down [SOFT LIMIT]");
    client.WriteLine(MSG_e_vel_2);
    TIMEOUT(protocol.Tick().elevation_speed==SPEED_E_2,LONG_TIMEOUT);
    state = protocol.Tick();
    while(state.elevation_speed==SPEED_E_2)
    {
        state = protocol.Tick();
        std::string msg = "Pos: "+std::to_string(state.elevation_position)
                +"/"+std::to_string(CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get());
        TestFramework::LogMsg(msg);
        SLEEPMS(100);
    }
    ASSERT(state.elevation_position==CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get());

    TestFramework::LogMsg("Go home");
    client.WriteLine(MSG_e_pos_0);
    TIMEOUT(protocol.Tick().elevation_position==CONFIG::ENGINE::ZERO_ELEVATION.Get(),LONG_TIMEOUT);
    TIMEOUT(protocol.Tick().elevation_speed==0,LONG_TIMEOUT);
    CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Set(CONFIG::ENGINE::HARD_LIMIT_MAX_ELEVATION.Get());
    CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Set(CONFIG::ENGINE::HARD_LIMIT_MIN_ELEVATION.Get());

    client.Close();
}

void TestProtocolMSO2::Inputs_outputs()
{
    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;

    TestFramework::LogMsg("Init 1 check");
    Inputs_outputs_init_1(PORT);
    TestFramework::LogMsg("Init 2 check");
    Inputs_outputs_init_2(PORT);

    TestFramework::LogMsg("Starting server");
    Protocol_MSO2 protocol;
    protocol.Init(GetInitState(),PORT);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);

    for(size_t i=0; i<Protocol_MSO2::NUMBER_OF_INPUTS;++i)
    {
        std::string msg = "Check "+std::to_string(i+1)
                +"/"+std::to_string(Protocol_MSO2::NUMBER_OF_INPUTS);
        TestFramework::LogMsg(msg);
        Inputs_outputs_check_input(&protocol, &client, i);
        Inputs_outputs_check_output(&protocol, &client, i);
    }
    client.Close();
}

void TestProtocolMSO2::Inputs_outputs_init_1(const int port)
{
    MSO_State init_state=GetInitState();
    Protocol_MSO2 protocol;
    protocol.Init(init_state,port);
    MSO_State new_state=protocol.Tick();
    ASSERT(new_state.inputs.size()==new_state.outputs.size());
    ASSERT(new_state.inputs.size()==Protocol_MSO2::NUMBER_OF_INPUTS);
    ASSERT(new_state.outputs.size()==Protocol_MSO2::NUMBER_OF_INPUTS);
    ASSERT(!init_state.isChanged(new_state));
    for(unsigned int i=0;i<new_state.inputs.size();++i)
    {
        ASSERT(!new_state.inputs[i]);
        ASSERT(!new_state.outputs[i]);
    }
}

void TestProtocolMSO2::Inputs_outputs_init_2(const int port)
{
    MSO_State init_state=GetInitState();
    for(unsigned int i=0;i<init_state.inputs.size();++i)
    {
        init_state.inputs[i]=true;
        init_state.outputs[i]=true;
    }

    Protocol_MSO2 protocol;
    protocol.Init(init_state,port);
    MSO_State new_state=protocol.Tick();

    for(unsigned int i=0;i<new_state.inputs.size();++i)
    {
        ASSERT(!new_state.inputs[i]);
        ASSERT(!new_state.outputs[i]);
    }
}

void TestProtocolMSO2::Inputs_outputs_check_input(
        Protocol_MSO2* const protocol, TcpClientSocket* const client,
        const size_t num)
{
    TestFramework::LogMsg("Disable input");
    Inputs_outputs_check_input_set_get_assert(protocol,client,num,false);
    TestFramework::LogMsg("Enable input");
    Inputs_outputs_check_input_set_get_assert(protocol,client,num,true);
    TestFramework::LogMsg("Reset input");
    Inputs_outputs_check_input_set_get_assert(protocol,client,num,false);
}

void TestProtocolMSO2::Inputs_outputs_check_input_set_get_assert(
        Protocol_MSO2* const protocol, TcpClientSocket* const client,
        const size_t num, bool value)
{
    const std::string MSG_I_GET      = "i\r\n";
    const std::string MSG_I_SET_WORD = "iset "+std::to_string(num)+" ";
    ASSERT(protocol!=nullptr);
    ASSERT(client!=nullptr);
    ASSERT(client->IsOpen());
    ASSERT(num<Protocol_MSO2::NUMBER_OF_INPUTS);

    MSO_State initial_state = protocol->Tick();
    //Set
    client->WriteLine(MSG_I_SET_WORD+(value?"1":"0")+"\r\n");
    SLEEPMS(SHORT_SLEEP);

    //Check state
    TIMEOUT(protocol->Tick().inputs[num]==value,LONG_TIMEOUT);

    //check TCP
    client->WriteLine(MSG_I_GET);
    SLEEPMS(SHORT_SLEEP);
    protocol->Tick();
    TIMEOUT(client->BytesAvailable()>0,LONG_TIMEOUT);
    std::string line = client->ReadLine(); //s0 data 18 i 1011001101011011
    line=line.substr(line.length()-Protocol_MSO2::NUMBER_OF_INPUTS,Protocol_MSO2::NUMBER_OF_INPUTS);
    for(unsigned int i=0;i<line.length(); ++i)
        ASSERT(line[i]=='0' || line[i]=='1');
    bool tcp_value = (line[num]=='1')?true:false;
    ASSERT(tcp_value==value);

    //equals
    ASSERT(tcp_value==protocol->Tick().inputs[num]);

    //no other changes
    MSO_State new_state = protocol->Tick();
    for(size_t i=0;i<Protocol_MSO2::NUMBER_OF_INPUTS; ++i)
        if(i!=num)
            ASSERT(initial_state.inputs[i]==new_state.inputs[i]);
}

void TestProtocolMSO2::Inputs_outputs_check_output(
        Protocol_MSO2* const protocol, TcpClientSocket* const client,
        const size_t num)
{
    TestFramework::LogMsg("Disable output");
    Inputs_outputs_check_output_set_get_assert(protocol,client,num,false);
    TestFramework::LogMsg("Enable output");
    Inputs_outputs_check_output_set_get_assert(protocol,client,num,true);
    TestFramework::LogMsg("Reset output");
    Inputs_outputs_check_output_set_get_assert(protocol,client,num,false);
}

void TestProtocolMSO2::Inputs_outputs_check_output_set_get_assert(
        Protocol_MSO2* const protocol, TcpClientSocket* const client,
        const size_t num, const bool value)
{
    const std::string MSG_O_GET      = "g\r\n";
    const std::string MSG_O_SET_WORD = "o "+std::to_string(num)+" ";
    ASSERT(protocol!=nullptr);
    ASSERT(client!=nullptr);
    ASSERT(client->IsOpen());
    ASSERT(num<Protocol_MSO2::NUMBER_OF_INPUTS);

    MSO_State initial_state = protocol->Tick();
    //Set
    client->WriteLine(MSG_O_SET_WORD+(value?"1":"0")+"\r\n");
    SLEEPMS(SHORT_SLEEP);

    //Check state
    TIMEOUT(protocol->Tick().outputs[num]==value,LONG_TIMEOUT);

    //check TCP
    client->WriteLine(MSG_O_GET);
    SLEEPMS(SHORT_SLEEP);
    protocol->Tick();
    TIMEOUT(client->BytesAvailable()>0,LONG_TIMEOUT);
    std::string line = client->ReadLine(); //s0 data 18 g 1011001101011011
    line=line.substr(line.length()-Protocol_MSO2::NUMBER_OF_INPUTS,Protocol_MSO2::NUMBER_OF_INPUTS);
    for(unsigned int i=0;i<line.length(); ++i)
        ASSERT(line[i]=='0' || line[i]=='1');
    bool tcp_value = (line[num]=='1')?true:false;
    ASSERT(tcp_value==value);

    //equals
    ASSERT(tcp_value==protocol->Tick().outputs[num]);

    //no other changes
    MSO_State new_state = protocol->Tick();
    for(size_t i=0;i<Protocol_MSO2::NUMBER_OF_INPUTS; ++i)
        if(i!=num)
            ASSERT(initial_state.outputs[i]==new_state.outputs[i]);

}

void TestProtocolMSO2::Period()
{
    const std::string MSG_PERIOD = "period 100\r\n";
    const int A_POS = 10000;
    const int E_POS = CONFIG::ENGINE::ZERO_ELEVATION.Get()+3333;

    const int PORT = BASE_PORT + PORT_CNT; ++PORT_CNT;

    Protocol_MSO2 protocol;
    MSO_State state = GetInitState();
    state.azimuth_syncronized=true;
    state.elevation_syncronized=true;
    state.azimuth_position=A_POS;
    state.elevation_position=E_POS;
    for(unsigned int i=0;i<state.inputs.size();i+=2)
        state.inputs[i]=true;

    protocol.Init(state,PORT);

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);
    client.WriteLine(MSG_PERIOD);
    TIMEOUT(protocol.Tick().tcp_last_got_data!="",LONG_TIMEOUT);
    protocol.Tick();
    TIMEOUT(client.BytesAvailable()>0,LONG_TIMEOUT);
    std::string line = client.ReadLine();

    MSO_State period = ParsePeriodLine(line);
    ASSERT(period.azimuth_position==A_POS);
    ASSERT(period.elevation_position==E_POS);
    ASSERT(period.voltage>0);
    for(unsigned int i=0;i<period.inputs.size();++i)
        ASSERT(period.inputs[i]==state.inputs[i]);
    client.Close();
}

MSO_State TestProtocolMSO2::ParsePeriodLine(std::string period_line)
{
    MSO_State result;
    result.timestamp=0;
    ASSERT(period_line.length()==59);
    std::string part;
    //0123456789A123456789B123456789C123456789D123456789E12345678
    //s0 data 48 per 18987 15502 +0000 +0000 207 1111111100111111
    result.tcp_last_got_data=period_line;
    result.tcp_last_sent_data=period_line;
    part = period_line.substr(15,5);
    result.azimuth_position=std::stoi(part);
    part = period_line.substr(21,5);
    result.elevation_position=std::stoi(part);
    part = period_line.substr(27,5);
    result.azimuth_speed=std::stoi(part);
    part = period_line.substr(33,5);
    result.elevation_speed=std::stoi(part);
    part = period_line.substr(39,3);
    result.voltage=std::stoi(part);
    part = period_line.substr(43,16);
    for(unsigned int i=0; i<part.length();++i)
        result.inputs.push_back(part[i]=='1');

    return result;
}

void TestProtocolMSO2::Reset()
{
    const int PORT = BASE_PORT + PORT_CNT+30; ++PORT_CNT;
    const std::string MSG_RESET = "reset\r\n";
    CONFIG::CONTROLLER::AZIMUTH_SYNCED_ON_INIT.Set(false);
    CONFIG::CONTROLLER::ELEVATION_SYNCED_ON_INIT.Set(true);

    MSO_State init_state = GetInitState();
    init_state.azimuth_position=10000;
    init_state.azimuth_syncronized=true;
    init_state.elevation_position=CONFIG::ENGINE::ZERO_ELEVATION.Get()+3333;
    init_state.elevation_syncronized=true;

    Protocol_MSO2 protocol;
    protocol.Init(init_state,PORT);
    protocol.Tick();

    TcpClientSocket client;
    client.Open("127.0.0.1",PORT);
    TIMEOUT(client.IsOpen(),LONG_TIMEOUT);
    TestFramework::LogMsg("Reset");
    client.WriteLine(MSG_RESET);

    TIMEOUT(protocol.Tick().azimuth_syncronized==CONFIG::CONTROLLER::AZIMUTH_SYNCED_ON_INIT.Get(),LONG_TIMEOUT);
    MSO_State state = protocol.Tick();
    ASSERT(state.elevation_syncronized==CONFIG::CONTROLLER::ELEVATION_SYNCED_ON_INIT.Get());
    ASSERT(state.azimuth_position==init_state.azimuth_position);
    ASSERT(state.elevation_position==init_state.elevation_position);
    TestFramework::LogMsg("Close");
    client.Close();
}

