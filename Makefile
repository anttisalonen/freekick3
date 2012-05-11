CXX      ?= g++
AR       ?= ar
CXXFLAGS ?= -std=c++0x -O2 -g3 -Werror
CXXFLAGS += -Wall

CXXFLAGS += $(shell sdl-config --cflags)
LDFLAGS  += $(shell sdl-config --libs) -lSDL_image -lSDL_ttf

LDFLAGS  += -lGL


CXXFLAGS += -Isrc
BINDIR       = bin

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

.PHONY: clean all

all: $(MATCHBIN)

$(BINDIR):
	mkdir -p $(BINDIR)

$(MATCHBIN): $(BINDIR) $(MATCHOBJS)
	$(CXX) $(LDFLAGS) $(MATCHOBJS) -o $(MATCHBIN)

%.dep: %.cpp
	@rm -f $@
	@$(CC) -MM $(CXXFLAGS) $< > $@.P
	@sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $*)\1.o $@ : ,g' < $@.P > $@
	@rm -f $@.P

clean:
	rm -f $(MATCHSRCDIR)/*.o $(MATCHSRCDIR)/*.dep $(MATCHBIN)
	rm -rf $(BINDIR)

-include $(MATCHDEPS)

