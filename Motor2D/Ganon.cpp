#include "Ganon.h"

Ganon::Ganon(iPoint position) :NPC(position)
{
	name = "Ganon";
	type = ENEMY;
}

Ganon::~Ganon()
{}

bool Ganon::Awake(pugi::xml_node &conf, uint id)
{


	return true;
}

bool Ganon::Start()
{


	return true;
}

bool Ganon::Update()
{


	return true;
}

void Ganon::Draw()
{

}

void Ganon::AddItem(Item* item)
{

}

void Ganon::Drop_item()
{

}

bool Ganon::CleanUp()
{
	return true;
}




