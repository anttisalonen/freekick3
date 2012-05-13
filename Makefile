CXX      ?= g++
AR       ?= ar
CXXFLAGS ?= -std=c++0x -O2 -g3 -Werror
CXXFLAGS += -Wall

CXXFLAGS += $(shell sdl-config --cflags)
LDFLAGS  += $(shell sdl-config --libs) -lSDL_image -lSDL_ttf

LDFLAGS  += -lGL

LDFLAGS += -ltinyxml


CXXFLAGS += -Isrc
BINDIR       = bin

COMMONSRCFILES = SDLSurface.cpp Texture.cpp SDL_utils.cpp
COMMONSRCDIR = src/common
COMMONSRCS = $(addprefix $(COMMONSRCDIR)/, $(COMMONSRCFILES))
COMMONOBJS = $(COMMONSRCS:.cpp=.o)
COMMONDEPS = $(COMMONSRCS:.cpp=.dep)
COMMONLIB = $(COMMONSRCDIR)/libcommon.a

LIBSOCCERSRCFILES = Player.cpp Team.cpp Match.cpp DataExchange.cpp
LIBSOCCERSRCDIR = src/soccer
LIBSOCCERSRCS = $(addprefix $(LIBSOCCERSRCDIR)/, $(LIBSOCCERSRCFILES))
LIBSOCCEROBJS = $(LIBSOCCERSRCS:.cpp=.o)
LIBSOCCERDEPS = $(LIBSOCCERSRCS:.cpp=.dep)
LIBSOCCERLIB = $(LIBSOCCERSRCDIR)/libsoccer.a

SOCCERBINNAME = freekick3
SOCCERBIN     = $(BINDIR)/$(SOCCERBINNAME)
SOCCERSRCDIR  = src/soccer
SOCCERSRCFILES = gui/Button.cpp gui/Screen.cpp gui/ScreenManager.cpp \
		 gui/MainMenuScreen.cpp gui/FriendlyScreen.cpp \
		 gui/Menu.cpp main.cpp
SOCCERSRCS = $(addprefix $(SOCCERSRCDIR)/, $(SOCCERSRCFILES))
SOCCEROBJS = $(SOCCERSRCS:.cpp=.o)
SOCCERDEPS = $(SOCCERSRCS:.cpp=.dep)

MATCHBINNAME = freekick3-match
MATCHBIN     = $(BINDIR)/$(MATCHBINNAME)
MATCHSRCDIR = src/match
MATCHSRCFILES = Math.cpp Clock.cpp Pitch.cpp Ball.cpp \
	   Match.cpp MatchHelpers.cpp MatchEntity.cpp Team.cpp Player.cpp PlayerActions.cpp \
	   Referee.cpp RefereeActions.cpp \
	   ai/PlayerAIController.cpp ai/AIActions.cpp ai/AIPlayStates.cpp ai/AIHelpers.cpp \
	   ai/AIGoalkeeperState.cpp ai/AIDefendState.cpp \
	   ai/AIKickBallState.cpp ai/AIOffensiveState.cpp \
	   MatchSDLGUI.cpp \
	   main.cpp

MATCHSRCS = $(addprefix $(MATCHSRCDIR)/, $(MATCHSRCFILES))
MATCHOBJS = $(MATCHSRCS:.cpp=.o)
MATCHDEPS = $(MATCHSRCS:.cpp=.dep)

.PHONY: clean all

all: $(MATCHBIN) $(SOCCERBIN)

$(BINDIR):
	mkdir -p $(BINDIR)

$(COMMONLIB): $(COMMONOBJS)
	$(AR) rcs $(COMMONLIB) $(COMMONOBJS)

$(LIBSOCCERLIB): $(LIBSOCCEROBJS)
	$(AR) rcs $(LIBSOCCERLIB) $(LIBSOCCEROBJS)

$(SOCCERBIN): $(BINDIR) $(COMMONLIB) $(LIBSOCCERLIB) $(SOCCEROBJS)
	$(CXX) $(LDFLAGS) $(SOCCEROBJS) $(LIBSOCCERLIB) $(COMMONLIB) -o $(SOCCERBIN)

$(MATCHBIN): $(BINDIR) $(COMMONLIB) $(LIBSOCCERLIB) $(MATCHOBJS)
	$(CXX) $(LDFLAGS) $(MATCHOBJS) $(LIBSOCCERLIB) $(COMMONLIB) -o $(MATCHBIN)

%.dep: %.cpp
	@rm -f $@
	@$(CC) -MM $(CXXFLAGS) $< > $@.P
	@sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $*)\1.o $@ : ,g' < $@.P > $@
	@rm -f $@.P

clean:
	find src/ -name '*.o' -exec rm -rf {} +
	find src/ -name '*.dep' -exec rm -rf {} +
	find src/ -name '*.a' -exec rm -rf {} +
	rm -rf $(MATCHBIN) $(SOCCERBIN)
	rmdir $(BINDIR)

-include $(MATCHDEPS) $(SOCCERDEPS)

