# Compiler
CXX := c++

# Compiler flags
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 #-g3 -fsanitize=address

# Source files
SRCS := main.cpp \
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
