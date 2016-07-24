/**
 * This file is part of SDLGamepad.
 *
 * SDLGamepad is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SDLGamepad is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Manuel Blanquett
 * mail.nalla@gmail.com
 */

/**********************************************************************/
#include "sdlgamepad.h"

#include <SDL/SDL.h>
// #undef main

char *testname = "No gamepad";
class SDLGamepadPrivate {
public:
    SDLGamepadPrivate() : gamepad(0)
    {}

    /**
     * SDL_Joystick object.
     *
     * This represents the currently opened SDL_Joystick object.
     */
    SDL_Joystick *gamepad;
};

/**********************************************************************/
SDLGamepad::SDLGamepad()
{
    buttons = -1;
    axes    = -1;
    index   = -1;
    loop    = false;
    tick    = MIN_RATE;
    gamepadName    = NULL;
    priv    = new SDLGamepadPrivate;
}

/**********************************************************************/
SDLGamepad::~SDLGamepad()
{
    loop = false;

    if (priv->gamepad) {
        SDL_JoystickClose(priv->gamepad);
    }

    SDL_Quit();

    delete priv;
}

/**********************************************************************/
bool SDLGamepad::init()
{
    uint numberOfJoysticks=0;
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        setName(testname);
        //emit gamepadName("Nothing there.");
        return false;
    }

    numberOfJoysticks = SDL_NumJoysticks();
    if (SDL_NumJoysticks() > 0) {
        setName((char*)SDL_JoystickName(0));
        //name = (char*)calloc(1,strlen(SDL_JoystickName(0)));
        //strcpy(name,SDL_JoystickName(0));
        emit gamepads(SDL_NumJoysticks());
        //emit gamepadName(QString::fromStdString((char*)name));

        if (!setGamepad(0)) {
            return false;
        }

        for (qint8 i = 0; i < buttons; i++) {
            buttonStates.append(0);
        }
    } else {
        setName(testname);
        emit gamepadNameSet(QString::fromStdString(getName()));
        return false;
    }

    loop = true;
    return true;
}


/**********************************************************************/
void SDLGamepad::run()
{
    while (loop) {
        updateAxes();
        updateButtons();
        msleep(tick);
    }
}

/**********************************************************************/
bool SDLGamepad::refresh()
{
    /*Here we need to stop any gamepad operations, clear the list of devices,
     * initialise and emit the data to Qt
     */
    return false;
}

/**********************************************************************/
bool SDLGamepad::setGamepad(qint16 index)
{
    if (index != this->index) {
        if (SDL_JoystickOpened(this->index)) {
            SDL_JoystickClose(priv->gamepad);
        }

        priv->gamepad = SDL_JoystickOpen(index);

        if (priv->gamepad) {
            buttons = SDL_JoystickNumButtons(priv->gamepad);
            axes    = SDL_JoystickNumAxes(priv->gamepad);

            if (axes >= 4) {
                this->index = index;
                return true;
            } else {
                buttons     = -1;
                axes        = -1;
                this->index = -1;
                qCritical("Gamepad has less than 4 axes");
            }
        } else {
            buttons     = -1;
            axes        = -1;
            this->index = -1;
            qCritical("Unable to open Gamepad!");
        }
    }

    return false;
}

/**********************************************************************/
void SDLGamepad::setTickRate(qint16 ms)
{
    tick = ms;
}

/**********************************************************************/
void SDLGamepad::updateAxes()
{
    if (priv->gamepad) {
        QListInt16 values;
        SDL_JoystickUpdate();

        for (qint8 i = 0; i < axes; i++) {
            qint16 value = SDL_JoystickGetAxis(priv->gamepad, i);

            if (value > -NULL_RANGE && value < NULL_RANGE) {
                value = 0;
            }

            values.append(value);
        }

        emit axesValues(values);
    }
}

/**********************************************************************/
void SDLGamepad::updateButtons()
{
    if (priv->gamepad) {
        SDL_JoystickUpdate();

        for (qint8 i = 0; i < buttons; i++) {
            qint16 state = SDL_JoystickGetButton(priv->gamepad, i);

            if (buttonStates.at(i) != state) {
                if (state > 0) {
                    emit buttonState((ButtonNumber)i, true);
                } else {
                    emit buttonState((ButtonNumber)i, false);
                }

                buttonStates.replace(i, state);
            }
        }
    }
}

/**********************************************************************/
void SDLGamepad::quit()
{
    loop = false;
}

/**********************************************************************/
qint16 SDLGamepad::getAxes()
{
    return axes;
}

/**********************************************************************/
qint16 SDLGamepad::getButtons()
{
    return buttons;
}

void SDLGamepad::setName(char* name)
{
    if(name)
    {
        gamepadName = (char*)calloc(1,strlen(name));
        strcpy(gamepadName,name);
    }
}

char *SDLGamepad::getName(void)
{
    if(gamepadName)
    {
        return gamepadName;
    }
    else
    {
        return NULL;
    }
}
