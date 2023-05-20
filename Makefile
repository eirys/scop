# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: etran <etran@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/06 03:40:09 by eli               #+#    #+#              #
#    Updated: 2023/05/19 23:23:41 by etran            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                    TARGETS                                   #
# ============================================================================ #

school		:=	$(shell env | grep 42paris | wc -l)

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
UTILS_DIR	:=	$(APP_DIR)/utils
IMG_DIR		:=	$(UTILS_DIR)/img
MODEL_DIR	:=	$(UTILS_DIR)/model

SUBDIRS		:=	$(APP_DIR) \
				$(TOOLS_DIR) \
				$(SUBMOD_DIR) \
				$(MODEL_DIR) \
				$(UTILS_DIR) \
				$(IMG_DIR)

OBJ_SUBDIRS	:=	$(addprefix $(OBJ_DIR)/,$(SUBDIRS))
INC_SUBDIRS	:=	$(addprefix $(SRC_DIR)/,$(SUBDIRS))

# cpp files
INC_FILES	:=	$(TOOLS_DIR)/utils.hpp \
				$(TOOLS_DIR)/math.hpp \
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
				$(SUBMOD_DIR)/debug_module.hpp \
				$(SUBMOD_DIR)/device.hpp \
				$(SUBMOD_DIR)/render_target.hpp \
				$(SUBMOD_DIR)/render_target_resources.hpp \
				$(SUBMOD_DIR)/descriptor_set.hpp \
				$(SUBMOD_DIR)/command_buffer.hpp \
				$(SUBMOD_DIR)/texture_sampler.hpp \
				$(SUBMOD_DIR)/vertex_input.hpp \
				$(SUBMOD_DIR)/graphics_pipeline.hpp \
				$(APP_DIR)/app.hpp

SRC_FILES	:=	$(TOOLS_DIR)/matrix.cpp \
				$(MODEL_DIR)/model.cpp \
				$(MODEL_DIR)/parser.cpp \
				$(IMG_DIR)/ppm_loader.cpp \
				$(IMG_DIR)/image_handler.cpp \
				$(SUBMOD_DIR)/window.cpp \
				$(SUBMOD_DIR)/debug_module.cpp \
				$(SUBMOD_DIR)/device.cpp \
				$(SUBMOD_DIR)/render_target.cpp \
				$(SUBMOD_DIR)/render_target_resources.cpp \
				$(SUBMOD_DIR)/descriptor_set.cpp \
				$(SUBMOD_DIR)/command_buffer.cpp \
				$(SUBMOD_DIR)/texture_sampler.cpp \
				$(SUBMOD_DIR)/vertex_input.cpp \
				$(SUBMOD_DIR)/graphics_pipeline.cpp \
				$(APP_DIR)/app.cpp \
				main.cpp

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

ifdef school!=0
	EXTRA	+=	-Wno-unused-private-field
endif

CFLAGS		:=	$(EXTRA)\
				-std=c++17 \
				$(INCLUDES) \
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

ifdef school!=0
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
	@$(CXX) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "\`$(NAME)\` was successfully created."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC)
	@mkdir -p $(OBJ_DIR) $(OBJ_SUBDIRS)
	@echo "Compiling file $<..."
	@$(CXX) $(CFLAGS) -c $< -o $@

$(SHD_DIR)/%.spv: $(SHD_DIR)/shader.%
	@echo "Compiling shader $<..."
	@$(GLSLC) $< -o $@

.PHONY: clean
clean:
	@${RM} $(OBJ_DIR)
	@echo "Cleaning object files."

.PHONY: fclean
fclean: clean
	@${RM} $(SHD_BIN)
	@echo "Removed shader binaries."
	@${RM} $(NAME)
	@echo "Removed $(NAME)."

.PHONY: re
re: fclean all