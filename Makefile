# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eli <eli@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/04/10 20:51:25 by eli              ###   ########.fr        #
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
INCLUDE	=	./inc
EXTRA	=	-Wall -Werror -Wextra
CFLAGS	=	-I$(INCLUDE) -std=c++17 -O2
LDFLAGS	=	-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SH_NAME	=	vert \
			frag
SHADERS	=	$(addprefix shaders/,$(SH_NAME))
SH_BIN	=	$(addsuffix .spv,$(SHADERS))

GLSLC	=	glslc

# ============================================================================ #
#                                     RULES                                    #
# ============================================================================ #

.PHONY: all
all: obj $(SH_BIN) $(NAME)

obj:
	@mkdir -p obj

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

obj/%.o: src/%.cpp $(INC)
	$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

shaders/%.spv: shaders/shader.%
	$(GLSLC) $< -o $@

.PHONY: test
test: all
	valgrind -s ./$(NAME)

.PHONY: clean
clean:
	rm -rf obj
	rm -rf $(SH_BIN)

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all
