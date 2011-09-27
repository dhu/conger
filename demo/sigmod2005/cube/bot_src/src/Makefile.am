MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS) -g -DHAS_SOCKLEN_T -DENABLE_SERIALIZATION=1 -Wno-deprecated
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS) -g -DHAS_SOCKLEN_T -DENABLE_SERIALIZATION=1 -Wno-deprecated

bin_PROGRAMS = cube_client

LDADD_EXTRAS =  -lz -lGL -lGLU -ldl -lcrypto

LDADD = \
	@NMSTLLIB@\
	@SDL_LIBS@\
	@SDL_MIXER_LIB@\
	@SDL_IMAGE_LIB@\
	@XERCESCLIB@\
	-L@BOREALISROOT@/src/modules/common\
	-L@BOREALISROOT@/src/modules/util\
	-L@BOREALISROOT@/src/modules/rpc\
	-L@BOREALISROOT@/src/modules/queryProcessor/statsMgr \
	-lborealiscommon\
	-lborealisqpstatsMgr \
	-lborealisutil\
	-L../enet -lenet\
	$(LDADD_EXTRAS)


INCLUDES = \
	-I${top_srcdir}/enet/include \
	@SDL_CFLAGS@ \
	@SDL_MIXER_INCLUDE@ \
	@SDL_IMAGE_INCLUDE@ \
	@NMSTLINCLUDE@ \
	@XERCESCINCLUDE@ \
	-I@BOREALISROOT@/src/modules/catalog \
	-I@BOREALISROOT@/src/modules/common \
	-I@BOREALISROOT@/src/modules/queryProcessor/statsMgr \
	-I@BOREALISROOT@/src/modules/util \
	-I@BOREALISROOT@/src/modules/rpc


cube_client_SOURCES = \
	bot.cpp \
	bot_ai.cpp \
	bot_util.cpp \
	bot_waypoint.cpp \
	client.cpp \
	clientextras.cpp \
	clientgame.cpp \
	clients2c.cpp \
	command.cpp \
	console.cpp \
	editing.cpp \
	entities.cpp \
	gamecommander.cpp \
	gamesubscriber.cpp \
	main.cpp \
	menus.cpp \
	monster.cpp \
	netio.cpp \
	physics.cpp \
	rendercubes.cpp \
	renderextras.cpp \
	rendergl.cpp \
	rendermd2.cpp \
	renderparticles.cpp \
	rendertext.cpp \
	rndmap.cpp \
	savegamedemo.cpp \
	server.cpp \
	serverbrowser.cpp \
	serverms.cpp \
	serverutil.cpp \
	sound.cpp \
	tools.cpp \
	weapon.cpp \
	world.cpp \
	worldio.cpp \
	worldlight.cpp \
	worldocull.cpp \
	worldrender.cpp
