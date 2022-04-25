#ifndef CONFIG_H
#define CONFIG_H
#include "property.h"
#include "utils.h"

class CONFIG
{
public:
    static std::string ToString();
    static Property<std::string> FILENAME;

    class CONTROLLER
    {
        public:
        static std::string ToString();
        static Property<std::string> INIT_INPUTS_STATE;
        static Property<std::string> INIT_OUTPUTS_STATE;
        static Property<bool> AZIMUTH_SYNCED_ON_INIT;
        static Property<bool> ELEVATION_SYNCED_ON_INIT;
        static Property<bool> FAST_SYNCHRONIZATION;
        static PropertyReadOnly<int> ADDITIONAL_SERIAL_LINES;
        static PropertyReadOnly<int> TICK_MS;
        static Property<std::string> ENDL;
    };

    class TCP
    {
        public:
        static std::string ToString();
        static Property<int> PORT;
        static Property<uint32_t> MAXBUFFERSIZE;
    };

    class ENGINE
    {
        public:
        static std::string ToString();
        static PropertyReadOnly<int> MAX_AZIMUTH;
        static Property<int> ZERO_ELEVATION ;
        static PropertyReadOnly<int> HARD_LIMIT_MAX_ELEVATION;
        static PropertyReadOnly<int> HARD_LIMIT_MIN_ELEVATION;
        static PropertyAtomic<int> LIMIT_MAX_ELEVATION;
        static PropertyAtomic<int> LIMIT_MIN_ELEVATION;
        static Property<int> MAX_AZIMUTH_SPEED_PER_SECOND;
        static Property<int> MAX_ELEVATION_SPEED_PER_SECOND;
        static Property<double> VEL_TO_EPS_COEFFICIENT;
        static PropertyReadOnly<int> TICK_MS;
    };

    class DRAWING
    {
        public:
        static std::string ToString();
        static PropertyReadOnly<int> HEIGHT;
        static PropertyReadOnly<char> CHAR_EMPTY;
        static PropertyReadOnly<char> CHAR_BORDER;
        static PropertyReadOnly<char> CHAR_CIRCLE;
        static PropertyReadOnly<char> CHAR_CENTER;
        static PropertyReadOnly<char> CHAR_POINTER;
    };

    class LOG
    {
    public:
        static std::string ToString();
        static PropertyReadOnly<std::string> FILENAME;
        static PropertyReadOnly<std::string> TEST_FILENAME;
        static Property<uint64_t> MAX_SIZE_BYTES;
    };

};

#endif // CONFIG_H
