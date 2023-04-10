# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eli <eli@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/04/10 19:42:22 by eli              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

NAME	=	scop

INC_F	=	utils.hpp \
			app.hpp

SRC_F	=	main.cpp

INC		=	$(addprefix	inc/,$(INC_F))
SRC		=	$(addprefix src/,$(SRC_F))
OBJS	=	$(subst src/,obj/,$(SRC:.cpp=.o))

CXX		=	c++
EXTRA	=	-Wall -Werror -Wextra
INCLUDE	=	./inc
CFLAGS	=	-I$(INCLUDE) -std=c++17 -O2
LDFLAGS	=	-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

GLSLC	=	glslc

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

.PHONY: shader
shader:
	$(GLSLC) shaders/shader.vert -o shaders/vert.spv
	$(GLSLC) shaders/shader.frag -o shaders/frag.spv

.PHONY: test
test: all
	valgrind -s ./$(NAME)

.PHONY: clean
clean:
	rm -rf obj
	rm shaders/vert.spv shaders/frag.spv

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all
