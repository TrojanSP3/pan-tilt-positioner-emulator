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
	Drawing();
    bool need_update_azimuth_char_map;
    bool need_update_elevation_char_map;
    int current_azimuth;
    int current_elevation;
    char** char_map_azimuth;
    char** char_map_elevation;
    int char_map_azimuth_height;
    int char_map_azimuth_width;
    int char_map_elevation_height;
    int char_map_elevation_width;
    int precision_in_mso2_units;
    int sqr_diff_from_center_to_draw_circle_min;
    int sqr_diff_from_center_to_draw_circle_max;

};

#endif // DRAWING_H
