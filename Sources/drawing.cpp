#include "drawing.h"

#include "config.h"
#include "utils.h"
#include "log.h"

#include <cmath>
#include <sstream>

#define DRAWING_DEBUG
#undef DRAWING_DEBUG

const std::string LOG_MODULE="Drawing";

inline int MSO_MAX()
{
    return CONFIG::ENGINE::MAX_AZIMUTH.Get();
}

const int PICTURE_BORDER_SIZE=1;

inline int ConvertPointToMSOUnits(int i, int j, int field_size)
{
    int result=-1;
    int x = i-field_size/2;
    int y = -j+field_size/2;
    if(y==0)
    {
        if(x>=0)
        {
            result=MSO_MAX()*1/4;//90
        }
        else
        {
            result=MSO_MAX()*3/4;//270
        }
    }
    else
    {
        if(x==0)
        {
            if(y>=0)
            {
                result=0;//0
            }
            else
            {
                result=MSO_MAX()/2;//180
            }
        }
        else
        {
            if(x*y>0)
            {
                double v_atan = atan(static_cast<double>(x)/y);
                result = Utils::RadToMsoRad(fabs(v_atan));
            }
            else
            {
                double v_atan = atan(static_cast<double>(y)/x);
                result = Utils::RadToMsoRad(fabs(v_atan));
            }

            if(x>0 && y<0)
                result+=MSO_MAX()/4;
            if(x<0 && y<0)
                result+=MSO_MAX()/2;
            if(x<0 && y>0)
                result+=MSO_MAX()*3/4;
        }
    }
    return result;
}

Drawing::Drawing()
{
	need_update_azimuth_char_map = true;
    need_update_elevation_char_map = true;
    current_azimuth=0;
    current_elevation=0;
    char_map_azimuth=nullptr;
    char_map_elevation=nullptr;
    char_map_azimuth_height=0;
    char_map_azimuth_width=0;
    char_map_elevation_height=0;
    char_map_elevation_width=0;
    precision_in_mso2_units=0;
    sqr_diff_from_center_to_draw_circle_min=0;
    sqr_diff_from_center_to_draw_circle_max=0;
}

Drawing::Drawing(int picture_height)
{
	Drawing();

    const int MIN_FIELD_SIZE = 3;
    if(picture_height<=MIN_FIELD_SIZE)
    {
        throw std::invalid_argument("Wrong picture height");
    }

    char_map_azimuth_height = picture_height;
    char_map_azimuth_width = char_map_azimuth_height;
    char_map_azimuth= new char*[char_map_azimuth_width];
    for(int i=0; i<char_map_azimuth_width;++i)
        char_map_azimuth[i]= new char[char_map_azimuth_height];

    char_map_elevation_height=picture_height;
    char_map_elevation_width=char_map_elevation_height/2+1;
    char_map_elevation= new char*[char_map_elevation_width];
    for(int i=0; i<char_map_elevation_width;++i)
        char_map_elevation[i]= new char[char_map_elevation_height];

    precision_in_mso2_units=MSO_MAX()/2/(picture_height-PICTURE_BORDER_SIZE*2);


    sqr_diff_from_center_to_draw_circle_min = ((picture_height-3)/2)*((picture_height-3)/2);
    sqr_diff_from_center_to_draw_circle_max = ((picture_height-1)/2)*((picture_height-1)/2);

}

Drawing::~Drawing()
{
    if(char_map_azimuth)
    {
        for(int i=0; i<char_map_azimuth_width;++i)
        {
            if(char_map_azimuth[i])
                delete [] char_map_azimuth[i];
        }
        delete [] char_map_azimuth;
        char_map_azimuth=nullptr;
    }

    if(char_map_elevation)
    {
        for(int i=0; i<char_map_elevation_width;++i)
        {
            if(char_map_elevation[i])
                delete [] char_map_elevation[i];
        }
        delete [] char_map_elevation;
        char_map_elevation=nullptr;
    }
}

void Drawing::SetAzimuth(int _val)
{
    current_azimuth=Utils::Mso_Azimuth(_val);
    need_update_azimuth_char_map = true;
}

void Drawing::SetElevation(int _val)
{
    current_elevation=Utils::Mso_Elevation(_val);
    need_update_elevation_char_map = true;
}

