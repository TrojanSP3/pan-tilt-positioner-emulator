#ifndef ENGINE_H
#define ENGINE_H

#include <atomic>
#include <thread>

#include "utils.h"
#include "config.h"

class Engine
{

public:
    Engine();
    ~Engine();
    void Start();
    void Stop();

    void SetAzimuthPosition(const int val);
    void SetElevationPosition(const int val);
    void SetAzimuthSpeed(const int val);
    void SetElevationSpeed(const int val);
    int GetAzimuthPosition() const;
    int GetElevationPosition() const;
    int GetAzimuthSpeed() const;
    int GetElevationSpeed() const;

    void Loop();
private:
    static void engine_thread_procedure(Engine* obj);
    std::thread* engine_thread=nullptr;
    std::atomic<bool> engine_thread_flag_stop;

    struct Axis
    {
          std::atomic<int> current_position={0};
          std::atomic<bool> move_by_position={false};
          std::atomic<int> target_position={0};

          std::atomic<int> current_speed={0};
          std::atomic<bool> move_by_speed={false};
          std::atomic<int> target_speed={0};

          virtual bool IsAzimuth() const = 0;
          virtual int PositionValueValidator(int val) const = 0;
          virtual int MAX_AXIS_SPEED_PER_SECOND() const = 0;
          virtual int MAX_AXIS_SPEED_PER_TICK() const = 0;
    };
    struct Azimuth: Axis
    {
        bool IsAzimuth() const override;
        int PositionValueValidator(int val) const override;
        int MAX_AXIS_SPEED_PER_SECOND() const override;
        int MAX_AXIS_SPEED_PER_TICK() const override;
    } azimuth;

    struct Elevation: Axis
    {
        bool IsAzimuth() const override;
        int PositionValueValidator(int val) const override;
        int MAX_AXIS_SPEED_PER_SECOND() const override;
        int MAX_AXIS_SPEED_PER_TICK() const override;
    } elevation;

    void UpdateAxisMovement(Axis* axis);
    void SetAxisPosition(Axis* axis, int val);
    void SetAxisSpeed(Axis* axis, int val);
    void MoveAxisByPosition(Axis* axis);
    void MoveAxisBySpeed(Axis* axis);

};

#endif // ENGINE_H
