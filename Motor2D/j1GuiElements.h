#ifndef __j1GUIELEMENTS_H__
#define __j1GUIELEMENTS_H__

#include "j1Module.h"
#include "j1InputManager.h"

enum ButtonState{normal, over, clicked};
enum FontName{GANONF,PIXEL,PIXELMORE,POKE1};


struct 	_TTF_Font;
class Animation;
class InputManager;
// ---------------------------------------------------

class Image : public j1GuiEntity
{
public:
	void Update();
	void Draw();

public:
	Image(SDL_Rect rectangle, iPoint position,std::string identifier, uint id);
	~Image();
	void AssignNumber(uint n);
public:
	bool start;


};

// ----------------------------------------------------

class Button : public j1GuiEntity
{
public:
	void Draw();
	void Update();
public:
	Button(SDL_Rect rectangle, iPoint pos, iPoint stat2, iPoint stat3, bool animated, std::string identifier, uint id, const char* textstring, uint textsize, iPoint textpos);
	~Button();

public:
	bool start, click;
	ButtonState state=normal;
	SDL_Rect texture2, texture3;
	Text* buttontext = nullptr;;
	Animation* anim=nullptr;
};

// -----------------------------------------------------

class Text : public j1GuiEntity
{
public:
	Text(FontName search, const char* write, SDL_Color color,uint length, iPoint pos, uint size,bool draw, std::string identifier, uint id);
	~Text();

public:
	void Draw();
	void Update();
	void Write(const char* string);
	void CheckString(std::string string);
	void Visible(bool yes);
private:
	std::string text;
	_TTF_Font* font;
	SDL_Texture* text_texture;
	uint length, size;
public:
	Text* next_line = nullptr;
	FontName font_name;
	SDL_Color color;
	bool draw = true;
};

// -------------------------------------------------------

class Dialogue : public j1GuiEntity
{
public:
	Dialogue(const char* string);
	~Dialogue();
public:
	void Draw();
	void Update();
	void PushLine(bool push);
	//void Clear(int more_erased);
public:
	float diferential = 0.5;
	Text* lines;
	bool push;
	uint timer;
	Selector* options;
	bool end = false;
};

class Selector : public j1GuiEntity
{
public:
	Selector(const char* first_option, const char* second_option, j1GuiEntity* parent);
	~Selector();
public:
	void Handle_Input();
private:
	Text* first, *second;
	Text* selector;
	bool position;//true up false down
};

// ------------------------------------------------------

class ZeldaMenu :public j1GuiEntity, public InputListener
{
public:
	ZeldaMenu();
	~ZeldaMenu();
public:
	void Update();
	void Handle_Input();
	void OnInputCallback(INPUTEVENT, EVENTSTATE); //TODO LOW -> check if its better to put button inputs here or not
public:
	void AddElement(j1GuiEntity* element);
	void ResetInventory();
	void Select(int value);
	void OpenClose(bool open);
	void Move(bool axis, float speed);
	void Click();
	void UnClick();
	void Do();

	Image* GetImage(uint id);
	Button* GetSelected();
	void ShowItemInfo();
private:
	std::vector<Button*>menu_buttons;
	std::vector<Image*>menu_images;
	std::vector<Text*>menu_texts;
	Button*selected;
public:
	uint id_selected;
};

//----------------------------------------------------------
class Pokemon;

class PokemonCombatHud :public j1GuiEntity
{
public:
	PokemonCombatHud(Pokemon* Link, Pokemon* Brendan);
	~PokemonCombatHud();
public:
	void Handle_Input();
	void Update();
public:
	void SetCd(uint newcd);
	void OpenClose(bool open);//true open false close
	void Move(bool x_axis, float speed);
	void LoadNewPokemon(Pokemon* pokemon, bool trainer);
	void CombatInfo(Pokemon* pokemon_1, Pokemon* pokemon_2);
	void GetDamage(uint damage, bool trainer);
public:
	bool cooldown;
	iPoint cdtime; //.X SHOWS CD TIME AND .Y SHOWS REMAINING CD TIME

private:
	std::vector<Image*>hud_images;

	Image* ability;
	Image* hp1;
	Image* hp2;
	Text* poke_hp_Link;
	Text* poke_hp_Brendan;

	iPoint hpbar_pLink;//.X SHOWS MAX HP, .Y SHOWS ACTUAL LIFE (SAME AS ABOVE) used for triangle operations with atlas pixels
	iPoint hpbar_pBrendan;
	char buffer[25]; //hp
	iPoint num_pokemons;

};
#endif // __j1GUIELEMENTS_H__