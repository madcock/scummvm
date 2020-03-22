/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ultima/ultima4/city.h"
#include "ultima/ultima4/context.h"
#include "ultima/ultima4/conversation/conversation.h"
#include "ultima/ultima4/creature.h"
#include "ultima/ultima4/object.h"
#include "ultima/ultima4/person.h"
#include "ultima/ultima4/player.h"

namespace Ultima {
namespace Ultima4 {

using Common::String;

City::City() : Map() {

}


City::~City() {
	for (PersonList::iterator i = _persons.begin(); i != _persons.end(); i++)
		delete *i;
	for (PersonRoleList::iterator j = _personRoles.begin(); j != _personRoles.end(); j++)
		delete *j;
	for (Std::vector<Dialogue *>::iterator k = _extraDialogues.begin(); k != _extraDialogues.end(); k++)
		delete *k;
}

/**
 * Returns the name of the city
 */
Common::String City::getName() {
	return _name;
}

/**
 * Adds a person object to the map
 */
Person *City::addPerson(Person *person) {
	// Make a copy of the person before adding them, so
	// things like angering the guards, etc. will be
	// forgotten the next time you visit :)
	Person *p = new Person(person);

	/* set the start coordinates for the person */
	p->setMap(this);
	p->goToStartLocation();

	_objects.push_back(p);
	return p;
}

/**
 * Add people to the map
 */
void City::addPeople() {
	PersonList::iterator current;

	// Make sure the city has no people in it already
	removeAllPeople();

	for (current = _persons.begin(); current != _persons.end(); current++) {
		Person *p = *current;
		if ((p->getTile() != 0)
		        && !(g_context->_party->canPersonJoin(p->getName(), NULL)
		             && g_context->_party->isPersonJoined(p->getName()))
		   )
			addPerson(p);
	}
}

/**
 * Removes all people from the current map
 */
void City::removeAllPeople() {
	ObjectDeque::iterator obj;
	for (obj = _objects.begin(); obj != _objects.end();) {
		if (isPerson(*obj))
			obj = removeObject(obj);
		else obj++;
	}
}

/**
 * Returns the person object at the given (x,y,z) coords, if one exists.
 * Otherwise, returns NULL.
 */
Person *City::personAt(const Coords &coords) {
	Object *obj;

	obj = objectAt(coords);
	if (isPerson(obj))
		return dynamic_cast<Person *>(obj);
	else
		return NULL;
}

/**
 * Returns true if the Map pointed to by 'punknown'
 * is a City map
 */
bool isCity(Map *punknown) {
	City *pCity;
	if ((pCity = dynamic_cast<City *>(punknown)) != NULL)
		return true;
	else
		return false;
}

} // End of namespace Ultima4
} // End of namespace Ultima
