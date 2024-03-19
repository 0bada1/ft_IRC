# Compiler
CXX := c++

# Compiler flags
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

# Source files
SRCS := main.cpp \
		Parse.cpp	\
		Server.cpp \
		Channel.cpp \
		Commands.cpp \
		User.cpp \
		Utils.cpp \


# Object files
OBJS := $(SRCS:.c=.o)

# Executable
NAME := ircserv

all: $(NAME)

irssi: rmirssi
	docker run -it --name irssi-container -e TERM -u $(id -u):$(id -g) \
	--log-driver=none \
    -v ${HOME}/.irssi:/home/user/.irssi:ro \
    irssi

rmirssi:
	docker rm -f irssi-container 2>/dev/null

# Build NAME
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files
clean:
	rm -f *.o

# Clean all executable and object
fclean: clean
	rm -f $(NAME)

# Rebuild NAME
re: fclean all

.PHONY: all clean fclean re
