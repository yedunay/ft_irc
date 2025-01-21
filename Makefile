NAME = ircserv

CXX = c++
OBJ_DIR = .objs

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -IIncludes
SRCS =  \
		Channel.cpp \
		Client.cpp \
		Server.cpp \
		Cmds.cpp \
		Cmds1.cpp \
		Cmds2.cpp \
		main.cpp \
		Tools.cpp



OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

all: $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "Server Created"

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
