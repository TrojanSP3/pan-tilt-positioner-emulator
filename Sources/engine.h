#ifndef ENGINE_H
#define ENGINE_H
#include "config.h"
#include "utils.h"


#include <thread>

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
    std::thread* engine_thread;
    std::atomic<bool> engine_thread_flag_stop;

    struct Axis
    {
        virtual ~Axis();
		Axis();
        std::atomic<int> current_position;
        std::atomic<bool> move_by_position;
        std::atomic<int> target_position;

        std::atomic<int> current_speed;
        std::atomic<bool> move_by_speed;
        std::atomic<int> target_speed;

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
