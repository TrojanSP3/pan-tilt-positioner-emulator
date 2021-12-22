#include "engine.h"

#include <math.h>

#include "utils.h"
#include "config.h"
#include "log.h"

const std::string LOGMODULE="Engine";

Engine::Engine()
{
    elevation.current_position.store(CONFIG::ENGINE::ZERO_ELEVATION.Get());
}

Engine::~Engine()
{
    Stop();
}

void Engine::Start()
{
    Stop();
    engine_thread_flag_stop.store(false);
    engine_thread = new std::thread(Engine::engine_thread_procedure, this);

}

void Engine::Stop()
{
    engine_thread_flag_stop.store(true);
    if(engine_thread!=nullptr)
    {
        if(engine_thread->joinable())
            engine_thread->join();
        delete engine_thread; engine_thread=nullptr;
    }
}

void Engine::engine_thread_procedure(Engine* obj)
{
    const std::string LOGNAME="engine_thread_procedure";
    if(obj == nullptr)
        return;
    try
    {
        Utils::SetThreadName("engine");
        obj->Loop();
    }
    catch(std::exception& ex)
    {
        std::string msg = std::string("EngineThreadException: ")+ex.what();
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }
    catch(...)
    {
        std::string msg = "EngineThreadException";
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }
}

void Engine::Loop()
{
    while(!engine_thread_flag_stop.load())
    {
        UpdateAxisMovement(&azimuth);
        UpdateAxisMovement(&elevation);
        Utils::Sleep(CONFIG::ENGINE::TICK_MS.Get());
    }

    azimuth.move_by_position.store(false);
    azimuth.move_by_speed.store(false);
    azimuth.target_speed.store(0);
    elevation.move_by_position.store(false);
    elevation.move_by_speed.store(false);
    elevation.target_speed.store(0);
}

void Engine::UpdateAxisMovement(Axis* axis)
{
    if(axis==nullptr)
        return;
    if(axis->move_by_position.load())
    {
        MoveAxisByPosition(axis);
    }
    else if(axis->move_by_speed.load())
    {
        MoveAxisBySpeed(axis);
    }
    else
    {
        axis->current_speed.store(0);
    }
}

void Engine::MoveAxisByPosition(Axis* axis)
{
    if(axis==nullptr)
        return;
    if(axis->current_position.load() != axis->target_position.load())
    {
        int additional_value = axis->IsAzimuth()? CONFIG::ENGINE::MAX_AZIMUTH.Get() : 0;
        int current_position = axis->current_position.load();
        int target_position = axis->target_position.load();

        int direction = target_position - current_position;

        if(axis->IsAzimuth())
        {
            //Мы должны двигаться по короткому пути
            if(abs(direction)>CONFIG::ENGINE::MAX_AZIMUTH.Get()/2)
            {
                direction=-direction;
                if(direction>0)
                {
                    target_position+=additional_value;
                }
                else
                {
                    current_position+=additional_value;
                }
            }
        }
        direction=direction/abs(direction);

        int MAX_AXIS_SPEED_PER_SECOND = axis->MAX_AXIS_SPEED_PER_SECOND();
        int MAX_AXIS_SPEED_PER_TICK = axis->MAX_AXIS_SPEED_PER_TICK();

        int current_axis_speed =
                Utils::Mso_EpsToVel(MAX_AXIS_SPEED_PER_SECOND*direction);
        axis->current_speed.store(current_axis_speed);

        double new_position = current_position+MAX_AXIS_SPEED_PER_TICK*direction;


        if(direction>0)
        {
            if(new_position>target_position)
                new_position=target_position;
        }
        else
        {
            if(new_position<target_position)
                new_position=target_position;
        }

        current_position= axis->PositionValueValidator(int(new_position));
        axis->current_position.store(current_position);
    }
    else
    {
        axis->move_by_position.store(false);
    }
}