ASCIIPicture Drawing::PaintAll()
{
    if(need_update_azimuth_char_map)
        UpdateCharMapForAzimuth();
    if(need_update_elevation_char_map)
        UpdateCharMapForElevation();

    std::stringstream result;

#ifdef DRAWING_DEBUG
    std::string msg_az =
            "A: "+std::to_string(current_azimuth)
            +" ("+std::to_string(Utils::MsoRadToDegrees(current_azimuth))+"*)";
    std::string msg_el =
            "E: "+std::to_string(current_elevation)
            +" ("+std::to_string(Utils::MsoRadToDegrees(current_elevation-CONFIG::ENGINE::MAX_AZIMUTH/4))+"*)";
    result<< msg_az+" "+msg_el+"\r\n";

    result<<"XX";
    for(int i=0;i<char_map_azimuth_width;++i)
    {
        if(i<10)
            result<<'0';
        result<<i;
    }
    result<<"X";
    for(int i=0;i<char_map_elevation_width;++i)
    {
        if(i<10)
            result<<'0';
        result<<i;
    }
    result<<"\r\n";
#endif

    for(int j=0; j<char_map_azimuth_height;++j)
    {
#ifdef DRAWING_DEBUG
        if(j<10)
            result<<'0';
        result<<j;
#endif
        for(int i=0; i<char_map_azimuth_width;++i)
        {

            result<<char_map_azimuth[i][j]<<' ';
        }

        result<<" ";

        if(j<char_map_elevation_height)
        {
            for(int i=0; i<char_map_elevation_width;++i)
            {
                result<<char_map_elevation[i][j]<<' ';;
            }
        }
        result<<"\r\n";
    }
    return result.str();
}

ASCIIPicture Drawing::PaintAzimuth()
{
    if(need_update_azimuth_char_map)
        UpdateCharMapForAzimuth();

    std::stringstream result;
#ifdef DRAWING_DEBUG
    std::string msg_az =
            "A: "+std::to_string(current_azimuth)
            +" ("+std::to_string(Utils::MsoRadToDegrees(current_azimuth))+"*)";
    result<< msg_az+"\r\n";

    result<<"XX";
    for(int i=0;i<char_map_azimuth_width;++i)
    {
        if(i<10)
            result<<'0';
        result<<i;
    }
    result<<"\r\n";
#endif
    for(int j=0; j<char_map_azimuth_height;++j)
    {
#ifdef DRAWING_DEBUG
        if(j<10)
            result<<'0';
        result<<j;
#endif
        for(int i=0; i<char_map_azimuth_width;++i)
        {
            result<<char_map_azimuth[i][j]<<' ';;
        }
        result<<"\r\n";
    }
    return result.str();
}

ASCIIPicture Drawing::PaintElevation()
{
    if(need_update_elevation_char_map)
        UpdateCharMapForElevation();

    std::stringstream result;
#ifdef DRAWING_DEBUG
    std::string msg_el =
            "E: "+std::to_string(current_elevation)
            +" ("+std::to_string(Utils::MsoRadToDegrees(current_elevation-CONFIG::ENGINE::MAX_AZIMUTH/4))+"*)";
    result<< msg_el+"\r\n";

    result<<"XX";
    for(int i=0;i<char_map_elevation_width;++i)
    {
        if(i<10)
            result<<'0';
        result<<i;
    }
    result<<"\r\n";
#endif
    for(int j=0; j<char_map_elevation_height;++j)
    {
#ifdef DRAWING_DEBUG
        if(j<10)
            result<<'0';
        result<<j;
#endif
        for(int i=0; i<char_map_elevation_width;++i)
        {
            result<<char_map_elevation[i][j]<<' ';;
        }
        result<<"\r\n";
    }
    return result.str();
}

void Drawing::UpdateCharMapForAzimuth()
{
    const std::string LOG_NAME="UpdateCharMapForAzimuth";

#ifdef DRAWING_DEBUG
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"BEGIN");
#endif

    double current_precision_part = static_cast<double>(current_azimuth)/precision_in_mso2_units;
    int mso_azimuth_min=
            int(std::floor(current_precision_part))*precision_in_mso2_units;
    int mso_azimuth_max=
            int(std::ceil(current_precision_part))*precision_in_mso2_units;
    if(mso_azimuth_min==mso_azimuth_max)
    {
        mso_azimuth_min-=precision_in_mso2_units/2;
        mso_azimuth_max+=precision_in_mso2_units/2;
    }

