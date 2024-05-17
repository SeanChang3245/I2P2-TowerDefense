#ifndef HOVERIMAGEBUTTON_HPP
#define HOVERIMAGEBUTTON_HPP

#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Group.hpp"
#include <list>
#include <string>

#define ROW_SPACING 30

namespace Engine 
{
	/// <summary>
	/// A clickable button, changes image and show detail when mouse hover.
	/// </summary>
    class HoverImageButton : public Engine::ImageButton
    {
    protected:
        // the backgroud image when showing detail
        // Image *background_img;
        // store all the detail Label 
        Group *informations;
        int num_of_row;
        float information_x;
        float information_y;
        float information_w;
        float information_h;
        unsigned char text_color_r;
        unsigned char text_color_g;
        unsigned char text_color_b;
        unsigned char text_color_a;
        

    public:
		explicit HoverImageButton(std::string img, std::string imgIn, 
                float img_x, float img_y,
                float inf_x , float inf_y, float inf_w, float inf_h,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a);

        void OnMouseMove(int mx, int my) override;
        void AddNewInformation(std::string content);
        void Draw() const override;

    };


}


#endif