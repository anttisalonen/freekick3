CXX      ?= g++
AR       ?= ar
CXXFLAGS ?= -std=c++0x -O2 -g3
CXXFLAGS += -Wall

CXXFLAGS += $(shell sdl-config --cflags)
LDFLAGS  += $(shell sdl-config --libs) -lSDL_image -lSDL_ttf

LDFLAGS  += -lGL


BINDIR  = bin
BINNAME = freekick3
BIN     = $(BINDIR)/$(BINNAME)

SRCDIR = src

SRCFILES = Pitch.cpp Match.cpp MatchEntity.cpp Team.cpp Player.cpp MatchSDLGUI.cpp \
	   Clock.cpp Referee.cpp RefereeActions.cpp PlayerAIController.cpp \
	   PlayerActions.cpp Texture.cpp main.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.dep)

.PHONY: clean all

all: $(BIN)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BIN): $(BINDIR) $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(BIN)

%.dep: %.cpp
	@rm -f $@
	@$(CC) -MM $(CPPFLAGS) $< > $@.P
	@sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $*)\1.o $@ : ,g' < $@.P > $@
	@rm -f $@.P

clean:
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*.dep $(BIN)
	rm -rf $(BINDIR)

-include $(DEPS)