#ifdef DRAWING_DEBUG
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"mso_azimuth_min: "+std::to_string(mso_azimuth_min));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"mso_azimuth_max: "+std::to_string(mso_azimuth_max));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"precision_in_mso2_units: "+std::to_string(precision_in_mso2_units));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"sqr_diff_from_center_to_draw_circle_min: "+std::to_string(sqr_diff_from_center_to_draw_circle_min));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"sqr_diff_from_center_to_draw_circle_max: "+std::to_string(sqr_diff_from_center_to_draw_circle_max));
#endif
    for(int i=0; i<char_map_azimuth_width;++i)
        for(int j=0; j<char_map_azimuth_height;++j)
        {
            char_map_azimuth[i][j]=CONFIG::DRAWING::CHAR_EMPTY.Get();

            if( i<=PICTURE_BORDER_SIZE - 1
                || j<=PICTURE_BORDER_SIZE - 1
                || i>=char_map_azimuth_width-PICTURE_BORDER_SIZE
                || j>=char_map_azimuth_height-PICTURE_BORDER_SIZE )
            {
                char_map_azimuth[i][j]=CONFIG::DRAWING::CHAR_BORDER.Get();
#ifdef DRAWING_DEBUG
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_BORDER");
#endif
                continue;
            }

            int sqr_diff_from_center_to_draw_circle=
                    static_cast<int>(
                        std::pow((i-char_map_azimuth_width/2),2)+
                        std::pow((j-char_map_azimuth_height/2),2)
                    );
            if(
                    sqr_diff_from_center_to_draw_circle>=sqr_diff_from_center_to_draw_circle_min
                    &&
                    sqr_diff_from_center_to_draw_circle<=sqr_diff_from_center_to_draw_circle_max
                    )
            {
                char_map_azimuth[i][j]=CONFIG::DRAWING::CHAR_CIRCLE.Get();
#ifdef DRAWING_DEBUG
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_CIRCLE");
#endif
                continue;
            }
            else
            {
                if(sqr_diff_from_center_to_draw_circle>sqr_diff_from_center_to_draw_circle_max)
                {
                    continue;
                }
            }

            if( i==char_map_azimuth_width/2 && j==char_map_azimuth_height/2 )
            {
                char_map_azimuth[i][j]=CONFIG::DRAWING::CHAR_CENTER.Get();
#ifdef DRAWING_DEBUG
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_CENTER");
#endif
                continue;
            }

            int azimuth_for_i_j_point = -1;
            if(current_azimuth<=MSO_MAX()/2)
            {//1-2 quarter
                if(i>=char_map_azimuth_width/2)
                {
                    if(current_azimuth<=MSO_MAX()/4)
                    {//1 quarter
                        if(j<=char_map_azimuth_height/2)
                        {
                            azimuth_for_i_j_point =
                                    ConvertPointToMSOUnits(i,j,char_map_azimuth_height);
                        }
                    }
                    else
                    {//2 quarter
                        if(j>=char_map_azimuth_height/2)
                        {
                            azimuth_for_i_j_point =
                                    ConvertPointToMSOUnits(i,j,char_map_azimuth_height);
                        }
                    }
                }
            }
            else
            {//3-4 quarter
                if(i<=char_map_azimuth_width/2)
                {
                    if(current_azimuth<=MSO_MAX()*3/4)
                    {//3 quarter
                        if(j>=char_map_azimuth_height/2)
                        {
                            azimuth_for_i_j_point =
                                    ConvertPointToMSOUnits(i,j,char_map_azimuth_height);
                        }
                    }
                    else
                    {//4 quarter
                        if(j<=char_map_azimuth_height/2)
                        {
                            azimuth_for_i_j_point =
                                    ConvertPointToMSOUnits(i,j,char_map_azimuth_height);
                            if(azimuth_for_i_j_point<MSO_MAX()/2)
                                azimuth_for_i_j_point+=MSO_MAX();
                        }
                    }
                }
            }
#ifdef DRAWING_DEBUG
            if(azimuth_for_i_j_point>=0)
            {
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] azimuth = "
                    +std::to_string(azimuth_for_i_j_point));
            }
#endif
            if(azimuth_for_i_j_point>=0
               && azimuth_for_i_j_point>=mso_azimuth_min
               && azimuth_for_i_j_point<=mso_azimuth_max)
            {
                char_map_azimuth[i][j]=CONFIG::DRAWING::CHAR_POINTER.Get();

#ifdef DRAWING_DEBUG
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_POINTER");
#endif
                continue;
            }
        }

#ifdef DRAWING_DEBUG
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"END");
#endif
    need_update_azimuth_char_map = false;
}

