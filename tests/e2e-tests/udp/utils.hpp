/*
** EPITECH PROJECT, 2022
** utils.hpp
** File description:
** PNL for Polymorph Network Library, not the music group.
*/

#pragma once

/**
 * @property Set 1 if the network is local (25ms tolerance), 0 if it's outside (250ms tolerance)
 */
#define PNL_LOCAL_MODE 1

#define PNL_TIME_OUT PNL_LOCAL_MODE ? 25 : 250
#define PNL_WAIT(time) std::this_thread::sleep_for(std::chrono::milliseconds(time));
#define PNL_WAIT_COND(condition, time) while(condition) { PNL_WAIT(time); }
#define PNL_WAIT_COND_LOOP(condition, loop, time) int i = 0; PNL_WAIT_COND(condition || ++i == loop, time)