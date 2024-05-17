#include <functional>
#include <memory>

#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Image.hpp"
#include "HoverImageButton.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"

namespace Engine {
	HoverImageButton::HoverImageButton(std::string img, std::string imgIn, 
                float img_x, float img_y,
                float inf_x, float inf_y, float inf_w, float inf_h,
                unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
        ImageButton(img, imgIn, img_x, img_y), 
        information_x(inf_x), information_y(inf_y), information_w(inf_w), information_h(inf_h),
        text_color_r(r), text_color_g(g), text_color_b(b), text_color_a(a)
    {
        informations = new Group();
        informations->Visible = false;
    }

    void HoverImageButton::OnMouseMove(int mx, int my)
    {
		mouseIn = Collider::IsPointInBitmap(Point((mx - Position.x) * GetBitmapWidth() / Size.x + Anchor.x * GetBitmapWidth(), (my - Position.y) * GetBitmapHeight() / Size.y + Anchor.y * GetBitmapHeight()), bmp);
        if (!mouseIn || !Enabled) 
        {   
            informations->Visible = false;
            bmp = imgOut;
        }
		else
        {
            informations->Visible = true;
            bmp = imgIn;  
        } 
    }

    void HoverImageButton::AddNewInformation(std::string content)
    {
        informations->AddNewObject(new Label(content, "pirulen.ttf", 25, 
                information_x, information_y + num_of_row * ROW_SPACING, 
                text_color_r, text_color_g, text_color_b, text_color_a));
        num_of_row++;
    }   

    void HoverImageButton::Draw() const
    {
        ImageButton::Draw();
        Engine::LOG(Engine::INFO) << "inforamtions visible: "<< informations->Visible;
        if(informations->Visible)   
            informations->Draw();
    }
}
