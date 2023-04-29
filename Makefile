# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eli <eli@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/04/29 21:20:58 by eli              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

# final binary
NAME		:=	scop

# directory names
SRC_DIR		:=	src
OBJ_DIR		:=	obj
SHD_DIR		:=	shaders

# cpp files
INC_FILES	:=	utils.hpp \
				vertex.hpp \
				ubo.hpp \
				app.hpp

SRC_FILES	:=	main.cpp \
				app.cpp

INC			:=	$(addprefix	$(SRC_DIR)/,$(INC_FILES))
SRC			:=	$(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJ			:=	$(addprefix $(OBJ_DIR)/,$(SRC_FILES:.cpp=.o))

# shaders
SHD_FILES	:=	vert \
				frag
SHD			:=	$(addprefix $(SHD_DIR)/,$(SHD_FILES))
SHD_BIN		:=	$(addsuffix .spv,$(SHD))

# compiler
CXX			:=	c++
EXTRA		:=	-Wall -Werror -Wextra
CFLAGS		:=	-std=c++17 -O2 -DNDEBUG -D__VERBOSE
LDFLAGS		:=	-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
ifdef school
	GLSLC	:=	~/my_sgoinfre/glslc
else
	GLSLC	:=	glslc
endif

# misc
RM			:=	rm -rf

# ============================================================================ #
#                                     RULES                                    #
# ============================================================================ #

.PHONY: all
all: $(NAME)

$(NAME): $(SHD_BIN) $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(SHD_DIR)/%.spv: $(SHD_DIR)/shader.%
	$(GLSLC) $< -o $@

.PHONY: test
test: all
	./$(NAME)

.PHONY: clean
clean:
	${RM} $(OBJ_DIR)
	${RM} $(SHD_BIN)

.PHONY: fclean
fclean: clean
	${RM} $(NAME)

.PHONY: re
re: fclean all
