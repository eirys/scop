# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eli <eli@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/04/27 13:56:02 by eli              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

NAME		:=	scop

# directory names
SRC_DIR		:=	src
OBJ_DIR		:=	obj
SHD_DIR		:=	shaders

# cpp files
INC_FILES	:=	utils.hpp \
				vertex.hpp \
				app.hpp

SRC_FILES	:=	main.cpp

INC			:=	$(addprefix	$(SRC_DIR)/,$(INC_F))
SRC			:=	$(addprefix $(SRC_DIR)/,$(SRC_F))
OBJ			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.cpp=.o))

# shaders
SH_FILES	:=	vert \
				frag
SHADERS		:=	$(addprefix $(SHD_DIR)/,$(SH_FILES))
SH_BIN		:=	$(addsuffix .spv,$(SHADERS))

# compiler
CXX			:=	c++
EXTRA		:=	-Wall -Werror -Wextra
CFLAGS		:=	-std=c++17 -O2 -DNDEBUG -D__VERBOSE
LDFLAGS		:=	-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
ifdef school
	GLSLC	=	~/my_sgoinfre/glslc
else
	GLSLC	=	glslc
endif

# misc
RM			:=	rm -rf

# ============================================================================ #
#                                     RULES                                    #
# ============================================================================ #

.PHONY: all
all: $(SH_BIN) $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(SHD_DIR)/%.spv: $(SHD_DIR)/shader.%
	$(GLSLC) $< -o $@

.PHONY: test
test: all
	./$(NAME)

.PHONY: clean
clean:
	${RM} $(OBJ_DIR)
	${RM} $(SH_BIN)

.PHONY: fclean
fclean: clean
	${RM} $(NAME)

.PHONY: re
re: fclean all
