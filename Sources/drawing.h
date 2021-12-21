#ifndef DRAWING_H
#define DRAWING_H

#include <string>

typedef std::string ASCIIPicture;

class Drawing
{
public:
    Drawing(int picture_height);
    ~Drawing();
    void SetAzimuth(int _mso2val);
    void SetElevation(int _mso2val);
    ASCIIPicture PaintAll();
    ASCIIPicture PaintAzimuth();
    ASCIIPicture PaintElevation();

protected:
    void UpdateCharMapForAzimuth();
    void UpdateCharMapForElevation();

private:
    bool need_update_azimuth_char_map = true;
    bool need_update_elevation_char_map = true;
    int current_azimuth=0;
    int current_elevation=0;
    char** char_map_azimuth=nullptr;
    char** char_map_elevation=nullptr;
    int char_map_azimuth_height=0;
    int char_map_azimuth_width=0;
    int char_map_elevation_height=0;
    int char_map_elevation_width=0;
    int precision_in_mso2_units=0;
    int sqr_diff_from_center_to_draw_circle_min=0;
    int sqr_diff_from_center_to_draw_circle_max=0;

};

#endif // DRAWING_H