void Drawing::UpdateCharMapForElevation()
{
    const std::string LOG_NAME="UpdateCharMapForElevation";
#ifdef DRAWING_DEBUG
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"BEGIN");
#endif

    const int elevation_shift=CONFIG::ENGINE::ZERO_ELEVATION.Get()-MSO_MAX()/4;
    int current_elevation_shift=current_elevation-elevation_shift;
    double current_precision_part = static_cast<double>(current_elevation_shift)/precision_in_mso2_units;
    int mso_elevation_min=
            int(std::floor(current_precision_part))*precision_in_mso2_units;
    int mso_elevation_max=
            int(std::ceil(current_precision_part))*precision_in_mso2_units;
    if(mso_elevation_min==mso_elevation_max)
    {
        mso_elevation_min-=precision_in_mso2_units/2;
        mso_elevation_max+=precision_in_mso2_units/2;
    }

#ifdef DRAWING_DEBUG
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"current_elevation_shift: "+std::to_string(current_elevation_shift));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"mso_elevation_min: "+std::to_string(mso_elevation_min));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"mso_elevation_max: "+std::to_string(mso_elevation_max));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"precision_in_mso2_units: "+std::to_string(precision_in_mso2_units));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"sqr_diff_from_center_to_draw_circle_min: "+std::to_string(sqr_diff_from_center_to_draw_circle_min));
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"sqr_diff_from_center_to_draw_circle_max: "+std::to_string(sqr_diff_from_center_to_draw_circle_max));
#endif

    for(int i=0; i<char_map_elevation_width;++i)
        for(int j=0; j<char_map_elevation_height;++j)
        {
            char_map_elevation[i][j]=CONFIG::DRAWING::CHAR_EMPTY.Get();

            if( i<=PICTURE_BORDER_SIZE - 1
                || j<=PICTURE_BORDER_SIZE - 1
                || i>=char_map_elevation_width-PICTURE_BORDER_SIZE
                || j>=char_map_elevation_height-PICTURE_BORDER_SIZE )
            {
                char_map_elevation[i][j]=CONFIG::DRAWING::CHAR_BORDER.Get();
#ifdef DRAWING_DEBUG
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_BORDER");
#endif
                continue;
            }

            int sqr_diff_from_center_to_draw_circle=
                    static_cast<int>(
                        std::pow((i),2)+
                        std::pow((j-char_map_elevation_height/2),2)
                    );

            if(
                sqr_diff_from_center_to_draw_circle>=sqr_diff_from_center_to_draw_circle_min
                &&
                sqr_diff_from_center_to_draw_circle<=sqr_diff_from_center_to_draw_circle_max
            )
            {
                char_map_elevation[i][j]=CONFIG::DRAWING::CHAR_CIRCLE.Get();
#ifdef DRAWING_DEBUG
                    LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                                "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_CIRCLE");
#endif
                continue;
            }
            else
            {
                if(sqr_diff_from_center_to_draw_circle>sqr_diff_from_center_to_draw_circle_max)
                {
                    continue;
                }
            }

            if( i==PICTURE_BORDER_SIZE && j==char_map_elevation_height/2 )
            {
                char_map_elevation[i][j]=CONFIG::DRAWING::CHAR_CENTER.Get();
#ifdef DRAWING_DEBUG
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_CENTER");
#endif
                continue;
            }

            int elevation_for_i_j_point = -1;
            if(current_elevation_shift<=MSO_MAX()/4)
            {//1 quarter
                if(j<=char_map_elevation_height/2)
                {
                    elevation_for_i_j_point =
                            ConvertPointToMSOUnits(
                                i+char_map_elevation_width-2
                                ,j
                                ,char_map_elevation_height);
                }
            }
            else
            {//2 quarter
                if(j>=char_map_elevation_height/2)
                {
                    elevation_for_i_j_point =
                            ConvertPointToMSOUnits(
                                i+char_map_elevation_width-2,
                                j,
                                char_map_elevation_height);
                }
            }

#ifdef DRAWING_DEBUG
            if(elevation_for_i_j_point>=0)
            {
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] elevation = "
                    +std::to_string(elevation_for_i_j_point));
            }
#endif
            if( elevation_for_i_j_point>=0
                && elevation_for_i_j_point>=mso_elevation_min
                && elevation_for_i_j_point<=mso_elevation_max)
            {
                char_map_elevation[i][j]=CONFIG::DRAWING::CHAR_POINTER.Get();
#ifdef DRAWING_DEBUG
                LOG::WriteDebug(LOG_MODULE,LOG_NAME,
                    "["+std::to_string(i)+","+std::to_string(j)+"] CHAR_POINTER");
#endif
                continue;
            }
        }

#ifdef DRAWING_DEBUG
    LOG::WriteDebug(LOG_MODULE,LOG_NAME,"END");
#endif
    need_update_elevation_char_map = false;
}

#ifdef DRAWING_DEBUG
    #undef DRAWING_DEBUG
#endif
