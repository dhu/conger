Official 'Cube with Bots' readme.

Current version: 0.2


Author:

Rick Helmus
E-Mail: rickhelmus@gmail.com
MSN: rick_helmus8@hotmail.com
ICQ: 224332897
AIM: rickakame


Features:

See the file 'changelog.txt' in your cube/bot folder.


Installation

Extract all the files to your cube folder.
Don't worry it won't overwrite any cube specific files.

Uninstalling:

If you want for some reason uninstall it(I don't see any reason since it won't modify
existing files and doesn't take much space) you have to delete these files:
cube/cube_bot_linux
cube/cube_bot_win.bat
cube/cube_bot-readme.txt
cube/data/menus_bot.cfg
all files in cube/bot/
cube/bin_unix/cube_bot_lin
cube/bin/cube_bot.exe



Starting a game:
Run either cube_bot_win.bat (for windows users) or cube_bot_lin (for linux users).

For linux users: if you have problems with running cube, you might want to 'chmod it'.
Type this in a console in your cube directory:
chmod +x ./cube_bot_lin
chmod 777 ./cube_bot_lin

To add a bot you can simply open the main menu(esc) and open the bot menu, from there
you can add a bot in a team or change the default bot skill.
You can also use the addbot command(see below) to add a bot.



Bot names/teams:

When you add a bot and don't specifiy a name, the bot will pick a name from the file
'bot_names.txt' in the folder cube/bot. This is a plain text file, you can modify existing names
and add up to 150 names. Note that names in cube can't contain more than 16 characters.
When you add a bot and don't specify his team, it will pick one from the file 'bot_teams.txt'
found in the cube/bot folder. This file can contain 20 different team names with a maximal
length of 5 characters.



Bot commands:

There are 2 ways to execute a (bot)command:
- Hit the chat key(default t) and type /<commandhere>
- Hit the console key(default ~) and type the command.

List of all bot specific commands:

Server side commands(only work if your the host):

addbot <teamname> <skill> <botname>
Adds a bot in the current game. Command arguments:

<teamname>:	Specifies the team for the bot, when you leave it blank or
		use "random" as team the bot will choose a team from the file
		'bot/bot_teams.txt'.
<skill>:	The skill of the bot; can be best, good, medium, worse or bad.
		When blank or "random" is used as skill, the bot will use skill good.
<name>:		The name for this bot. When blank the bot will pick a name from the
		file 'bot/bot_names.txt'

Examples:
To add a bot with a random team, default skill(good) and a random name:
addbot
To add a bot in team red, default skill with the name Joe:
addbot red random Joe


botsshoot 1|0
Enables or disables bot shooting. 1 is enabled, 0 is disabled.


idlebots 1|0
When enabled, all bots are idle(they won't 'think'). 1 is enabled, 0 is disabled.

kickbot <botname>
Will kick the bot with the name <botname> from the server.

Example: To kick bot Joe from the server:
kickbot Joe


kickallbots
Will kick all bots from the server


botskill <botname> <skill>
Will change the skill of the bot with the name <botname> to <skill>.
<skill> can be best, good, medium, worse or bad.

Example:
To change the skill of the bot Joe to best:
botskill Joe best


botskillall <skill>
Will change the bot skill and the default skill to <skill>.
<skill> can be best, good, medium, worse or bad.


client side commands(Works if your a host or joined a server):

drawbeamtobots
Will draw some particles(same as the rifle) to all bots. This more used for
debugging to see where the bots are :-)


togglegrap
Will toggle the focus of the mouse in game. Normally you can use your mouse to look around,
when you type this command your mouse cursor is visible and can be used as normally. This is only
usefull when you run cube windowed ofcourse.


togglebotview <botname>
When used you will see what the bot with the name <botname> sees. Type it again
(with or without name) to return to the game(you will respawn).



Waypoints:

Waypoints are used by bots to help them navigate through a map. A waypoint
is simply a spot in a map, by connecting them with other waypoints bots know how to
move from waypoint to waypoint. Note that waypoints can be connected 1 way or 2 way,
if a waypoint is connected 1 way this means that waypoint A can be connected with B, but
B is NOT connected to A. 
Bots can use waypoints since version 0.2. Currently its not 100% finished,
therefore I don't have made any good documentation for it didn't made any waypoints
yet. However, if you want to make some waypoints(so you can send them to me and I can include
them in the next release :-) ) you can use the following commands:

addwp - Adds a waypoint, all nearby waypoints will be connected with it.
delwp - Deletes the nearest waypoint.
wpvisble 1|0 - When enabled(1) you can view all the waypoint in the current map, 0 will
               disable this. (Waypoints will be automaticly visible when you use any
	       waypoint command). Jump waypoints are red lines, the nearest normal waypoint
	       is a green line others are blue lines. Connections between waypoints are
	       white lines.
wpsave - Saves the waypoints to a file.
wpload - Loads waypoints from a file, only handy when you want to undo your changes.(Waypoints will
         be loaded automaticly when a map is started).
autowp 1|0 - Enables(1) or disables(0) autowaypointing. When enabled you will drop waypoints while
             you walk around in the map.
addtelewps - Adds waypoints on all teleporters and their destination in the current map and connect
             the waypoint at a teleporter with the waypoint at its destination.
addpath1way1 - Selects first waypoint to connect with another waypoint.
addpath1way2 - Will connect the waypoint you selected with the command 'addpath1way1' with the
               nearest waypoint.
addpath2way1, addpath2way2 - Same as above, though it will connect the waypoint both waypoints
                             with each other(that is there is a connection between the one
			     you select with 'addpath1way1' and the nearest waypoint AND there
			     is a connection between the nearest waypoint and the one you
			     selected with 'addpath1way1').
delpath1way1, delpath1way2, - Same as above, but will remove connections between waypoints.
delpath2way1, delpath2way2
setjumpwp - Makes the nearest waypoint a "jump waypoint" this simply means that a bot will
            jump when it reaches this waypoint(usefull when the bot has to jump over an obstacle
	    for example).
unsetjumpwp - Removes the jump flag from the nearest waypoint.

All this stuff may be very cryptic to you, I will make better docs for the next version.



Dedicated servers:

Unfortunaly the bots won't work with dedicated servers (yet). This is because
cube is mostly client side. For dedicated servers the bots have to be server side.
So this would simply mean that most of the current code has to be moved to the server
in order to let the bots 'do their thing'.



Source:

You can find the modified source in the "bot_src" directory. 90% off all the changes are
marked by me with comments(mostly something like 'added by Rick').



Credits:

Wouter van Oortmerssen aka Aardappel and all the other people
that made cube as what it is now(see the cube readme).

Pierre-Marie Baty (PMB): for most of the current navigation code and other misc. code.
Check out his RACC bot for half-life at racc.bots-united.com

Botman : 'The father of all half-life bots'. He created the HPB bot(hpb-bot.bots-united.com) which
is the base for almost all half-life bots. A lot of ideas are inspired by him.

arghvark : Fix for ATI based gfx cards on linux.

People at bots-united forums and cube forums: For feedback, suggestions, reporting bugs
and bot names :)
