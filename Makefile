# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eli <eli@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/04/06 14:24:39 by eli              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

NAME	=	scop

INC_F	=	utils.hpp
INC		=	$(addprefix	inc/,$(INC_F))

SRC_F	=	main.cpp
SRC		=	$(addprefix src/,$(SRC_F))
OBJS	=	$(subst src/,obj/,$(SRC:.cpp=.o))

CXX		=	c++
EXTRA	=	-Wall -Werror -Wextra
INCLUDE	=	./inc
CFLAGS	=	-I$(INCLUDE) -std=c++17 -O2

LDFLAGS	=	-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# ============================================================================ #
#                                     RULES                                    #
# ============================================================================ #

.PHONY: all
all: obj $(NAME)

obj:
	mkdir -p obj

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

obj/%.o: src/%.cpp $(INC)
	$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: test
test: all
	./$(NAME)

.PHONY: clean
clean:
	rm -rf obj

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all