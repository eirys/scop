# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: etran <etran@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/05/12 23:31:37 by etran            ###   ########.fr        #
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
TOOLS_DIR	:=	tools
APP_DIR		:=	app
SUBMOD_DIR	:=	$(APP_DIR)/submodules
MODEL_DIR	:=	$(APP_DIR)/model
UTILS_DIR	:=	$(APP_DIR)/utils
IMG_DIR		:=	$(UTILS_DIR)/img

SUBDIRS		:=	$(APP_DIR) \
				$(TOOLS_DIR) \
				$(SUBMOD_DIR) \
				$(MODEL_DIR) \
				$(UTILS_DIR) \
				$(IMG_DIR)

OBJ_SUBDIRS	:=	$(addprefix $(OBJ_DIR)/,$(SUBDIRS))
INC_SUBDIRS	:=	$(addprefix $(SRC_DIR)/,$(SUBDIRS))

# external libraries
STB_PATH	:=	$(LIB_DIR)/stb

# cpp files
INC_FILES	:=	$(TOOLS_DIR)/utils.hpp \
				$(TOOLS_DIR)/matrix.hpp \
				$(TOOLS_DIR)/vector.hpp \
				$(UTILS_DIR)/vertex.hpp \
				$(UTILS_DIR)/uniform_buffer_object.hpp \
				$(MODEL_DIR)/model.hpp \
				$(MODEL_DIR)/parser.hpp \
				$(IMG_DIR)/image_loader.hpp \
				$(IMG_DIR)/image_handler.hpp \
				$(IMG_DIR)/ppm_loader.hpp \
				$(SUBMOD_DIR)/window.hpp \
				$(APP_DIR)/app.hpp

SRC_FILES	:=	main.cpp \
				$(MODEL_DIR)/model.cpp \
				$(MODEL_DIR)/parser.cpp \
				$(IMG_DIR)/ppm_loader.cpp \
				$(IMG_DIR)/image_handler.cpp \
				$(SUBMOD_DIR)/window.cpp \
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
INCLUDES	:=	$(addprefix -I./,\
				$(INC_SUBDIRS) \
				$(STB_PATH))

CFLAGS		:=	$(EXTRA) \
				-std=c++17 \
				$(INCLUDES) \
				-O3 \
				-g \
				-DNDEBUG \
				-D__DEBUG

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

tmp:
	$(CXX) $(CFLAGS) $(wildcard src/$(IMG_DIR)/*.cpp)

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

.PHONY: fclean
fclean: clean
	${RM} $(SHD_BIN)
	${RM} $(NAME)

.PHONY: re
re: fclean all