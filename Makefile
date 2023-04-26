# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: etran <etran@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/04/26 11:45:52 by etran            ###   ########.fr        #
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

ifdef school
	GLSLC	=	~/my_sgoinfre/glslc
else
	GLSLC	=	glslc
endif

RM		=	rm -rf

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
	./$(NAME)

.PHONY: clean
clean:
	${RM} obj
	${RM} $(SH_BIN)

.PHONY: fclean
fclean: clean
	${RM} $(NAME)

.PHONY: re
re: fclean all
