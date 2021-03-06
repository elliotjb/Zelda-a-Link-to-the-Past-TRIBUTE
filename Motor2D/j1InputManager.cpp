#include "j1InputManager.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1FileSystem.h"
#include "j1Input.h"
#include "j1Fonts.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "SDL\include\SDL.h"

InputManager::InputManager() : j1Module()
{
	name = "inputManager";
}

// Destructor
InputManager::~InputManager()
{}

// Called when before render is available
bool InputManager::Awake(pugi::xml_node& conf)
{
	bool ret = true;
	for (pugi::xml_node tmp = conf.child("action"); tmp != nullptr; tmp = tmp.next_sibling())
	{
		std::pair<int, INPUTEVENT> new_action;
		new_action.first = tmp.attribute("button").as_int();
		new_action.second = (INPUTEVENT)tmp.attribute("event").as_int();
		actions.insert(new_action);
	}

	for (pugi::xml_node tmp = conf.child("action2"); tmp != nullptr; tmp = tmp.next_sibling())
	{
		std::pair<int, INPUTEVENT> new_action;
		new_action.first = tmp.attribute("button").as_int();
		new_action.second = (INPUTEVENT)tmp.attribute("event").as_int();
		actions_2.insert(new_action);
	}

	actions_inUse = actions;

	return ret;
}

void InputManager::ChangeInput(bool isNormal)
{
	if (isNormal)
	{
		actions_inUse = actions;
		//LOG("CONFIGURATO NORMAL");
	}
	else
	{
		actions_inUse = actions_2;
		//LOG("CONFIGURATO TACTIC");
	}

}

// Called before all Updates
bool InputManager::PreUpdate()
{
	BROFILER_CATEGORY("DoPreUpdate_InputManager", Profiler::Color::SaddleBrown);
	bool ret = true;
	return ret;
}

bool InputManager::Update(float dt)
{
	BROFILER_CATEGORY("DoUpdate_InputManager", Profiler::Color::SlateGray);

	//TODO 7 uncomment this after doned
	/*if (EventPressed(PAUSE) == E_DOWN)
	{
		ChangeInputEvent(MUP);
	}*/
	if (App->scene->ingame)
	{
		if (App->scene->use_tatical)
		{
			if (App->scene->gamestate == INMENU)
			{
				if (App->scene->gamestate_input != INMENU)
				{
					App->scene->gamestate_input = INMENU;
					change = true;
				}
			}
			else if(App->scene->gamestate == INGAME)
			{
				if (App->scene->gamestate_input != INGAME)
				{
					App->scene->gamestate_input = INGAME;
					change = true;
				}
			}

			if (change)
			{
				change = false;
				if (App->scene->gamestate_input == INMENU)
				{
					ChangeInput(true);
				}
				else if (App->scene->gamestate_input == INGAME)
				{
					ChangeInput(false);
				}
			}
		}
	}
	else
	{
		ChangeInput(true);
	}
	return true;
}

// Called after all Updates
bool InputManager::PostUpdate()
{
	CallListeners();
	if (!current_action.empty())
	{
		current_action.clear();
	}

	return true;
}

// Called before quitting
bool InputManager::CleanUp()
{
	bool ret = true;


	return ret;
}

void InputManager::InputDetected(int button, EVENTSTATE state)
{
	if (next_input_change == false)
	{
		std::multimap<int, INPUTEVENT>::iterator tmp = actions_inUse.find(button);
		if (tmp != actions_inUse.end())
		{
			std::pair<INPUTEVENT, EVENTSTATE> new_current_action;
			new_current_action.first = (*tmp).second;
			new_current_action.second = state;
			current_action.insert(new_current_action);
		}
	}
	else
	{
		ChangeEventButton(button);
	}
}

void InputManager::ChangeInputEvent(INPUTEVENT change_ev)
{
	next_input_change = true;
	event_to_change = change_ev;
}

bool InputManager::ChangeEventButton(int new_button)
{
	bool ret = false;

	std::multimap<int, INPUTEVENT>::iterator tmp = actions_inUse.find(new_button);
	if (tmp != actions_inUse.end())
	{
		LOG("This button is actually in another action");
		return ret;
	}
	//Look for the event to erase it
	tmp = actions_inUse.begin();
	while ((*tmp).second != event_to_change)
	{
		tmp++;
	}
	actions_inUse.erase(tmp);

	//This is the event with the new button
	std::pair<int, INPUTEVENT> event_changed;
	event_changed.first = new_button;
	event_changed.second = event_to_change;
	actions_inUse.insert(event_changed);

	//Reset the variables
	next_input_change = false;
	event_to_change = NO_EVENT;

	ret = true;


	return ret;
}

EVENTSTATE InputManager::EventPressed(INPUTEVENT action) const
{
	std::multimap<INPUTEVENT, EVENTSTATE>::const_iterator tmp = current_action.find(action);

	if (tmp != current_action.end())
	{
		return tmp->second;
	}

	return E_NOTHING;
}

void InputManager::AddListener(InputListener* new_listener)
{
	if (new_listener)
	{
		new_listener->input_active = true;
		listeners.push_back(new_listener);
	}

}

void InputManager::CallListeners()
{
	if (!current_action.empty())
	{
		for (std::list<InputListener*>::iterator it = listeners.begin(); it != listeners.end(); it++)
		{
			if ((*it)->input_active)
			{
				std::multimap<INPUTEVENT, EVENTSTATE>::iterator frame_actions = current_action.begin();
				while (frame_actions != current_action.end())
				{
					(*it)->OnInputCallback(frame_actions->first, frame_actions->second);
					frame_actions++;
				}
			}
		}
	}
}