void Engine::MoveAxisBySpeed(Axis* axis)
{
    if(axis==nullptr)
        return;
    axis->current_speed.store(axis->target_speed.load());

    int engine_speed_per_second=Utils::Mso_VelToEps(axis->current_speed.load());
    double engine_speed_per_tick=double(engine_speed_per_second)*CONFIG::ENGINE::TICK_MS.Get()/1000;

    int current_position=axis->current_position.load();
    double new_position=current_position+engine_speed_per_tick;

    if(engine_speed_per_tick>0)
        new_position=ceil(new_position);
    else
        new_position=floor(new_position);

    if(!axis->IsAzimuth())
    {
        if(new_position<CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get())
        {
            new_position=CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get();
            axis->move_by_speed.store(false);
            axis->current_speed.store(0);
        }
        if(new_position>CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get())
        {
            new_position=CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get();
            axis->move_by_speed.store(false);
            axis->current_speed.store(0);
        }

    }

    current_position=axis->PositionValueValidator(int(new_position));
    axis->current_position.store(current_position);
}

void Engine::SetAzimuthPosition(const int val)
{
    SetAxisPosition(&azimuth, val);
}
void Engine::SetElevationPosition(const int val)
{
    SetAxisPosition(&elevation, val);
}

void Engine::SetAxisPosition(Axis* axis, int val)
{
    if(axis==nullptr)
        return;
    if(!engine_thread_flag_stop.load())
    {
        int new_val= axis->PositionValueValidator(val);
        axis->target_position.store(new_val);
        axis->move_by_speed.store(false);
        axis->move_by_position.store(true);
    }
}

void Engine::SetAzimuthSpeed(const int val)
{
    SetAxisSpeed(&azimuth, val);
}
void Engine::SetElevationSpeed(const int val)
{
    SetAxisSpeed(&elevation, val);
}

void Engine::SetAxisSpeed(Axis* axis, int val)
{
    if(axis==nullptr)
        return;
    if(!engine_thread_flag_stop.load())
    {
        int MAX_AXIS_SPEED_PER_SECOND = axis->MAX_AXIS_SPEED_PER_SECOND();

        if( abs(val) > Utils::Mso_EpsToVel(MAX_AXIS_SPEED_PER_SECOND))
        {
            val=Utils::Mso_EpsToVel(MAX_AXIS_SPEED_PER_SECOND) * (val)/abs(val);
        }

        axis->target_speed.store(val);
        axis->move_by_speed.store(true);
        axis->move_by_position.store(false);
    }
}

int Engine::GetAzimuthPosition() const
{
    return azimuth.current_position.load();
}
int Engine::GetElevationPosition() const
{
    return elevation.current_position.load();
}
int Engine::GetAzimuthSpeed() const
{
    return azimuth.current_speed.load();
}
int Engine::GetElevationSpeed() const
{
    return elevation.current_speed.load();
}

bool Engine::Azimuth::IsAzimuth() const
{
    return true;
}
int Engine::Azimuth::PositionValueValidator(int val) const
{
    return Utils::Mso_Azimuth(val);
}
int Engine::Azimuth::MAX_AXIS_SPEED_PER_SECOND() const
{
    return CONFIG::ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND.Get();
}
int Engine::Azimuth::MAX_AXIS_SPEED_PER_TICK() const
{
    return static_cast<int>( static_cast<double>(MAX_AXIS_SPEED_PER_SECOND())*CONFIG::ENGINE::TICK_MS.Get()/1000);
}
bool Engine::Elevation::IsAzimuth() const
{
    return false;
}
int Engine::Elevation::PositionValueValidator(int val) const
{
    return Utils::Mso_Elevation(val);
}
int Engine::Elevation::MAX_AXIS_SPEED_PER_SECOND() const
{
    return CONFIG::ENGINE::MAX_ELEVATION_SPEED_PER_SECOND.Get();
}
int Engine::Elevation::MAX_AXIS_SPEED_PER_TICK() const
{
    return static_cast<int>( static_cast<double>(MAX_AXIS_SPEED_PER_SECOND())*CONFIG::ENGINE::TICK_MS.Get()/1000);
}
