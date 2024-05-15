#include <string>
#include <allegro5/allegro_primitives.h>
#include <allegro5/timer.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include "TextInput.hpp"
#include "Engine/LOG.hpp"


void Engine::TextInputBlock::OnKeyDown(int keyCode)
{
    char input_char = '\0';
    std::string &cur_name = this->user_entered_text->Text;

    if(ALLEGRO_KEY_A <= keyCode && keyCode <= ALLEGRO_KEY_Z)
        input_char = keyCode - ALLEGRO_KEY_A + 'a';
    else if(ALLEGRO_KEY_0 <= keyCode && keyCode <= ALLEGRO_KEY_9)
        input_char = keyCode - ALLEGRO_KEY_0 + '0';
    else if(keyCode == ALLEGRO_KEY_BACKSPACE && cur_name.size())
        cur_name.pop_back();

    if(cur_name.size() < MAX_NAME_LEN && input_char != '\0')
        cur_name += input_char;

    if(cur_name.empty())
        default_message->Visible = true;
    else
        default_message->Visible = false;

}

void Engine::TextInputBlock::Draw() const
{
    // al_draw_filled_rectangle(200, 100, 400, 200, al_map_rgb(255, 255, 255));
    Group::Draw();
}

int Engine::TextInputBlock::get_text_length() const 
{
    return this->user_entered_text->Text.size();
}

Engine::TextInputBlock::TextInputBlock(float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, float anchorX, float anchorY)
{
    default_message = new Label("enter your name", "pirulen.ttf", 20, x, y, r, g, b, a, 0.5, 0.5);
    user_entered_text = new Label("", "pirulen.ttf", 48, x, y, r, g, b, a, 0.5, 0.5);
    background_img = new Image("textinput/TextInputBlock.jpg", x-250, y-37.5, 500, 75, 0, 0);

    AddNewObject(background_img);
    AddNewObject(default_message);
    AddNewObject(user_entered_text);
}

static std::string get_date()
{
    std::string month;
    std::string day;

    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    month = std::to_string(now->tm_mon+1);
    day = std::to_string(now->tm_mday);

    if(month.size() == 1)
        month = "0" + month;
    if(day.size() == 1)
        day = "0" + day;

    return month + "/" + day;
}

void Engine::TextInputBlock::save_input_to_file(int score)
{
    std::string date = get_date();
    
    Engine::LOG(Engine::INFO) << "enter save_input_to_file";

    // write into scoreboard under the workspace, used to store data between rounds
    std::ofstream fout("../Resource/scoreboard.txt", std::ios_base::app);
    fout << this->user_entered_text->Text << " " << score << " " << date << '\n';
    fout.close();

    // write into scoreboard under build folder, used to show data in the same round
    fout.open("Resource/scoreboard.txt", std::ios_base::app);
    fout << this->user_entered_text->Text << " " << score << " " << date << '\n';
}
