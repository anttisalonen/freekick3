CXX      ?= g++
AR       ?= ar
CXXFLAGS ?= -std=c++0x -O2 -g3 -Werror
CXXFLAGS += -Wall

CXXFLAGS += $(shell sdl-config --cflags)
LDFLAGS  += $(shell sdl-config --libs) -lSDL_image -lSDL_ttf

LDFLAGS  += -lGL


CXXFLAGS += -Isrc
BINDIR       = bin

COMMONSRCFILES = SDL_utils.cpp

COMMONSRCDIR = src/common
COMMONSRCS = $(addprefix $(COMMONSRCDIR)/, $(COMMONSRCFILES))
COMMONOBJS = $(COMMONSRCS:.cpp=.o)
COMMONDEPS = $(COMMONSRCS:.cpp=.dep)

COMMONLIB = $(COMMONSRCDIR)/libcommon.a

MATCHBINNAME = freekick3-match
MATCHBIN     = $(BINDIR)/$(MATCHBINNAME)

MATCHSRCDIR = src/match

MATCHSRCFILES = Math.cpp Clock.cpp SDLSurface.cpp Texture.cpp Pitch.cpp Ball.cpp \
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

SOCCERBINNAME = freekick3
SOCCERBIN     = $(BINDIR)/$(SOCCERBINNAME)

SOCCERSRCDIR = src/soccer

SOCCERSRCFILES = Menu.cpp main.cpp

SOCCERSRCS = $(addprefix $(SOCCERSRCDIR)/, $(SOCCERSRCFILES))
SOCCEROBJS = $(SOCCERSRCS:.cpp=.o)
SOCCERDEPS = $(SOCCERSRCS:.cpp=.dep)

.PHONY: clean all

all: $(MATCHBIN) $(SOCCERBIN)

$(BINDIR):
	mkdir -p $(BINDIR)

$(COMMONLIB): $(COMMONOBJS)
	$(AR) rcs $(COMMONLIB) $(COMMONOBJS)

$(MATCHBIN): $(BINDIR) $(COMMONLIB) $(MATCHOBJS)
	$(CXX) $(LDFLAGS) $(MATCHOBJS) $(COMMONLIB) -o $(MATCHBIN)

$(SOCCERBIN): $(BINDIR) $(COMMONLIB) $(SOCCEROBJS)
	$(CXX) $(LDFLAGS) $(SOCCEROBJS) $(COMMONLIB) -o $(SOCCERBIN)

%.dep: %.cpp
	@rm -f $@
	@$(CC) -MM $(CXXFLAGS) $< > $@.P
	@sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $*)\1.o $@ : ,g' < $@.P > $@
	@rm -f $@.P

clean:
	find src/ -name '*.o' -exec rm -rf {} +
	find src/ -name '*.dep' -exec rm -rf {} +
	find src/ -name '*.a' -exec rm -rf {} +
	rm -rf $(BINDIR)

-include $(MATCHDEPS) $(SOCCERDEPS)

