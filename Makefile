# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eli <eli@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/05/06 12:49:51 by eli              ###   ########.fr        #
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
LIB_DIR		:=	lib

# subdirectories
APP_DIR		:=	app
TOOLS_DIR	:=	tools
WINDOW_DIR	:=	$(APP_DIR)/window
UTILS_DIR	:=	$(APP_DIR)/utils

OBJ_SUBDIRS	:=	$(addprefix $(OBJ_DIR)/,\
				$(APP_DIR) \
				$(TOOLS_DIR) \
				$(WINDOW_DIR) \
				$(UTILS_DIR))

# external libraries
STB_PATH	:=	$(LIB_DIR)/stb
TOL_PATH	:=	$(LIB_DIR)/tol

# cpp files
INC_FILES	:=	$(TOOLS_DIR)/utils.hpp \
				$(TOOLS_DIR)/matrix.hpp \
				$(TOOLS_DIR)/vector.hpp \
				$(UTILS_DIR)/vertex.hpp \
				$(UTILS_DIR)/uniform_buffer_object.hpp \
				$(WINDOW_DIR)/window.hpp \
				$(APP_DIR)/app.hpp

SRC_FILES	:=	main.cpp \
				$(WINDOW_DIR)/window.cpp \
				$(APP_DIR)/app.cpp

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
CFLAGS		:=	-std=c++17 \
				-I./$(SRC_DIR)/$(APP_DIR) \
				-I./$(SRC_DIR)/$(TOOLS_DIR) \
				-I./$(SRC_DIR)/$(UTILS_DIR) \
				-I./$(SRC_DIR)/$(WINDOW_DIR) \
				-I./$(STB_PATH) \
				-I./${TOL_PATH} \
				-O3 \
				-DNDEBUG \
				-D__VERBOSE

LDFLAGS		:=	-lglfw \
				-lvulkan \
				-ldl \
				-lpthread \
				-lX11 \
				-lXxf86vm \
				-lXrandr \
				-lXi

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
	@mkdir -p $(OBJ_DIR) $(OBJ_SUBDIRS)
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