#include "j1DynamicItems.h"
#include "j1App.h"
#include "j1Textures.h"

DynamicItems::DynamicItems(iPoint position) :j1SceneElement(position)
{
	type = DYNITEMS;
	name = "DynItems";
}

DynamicItems::~DynamicItems()
{

}

bool DynamicItems::Awake(pugi::xml_node &conf, uint id)
{
	bool stop_search = false;
	pugi::xml_node temp = conf;
	for (int s_id = temp.attribute("id").as_int(0); stop_search == false; s_id = temp.attribute("id").as_int(0))
	{
		if (id == s_id)
		{
			name = temp.attribute("name").as_string("");
			std::string es = temp.attribute("file").as_string("");
			texture = App->tex->Load(es.c_str());
			canBlit = true;
			stop_search = true;
		}
		temp = temp.next_sibling();
	}

	return true;
}

bool DynamicItems::Start()
{
	return true;
}

bool DynamicItems::Update()
{
	return true;
}

void DynamicItems::Draw()
{
	if (canBlit == true)
	{
		App->render->Blit(texture, position.x, position.y);
	}
}

bool DynamicItems::CleanUp()
{
	return true;
}